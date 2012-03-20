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
 * $Id: sub.cc,v 2.17 2008-10-10 09:33:04 sprenger Exp $
 */

#include <iomanip>
using namespace std;

#include "abacus/master.h"
#include "abacus/exceptions.h"
#include "abacus/sub.h"
#include "abacus/lpsub.h"
#include "abacus/tailoff.h"
#include "abacus/constraint.h"
#include "abacus/branchrule.h"
#include "abacus/lpvarstat.h"
#include "abacus/slackstat.h"
#include "abacus/bprioqueue.h"
#include "abacus/variable.h"
#include "abacus/column.h"
#include "abacus/poolslot.h"
#include "abacus/cutbuffer.h"
#include "abacus/opensub.h"
#include "abacus/fixcand.h"
#include "abacus/setbranchrule.h"
#include "abacus/boundbranchrule.h"
#include "abacus/active.h"
#include "abacus/standardpool.h"



  ABA_SUB::ABA_SUB(
               ABA_MASTER *master,
               double conRes, 
               double varRes, 
               double nnzRes, 
               bool relativeRes,
               ABA_BUFFER<ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE> *> *constraints,
               ABA_BUFFER<ABA_POOLSLOT<ABA_VARIABLE, ABA_CONSTRAINT> *> *variables) 
    :  
    master_(master),  
    actCon_(0),  
    actVar_(0),  
    father_(0),  
    lp_(0),  
    fsVarStat_(0),  
    lpVarStat_(0),  
    lBound_(0),  
    uBound_(0),  
    slackStat_(0),  
    tailOff_(0),  
    dualBound_(master->dualBound()),  
    nIter_(0),  
    lastIterConAdd_(0),  
    lastIterVarAdd_(0),  
    branchRule_(0),  
    allBranchOnSetVars_(true),  
    lpMethod_(ABA_LP::Primal),  
    addVarBuffer_(0),  
    addConBuffer_(0),  
    removeVarBuffer_(0),  
    removeConBuffer_(0),  
    xVal_(0),  
    yVal_(0),  
    genNonLiftCons_(false),  
    level_(1),  
    id_(1),  
    status_(Unprocessed),  
    sons_(0),  
    maxIterations_(master->maxIterations()),  
    nOpt_(0),  
    relativeReserve_(relativeRes),  
    varReserve_(varRes),  
    conReserve_(conRes),  
    nnzReserve_(nnzRes),  
    activated_(false),  
    ignoreInTailingOff_(false),  
    //lastLP_(ABA_LP::BarrierAndCrossover),
    lastLP_(ABA_LP::Primal),
    localTimer_(master),
    forceExactSolver_(false)
  {
    // initialize the active constraints of the root node 
    int maxCon;
    
    if (constraints) {
      // initialize the active constraints with \a constraints 
  if (relativeRes)
    maxCon   = (int) (constraints->number() * (1.0 + conRes/100.0));
  else
    maxCon   = constraints->number() + (int) conRes;
        
  actCon_ = new ABA_ACTIVE<ABA_CONSTRAINT, ABA_VARIABLE>(master_, maxCon);
      
  actCon_->insert(*constraints);

    }
    else  {
     // initialize the active constraints with the default constraint pool 
  if (relativeRes)
    maxCon  = (int) (master_->conPool()->number() * (1.0 + conRes/100.0));
  else
    maxCon = master_->conPool()->number() + (int) conRes;
        
  actCon_ = new ABA_ACTIVE<ABA_CONSTRAINT, ABA_VARIABLE>(master_, maxCon);

  const int nConPool = master_->conPool()->number();

  for (int i = 0; i < nConPool; i++)
    actCon_->insert(master_->conPool()->slot(i));

    }

    slackStat_ = new ABA_ARRAY<ABA_SLACKSTAT*>(master_, maxCon);

    const int nConstraints = nCon();

    for (int i = 0; i < nConstraints; i++)
      (*slackStat_)[i] = new ABA_SLACKSTAT(master_);

    // initialize the active variables of the root node 
  int maxVar;
  if (variables) {
    // initialize the active variables with \a variables 
  if (relativeRes)
    maxVar  = (int) (variables->number() * (1.0 + varRes/100.0));
  else
    maxVar = variables->number() + (int) varRes;
  actVar_ = new ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT>(master_, maxVar);
      
  actVar_->insert(*variables);

  }
  else  {
    // initialize the active variables with the default variable pool 
  if (relativeRes)
    maxVar  = (int) (master_->varPool()->number() * (1.0 + varRes/100.0));
  else
    maxVar = master_->varPool()->number() + (int) varRes;
  actVar_ = new ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT>(master_, maxVar);

  const int nVarPool = master_->varPool()->number();
  for (int i = 0; i < nVarPool; i++)
    actVar_->insert(master_->varPool()->slot(i));

  }

// initializes the local variables statuses and the bounds
/* By initializing \a *fsVarStat_ with the global status we
 *   can both handle prefixed variables and enable automatically
 *   that all fixed variables stay fixed in the case of a restart
 *   of the optimization process.
 */
  fsVarStat_  = new ABA_ARRAY<ABA_FSVARSTAT*>(master_, maxVar);
  lpVarStat_  = new ABA_ARRAY<ABA_LPVARSTAT*>(master_, maxVar);
  lBound_     = new ABA_ARRAY<double>(master_, maxVar);
  uBound_     = new ABA_ARRAY<double>(master_, maxVar);
                                 
  ABA_VARIABLE *v;

  const int nVariables = nVar();
  for (int i = 0; i < nVariables; i++) {
    v                 = variable(i);
    (*fsVarStat_)[i]  = new ABA_FSVARSTAT(v->fsVarStat());
    (*lpVarStat_)[i]  = new ABA_LPVARSTAT(master_);
    (*lBound_)[i]     = v->lBound();
    (*uBound_)[i]     = v->uBound();
  }


  //! register the subproblem at the master 

  }

  ABA_SUB::ABA_SUB(ABA_MASTER *master, ABA_SUB *father, ABA_BRANCHRULE *branchRule) 
    :  
    master_(master),  
    actCon_(0),  
    actVar_(0),  
    father_(father),  
    lp_(0),  
    fsVarStat_(0),  
    lpVarStat_(0),  
    lBound_(0),  
    uBound_(0),  
    slackStat_(0),  
    tailOff_(0),  
    dualBound_(father->dualBound_),  
    nIter_(0),  
    lastIterConAdd_(0),  
    lastIterVarAdd_(0),  
    branchRule_(branchRule),  
    lpMethod_(ABA_LP::Dual),  
    addVarBuffer_(0),  
    addConBuffer_(0),  
    removeVarBuffer_(0),  
    removeConBuffer_(0),  
    xVal_(0),  
    yVal_(0),  
    bInvRow_(0),  
    genNonLiftCons_(false),  
    level_(father->level() + 1),  
    id_(master->nSub() + 1),  
    status_(Unprocessed),  
    sons_(0),  
    maxIterations_(master->maxIterations()),  
    nOpt_(0),  
    relativeReserve_(father_->relativeReserve_),  
    varReserve_(father_->varReserve_),  
    conReserve_(father_->conReserve_),  
    nnzReserve_(father_->nnzReserve_),  
    activated_(false),  
    ignoreInTailingOff_(false) ,  
    //lastLP_(ABA_LP::BarrierAndCrossover),
    lastLP_(ABA_LP::Primal),
    localTimer_(master),
    forceExactSolver_(false)
  {
    branchRule_->initialize(this);
    allBranchOnSetVars_ = father_->allBranchOnSetVars_ &&
                                             branchRule_->branchOnSetVar();
#ifdef ABACUS_PARALLEL
// determine the initial maximal number of constraints and variables
/* We overestimate the initial number of constraints and variables in
 *   order to avoid too many reallocations during the optimization process.
 *
 *   If the subproblem is the root node or has been processed already, then
 *   this overestimation is not required, because extra memory has either
 *   been allocated already in the constructor or is available from the
 *   last optimization respectively.
 */   
  int initialMaxVar;
  int initialMaxCon;

#ifdef ABACUS_PARALLEL
  if (status_ == Unprocessed && level_ > 1) {
#else
  if (status_ == Unprocessed && this != master_->root()) {
#endif
    if (relativeReserve_) {
      initialMaxVar = (int) (father_->nVar() *(1.0 + varReserve_/100.0));
      initialMaxCon = (int) (father_->nCon() *(1.0 + conReserve_/100.0));
    }
    else {
      initialMaxVar = father_->nVar() + (int) varReserve_;
      initialMaxCon = father_->nCon() + (int) conReserve_;
    }
  }
  else {
    initialMaxVar = maxVar();
    initialMaxCon = maxCon();
  }

    initializeVars(initialMaxVar);
    initializeCons(initialMaxCon);
    // in the parallel version the father is no longer needed (prohibit access)
    father_ = 0;
#else
    master_->newSub(level_);
    master_->treeInterfaceNodeBounds(id_, lowerBound(), upperBound());
#endif
  }
  
  ABA_SUB::~ABA_SUB()
  {
    if (sons_) {
      const int nSons = sons_->number();

      for (int i = 0; i < nSons; i++) delete (*sons_)[i];
      delete sons_;
    }
    else if (status_ == Unprocessed || status_ == Dormant)
      master_->openSub()->remove(this);
  }



  int ABA_SUB::optimize()
  {

    PHASE phase;  //!< current phase of the subproblem optimization

// update the global dual bound 
/* The global dual bound is the maximum (minimum) of the
 *   dual bound of the subproblem and the dual bounds of the
 *   subproblems which still have to be processed if this
 *   is a maximization (minimization) problem.
 */  
  double newDual = dualBound_;
    
  if (master_->optSense()->max()) {
    if (master_->openSub()->dualBound() > newDual)
      newDual = master_->openSub()->dualBound();
  }
  else
    if (master_->openSub()->dualBound() < newDual)
      newDual = master_->openSub()->dualBound();

  if (master_->betterDual(newDual)) master_->dualBound(newDual);

  
// output a banner for the subproblem 
  master_->out() << endl << "************************************************" << endl;
  master_->out() << "Subproblem " << id_ << " on Level " << level_ << ":" << endl << endl;

  if (master_->optSense()->max()) {
    master_->out(1) << "Global Lower Bound: " << lowerBound()       << endl;
    master_->out(1) << "Local  Upper Bound: " << upperBound()       << endl;
    master_->out(1) << "Global Upper Bound: " << master_->upperBound() << endl;
  }
  else {
    master_->out(1) << "Local  Lower Bound: " << lowerBound()       << endl;
    master_->out(1) << "Global Lower Bound: " << master_->lowerBound() << endl;
    master_->out(1) << "Global Upper Bound: " << upperBound()       << endl;
  }

  master_->out(1) << "Current Guarantee : ";
  master_->printGuarantee();
  master_->out() << endl << endl;


    ++nOpt_;

    phase = _activate ();

    while (phase != Done) {
      switch (phase) {
        case Cutting:   phase = cutting();
                        break;
        case Branching: phase = branching();
                        break;
        case Fathoming: phase = fathoming ();
                        break;
        default:        
			char *_error=0;
			sprintf(_error,"ABA_SUB::optimize(): unknown phase %i\nFurther processing not possible.",phase);
			THROW_PARAM(AlgorithmFailureException,afcPhase,_error);
      }
    }
    _deactivate ();

  //! output some infos about the enumeration tree 
  if (master_->outLevel() == ABA_MASTER::Subproblem ||
      master_->outLevel() == ABA_MASTER::LinearProgram ||
      master_->logLevel() == ABA_MASTER::Subproblem ||
      master_->logLevel() == ABA_MASTER::LinearProgram   ) {

// ABA_SUB::optimize(): turn the streams on
/* If the \a logLevel is not \a Subproblem or \a LinearProgram and turned on then
 *   we have to turn it off, such that there is not this redundant output.
 *   With the variable \a turnLogOn we memorize if we have to turn the logging
 *   on again after the output is performed.
 */  
  bool turnOutOn = false;

  if (master_->outLevel() == ABA_MASTER::Subproblem ||
      master_->outLevel() == ABA_MASTER::LinearProgram  )
    master_->out().on();
  else if (master_->out().isOn()) {
    master_->out().off();
    turnOutOn = true;
  }
    

  bool turnLogOn = false;

  if (master_->logLevel() == ABA_MASTER::Subproblem ||
      master_->logLevel() == ABA_MASTER::LinearProgram)
    master_->out().logOn();
  else if (master_->out().isLogOn()) {
    master_->out().logOff();
    turnLogOn = true;
  }

// output a line about the subproblem optimization 
/* We output the total number of subproblems, the number of open subproblems,
 *   the number of iterations in the cutting plane phase, the dual bound,
 *   and the primal bound.
 */  
  master_->out() << setWidth(7) << master_->nSub() << " ";
  master_->out() << setWidth(7) << master_->openSub()->number() << "  ";
  master_->out() << setWidth(8) << id_ << " ";
  master_->out() << setWidth(7) << nIter_ << " ";
  if (infeasible())
    master_->out() << setWidth(10) << "infeas" << " ";
  else
    master_->out() << setWidth(10) << dualBound() << " ";
  master_->out() << setWidth(10) << master_->dualBound() << " ";
  if (master_->feasibleFound())
    master_->out() << setWidth(10) << master_->primalBound();
  else
    master_->out() << setWidth(10) << "---";
  master_->out() << endl;

// ABA_SUB::optimize(): turn the streams off 
  if (master_->outLevel() == ABA_MASTER::Subproblem ||
      master_->outLevel() == ABA_MASTER::LinearProgram)
    master_->out().off();
  if (master_->logLevel() == ABA_MASTER::Subproblem ||
      master_->logLevel() == ABA_MASTER::LinearProgram)
    master_->out().logOff();

  if (turnLogOn)
    master_->out().logOn();

  if (turnOutOn)
    master_->out().on();

  }
  else {
    master_->out() << "Enumeration Tree" << endl;
    master_->out(1) << "Number of Subproblems:   " << master_->nSub() << endl;
    master_->out(1) << "Number of Open Problems: " << master_->openSub()->number() << endl;
  }


    return 0;
  }

  ABA_SUB::PHASE ABA_SUB::_activate()
  {
// activate the subproblem in the VBC-Tool 
  master_->treeInterfacePaintNode(id_, 6);

// can the subproblem be fathomed without processing?
/* If during the subproblem was waiting for further processing
 *   in the list of open subproblems,
 *   a primal bound better than its dual bound has been found, then
 *   we can fathom the subproblem immediately.
 */
  if (boundCrash()) return Fathoming;

#ifdef ABACUS_PARALLEL
    assert(actCon_ && actVar_);
    // actVar_, actCon_ etc. are already valid.
    // we only have to allocate the local members
    int initialMaxVar = actVar_->max();
    int initialMaxCon = actCon_->max();
  
  // allocate local members of the subproblem 
  tailOff_  = new ABA_TAILOFF(master_);
  addVarBuffer_    = new ABA_CUTBUFFER<ABA_VARIABLE, ABA_CONSTRAINT>(master_, 
                                                       master_->maxVarBuffered());
  addConBuffer_    = new ABA_CUTBUFFER<ABA_CONSTRAINT, ABA_VARIABLE>(master_,
                                                       master_->maxConBuffered());
  removeVarBuffer_ = new ABA_BUFFER<int>(master_, initialMaxVar);
  removeConBuffer_ = new ABA_BUFFER<int>(master_, initialMaxCon);
  xVal_            = new double[initialMaxVar];
  yVal_            = new double[initialMaxCon];

    if (status_ == Unprocessed && branchRule_) {
      if (branchRule_->extract(this))
        return Fathoming;
    }
    else
      assert(branchRule_ == 0);
#else

// determine the initial maximal number of constraints and variables
/* We overestimate the initial number of constraints and variables in
 *   order to avoid too many reallocations during the optimization process.
 *
 *   If the subproblem is the root node or has been processed already, then
 *   this overestimation is not required, because extra memory has either
 *   been allocated already in the constructor or is available from the
 *   last optimization respectively.
 */   
  int initialMaxVar;
  int initialMaxCon;

#ifdef ABACUS_PARALLEL
  if (status_ == Unprocessed && level_ > 1) {
#else
  if (status_ == Unprocessed && this != master_->root()) {
#endif
    if (relativeReserve_) {
      initialMaxVar = (int) (father_->nVar() *(1.0 + varReserve_/100.0));
      initialMaxCon = (int) (father_->nCon() *(1.0 + conReserve_/100.0));
    }
    else {
      initialMaxVar = father_->nVar() + (int) varReserve_;
      initialMaxCon = father_->nCon() + (int) conReserve_;
    }
  }
  else {
    initialMaxVar = maxVar();
    initialMaxCon = maxCon();
  }

  // allocate local members of the subproblem 
  tailOff_  = new ABA_TAILOFF(master_);
  addVarBuffer_    = new ABA_CUTBUFFER<ABA_VARIABLE, ABA_CONSTRAINT>(master_, 
                                                       master_->maxVarBuffered());
  addConBuffer_    = new ABA_CUTBUFFER<ABA_CONSTRAINT, ABA_VARIABLE>(master_,master_->maxConBuffered());
  removeVarBuffer_ = new ABA_BUFFER<int>(master_, initialMaxVar);
  removeConBuffer_ = new ABA_BUFFER<int>(master_, initialMaxCon);
  xVal_            = new double[initialMaxVar];
  yVal_            = new double[initialMaxCon];

// perform activations for unprocessed non-root nodes 
/* The initialization of constraints and variables is performed by
 *   virtual functions such that easily other initialization methods
 *   can be applied. 
  
 *   If there is a contradiction between a branching rule and the local
 *   information of the subproblem we can immediately fathom the subproblem.
 */
  if (status_ == Unprocessed && this != master_->root()) {
    initializeVars(initialMaxVar);
    initializeCons(initialMaxCon);
    if (branchRule_->extract(this))
      return Fathoming;
  }

#endif
    
// remove missing variables and constraints from the active sets  
/* After the function \a _activate() is performed we assume during the
 *   complete subproblem optimization that all active
 *   variables and constraints are available in some pool! Therefore, we
 *   remove now all missing variables and constraints from their active sets.
 */  

// remove missing variables from the active variables 
/* It is a fatal error if a fixed or set variable is missing.
 */
  ABA_BUFFER<int> removeVars(master_, nVar());

  int nVariables = nVar();

  for (int i = 0; i < nVariables; i++)
    if ((*actVar_)[i] == 0) {
      removeVars.push(i);
      if ((*fsVarStat_)[i]->fixedOrSet()) {
        const char *_error="ABA_SUB::_activate(): active fixed or set variable not available in pool";
  	THROW_PARAM(AlgorithmFailureException,afcActive,_error);
      }
      delete (*fsVarStat_)[i];
      delete (*lpVarStat_)[i];
    }

  if (removeVars.number()) {
    master_->out() << removeVars.number() << " variables missing for initialization" << endl;

    actVar_->remove(removeVars);
    fsVarStat_->leftShift(removeVars);
    lpVarStat_->leftShift(removeVars);
    lBound_->leftShift(removeVars);
    uBound_->leftShift(removeVars);
  }

// remove missing constraints from the active variables 
  ABA_BUFFER<int> removeCons(master_, nCon());

  const int nConstraints = nCon();
  for (int i = 0; i < nConstraints; i++)
    if ((*actCon_)[i] == 0) {
      removeCons.push(i);
      delete (*slackStat_)[i];
    }
  if (removeCons.number())
    master_->out() << removeCons.number() << " constraints missing for initialization" << endl;

  actCon_->remove(removeCons);
  slackStat_->leftShift(removeCons);


// set the active flags of variables and constraints      
/* Setting \a status_ to \a Active at this point is necessary, since if the
 *   subproblem turns out to be fathomed already during processing
 *   this function, then in \a fathom() the variables and constraints
 *   have to be deactivated.
 */  
  const int nActVar = actVar_->number();
  for (int i = 0; i < nActVar; i++)
    (*actVar_)[i]->activate();

  const int nActCon = actCon_->number();
  for (int i = 0; i < nActCon; i++)
    (*actCon_)[i]->activate();

  status_  = Active;  

// perform problem specific activations 
/* We have to memorize if activate() has been called such that in 
 *   \a _deactivate() only \a deactivate() is called when \a activate() has
 *   been performed. This is necessary because these lines are only
 *   reached if the dual bound is still better than the primal bound.
 */
  activate();
  activated_ = true;

// update fixed and set variables and set by logical implications
/* We update global variable fixings which have been performed
 *   while the subproblem was sleeping. If there is a contradiction to
 *   set variables we can fathom the node.

 *   The adaption of branching variables may allow us to set further
 *   variables by logical implications. Again contradictions to
 *   already fixed variables can lead to an immediate \a Fathoming
 *   of the node.
 */
  ABA_FSVARSTAT *global;     //!< global status of a variable
  ABA_FSVARSTAT *local;      //!< local status of a variable
  double     newBound;   //!< the new local bound

  nVariables = nVar();
  for (int i = 0; i < nVariables; i++) {
    global = variable(i)->fsVarStat();
    local  = (*fsVarStat_)[i];
    if (global->fixed()) {
      if (global->contradiction(local)) {
        infeasibleSub();
        return Fathoming;
      }
      local->status(global);
      newBound      = fixSetNewBound(i);
      (*lBound_)[i] = newBound;
      (*uBound_)[i] = newBound;
    }
  }

  bool newValues;  //!< in this context only required as a dummy

  if (_setByLogImp(newValues)) {
    infeasibleSub();
    return Fathoming;
  }

  // output the dimension of the subproblem 
  if (master_->out().isOn() || master_->out().isLogOn()) {
  
  // output number of active constraints and variables 
  master_->out() << endl;
  master_->out() << "Subproblem Size" << endl;
  master_->out(1) << "Number of Active Constraints : " << nCon() << endl;
  master_->out(1) << "Number of Active Variables   : " << nVar() << endl;

  // output number of fixed and set variables 
  int nFixed = 0;  //!< number of fixed variables
  int nSet   = 0;  //!< number of set variables

  const int nVariables = nVar();
  for (int i = 0; i < nVariables; i++)
    if ((*fsVarStat_)[i]->fixed()) ++nFixed;
    else if ((*fsVarStat_)[i]->set()) ++nSet;

  master_->out(1) << "Number of Fixed Variables    : " << nFixed << endl;
  master_->out(1) << "Number of Set Variables      : " << nSet   << endl;

  }

// initialize the linear program of the subproblem
/* If the \a LP turns out to be infeasible already in
 *   the initialization phase, we can again fathom the \bac\ node.
 */  
  if (initializeLp()) {
    infeasibleSub();
    return Fathoming;
  }

    
    return Cutting;
  }

  void ABA_SUB::activate()
  { }

  void ABA_SUB::initializeVars(int maxVar)
  {
    actVar_ = new ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT>(master_, 
                                                   father_->actVar_, maxVar);

    fsVarStat_ = new ABA_ARRAY<ABA_FSVARSTAT*>(master_, maxVar);
    lpVarStat_ = new ABA_ARRAY<ABA_LPVARSTAT*>(master_, maxVar);
    lBound_    = new ABA_ARRAY<double>(master_, maxVar);
    uBound_    = new ABA_ARRAY<double>(master_, maxVar);

    const int nVariables = nVar();
                                 
    for (int i = 0; i < nVariables; i++) {
      (*lpVarStat_)[i]  = new ABA_LPVARSTAT(father_->lpVarStat(i));
      (*fsVarStat_)[i]  = new ABA_FSVARSTAT(father_->fsVarStat(i));
      (*lBound_)[i]     = father_->lBound(i);
      (*uBound_)[i]     = father_->uBound(i);
    }
  }

  void ABA_SUB::initializeCons(int maxCon)
  {
    actCon_ = new ABA_ACTIVE<ABA_CONSTRAINT, ABA_VARIABLE>(master_, 
                                                  father_->actCon_, maxCon);

    slackStat_ = new ABA_ARRAY<ABA_SLACKSTAT*>(master_, maxCon);

    const int nConstraints = nCon();
    
    for (int i = 0; i < nConstraints; i++)
      (*slackStat_)[i] = new ABA_SLACKSTAT(*(father_->slackStat(i)));
  }
  
  void ABA_SUB::_deactivate()
  {
    if (activated_) deactivate();
    // deactivate the subproblem in the VBC-Tool 
  master_->treeInterfacePaintNode(id_, 1);

  // delete members redundant for inactive subproblems 
  delete tailOff_;
  tailOff_ = 0;

  localTimer_.start(true);

  delete lp_;

  master_->lpTime_.addCentiSeconds(localTimer_.centiSeconds());

  lp_ = 0;

  delete addVarBuffer_;
  addVarBuffer_ = 0;
  delete addConBuffer_;
  addConBuffer_ = 0;
  delete removeVarBuffer_;
  removeVarBuffer_ = 0;
  delete removeConBuffer_;
  removeConBuffer_ = 0;
  delete [] xVal_;
  xVal_ = 0;
  delete [] yVal_;
  yVal_ = 0;

// reset the active flags of variables and constraints 
/* If the node being deactivated has just been fathomed then
 *   \a actVar_ and \a actCon_ are 0. In this case the deactivation
 *   has been performed already in the function \a fathom().
 */  
  int i;
  
  if (actVar_) {
    const int nActVar = actVar_->number();
    for (i = 0; i < nActVar; i++)
      (*actVar_)[i]->deactivate();
  }

  if (actCon_) {
    const int nActCon = actCon_->number();
    for (i = 0; i < nActCon; i++)
      (*actCon_)[i]->deactivate();
  }
    
// deactive the root node 
  if (this == master_->root())
    master_->rootDualBound(dualBound_);

  }

  void ABA_SUB::deactivate()
  { }

  int ABA_SUB::_setByLogImp(bool &newValues)
  {
    master_->out() << "Setting Variables by Logical Implications: " << flush;
  // call the virtual function to set variables by logical implications 
  ABA_BUFFER<int>        variables(master_, nVar());
  ABA_BUFFER<ABA_FSVARSTAT*> status(master_, nVar());

  setByLogImp(variables, status);

  // check for contradictions and variables set to new values 
  int contra = 0;
  bool lNewValues;

  newValues = false;

  const int nVariables = variables.number();
    
  for (int i = 0; i < nVariables; i++) {
    contra = set(variables[i], status[i], lNewValues);
    if (contra) break;
    if (lNewValues) newValues = true;
  }

// ABA_SUB::_setByLogImp(): clean up and return 
  for (int i = 0; i < nVariables; i++)
    delete status[i];

  if (contra)
    master_->out() << "contradiction found" << endl;
  else
    master_->out() << nVariables << " variables set" << endl;
    
  return contra;

  }

  void ABA_SUB::setByLogImp(ABA_BUFFER<int> &variables, 
                            ABA_BUFFER<ABA_FSVARSTAT*> &status)
  { }

   ABA_SUB::PHASE ABA_SUB::cutting ()
   {
// ABA_SUB::cutting(): local variables
/* Before we are going to branch we would like to remove (e.g., non-binding)
 *   constraints. Such
 *   final modifications can be performed in the function
 *   \a prepareBranching(). If in this function the problem is modified,
 *   the variable \a lastIteration becomes \a true and now we perform the
 *   branching indeed. Only for convenience we modify the active constraints
 *   at the beginning of the cutting plane algorithm in this case.
 */
   int status;      //!< return status of some called functions
   bool newValues;  //!< \a true if variable fix or set to new value
   bool lastIteration = false; 

   int nVarRemoved;
   int nConRemoved;
   int nVarAdded;
   int nConAdded;
   //bool doFixSet = true;


     while (1) {
// add and remove variables and constraints
/* Added/removed variables/constraints are not directly added/removed but
 *   stored in a buffer. Here, at the beginning of the inner loop of the
 *   cutting plane algorithm we update the active constraints and variables
 *   and the linear program. If more constraints/variables are buffered than
 *   actually should be added, we select the best ones if possible.
 */  

// test if feasibility of basis could be destroyed 
/* This test does not check all possibilities, e.g., variables
 *   might have been fixed, and hence, destroy the feasibility of the basis.
 */

  if (addVarBuffer_->number() && addConBuffer_->number()) {
    master_->out() << "ABA_SUB::cutting(): WARNING: adding variables and constraints" << endl;
    master_->out() << "                         basis might become infeasible" << endl;
  }

  if (removeVarBuffer_->number() && removeConBuffer_->number()) {
    master_->out() << "ABA_SUB::cutting(): WARNING: removing variables and constraints" << endl;
    master_->out() << "                         basis might become infeasible" << endl;
  }



  master_->out() << endl;
  master_->out() << "Update the Problem" << endl;

  // remove all buffered constraints   
  if (removeConBuffer_->number()) {
    nConRemoved = _removeCons(*removeConBuffer_);
    removeConBuffer_->clear();
    master_->out(1) << "removed constraints: " << nConRemoved << endl;
  }
  else
    nConRemoved = 0;

  // remove all buffered variables 
  if (removeVarBuffer_->number()) {
    nVarRemoved = _removeVars(*removeVarBuffer_);
    removeVarBuffer_->clear();
    master_->out(1) << "removed variables:   " << nVarRemoved << endl;
  }
  else
    nVarRemoved = 0;

// select constraints from the buffer and add them 
/* The function \a _selectCons() tries to select the best constraints
 *   of the buffered ones if more constraints have been generated than should be
 *   added.
 */  
  if (addConBuffer_->number()) {
      ABA_BUFFER<ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE>*> newCons(master_,
                                                      addConBuffer_->number());
    
    _selectCons(newCons);
    nConAdded = addCons(newCons);
    lastIterConAdd_ = nIter_;
    master_->out(1) << "added constraints:   " << nConAdded << endl;
  }
  else
    nConAdded = 0;

// select variables from the buffer and add them
/* The function \a _selectVars() tries to select the best variables
 *   of the buffered ones if more variables have been generated than should be
 *   added.
 */
      
  if (addVarBuffer_->number()) {
// check if there are non-liftable constraints 
/* If variables are added but non-liftable constraints are present, then
 *   we cannot generate the columns correctly.
 */  
#ifdef ABACUSSAFE
  const int nConstraints = nCon();

  for (int i = 0; i < nConstraints; i++)
    if (!constraint(i)->liftable()) {
      master_->err() << "ABA_SUB::cutting(): adding variables, where constraint ";
      master_->err() << i << " cannot be lifted" << endl;
    }
#endif    

    ABA_BUFFER<ABA_POOLSLOT<ABA_VARIABLE, ABA_CONSTRAINT> *> newVars(master_,  
                                                      addVarBuffer_->number());

    _selectVars(newVars);
    nVarAdded = addVars(newVars);
    lastIterVarAdd_ = nIter_;
    master_->out(1) << "added variables:     " << nVarAdded << endl;
  }
  else
    nVarAdded = 0;

  
  // decide whether to use an approximate solver or not
  if (master_->solveApprox() && solveApproxNow() && !forceExactSolver_)
     lpMethod_ = ABA_LP::Approximate;
  else
     lpMethod_ = chooseLpMethod(nVarRemoved, nConRemoved, nVarAdded, nConAdded);


// is this the last iteration before \a Branching? 
/* If we entered the cutting plane algorithm only to remove constraints
 *   before the branching is performed, we do not solve the LP-relaxation.
 */  
  if (lastIteration) return Branching;

// solve the LP-relaxation
/* If the function \a solveLp() returns 1, then the linear program is infeasible,
 *   also in respect to possibly inactive variables. If it returns 2, then
 *   the linear program is infeasible, but inactive variables have been
 *   generated. Therefore we iterate.

 *   If the function \a _pricing() returns a nonzero \a status, then the variables
 *   have been added. Hence, we iterate. Otherwise, the LP-solution is a dual
 *   bound for the subproblem and we can check the guarantee requirements.
 */  
  ++nIter_;

  status = solveLp();
  if (status == 1) return Fathoming;
  if (status == 2) continue;

// print the LP-solution for output level \a Subproblem and \a LinearProgram
/* For output level \a Subproblem we print a line only for the first solved
 *   linear program of the root node. For output level \a LinearProgram a line
 *   is output in each iteration.
 */  
  if (master_->outLevel() == ABA_MASTER::LinearProgram ||
      master_->logLevel() == ABA_MASTER::LinearProgram ||
      (this == master_->root() && nIter_ == 1
       && (master_->outLevel() == ABA_MASTER::Subproblem ||
           master_->logLevel() == ABA_MASTER::Subproblem   ))) {

  // ABA_SUB::cutting(): turn the streams on 
  bool turnOutOn = false;

  if (master_->outLevel() == ABA_MASTER::LinearProgram) 
    master_->out().on();
  else if (master_->out().isOn()) {
    master_->out().off();
    turnOutOn = true;
  }

  bool turnLogOn = false;
    
  if (master_->logLevel() == ABA_MASTER::LinearProgram) 
    master_->out().logOn();
  else if (master_->out().isLogOn()) {
    master_->out().logOff();
    turnLogOn = true;
  }

  // output a line about the linear program 
  master_->out() << setWidth(7) << master_->nSub() << " ";
  master_->out() << setWidth(7) << master_->openSub()->number() << "  ";
  master_->out() << setWidth(8) << id_ << " ";
  master_->out() << setWidth(7) << nIter_ << " ";
  master_->out() << setWidth(10) << lp_->value() << " ";
  master_->out() << setWidth(10) << master_->dualBound() << " ";
  if (master_->feasibleFound())
    master_->out() << setWidth(10) << master_->primalBound();
  else
    master_->out() << setWidth(10) << "---";
  master_->out() << endl;

  //! ABA_SUB::cutting(): turn the streams off 
  if (master_->outLevel() == ABA_MASTER::LinearProgram) 
    master_->out().off();
  if (master_->logLevel() == ABA_MASTER::LinearProgram) 
    master_->out().logOff();

  if (turnLogOn)
    master_->out().logOn();

  if (turnOutOn)
    master_->out().on();

  }


  if (master_->primalViolated(dualRound(lp_->value()))) {
    status = _pricing(newValues);
    if (status)       continue;
    // if the last LP was solved approximate, switch to the exact solver 
    // and iterate
    if( lastLP_ == ABA_LP::Approximate ) {
       forceExactSolver_ = true;
       lpMethod_ = ABA_LP::Dual;
       continue;
    }
    return Fathoming;
  }

  // count the number of discrete variables being fractional 
  int nFractional = 0;
  int nDiscrete   = 0;
  double frac;

  const int nVariables = nVar();
  
  for (int i = 0; i < nVariables; i++)
    if (variable(i)->discrete()) {
      ++nDiscrete;
      frac = fracPart(xVal_[i]);
      if ((frac > master_->eps()) && (frac < 1.0 - master_->machineEps()))
        ++nFractional;
    }

  master_->out()  << endl;
  master_->out(1) << nFractional << " of " << nDiscrete << " discrete variables ";
  master_->out()  << "are fractional" << endl;


// make a feasibility test
/* The function \a betterPrimal() might return \a false although we have
 *   a better feasible solution, because the primal bound might have been
 *   updated already in the function \a feasible(). This is an optional
 *   feature for the user of the framework in order to simplify the bookkeeping
 *   according to his needs. If no variables are added by the function
 *   \a _pricing(), then the LP solution is also dual feasible and we can
 *   fathom the subproblem, otherwise we continue the cutting plane algorithm.
 */  
  if (feasible()) {
    master_->out() << "LP-solution is feasible" << endl;
    if (master_->betterPrimal(lp_->value()))
      master_->primalBound(lp_->value());
     
    status = _pricing(newValues);
    if (status)     continue;
    return Fathoming;
  }

// improve the primal solution 
/* Even if the function \a _improve() returns a nonzero value indicating
 *   that a better solution has been found, we check if it is better than
 *   the current primal bound, as the primal bound might have been
 *   already updated during the application of the primal heuristics.
 *   Like in the function \a feasible() this is an optional feature to simplify
 *   the bookkeeping of the user.

 *   If we have found a better solution we reset the tailing off control
 *   because the subproblem seems to be promising.

 *   It is not unusual that inactive variables are added during the
 *   application of primal heuristics. In this case we go immediately to
 *   the beginning of the cutting plane loop without separating variables
 *   are constraints.
 */  
  double primalValue;  //!< value of a feasible solution found by primal heuristics
  
  status = _improve(primalValue);

  if (status && master_->betterPrimal(primalValue))
    master_->primalBound(primalValue);

  if (status) {
    tailOff_->reset();
    if (master_->primalViolated(dualRound(lp_->value()))) {
      status = _pricing(newValues);
      if (status)       continue;
      return Fathoming;
    }
  }
  
  if (addVarBuffer_->number()) continue;


// test some minor termination criteria 
/* Note, if \a pausing() returns \a true, then we enter the
 *   \a Branching phase but there no subproblems are generated.
 */  
  bool terminate = false;  //!< becomes \a true if one of the criteria is satisfied
  bool forceFathom = false;  //!< becomes \a true if fathoming should be forced

// check if problem specific fathoming criteria is satisfied 
/* The default implementation of \a exceptionFathom() returns always \a false.
 */
  if (exceptionFathom()) {
    master_->out() << "exceptionFathom(): try fathoming.";
    terminate = true;
    forceFathom = true;
    master_->status(ABA_MASTER::ExceptionFathom);
  }

// check if problem specific branching criteria is satisfied
/* The default implementation of \a exceptionBranch() returns always \a false.
 */
  if (exceptionBranch()) {
    master_->out() << "exceptionBranch(): try branching." << endl;
    terminate = true;
  }

  // check if maximal CPU time is exceeded 
  if (!terminate &&
      master_->totalTime_.exceeds(master_->maxCpuTime())) {
    master_->out() << "Maximal CPU time ";
    master_->out() << master_->maxCpuTime() << " exceeded" << endl;
    master_->out() << "Stop subproblem optimization." << endl;
    master_->status(ABA_MASTER::MaxCpuTime);
    terminate = true;
    forceFathom = true;
  }

  // check if maximal elapsed time is exceeded 
  if (!terminate &&
      master_->totalCowTime_.exceeds(master_->maxCowTime())) {
    master_->out() << "Maximal elapsed time ";
    master_->out() << master_->maxCowTime() << " exceeded" << endl;
    master_->out() << "Stop subproblem optimization." << endl;
    master_->status(ABA_MASTER::MaxCowTime);
    terminate = true;
    forceFathom = true;
  }

  // check if there is a tailing-off effect 
  if (tailOff_->tailOff()) {
    master_->out() << "Try to tail off subproblem processing" << endl;
    terminate = tailingOff();
    if (!terminate) {
      master_->out() << "problem specific resolution: ";
      master_->out() << "no branching enforced" << endl;
      tailOff_->reset();
    }
  }

  // should we pause the subproblem 
  if (!terminate && pausing()) {
    master_->out() << "Try to pause subproblem" << endl;
    terminate = true;
  }

  // check if the iteration limit is reached 
  if (!terminate && (maxIterations_ > 0) && (nIter_ >= maxIterations_)) {
    master_->out() << "Iteration limit reached in subproblem: enforce branching" << endl;
    terminate = true;
  }

// price out inactive variables if a termination criterion is fulfilled
/* The guarantee and the time limit criteria cause a fathoming of the
 *   subproblem, whereas the other criteria cause a branching.
 *   In the function \a prepareBranching() the active constraints and
 *   variables can  still be modified. In this case the modifications
 *   takes place at the beginning of the cutting plane algorithm. But nevertheless,
 *   after the modifications the linear program is not solved.
 */  
  if (terminate) {
     // if there has been tailing off while solving approximate
     // switch to the exact solver and iterate
     if (lastLP_ == ABA_LP::Approximate) {
	forceExactSolver_ = true;
	continue;
     }
    status = _pricing(newValues);
    if (status)       continue;
    if (guaranteed() || forceFathom) return Fathoming;
    if (newValues)    continue;
    status = prepareBranching(lastIteration);
    if (status) continue;
    else        return Branching;
  }


// perform primal and dual separation 
// should we skip the separation in this subproblem 
  if (master_->skippingMode() == ABA_MASTER::SkipByNode) {
    if ((master_->nSubSelected() - 1) % master_->skipFactor() != 0)
      return Branching;
  }
  else {
    if ((level_ - 1) % master_->skipFactor() != 0)
      return Branching;
  }

  if (primalSeparation()) {
// perform primal separation 
/* We do not check the return status for a successful separation, but check
 *   if new constraints have been stored in the buffer \a cutBuffer_ because
 *   violated inequalities might have been generated already earlier.

 *   We eliminate constraints only if also constraints are generated, because
 *   we prefer to have the same constraint set if variables are generated
 *   by \a _pricing(). If no variables are added in \a _pricing(), then
 *   the function
 *   \a prepareBranching() performs the elimination of the constraints.
 */  
  status = _separate();
       
  if (addConBuffer_->number()) _conEliminate();
  else {
    status = _pricing(newValues);
    if (status) continue;
    else {
      if (newValues)    continue;
      if (guaranteed()) return Fathoming;
      status = prepareBranching(lastIteration);
      if (status) continue;
      else        return Branching;
    }
  }

  }
      else {  //!< dual separation
	// perform dual separation    
	/* Like in the previous section for the separation we check also the buffer
	 *   for new generated variables. 
	 */  
	status = _pricing(newValues);
       
	if (addVarBuffer_->number()) _varEliminate();
	else if (guaranteed()) return Fathoming;
	else if (newValues)    continue;
	else {
	  if (_separate()) continue;
	  else {
	    status = prepareBranching(lastIteration);
	    if (status) continue;
	    else        return Branching;
	  }
	}

      }

      }   //!< while
     }

  int ABA_SUB::prepareBranching(bool &lastIteration)
  {
    lastIteration = true;
    int nElim = _conEliminate();

    if (nElim) {
      lpMethod_ = ABA_LP::Primal;
      return 1;
    }
    else return 0;
  }
 
  int ABA_SUB::solveLp ()
  {
// output some infos on the linear program
/* The ``true'' number of nonzeros is the number of nonzeros not including
 *   the coefficients of the eliminated variables.
 */  
  master_->out() << endl;
  master_->out() << "Solving LP " << nIter_ << endl;
  master_->out(1) << "Number of Constraints:  " <<  nCon() << endl;
  master_->out(1) << "Number of Variables  :  " <<  nVar() << "   (not eliminated ";
  master_->out()  << lp_->trueNCol() << ")" << endl;
  master_->out(1) << "True nonzeros        :  " << lp_->trueNnz() << endl;

  // optimize the linear program 
  ABA_LP::OPTSTAT status;

  master_->countLp();

  localTimer_.start(true);

  status = lp_->optimize(lpMethod_);
  lastLP_ = lpMethod_;
  
  master_->lpSolverTime_.addCentiSeconds( lp_->lpSolverTime_.centiSeconds() );
  lp_->lpSolverTime_.reset();

  master_->lpTime_.addCentiSeconds(localTimer_.centiSeconds());

  if (master_->printLP())
    master_->out() << *lp_;

#ifdef TTT1
  char fileName[256];
  sprintf(fileName, "%s.%d.bas", master_->problemName()->string(), 
          master_->nLp());

  if(lp_->writeBasisMatrix(fileName)) {
    master_->err() << "Writing basis to file " << fileName << " failed." << endl;
  }
#endif

  // buffer the solution of the linear program 
  if (lp_->xValStatus() != ABA_LP::Available) {
    if (!lp_->infeasible()) {
      const char *_error="ABA_SUB::solveLp(): no LP-solution available.";
      THROW_PARAM(AlgorithmFailureException,afcNoSolution,_error);
    }
  }
  else {
    const int nVariables = nVar();
    for (int i = 0; i < nVariables; i++)
      xVal_[i] = lp_->xVal(i);
  }

// buffer the dual variables of the linear program 
/* If there are constraints but the dual variables are missing we
 *   stop for safety.
 */  
  if ((lp_->yValStatus() != ABA_LP::Available) && nCon()) {
    const char *_error="ABA_SUB::solveLp(): no dual variables available.";
     THROW_PARAM(AlgorithmFailureException,afcNoSolution,_error);
  }
  else {
    const int nConstraints = nCon();
    for (int i = 0; i < nConstraints; i++)
      yVal_[i] = lp_->yVal(i);
  }

  // analyze the result of the linear program 
  if (status == ABA_LP::Optimal) {
    
// output the solution and get the basis 
/* The flag \a ignoreInTailingOff_ might have been set by the function
 *   \a ingnoreInTailingOff() such that the current LP solution is not
 *   considered in the tailing off analysis.
 */
  master_->out() <<::endl;
  master_->out(1) << "LP-solution            : " << lp_->value() <<::endl;
  master_->out(1) << "Best feasible solution : " << master_->primalBound()  << endl;

  if (ignoreInTailingOff_)
    ignoreInTailingOff_ = false;
  else 
    tailOff_->update(lp_->value());

  getBase();    

  return 0;

  }
  else  if (status == ABA_LP::Infeasible) {
// try to add variables to make the linear program feasible
/* The function \a infeasibleSub() sets the dual bound correctly (plus
 *   or minus infinity) for an infeasible subproblem.
 */ 
  if (!master_->pricing()) {
    infeasibleSub();
    return 1;
  }
  if (!removeNonLiftableCons()) return 2;
  getBase();
  if (_makeFeasible ()) {
    infeasibleSub();
    return 1;
  }
  else return 2;

  }
  else {
    // stop, a severe error occurred during the solution of the LP 
  	char *_error= new char[255];
	sprintf(_error,"ABA_SUB::solveLp() return status of ABA_LP::optimize() is\n %i (do not know how to proceed)",status);
 	THROW_PARAM(AlgorithmFailureException,afcNoSolution,_error);
	return 0;
  }

  }

  bool ABA_SUB::exceptionFathom()
  {
    return false;
  }

  bool ABA_SUB::exceptionBranch()
  {
    return false;
  }

  bool ABA_SUB::fixAndSetTime()
  {
    return true;
  }

  int ABA_SUB::_makeFeasible()
  {
    if (!master_->pricing()) return 1;

    master_->out() << "ABA_SUB::_makeFeasible()" << endl;
    
// make the current basis global dual feasible 
/* If the variables are added we return and solve the linear program again
 *   since these variables might restore already the feasibility and we
 *   can continue this function only if the basis is dual feasible.

 *   The second argument of \a _pricing() is \a false, because no variables
 *   should be fixed and set (it is useless for an infeasible subproblem).
 */
  bool newValues;  //!< only a dummy here
  
  int status = _pricing(newValues, false);
  if (status == 1)
    return 0;
  else if (status == 2) {
    const char *_error="ABA_SUB::_makeFeasible(): pricing failed due to\nnon-liftable constraints";
    THROW_PARAM(AlgorithmFailureException,afcMakeFeasible,_error);
  }

// can we fathom the subproblem?
/* If the basis is dual feasible, then the associated objective function
 *   value is a dual bound for the optimization of the subproblem. Hence
 *   we can fathom the node if the primal bound is violated.
 */
  if (master_->primalViolated(dualRound(lp_->value()))) return 1;

// emulate an iteration of the dual simplex method
/* An inactive variable is only a candidate if it is not implicitly set 0, i.e.,
 *   let \f$l\f$ be the objective function value of the linear program, \f$p\f$ be
 *   the value of the best known primal feasible solution, and \f$r_e\f$ the
 *   reduced cost of the variable \f$e\f$. If our problem is a maximization problem
 *   \f$e\f$ is only added if \f$l + r_e >= p\f$ holds, or if it is minimization problem
 *   \f$l + r_e <= p\f$ holds.
 *
 *   Let \f$B\f$ be the basis matrix corresponding to the dual feasible LP solution,
 *   at which the primal infeasibility was detected. For each candidate variable
 *   \f$e\f$ let \f$a_e\f$ be the column of the constraint matrix corresponding to \f$e\f$
 *   and solve the system \f$B z = a_e\f$. Let \f$z_b\f$ be the component of \f$z\f$
 *   corresponding to basis variable \f$b\f$. Activating \f$e\f$ ``reduces some
 *   infeasibility'' if one of the following holds, where \f$l_b\f$ and \f$u_b\f$
 *   are the local lower and upper bounds of variable \f$b\f$.
 *
 *   \shortitem{--} \f$b\f$ is a structural variable (i.e., not a slack variable) and
 *   \f[ x_b < l_b \hbox{\rm\ and } z_b < 0\f]
 *   or
 *   \f[ x_b > u_b \hbox{\rm\ and } z_b > 0\f]
 *
 *   \shortitem{--} \f$b\f$ is a slack variable and
 *   \f[ x_b < 0 \hbox{\rm\ and } z_b < 0.\f]
 *
 *   We refer the reader to \ref{Pad95} for an excellent description of the
 *   dual simplex method.
 */  
  bInvRow_ = new double[nCon()];

  status = lp_->getInfeas(infeasCon_, infeasVar_, bInvRow_);

  if (status) {
    const char *_error="ABA_SUB::_makeFeasible(): lp_->getInfeas() failed";
    THROW_PARAM(AlgorithmFailureException,afcMakeFeasible,_error);
  }

  status = makeFeasible();

  delete bInvRow_;
  bInvRow_ = 0;

  if (status) return 1;
  else        return 0;

  }

  int ABA_SUB::makeFeasible()
  {
    return 1;
  }

  bool ABA_SUB::goodCol(ABA_COLUMN &col, 
                        ABA_ARRAY<double> &row,
                        double x, 
                        double lb, 
                        double ub)
  {
    double p = 0.0;

    const int nnz = col.nnz();

    for (int i = 0; i < nnz; i++)
      p += col.coeff(i) * row[col.support(i)];

    if (x < lb) {
      if (p < -master_->eps()) return true;
      else                  return false;
    }
    else if (x > ub) {
      if (p > master_->eps()) return true;
      else                 return false;
    }
    else {
      const char *_error="ABA_SUB::goodCol(): variable is feasible!?";
      THROW_PARAM(AlgorithmFailureException,afcUnknown,_error);
      return false;  //!< to suppress warning of gcc
    }
  }

  int ABA_SUB::pricing()
  {
    return 0;
  }

  int ABA_SUB::_pricing(bool &newValues, bool doFixSet)
  {
    int nNew = 0;

    newValues = false;

    if (master_->pricing()) {

      if (!removeNonLiftableCons()) return 2;
    
      master_->out() << endl;
      master_->out() << "Price out Inactive Variables" << endl;

      localTimer_.start(true);
      nNew = pricing();
      master_->pricingTime_.addCentiSeconds(localTimer_.centiSeconds());
    }

    if (nNew) {
      if(doFixSet && fixAndSetTime()) 
        fixing(newValues);  //!< only with old candidates
    } else {
      if (betterDual(lp_->value())) dualBound(dualRound(lp_->value()));
      if (doFixSet && fixAndSetTime()) {
        if (master_->primalViolated(dualBound()))
          fixing(newValues);
        else
          fixAndSet(newValues);
      }
    }
    
    if (nNew) return 1;
    else      return 0;
  }

  bool ABA_SUB::primalSeparation()
  {
    if (master_->cutting()) {
      if (master_->pricing()) {
        if (addConBuffer_->number()) return true;
        int pricingFrequency = master_->pricingFreq();

        if (pricingFrequency && nIter_ % pricingFrequency == 0)
          return false;
        else
          return true;
      }
      else return true;
    }
    else return false;
  }

  double ABA_SUB::dualRound(double x)
  {
    if (master_->objInteger()) {
      if (master_->optSense()->max()) return floor(x + master_->eps());
      else                         return ceil(x  - master_->eps());
    }
    else return x;
  }

  bool ABA_SUB::guaranteed()
  {
    double lb = lowerBound();

    if (fabs(lb) < master_->machineEps()){
      if (fabs(upperBound()) < master_->machineEps()) return true;
      else                                            return false;
    }
    
    if (guarantee() + master_->machineEps() < master_->requiredGuarantee()) {
      master_->out() << "Subproblem guarantee reached" << endl;
      master_->status(ABA_MASTER::Guaranteed);
      return true;
    }
    else
      return false;
  }

  double ABA_SUB::guarantee()
  {
    double lb = lowerBound();

    if (fabs(lb) < master_->machineEps()) {
      if (fabs(upperBound()) < master_->machineEps()) return 0.0;
      else {
        const char *_error="ABA_SUB::guarantee(): cannot compute guarantee\nwithh lower bound 0";
        THROW_PARAM(AlgorithmFailureException,afcGuarantee,_error);
      }
    }
    
    return fabs((upperBound() - lb)/lb * 100.0); 

  }

  bool ABA_SUB::ancestor(const ABA_SUB *sub) const
  {
    const ABA_SUB *current = sub;

    do {
      if (this == current) return true;
      if (current == master_->root()) break;
      current = current->father();
    } while (1);

    return false;
  }

  bool ABA_SUB::removeNonLiftableCons()
  {
    if (!genNonLiftCons_) return true;
    
    int nNonLiftable = 0;

    const int nConstraints = nCon();
    
    for (int i = 0; i < nConstraints; i++)
      if (!constraint(i)->liftable()) {
        removeCon(i);
        nNonLiftable++;
      }

    genNonLiftCons_ = false;

    if (nNonLiftable) {
      master_->out() << "Removing " << nNonLiftable << " non-liftable constraints" << endl;
      lpMethod_ = ABA_LP::Primal;
      return false;
    }
    return true;
      
  }

  ABA_LP::METHOD ABA_SUB::chooseLpMethod(int nVarRemoved, 
                                         int nConRemoved,
                                         int nVarAdded, 
                                         int nConAdded)
  {
    ABA_LP::METHOD lpMethod = ABA_LP::Primal;

    if (nIter_ == 0) {
      if (this == master_->root())
        //lpMethod = ABA_LP::BarrierAndCrossover;
        lpMethod = ABA_LP::Primal;
      else
        lpMethod = ABA_LP::Dual;
    }
      
    if (nConAdded)
      lpMethod = ABA_LP::Dual;
    else if (nConRemoved)
      lpMethod = ABA_LP::Primal;

    if (nVarAdded)
      lpMethod = ABA_LP::Primal;
    else if (nVarRemoved)
      lpMethod = ABA_LP::Dual;

    if (nConAdded && nVarAdded)
      //lpMethod = ABA_LP::BarrierAndCrossover;
      lpMethod = ABA_LP::Primal;

    return lpMethod;
  }

  void ABA_SUB::removeVars(ABA_BUFFER<int> &remove)
  {
    const int nRemove = remove.number();

    for (int i = 0; i < nRemove; i++)
      removeVarBuffer_->push(remove[i]);
  }

  void ABA_SUB::_selectVars(ABA_BUFFER<ABA_POOLSLOT<ABA_VARIABLE, ABA_CONSTRAINT>*> &newVars)
  {
    selectVars();
    addVarBuffer_->sort(master_->maxVarAdd());
    addVarBuffer_->extract(master_->maxVarAdd(), newVars);
  }

  void ABA_SUB::selectVars()
  { }

  void ABA_SUB::_selectCons(ABA_BUFFER<ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE> *> &newCons)
  {
    selectCons();
    addConBuffer_->sort(master_->maxConAdd());
    addConBuffer_->extract(master_->maxConAdd(), newCons);
  }

  void ABA_SUB::selectCons()
  { }

  int ABA_SUB::addCons(ABA_BUFFER<ABA_CONSTRAINT*> &constraints,
                       ABA_POOL<ABA_CONSTRAINT, ABA_VARIABLE> *pool,
                       ABA_BUFFER<bool> *keepInPool,
                       ABA_BUFFER<double> *rank)
  {
    int       status;
    int       nAdded = 0;
    ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE> *slot;
    bool      keepIt;
    const int nConstraints = constraints.number();

    int       lastInserted = nConstraints;

    if (pool == 0) pool = master_->cutPool();

    for (int i = 0; i < nConstraints; i++) {
      slot = pool->insert(constraints[i]);
      if (slot == 0) {
        lastInserted = i - 1;
        break;
      }
      else {
        if (keepInPool) 
          keepIt = (*keepInPool)[i];
        else
          keepIt = false;
        if (rank) 
          status = addConBuffer_->insert(slot, keepIt, (*rank)[i]);
        else
          status = addConBuffer_->insert(slot, keepIt);
        if (status) {
          if (!keepIt && slot->conVar()->deletable()) 
            slot->removeConVarFromPool();
        }
        else
          nAdded++;
      }
    }
  
  // delete the constraints that could be not inserted into the pool 
  if (lastInserted < nConstraints) {
    master_->out() << "ABA_SUB::addCons(): pool too small,";
    master_->out() << " deleting " << nConstraints - lastInserted;
    master_->out() << " constraints." << endl;

    for (int i = lastInserted + 1; i < nConstraints; i++)
      delete constraints[i];
  }


    return nAdded;
  }

  int ABA_SUB::addCons(
                 ABA_BUFFER<ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE>*> &newCons)
  {
  // ABA_SUB::addCons(): local variables 
  const int nNewCons = newCons.number();

  ABA_BUFFER<ABA_CONSTRAINT*> cons(master_, nNewCons);
  int i;

  // require the new constraints a reallocation? 
  if (nCon() + nNewCons >= maxCon()) {
    int newMax = ((maxCon() + nNewCons)*11)/10 + 1;
    conRealloc(newMax);
  }

  // get the constraints from the pool slots 
  for (i = 0; i < nNewCons; i++) {
    newCons[i]->conVar()->activate();
    cons.push((ABA_CONSTRAINT*) newCons[i]->conVar());
  }

  // compute the average distance of the added cuts 
  if (master_->showAverageCutDistance()) {
    double averageDistance;

    averageDistance = 0.0;

    for (i = 0; i < nNewCons; i++)
       averageDistance += cons[i]->distance(xVal_, actVar_);
    
    averageDistance /= nNewCons;

    master_->out(1) << "average distance of cuts: " << averageDistance << endl;
  }

  // add the constraints to the active constraints and the LP 
  for (i = 0; i < nNewCons; i++)
    (*slackStat_)[nCon() + i] = new ABA_SLACKSTAT(master_, ABA_SLACKSTAT::Unknown);
  actCon_->insert(newCons);

  localTimer_.start(true);
  lp_->addCons(cons);
  master_->lpTime_.addCentiSeconds(localTimer_.centiSeconds());

  master_->addCons(nNewCons);

  return nNewCons;

  }

  int ABA_SUB::addVars(ABA_BUFFER<ABA_VARIABLE*> &variables,
                       ABA_POOL<ABA_VARIABLE, ABA_CONSTRAINT> *pool,
                       ABA_BUFFER<bool> *keepInPool,
                       ABA_BUFFER<double> *rank)
  {
      
    int       status;
    int       nAdded = 0;
    ABA_POOLSLOT<ABA_VARIABLE, ABA_CONSTRAINT> *slot;
    bool      keepIt;
    const int nVariables = variables.number();
    int       lastInserted = nVariables;


    if (pool == 0) pool = master_->varPool();

    for (int i = 0; i < nVariables; i++) {
      slot = pool->insert(variables[i]);
      if (slot == 0) {
        lastInserted = i - 1;
        break;
      }
      else {
        if (keepInPool) 
          keepIt = (*keepInPool)[i];
        else
          keepIt = false;
        if (rank) 
          status = addVarBuffer_->insert(slot, keepIt, (*rank)[i]);
        else
          status = addVarBuffer_->insert(slot, keepIt);
        if (status) {
          if (!keepIt && slot->conVar()->deletable()) 
            slot->removeConVarFromPool();
        }
        else
          nAdded++;
      }
    }
  
  // delete the variables that could be not inserted into the pool 
  if (lastInserted < nVariables) {
    master_->out() << "ABA_SUB::addVars(): pool too small,";
    master_->out() << " deleting " << nVariables - lastInserted;
    master_->out() << " variables." << endl;

    for (int i = lastInserted + 1; i < nVariables; i++)
      delete variables[i];
  }


    return nAdded;
  }

  int ABA_SUB::addVars(
                 ABA_BUFFER<ABA_POOLSLOT<ABA_VARIABLE, ABA_CONSTRAINT> *> &newVars)
  {
    activateVars(newVars);
    addVarsToLp(newVars);
    
    tailOff_->reset();

    return newVars.number();
  }

  int ABA_SUB::variablePoolSeparation(int ranking, 
                                      ABA_POOL<ABA_VARIABLE, ABA_CONSTRAINT> *pool,
                                      double minAbsViolation)
  {
    if (pool)
      return pool->separate(yVal_, actCon_, this, addVarBuffer_, 
                            minAbsViolation, ranking);
    else
      return master_->varPool()->separate(yVal_, actCon_, this, addVarBuffer_,
                                          minAbsViolation, ranking);
  }

  int ABA_SUB::constraintPoolSeparation(int ranking,
                                        ABA_POOL<ABA_CONSTRAINT, ABA_VARIABLE> *pool,
                                        double minViolation)
  {
    if (pool)
      return pool->separate(xVal_, actVar_, this, addConBuffer_,
                            minViolation, ranking);
    else
      return master_->cutPool()->separate(xVal_, actVar_, this, addConBuffer_,
                                          minViolation, ranking);
  }

  bool ABA_SUB::objAllInteger()
  {
    ABA_VARIABLE *v;
    double x;

    const int nVariables = nVar();

    for (int i = 0; i < nVariables; i++) {
      v = variable(i);
      if (v->discrete()) {
        x = v->obj();
        if (x - floor(x) > master_->machineEps()) {
          return false;
        }
      }
      else
        return false;
    }
    
    master_->out() << "objective function values of feasible solutions are integer" << endl;

    return true;
  }

  bool ABA_SUB::integerFeasible()
  {
    double frac;

    const int nVariables = nVar();
    
    for (int i = 0; i < nVariables; i++) 
      if (variable(i)->discrete()) {
        frac = fracPart(xVal_[i]);
        if ((frac > master_->machineEps()) &&
            (frac < 1.0 - master_->machineEps()))
            return false;
      }

    return true;
  }

  void ABA_SUB::ignoreInTailingOff()
  {
    master_->out(1) << "next LP solution ignored in tailing off" << endl;
    ignoreInTailingOff_ = true;
  }

  ABA_SUB::PHASE ABA_SUB::branching()
  {
    master_->out() << endl << "Branching Phase" << endl << endl;

  // check if the maximum enumeration level is reached 
  if (level_ == master_->maxLevel()) {
    master_->out() << "Maximum enumeration level " << master_->maxLevel();
    master_->out() << " reached, no branching" << endl;
    master_->status(ABA_MASTER::MaxLevel);
    return Fathoming;
  }

// check if the subproblem becomes dormant without branching
/* Sometimes it turns out to be appropriate to stop the optimization
 *   of a specific subproblem without creating any sons but putting
 *   the node back into the list of open subproblems. Per default no
 *   pausing is performed but the virtual function \a pausing() can
 *   be redefined in derived classes.

 *   Then we check the parameter if only after processing a node
 *   several times its sons should be generated \a (delayedBranching(nOpt_).
 *   This idea
 *   is motivated by the pool separation. When such a dormant
 *   node is awaked in the meantime pool constraints might have
 *   become available which are violated by the last \a LP-solution.

 *   A subproblem can be only inserted in the set of open subproblems without
 *   branching if there are other subproblems for further processing.

 *   The statuses of the variables (\a fsVarStat, \a lpVarStat) are
 *   not deleted, when a subproblem becomes \a Dormant.
 */
  if (pausing() || master_->delayedBranching(nOpt_))
    if (!master_->openSub()->empty()) {
      master_->out() << "making node dormant" << endl;
      master_->openSub()->insert(this);
      status_ = Dormant;
      nDormantRounds_ = 0;
      return Done;
    }

// generate the branching rules 
/* If no branching rule is found we can fathom the subproblem.
 *   A branch rule defines the modifications of the current subproblem for
 *   a new subproblem. 
 */
  ABA_BUFFER<ABA_BRANCHRULE*> rules(master_, nVar());

  localTimer_.start(true);
  int status = generateBranchRules(rules);
  master_->branchingTime_.addCentiSeconds( localTimer_.centiSeconds() );

  if (status)
    return Fathoming;

// generate the sons 
/* For each branch rule a new subproblem is generated.
 */

  const int nRules = rules.number();

  master_->out() << "Number of new problems : " << nRules << endl;

  sons_ = new ABA_BUFFER<ABA_SUB*>(master_, nRules);

  ABA_SUB *newSub;

  for (int i = 0; i < nRules; i++) {
    newSub = generateSon(rules[i]);
    master_->openSub()->insert(newSub);
    sons_->push(newSub);
    master_->treeInterfaceNewNode(newSub);
  }


    status_ = Processed;

    return Done;
  }

  int ABA_SUB::generateBranchRules(ABA_BUFFER<ABA_BRANCHRULE*> &rules)
  {
    return branchingOnVariable(rules);
  }

  int ABA_SUB::branchingOnVariable(ABA_BUFFER<ABA_BRANCHRULE*> &rules)
  {
    // select the branching variable 
    int branchVar;
    
    int status = selectBranchingVariable(branchVar);

    if (status) {
      master_->out() << "no branching variable found" << endl;
      return 1;
    }
    
    master_->out() << endl;
    if (variable(branchVar)->binary()) master_->out() << "Binary ";
    else                               master_->out() << "Integer ";
    
    master_->out() << "Branching Variable     : ";
    master_->out() << branchVar << " (value: " << xVal_[branchVar];
    master_->out() << ", cost: " << variable(branchVar)->obj() << ") ";
    master_->out () << endl;

// generate the two rules for the branching variable
/* A binary branching variable is set to 0 in one of the
 *   two subproblems, and set to 1 in the other subproblem. For an
 *   integer branching variable we have to modify its lower and upper bound.
 */  
  if (variable(branchVar)->binary()) {
    rules.push(new ABA_SETBRANCHRULE(master_, branchVar,
                                 ABA_FSVARSTAT::SetToUpperBound));
    rules.push(new ABA_SETBRANCHRULE(master_, branchVar,
                                 ABA_FSVARSTAT::SetToLowerBound));
  }
  else {
	double splitVal=floor(xVal_[branchVar] + master_->eps());
	if(splitVal >= uBound(branchVar)){
		splitVal = splitVal - 1;
	}

	// [splitVal+1,ubound]
	rules.push(new ABA_BOUNDBRANCHRULE(
			master_,
			branchVar,
            splitVal+1.0,
            uBound(branchVar)
	));

	// [lbound, splitVal]
	rules.push(new ABA_BOUNDBRANCHRULE(
			master_,
			branchVar,
			lBound(branchVar),
			splitVal
	));
  }
  return 0;
}

#ifdef TTT
extern "C" {
#include "cplex.h"
}
#endif
  int ABA_SUB::selectBranchingVariable(int &variable)
  {
  // select the candidates for branching variables 
  ABA_BUFFER<int> candidates(master_, master_->nBranchingVariableCandidates());

  int status = selectBranchingVariableCandidates(candidates);

  if (status) return 1;

  if (candidates.number() == 1) {
    variable = candidates[0];
    return 0;
  }

#ifdef TTT

// perform strong branching with help of Cplex internals  
/* The call of \a strongbranching() is only correct if no variables are
 *   eliminated. 
 */
  int *goodlist = new int[candidates.number()];
  double *down  = new double[candidates.number()];
  double *up    = new double[candidates.number()];

  master_->out(1) << candidates.number() << " candidates" << endl;

  for (int i = 0; i < candidates.number(); i++)
    goodlist[i] = candidates[i];
 
  status = strongbranch(((ABA_CPLEXIF*) lp_)->cplexLp(), goodlist, 
                        candidates.number(), down, up, 100000);

  if (status) {
    char *_error="strong branching failed";
    THROW_PARAM(AlgorithmFailureException,afcUnknown,_error);
  }

  for (int i = 0; i < candidates.number(); i++) 
    master_->out(1) << down[i] << ' ' << up[i] << endl;

  int bestCand = 0;
  double bestVal;
  double val;

  if (master_->optSense()->max()) {
    if (down[0] < up[0]) bestVal = up[0];
    else                 bestVal = down[0];
  }
  else {
    if (down[0] > up[0]) bestVal = up[0];
    else                 bestVal = down[0];
  }

  for (int i = 1; i < candidates.number(); i++) 
    if (master_->optSense()->max()) {
      if (down[i] < up[i]) val = up[i];
      else                 val = down[i];
      if (val < bestVal) {
        bestCand = i;
        bestVal  = val;
      }
    }
    else {
      if (down[i] > up[i]) val = up[i];
      else                 val = down[i];
      if (val > bestVal) {
        bestCand = i;
        bestVal  = val;
      }
    }

  delete [] goodlist;
  delete [] up;
  delete [] down;

  master_->out(1) << "selecting sample " << bestCand << endl;

  variable = candidates[bestCand];
  return 0;

#endif

  // generate the two branching rules for each candidate 
  const int nCandidates = candidates.number();

  ABA_BUFFER<ABA_BRANCHRULE*> **samples = new ABA_BUFFER<ABA_BRANCHRULE*>*[nCandidates];

  for (int i = 0; i < nCandidates; i++) {
    samples[i] = new ABA_BUFFER<ABA_BRANCHRULE*>(master_, 2);
    samples[i]->push(new ABA_SETBRANCHRULE(master_, candidates[i], 
                                       ABA_FSVARSTAT::SetToUpperBound));
    samples[i]->push(new ABA_SETBRANCHRULE(master_, candidates[i], 
                                       ABA_FSVARSTAT::SetToLowerBound));
  }

  // evaluate the candidates and select the best ones 
  int best = selectBestBranchingSample(nCandidates, samples);

  if (best == -1) {
    const char *_error="ABA_SUB::selectBranchingVariable(): internal error,\nselectBestBranchingSample returned -1";
    THROW_PARAM(AlgorithmFailureException,afcBranchingVariable,_error);
  }

  variable = candidates[best];

  for (int i = 0; i < nCandidates; i++) {
    delete (*samples[i])[0];
    delete (*samples[i])[1];
    delete samples[i];
  }
  delete [] samples;

  return 0;

  }

  int ABA_SUB::selectBranchingVariableCandidates(ABA_BUFFER<int> &candidates)
  {
    int status=0;

    if (master_->branchingStrategy() == ABA_MASTER::CloseHalf) {
      status = closeHalf(candidates, ABA_VARTYPE::Binary);
      if (status) 
        status = closeHalf(candidates, ABA_VARTYPE::Integer);
      if (status)
        status = findNonFixedSet(candidates, ABA_VARTYPE::Binary);
      if (status)
        status = findNonFixedSet(candidates, ABA_VARTYPE::Integer);
    }
    else if (master_->branchingStrategy() == ABA_MASTER::CloseHalfExpensive) {
     status = closeHalfExpensive(candidates, ABA_VARTYPE::Binary);
     if (status) 
        status = closeHalfExpensive(candidates, ABA_VARTYPE::Integer);
      if (status)
        status = findNonFixedSet(candidates, ABA_VARTYPE::Binary);
      if (status)
        status = findNonFixedSet(candidates, ABA_VARTYPE::Integer);
    }
    else {
      const char *_error="ABA_SUB::selectBranchingVariable(): unknown strategy";
      THROW_PARAM(AlgorithmFailureException,afcStrategy,_error);
    }

    return status;
  }

  int ABA_SUB::closeHalf(int &branchVar, ABA_VARTYPE::TYPE branchVarType)
  {
    ABA_BUFFER<int> variables(master_, 1);

    int status = closeHalf(variables, branchVarType);

    if (status) 
      return 1;
    else {
      branchVar = variables[0];
      return 0;
    }

  }

  int ABA_SUB::closeHalf(ABA_BUFFER<int> &variables, ABA_VARTYPE::TYPE branchVarType)
  {
  // ABA_SUB::closeHalf(): check the branching variable type 
  if (branchVarType == ABA_VARTYPE::Continuous) {
    const char *_error="ABA_SUB::closeHalf(): we cannot branch on a\n continuous variable.";
    THROW_PARAM(AlgorithmFailureException,afcCloseHalf,_error);
  }

  // search fractional variables closest to \f$0.5\f$ 
  ABA_BPRIOQUEUE<int, double> closest(master_, variables.size());
  //changed uninit.value minkey to 0.
  double diff;
  double minKey=0.;
  int    min;

  const int nVariables = nVar();
  for (int i = 0; i < nVariables; i++)
    if ((variable(i)->varType() == branchVarType)
        && !(*fsVarStat_)[i]->fixedOrSet()
        && !( lBound(i) == uBound(i) )
    ) {
      diff = fabs(fracPart(xVal_[i]) - 0.5);
      if (diff < 0.5 - master_->machineEps()) {
        if (closest.number() < closest.size())
          closest.insert(i, -diff);
        else {
          (void) closest.getMinKey(minKey);
          if (diff < -minKey) {
            (void) closest.extractMin(min);
            closest.insert(i, -diff);
          }
        }
      }
    }

  // copy the best variables in the buffer \a variables 
  while(!closest.extractMin(min))
    variables.push(min);

  if (variables.number()) return 0;
  else                    return 1;

  }

  int ABA_SUB::closeHalfExpensive(int &branchVar, ABA_VARTYPE::TYPE branchVarType)
  {
    ABA_BUFFER<int> branchVarBuffer(master_, 1);

    int status = closeHalfExpensive(branchVarBuffer, branchVarType);

    if (!status) branchVar = branchVarBuffer[0];

    return status;
}

  int ABA_SUB::closeHalfExpensive(ABA_BUFFER<int> &branchVar, 
                                  ABA_VARTYPE::TYPE branchVarType)
  {
  // local variables (ABA_SUB::closeHalfExpensive()) 
  ABA_BPRIOQUEUE<int, double> candidates(master_, branchVar.size());
  int    i;              /* loop index */
  double fraction;       /* fraction of x-value of a variable */
  double eps         = master_->machineEps();
  double oneMinusEps = 1.0 - eps;

  // check the selected branching variable type 
  if (branchVarType == ABA_VARTYPE::Continuous) {
    const char *_error="ABA_SUB::closeHalfExpensive(): we cannot branch on a\ncontinuous variable.";
    THROW_PARAM(AlgorithmFailureException,afcCloseHalf,_error);
  }

// determine interval for fraction of candidates   
/* First we determine \a lower the maximal LP-value of a variable less than
 *   \f$0.5\f$ and \a upper the minimal LP-value of a variable greater than \f$0.5\f$.
 *   Finally, \a lower and \a upper are scaled. 
 */  
  double lower = eps;
  double upper = oneMinusEps;

  const int nVariables = nVar();

  for (i = 0; i < nVariables; i++)
    if ((variable(i)->varType() == branchVarType) 
        && !(*fsVarStat_)[i]->fixedOrSet()
        && !( lBound(i) == uBound(i) )
      ) {
      fraction = fracPart(xVal_[i]);

      if (fraction <= 0.5 && fraction > lower) lower = fraction;
      if (fraction >= 0.5 && fraction < upper) upper = fraction;
    }  

  if (lower == eps && upper == oneMinusEps) return 1;

  double scale = 0.25;

  lower   = (1.0 - scale) * lower;
  upper = upper + scale * (1.0-upper);

// select the most expensive variables from interval
/* Under \a cost in this context we understand the absolute value of the
 *   objective function coefficient.
 */  
  // changed uninit. Value of minCostCandidate to 0.!
  double cost;               //!< cost of current variable
  double minCostCandidate=0.;   //!< cost of worst variable in priority queue
  int    dummy;              //!< for extracting item of priority queue

  for (i = 0; i < nVariables; i++)
    if ((variable(i)->varType() == branchVarType)
        && !(*fsVarStat_)[i]->fixedOrSet()) {

// check if this variable might a candidate
/* We select the variable either if there are not enough candidates,
 *   otherwise, we check if its cost are higher than those of the worst
 *   element of \a candidate. In this case we replace this element with the
 *   variable \a i.
 */  
  fraction = fracPart(xVal_[i]);

  if (lower <= fraction && fraction <= upper) {
    cost     = fabs(variable(i)->obj());
    if (candidates.number() < candidates.size())
      candidates.insert(i, cost);
    else {
      if (candidates.getMinKey(minCostCandidate)) {
        const char *_error="ABA_SUB::CloseHalfExpensive(): internal error:\ncandidate priorirty queue is empty.";
        THROW_PARAM(AlgorithmFailureException,afcCloseHalf,_error);
      }
      if (cost > minCostCandidate) {
        (void) candidates.extractMin(dummy);
        candidates.insert(i, cost);
      }
    }
  }

  }

  // copy the ``best'' variables to \a branchVar 
  if (candidates.number() == 0) {
    const char *_error="ABA_SUB::closeHalfExpensive(): where is the fractional variable?";
    THROW_PARAM(AlgorithmFailureException,afcCloseHalf,_error);
  }

  while (!candidates.extractMin(dummy))
    branchVar.push(dummy);

  return 0;

  }

  int ABA_SUB::findNonFixedSet(int &branchVar, ABA_VARTYPE::TYPE branchVarType)
  {
    ABA_BUFFER<int> variables(master_, 1);

    int status = findNonFixedSet(variables, branchVarType);
    
    if (status)
      return 1;
    else {
      branchVar = variables[0];
      return 0;
    }
  }

  int ABA_SUB::findNonFixedSet(ABA_BUFFER<int> &branchVar, 
                               ABA_VARTYPE::TYPE branchVarType)
  {
  // ABA_SUB::findNonFixedSet: check the selected branching variable type 
  if (branchVarType == ABA_VARTYPE::Continuous) {
    const char *_error="ABA_SUB::findNonFixedSet(): we cannot branch on a\ncontinuous variable.";
    THROW_PARAM(AlgorithmFailureException,afcUnknown,_error);
  }


    const int nVariables = nVar();
    
    for (int i = 0; i < nVariables; i++)
      if ((variable(i)->varType() == branchVarType)
          && !(*fsVarStat_)[i]->fixedOrSet()
          && !( lBound(i) == uBound(i) )) {
        branchVar.push(i);
        if (branchVar.full()) return 0;
      }

    if (branchVar.number()) return 0;
    else                    return 1;
  }

  int ABA_SUB::selectBestBranchingSample(int nSamples,
                                         ABA_BUFFER<ABA_BRANCHRULE*> **samples)
  {
// allocate memory (ABA_SUB::selectBestBranchingSample()) 
  ABA_ARRAY<double> **rank = new ABA_ARRAY<double>*[nSamples];

  for (int i = 0; i < nSamples; i++)
    rank[i] = new ABA_ARRAY<double>(master_, samples[i]->number());

// compute the ranks and select the best sample 
  master_->out() << "Computing ranks of branching samples: "<< endl;
  int best = 0;

  for (int i = 0; i < nSamples; i++) {
    rankBranchingSample(*(samples[i]), *(rank[i]));
    master_->out(1) << "Sample " << i << ": ";
    for (int j = 0; j < samples[i]->number(); j++)
        master_->out() << (*(rank[i]))[j] << ' ';
    master_->out() << endl;
    if (i > 0 &&
        compareBranchingSampleRanks(*(rank[best]), *(rank[i])) == -1) 
      best = i;
  }

  master_->out() << endl;
  master_->out() << "Selecting branching sample " << best << "." << endl;

// delete memory (ABA_SUB::selectBestBranchingSample()) 
for (int i = 0; i < nSamples; i++)
  delete rank[i];
  delete [] rank;

  return best;

  }

  void ABA_SUB::rankBranchingSample(ABA_BUFFER<ABA_BRANCHRULE*> &sample,
                                    ABA_ARRAY<double> &rank) 
  {
    const int nSample = sample.number();

    for (int i = 0; i < nSample; i++)
      rank[i] = rankBranchingRule(sample[i]);
  }

  double ABA_SUB::rankBranchingRule(ABA_BRANCHRULE *branchRule)
  {
    return lpRankBranchingRule(branchRule, master_->nStrongBranchingIterations());
  }

  double ABA_SUB::lpRankBranchingRule(ABA_BRANCHRULE *branchRule, int iterLimit)
  {
  // add the branching rule and solve the linear program 
  // set the new iteration limit 
  int oldIterLimit;

  if (iterLimit >= 0) {
    if (lp_->getSimplexIterationLimit(oldIterLimit)) {
      master_->err() << "WARNING: ";
      master_->err() << "ABA_SUB::lpRankBranchingRule(): ";
      master_->err() << "getting the iteration limit of the LP-solver failed.";
      master_->err() << endl;
      oldIterLimit = -1;
    }
    else {
      if (lp_->setSimplexIterationLimit(iterLimit)) {
        master_->err() << "WARNING: ";
        master_->err() << "ABA_SUB::lpRankBranchingRule(): ";
        master_->err() << "setting the iteration limit of the LP-solver failed.";
        master_->err() << endl;
        oldIterLimit = -1;
      }
    }      
  }

  // load the final basis of the subproblem optimization 
  ABA_ARRAY<ABA_LPVARSTAT::STATUS> vStat(master_, nVar());
  ABA_ARRAY<ABA_SLACKSTAT::STATUS> sStat(master_, nCon());

  const int nVariables = nVar();

  for (int i = 0; i < nVariables; i++) 
    vStat[i] = lpVarStat(i)->status();

  const int nConstraints = nCon();

  for (int i = 0; i < nConstraints; i++)
    sStat[i] = slackStat(i)->status();

  lp_->loadBasis(vStat, sStat);


  branchRule->extract(lp_);
  localTimer_.start(true);
  lp_->optimize(ABA_LP::Dual);
  master_->lpTime_.addCentiSeconds(localTimer_.centiSeconds());

  // get the \a value of the linear program 
  double value;
  if (lp_->infeasible()) {
    if (master_->optSense()->max()) value = -master_->infinity();
    else                            value =  master_->infinity();
  }
  else 
    value = lp_->value();

  // remove the branching rule 
  // set the iteration limit back to its old value 
  if (iterLimit >= 0 && oldIterLimit >=0)
    if (lp_->setSimplexIterationLimit(oldIterLimit)) {
      const char *_error="ABA_SUB::lpRankBranchingRule(): setting the iteration limit of LP-solver failed";
      THROW_PARAM(AlgorithmFailureException,afcBranchingRule,_error);
    }

  branchRule->unExtract(lp_);

    return value;
  }

  int ABA_SUB::compareBranchingSampleRanks(ABA_ARRAY<double> &rank1,
                                           ABA_ARRAY<double> &rank2)
  {
  // build up a priority queue for each rank 
  ABA_BPRIOQUEUE<int, double> prio1(master_, rank1.size());
  ABA_BPRIOQUEUE<int, double> prio2(master_, rank2.size());

  const int s1 = rank1.size();
  const int s2 = rank2.size();
  
  if (master_->optSense()->max()) {
    for (int i = 0; i < s1; i++) 
      prio1.insert(i, -rank1[i]);

    for (int i = 0; i < s2; i++) 
      prio2.insert(i,-rank2[i]);
  }
  else {
    for (int i = 0; i < s1; i++) 
      prio1.insert(i, rank1[i]);
    for (int i = 0; i < s2; i++) 
      prio2.insert(i, rank2[i]);
  }

  // prefer the rank where the minimal change is maximal 
  double minKey1;
  double minKey2;

  int min1;
  int min2;

  while(!prio1.getMinKey(minKey1) && !prio2.getMinKey(minKey2)) {
    if (!master_->equal(minKey1, minKey2)) {
      if (minKey1 > minKey2) return  1;
      else                   return -1;
    }
    else {
      (void) prio1.extractMin(min1);
      (void) prio2.extractMin(min2);
    }
  }
  
  return 0;

  }

  ABA_SUB::PHASE ABA_SUB::fathoming()
  {
    master_->out() << endl;
    master_->out() << "Fathoming Phase" << endl;
    
    fathom(true);

    return Done;
  }

  void ABA_SUB::fathom(bool reoptimize)
  {
    // ABA_SUB::fathom(): output some infos 
  master_->out(1) << "node " << id_ << " fathomed" << endl;

// reset the flags of the active variables and constraints   
/* If an active subproblem is fathomed, then the active variables
 *   and constraints have to be deactivated. This can only be done
 *   if these sets are allocated already.
 *
 *   Then, we can set the status of the subproblem to \a Fathomed.
 */
  if (status_ == Active) {
    if (actVar_) {
      int nActVar = actVar_->number();
      for (int i = 0; i < nActVar; i++) (*actVar_)[i]->deactivate();
    }

    if (actCon_) {
      const int nActCon = actCon_->number();
      for (int i = 0; i < nActCon; i++) (*actCon_)[i]->deactivate();
    }
  }

  status_ = Fathomed;

// update the dual bound of the subproblem
/* If the subproblem is not a leaf of the enumeration tree, we
 *   update its dual bound with the minimal (maximal) dual bound
 *   of its sons if the optimization problem is a minimization
 *   (maximization) problem. 

 *   We update the dual bound only if the sons define a better dual
 *   bound. If heuristic separation methods are applied or a
 *   reoptimization of a subproblem has been performed, then it is
 *   possible that the dual bound defined by the sons is worse than the
 *   dual bound of the subproblem.
 */
   if (sons_) {
     double newDualBound = (*sons_)[0]->dualBound();
   
     if (master_->optSense()->max()) {
       const int nSons = sons_->number();
       for (int i = 1; i < nSons; i++)
         if (newDualBound < (*sons_)[i]->dualBound())
           newDualBound = (*sons_)[i]->dualBound();
     }
     else {
       const int nSons = sons_->number();
       for (int i = 1; i < nSons; i++)
         if (newDualBound > (*sons_)[i]->dualBound())
           newDualBound = (*sons_)[i]->dualBound();
     }
     if (betterDual(newDualBound))
       dualBound(newDualBound);
   }

// delete allocated memory of the fathomed subproblem    
/* A fathomed subproblem will neither be required for the initialization of
 *   one of its sons nor in a reoptimization to determine the new candidates
 *   for fixing variables. Hence we can delete all local memory. 
 */

  if (fsVarStat_) {
    const int nVariables = nVar();
    for (int i = 0; i < nVariables; i++)
      delete (*fsVarStat_)[i];
  }

  if (lpVarStat_) {
    const int nVariables = nVar();
    for (int i = 0; i < nVariables; i++)
      delete (*lpVarStat_)[i];
  }

  delete fsVarStat_;
  fsVarStat_ = 0;
  delete lpVarStat_;
  lpVarStat_ = 0;
    
  delete lBound_;
  lBound_ = 0;
  delete uBound_;
  uBound_ = 0;

  if (slackStat_) {
    const int nConstraints = nCon();
    for (int i = 0; i < nConstraints; i++)
      delete (*slackStat_)[i];
    delete slackStat_;
    slackStat_ = 0;
  }
    
  delete actCon_;
  actCon_ = 0;
  delete actVar_;
  actVar_ = 0;

  delete branchRule_;
  branchRule_ = 0;

  // check if the root node is fathomed 
  if (this == master_->root()) {
    master_->out(2) << "root node fathomed" << endl;
    return;
  }

  // count the number of unfathomed sons of the father 
  int   nuf = 0;            //!< number of unfathomed sons of \a father_

  for (int i = 0; i < father_->sons_->number(); i++)
    if ((*(father_->sons_))[i]->status_ != Fathomed) ++nuf;

// process the father  
/* If all sons of the father are fathomed we can fathom the father
 *   recursively. If only one son of the father is unfathomed and the father
 *   is the root of the remaining \bac\ tree, then this unfathomed son
 *   becomes the new root of the remaining \bac\ tree.
 *   As in this case, there is exactly one unfathomed son, this
 *   is son \a i when the \a for-loop is left by the \a break instruction.
 */
    if (nuf==0) 
      father_->fathom(reoptimize);
    else if (nuf == 1 && father_ == master_->rRoot()) {
      int i;
      for (i = 0; i < father_->sons_->number(); i++)
        if ((*(father_->sons_))[i]->status_ != Fathomed) break;

      master_->rRoot((*(father_->sons_))[i], reoptimize);
    }

  }

  int ABA_SUB::fixAndSet(bool &newValues)
  {
    int  status;
    bool lNewValues;

    newValues = false;

    status = fixing(lNewValues, true);
    if (lNewValues) newValues = true;
    if (status)     return 1;
    
    status = setting(lNewValues);
    if (lNewValues) newValues = true;
    if (status)     return 1;

    return 0;
  }

  int ABA_SUB::fixing(bool &newValues, bool saveCand)
  {
    int  status;
    bool lNewValues = false;

    newValues = false;

    status = fixByRedCost(lNewValues, saveCand);
    if (lNewValues) newValues = true;
    if (status)     return 1;
    
    status = _fixByLogImp(lNewValues);
    if (lNewValues) newValues = true;
    if (status)     return 1;
    
    return 0;
  }

  int ABA_SUB::setting(bool &newValues)
  {
    int  status;
    bool lNewValues;

    newValues = false;

    status = setByRedCost();
    if (status) return 1;
    
    status = _setByLogImp(lNewValues);
    if (lNewValues) newValues = true;
    if (status)     return 1;
    
    return 0;
  }

  int ABA_SUB::fixByRedCost(bool &newValues, bool saveCand)
  {
    if (!master_->fixSetByRedCost()) return 0;
    
    newValues = false;

    if (this == master_->rRoot() && saveCand) 
      master_->fixCand()->saveCandidates(this);

    master_->fixCand()->fixByRedCost(addVarBuffer_);

// update the global fixings also in the subproblem 
/* If a contradiction between a global fixing a the local status of the
 *   variable is detected we immediately stop such that the subproblem can
 *   be fathomed.
 */  
  ABA_FSVARSTAT *global;
  bool       lNewValues;

  const int nVariables = nVar();  

  for (int i = 0; i < nVariables; i++) {
    global = variable(i)->fsVarStat();
    if (global->fixed() && global->status() != (*fsVarStat_)[i]->status()) {
      if (fix(i, global, lNewValues)) return 1;
      if (lNewValues) newValues = true;
    }
  }
  return 0;

    
  }

  int ABA_SUB::_fixByLogImp(bool &newValues)
  {
    master_->out() << "Fixing Variables by Logical Implications:  " << flush;
  // call the virtual function to fix variables by logical implications 
  ABA_BUFFER<int>        variables(master_, nVar());
  ABA_BUFFER<ABA_FSVARSTAT*> status(master_, nVar());

  fixByLogImp(variables, status);

  // check if \a fixByLogImp() caused contradictions or fixed variables to new values 
  int contra = 0;
  int stat;
  bool lNewValues;

  newValues = false;

  const int nVariables = variables.number();
    
  for (int i = 0; i < nVariables; i++) {
    stat = fix(variables[i], status[i], lNewValues);
    if (stat)       contra    = 1;
    if (lNewValues) newValues = true;
  }

  // ABA_SUB::_fixByLogImp(): clean up and return 
  for (int i = 0; i < nVariables; i++)
    delete status[i];

  if (contra)
    master_->out() << "contradiction" << endl;
  else
    master_->out() << nVariables << " variables fixed" << endl;
    
  return contra;

  }

  void ABA_SUB::fixByLogImp(ABA_BUFFER<int> &variables, 
                            ABA_BUFFER<ABA_FSVARSTAT*> &status)
  { }

  int ABA_SUB::setByRedCost()
  {

    if (!master_->fixSetByRedCost()) return 0;

  // ABA_SUB::setByRedCost(): local variables 
  int  nSet = 0;  //!< number of variables set 
  bool dummy =false;     //!< required to call function \a set(), no new values possible here
  int i;          //!< loop index


    master_->out() << "Setting Variables by Reduced Costs:        " << flush;

    if (master_->optSense()->max()) {
// set by reduced costs for maximization problems
/* In maximization problems the dual bound should not fall below the
 *   primal bound. Remember, the reduced cost of a (nonbasic) variable is the
 *   change of the object function if the variable becomes basic and
 *   changes one unit from its current value. As discrete variables can
 *   take only integer values, we analyzes what would happen if the
 *   value of the variable would decrease by one unit, if it is currently
 *   at its upper bound, or increase by one unit if it is currently at its
 *   lower bound.
 *
 *   Even for integer objective function values we require a violation of
 *   at least \a master_->eps(), otherwise a variable might be set to the wrong
 *   value.
 */  
  const int nVariables = nVar();

  for (i = 0; i < nVariables; i++) 
    if (variable(i)->discrete() && !variable(i)->fsVarStat()->fixed())
      if (lpVarStat(i)->status() == ABA_LPVARSTAT::AtUpperBound) {
        if (lp_->value() - lp_->reco(i) + master_->eps() 
            < master_->primalBound())
          if (set(i, ABA_FSVARSTAT::SetToUpperBound, dummy)) return 1;
          else 
            ++nSet;
      }
      else if (lpVarStat(i)->status() == ABA_LPVARSTAT::AtLowerBound) {
	 if (lp_->value() + lp_->reco(i) + master_->eps() 
            < master_->primalBound())
	    if (set(i, ABA_FSVARSTAT::SetToLowerBound, dummy)) return 1;
          else 
            ++nSet;
      }

    }
    else {
       // set by reduced costs for minimization problems      
       /* In minimization problems the dual bound should not exceed the
	*   primal bound.
	*/ 
       const int nVariables = nVar();

       for (i = 0; i < nVariables; i++)
	  if (variable(i)->discrete() && !variable(i)->fsVarStat()->fixed()) 
	     if (lpVarStat(i)->status() == ABA_LPVARSTAT::AtUpperBound) {
		if (lp_->value() - lp_->reco(i) - master_->eps() 
		      > master_->primalBound())
		   if (set(i, ABA_FSVARSTAT::SetToUpperBound, dummy)) return 1;
		   else
		      ++nSet;
	     }
	     else if (lpVarStat(i)->status() == ABA_LPVARSTAT::AtLowerBound) {
		if (lp_->value() + lp_->reco(i) - master_->eps() 
		      > master_->primalBound())
		   if (set(i, ABA_FSVARSTAT::SetToLowerBound, dummy)) return 1;
		   else 
		      ++nSet;
       }
       
    }

    master_->out() << nSet << " variables set" << endl;
    return 0;
  }

  ABA_CONSTRAINT *ABA_SUB::constraint(int i) const
  {
    return (*actCon_)[i];
  }

  ABA_VARIABLE *ABA_SUB::variable(int i) const
  {
    return (*actVar_)[i];
  }
 
  void ABA_SUB::reoptimize ()
  {
    PHASE phase;  //!< current phase of the subproblem optimization

    // output a banner for the subproblem
  master_->out() << endl << "************************************************" << endl;
  master_->out() << "Subproblem " << id_ << " on Level " << level_ << ":" << endl << endl;

  if (master_->optSense()->max()) {
    master_->out(1) << "Global Lower Bound: " << lowerBound()       << endl;
    master_->out(1) << "Local  Upper Bound: " << upperBound()       << endl;
    master_->out(1) << "Global Upper Bound: " << master_->upperBound() << endl;
  }
  else {
    master_->out(1) << "Local  Lower Bound: " << lowerBound()       << endl;
    master_->out(1) << "Global Lower Bound: " << master_->lowerBound() << endl;
    master_->out(1) << "Global Upper Bound: " << upperBound()       << endl;
  }

  master_->out(1) << "Current Guarantee : ";
  master_->printGuarantee();
  master_->out() << endl << endl;


    master_->out() << "reoptimization starts" << endl;

    phase = _activate ();
    
    if (phase == Fathoming) fathomTheSubTree ();
    else {
      phase = cutting ();
      if (phase == Fathoming) fathomTheSubTree ();
    }

    _deactivate ();

    status_ = Processed;

  }
  
  double ABA_SUB::lowerBound() const
  {
    if (master_->optSense()->max()) return master_->primalBound();
    else                        return dualBound_;
  }

  double ABA_SUB::upperBound() const
  {
    if (master_->optSense()->min()) return master_->primalBound();
    else                         return dualBound_;
  }

/*! @name 
 *   Like in the class ABA_MASTER we work again with primal and dual
 *   bounds such that the code works both for minimization and maximization
 *   problems.
 */
//@{

  void ABA_SUB::dualBound(double x)
  {
    if (master_->optSense()->max()) {
      if (x > dualBound_) {
        master_->err() << "Warning: ABA_SUB::dualBound(): worse dual ";
        master_->err() << "bound " << x << "ignored." << endl;
        master_->err() << "Keeping old dual bound " << dualBound_ << "." << endl;
        return;
      }
    }
    else
      if (x < dualBound_) {
        master_->err() << "Warning: ABA_SUB::dualBound(): worse dual ";
        master_->err() << "bound " << x << "ignored." << endl;
        master_->err() << "Keeping old dual bound " << dualBound_ << "." << endl;
        return;
      }

    dualBound_ = x;

    if (this == master_->root() && master_->betterDual(dualBound_)) 
      master_->dualBound(dualBound_);

    if (status_ == Active) {
      if (master_->optSense()->max())
        master_->treeInterfaceNodeBounds(id_, master_->primalBound(), 
                                         dualBound_);
      else
        master_->treeInterfaceNodeBounds(id_, dualBound_, 
                                         master_->primalBound());
    }
    
  }

  bool ABA_SUB::betterDual(double x) const
  {
    if (master_->optSense()->max()) return x < dualBound_ ? true : false;
    else                            return x > dualBound_ ? true : false;
  }

  bool ABA_SUB::boundCrash() const
  {
    return(master_->primalViolated(dualBound_));
  }
//@}

  void ABA_SUB::maxIterations(int max)
  {
    master_->out() << "Setting maximal number of iterations in the ";
    master_->out() << "cutting plane phase to " << max << endl;
    maxIterations_ = max;
  }

  void ABA_SUB::getBase()
  {
    if (lp_->basisStatus() == ABA_LP::Available) {
  // get the LP status of the variables
/* There may be variables which are fixed or set but which could
 *   not be eliminated since their previous ABA_LPVARSTAT has been
 *   \a Basic. The ABA_LPVARSTAT of these variables after the solution
 *   of the linear program could cause a wrong fixing or setting.
 *   Hence we assign to them the status \a Unknown if the status
 *   is not \a Basic.
 */ 
  ABA_LPVARSTAT::STATUS newStat;

  const int nVariables = nVar();

  for (int i = 0; i < nVariables; i++) {
    newStat = lp_->lpVarStat(i);
    if (newStat != ABA_LPVARSTAT::Eliminated) {
      if ((*fsVarStat_)[i]->fixedOrSet() && newStat != ABA_LPVARSTAT::Basic)
        (*lpVarStat_)[i]->status(ABA_LPVARSTAT::Unknown);
      else
        (*lpVarStat_)[i]->status(newStat);
    }
    else
      (*lpVarStat_)[i]->status(ABA_LPVARSTAT::Eliminated);
  }

  // get the LP status of the slack variables 
  const int nActCon = actCon_->number();
  for (int i = 0; i < nActCon; i++)
    (*slackStat_)[i]->status(lp_->slackStat(i));

    }
  }

  int ABA_SUB::fix(int i, ABA_FSVARSTAT *newStat, bool &newValue)
  {
    ABA_VARIABLE *v = variable(i);

    int contra = 0;

    if (fsVarStat(i)->contradiction(newStat)) contra = 1;
    else fsVarStat(i)->status(newStat);
      
    if (!v->fsVarStat()->fixed())    
      master_->newFixed(1);

    v->fsVarStat()->status(newStat);

  // is variable fixed to a new value 
  double x = xVal_[i];
  if ((newStat->status() == ABA_FSVARSTAT::FixedToLowerBound
                                       && x > lBound(i) + master_->eps()) ||
      (newStat->status() == ABA_FSVARSTAT::FixedToUpperBound
                                       && x < uBound(i) - master_->eps()) ||
      (newStat->status() == ABA_FSVARSTAT::Fixed
                                       && !(master_->equal(x, newStat->value())))
      )
        newValue = true;
      else
        newValue = false;

  // update the bounds of the fixed variable 
  double newBound = fixSetNewBound(i);
    
  (*lBound_)[i] = newBound;
  (*uBound_)[i] = newBound;

  variable(i)->lBound(newBound);
  variable(i)->uBound(newBound);
    
  updateBoundInLp(i);


    return contra;
  }

  int ABA_SUB::set(int i, ABA_FSVARSTAT *newStat, bool &newValue)
  {
    return set(i, newStat->status(), newStat->value(), newValue);
  }

  int ABA_SUB::set(int i, ABA_FSVARSTAT::STATUS newStat, bool &newValue)
  {
    if (newStat == ABA_FSVARSTAT::Set) {
      const char *_error="ABA_SUB::set() no value specified for status Set \n setting to value not implemented";
      THROW_PARAM(AlgorithmFailureException,afcUnknown,_error);
    }

    return set (i, newStat, 0.0, newValue);
  }

  int ABA_SUB::set(int i, ABA_FSVARSTAT::STATUS newStat, double value, bool &newValue)
  {
    ABA_VARIABLE *v = variable(i);

    if (v->fsVarStat()->contradiction(newStat, value)) return 1;
    else {
      (*fsVarStat_)[i]->status(newStat, value);

// is variable set to a new value
/* If a variable is fixed according to logical implications before
 *   the subproblem is processed, then no \a lp_ is available.
 */  
  if (lp_ == 0) newValue = false;
  else {
    double x = xVal_[i];
    if ((newStat == ABA_FSVARSTAT::SetToLowerBound
         && x > (*lBound_)[i] + master_->eps())   ||
        (newStat == ABA_FSVARSTAT::SetToUpperBound
         && x < (*uBound_)[i] - master_->eps())   ||
        (newStat == ABA_FSVARSTAT::Set
         && !(master_->equal(x, value)))            )
            newValue = true;
        else
          newValue = false;
  }

// update the bounds of the set variables        
/* The function \a updateBoundInLp() checks if a linear program is present
 *   in the subproblem. The bounds in the linear program can only be
 *   changed if the variable is not eliminated. However if the variable
 *   is set to a different bound, then we would have detected a contradiction.
 */  
  double newBound = fixSetNewBound(i);
    
  (*lBound_)[i] = newBound;
  (*uBound_)[i] = newBound;

  updateBoundInLp(i);


      return 0;
    }
  }

  void ABA_SUB::updateBoundInLp(int i)
  {
    if (lp_ == 0 || lp_->eliminated(i)) return;

    double newBound = (*lBound_)[i];

    lp_->changeLBound(i, newBound);
    lp_->changeUBound(i, newBound);
    
  }

  double ABA_SUB::fixSetNewBound(int i)
  {
    switch ((*fsVarStat_)[i]->status()) {
      case ABA_FSVARSTAT::SetToLowerBound:
        return (*lBound_)[i];
      case ABA_FSVARSTAT::FixedToLowerBound:
        return variable(i)->lBound();
      case ABA_FSVARSTAT::SetToUpperBound:
        return (*uBound_)[i];
      case ABA_FSVARSTAT::FixedToUpperBound:
        return variable(i)->uBound();
      case ABA_FSVARSTAT::Set:
      case ABA_FSVARSTAT::Fixed:
        return (*fsVarStat_)[i]->value();
      default:
        const char *_error="ABA_SUB::fixSetNewBound(): variable neither fixed nor set";
	THROW_PARAM(AlgorithmFailureException,afcFixSet,_error);
        return 0.0;
    }
  }

  bool ABA_SUB::pausing()
  {
    return false;
  }

  int ABA_SUB::_conEliminate()
  {
    ABA_BUFFER<int> eliminate(master_, nCon());
    
    conEliminate(eliminate);

    removeCons(eliminate);

    master_->out() << eliminate.number() << " constraints eliminated" << endl;

    return eliminate.number();
  }

  void ABA_SUB::conEliminate(ABA_BUFFER<int> &remove)
  {
    switch (master_->conElimMode()) {
      case ABA_MASTER::NonBinding: nonBindingConEliminate(remove);
                               break;
      case ABA_MASTER::Basic:      basicConEliminate(remove);
                               break;
      default:                 break;
    }
  }

  void ABA_SUB::nonBindingConEliminate(ABA_BUFFER<int> &remove)
  {
    const int conElimAge = master_->conElimAge() - 1;

    const int nConstraints = nCon();

    for (int i = 0; i < nConstraints; i++) 
      if ((*actCon_)[i]->dynamic()) {
        if (fabs(lp_->slack(i)) > master_->conElimEps()) {
          if (actCon_->redundantAge(i) >= conElimAge) {
            remove.push(i);
	  }
          else
            actCon_->incrementRedundantAge(i);
        }
        else
          actCon_->resetRedundantAge(i);
      }
  }

  void ABA_SUB::basicConEliminate(ABA_BUFFER<int> &remove)
  {
    const int conElimAge   = master_->conElimAge() - 1;
    const int nConstraints = nCon();

    for (int i = 0; i < nConstraints; i++) 
      if ((*actCon_)[i]->dynamic()) {
        if ((*slackStat_)[i]->status() == ABA_SLACKSTAT::Basic) {
          if (actCon_->redundantAge(i) >= conElimAge)
            remove.push(i);
          else
            actCon_->incrementRedundantAge(i);
        }
        else
          actCon_->resetRedundantAge(i);
      }
  }

  int ABA_SUB::_varEliminate()
  {
    ABA_BUFFER<int> eliminate(master_, nVar());
    
    varEliminate(eliminate);

    removeVars(eliminate);

    master_->out() << eliminate.number() << " variables eliminated" << endl;

    return eliminate.number();
  }

  void ABA_SUB::varEliminate(ABA_BUFFER<int> &remove)
  {
    if (master_->varElimMode() == ABA_MASTER::ReducedCost)
      redCostVarEliminate(remove);
  }

  void ABA_SUB::redCostVarEliminate(ABA_BUFFER<int> &remove)
  {
    bool max = master_->optSense()->max();
    const int  varElimAge = master_->varElimAge() - 1;
    const double eps = master_->machineEps();
    
    const int nVariables = nVar();
    for (int i = 0; i < nVariables; i++) 
      if (variable(i)->dynamic() && !(*fsVarStat_)[i]->fixedOrSet()
          && fabs(xVal_[i]) < eps) {

        bool bad = false;

        if (!lpVarStat(i)->basic()) {
          if (max) {
            if (lp_->reco(i) < -master_->varElimEps()) bad = true;
          }
          else {
            if (lp_->reco(i) > master_->varElimEps()) bad = true;
          }
        }

        if (bad) {
          if (actVar_->redundantAge(i) >= varElimAge)
            remove.push(i);
          else
            actVar_->incrementRedundantAge(i);
        }
        else
          actVar_->resetRedundantAge(i);
      }
  } 

  void ABA_SUB::fathomTheSubTree()
  {
    master_->out() << "fathom complete subtree" << endl;

    if (status_ != Fathomed) {
      if (status_ == Dormant || status_ == Unprocessed)
        master_->openSub()->remove(this);
     
      if (sons_) for (int i = 0; i < sons_->number(); i++)
        (*sons_)[i]->fathomTheSubTree();
      else
        fathom(false); //!< no reoptimization desired
    }
  }

  int ABA_SUB::_separate()
  {
  // should we separate cuts at all? 
  if (!master_->cutting()) return 0;

  // separate cuts 
  int nCuts;

  master_->out() << endl;
  master_->out() << "Separation of Cutting Planes" << endl;

  localTimer_.start(true);
  nCuts = separate();
  master_->separationTime_.addCentiSeconds( localTimer_.centiSeconds() );

  return nCuts;

  }
 
  int ABA_SUB::separate()
  {
    master_->out() << endl << "no separation implemented" << endl;
    return 0;
  }

  int ABA_SUB::_improve(double &primalValue)
  {
    if (master_->pbMode() != ABA_MASTER::NoPrimalBound) return 0;
    master_->out() << endl;
    master_->out() << "Apply Primal Heuristic" << endl;
    
    localTimer_.start(true);

    int status = improve(primalValue);

    master_->improveTime_.addCentiSeconds( localTimer_.centiSeconds() );

    return status;
  }

  int ABA_SUB::improve(double &primalValue)
  {
    master_->out() << endl << "no primal heuristic implemented" << endl;
    return 0;
  }

  void ABA_SUB::infeasibleSub()
  {
    master_->out() << "infeasible subproblem" << endl;
    
    if (master_->optSense()->max()) dualBound_ = -master_->infinity();
    else                            dualBound_ =  master_->infinity();

    master_->treeInterfaceNodeBounds(id_, lowerBound(), upperBound());
  }

  bool ABA_SUB::infeasible()
  {
    if (master_->optSense()->max()) {
      if (dualBound_ == -master_->infinity()) return true;
      else                                    return false;
    }
    else {
      if (dualBound_ ==  master_->infinity()) return true;
      else                                    return false;
    }
  }
  

  void ABA_SUB::activateVars(ABA_BUFFER<ABA_POOLSLOT<ABA_VARIABLE, ABA_CONSTRAINT> *> &newVars)
  {
  // perform a reallocation if required by the new variables 
  int n = nVar();
    
  if (n + newVars.number() >= maxVar()) {
    int newMax = ((maxVar() + newVars.number())*11)/10 + 1;
    varRealloc(newMax);
  }

  // add the new variables to the data structures of the subproblem 
  ABA_VARIABLE *v;

  const int nNewVars = newVars.number();
    
  for (int i = 0; i < nNewVars; i++) {
    v = (ABA_VARIABLE *) newVars[i]->conVar();
      
    (*fsVarStat_)[n + i] = new ABA_FSVARSTAT(v->fsVarStat());
    (*lpVarStat_)[n + i] = new ABA_LPVARSTAT(master_, ABA_LPVARSTAT::Unknown);
    (*lBound_)[n + i]    = v->lBound();
    (*uBound_)[n + i]    = v->uBound();

    v->activate();
  }

  actVar_->insert(newVars);

  master_->addVars(nNewVars);

  }

  void ABA_SUB::addVarsToLp(ABA_BUFFER<ABA_POOLSLOT<ABA_VARIABLE, ABA_CONSTRAINT>*> &newVars,
                        ABA_BUFFER<ABA_FSVARSTAT*> *localStatus)
  {
  // ABA_SUB::addVarsToLp(): local variables 
  const int nNewVars = newVars.number();
  ABA_BUFFER<ABA_VARIABLE*>  vars(master_, nNewVars);
  ABA_BUFFER<ABA_FSVARSTAT*> stat(master_, nNewVars);
  ABA_BUFFER<double>     lb(master_, nNewVars);
  ABA_BUFFER<double>     ub(master_, nNewVars);
  ABA_VARIABLE          *v;

  // get the new variables together with their status and bounds 
  for (int i = 0; i < nNewVars; i++) {
    v = (ABA_VARIABLE *) newVars[i]->conVar();

    vars.push(v);
      
    if (localStatus) {
      if (v->fsVarStat()->contradiction((*localStatus)[i])) {
        const char *_error="ABA_SUB::addVarsToLp(): local status contradicts global status";
	THROW_PARAM(AlgorithmFailureException,afcAddVar,_error);
      }
      stat.push((*localStatus)[i]);
    }
    else stat.push(v->fsVarStat());
      
    lb.push(v->lBound());
    ub.push(v->uBound());
  }

// add the new variables to the linear program  
/* If more than one variable is added we generate the expanded format
 *   of the constraints, such that the columns can be determined more
 *   efficiently. However, this threshold is completely experimental
 *   and problem specific!
 */  
  const int nConstraints = nCon();
  if (vars.number() > 1)
    for (int i = 0; i < nConstraints; i++) constraint(i)->_expand();
  
  localTimer_.start(true);
    
  lp_->addVars(vars, stat, lb, ub);

  master_->lpTime_.addCentiSeconds(localTimer_.centiSeconds());

  if (vars.number() > 1)
    for (int i = 0; i < nConstraints; i++) constraint(i)->_compress();

  }

  int ABA_SUB::_removeVars(ABA_BUFFER<int> &remove)
  {
    const int nRemove = remove.number();

    if (nRemove) {
// sort the variables which are removed
/* The following functions removing the variables from the data structures
 *   of the subproblem require the variables sorted in increasing order.
 *   This sorting can be performed in linear time.
 */  
  ABA_ARRAY<bool> marked(master_, nVar(), false);

  for (int i = 0; i < nRemove; i++)
    marked[remove[i]] = true;

  ABA_BUFFER<int> removeSorted(master_, nRemove);

  const int nVariables = nVar();
  for (int i = 0; i < nVariables; i++)
    if (marked[i]) removeSorted.push(i);

  // remove the variables from the subproblem 
  const int nRemoveSorted = removeSorted.number();

  localTimer_.start(true);
  lp_->removeVars(removeSorted);
  master_->lpTime_.addCentiSeconds(localTimer_.centiSeconds());
      
  for (int i = 0; i < nRemoveSorted; i++) {
    delete (*fsVarStat_)[removeSorted[i]];
    delete (*lpVarStat_)[removeSorted[i]];
  }
  for (int i = 0; i < nRemoveSorted; i++)
    (*actVar_)[removeSorted[i]]->deactivate();
      
  actVar_->remove(removeSorted);

  fsVarStat_->leftShift(removeSorted);
  lpVarStat_->leftShift(removeSorted);
  uBound_->leftShift(removeSorted);
  lBound_->leftShift(removeSorted);
      
  master_->removeVars(nRemoveSorted);


    }
    return remove.number();
  }

  void ABA_SUB::removeCons(ABA_BUFFER<int> &remove)
  {
    const int nRemove = remove.number();

    for (int i = 0; i < nRemove; i++)
      removeConBuffer_->push(remove[i]);
  }

  void ABA_SUB::removeCon(int i)
  {
    removeConBuffer_->push(i);
  }

  int ABA_SUB::_removeCons(ABA_BUFFER<int> &remove)
  {
    const int nRemove = remove.number();
    if (nRemove) {
// sort the constraints which are removed  
/* The following functions to remove the variables from the data structures
 *   in the subproblem require the constraints sorted in increasing order.
 *   This sorting is performed in linear time.
 */  
  ABA_ARRAY<bool> marked(master_, nCon(), false);

  for (int i = 0; i < nRemove; i++)
    marked[remove[i]] = true;

  ABA_BUFFER<int> removeSorted(master_, nRemove);

  const int nConstraints = nCon();
  for (int i = 0; i < nConstraints; i++)
    if (marked[i]) removeSorted.push(i);

  // remove the constraints from the subproblem 
  const int nRemoveSorted = removeSorted.number();
  for (int i = 0; i < nRemoveSorted; i++) {
    (*actCon_)[removeSorted[i]]->deactivate();
    delete (*slackStat_)[removeSorted[i]];
  }

  actCon_->remove(removeSorted);
  slackStat_->leftShift(removeSorted);
  localTimer_.start(true);
  lp_->removeCons(removeSorted);
  master_->lpTime_.addCentiSeconds(localTimer_.centiSeconds());

  master_->removeCons(nRemoveSorted);

    }
    return nRemove;
  }

  void ABA_SUB::varRealloc(int newSize)
  {
    actVar_->realloc(newSize);
    fsVarStat_->realloc(newSize);
    lpVarStat_->realloc(newSize);
    lBound_->realloc(newSize);
    uBound_->realloc(newSize);
    removeVarBuffer_->realloc(newSize);

    double *xValNew = new double[newSize];

    const int nVariables = nVar();

    for (int i = 0; i < nVariables; i++)
      xValNew[i] = xVal_[i];

    delete [] xVal_;
    xVal_ = xValNew;

    localTimer_.start(true);
    lp_->varRealloc(newSize);
    master_->lpTime_.addCentiSeconds(localTimer_.centiSeconds());
  }

  void ABA_SUB::conRealloc(int newSize)
  {
    actCon_->realloc(newSize);
    slackStat_->realloc(newSize);
    removeConBuffer_->realloc(newSize);
    localTimer_.start(true);
    lp_->conRealloc(newSize);
    master_->lpTime_.addCentiSeconds(localTimer_.centiSeconds());

    double *yValNew = new double[newSize];

    const int nConstraints = nCon();

    for (int i = 0; i < nConstraints; i++)
      yValNew[i] = yVal_[i];

    delete [] yVal_;
    yVal_ = yValNew;

  }

  int ABA_SUB::initializeLp()
  {
    while (1) {
      localTimer_.start(true);
      lp_ = generateLp();
      master_->lpTime_.addCentiSeconds(localTimer_.centiSeconds());
      
      if (lp_->infeasible()) {
        if (_initMakeFeas()) return 1;
        else                 delete lp_;
      }
      else return 0;
    }
  }

  int ABA_SUB::_initMakeFeas()
  {
    if (!master_->pricing()) return 1;

// find variables which could make initial LP feasible
/* The determination of useful variables has be implemented
 *   problem specifically in the function \a initMakeFeas().
 */  
  ABA_BUFFER<ABA_INFEASCON*>   *infeasCon = lp_->infeasCon();
  ABA_BUFFER<ABA_VARIABLE*>  newVars(master_, infeasCon->number());
  ABA_POOL<ABA_VARIABLE, ABA_CONSTRAINT> *pool = 0;

  int status = initMakeFeas(*infeasCon, newVars, &pool);

  if (status) return 1;

// insert the variables in a pool and determine the pool slots \a newSlots  
/* If not differently specified with the help of the function
 *   \a initMakeFeas() we use the default variable pool for the new variables.
 */  
  const int nNewVars = newVars.number();

  ABA_BUFFER<ABA_POOLSLOT<ABA_VARIABLE, ABA_CONSTRAINT> *> newSlots(master_, nNewVars);
  ABA_POOLSLOT<ABA_VARIABLE, ABA_CONSTRAINT> *slot;

  if (pool == 0) pool = master_->varPool();
    
  for (int i = 0; i < nNewVars; i++) {
    slot = pool->insert(newVars[i]);
  
    if (slot == 0) {
      const char *_error="ABA_SUB::_initMakeFeas(): pool too small to insert all constraints";
      THROW_PARAM(AlgorithmFailureException,afcMakeFeasible,_error);
    }

    newSlots.push(slot);
  }


    activateVars(newSlots);

    return 0;
  }

  int ABA_SUB::initMakeFeas(ABA_BUFFER<ABA_INFEASCON*> &infeasCons,
                            ABA_BUFFER<ABA_VARIABLE*> &newVars,
                            ABA_POOL<ABA_VARIABLE, ABA_CONSTRAINT> **pool)
  {
    return 1;
  }

  bool ABA_SUB::tailingOff()
  {
    return true;
  }

  bool ABA_SUB::solveApproxNow()
  {
     return false;
  }
