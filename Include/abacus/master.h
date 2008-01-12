/*!\file
 * \author Matthias Elf
 * \brief the master of the optimization.
 *
 * As the name already indicates, the class ABA_MASTER is the central
 * object of the framework. The most important tasks of the class ABA_MASTER
 * is the management of the implicit enumeration. Moreover, it provides already
 * default implementations for constraints, cutting planes, and
 * variables pools. The class ABA_MASTER also stores various parameter
 * settings and compiles statistics about the solution process.
 *
 * \par
 * The class ABA_MASTER is an abstract class from which a problem specific
 * master has to be derived.
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
 * $Id: master.h,v 2.7 2007/07/04 10:24:35 baumann Exp $
 */


#ifndef ABA_MASTER_H
#define ABA_MASTER_H

#include "abacus/global.h"
#include "abacus/optsense.h"
#include "abacus/bstack.h"
#include "abacus/hash.h"
#include "abacus/cputimer.h"
#include "abacus/cowtimer.h"
#include "abacus/string.h"
#include "abacus/standardpool.h"

#ifdef ABACUS_PARALLEL
#include "abacus/message.h"
class ABA_CONVAR;
class ABA_PARMASTER;
class ABA_NOTIFYSERVER;
#endif

class ABA_SUB;
class ABA_BRANCHRULE;
class ABA_VARIABLE;
class ABA_CONSTRAINT;

class ABA_HISTORY;
class ABA_OPENSUB;
class ABA_FIXCAND;
class ABA_LPMASTEROSI;
class OsiSolverInterface;

template<class BaseType, class CoType> class ABA_STANDARDPOOL;

  class  ABA_MASTER :  public ABA_GLOBAL  {
    friend class ABA_SUB;
    friend class ABA_FIXCAND;
#ifdef ABACUS_PARALLEL
    friend ABA_NOTIFYSERVER;
    friend ABA_PARMASTER;
#endif
    public:

    /*! The various statuses of the optimization process.
     *
     * \param Optimal The optimization terminated with an error and
     *                  without reaching one of the resource limits.
     *                  If there is a feasible solution then the
     *                  optimal solution has been computed. 
     * \param Error An error occurred during the optimization process.
     * \param Unprocessed The initial status, before the optimization
     *                      starts.
     * \param Processing The status while the optimization is performed.
     * \param Guaranteed If not the optimal solution is determined, but
     *                     the required guarantee is reached, then
     *			   the status is \a Guaranteed.
     * \param MaxLevel The status, if subproblems are ignored since the
     *   	         maximum enumeration level is exceeded.
     * \param MaxCpuTime The status, if the optimization terminates since
     *                     the maximum cpu time is exceeded.
     * \param MaxCowTime The status, if the optimization terminates since
     *                     the maximum wall-clock time is exceeded.
     * \param ExceptionFathom The status, if at least one subproblem
     *                          has been fathomed according to a
     *                          problem specific criteria determined
     *                          in the function ABA_SUB::exceptionFathom().
     */
      enum STATUS {Optimal, Error, OutOfMemory, Unprocessed, Processing,
                   Guaranteed, MaxLevel, MaxCpuTime,
                   MaxCowTime, ExceptionFathom};

      /*! \brief Literal values for the enumerators of the corresponding
       *  enumeration type. The order of the enumerators is preserved.
       *  (e.g., {\tt STATUS\_[0]=="Optimal"})
       */
      static const char* STATUS_[];

      /*! This enumeration defines the different output levels:
       *
       * \param Silent No output at all.
       * \param Statistics No output during the optimization, but output
       *                   of final statistics.
       * \param Subproblem In addition to the previous level also a single
       *                   line of output after every subproblem optimization.
       * \param LinearProgram In addition to the previous level also a single
       *                      line of output after every solved linear
       *		      program.
       * \param Full Tons of output.
       */
      enum OUTLEVEL {Silent, Statistics, Subproblem, LinearProgram, Full};


      /*! \brief Literal values for the enumerators of the corresponding
       *  enumeration type. The order of the enumerators is preserved.
       *  (e.g., {\tt OUTLEVEL\_[0]=="Silent"})
       */
      static const char* OUTLEVEL_[];


      /* \brief The enumeration defining the different enumeration strategies for
       *        the branch and bound algorithm.
       *
       *  \param BestFirst Best-first search, i.e., select the subproblem
       *                  with best dual bound, i.e., the subproblem
       *                  having minimal dual bound for a minimization
       *                  problem, or the subproblem having maximal
       *                  dual bound for a maximization problem.
       *  \param BreadthFirst Breadth-first search, i.e., select the subproblem
       *                     with minimal level in the enumeration tree.
       *  \param DepthFirst Depth-first search, i.e., select the subproblem
       *                    with maximal level in the enumeration tree.
       *  \param DiveAndBest As long as no primal feasible solution is known
       *                    the next subproblem is selected according
       *		    to the depth-first search strategy, otherwise
       *		    the best-first search strategy is applied.
       */
      enum ENUMSTRAT {BestFirst, BreadthFirst, DepthFirst, DiveAndBest};
      /*! \brief Literal values for the enumerators of the corresponding
       *  enumeration type. The order of the enumerators is preserved.
       *  (e.g., {\tt ENUMSTRAT\_[0]=="BestFirst"})
       */
      static const char *ENUMSTRAT_[];

      /*! \brief This enumeration defines the two currently implemented
       *         branching variable selection strategies.
       *
       *  \param CloseHalf Selects the variable with fractional part
       *                   closest to \f$0.5\f$.
       *  \param CloseHalfExpensive Selects the variable with fractional
       *                            part close to \f$0.5\f$ (within some
       *			    interval around \f$0.5\f$) and has highest
       *			    absolute objective function coefficient.
       */
      enum BRANCHINGSTRAT {CloseHalf, CloseHalfExpensive};

      /*! \brief Literal values for the enumerators of the corresponding
       *  enumeration type. The order of the enumerators is preserved.
       *  (e.g., {\tt BRANCHINGSTRAT\_[0]=="CloseHalf"})
       */
      static const char *BRANCHINGSTRAT_[];

      /*! \brief This enumeration provides various methods for the initialization of the
       *  primal bound. 
       *
       *  The modes \a OptimalPrimalBound and
       *  \a OptimalOnePrimalBound
       *  can be useful in the testing phase. For these modes the value
       *  of an optimum solution must stored in the file given
       *  by the parameter {\tt OptimumFileName} in the parameter file.   
       *
       *  \param NoPrimalBound The primal bound is initialized with
       *                       \f$-\infty\f$ for maximization problems and
       *	               \f$\infty\f$ for minimization problems, respectively.
       *  \param OptimalPrimalBound The primal bound is initialized with the value
       *                            of the optimum solution.
       *  \param OptimalOnePrimalBound The primal bound is initialized with the value of
       *                               optimum solution minus 1 for maximization problems
       *	                       and with the value of the optimum solution plus one
       *	                       for minimization problems, respectively.
       */
      enum PRIMALBOUNDMODE {NoPrimalBound, Optimum, OptimumOne};

      /*! \brief Literal values for the enumerators of the corresponding
       *  enumeration type. The order of the enumerators is preserved.
       *  (e.g., {\tt PRIMALBOUNDMODE\_[0]=="None"})
       */
      static const char* PRIMALBOUNDMODE_[];

      /*! The way nodes are skipped for the generation of cuts.
       *
       *  \param SkipByNode Cuts are only generated in every {\tt SkipFactor}
       *                    subproblem, where {\tt SkipFactor} can be
       *                    controlled with the parameter file {\tt .abacus}.
       *  \param SkipByLevel Cuts are only generated in every
       *                     {\tt SkipFactor} level of the enumeration tree.
       */
      enum SKIPPINGMODE{SkipByNode, SkipByLevel};

      /*! \brief Literal values for the enumerators of the corresponding
       *  enumeration type. The order of the enumerators is preserved.
       *  (e.g., {\tt SKIPPINGMODE\_[0]=="None"})
       */
      static const char* SKIPPINGMODE_[];

      /*! \brief This enumeration defines the ways for automatic constraint elimination
       *  during the cutting plane phase.
       *
       *  \param NoConElim No constraints are eliminated.
       *  \param NonBinding Nonbinding constraints are eliminated.
       *  \param Basic Constraints with basic slack variable are eliminated.
       */
      enum CONELIMMODE {NoConElim, NonBinding, Basic};


      /*! \brief Literal values for the enumerators of the corresponding
       *  enumeration type. The order of the enumerators is preserved.
       *  (e.g., {\tt CONELIMMODE\_[0]=="None"})
       */
      static const char* CONELIMMODE_[];

      /*! \brief This enumeration defines the ways for automatic variable elimination
       *  during the column generation algorithm.
       *
       *  \param NoVarElim No variables are eliminated.
       *  \param ReducedCost Variables with high absolute reduced costs
       *                     are eliminated.
       */
      enum VARELIMMODE {NoVarElim, ReducedCost};

      /*! \brief Literal values for the enumerators of the corresponding
       *  enumeration type. The order of the enumerators is preserved.
       *  (e.g., {\tt VARELIMMODE\_[0]=="None"})
       */
      static const char* VARELIMMODE_[];

      /*! \brief This enumeration defines what kind of output can be generated
       *  for the VBCTOOL.
       *
       *  \param None No output for the tree interface.
       *  \param File Output for the tree interface is written to a
       *              file.
       *  \param Pipe Output for the tree interface is pipe to the
       *	      standard output.
       */
      enum VBCMODE {NoVbc, File, Pipe};

      /*! \brief Literal values for the enumerators of the corresponding
       *  enumeration type. The order of the enumerators is preserved.
       *  (e.g., {\tt VBCMODE\_[0]=="None"})
       */
      static const char* VBCMODE_[];

/*************************************************************************
 *      These are all solvers supported by OSI, 
 *      see https://projects.coin-or.org/Osi .
 *************************************************************************/
      /*! \brief This enumeration defines which solvers can be used to solve theLP-relaxations.
       */
	enum OSISOLVER {Cbc, Clp, CPLEX, DyLP, FortMP, GLPK, MOSEK, OSL, SoPlex, SYMPHONY, Vol, XPRESS_MP};

      /*! \brief Array for the literal values for possible Osi solvers
       */
      static const char* OSISOLVER_[];

/*! The constructor.
 *
 *        \param problemName The name of the problem being solved.
 *                               Must not be a 0-pointer.
 *        \param cutting If \a true, then cutting planes can be
 *                           generated if the function ABA_SUB::separate()
 *                           is redefined.
 *        \param pricing If \a true, then inactive variables are priced in,
 *                           if the function ABA_SUB::pricing() is redefined.
 *        \param optSense The sense of the optimization. The default
 *                            value is ABA_OPTSENSE::Unknown. If the sense
 *                            is unknown when this constructor is called,
 *                            e.g., if it is
 *                            read from a file in the constructor of the
 *                            derived class, 
 *                            then it must be initialized in the constructor of
 *                            the derived class.
 *        \param eps The zero-tolerance used within all member functions of
 *                    objects which have a pointer to this master
 *                    (default value \a 1.0e-4).
 *        \param machineEps The machine dependent zero tolerance
 *                            (default value \a 1.0e-7).
 *        \param infinity All values greater than \a infinity are
 *                         regarded as ``infinite big'', all values less than
 *                         \a -infinity are regarded as ``infinite small''
 *                         (default value \a 1.0e30).
 *       \param readParamFromFile If true, then the parameter file 
 *            .abacus is read, otherwise the parameters are initialized
 *              with default values (default \a true).
 *                                       
 *   The members \a primalBound_ and \a dualBound_ stay uninitialized
 *   since this can only be done when the sense of optimization
 *   (minimization or maximization) is known. The initialization
 *   is performed automatically in the function \a optimize().
*/  
      ABA_MASTER(const char *problemName, bool cutting, bool pricing,
                 ABA_OPTSENSE::SENSE optSense = ABA_OPTSENSE::Unknown,
                 double eps = 1.0e-4, double machineEps = 1.0e-7,
                 double infinity = 1.0e30,
                 bool readParamFromFile = true);

//! The destructor.
      virtual ~ABA_MASTER();
#ifdef ABACUS_PARALLEL

/*! \brief The virtual function \a unpackConVar()
 *   constructs and unpacks an object of some subclass of ABA_CONVAR from a Message.
 *
 *   The function is
 *   called every time an object of some subclass of ABA_CONSTRAINT
 *   or ABA_VARIABLE has to be recreated at the receiving processor.
 *
 *   \par
 *   This function has to be implemented by the user if user defined subclasses
 *   of ABA_CONSTRAINT or ABA_VARIABLE are used. In this case
 *   the \a unpackConVar() function of the derived class has to call
 *   the \a unpackConVar() function of the base class if it cannot construct
 *   an object of the requested class.   
 *
 *   \return A Pointer to a newly constructed object of the subclass of ABA_CONVAR
 *   specified by \a classId.
 *
 *   \param msg The message from which the ABA_CONVAR is unpacked.
 *   \param classId
 *     The class identification of the subclass of ABA_CONVAR
 *      which should be unpacked.
 */
      virtual ABA_CONVAR *unpackConVar(ABA_MESSAGE &msg, int classId) const;

/*! Construicts and unpacks an object of some subclass of ABA_BRANCHRULE from a Message.
 *
 *   It has to be redefined
 *   in a derived class if user defined subclasses of ABA_BRANCHRULE are used.
 *   The \a unpackBranchrule() function of the derived class has to call
 *   the \a unpackBranchrule() function of the base class if it cannot construct
 *   an object of the requested subclass of ABA_BRANCHRULE. 
 *
 *   \return A Pointer to a newly constructed object of the subclass of ABA_BRANCHRULE
 *   specified by \a classId.
 *
 *   \param msg The message from which the ABA_BRANCHRULE is unpacked.
 *   \param classId
 *     The class identification of the subclass of ABA_BRANCHRULE
 *      which should be unpacked.
 */
      virtual ABA_BRANCHRULE *unpackBranchrule(ABA_MESSAGE &msg, int classId) const;

/*! \brief The pure virtual function \a unpackSub()
 *   constructs and unpacks an object of the user defined subclass of ABA_SUB from a Message.
 *
 *   This function has to be implemented by the user.
 *
 *   \return A Pointer to a newly constructed object of the user defined subclass of ABA_SUB.
 *
 *   \param msg The message from which the subclass of ABA_SUB is unpacked.
 */
      virtual ABA_SUB *unpackSub(ABA_MESSAGE &msg) = 0;
#endif

/*! Performs the optimization by \bab.
 *
 *   The status of the optimization.
 */
      STATUS optimize();

/*! @name bounds 
 *   In order to embed both minimization and maximization problems in this
 *   system we work internally with primal bounds, i.e., a value which
 *   is worse than the best known solution (often a value of a feasible
 *   solution), and dual bounds, i.e., a bound
 *   which is better than the best known solution. Primal and dual bounds
 *   are then interpreted as lower or upper bounds according to the
 *   sense of the optimization.
 *
*/
//@{

/*! \return The value of the global lower bound.
 */
      double lowerBound() const;

/*! \return The value of the global upper bound.
 */
      double upperBound() const;

/*! \return The value of the primal bound,
 *   i.e., the \a lowerBound() for a maximization problem and the
 *   \a upperBound() for a minimization problem, respectively.
*/
      double primalBound() const;
#ifdef ABACUS_PARALLEL
/*! \brief This version of the function \a primalBound()
 *   sets the primal bound to \a x
 *   and makes a new entry in the solution history. It is an error if the
 *   primal bound gets worse.
 *
 *   \param x The new value of the primal bound.
*/
      void primalBound(double x, bool broadcast = true);
#else

/*! \brief This version of the function \a primalBound()
 *   sets the primal bound to \a x
 *   and makes a new entry in the solution history. It is an error if the
 *   primal bound gets worse.
 *
 *   \param x The new value of the primal bound.
*/
      void primalBound(double x);
#endif

/*! \return The value of the dual bound,
 *   i.e., the \a upperBound() for a maximization problem and the
 *   \a lowerBound() for a minimization problem, respectively.
 */
      double dualBound() const;

/*! \brief This version of the function \a dualBound()
 *   sets the dual bound to \a x and makes a new entry in the solution history.
 *
 *   It is an error if the
 *   dual bound gets worse.
 *
 *   \param x The new value of the dual bound.
 */
      void dualBound(double x);

/*! \return true If \a x is better than the best known dual bound.
 *
 *   \return false otherwise.
 *
 *   \param x The value being compared with the best know dual bound.
 */
      bool betterDual(double x) const;

/*! Can be used to compare a value with the one of the best known primal bound.
 *
 *   If the objective function values of all feasible solutions are
 *   integer, then we do not have to be so carefully.
 *
 *   \return true If \a x is not better than the best known primal bound,
 *   \return false otherwise.
 *
 *   \param x The value being compared with the primal bound.
 */
      bool primalViolated(double x) const;

/*! Can be used to check if a value is better than the best know primal bound.
 *
 *   \return true If \a x is better than the best known primal bound,
 *   \return false otherwise.
 *
 *   \param x The value compared with the primal bound.
 */
      bool betterPrimal(double x) const;

/*! \brief We use this function ,e.g., to adapt the enumeration strategy
 *   in the \a DiveAndBest-Strategy.
 *
 *   This function is only correct
 *   if any primal bound better than plus/minus infinity corresponds
 *   to a feasible solution.
 *
 *   \return true If a feasible solution of the optimization problem has
 *                   been found.
 *   \return false otherwise.
 */
      bool feasibleFound() const;
//@}

/*! \return The enumeration strategy.
 */
      ENUMSTRAT enumerationStrategy() const;

/*! \brief This version of  the function \a enumerationStrategy()
 *   changes the enumeration strategy.
 *
 *   \param strat The new enumeration strategy.
 */
      void enumerationStrategy(ENUMSTRAT strat);

/*! \brief Analyzes the enumeration strategy set in the parameter file {\tt .abacus}
 *   and calls the corresponding comparison function for the subproblems
 *   \a s1 and \a s2. This function should be redefined for application
 *   specific enumeration strategies.
 *
 *   \return 1 If \a s1 has higher priority than \a s2
 *   \return 0 if \a s2 has higher priority it returns \f$-1\f$, and if both subproblems have equal priority
 *
 *   \param s1 A pointer to subproblem.
 *   \param s2 A pointer to subproblem.
 */
      virtual int enumerationStrategy(const ABA_SUB *s1, const ABA_SUB *s2);

/*! \brief Can be used to check
 *   if the guarantee requirements are fulfilled, i.e., the difference
 *   between upper bound and the lower bound in respect to the lowerBound
 *   is less than 
 *   this guarantee value in percent.
 *
 *   If the lower bound is zero, but the upper bound is nonzero,
 *   we cannot give any guarantee.
 *
 *   \warning A guarantee for a solution can only be given if the
 *   pricing problem is solved exactly or no column generation is performed
 *   at all.
 *
 *   \return true If the guarantee requirements are fulfilled,
 *   \return false otherwise.
 */
      bool guaranteed();

/*! Can be used to access the guarantee which can be given for the best known feasible solution.
 *
 *   It is an error
 *   to call this function if the lower bound is zero, but the upper bound
 *   is nonzero.
 *
 *   \return The guarantee for best known feasible solution in percent.
 */
      double guarantee();

/*! Writes the guarantee nicely formated on the output stream associated with this object.
 *
 *   If no bounds are available, or the lower bound is zero, but the
 *   upper bound is nonzero, then we cannot give any guarantee.
 */      void printGuarantee();

/*! \brief Can be used to control the correctness of the
 *   optimization if the value of the optimum solution has been loaded.
 *
 *   This is done, if a file storing the optimum value is specified with
 *   the parameter {\tt OptimumFileName} in the configuration file
 *   {\tt .abacus}.
 *
 *   \return true If the optimum solution of the problem is known and
 *                   equals the primal bound,
 *   \return false otherwise.
 */
      bool check();

/*! \brief Opens the file 
 *   specified with the parameter {\tt OptimumFileName} in the configuration
 *   file {\tt .abacus} and tries to find a line
 *   with the name of the problem instance (as specified in the
 *   constructor of ABA_MASTER) as first string.
 *
 *   \return true If a line with \a problemName_ has been found,
 *   \return false otherwise.
 *
 *   \param optVal If the return value is \a true, then \a optVal holds the 
 *                     optimum value found in
 *                     the line with the name of the problem instance
 *                     as first string. Otherwise, \a optVal is undefined.
 */
      bool knownOptimum(double &optVal);

/*! Does nothing but can be redefined in derived classes for output before the timing statistics.
*/
      virtual void output();

/*! \return true If \a cutting has been set to \a true in the call of the
 *                   constructor of the class ABA_MASTER, i.e., if cutting
 *                   planes should be generated in the subproblem optimization.
 *   \return false otherwise.
 */
      bool cutting() const;

/*! \return true If \a pricing has been set to true in the call of the
 *                   constructor of the class ABA_MASTER, i.e., if a columns
 *                   should be generated in the subproblem optimization.
 *   \return false otherwise.
 */
      bool pricing() const;

/*! \return A pointer to the object holding the optimization sense of the problem.
 */
      const ABA_OPTSENSE *optSense() const;

/*! \return A pointer to the object storing the solution history of this branch and
 *   cut problem.
 */
      ABA_HISTORY *history() const;

/*! \return A pointer to the set of open subproblems.
 */
      ABA_OPENSUB *openSub() const;

/*! \return A pointer to the default pool storing the constraints of the problem
 *   formulation.
 */
      ABA_STANDARDPOOL<ABA_CONSTRAINT, ABA_VARIABLE> *conPool() const;

/*! \return A pointer to the default pool for the generated cutting planes.
 */
      ABA_STANDARDPOOL<ABA_CONSTRAINT, ABA_VARIABLE> *cutPool() const;

/*! \return A pointer to the default pool storing the variables.
 */
      ABA_STANDARDPOOL<ABA_VARIABLE, ABA_CONSTRAINT> *varPool() const;

/*! Can be used to access the root node of the \bab\ tree.
 *
 *   \return  A pointer to the root node of the enumeration tree.
*/
      ABA_SUB *root() const;

/*! \return A pointer to the root of the remaining \bab\ tree,
 *   i.e., the subproblem which is an ancestor of all open
 *   subproblems and has highest level in the tree.
 */
      ABA_SUB *rRoot() const;

/*! \return The status of the ABA_MASTER.
 */
      STATUS status() const;

/*! \return A pointer to the name of the instance being optimized (as specified in
 *   the constructor of this class).
 */
      const ABA_STRING *problemName() const;

/*! \return A pointer to the timer measuring the total wall clock time.
 */
      const ABA_COWTIMER *totalCowTime() const;

/*! True, if an approximative solver should be used
 */
      inline bool solveApprox() const;

/*! \return A pointer to the timer measuring the total cpu time for the
 *   optimization.
 */
      const ABA_CPUTIMER *totalTime() const;

/*! \return A pointer to the timer measuring the cpu time spent in members of
 *   the LP-interface.
 */
      const ABA_CPUTIMER *lpTime() const;

/*! \return A pointer to the timer measuring  the cpu time required by the LP solver.
 */
      const ABA_CPUTIMER *lpSolverTime() const;

/*! \return A pointer to the timer measuring the cpu time spent in the
 *   separation of cutting planes.
 */
      const ABA_CPUTIMER *separationTime() const;

/*! \return A pointer to the timer measuring
 *   the cpu time spent in the heuristics for the computation of feasible
 *   solutions.
 */
      const ABA_CPUTIMER *improveTime() const;

/*! \return A pointer to the timer measuring the cpu time spent in pricing.
 */
      const ABA_CPUTIMER *pricingTime() const;

/*! \return A pointer to the timer measuring the cpu time spent in finding and
 *   selecting the branching rules.
 */
      const ABA_CPUTIMER *branchingTime() const;

/*! \return The number of generated subproblems.
 */
      int nSub() const;

/*! \return The number of optimized linear programs (only LP-relaxations).
 */
      int nLp() const;

/*! \return The highest level in the tree which has been reached during the implicit
 *   enumeration.
 */
      int highestLevel() const;

/*! \return The number of root changes of the remaining \bac\ tree.
 */
      int nNewRoot() const;

/*! \return The number of subproblems which have already
 *   been selected from the set of open subproblems.
 */
      int nSubSelected() const;

/*! \brief Writes all parameters of the class
 *   ABA_MASTER together
 *   with their values to the global output stream.
*/
      void printParameters();

/*! \return The branching strategy.
 */
      BRANCHINGSTRAT branchingStrategy() const;

/*! Changes the branching strategy.
 *
 *   \param strat The new branching strategy.
 */
      void branchingStrategy(BRANCHINGSTRAT strat);

/*! \return The Lp Solver.
 */
      OSISOLVER defaultLpSolver() const;

/*! Changes the default Lp solver.
 *
 *   \param osiSolver The new solver.
 */
      void defaultLpSolver(OSISOLVER osiSolver);


      ABA_LPMASTEROSI *lpMasterOsi() const 
        { return lpMasterOsi_; }

/*!  \return The number of variables that should be tested for the selection of the
 *   branching variable.
 */     
      int nBranchingVariableCandidates() const;

/*! \brief This version of the function \a nbranchingVariableCandidates()
 *   sets the number of tested branching variable candidates.
 *
 *   \param n The new value of the number of tested variables for 
 *                becoming branching variable.
 */
      void nBranchingVariableCandidates(int n);

/*! The guarantee specification for the optimization.
 */
      double requiredGuarantee() const;

/*! \brief This version of the function \a requiredGuarantee()
 *   changes the guarantee specification.
 *
 *   \param g The new guarantee specification (in percent). 
 *                This must be a nonnative
 *                value. Note, if the guarantee specification is changed
 *                after a single node of the enumeration tree has been 
 *                fathomed, then the overall guarantee might differ from
 *                the new value.
 */
      void requiredGuarantee(double g);

/*! \return The maximal depth up to
 *   which the enumeration should be performed.
 *   By default the maximal enumeration depth is \a INT\_MAX. 
 */
      int maxLevel() const;

/*! \brief This version of the function \a maxLevel()
 *   changes the maximal enumeration depth.
 *
 *   If it is set to 1
 *   the \bac\ algorithm becomes a pure cutting plane algorithm.
 *
 *   \param max The new value of the maximal enumeration level.
 */
      void maxLevel(int ml);

/*! \return The maximal cpu time which can be used by the optimization.
 */
      const ABA_STRING& maxCpuTime() const;

/*! Sets the maximal usable cpu time for the optimization.
 *
 *   \param t The new value of the maximal cpu time in the form
 *    {\tt "hh:mm:ss"}.
 */    
      void maxCpuTime(const ABA_STRING &t);

/*! \brief The function \a maxCowTime().
 *
 *   \return The maximal wall-clock time for the optimization.
 */
      const ABA_STRING& maxCowTime() const;

/*! \brief This version of the  function \a maxCowTime()
 *   set the maximal wall-clock time for the optimization.
 *
 *   \param t The new value of the maximal wall-clock time
 *                in the form {\tt hh:mm:ss}.
 */
      void maxCowTime(const ABA_STRING &t);

/*! \return true Then we assume that all feasible solutions have integral
 *                   objective function values,
 *   \return false otherwise.
 */
      bool objInteger() const;

/*! \brief This version of function \a objInteger()
 *   sets the assumption
 *   that the objective function values of all feasible solutions are
 *   integer.
 *
 *   \param b The new value of the assumption.
 */
      void objInteger(bool b);

/*! \brief The function \a tailOffNLp().
 *
 *   \return The number of linear programs considered in the tailing off analysis.
 */
      int tailOffNLp() const;

/*! Sets the number of linear programs considered in the tailing off analysis.
 *
 *   This new value is only
 *   relevant for subproblems activated {\bf after} the change of this value.
 *
 *   \param n The new number of LPs for the tailing off analysis.
 */
      void tailOffNLp(int n);

/*! \brief The function \a tailOffPercent().
 *
 *   \return The minimal change of the dual bound for the tailing off analysis in percent.
 */
      double tailOffPercent() const;

/*! \brief This version of the function \a tailOffPercent()
 *   sets the minimal change of the dual bound for the tailing off analysis.
 *
 *   This change is only
 *   relevant for subproblems activated {\bf after} calling this function.
 *
 *   \param p The new value for the tailing off analysis.
 */
      void tailOffPercent(double p);

/*! \return The output mode.
 */
      OUTLEVEL outLevel() const;

/*! \brief The version of the function \a outLevel() sets the output mode.
 *
 *   \param mode The new value of the output mode.
 */
      void outLevel(OUTLEVEL mode);

/*! \return The output mode for the log-file.
 */
      OUTLEVEL logLevel() const;

/*! \brief This version of the function \a logLevel()
 *   sets the output mode for the log-file.
 *
 *   \param mode The new value of the output mode.
 */
      void logLevel(OUTLEVEL mode);

/*! \return true If the number of optimizations \a nOpt of a subproblem
 *                   exceeds the delayed branching threshold,
 *   \return false otherwise.
 *
 *   \param nOpt The number of optimizations of a subproblem.
 */
      bool delayedBranching(int nOpt_) const;

/*! \brief Sets the number of
 *   optimizations of a subproblem until sons are created in
 *   ABA_SUB::branching().
 *
 *   If this value is 0, then a branching step is performed at the
 *   end of the subproblem optimization as usually if the subproblem
 *   can be fathomed. Otherwise, if this value is strictly positive,
 *   the subproblem is put back for a later optimization. This can be
 *   advantageous if in the meantime good cutting planes or primal bounds
 *   can be generated. The number of times the subproblem is put back
 *   without branching is indicated by this value.
 *
 *   \param threshold The new value of the delayed branching threshold.
 */
      void dbThreshold(int threshold);

/*! \return The number of optimizations
 *   of a subproblem until sons are created. For further detatails we
 *   refer to \a dbThreshold(int).
 */
      int dbThreshold() const;

/*! \return The maximal number of rounds, i.e., number of subproblem optimizations,
 *   a subproblem is dormant, i.e., it is not selected from the set
 *   of open subproblem if its status is \a Dormant, if possible.
 */
      int minDormantRounds() const;

/*! Sets the number of rounds a subproblem should stay dormant.
 *
 *   \param nRounds The new minimal number of dormant rounds.
 */
      void minDormantRounds(int nRounds);

/*! \return The mode of the primal bound initialization.
 */
      PRIMALBOUNDMODE pbMode() const;

/*! Sets the mode of the primal bound initialization.
 *
 *   \param mode The new mode of the primal bound initialization.
 */
      void pbMode(PRIMALBOUNDMODE mode);

/*! \return The number of linear programs
 *   being solved between two additional pricing steps.
 *   If no additional pricing steps should be
 *   executed this parameter has to be set to 0.
 *   The default value of the pricing frequency is 0. This parameter
 *   does not influence the execution of pricing steps which are
 *   required for the correctness of the algorithm. 
 */
      int pricingFreq () const;

/*! \brief This version of the function \a pricingFreq()
 *   sets the number of linear programs
 *   being solved between two additional pricing steps.
 *
 *   \param f The pricing frequency.
 */
      void pricingFreq(int f);

/*! \return The frequency of subproblems in which constraints or variables should be
 *   generated.
 */
      int skipFactor() const;

/*! \brief This version of the function \a skipFactor()
 *   sets the frequency for constraint and variable generation.
 *
 *   \param f The new value of the frequency.
 */
      void skipFactor(int f);

/*! \fn void ABA_MASTER::skippingMode(SKIPPINGMODE mode)
 * \brief This version of the function \a skippingMode()
 *   sets the skipping strategy.
 *
 *   \param mode The new skipping strategy.
 */
      void skippingMode(SKIPPINGMODE mode);
    
/*! \return The skipping strategy.
 */
      SKIPPINGMODE skippingMode() const;

/*! \return The mode for the elimination of constraints.
 */
      CONELIMMODE conElimMode() const;

/*! Changes the constraint elimination mode.
 *
 *   \param mode The new constraint elimination mode.
 */
      void conElimMode(CONELIMMODE mode);

/*! \return The mode for the elimination of variables.
 */
      VARELIMMODE varElimMode() const;

/*! Changes the variable elimination mode.
 *
 *   \param mode The new variable elimination mode.
 */
      void varElimMode(VARELIMMODE mode);

/*! \return The zero tolerance for the elimination
 *   of constraints by the slack criterion.
 */
      double conElimEps() const;

/*! Changes the tolerance for the elimination of constraints by the slack criterion.
 *
 *   \param eps The new tolerance.
 */
      void conElimEps(double eps);

/*! \return The zero tolerance for the elimination of variables by the reduced cost
 *   criterion.
 */
      double varElimEps() const;


/*! Changes the tolerance for the elimination 
 *   of variables by the reduced cost criterion.
 *
 *   \param eps The new tolerance.
 */ 
      void varElimEps(double eps);
 
/*! \return The age for the elimination of variables by the reduced cost
 *   criterion.
 */
      int varElimAge() const;
    
/*! Changes the age for the elimination of variables by the reduced cost criterion.
 *
 *   \param eps The new age.
 */
      void varElimAge(int eps);

/*! \return The age for the elimination of constraints.
 */
      int conElimAge() const;

/*! Changes the age for the elimination of constraints.
 *
 *   \param eps The new age.
 */
      void conElimAge(int eps);

/*! \return true Then variables are fixed and set by reduced cost criteria.
 *   \return false Then no variables are fixed or set by reduced cost criteria.
 */
      bool fixSetByRedCost() const;

/*! Turns fixing and setting variables by reduced cost on or off.
 *
 *   \param on If \a true, then variable fixing and setting by reduced
 *                 cost is turned on. Otherwise it is turned of.
*/
      void fixSetByRedCost(bool on);

/*! \return true Then the linear program is output every iteration of the
 *                   subproblem optimization.
 *   \return false The linear program is not output.
 */
      bool printLP() const;

/*! Turns the output of the linear program in  every iteration on or off.
 *
 *   \param on If \a true, then the linear program is output,
 *                 otherwise it is not output.
 */
      void printLP(bool on);

/*! \return The maximal number of constraints
 *   which should be added in every iteration of the cutting plane algorithm.
 */
      int maxConAdd() const;

/*! \brief Sets the maximal number of constraints
 *   that are added in an iteration of the cutting plane algorithm.
 *
 *   \param max The maximal number of constraints.
 */
      void maxConAdd(int max);

/*! \return The size of the buffer for
 *   generated constraints in the cutting plane algorithm.
 */
      int maxConBuffered() const;

/*! \brief Changes the maximal number of
 *   constraints that are buffered in an iteration of the cutting plane
 *   algorithm.
 *
 *   \note This function changes only the default value for subproblems
 *   that are activated after its call. 
 *
 *   \param max The new maximal number of buffered constraints.
 */
      void maxConBuffered(int max);

/*! \return The maximal number of variables which
 *   should be added in the column generation algorithm.
*/
      int maxVarAdd() const;

/*! \brief Changes the maximal number of variables that
 *   are added in an iteration of the subproblem optimization.
 *
 *   \param max The new maximal number of added variables.
 */
      void maxVarAdd(int max);

/*! \return The size of the buffer for the
 *   variables generated in the column generation algorithm.
 */
      int maxVarBuffered() const;

/*! \brief Changes the maximal number of
 *   variables that are buffered in an iteration of the subproblem optimization.
 *
 *   \note This function changes only the default value for subproblems
 *   that are activated after its call. 
 *
 *   \param max The new maximal number of buffered variables.
 */
      void maxVarBuffered(int max);

/*! \return The maximal number of iterations per subproblem optimization
 *   (-1 means no iteration limit).
 */
      int maxIterations() const;

/*! \brief Changes the default value for the
 *   maximal number of iterations of the optimization of a subproblem.
 *
 *   \note This function changes only this value for subproblems that 
 *   are constructed after this function call. For already constructed
 *   objects the value can be changed with the function 
 *   ABA_SUB::maxIterations().
 *
 *   \param max The new maximal number of iterations of the subproblem
 *                  optimization (-1 means no limit).
 */
      void maxIterations(int max);

/*! \return true Then we try to eliminate fixed and set variables from the
 *                   linear program.
 *   \return false Fixed or set variables are not eliminated.
 */
      bool eliminateFixedSet() const;

/*! \brief This version of the function \a eliminateFixedSet()
 *   can be used to turn the elimination of fixed and set variables on or off.
 *
 *   \param turnOn The elimination is turned on if \a turnOn is \a true,
 *                     otherwise it is turned off.
 */
      void eliminateFixedSet(bool turnOn);

/*! \return true Then a new root of the remaining \bab\ tree is reoptimized
 *                    such that the associated
 *                    reduced costs can be used for the fixing of variables.
 *   \return false A new root is not reoptimized.
 */
      bool newRootReOptimize() const;

/*! Turns the reoptimization of new root nodes of the remaining branch and bound tree on or off.
 *
 *   \param on If \a true, new root nodes are reoptimized.
 */
      void newRootReOptimize(bool on);

/*! \return The name of the file that stores the optimum solutions.
 */
      const ABA_STRING &optimumFileName() const;

/*! Changes the name of the file in which the value of the optimum solution is searched.
 *
 *   \param name The new name of the file.
*/
      void optimumFileName(const char *name);

/*! \return true Then the average distance of the fractional solution
 *                   from all added cutting planes is output every iteration
 *                   of the subproblem optimization.
 *   \return false The average cut distance is not output.
 */
      bool showAverageCutDistance() const;

/*! \brief Turns the output of the average
 *   distance of the added cuts from the fractional solution on or off.
 *
 *   \param on If \a true the output is turned on, otherwise it is
 *                 turned off.
 */
      void showAverageCutDistance(bool on);

/*! \return The mode of output for the Vbc-Tool.
 */
      VBCMODE vbcLog() const;

/*! Changes the mode of output for the Vbc-Tool.
 *  
 *   This function should only be called before the optimization is
 *   started with the function ABA_MASTER::optimize().
 *
 *   \param mode The new mode.
 */
      void vbcLog(VBCMODE mode);

/*! Set solver specific parameters.
 *  The default does nothing.
 *  \return true if an error has occured
 *  \false otherwise
 */
      virtual bool setSolverParameters(OsiSolverInterface* interface, bool solverIsApprox);

#ifdef ABACUS_PARALLEL
     ABA_PARMASTER *parmaster() const { return parmaster_; }
#endif
    protected:

/*! Sets up the default pools for variables, constraints, and cutting planes.
 *
 *   \param constraints The constraints of the problem formulation
 *                          are inserted in the constraint pool. The size
 *                          of the constraint pool equals the number of
 *                          \a constraints.
 *   \param variables The variables of the problem formulation are
 *                        inserted in the variable pool.
 *   \param varPoolSize The size of the pool for the variables. If
 *                          more variables are added the variable pool
 *                          is automatically reallocated.
 *   \param cutPoolSize The size of the pool for cutting planes.
 *   \param dynamicCutPool If this argument is true, then the cut
 *                             is automatically reallocated if more
 *                             constraints are inserted than \a cutPoolSize.
 *                             Otherwise, non-active constraints are removed
 *                             if the pool becomes full.
 *                             The default value is false.
 */ 
      virtual void initializePools(ABA_BUFFER<ABA_CONSTRAINT*> &constraints,
                                   ABA_BUFFER<ABA_VARIABLE*> &Variables,
                                   int varPoolSize,
                                   int cutPoolSize,
                                   bool dynamicCutPool = false);

/*! \brief Is overloaded such that
 *   also a first set of cutting planes can be inserted into the cutting 
 *   plane pool.
 *
 *   \param constraints The constraints of the problem formulation
 *                          are inserted in the constraint pool. The size
 *                          of the constraint pool equals the number of
 *                          \a constraints.
 *   \param cuts The constraints that are inserted in the cutting
 *                   plane pool. The number of constraints in the buffer
 *                   must be less or equal than the size of the cutting
 *                   plane pool \a cutPoolSize.
 *   \param variables The variables of the problem formulation are
 *                        inserted in the variable pool.
 *   \param varPoolSize The size of the pool for the variables. If
 *                          more variables are added the variable pool
 *                          is automatically reallocated.
 *   \param cutPoolSize The size of the pool for cutting planes.
 *   \param dynamicCutPool If this argument is true, then the cut
 *                             is automatically reallocated if more
 *                             constraints are inserted than \a cutPoolSize.
 *                             Otherwise, non-active constraints are removed
 *                             if the pool becomes full.
 *                             The default value is false.
 */
      virtual void initializePools(ABA_BUFFER<ABA_CONSTRAINT*> &constraints,
                                   ABA_BUFFER<ABA_CONSTRAINT*> &cuts,
                                   ABA_BUFFER<ABA_VARIABLE*> &Variables,
                                   int varPoolSize,
                                   int cutPoolSize,
                                   bool dynamicCutPool = false);

/*! \brief Can be used to initialize the sense
 *   of the optimization in derived classes, if this has not been already 
 *   performed when the constructor of ABA_MASTER has been called.
 *
 *   \param sense The sense of the optimization (ABA_OPTSENSE::Min or
 *                   ABA_OPTSENSE::Max).
 */
      void initializeOptSense(ABA_OPTSENSE::SENSE sense);

/*! Implements the best first search enumeration.
 *
 *   If the bounds of both subproblems are equal, then
 *   the subproblems are compared with the function \a equalSubCompare().
 *
 *   \return -1 If subproblem \a s1 has a worse dual bound than \a s2,
 *                 i.e., if it has a smaller dual bound for minimization or
 *                 a larger dual bound for maximization problems.
 *   \return 1 If subproblem \a s2 has a worse dual bound than \a s1.
 *   \return 0 If both subproblems have the same priority in the
 *                enumeration strategy.
 *
 *   \param s1 A subproblem.
 *   \param s2 A subproblem.
 */
      int bestFirstSearch(const ABA_SUB* s1, const ABA_SUB* s2) const;

/*! \brief Is called from the function
 *   \a bestFirstSearch() and from the function \a depthFirstSearch()
 *   if the subproblems \a s1 and \a s2 have the same priority.
 *
 *   If both subproblems were generated by setting a binary variable, then
 *   that subproblem has higher priority of which the branching variable is
 *   set to upper bound.
 *
 *   This function can be redefined to resolve equal subproblems according
 *   to problem specific criteria.
 *   As the root node is compared with itself and has no branching rule,
 *   we have to insert the first line of this function.
 *
 *   \param s1 A subproblem.
 *   \param s2 A subproblem.
 *
 *   \return 0 If both subproblems were not generated by setting a
 *                variable, or the branching variable of both subproblems
 *                is set to the same bound.
 *   \return 1 If the branching variable of the first subproblem
 *              ist set to the upper bound.
 *   \return -1 If the branching variable of the second subproblem
 *              ist set to the upper bound.
 */
      virtual int equalSubCompare(const ABA_SUB *s1, const ABA_SUB *s2) const;

/*! \brief Implements the depth first search
 *   enumeration strategy, i.e., the subproblem with maximum \a level
 *   is selected.
 *
 *   If the level of both subproblems are equal, then
 *   the subproblems are compared with the function \a equalSubCompare().
 *
 *   \return -1 If subproblem \a s1 has higher priority,
 *   \return 0 if both subproblems have equal priority,
 *   \return 1 otherwise.
 *
 *   \param s1 The first subproblem.
 *   \param s2 The second subproblem.
 */
      int depthFirstSearch(const ABA_SUB* s1, const ABA_SUB* s2) const;

/*! \brief Implements the breadth first search
 *   enumeration strategy, i.e., the subproblem with minimum \a level
 *   is selected.
 *
 *   If both subproblems have the same \a level, the smaller
 *   one is the one which has been generated earlier, i.e., the one with
 *   the smaller \a id.
 *
 *   \return -1 If subproblem \a s1 has higher priority,
 *   \return 0 if both subproblems have equal priority,
 *   \return 1 otherwise.
 *
 *   \param s1 The first subproblem.
 *   \param s2 The second subproblem.
 */
      int breadthFirstSearch(const ABA_SUB* s1, const ABA_SUB* s2) const;


/*! \brief Performs depth-first search until a
 *   feasible solution is found, then the search process is continued
 *   with best-first search.
 *
 *   \return -1 If subproblem \a s1 has higher priority,
 *   \return 0 if both subproblems have equal priority,
 *   \return 1 otherwise.
 *
 *   \param s1 The first subproblem.
 *   \param s2 The second subproblem.
 */
      int diveAndBestFirstSearch(const ABA_SUB *s1, const ABA_SUB* s2) const;

/*! \brief Is only a dummy. This
 *   function can be used to initialize parameters of derived classes
 *   and to overwrite parameters read from the file {\tt .abacus} by the
 *   function \a \_initializeParameters().
*/
      virtual void initializeParameters();

/*! \return Should return a pointer
 *  to the first subproblem of the optimization, i.e., the root node
 *  of the enumeration tree. This is a pure virtual function since
 *  a pointer to a problem specific subproblem should be returned,
 *  which is derived from the class ABA_SUB.
 */
      virtual ABA_SUB *firstSub() = 0;

/*! \brief The default implementation of \a initializeOptimization()
 *   does nothing.
 *
 *   This
 *   virtual function can be used as an entrance point to perform
 *   some initializations after \a optimize() is called.
*/
      virtual void initializeOptimization();

/*! \brief The default implementation of \a terminateOptimization()
 *   does nothing.
 *
 *   This
 *   virtual function can be used as an entrance point 
 *   after the optimization process is finished.
 */
      virtual void terminateOptimization();
      
#ifdef ABACUS_PARALLEL
     ABA_PARMASTER *parmaster_;
#endif

    private:

/*! \brief Reads the parameter-file
 *   {\tt .abacus}, which is searched in the directory given by the
 *   environment variable ABACUS_DIR, and calls the  virtual function \a initializeParameters()
 *   which can initialize parameters of derived classes and overwrite
 *   parameters of this class.
 *
 *   All parameters are first inserted together
 *   with their values in a parameter table in the function \a readParameters().
 *   If the virtual dummy function \a initializeParameters() is redefined
 *   in a derived class and also reads a parameter file with the function
 *   \a readParameters(), then already inserted parameters can be overwritten.
 *
 *   \par
 *   After all parameters are input we extract with the function 
 *   \a assignParameter()
 *   all parameters. Problem specific parameters should be extracted in
 *   a redefined version of \a initializeParameters().
 *   extracted from this table 
 */
      void _initializeParameters();
      void _createLpMasters();
      void _deleteLpMasters();
      void _initializeLpParameters();

/*! \brief Initializes the LP solver
 *   specific default Parameters if they are not read from the parameter-file
 *   {\tt .abacus}.
 *
 *   This function is implemented in the file \a lpif.cc.
 */
      void _setDefaultLpParameters();

/*! Prints the LP solver specific parameters.
 *
 *   This function is implemented in the file \a lpif.cc.
 */
      void _printLpParameters();

/*! Prints the LP solver specific statistics.
   *
   *   This function is implemented in the file \a lpif.cc.
   */
      void _outputLpStatistics();

/*! Returns a pointer to an open subproblem for further processing.
 *
 *   If the set of open subproblems is empty or
 *   one of the criteria for early termination of the optimization
 *   (maximal cpu time, maximal elapsed time, guarantee) is
 *   fulfilled 0 is returned.
 */
      ABA_SUB   *select();
      int initLP();

/*! Writes the string \a info to the stream associated with the Tree Interface.
 *
 *   A \$ is preceded if
 *   the output is written to standard out for further pipelining.
 *   If \a time is true a time string is written in front of the
 *   information. The default value of \a time is \a true.
*/
      void writeTreeInterface(const char *info, bool time = true) const;

/*! \brief Adds the subproblem \a sub to the
 *   stream storing information for graphical output of the enumeration
 *   tree if this logging is turned on.
 */
      void treeInterfaceNewNode(ABA_SUB *sub) const;
#ifdef ABACUS_PARALLEL

/*! \brief Adds the subproblem \a sub to the
 *   stream storing information for graphical output of the enumeration
 *   tree if this logging is turned on.
 */
      void treeInterfaceNewNode(int fatherId, int id) const;
      mutable ACE_Thread_Mutex treeInterfaceMutex_;
#endif

/*! Assigns the \a color to the subproblem \a sub in the Tree Interface.
*/
      void treeInterfacePaintNode(int id, int color) const;

/*! Passes the new lower bound \a lb to the Tree Interface.
*/
      void treeInterfaceLowerBound(double lb) const;

/*! Passes the new upper bound \a ub to the Tree Interface.
*/
      void treeInterfaceUpperBound(double ub) const;

/*! \brief Updates the node information
 *   in the node with number \a id by writing the lower bound \a lb
 *   and the upper bound \a ub to the node.
*/
      void treeInterfaceNodeBounds(int id, double lb, double ub);

/*! Registers a new subproblem which is on level \a level in enumeration tree.
 *
 *   It is called each time a new
 *   subproblem is generated.
 */
      void newSub(int level);
 
/*! \brief Increments the counter for linear programs and should be called in each optimization call of the LP-relaxation.
*/
      void countLp();

/*! \brief Increments the counter of the number of fixed variables by \a n.
*/
      void newFixed(int n);

/*! \brief Increments the counter for the total number of added constraints by \a n.
*/
      void addCons(int n);

/*! \brief Increments the counter for the total number of removed constraints by \a n.
*/
      void removeCons(int n);

/*! \brief Increments the counter for the total number of added variables by \a n.
*/
      void addVars(int n);

/*! \brief Increments the counter for the total number of removed variables by \a n.
*/
      void removeVars(int n);

/*! returns a pointer to the object storing the variables which are candidates for being fixed.
 */
      ABA_FIXCAND *fixCand() const;

/*! Sets the root of the remaining \bac\ tree to \a newRoot.
 *
 *   If \a reoptimize is \a true a reoptimization of the
 *   subproblem \a *newRoot is performed.
 *   This is controlled via a function argument since it might not be
 *   desirable when we find a new \a rRoot_ during the fathoming
 *   of a complete subtree ABA_SUB::FathomTheSubtree().
 */
      void rRoot(ABA_SUB *newRoot, bool reoptimize);

/*! This version of the function \a status() sets the status of the ABA_MASTER.
 */
      void status(STATUS stat);

/*! Updates the final dual bound of the root node.
 *
 *   This function should be only called at the end of the root node
 *   optimization.
 */
      void rootDualBound(double x);
      void theFuture();

      /*! The name of the optimized problem.
       */
      ABA_STRING      problemName_;
      bool readParamFromFile_;

      /*! The sense of the objective function.
       */
      ABA_OPTSENSE    optSense_;

      /*! The root node of the enumeration tree.
       */
      ABA_SUB        *root_;

      /*! The root node of the remaining enumeration tree.
       */
      ABA_SUB        *rRoot_;

      /*! The set of open subproblems.
       */
      ABA_OPENSUB    *openSub_;

      /*! The solution history.
       */
      ABA_HISTORY    *history_;

      /*! The enumeration strategy.
       */
      ENUMSTRAT   enumerationStrategy_;

      /*! The branching strategy.
       */
      BRANCHINGSTRAT branchingStrategy_;

      /*! \brief The number of candidates
       *  that are evaluated for branching on variables.
      */
      int            nBranchingVariableCandidates_;

      /*! The default LP-Solver.
       */
      OSISOLVER       defaultLpSolver_;

      ABA_LPMASTEROSI *lpMasterOsi_;

      /*! The default pool with the constraints of the problem formulation.
       */
      ABA_STANDARDPOOL<ABA_CONSTRAINT, ABA_VARIABLE> *conPool_;


      /*! The default pool of dynamically generated constraints.
       */
      ABA_STANDARDPOOL<ABA_CONSTRAINT, ABA_VARIABLE> *cutPool_;

      /*! The default pool with the variables of the problem formulation.
      */
      ABA_STANDARDPOOL<ABA_VARIABLE, ABA_CONSTRAINT> *varPool_;

      /*! The best known primal bound.
       */
      double           primalBound_;

      /*! The best known dual bound.
       */
      double           dualBound_;

      /*! The best known dual bound at the end of the optimization of the root node.
       */
      double            rootDualBound_;

      /*! The variables which are candidates for being fixed.
       */
      ABA_FIXCAND         *fixCand_;

      /*! If \a true, then constraints are generated in the optimization.
       */
      bool             cutting_;

      /*! If \a true, then variables are generated in the optimization.
       */
      bool             pricing_;

      /*! If \a true, then an approximative solver is used to solve
       * linear programs
       */
      bool             solveApprox_;

      /*! \brief The number of subproblems already selected from the list of
       *  open subproblems.
       */
      int              nSubSelected_;

      /*! \brief Ouput for the Tree Interface is generated
       *         depending on the value of this variable.
       */
      VBCMODE          VbcLog_;

      /*! A pointer to the log stream for the VBC-Tool.
       */
      ostream         *treeStream_;

      /*! \brief The guarantee in percent which should be 
       * reached when the optimization stops.
       *     
       * If this value is \f$0.0\f$, then the optimum solution is determined.
       */
      double      requiredGuarantee_;

      /*! The maximal level in enumeration tree.
       *
       *  Up to this level subproblems are considered in the enumeration.
       */
      int         maxLevel_;

      /*! The maximal available cpu time.
       */
      ABA_STRING      maxCpuTime_;

      /*! The maximal available wall-clock time.
       */
      ABA_STRING      maxCowTime_;

      /*! \brief \a true, if all objective function values of feasible
       *         solutions are assumed to be integer.
       */
      bool        objInteger_;

      /*! The number of LP-iterations for the tailing off analysis.
       */
      int         tailOffNLp_;

      /*! The minimal change of the LP-value on the tailing off analysis.
       */
      double      tailOffPercent_;

      /*! The number of optimizations of an ABA_SUB until branching is performed.
      */
      int         dbThreshold_;

      /*! \brief The minimal number of rounds, i.e., number of subproblem optimizations,
       *  a subproblem is dormant, i.e., it is not selected from the set
       *  of open subproblem if its status is \a Dormant, if possible.
       */
      int         minDormantRounds_;

      /*! The output mode.
       */
      OUTLEVEL    outLevel_;

      /*! The amount of output written to the log file.
       */
      OUTLEVEL    logLevel_;

      /*! The mode of the primal bound initialization.
       */
      PRIMALBOUNDMODE pbMode_;

      /*! The number of solved LPs between two additional pricing steps.
       */
      int         pricingFreq_;

      /*! \brief The frequency constraints or variables are generated depending on the
       *  skipping mode.
       */
      int         skipFactor_;

      /*! \brief Either constraints are generated only every \a skipFactor_ subproblem
       *  (\a SkipByNode) only every \a skipFactor_ level (\a SkipByLevel).
       */
      SKIPPINGMODE skippingMode_;

      /*! If \a true, then variables are fixed and set by reduced cost criteria.
       */
      bool             fixSetByRedCost_;

      /*! If \a true, then the linear program is output every iteration.
       */
      bool             printLP_;

      /*! \brief The maximal number of added constraints per iteration of the
       *  cutting plane algorithm.
       */
      int              maxConAdd_;

      /*! The size of the buffer for generated cutting planes.
       */
      int              maxConBuffered_;

      /*! \brief The maximal number of added variables per iteration of the column
       *  generation algorithm.
       */
      int              maxVarAdd_;

      /*! The size of the buffer for generated variables.
       */
      int              maxVarBuffered_;

      /*! \brief The maximal number of iterations of the cutting plane/column generation
       *  algorithm in the subproblem.
       */
      int              maxIterations_;

      /*! If \a true, then nonbasic fixed and set variables are eliminated.
       */
      bool             eliminateFixedSet_;

      /*! \brief If \a true, then an already earlier processed node is reoptimized
       *  if it becomes the new root of the remaining \bab\ tree.
       */
      bool             newRootReOptimize_;

      /*! \brief The name of a file storing a list of optimum solutions of
       *  problem instances.
       */
      ABA_STRING       optimumFileName_;

      /*! \brief If \a true then the average distance of the added cutting planes
       *  is output every iteration of the cutting plane algorithm.
       */
      bool             showAverageCutDistance_;

      /*! \brief The way constraints are automatically eliminated in the cutting
       *  plane algorithm.
       */
      CONELIMMODE      conElimMode_;

      /*! \brief The way variables are automatically eliminated in the column
       *  generation algorithm.
       */
      VARELIMMODE      varElimMode_;

      /*! \brief The tolerance for the elimination of constraints by the
       *  mode \a NonBinding/
       */
      double           conElimEps_;

      /*! \brief The tolerance for the elimination of variables by the
       *  mode \a ReducedCost.
       */
      double           varElimEps_;

      /*! \brief The number of iterations an elimination criterion must be
       *  satisfied until a constraint can be removed.
       */
      int              conElimAge_;

      /*! \brief The number of iterations an elimination criterion must be
       *  satisfied until a variable can be removed.
       */
      int              varElimAge_;

      /*! The current status of the optimization.
       */
      STATUS           status_;

      /*! The timer for the total elapsed time.
       */
      ABA_COWTIMER  totalCowTime_;

      /*! The timer for the total cpu time for the optimization.
       */
      ABA_CPUTIMER totalTime_;

      /*! The timer for the cpu time spent in the LP-interface.
       */
      ABA_CPUTIMER lpTime_;
      ABA_CPUTIMER lpSolverTime_;

      /*! The timer for the cpu time spent in the separation
       */
      ABA_CPUTIMER separationTime_;

      /*! \brief The timer for the cpu time spent in the heuristics
       *         for the computation of feasible solutions.
       */
      ABA_CPUTIMER improveTime_;

      /*! The timer for the cpu time spent in pricing.
       */
      ABA_CPUTIMER pricingTime_;

      /*! The timer for the cpu time spent in determining the branching rules.
       */
      ABA_CPUTIMER branchingTime_;

      /*! The number of generated subproblems.
       */
      int nSub_;

      /*! The number of solved LPs.
       */
      int nLp_;

      /*! The highest level which has been reached in the enumeration tree.
       */
      int highestLevel_;

      /*! The total number of fixed variables.
       */
      int nFixed_;

      /*! The total number of added constraints.
       */
      int nAddCons_;

      /*! The total number of removed constraints.
       */
      int nRemCons_;

      /*! The total number of added variables.
       */
      int nAddVars_;

      /*! The total number of removed variables.
       */
      int nRemVars_;

      /*! The number of changes of the root of the remaining \bab\ tree.
       */
      int nNewRoot_;
      ABA_MASTER(const ABA_MASTER &rhs);
      const ABA_MASTER &operator=(const ABA_MASTER& rhs);
  };


inline double ABA_MASTER::lowerBound() const
  {
    if (optSense_.max()) return primalBound_;
    else                 return dualBound_;
  }

inline double ABA_MASTER::upperBound() const
  {
    if (optSense_.max()) return dualBound_;
    else                 return primalBound_;
  }

inline double ABA_MASTER::primalBound() const
  {
    return primalBound_;
  }

inline double ABA_MASTER::dualBound() const
  {
    return dualBound_;
  }

inline ABA_SUB* ABA_MASTER::root() const
  {
    return root_;
  }

inline ABA_SUB* ABA_MASTER::rRoot() const
  {
    return rRoot_;
  }

inline const ABA_OPTSENSE *ABA_MASTER::optSense() const
  {
    return &optSense_;
  }

inline ABA_HISTORY *ABA_MASTER::history() const
  {
    return history_;
  }

inline ABA_OPENSUB *ABA_MASTER::openSub() const
  {
    return openSub_;
  }

inline ABA_FIXCAND *ABA_MASTER::fixCand() const
  {
    return fixCand_;
  }

inline ABA_STANDARDPOOL<ABA_CONSTRAINT, ABA_VARIABLE> *ABA_MASTER::conPool() const
  {
    return conPool_;
  }

inline ABA_STANDARDPOOL<ABA_CONSTRAINT, ABA_VARIABLE> *ABA_MASTER::cutPool() const
  {
    return cutPool_;
  }

inline ABA_STANDARDPOOL<ABA_VARIABLE, ABA_CONSTRAINT> *ABA_MASTER::varPool() const
  {
    return varPool_;
  }

inline bool ABA_MASTER::cutting() const
  {
    return cutting_;
  }

inline bool ABA_MASTER::pricing() const
  {
    return pricing_;
  }
inline bool ABA_MASTER::solveApprox() const
  {
    return solveApprox_;
  }

inline const ABA_COWTIMER *ABA_MASTER::totalCowTime() const
  {
    return &totalCowTime_;
  }

inline const ABA_CPUTIMER *ABA_MASTER::totalTime() const
  {
    return &totalTime_;
  }

inline const ABA_CPUTIMER *ABA_MASTER::lpTime() const
  {
    return &lpTime_;
  }

inline const ABA_CPUTIMER *ABA_MASTER::lpSolverTime() const
  {
    return &lpSolverTime_;
  }

inline const ABA_CPUTIMER *ABA_MASTER::separationTime() const
  {
    return &separationTime_;
  }

inline const ABA_CPUTIMER *ABA_MASTER::improveTime() const
  {
    return &improveTime_;
  }

inline const ABA_CPUTIMER *ABA_MASTER::pricingTime() const
  {
    return &pricingTime_;
  }

inline const ABA_CPUTIMER *ABA_MASTER::branchingTime() const
  {
    return &branchingTime_;
  }

inline void ABA_MASTER::countLp()
  {
    ++nLp_;
  }

inline void ABA_MASTER::newFixed(int n)
  {
    nFixed_ += n;
  }

inline void ABA_MASTER::addCons(int n)
  {
    nAddCons_ += n;
  }

inline void ABA_MASTER::removeCons(int n)
  {
    nRemCons_ += n;
  }

inline void ABA_MASTER::addVars(int n)
  {
    nAddVars_ += n;
  }

inline void ABA_MASTER::removeVars(int n)
  {
    nRemVars_ += n;
  }

inline int ABA_MASTER::nSub() const
  {
    return nSub_;
  }

inline int ABA_MASTER::nLp() const
  {
    return nLp_;
  }

inline int ABA_MASTER::highestLevel () const
  {
    return highestLevel_;
  }

inline int ABA_MASTER::nNewRoot() const
  {
    return nNewRoot_;
  }

inline int ABA_MASTER::nSubSelected() const
  {
    return nSubSelected_;
  }

inline bool ABA_MASTER::fixSetByRedCost() const
  {
    return fixSetByRedCost_;
  }

inline void ABA_MASTER::fixSetByRedCost(bool on)
  {
    fixSetByRedCost_ = on;
  }

inline bool ABA_MASTER::printLP() const
  {
    return printLP_;
  }

inline void ABA_MASTER::printLP(bool on)
  {
    printLP_ = on;
  }

inline int ABA_MASTER::maxConAdd() const
  {
    return maxConAdd_;
  }

inline void ABA_MASTER::maxConAdd(int max)
  {
    maxConAdd_ = max;
  }

inline int ABA_MASTER::maxConBuffered() const
  {
    return maxConBuffered_;
  }

inline void ABA_MASTER::maxConBuffered(int max) 
  {
    maxConBuffered_ = max;
  }

inline int ABA_MASTER::maxVarAdd() const
  {
    return maxVarAdd_;
  }

inline void ABA_MASTER::maxVarAdd(int max)
  {
    maxVarAdd_ = max;
  }

inline int ABA_MASTER::maxVarBuffered() const
  {
    return maxVarBuffered_;
  }

inline void ABA_MASTER::maxVarBuffered(int max) 
  {
    maxVarBuffered_ = max;
  }

inline int ABA_MASTER::maxIterations() const
  {
    return maxIterations_;
  }

inline void ABA_MASTER::maxIterations(int max)
  {
    maxIterations_ = max;
  }

inline const ABA_STRING &ABA_MASTER::optimumFileName() const
  {
    return optimumFileName_;
  }

inline void ABA_MASTER::optimumFileName(const char *name)
  {
    optimumFileName_ = name;
  }

inline ABA_MASTER::STATUS ABA_MASTER::status() const
  {
    return status_;
  }

inline void ABA_MASTER::status(STATUS stat)
  {
    status_ = stat;
  }

inline bool ABA_MASTER::eliminateFixedSet() const
  {
    return eliminateFixedSet_;
  }

inline void ABA_MASTER::eliminateFixedSet(bool turnOn)
  {
    eliminateFixedSet_ = turnOn;
  }

inline bool ABA_MASTER::newRootReOptimize() const
  {
    return newRootReOptimize_;
  }

inline void ABA_MASTER::newRootReOptimize(bool on)
  {
    newRootReOptimize_ = on;
  }

inline bool ABA_MASTER::showAverageCutDistance() const
  {
    return showAverageCutDistance_;
  }

inline void ABA_MASTER::showAverageCutDistance(bool on)
  {
    showAverageCutDistance_ = on;
  }

inline ABA_MASTER::VBCMODE ABA_MASTER::vbcLog() const
  {
    return VbcLog_;
  }

inline void ABA_MASTER::vbcLog(VBCMODE mode)
  {
    VbcLog_ = mode;
  }

inline ABA_MASTER::CONELIMMODE ABA_MASTER::conElimMode() const
  {
    return conElimMode_;
  }

inline void ABA_MASTER::conElimMode(CONELIMMODE mode)
  {
    conElimMode_ = mode;
  }

inline ABA_MASTER::VARELIMMODE ABA_MASTER::varElimMode() const
  {
    return varElimMode_;
  }

inline void ABA_MASTER::varElimMode(VARELIMMODE mode)
  {
    varElimMode_ = mode;
  }

inline double ABA_MASTER::conElimEps() const
  {
    return conElimEps_;
  }

inline void ABA_MASTER::conElimEps(double eps)
  {
    conElimEps_ = eps;
  }

inline double ABA_MASTER::varElimEps() const
  {
    return varElimEps_;
  }

inline void ABA_MASTER::varElimEps(double eps)
  {
    varElimEps_ = eps;
  }

inline int ABA_MASTER::varElimAge() const
  {
    return varElimAge_;
  }

inline void ABA_MASTER::varElimAge(int age)
  {
    varElimAge_ = age;
  }

inline int ABA_MASTER::conElimAge() const
  {
    return conElimAge_;
  }

inline void ABA_MASTER::conElimAge(int age)
  {
    conElimAge_ = age;
  }

inline ABA_MASTER::ENUMSTRAT ABA_MASTER::enumerationStrategy() const
  {
    return enumerationStrategy_;
  }

inline void ABA_MASTER::enumerationStrategy(ENUMSTRAT strat)
  {
    enumerationStrategy_ = strat;
  }

inline ABA_MASTER::BRANCHINGSTRAT ABA_MASTER::branchingStrategy() const
  {
    return branchingStrategy_;
  }

inline void ABA_MASTER::branchingStrategy(BRANCHINGSTRAT strat)
  {
    branchingStrategy_ = strat;
  }

inline ABA_MASTER::OSISOLVER ABA_MASTER::defaultLpSolver() const
  {
    return defaultLpSolver_;
  }

inline void ABA_MASTER::defaultLpSolver(OSISOLVER lpSolver)
  {
    defaultLpSolver_ = lpSolver;
  }

inline int ABA_MASTER::nBranchingVariableCandidates() const
  {
    return nBranchingVariableCandidates_;
  }

inline double ABA_MASTER::requiredGuarantee() const
  {
    return requiredGuarantee_;
  }

inline int ABA_MASTER::maxLevel() const
  {
    return maxLevel_;
  }

inline const ABA_STRING& ABA_MASTER::maxCpuTime() const
  {
    return maxCpuTime_;
  }

inline void ABA_MASTER::maxCpuTime(const ABA_STRING &t)
  {
    maxCpuTime_ = t;
  }

inline const ABA_STRING& ABA_MASTER::maxCowTime() const
  {
    return maxCowTime_;
  }

inline void ABA_MASTER::maxCowTime(const ABA_STRING &t)
  {
    maxCowTime_ = t;
  }

inline bool ABA_MASTER::objInteger() const
  {
    return objInteger_;
  }

inline void ABA_MASTER::objInteger(bool b)
  {
    objInteger_ = b;
  }

inline int ABA_MASTER::tailOffNLp() const
  {
    return tailOffNLp_;
  }

inline void ABA_MASTER::tailOffNLp(int n)
  {
    tailOffNLp_ = n;
  }

inline double ABA_MASTER::tailOffPercent() const
  {
    return tailOffPercent_;
  }

inline ABA_MASTER::OUTLEVEL ABA_MASTER::outLevel() const
  {
    return outLevel_;
  }

inline void ABA_MASTER::outLevel(OUTLEVEL mode)
  {
    outLevel_ = mode;
  }

inline ABA_MASTER::OUTLEVEL ABA_MASTER::logLevel() const
  {
    return logLevel_;
  }

inline void ABA_MASTER::logLevel(OUTLEVEL mode)
  {
    logLevel_ = mode;
  }

inline void ABA_MASTER::dbThreshold(int threshold)
  {
    dbThreshold_ = threshold;
  }

inline int ABA_MASTER::dbThreshold() const
  {
    return dbThreshold_;
  }

inline int ABA_MASTER::minDormantRounds() const
  {
    return minDormantRounds_;
  }

inline void ABA_MASTER::minDormantRounds(int nRounds)
  {
    minDormantRounds_ = nRounds;
  }

inline ABA_MASTER::PRIMALBOUNDMODE ABA_MASTER::pbMode() const
  {
    return pbMode_;
  }

inline void ABA_MASTER::pbMode(PRIMALBOUNDMODE mode)
  {
    pbMode_ = mode;
  }

inline int ABA_MASTER::pricingFreq() const
  {
    return pricingFreq_;
  }

inline int ABA_MASTER::skipFactor() const
  {
    return skipFactor_;
  }

inline ABA_MASTER::SKIPPINGMODE ABA_MASTER::skippingMode() const
  {
    return skippingMode_;
  }

inline void ABA_MASTER::skippingMode(SKIPPINGMODE mode)
  {
    skippingMode_ = mode;
  }


#endif  // ABA_MASTER_H


/*! \class ABA_MASTER
 *  \brief Class ABA_MASTER is the central
 * object of the framework. The most important tasks of the class ABA_MASTER
 * is the management of the implicit enumeration. Moreover, it provides already
 * default implementations for constraints, cutting planes, and
 * variables pools
 */

