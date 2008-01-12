/*!\file
 * \author Matthias Elf
 *
 * \par License:
 * This file is part of ABACUS - A Branch And CUt System
 * Copyright (C) 1995 - 2003                                                  
 * University of Cologne, Germany                                             
 * 
 * \par                                                                           
 * This library is free software; you can redistribute it and/or              
 * modify it under the terms of the GNU Lesser General Public                 
 * License as published by the Free Software Foundation; either               
 * version 2.1 of the License, or (at your option) any later version.         
 *     
 * \par                                                                       
 * This library is distributed in the hope that it will be useful,            
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU          
 * Lesser General Public License for more details.                            
 *     
 * \par                                                                       
 * You should have received a copy of the GNU Lesser General Public           
 * License along with this library; if not, write to the Free Software        
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  
 *
 * \see http://www.gnu.org/copyleft/gpl.html
 *
 * $Id: master.cc,v 2.11 2007/07/26 09:31:59 baumann Exp $
 */

#include "abacus/master.h"
#include "abacus/sub.h"
#include "abacus/lpsub.h"
#include "abacus/lpvarstat.h"
#include "abacus/variable.h"
#include "abacus/history.h"
#include "abacus/opensub.h"
#include "abacus/fixcand.h"
#include "abacus/setbranchrule.h"
#include "abacus/standardpool.h"

#ifdef ABACUS_PARALLEL
#include "abacus/parmaster.h"
#include "abacus/valbranchrule.h"
#include "abacus/boundbranchrule.h"
#include "abacus/conbranchrule.h"
#include "abacus/numcon.h"
#include "abacus/rowcon.h"
#include "abacus/srowcon.h"
#include "abacus/numvar.h"
#include "abacus/colvar.h"
#include "abacus/subserver.h"
#include "abacus/notifyserver.h"
#include "abacus/notification.h"
#include "abacus/broadcast.h"
#endif

#include <string.h>
#include <stdio.h>

#include <math.h>

extern "C" {
#include <unistd.h>
}

const char* ABA_MASTER::STATUS_[]={"Optimal", "Error", "OutOfMemory", "Unprocessed", "Processing",
   "Guaranteed", "MaxLevel", "MaxCpuTime",
   "MaxCowTime", "ExceptionFathom"};
const char* ABA_MASTER::OUTLEVEL_[] = {"Silent","Statistics","Subproblem",
   "LinearProgram","Full"};
const char * ABA_MASTER::ENUMSTRAT_[]={"BestFirst","BreadthFirst",
   "DepthFirst","DiveAndBest"};
const char * ABA_MASTER::BRANCHINGSTRAT_[]={"CloseHalf","CloseHalfExpensive"};
const char* ABA_MASTER::PRIMALBOUNDMODE_[]={"None","Optimum","OptimumOne"};
const char* ABA_MASTER::SKIPPINGMODE_[]={"SkipByNode","SkipByLevel"};
const char* ABA_MASTER::CONELIMMODE_[]={"None","NonBinding","Basic"};
const char* ABA_MASTER::VARELIMMODE_[]={"None","ReducedCost"};
const char* ABA_MASTER::VBCMODE_[]={"None","File","Pipe"};

// Possible values for the defaultLpSolver_ parameter
const char* ABA_MASTER::OSISOLVER_[]={"Cbc","Clp","CPLEX","DyLP","FortMP",\
   "GLPK","MOSEK","OSL","SoPlex",\
      "SYMPHONY","XPRESS_MP"};


ABA_MASTER::ABA_MASTER(const char *problemName, bool cutting, bool pricing,
      ABA_OPTSENSE::SENSE optSense,
      double eps, double machineEps, double infinity,
      bool readParamFromFile) 
:  
ABA_GLOBAL(eps, machineEps, infinity),            
   problemName_(this, problemName),  
   readParamFromFile_(readParamFromFile),  
   optSense_(optSense),  
   root_(0),  
   rRoot_(0),  
   openSub_(0),  
   history_(0), 
   enumerationStrategy_(BestFirst),  
   branchingStrategy_(CloseHalfExpensive),  
   nBranchingVariableCandidates_(1),  
   defaultLpSolver_(Clp),  
   lpMasterOsi_(0),  
   conPool_(0),  
   cutPool_(0),  
   varPool_(0),  
   fixCand_(0),  
   cutting_(cutting),  
   pricing_(pricing),  
   solveApprox_(false),
   nSubSelected_(0),  
   VbcLog_(NoVbc),  
   treeStream_(0),  
   requiredGuarantee_(0.0),  
   maxLevel_(INT_MAX),  
   maxCpuTime_(this, "999999:59:59"),  
   maxCowTime_(this, "999999:59:59"),  
   objInteger_(false),  
   tailOffNLp_(0),  
   tailOffPercent_(0.000001),  
   dbThreshold_(0),  
   minDormantRounds_(1),  
   outLevel_(Full),  
   logLevel_(Silent),  
   pbMode_(NoPrimalBound),  
   pricingFreq_(0),  
   skipFactor_(1),  
   skippingMode_(SkipByNode),  
   fixSetByRedCost_(true),  
   printLP_(false),  
   maxConAdd_(100),  
   maxConBuffered_(100),  
   maxVarAdd_(100),  
   maxVarBuffered_(100),  
   maxIterations_(INT_MAX),  
   eliminateFixedSet_(false),  
   newRootReOptimize_(false),  
   optimumFileName_(this),  
   showAverageCutDistance_(false),  
   conElimMode_(NoConElim),  
   varElimMode_(NoVarElim),  
   conElimEps_(0.001),  
   varElimEps_(0.001),  
   conElimAge_(1),  
   varElimAge_(1),  
   status_(Unprocessed),  
   totalCowTime_(this),  
   totalTime_(this),  
   lpTime_(this),  
   lpSolverTime_(this),  
   separationTime_(this),  
   improveTime_(this),  
   pricingTime_(this),  
   branchingTime_(this),  
   nSub_(0),  
   nLp_(0),  
   highestLevel_(0),  
   nFixed_(0),  
   nAddCons_(0),  
   nRemCons_(0),  
   nAddVars_(0),  
   nRemVars_(0),  
   nNewRoot_(0)
{
   _createLpMasters();
   // ABA_MASTER::ABA_MASTER(): allocate some members 
   history_ = new ABA_HISTORY(this);
   openSub_ = new ABA_OPENSUB(this);
   fixCand_ = new ABA_FIXCAND(this);

#ifdef ABACUS_PARALLEL
   parmaster_ = new ABA_PARMASTER(this);
#endif

   _initializeParameters();

   if (outLevel_ == Silent)
      out().off();
   if (logLevel_ == Silent)
      out().logOff();

   // start the timers 
   totalCowTime_.start();
   totalTime_.start();

}

ABA_MASTER::~ABA_MASTER()
{
#ifdef ABACUS_PARALLEL
   delete parmaster_;
#endif
   if (treeStream_ != &cout) delete treeStream_;
   delete history_;
   delete conPool_;
   delete cutPool_;
   delete varPool_;
   delete openSub_;
   delete fixCand_;
   _deleteLpMasters();
}



#ifdef ABACUS_PARALLEL

ABA_CONVAR *ABA_MASTER::unpackConVar(ABA_MESSAGE &msg, int classId) const
{
   switch (classId) {
      case NUMCON_CLASSID:
	 return new ABA_NUMCON(this, msg);
      case ROWCON_CLASSID:
	 return new ABA_ROWCON(this, msg);
      case SROWCON_CLASSID:
	 return new ABA_SROWCON(this, msg);
      case NUMVAR_CLASSID:
	 return new ABA_NUMVAR(this, msg);
      case COLVAR_CLASSID:
	 return new ABA_COLVAR(this, msg);
   }
   err() << "ABA_MASTER::unpackConVar() : cannot unpack subclass of"
					  << " ABA_CONVAR with classId = " << classId << "." << endl;
   exit(Fatal);
   return 0;           //!< to suppress compiler warning
}

ABA_BRANCHRULE *ABA_MASTER::unpackBranchrule(ABA_MESSAGE &msg, int classId) const
{
   switch (classId) {
      case SETBRANCHRULE_CLASSID:
	 return new ABA_SETBRANCHRULE(this, msg);
      case VALBRANCHRULE_CLASSID:
	 return new ABA_VALBRANCHRULE(this, msg);
      case CONBRANCHRULE_CLASSID:
	 return new ABA_CONBRANCHRULE(this, msg);
      case BOUNDBRANCHRULE_CLASSID:
	 return new ABA_BOUNDBRANCHRULE(this, msg);
   }
   err() << "ABA_MASTER::unpackBranchrule() : cannot unpack subclass of"
					      << " ABA_BRANCHRULE with classId = " << classId << "." << endl;
   exit(Fatal);
   return 0;           //!< to suppress compiler warning
}

#endif

ABA_MASTER::STATUS ABA_MASTER::optimize()
{
   // startup the optimization
   /* We call \a _initializeParameters() again, because the call in the
    *   constructor of ABA_MASTER will not call a possibly redefined virtual
    *   function in a derived class.
    *
    *   The default implementation of the virtual function \a initializeOptimization()
    *   does nothing. A reimplementation can be used for problem specific
    *   initializations.
    */
   _initializeParameters();

   // output the ABACUS banner 
   /* We do not output the banner right at the beginning of the constructor,
    *   because when a problem specific parameter file is read
    *   in the function \a _initializeParameters() the output level might become
    *   \a Silent.
    */  
   if (outLevel_ == Silent) out().off();
   if (logLevel_ == Silent) out().logOff();

   out() << endl;
   out() << "ABACUS, Version " << ABACUS_VERSION_STRING << ", " << __DATE__
      << ", " << __TIME__ << endl;
   out() << "Copyright (c) Universitaet Koeln, Germany" << endl << endl;
#ifdef ABACUS_PARALLEL
   out() << "- PARALLEL VERSION -" << endl;
#endif

   out() << endl;
   out() << "BRANCH AND CUT OPTIMIZATION" << endl << endl;

   if (outLevel_ != Full) out().off();
   if (logLevel_ != Full) out().logOff();

#ifdef ABACUS_PARALLEL
   if (parmaster_->isHostZero()) {
      //! initialize the tree-log file
      if (VbcLog_ == File) {
	 char *treeStreamName = new char[strlen(problemName_.string())+40];

	 sprintf(treeStreamName, "%s.%ld.tree", problemName_.string(), getpid());
	 treeStream_ = new ofstream(treeStreamName);

	 delete [] treeStreamName;
      }
      else if (VbcLog_ == Pipe)
      {
	 ABA_STRING pipeName(this);
	 if (getParameter("VbcPipeName", pipeName)==0)
	    treeStream_ = new ofstream(pipeName.string());
	 else
	    treeStream_ = &cout;
      }

      // write the first lines of input for the tree interface
      if (VbcLog_ != NoVbc) {
	 writeTreeInterface("#TYPE: COMPLETE TREE", false);
	 writeTreeInterface("#TIME: SET", false);
	 writeTreeInterface("#BOUNDS: SET", false);
	 writeTreeInterface("#INFORMATION: STANDARD", false);
	 writeTreeInterface("#NODE_NUMBER: NONE", false);
      }

   }
#else
   // initialize the tree-log file
   /* If \a VbcLog_ is \a File, then we generate a log-file which can be read
    *   by the VBC-tool to visualize the implicit enumeration. The name
    *   of the log-file is composed of the problem name, the process ID, and
    *   the extension {\tt .tree}. The length of the array storing the name
    *   of the log-file is sufficient for process ids represented by integers with
    *   up to 64 bit (a 64 bit integer has most 19 decimal digits).
    *   If \a VbcLog_ is \a Pipe then we write the instructions to standard out.
    */  
   if (VbcLog_ == File) {
      char *treeStreamName = new char[strlen(problemName_.string())+40];

      sprintf(treeStreamName, "%s.%d.tree", problemName_.string(), getpid());
      treeStream_ = new ofstream(treeStreamName);

      delete [] treeStreamName;
   }
   else if (VbcLog_ == Pipe)
   {
      ABA_STRING pipeName(this);
      if (getParameter("VbcPipeName", pipeName)==0)
	 treeStream_ = new ofstream(pipeName.string());
      else
	 treeStream_ = &cout;
   }

#endif

   // initialize the primal and the dual bound
   /* The initialization of the primal and dual bound cannot be performed
    *   in any case in the constructor, as the sense of optimization
    *   might be unknown, if it is read from the constructor of a derived class.
    *   Therefore, we initialize these bounds now. If the initialization of the
    *   sense of the optimization has been forgotten, then the program terminates
    *   with an error.
    */

   // initialize the bounds according to sense of optimization 
   switch (optSense_.sense()) {
      case ABA_OPTSENSE::Min:
	 primalBound_ =  infinity();
	 dualBound_   = -infinity();
	 break;
      case ABA_OPTSENSE::Max:
	 primalBound_ = -infinity();
	 dualBound_   =  infinity();
	 break;
      case ABA_OPTSENSE::Unknown:
	 err() << "ABA_MASTER::optimize(): optimization sense unknown." << endl;
	 err() << "Specify optimization sense in the constructor or " << endl;
	 err() << "use function initializeOptSense()." << endl;
	 exit (Fatal);
   }

   // depending on \a PRIMALBOUNDMODE reinitialize \a primalBound 
   double opt;
   if (knownOptimum(opt)) {
      switch (pbMode_) {
	 case Optimum: primalBound_= opt;
		       break;
	 case OptimumOne:
		       if (optSense_.max()) primalBound_ = opt - 1.0;
		       else                 primalBound_ = opt + 1.0;
	 default: break;
      }
   }


   initializeOptimization();
   // print the parameters
   /* The parameters are only not printed if the output level or the log
    *   level is \a Silent.
    */
   bool turnOutOff = false;
   if (outLevel_ != Silent && !out().isOn()) {
      out().on();
      turnOutOff = true;
   }

   bool turnLogOff = false;
   if (logLevel_ != Silent && !out().isLogOn()) {
      out().logOn();
      turnLogOff = true;
   }

   printParameters();

   if (turnOutOff) out().off();
   if (turnLogOff) out().logOff();

   // output a headline for output level \a Subproblem and \a LinearProgram 
   if (outLevel_ == Subproblem ||
	 outLevel_ == LinearProgram ||
	 logLevel_ == Subproblem ||
	 logLevel_ == LinearProgram   ) {

      // turn the streams on 
      bool turnOn = false;

      if (outLevel_ == Subproblem ||
	    outLevel_ == LinearProgram  )
	 out().on();
      else if (out().isOn()) {
	 out().off();
	 turnOn = true;
      }

      bool turnLogOn = false;

      if (logLevel_ == Subproblem ||
	    logLevel_ == LinearProgram)
	 out().logOn();
      else if (out().isLogOn()) {
	 out().logOff();
	 turnLogOn = true;
      }

      out() << endl;
      out() << "   #sub   #open   current   #iter         LP       dual     primal" << endl;

      // turn the streams off 
      if (outLevel_ == Subproblem ||
	    outLevel_ == LinearProgram)
	 out().off();
      if (logLevel_ == Subproblem ||
	    logLevel_ == LinearProgram)
	 out().logOff();

      if (turnOn)
	 out().on();

      if (turnLogOn)
	 out().logOn();

   }


   // perform the branch and bound algorithm 
   // initialize the root node 
   /* The function \a firstSub() is a pure virtual function returning the problem
    *   specific subproblem derived from the class ABA_SUB, which is associated with
    *   the root node of the \bab\ tree.
    */
   status_ = Processing;
#ifdef ABACUS_PARALLEL
   if (parmaster_->isHostZero()) {
      root_  = firstSub();
      openSub_->insert(root_);
      //!< treeInterfaceNewNode(root_);
   }
#else
   root_   = firstSub();
   rRoot_  = root_;

   openSub_->insert(root_);

   treeInterfaceNewNode(root_);
#endif

#ifdef ABACUS_PARALLEL
   //! test, is root used later?
   root_ = rRoot_ = 0;
#endif

   // process all subproblems 
   /* The following lines of code are the core of the \bab\ algorithm.
    *   The function \a select() returns subproblems which still have to be
    *   processed until either the set of open subproblem becomes empty or
    *   one of the resource limits (e.g., maximal cpu time) is exceeded.
    *   If the optimization of a subproblem fails we quit the optimization
    *   immediately..
    */  
   ABA_SUB *current;

   while ((current = select())) {
      ++nSubSelected_;

      if (current->optimize()) {
	 status_ = Error;
	 break;
      }
#ifdef ABACUS_PARALLEL
      if (current->status() == ABA_SUB::Fathomed)
	 delete current;
#endif
   }

   if (status_ == Processing) status_ = Optimal;


   // output history and statistics
   /* The virtual dummy function \a output() can be redefined in derived classes
    *   for problem specific output.
    */  
   // turn the output on if the output level is not \a Silent 
   if (outLevel_ != Silent) out().on();
   if (logLevel_ != Silent) out().logOn();

   // output the solution history 
   out() << endl;
   out() << *history_ << endl;

   // output information on the tree, variables, constraints, etc. 
   const int w = 6;

   out() << "Miscellaneous Statistics" << endl << endl;
   out() << "  Dual bound of the root node       : ";
   out() << setWidth(w) << rootDualBound_     << endl;
   out() << "  Number of subproblems             : ";
   out() << setWidth(w) << nSub_     << endl;
   out() << "  Number of solved LPs              : ";
   out() << setWidth(w) << nLp_      << endl;
   out() << "  Highest level in tree             : ";
   out() << setWidth(w) << highestLevel_   << endl;
   out() << "  Number of fixed variables         : ";
   out() << setWidth(w) << nFixed_   << endl;
   out() << endl;
   out() << "  Number of added constraints       : ";
   out() << setWidth(w) << nAddCons_ << endl;
   out() << "  Number of removed constraints     : ";
   out() << setWidth(w) << nRemCons_ << endl;
   out() << "  Number of added variables         : ";
   out() << setWidth(w) << nAddVars_ << endl;
   out() << "  Number of removed variables       : ";
   out() << setWidth(w) << nRemVars_ << endl;
   out() << endl;
   out() << "  Number of root changes            : ";
   out() << setWidth(w) << nNewRoot_ << endl;

   _outputLpStatistics();
#ifdef ABACUS_PARALLEL
   parmaster_->outputStatistics();
#endif

   out() << endl << endl;


   // output the timing statistics
   /* The cpu time for branching may include time for linear programming.
    *   Therefore, we should not subract it from the total time. Otherwise,
    *   the LP time for LP ranking of branching rules would be subtracted twice.
    */
   double lpPercent;   //!< percentage of lp time of total cpu time
   double sepPercent;  //!< percentage of separation time of total cpu time
   double heuPercent;  //!< percentage of heuristic time of total cpu time
   double priPercent;  //!< percentage of pricing time of total cpu time
   double misPercent;  //!< percentage of miscellaneous time of total cpu time

   long   misTime;     //!< miscellaneous time in seconds

   double totcsecs = (double) totalTime_.centiSeconds();

   misTime =   totalTime_.centiSeconds() - lpTime_.centiSeconds()
      - separationTime_.centiSeconds() - improveTime_.centiSeconds()
      -pricingTime_.centiSeconds();

   ABA_CPUTIMER misTimer(this, misTime);  //!< required for nice output of the value

   if (totcsecs < eps()) totcsecs = 1.0;  //!< avoid zero division in following lines

   lpPercent  = lpTime_.centiSeconds()/totcsecs*100.0;
   sepPercent = separationTime_.centiSeconds()/totcsecs*100.0;
   heuPercent = improveTime_.centiSeconds()/totcsecs*100.0;
   priPercent = pricingTime_.centiSeconds()/totcsecs*100.0;
   misPercent = misTime/totcsecs*100.0;

   const int wpc = 7;

   out() << "Timing Statistics" << endl << endl;
   out() << "  Elapsed time           : " << totalCowTime_ << endl;
   out() << "  Total cpu time         : " << totalTime_   << endl;
   out() << "  LP cpu time            : " << lpTime_;
   out() << "  (" << setWidth(wpc) <<  lpPercent << "%)" << endl;
   out() << "  LP solver cpu time     : " << lpSolverTime_ << endl;
   out() << "  Separation cpu time    : " << separationTime_;
   out() << "  (" << setWidth(wpc) <<  sepPercent << "%)" << endl;
   out() << "  Heuristics cpu time    : " << improveTime_;
   out() << "  (" << setWidth(wpc) << heuPercent << "%)" << endl;
   out() << "  Pricing cpu time       : " << pricingTime_;
   out() << "  (" << setWidth(wpc) << priPercent << "%)" << endl;
   out() << "  Branching cpu time     : " << branchingTime_ << endl;
   out() << "  Miscellaneous cpu time : " << misTimer;
   out() << "  (" << setWidth(wpc) << misPercent << "%)" << endl;


   output();

   // output the value of the best solution 
   out() << endl;
   if (feasibleFound()) 
      out() << "Best solution: " << primalBound_ << endl;
   else
      out() << "No feasible solution found." << endl;

   // output the status of the optimization 
   out() << endl << "ABACUS optimization terminated with status ";

   out() << STATUS_[status_];

   out() << "." << endl;

   // ABA_MASTER::optimize(): clean up and return
   /* Before cleaning up we call the virtual function \a terminateOptimization().
    *   The default implementation of this function does nothing but it
    *   can be used as entrance point for problem specific actions
    *   (e.g., visualization of the best solution).
    *
    *   By deleting the \a root_ of the branch and cut tree, recursively the
    *   destructors of all subproblems in the \bac\ tree are called.
    *
    *   After the optimization we delete the candidates for fixing such
    *   that the variable pools can be safely deleted, as it is an error
    *   to delete a still referenced slot of a pool.
    */

   terminateOptimization();

   delete root_;

   fixCand_->deleteAll();

#ifdef ABACUS_PARALLEL
   if (VbcLog_ == Pipe && parmaster_->isHostZero()) {
#else
      if (VbcLog_ == Pipe) {
#endif
	 char string[256];
	 sprintf(string, "#END_OF_OUTPUT");
	 writeTreeInterface(string);
      }

      return status_;

   }

   void ABA_MASTER::initializeOptimization()
   {}

   void ABA_MASTER::initializePools(ABA_BUFFER<ABA_CONSTRAINT*>  &constraints,
	 ABA_BUFFER<ABA_VARIABLE*>    &variables,
	 int varPoolSize,
	 int cutPoolSize,
	 bool dynamicCutPool)
   {
      // initialize the constraint pool of the ABA_MASTER 
      const int nCons = constraints.number();

      conPool_ = new ABA_STANDARDPOOL<ABA_CONSTRAINT, ABA_VARIABLE>(this, nCons);

#ifdef ABACUS_PARALLEL
      ABA_ID id;
      ABA_IDMAP<ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE>  > *conPoolMap =
	 conPool_->identificationMap();
      int index = conPoolMap->index();
      for (int i = 0; i < nCons; i++) {
	 ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE> *ps = conPool_->insert(constraints[i]);
	 id.initialize(i+1, -1, index);   //!< initialized constraints (proc=-1)
	 ps->setIdentification(id);
      }
#else
      for (int i = 0; i < nCons; i++)
	 conPool_->insert(constraints[i]);
#endif

      // initialize the variable pool of the ABA_MASTER
      /* The size of the variable pool must be at least the number of variables
       *   which are inserted in this function. A variable pool is always an
       *   automatically growing pool, as the addition of a variable must be
       *   always possible for the correctness of the algorithm. Therefore, the
       *   third argument of the constructor of ABA_STANDARDPOOL is \a true.
       */  
      const int nVars = variables.number();

      if (varPoolSize < nVars) varPoolSize = nVars;

      varPool_ = new ABA_STANDARDPOOL<ABA_VARIABLE, ABA_CONSTRAINT>(this, varPoolSize, true);

#ifdef ABACUS_PARALLEL
      ABA_IDMAP<ABA_POOLSLOT<ABA_VARIABLE, ABA_CONSTRAINT>  > *varPoolMap =
	 varPool_->identificationMap();
      index = varPoolMap->index();
      for (int i = 0; i < nVars; i++) {
	 ABA_POOLSLOT<ABA_VARIABLE, ABA_CONSTRAINT> *ps = varPool_->insert(variables[i]);
	 id.initialize(i+1, -1, index);   //!< initialized variables (proc=-1)
	 ps->setIdentification(id);
      }
#else
#ifdef ABACUS_NO_FOR_SCOPE
      for (i = 0; i < nVars; i++)
#else
	 for (int i = 0; i < nVars; i++)
#endif
	    varPool_->insert(variables[i]);
#endif

      // set up the pool for dynamically generated constraints
      /* If the third argument of the constructor of ABA_STANDARDPOOL is missing
       *   the size of the pool is fixed, of the third argument is \a true then
       *   the pool is automatically increased if it is full and an insertion
       *   is performed.
       */  
      if (cutPoolSize > 0)
	 cutPool_ = new ABA_STANDARDPOOL<ABA_CONSTRAINT, ABA_VARIABLE>(this, cutPoolSize,
	       dynamicCutPool);



   }

   void ABA_MASTER::initializePools(ABA_BUFFER<ABA_CONSTRAINT*> &constraints,
	 ABA_BUFFER<ABA_CONSTRAINT*> &cuts,
	 ABA_BUFFER<ABA_VARIABLE*> &variables,
	 int varPoolSize,
	 int cutPoolSize,
	 bool dynamicCutPool)
   {
      initializePools(constraints, variables, varPoolSize, cutPoolSize,
	    dynamicCutPool);

      // is there enough space for all the cuts in the cut pool
      /* Even if the cut pool is dynamic it is an error if the cut pool is
       *   too small. Because before the cut pool is enlarged, inactive, i.e.\
       *   just inserted, constraints would be removed.
       */
      const int nCuts = cuts.number();

      if (nCuts > cutPoolSize) {
	 err() << "ABA_MASTER::initializePools(): size of cutting plane ";
	 err() << " too small for all initialize cuts." << endl;
	 err() << "size of cut pool: " << cutPoolSize << endl;
	 err() << "number of cuts  : " << nCuts << endl;
	 exit(Fatal);
      }


#ifdef ABACUS_PARALLEL
      ABA_ID id;
      int index = cutPool_->identificationMap()->index();
      for (int i = 0; i < nCuts; i++) {
	 ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE> *ps = cutPool_->insert(cuts[i]);
	 id.initialize(i+1, -1, index);   //!< initialized cuts (proc=-1)
	 ps->setIdentification(id);
      }}
#else
      for (int i = 0; i < nCuts; i++)
	 cutPool_->insert(cuts[i]);
#endif
}

void ABA_MASTER::initializeOptSense(ABA_OPTSENSE::SENSE sense)
{
   optSense_.sense(sense);
}

void ABA_MASTER::terminateOptimization()
{ }

ABA_SUB* ABA_MASTER::select()
{
   // check if we should terminate the optimization
   /* If one of the criteria for early termination is satisfied then
    *   we fathom all subproblems of the tree, in order to perform a
    *   correct cleaning up.
    *
    *   The maximal level of the enumeration tree is no termination criterion
    *   in this sense, because it only prevents the generation of further
    *   sons of subproblems having this maximal level, but does not
    *   stop the optimization.
    */  
   if (totalTime_.exceeds(maxCpuTime())) {
      out() << "Maximal CPU time " << maxCpuTime() << " exceeded." << endl;
      out() << "Stop optimization." << endl;
      root_->fathomTheSubTree();
      status_ = MaxCpuTime;
      return 0;
   }

   if (totalCowTime_.exceeds(maxCowTime())) {
      out() << "Maximal elapsed time " << maxCowTime() << " exceeded." << endl;
      out() << "Stop optimization." << endl;
      root_->fathomTheSubTree();
      status_ = MaxCowTime;
      return 0;
   }

   if (guaranteed()) {
      out() << endl;
      out() << "Guarantee " << requiredGuarantee() << " % reached." << endl;
      out() << "Terminate optimization." << endl;
      status_ = Guaranteed;
      root_->fathomTheSubTree();
      return 0;
   }


   return openSub_->select();
}

int ABA_MASTER::enumerationStrategy(const ABA_SUB *s1, const ABA_SUB
      *s2)
{
   switch (enumerationStrategy_) {
      case BestFirst:    return bestFirstSearch(s1, s2);
      case BreadthFirst: return breadthFirstSearch(s1, s2);
      case DepthFirst:   return depthFirstSearch(s1, s2);
      case DiveAndBest:  return diveAndBestFirstSearch(s1, s2);
      default: err() << "ABA_MASTER::enumerationStrategy(): Unknown enumeration strategy" << endl;
	       exit(Fatal);
	       return 0;
   }
}

int ABA_MASTER::bestFirstSearch(const ABA_SUB *s1, const ABA_SUB *s2) const
{
   double dual1 = s1->dualBound();
   double dual2 = s2->dualBound();

   if (optSense()->max()) {
      if (dual1 > dual2) return  1;
      if (dual1 < dual2) return -1;
      return equalSubCompare(s1, s2);
   }
   else {
      if (dual1 > dual2) return -1;
      if (dual1 < dual2) return  1;
      return equalSubCompare(s1, s2);
   }
}

int ABA_MASTER::equalSubCompare(const ABA_SUB *s1, const ABA_SUB *s2) const
{
   if (!s1->branchRule() || !s2->branchRule()) return 0;

   if (s1->branchRule()->branchOnSetVar()
	 && s2->branchRule()->branchOnSetVar()) {
      if (((ABA_SETBRANCHRULE*) s1->branchRule())->setToUpperBound()) {
	 if (((ABA_SETBRANCHRULE*) s2->branchRule())->setToUpperBound())
	    return 0;
	 else 
	    return 1;
      }
      else if (((ABA_SETBRANCHRULE*) s2->branchRule())->setToUpperBound())
	 return -1;
      return 0;
   }
   else return 0;
}

int ABA_MASTER::depthFirstSearch(const ABA_SUB* s1, const ABA_SUB* s2) const
{
   if(s1->level() > s2->level()) return  1;
   if(s1->level() < s2->level()) return -1;
   return equalSubCompare(s1, s2);
}

int ABA_MASTER::breadthFirstSearch(const ABA_SUB* s1, const ABA_SUB* s2) const
{
   if (s1->level() > s2->level()) return -1;
   if (s1->level() < s2->level()) return  1;

   if (s1->id() < s2->id()) return 1;
   return -1;
}

int ABA_MASTER::diveAndBestFirstSearch(const ABA_SUB *s1, const ABA_SUB* s2) const
{
   if (feasibleFound()) return bestFirstSearch(s1, s2);
   else                       return depthFirstSearch(s1, s2);
}

#ifdef ABACUS_PARALLEL
void ABA_MASTER::primalBound(double x, bool broadcast)
#else
void ABA_MASTER::primalBound(double x)
#endif
{
   if (optSense()->max()) {
      if (x < primalBound_) {
	 err() << "Error: ABA_MASTER::primalBound(): got worse" << endl;
	 err() << "old bound: " << primalBound_ << " new bound: " << x << endl;
	 exit (Fatal);
      }
   }
   else
      if (x > primalBound_) {
	 err() << "Error: ABA_MASTER::primalBound(): got worse" << endl;
	 err() << "old bound: " << primalBound_ << " new bound: " << x << endl;
	 exit (Fatal);
      }

   // make sure that this is an integer value for \a objInteger_  
   /* After the test for integrality, we round the value to the next
    *   integer. The reason is that a new primal bound coming obtained
    *   by the solution of the relaxation in a subproblem might have some
    *   garbage in it (e.g., \a 10.000000000034 or \a 9.999999999987). 
    *   With this garbage the 
    *   fathoming of node might fail in the subproblem optimization, although
    *   it is already correct.
    */
   if (objInteger_) {
      if (!isInteger(x, eps())) {
	 err() << "ABA_MASTER::primalBound(): value ";
	 err() << setPrecision(30) << x << " is not integer, ";
	 err() << "but feasible solutions with integer objective ";
	 err() << "function values are expected." << endl;
	 exit(Fatal);
      }

      x = floor(x + eps());
   }


#ifdef ABACUS_PARALLEL
   if (broadcast && betterPrimal(x) )
      parmaster_->newPrimalBound(x);
#endif
   primalBound_ = x;

   // update the primal bound in the Tree Interface 
   if (optSense()->max()) treeInterfaceLowerBound(x);
   else                   treeInterfaceUpperBound(x);

   history_->update();
}

void ABA_MASTER::dualBound(double x)
{

   if (optSense()->max()) {
      if (x > dualBound_) {
	 err() << "Error: ABA_MASTER::dualBound(): got worse" << endl;
	 err() << "old bound: " << dualBound_ << " new bound: " << x << endl;
	 exit (Fatal);
      }
   }
   else
      if (x < dualBound_) {
	 err() << "Error: ABA_MASTER::dualBound(): got worse" << endl;
	 err() << "old bound: " << dualBound_ << " new bound: " << x << endl;
	 exit (Fatal);
      }

   dualBound_ = x;

   //! update the dual bound in the Tree Interface 
   if (optSense()->max()) treeInterfaceUpperBound(x);
   else                   treeInterfaceLowerBound(x);


   history_->update();
}

bool ABA_MASTER::betterDual(double x) const
{
   if (optSense_.max()) {
#ifdef ABACUS_NO_BOOL
      if (x < dualBound_) return true;
      else                return false;
#else
      return x < dualBound_;
#endif
   }
   else {
#ifdef ABACUS_NO_BOOL
      if (x > dualBound_) return true;
      else                return false;
#else
      return x > dualBound_;
#endif
   }
}

bool ABA_MASTER::primalViolated(double x) const
{
   if (optSense_.max()) {
      if (objInteger_) {
#ifdef ABACUS_NO_BOOL 
	 if (x <= primalBound_) return true;
	 else                   return false;
#else
	 return x <= primalBound_;
#endif
      }
      else {
#ifdef ABACUS_NO_BOOL
	 if (x + eps() <= primalBound_) return true;
	 else                           return false;
#else
	 return x + eps() <= primalBound_;
#endif
      }
   }
   else {
      if (objInteger_) {
#ifdef ABACUS_NO_BOOL
	 if (x >= primalBound_) return true;
	 else                   return false;
#else
	 return x >= primalBound_;
#endif
      }
      else {
#ifdef ABACUS_NO_BOOL
	 if (x - eps() >= primalBound_) return true;
	 else                           return false;
#else
	 return x - eps() >= primalBound_;
#endif
      }
   }
}

bool ABA_MASTER::betterPrimal(double x) const
{
   if (optSense_.max()) {
#ifdef ABACUS_NO_BOOL
      if (x > primalBound_) return true;
      else                  return false;
#else
      return x > primalBound_;
#endif
   }
   else {
#ifdef ABACUS_NO_BOOL
      if (x < primalBound_) return true;
      else                  return false;
#else
      return x < primalBound_;
#endif
   }
}

bool ABA_MASTER::feasibleFound() const
{
   if (optSense_.max()) {
#ifdef ABACUS_NO_BOOL
      if (primalBound_ > -infinity()) return true;
      else                            return false;
#else
      return primalBound_ > -infinity();
#endif
   }
   else {
#ifdef ABACUS_NO_BOOL
      if (primalBound_ <  infinity()) return true;
      else                            return false;
#else
      return primalBound_ <  infinity();
#endif
   }
}

void ABA_MASTER::rRoot(ABA_SUB *newRoot, bool reoptimize)
{
#ifdef ABACUS_PARALLEL
   out() << "WARNING: ABA_MASTER::rRoot() not supported in parallel version (ignored)." << endl;
#else
   if (rRoot_ == newRoot) return;

   rRoot_ = newRoot;

   out(1) << "subproblem " << newRoot->id() << " is now root of remaining tree" << endl;

   if ((newRoot->status() == ABA_SUB::Processed ||
	    newRoot->status() == ABA_SUB::Dormant     ) && newRootReOptimize_)
      newRoot->reoptimize();

   ++nNewRoot_;
#endif
}

bool ABA_MASTER::guaranteed()
{
   if (fabs(lowerBound()) < machineEps() &&
	 fabs(upperBound()) > machineEps())
      return false;

   if (guarantee() + machineEps() < requiredGuarantee_) 
      return true;
   else
      return false;
}

double ABA_MASTER::guarantee()
{
   double lb = lowerBound();

   if (fabs(lb) < machineEps()) {
      if (fabs(upperBound()) < machineEps())
	 return 0.0;
      else {    
	 err() << "ABA_MASTER::guarantee(): cannot compute guarantee with lower bound 0" << endl;
	 exit (Fatal);
      }
   }
   return fabs((upperBound() - lb)/lb * 100.0); 
}

void ABA_MASTER::printGuarantee()
{
   double lb = lowerBound();
   double ub = upperBound();

   if (lb == -infinity() || ub == infinity() ||
	 ((fabs(lb) < machineEps()) && (fabs(ub) > machineEps())))
      out() << "---";
   else
      out() << guarantee() << '%';
}

bool ABA_MASTER::check()
{
   double optVal;

   if (!knownOptimum(optVal)) return false;

   if (optVal - eps() < primalBound() && primalBound() < optVal + eps())
      return true;
   else
      return false;
}

bool ABA_MASTER::knownOptimum(double &optVal)
{
   // open the file storing the optimum solutions 
   ifstream optimumFile(optimumFileName_.string(), ios::in );

   if (!optimumFile) return false;

   // go through the lines of the file until the problem is found
   /* As the problem name might be preceded by a path we check if the
    *   \a name read from the file, matches the last letters of the problem name.
    */  
   char name[256];
   double value;

   while (!optimumFile.eof()) {
      optimumFile >> name >> value;

      if (problemName_.ending(name)) {
	 optVal = value;
	 return true;
      }
      optimumFile >> ws;
   }

   return false;

}

void ABA_MASTER::output()
{ }

// Just for fun.

void ABA_MASTER::theFuture()
{
   out() << "making love" << endl;
   out() << "riding my bike" << endl;
   out() << "climbing a mountain" << endl;
   out() << "using my skies" << endl;
   out() << "drinking a beer" << endl << endl;
   out() << "  ... and forgetting about ABACUS" << endl;
}

void ABA_MASTER::writeTreeInterface(const char *info, bool time) const
{
   if (VbcLog_ == NoVbc) return;

#ifdef ABACUS_PARALLEL
   if (!parmaster_->isHostZero()) {
      int len = strlen(info);
      ABA_NOTIFICATION msg(this, 0);
      msg.pack(ABA_NOTIFYSERVER::TreeInterfaceTag);
      msg.pack(len);
      msg.pack(info, len);
      msg.send();
      return;
   }
   treeInterfaceMutex_.acquire();
#endif
   if (VbcLog_ == Pipe) *treeStream_ << '$';
   if (VbcLog_ == File && time) *treeStream_ << totalTime_ << ' ';
   *treeStream_ << info << endl;
#ifdef ABACUS_PARALLEL
   treeInterfaceMutex_.release();
#endif
}

void ABA_MASTER::treeInterfaceNewNode(ABA_SUB *sub) const
{
   if (VbcLog_ != NoVbc) {
      char string[256];
      int fatherId;

#ifdef ABACUS_PARALLEL
      fatherId = sub->fatherId();
#else
      if (sub == root_) fatherId = 0;
      else              fatherId = sub->father()->id();
#endif
      sprintf(string, "N %d %d 5", fatherId, sub->id());
      writeTreeInterface(string);
   }
}

#ifdef ABACUS_PARALLEL
void ABA_MASTER::treeInterfaceNewNode(int fatherId, int id) const
{
   if (VbcLog_ != NoVbc) {
      char string[256];
      sprintf(string, "N %d %d 5", fatherId, id);
      writeTreeInterface(string);
   }
}
#endif

void ABA_MASTER::treeInterfacePaintNode(int id, int color) const
{
   if (VbcLog_ == NoVbc) return;

   char string[256];
   sprintf(string, "P %d %d", id, color);
   writeTreeInterface(string);
}

void ABA_MASTER::treeInterfaceLowerBound(double lb) const
{
   if (VbcLog_ == NoVbc) return;

   char string[256];
   sprintf(string, "L %f", lb);
   writeTreeInterface(string);
}    

void ABA_MASTER::treeInterfaceUpperBound(double ub) const
{
   if (VbcLog_ == NoVbc) return;

   char string[256];
   sprintf(string, "U %f", ub);
   writeTreeInterface(string);
}

void ABA_MASTER::treeInterfaceNodeBounds(int id, double lb, double ub)
{
   if (VbcLog_ == NoVbc) return;

   char string[256];

   if (isInfinity(fabs(lb))) {
      if (isInfinity(fabs(ub)))
	 sprintf(string, "I %d \\iLower Bound: ---\\nUpper Bound:  ---\\i",
	       id);
      else
	 sprintf(string, "I %d \\iLower Bound: ---\\nUpper Bound:  %3.2f\\i",
	       id, ub);
   }
   else {
      if (isInfinity(fabs(ub)))
	 sprintf(string, "I %d \\iLower Bound: %3.2f\\nUpper Bound:  ---\\i",
	       id, lb);
      else
	 sprintf(string, "I %d \\iLower Bound: %3.2f\\nUpper Bound:  %3.2f\\i",
	       id, lb, ub);
   }

   writeTreeInterface(string);
}

const ABA_STRING *ABA_MASTER::problemName() const
{
   return &problemName_;
}

void ABA_MASTER::newSub(int level)
{
   ++nSub_;
   if(level > highestLevel_) highestLevel_ = level;
}

void ABA_MASTER::_initializeParameters()
{
   // ABA_MASTER::_initializeParameters(): local variables
   /* The function \a _initializeLpParameters() initializes the LP solver
    *   specific Parameters. It is called after the parameter-file
    *   {\tt .abacus} was read and all general parameters were processed.
    *
    *   This function is implemented in the file \a lpif.cc.
    */
   ABA_STRING stringVal(this);   //!< auxiliary variable for reading strings 


   if(readParamFromFile_) {
      // set up the name of the configuration file
      /* For UNIX systems the location of the configuration file {\tt
       *   .abacus} is given by the environment variable ABACUS_DIR. For the
       *   Visual \CPLUSPLUS/ compiler the configuration file is expected to be
       *   be located in the directory the program is called from.
       */
      // set up the name of the configuration file 
      char *abacusDir = getenv("ABACUS_DIR");
      if (!abacusDir) {
	 err() << "environment variable ABACUS_DIR not found" << endl;
	 exit(Fatal);
      }

      char *configFileName = new char[strlen(abacusDir) + strlen("/.abacus") + 1];

      sprintf(configFileName, "%s/.abacus", abacusDir);

      readParameters(configFileName);
      // close configuration file 
      delete [] configFileName;

   }else {

      // set default values for abacus parameters 
      insertParameter("EnumerationStrategy","BestFirst");
      insertParameter("BranchingStrategy","CloseHalfExpensive");
      insertParameter("NBranchingVariableCandidates","1");
      insertParameter("Guarantee","0.0");
      insertParameter("MaxLevel","999999");
      insertParameter("MaxCpuTime","99999:59:59");
      insertParameter("MaxCowTime","99999:59:59");
      insertParameter("ObjInteger","false");
      insertParameter("TailOffNLps","0");
      insertParameter("TailOffPercent","0.0001");
      insertParameter("DelayedBranchingThreshold","0");
      insertParameter("MinDormantRounds","1");
      insertParameter("OutputLevel","Full");
      insertParameter("LogLevel","Silent");
      insertParameter("PrimalBoundInitMode","None");
      insertParameter("PricingFrequency","0");
      insertParameter("SkipFactor","1");
      insertParameter("SkippingMode","SkipByNode");
      insertParameter("FixSetByRedCost","true");
      insertParameter("PrintLP","false");
      insertParameter("MaxConAdd","100");
      insertParameter("MaxConBuffered","100");
      insertParameter("MaxVarAdd","500");
      insertParameter("MaxVarBuffered","500");
      insertParameter("MaxIterations","-1");
      insertParameter("EliminateFixedSet","false");
      insertParameter("NewRootReOptimize","false");
      insertParameter("ShowAverageCutDistance","false");
      insertParameter("ConstraintEliminationMode","Basic");
      insertParameter("ConElimEps","0.001");
      insertParameter("ConElimAge","1");
      insertParameter("VariableEliminationMode","ReducedCost");
      insertParameter("VarElimEps","0.001");
      insertParameter("VarElimAge","1");
      insertParameter("VbcLog","None");
      insertParameter("DefaultLpSolver","Clp");
      insertParameter("SolveApprox","false");

      _setDefaultLpParameters();

#ifdef ABACUS_PARALLEL
      parmaster_->setDefaultParameters();
#endif

   };

   initializeParameters();

#ifdef ABACUS_PARALLEL
   parmaster_->initializeParameters();
#endif

   // get the enumeration strategy 

   enumerationStrategy_= (ENUMSTRAT)
      findParameter("EnumerationStrategy",4,ENUMSTRAT_);

   // get the branching strategy 

   branchingStrategy_=(BRANCHINGSTRAT)
      findParameter("BranchingStrategy",2,BRANCHINGSTRAT_);

   // get the number of tested candidates for branching variables 
   assignParameter(nBranchingVariableCandidates_,
	 "NBranchingVariableCandidates",0, INT_MAX);

   // get the solution guarantee 
   assignParameter(requiredGuarantee_,"Guarantee",0.0,infinity());

   // get the maximal level in the enumeration tree 
   assignParameter(maxLevel_,"MaxLevel",1,INT_MAX);

   // get the maximal cpu time 
   assignParameter(stringVal,"MaxCpuTime",0);
   maxCpuTime(stringVal);

   // get the maximal wall clock time 
   assignParameter(stringVal,"MaxCowTime",0);
   maxCowTime(stringVal);

   // get the integrality condition on the objective function 
   assignParameter(objInteger_,"ObjInteger" );

   // get the number of linear programs for the tailing off analysis 
   assignParameter(tailOffNLp_,"TailOffNLps",INT_MIN,INT_MAX );

   // get the minimal improvement for the tailing off analysis 
   assignParameter(tailOffPercent_, "TailOffPercent", 0.0, infinity());

   // get the threshold for delayed branching 
   assignParameter(dbThreshold_, "DelayedBranchingThreshold",0,INT_MAX);

   // get the minimal number of rounds a subproblem should stay dormant 
   assignParameter(minDormantRounds_, "MinDormantRounds",1,INT_MAX );

   // get the output and log level 

   if (outLevel_ != Silent) {
      outLevel_=(OUTLEVEL)
	 findParameter("OutputLevel",5,OUTLEVEL_);
   }
   logLevel_=(OUTLEVEL)findParameter("LogLevel",5,OUTLEVEL_);

   // initialize the log-files 
   /* In order to avoid memory leaks we check if the log-files have not been
    *   initialized somewhere else.
    */  
   if (logLevel_ != Silent) {
      char *problemName = problemName_.string();
      char *logFileName = new char[strlen(problemName)+20];

      if (!out().isLogOn()) {
#ifdef ABACUS_PARALLEL
	 sprintf(logFileName, "%s.log.%d", problemName, parmaster_->hostId());
#else
	 strcpy(logFileName, problemName);
	 strcat(logFileName, ".log");
#endif
	 out().logOn(logFileName);
      }

      if (!err().isLogOn()) {
#ifdef ABACUS_PARALLEL
	 sprintf(logFileName, "%s.error.log.%d", problemName, parmaster_->hostId());
#else
	 strcpy(logFileName, problemName);
	 strcat(logFileName, ".error.log");
#endif

	 err().logOn(logFileName);
      }

      delete [] logFileName;
   }

   // get the primal bound initialization mode 

   pbMode_=(PRIMALBOUNDMODE)
      findParameter("PrimalBoundInitMode",3,PRIMALBOUNDMODE_);

   // get the pricing frequency 
   assignParameter(pricingFreq_,"PricingFrequency",0,INT_MAX);

   // get the skip factor 
   assignParameter(skipFactor_,"SkipFactor",0,INT_MAX);

   // get the skipping mode 

   skippingMode_=(SKIPPINGMODE)
      findParameter("SkippingMode",2,SKIPPINGMODE_ );

   // is fixing/setting by reduced costs turned on? 
   assignParameter(fixSetByRedCost_,"FixSetByRedCost");

   // should the LP be output every iteration? 
   assignParameter(printLP_, "PrintLP");

   // get the maximal number of added constraints 
   assignParameter(maxConAdd_,"MaxConAdd",0,INT_MAX );

   // get the size of the buffer for constraints 
   assignParameter(maxConBuffered_,"MaxConBuffered",0,INT_MAX);


   // get the maximal number of added variables 
   assignParameter(maxVarAdd_,"MaxVarAdd",0,INT_MAX);

   // get the size of the buffer for variables 
   assignParameter(maxVarBuffered_,"MaxVarBuffered",0,INT_MAX);

   // get the maximal number of iterations in the cutting plane phase 
   assignParameter(maxIterations_,"MaxIterations",-1,INT_MAX);

   // should fixed or set variables be eliminated from the LP? 
   assignParameter(eliminateFixedSet_,"EliminateFixedSet");

   // should a new root be reoptimized? 
   assignParameter(newRootReOptimize_,"NewRootReOptimize");


   // get the name of the file storing the optimal solutions       
   /* The name of the file storing the optimal solutions is only an optional
    *   parameter which can be missing.
    */  
   getParameter("OptimumFileName",optimumFileName_);

   // should the average distance of the cuts per iteration be output? 
   assignParameter(showAverageCutDistance_,"ShowAverageCutDistance");

   // get the constraint elimination mode 
   conElimMode_=(CONELIMMODE)
      findParameter("ConstraintEliminationMode",3,CONELIMMODE_);

   // get the variable elimination mode 
   varElimMode_=(VARELIMMODE)findParameter("VariableEliminationMode",2,
	 VARELIMMODE_);

   // get the tolerance for variable elimination 
   assignParameter(varElimEps_,"VarElimEps",0.0,infinity() );

   // get the tolerance for constraint elimination 
   assignParameter(conElimEps_,"ConElimEps",0.0,infinity() );

   // get the age for constraint elimination 
   assignParameter(conElimAge_,"ConElimAge",1,INT_MAX);

   // get the age for variable elimination 
   assignParameter(varElimAge_,"VarElimAge",1,INT_MAX);

   // should a log-file of the enumeration tree be generated? 
   VbcLog_=(VBCMODE)findParameter("VbcLog",3,VBCMODE_);


   //! get the default LP-solver 
   defaultLpSolver_=(OSISOLVER)findParameter("DefaultLpSolver",
	 11,OSISOLVER_);
   assignParameter(solveApprox_, "SolveApprox", false);


   _initializeLpParameters();
}

void ABA_MASTER::initializeParameters()
{ }

void ABA_MASTER::printParameters()
{
   out() << "Branch and Cut Parameters:" << endl << endl;

   out() << "  Enumeration strategy                   : ";
   out() << ENUMSTRAT_[enumerationStrategy_];
   out() << endl;

   out() << "  Branching Strategy                     : ";
   out() << BRANCHINGSTRAT_[branchingStrategy_];
   out() << endl;
   out() << "  Tested candidates for branching var.   : ";
   out() << nBranchingVariableCandidates_ << endl;
   out() << "  Guarantee                              : ";
   out() << requiredGuarantee_ << " %" << endl;
   out() << "  Maximal enumeration level              : ";
   out() << maxLevel_ << endl;
   out() << "  CPU time limit                         : ";
   out() << maxCpuTime_ << endl;
   out() << "  Wall-clock time limit                  : ";
   out() << maxCowTime_ << endl;
   out() << "  Objective function values integer      : ";
   out() << onOff(objInteger_) << endl;
   out() << "  Tailing Off Parameters" << endl;
   out() << "                    Number of LPs        : ";
   out() << tailOffNLp_ << endl;
   out() << "                    Minimal improvement  :    ";
   out() << tailOffPercent_ << '%' << endl;
   out() << "  Delayed branching threshold            : ";
   out() << dbThreshold_ << endl;
   out() << "  Maximal number of dormant rounds       : ";
   out() << minDormantRounds_ << endl;

   out() << "  Output Level                           : ";
   out() << OUTLEVEL_[outLevel_];
   out() << endl;

   out() << "  Log Level                              : ";
   out() << OUTLEVEL_[logLevel_];
   out() << endl;

   out() << "  Primal Bound Initialization            : ";
   out() << PRIMALBOUNDMODE_[pbMode_];
   out() << endl;
   out() << "  Frequency of additional pricing        : ";
   out() << pricingFreq_ << " LPs" << endl;
   out() << "  Cutting skip factor                    : ";
   out() << skipFactor_ << endl;
   out() << "  Skipping mode                          : ";
   if (skippingMode_ == SkipByNode) out() << "by node" << endl;
   else                        out() << "by tree" << endl;

   out() << "  Fix/set by reduced costs               : ";
   out() << onOff(fixSetByRedCost_) << endl;
   out() << "  Output of the linear program           : ";
   out() << onOff(printLP_) << endl;
   out() << "  Maximal number of added constraints    : ";
   out() << maxConAdd_ << endl;
   out() << "  Maximal number of buffered constraints : ";
   out() << maxConBuffered_ << endl;
   out() << "  Maximal number of added variables      : ";
   out() << maxVarAdd_ << endl;
   out() << "  Maximal number of buffered variables   : ";
   out() << maxVarBuffered_ << endl;
   out() << "  Maximal number of iterations per" << endl;
   out() << "                     cutting plane phase : ";
   out() << maxIterations_ << endl;
   out() << "  Elimination of fixed and set variables : ";
   out() << onOff(eliminateFixedSet_) << endl;
   out() << "  Reoptimization after a root change     : ";
   out() << onOff(newRootReOptimize_) << endl;
   out() << "  File storing optimum solutions         : ";
   out() << optimumFileName_ << endl;
   out() << "  Show average distance of added cuts    : ";
   out() << onOff(showAverageCutDistance_) << endl;
   out() << "  Elimination of constraints             : ";
   out() << CONELIMMODE_[conElimMode_] << endl;
   out() << "  Elimination of variables               : ";
   out() << VARELIMMODE_[varElimMode_] << endl;
   out() << "  Tolerance for constraint elimination   : ";
   out() << conElimEps_ << endl;
   out() << "  Tolerance for variable elimination     : ";
   out() << varElimEps_ << endl;
   out() << "  Age for constraint elimination         : ";
   out() << conElimAge_ << endl;
   out() << "  Age for variable elimination           : ";
   out() << varElimAge_ << endl;
   out() << "  Default LP-solver                      : ";
   out() << OSISOLVER_[defaultLpSolver_] << endl;
   out() << "  Usage of approximate solver            : ";
   out() << onOff(solveApprox_) << endl;
   _printLpParameters();
#ifdef ABACUS_PARALLEL
   parmaster_->printParameters();
#endif
}

void ABA_MASTER::nBranchingVariableCandidates(int n)
{
   if (n < 1) {
      err() << "ABA_MASTER::nBranchingVariableCandidates() invalid argument" << endl;
      err() << "correct value: positive integer number" << endl;
      exit(Fatal);
   } 
   nBranchingVariableCandidates_ = n;
}

void ABA_MASTER::requiredGuarantee(double g)
{
   if (g < 0.0) {
      err() << "ABA_MASTER::guarantee(" << g << "): ";
      err() << "choose nonnegative value." << endl;
      exit (Fatal);
   }

   requiredGuarantee_ = g;
}

void ABA_MASTER::maxLevel(int max)
{
   if (max < 1) {
      err() << "ABA_MASTER::maxLevel(" << max << "): ";
      err() << "only positive integers are valid" << endl;
      exit(Fatal);
   }
   maxLevel_ = max;
}

void ABA_MASTER::tailOffPercent(double p) 
{
   if (p < 0.0) {
      err() << "ABA_MASTER::tailing_off(p): choose nonnegative value" << endl;
      exit(Fatal);
   }
   tailOffPercent_ = p;
}

bool ABA_MASTER::delayedBranching(int nOpt) const
{
   if (nOpt >= dbThreshold_ + 1) return false;
   else                          return true;
}

void ABA_MASTER::pricingFreq(int f)
{
   if (f < 0) {
      err() << "ABA_MASTER::pricingFreq(): nonnegative frequency expected" << endl;
      exit(Fatal);
   }
   pricingFreq_ = f;
}

void ABA_MASTER::skipFactor(int f)
{
   if (f < 0) {
      err() << "ABA_MASTER::skipFactor(): nonnegative value expected" << endl;
      exit(Fatal);
   }
   skipFactor_ = f;
}

void ABA_MASTER::rootDualBound(double x) 
{
   rootDualBound_ = x;
}

bool ABA_MASTER::setSolverParameters(OsiSolverInterface* interface, bool solverIsApprox)
{
   return false;
}
