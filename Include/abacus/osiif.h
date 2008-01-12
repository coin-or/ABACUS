/*!\file
 * \author Frank Baumann
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
 */

#ifndef OSIIF_H
#define OSIIF_H
#include "abacus/lp.h"
#include "abacus/optsense.h"
#include "abacus/lpvarstat.h"
#include "abacus/slackstat.h"
#include "abacus/csense.h"
#include "abacus/osiinclude.h"
#include <CoinPackedVector.hpp>
#include <CoinWarmStartBasis.hpp>
#include <CoinBuild.hpp>

class ABA_LPMASTEROSI;

  class  ABA_OSIIF :  public virtual ABA_LP  {
    public:

/*! \brief This constructor
 * does not initialize the problem data of the linear
 *  program. It must be loaded later with the function \a initialize().
 *
 *  \param master A pointer to the corresponding master of the
 * optimization.
 */
    ABA_OSIIF(ABA_MASTER *master);

/*! A constructor with initialization.
 *
 *    \param master A pointer to the corresponding master of the optimization.
 *    \param sense The sense of the objective function.
 *    \param nCol The number of columns (variables).
 *    \param maxCol The maximal number of columns.
 *    \param nRow The number of rows.
 *    \param maxRow The maximal number of rows.
 *    \param obj An array with the objective function coefficients.
 *    \param lb An array with the lower bounds of the columns.
 *    \param ub An array with the upper bounds of the columns.
 *    \param rows An array storing the rows of the problem.
 */
      ABA_OSIIF(ABA_MASTER *master,
                  ABA_OPTSENSE sense, 
                  int nRow, 
                  int maxRow, 
                  int nCol, 
                  int maxCol,
                  ABA_ARRAY<double> &obj, 
                  ABA_ARRAY<double> &lb, 
                  ABA_ARRAY<double> &ub,
                  ABA_ARRAY<ABA_ROW*> &rows);

//! The destructor.
      virtual ~ABA_OSIIF();

/*!
 * The enumeration of possible solver types
 */
      enum SOLVERTYPE {Exact, Approx};

      inline SOLVERTYPE currentSolverType()
      { return currentSolverType_; }

      inline OsiSolverInterface* osiLP();
	 

    private:

      /****************************************************************
       * Pointer to the Osi solver interface. It is later typecast to 
       * a pointer to an object of a solver specific derived class.
       * See osiif.cc, ABA_OSIIF::_initialize.
       ****************************************************************/
      OsiSolverInterface *osiLP_;

      /***************************************************
       * Helper functions to free allocated memory
       **************************************************/

      void freeDouble(const double*&);

      void freeDouble(double*&);

      void freeInt(int*&);
      
      void freeChar(char*&);

      void freeChar(const char*&);

      void freeStatus(CoinWarmStartBasis::Status*&);

/*! \brief Implements the corresponding pure virtual
 *  function of the base class \a LP and loads the linear program
 *  defined by the following arguments to the solver.
 *
 *    \param sense The sense of the objective function.
 *    \param nCol The number of columns (variables).
 *    \param maxCol The maximal number of columns.
 *    \param nRow The number of rows.
 *    \param maxRow The maximal number of rows.
 *    \param obj An array with the objective function coefficients.
 *    \param lb An array with the lower bounds of the columns.
 *    \param ub An array with the upper bounds of the columns.
 *    \param rows An array storing the rows of the problem.
*/
      virtual void    _initialize(ABA_OPTSENSE sense, int nRow, int maxRow,
                                  int nCol, int maxCol,
                                  ABA_ARRAY<double> &obj, ABA_ARRAY<double> &lBound,
                                  ABA_ARRAY<double> &uBound, ABA_ARRAY<ABA_ROW*> &rows);

 /*! Loads a basis to the solver
 * 
 * \param lpVarStat An array storing the status of the columns.
 * \param lpVarStat An array storing the status of the slack variables.
 */
      virtual void _loadBasis(ABA_ARRAY<ABA_LPVARSTAT::STATUS> &lpVarStat,
                              ABA_ARRAY<ABA_SLACKSTAT::STATUS> &slackStat);

 /*! Returns the sense of the optimization.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual ABA_OPTSENSE _sense() const;

 /*! \brief This version of the function \a _sense()
  * changes the sense of the optimization. 
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual void     _sense(const ABA_OPTSENSE &newSense);

 /*! Returns the number of rows of the linear program.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual int      _nRow()  const;

 /*! Returns the maximal number of rows of the linear program.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual int      _maxRow() const;

 /*! Returns the number of columns of the linear program.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual int      _nCol() const;

 /*! Returns the maximal number of columns of the linear program.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual int      _maxCol() const;

 /*! Returns the objective function coefficient of column \a i.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual double   _obj(int i) const; 

 /*! Returns the lower bound of column \a i.
  *
  * It implements the pure virtual function of the base class \a LP.
  */             
      virtual double   _lBound(int i) const;

 /*! Returns the upper bound of column \a i.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual double   _uBound(int i) const;

 /*! Returns the right hand side of row \a i.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual double   _rhs(int i) const;

 /*! Stores a copy of row \a i in \a r.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual void     _row(int i, ABA_ROW &r) const;

 /*! \brief Returns the number of nonzero elements in the
  * constraint matrix (not including the right hand side).
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual int      _nnz() const;


 /*! Calls the primal simplex method.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual OPTSTAT _primalSimplex();

 /*! Calls the dual simplex method.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual OPTSTAT _dualSimplex();

 /*! Calls the barrier method.
  * 
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual OPTSTAT _barrier(bool doCrossover);

 /*! Calls an approximate method.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual OPTSTAT _approx();

/*! Returns the optimum value of the linear program.
  *
  * It implements the pure virtual function of the base class \a LP.
  */ 
      virtual double _value() const;

 /*! Returns the value of the column \a i.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual double _xVal(int i);

 /*! Returns the value of the column \a i.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual double _barXVal(int i);

 /*! Returns the reduced cost of the column \a i.
  *
  * It implements the pure virtual function of the base class \a LP.
  */ 
      virtual double _reco(int i);

 /*! Returns the value of the slack column of the row \a i.
  *
  * It implements the pure virtual function of the base class \a LP.
  */ 
      virtual double _slack(int i);

 /*! Returns the value of the dual column of the row \a i.
  *
  * It implements the pure virtual function of the base class \a LP.
  */ 
      virtual double _yVal(int i);

 /*! Returns the status of the column \a i.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual ABA_LPVARSTAT::STATUS _lpVarStat(int i);

 /*! Returns the status of the slack column \a i.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual ABA_SLACKSTAT::STATUS _slackStat(int i);
     
/*! \brief Can be called if the last linear program
 *  has been solved with the dual simplex method and is infeasible.
 *  This function is currently not supported by the interface.
 *
 *  In this case it computes the infeasible basic variable or constraint
 *  and the corresponding row \a nInvRow of the basis inverse.
 *  Either \a infeasRow or \a infeasCol is nonnegative. Then this number
 *  refers to an infeasible variable or slack variable, respectively.
 *  The function returns 0 if it is successful, 1 otherwise.
 *
 *  Currently this featureis not supported by the Open Solver Interface,
 *  therefore a call to this function always returns an error status.
 *
 * \par
 *  It implements the pure virtual function of the base class \a LP.
 */
      virtual int  _getInfeas(int &infeasRow, int &infeasCol, double *bInvRow);

/*! Removes the rows listed in \a ind.
 *
 * It implements the pure virtual function of the base class \a LP.
 */
      virtual void _remRows(ABA_BUFFER<int> &ind);

/*! Adds the \a rows to the linear program.
 *  
 *  It implements the pure virtual function of the base class \a LP.
 */
      virtual void _addRows(ABA_BUFFER<ABA_ROW*> &newRows) ;

/*! Removes the columns listed in \a vars.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual void _remCols(ABA_BUFFER<int> &vars);

/*! Adds the columns \a newCols to the linear program.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual void _addCols(ABA_BUFFER<ABA_COLUMN*> &newVars);

 /*! Sets the right hand side of the linear program to \a newRhs. 
  *
  * This array must have at least length of the
  * number of rows.
  * This function implements the pure virtual function of the base class \a LP.
  */
      virtual void _changeRhs(ABA_ARRAY<double> &newRhs);

 /*! Sets the lower bound of column \a i to \a newLb.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual void _changeLBound(int i, double newLb);

 /*! Sets the upper bound of column \a i to \a newLb.
  *
  * It implements the pure virtual function of the base class \a LP.
  */
      virtual void _changeUBound(int i, double newUb);

 /*! \brief Pivots the slack variables
  * stored in the buffer \a rows into the basis. This function defines
  * the pure virtual function of the base class \a LP.
  * This function is currently not supported by the interface.
  *
  * \return 0 All variables could be pivoted in,
  * \return 1 otherwise.
  *
  * \param rows The numbers of the slack variables that should be
  *   pivoted in.
  */
      virtual int _pivotSlackVariableIn(ABA_BUFFER<int> &rows);

/*! \brief Extracts the solution, i.e., the value,
  * the status, the values of the variables, slack variables, and dual
  * variables, the reduced costs, and the statuses of the variables
  * and slack variables form the internal solver data structure.
  */
      void              getSol();

 /*! \brief Converts the ABACUS representation of the
  * row sense to the Osi representation.
  */
      char              csense2osi(ABA_CSENSE *sense)             const;

 /*! \brief Converts the OSI representation of the
  * row sense to the ABACUS representation.
  */
      ABA_CSENSE::SENSE     osi2csense(char sense)                const;

 /*! \brief Converts the ABACUS variable status to OSI format
  */
      CoinWarmStartBasis::Status lpVarStat2osi(ABA_LPVARSTAT::STATUS stat) const;

 /*! \brief Converts the OSI variable status to ABACUS format
  */
      ABA_LPVARSTAT::STATUS osi2lpVarStat(CoinWarmStartBasis::Status stat)               const;

 /*! \brief Converts the ABACUS slack status to OSI format
  */
      CoinWarmStartBasis::Status slackStat2osi(ABA_SLACKSTAT::STATUS stat) const;

 /*! \brief Converts the OSI slack status to ABACUS format
  */
      ABA_SLACKSTAT::STATUS osi2slackStat(CoinWarmStartBasis::Status stat)               const;
      
 /*! \brief Allocates an Open Solver Interface of type defaultOsiSolver
  */
      OsiSolverInterface* getDefaultInterface();

 /*! \brief Switches between exact and approximate solvers
  */
      OsiSolverInterface* switchInterfaces(SOLVERTYPE newMethod);

 /*! \brief Initializes the problem with a dummy row
  *  To be used with CPLEX if there are no rows
  */
      void loadDummyRow(OsiSolverInterface* s2, const double* lbounds, const double* ubounds, const double* objectives);

/*! Reallocates the internal memory such that newSize rows can be stored.
 *  This function is obsolete, as memory management is completely handled by Osi.
 *
 *  It implements the corresponding pure virtual function of the base
 *  class \a LP.
 *  If a reallocation is performed in the base class \a LP, we
 *  reinitialize the internal data structure. Actually this
 *  reinitialization is redundant since it would be performed
 *  automatically if \a addRows() or \a addCols() fail. However,
 *  to be consistent, and if a reallocation is performed to
 *  decrease the size of the arrays we call \a reinitialize().
 */
      void _rowRealloc(int newSize);

/*! Reallocates the internal memory such that \a newSize columns can be stored.
 *  This function is obsolete, as memory management is completely handled by Osi.
 *
 * It implements the corresponding pure virtual function of the base
 * class \a LP.
 */
      void _colRealloc(int newSize);
     
 /*! Changes the iteration limit of the Simplex algorithm.
  *
  * This function defines a pure virtual function of the base class \a LP.
  *
  * \return 0 If the iteration limit could be set,
  * \return 1 otherwise.
  *
  * \param limit The new value of the iteration limit.
  */
      virtual int _setSimplexIterationLimit(int limit);
      
/*! Defines a pure virtual function of the base class \a LP.
 *
 *  \return 0 If the iteration limit could be retrieved,
 *  \return 1 otherwise. 
 *
 *  \param limit Stores the iteration limit if the return value is 0.
 */
      virtual int _getSimplexIterationLimit(int &limit);
      ABA_LPMASTEROSI *lpMasterOsi_;

/*! The value of the optimal solution.
 */
      double  value_;

/*! \brief An array storing the values of the variables after the linear program
 *         has been optimized.
 */
      const double *xVal_;
      const double *barXVal_;

/*! \brief An array storing the values of the reduced costs after the linear program
 *  has been optimized.
 */
      const double *reco_;

/*! \brief An array storing the values of the dual variables after the linear program
 *  has been optimized.
 */
      const double *yVal_;

/*! \brief An array storing the statuses of the variables after the linear program
 *  has been optimized.
 */
      const char *cStat_;

/*! \brief The number of columns currently used in the LP
 */
      int numCols_;

/*! \brief The number of rows currently used in the LP
 */
      int numRows_;

/*! \brief An array storing the statuses of the slack variables after the linear program
 *  has been optimized.
 */
      const char *rStat_;

/*! \brief An array storing the right hand sides of the linear program
 */
      const double *rhs_;
 
/*! \brief An array storing the row activity of the linear program
 */
      const double *rowactivity_;

/*! \brief An array storing the row senses of the linear program
 */
      const char *rowsense_;

/*! \brief An array storing the column upper bounds of the linear program
 */
      const double *colupper_;
      
/*! \brief An array storing the column lower bounds of the linear program
 */
      const double *collower_;

/*! \brief An array storing the objective function coefficients of the linear program
 */
      const double *objcoeff_;

/*! \brief A warm start object storing information about a basis of the linear program
 */
      CoinWarmStartBasis *ws_;

/*! \brief The type of the current solver interface
 */
      SOLVERTYPE currentSolverType_;

      ABA_OSIIF(const ABA_OSIIF &rhs);
      const ABA_OSIIF &operator=(const ABA_OSIIF &rhs);

inline void
convertSenseToBound(double inf, const char sense, const double right,
                                         const double range,
                                         double& lower, double& upper) const
 {
   switch (sense) {
   case 'E':
     lower = upper = right;
     break;
   case 'L':
     lower = -inf;
     upper = right;
     break;
   case 'G':
     lower = right;
     upper = inf;
     break;
   case 'R':
     lower = right - range;
     upper = right;
     break;
   case 'N':
     lower = -inf;
     upper = inf;
     break;
   }
 }

  };

inline OsiSolverInterface* ABA_OSIIF::osiLP()
{ return osiLP_; }
#endif
