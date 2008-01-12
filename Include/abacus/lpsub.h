/*!\file
 * \author Matthias Elf
 * \brief linear program of a subproblem.
 *
 * This class is derived from the class \a LP to implement the
 * linear programming relaxations of a subproblem. We require
 * this class as the ABA_CONSTRAINT/ABA_VARIABLE format of the
 * constraints/variables has to be transformed to the ABA_ROW/ABA_COLUMN format
 * required by the class \a LP. Moreover the class ABA_LPSUB is also
 * a preprocessor for the linear programs. Currently we only
 * provide the elimination of (nonbasic) fixed and set variables.
 * Future extensions should be considered.
 *
 * \par
 * The class ABA_LPSUB is still an abstract class independent of the
 * used LP-solver. The class for solving LP-relaxation with the
 * LP-solvers supported by the Open Solver Interface (OSI) is the class 
 * ABA_LPSUBOSI.
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


#ifndef ABA_LPSUBH
#define ABA_LPSUBH

#include "abacus/lp.h"

class ABA_INFEASCON;
class ABA_SUB;
class ABA_MASTER;
class ABA_FSVARSTAT;
class ABA_CONSTRAINT;
class ABA_VARIABLE;
  class  ABA_LPSUB :  public virtual ABA_LP  {
    friend class ABA_SUB;
    friend class ABA_SETBRANCHRULE;
    friend class ABA_BOUNDBRANCHRULE;
    friend class ABA_VALBRANCHRULE;
    friend class ABA_CONBRANCHRULE;
    friend class COPBRANCHRULE;
    public: 

/*! The constructor.
 *
 *    \param master A pointer to the corresponding master of the optimization.
 *    \param sub The subproblem of which the LP-relaxation is
 *                   solved.
*/
      ABA_LPSUB (ABA_MASTER *master, const ABA_SUB *sub);

/*! \brief The destructor
 *   deletes the components of \a infeasCons_ since they might
 *   have been allocated in the constructor and ABA_SUB::initializeLp() deletes
 *   after having tried to add variables restoring feasibility immediately
 *   ABA_LPSUB. Afterwards the constructor of ABA_LPSUB is called again.
*/
      virtual ~ABA_LPSUB();
      const ABA_SUB * sub() const;

/*! \return The number of columns which are
 *   passed to the LP-solver, i.e., the number of active variables
 *   of the subproblem minus the number of eliminated variables.
*/
      int trueNCol() const;

/*! \return The number of nonzeros which
 *   are currently present in the constraint matrix of the LP-solver.
*/
      int trueNnz() const;

/*! \brief We have to redefine the function \a lBound(i) since variables may have
 *   been eliminated.
 *
 *   \return The lower bound of variable \a i. If a variable is eliminated, we
 *   return the value the eliminated variable is fixed or set to.
 *
 *   \param i The number of a variable.
*/
      double lBound(int i)  const;

/*! \brief We have to redefine the function \a uBound(i) since variables may have
 *   been eliminated.
 *
 *   \return The upper bound of variable \a i. If a variable is eliminated, we
 *   return the value the eliminated variable is fixed or set to.
 *
 *   \param i The number of a variable.*
*/
      double uBound(int i)  const;

/*! \brief Since variables might be eliminated we have to add to the solution value
 *   of the LP-solver the objective function part of the eliminated
 *   variables, to get the right value of \a value().
 *
 *   \return The objective function value of the linear program.
*/
      virtual double value() const;

/*! \brief We have to redefine the function \a xVal(i) since variables may have
 *   been eliminated.
 *
 *   \return The \f$x\f$-value of variable \a i after the solution of the linear program.
 */
      virtual double xVal(int i);

/*! \brief We have to redefine the function \a barXVal(i) since variables may have
 *   been eliminated.
 *   
 *   \return The \f$x\f$-value of variable \a i after the solution of the linear
 *   program before crossing over to a basic solution.
*/
      virtual double barXVal(int i);

/*! We define the reduced costs of eliminated variables as 0.
 *
 *   \return The reduced cost of variable \a i.
*/
      virtual double reco(int i);

/*! \return The status of the variable
 *   in the linear program. If the variable \a i is eliminated,
 *   then ABA_LPVARSTAT::Eliminated is returned.
*/
      virtual ABA_LPVARSTAT::STATUS lpVarStat(int i);


/*! \brief Is called if the last linear program
 *   has been solved with the dual simplex method and is infeasible.
 *   
 *   In this case it computes the infeasible basic variable or constraint
 *   and the corresponding row of the basis inverse.
 *
 *   \return 0 If no error occurs,
 *   \return 1 otherwise.
 *
 *   \param infeasCon If nonnegative, this is the number of the
 *                        infeasible slack variable.
 *   \param infeasVar If nonnegative, this is the number of the
 *                        infeasible structural variable. Note, either
 *                        \a infeasCon or \a infeasVar is nonnegative.
 *   \param bInvRow An array containing the corresponding row
 *                      of the basis inverse.
*/      virtual int getInfeas(int &infeasCon, int &infeasVar, double *bInvRow);

/*!  \return true If the \a LP turned out to be
 *                   infeasible either if the base class \a LP detected an infeasibility
 *                   during the solution of the linear program
 *                   or infeasible constraints have been memorized during the
 *                   construction of the LP or during the addition of constraints,
 *                 }
 *   \return false otherwise.
*/
      virtual bool infeasible() const;

/*! return A pointer to the buffer holding the infeasible constraints.
*/
      ABA_BUFFER<ABA_INFEASCON*> *infeasCon();

/*! Loads a new basis for the linear program.
 *  
 *   The function redefines a virtual function of the base class \a LP. 
 *   Eliminated variables have to be considered when the basis is loaded.
 *
 *      \param lpVarStat An array storing the status of the columns.
 *      \param slackStat An array storing the status of the slack variables.
 */
      virtual void loadBasis(ABA_ARRAY<ABA_LPVARSTAT::STATUS> &lpVarStat,
                             ABA_ARRAY<ABA_SLACKSTAT::STATUS> &slackStat);
    protected: 

/*! \brief The function \a initialize()
 *   has to be called in the constructor
 *   of the class derived from this class and from a class implementing
 *   an LP-solver.
 *
 *   This function will pass the linear program of
 *   the associated subproblem to the solver.
 */
      virtual void initialize();
      
    private:

/*! Performs the optimization of the linear program with method \a method.
 *
 *   This function redefines a virtual function of the base class \a LP.
 *
 *   \par
 *   We have to reimplement \a optimize() since there might be infeasible
 *   constraints.
 *   If a linear program turns out to be infeasible but has not been
 *   solved with the dual simplex method we solve it again to
 *   find a dual feasible basis which can be used to determine
 *   inactive variables restoring feasibility.
 *   Before the optimization can be performed the infeasible constraints
 *   must be removed with the function \a _initMakeFeas(), then the
 *   \a LP should be deleted and reconstructed. This is done by the function
 *   \a solveLp() in the cutting plane algorithm of the class ABA_SUB.
*/
      virtual OPTSTAT optimize(METHOD method);

/*! Removes all constraints listed in the buffer \a ind from the linear program.
*/
      virtual void removeCons(ABA_BUFFER<int> &ind);

/*! Removes the variables with names given in \a vars from the linear program.
*/
      virtual void removeVars(ABA_BUFFER<int> &vars);

/*! Adds the constraints \a newCons to the linear program.
*/
      virtual void addCons(ABA_BUFFER<ABA_CONSTRAINT*> &newCons);

/*!      \param vars The new variables which are added to the linear
 *                       program.
 *       \param fsVarstat The status of fixing/setting of the new variables.
 *       \param lb The lower bounds of the new variables.
 *       \param ub The upper bounds of the new variables.
*/
      virtual void addVars(ABA_BUFFER<ABA_VARIABLE*> &vars,
                           ABA_BUFFER<ABA_FSVARSTAT*> &fsVarStat,
                           ABA_BUFFER<double> &lb, ABA_BUFFER<double> &ub);

/*! Sets the lower bound of variable \a i to \a newLb. 
 *
 *   It is not allowed to change the lower bound of an
 *   eliminated variable. This will cause a run-time error.
*/
      virtual void changeLBound(int i, double newLb);

/*! Sets the upper bound of variable \a i to \a newUb.
 *
 *   It is not allowed to change the upper bound of
 *   an eliminated variable. This will cause a run-time error.
*/
      virtual void changeUBound(int i, double newUb);

/*! Sets the maximal number of variables to \a newSize.
*/
      virtual void varRealloc(int newSize);

/*! Sets the maximal number of constraints to \a newSize.
 */
      virtual void conRealloc(int newSize);

/*! Generates the row format of the constraint \a cons and stores it in \a rows.
*/
      void constraint2row(ABA_BUFFER<ABA_CONSTRAINT*> &newCons,
                          ABA_BUFFER<ABA_ROW*> &newRows);

/*! Returns \a true if the function can be  eliminated.
 *
 *   This function may be only applied to variables which are fixed or set!
 *   It is sufficient for turning off any variable elimination to return always
 *   \a false by this function.
*/
      bool           eliminable(int i) const;

/*! \brief Returns \a true if the variable
 *   \a i is actually eliminated from the \a LP.
 *
 *   This function can
 *   give different results than the function \a eliminate(i) since
 *   the condition to eliminate a variable might have become \a true
 *   after the \a LP has been set up.
*/
      bool           eliminated(int i) const;

/*! Returns the value the variable \a i to which it is fixed or set to.
 *
 *   The value of an eliminated variable is defined by the bound to which
 *   it is fixed or set.
 *   There is no reason to distinguish between \a sub_ and \a master_
 *   in the \a switch statement, since both values should be equal.
*/
      virtual double elimVal(int i) const;

/*! Returns the value a variable is fixed or set to.
 *
 *      \param fsVarStat A pointer to the status of the variable.
 *      \param lb The lower bound of the variable.
 *      \param ub The upper bound of the variable.
*/
      virtual double elimVal(ABA_FSVARSTAT *stat, double lb, double ub) const;
      void initialize(ABA_OPTSENSE sense, int nRow, int maxRow,
                      int nCol, int maxCol,
                      ABA_ARRAY<double> &obj, ABA_ARRAY<double> &lBound,
                      ABA_ARRAY<double> &uBound, ABA_ARRAY<ABA_ROW*> &rows);  
      void initialize(ABA_OPTSENSE sense, int nRow, int maxRow,
                      int nCol, int maxCol,
                      ABA_ARRAY<double> &obj, ABA_ARRAY<double> &lBound,
                      ABA_ARRAY<double> &uBound, ABA_ARRAY<ABA_ROW*> &rows,
                      ABA_ARRAY<ABA_LPVARSTAT::STATUS> &lpVarStat,
                      ABA_ARRAY<ABA_SLACKSTAT::STATUS> &slackStat);
      int        nCol()   const;      
      int        maxCol() const;      
      int        nnz()    const;      
      double     obj(int i)     const;

      void rowRealloc(int newSize);
      void colRealloc(int newSize);

      /*! A pointer to the corresponding subproblem.
       */
      const ABA_SUB          *sub_;

      /*! \brief After the elimination of variables the internal variables are
       *  again numbered consecutively starting with 0. \a orig2lp_[i] is
       *  the internal number of the variable \a i. This is \a -1 if the variable
       *  is eliminated.
       */
      ABA_ARRAY<int>         orig2lp_;

      /*! Orignial number of a (non-eliminated) variable.
       */
      ABA_ARRAY<int>         lp2orig_;

      /*! Buffer storing the infeasible constraints found be the constructor.
       */
      ABA_BUFFER<ABA_INFEASCON*> infeasCons_;

      /*! \brief The constant which has been added to the objective function
       *  value due to the elimination of  variables.
       */
      double             valueAdd_;

      /*! The number of original variables of the linear program.
       */
      int                nOrigVar_;
      ABA_LPSUB(const ABA_LPSUB &rhs);
      const ABA_LPSUB &operator=(const ABA_LPSUB &rhs);
  };


inline const ABA_SUB *ABA_LPSUB::sub() const
{
    return sub_;
}

inline bool ABA_LPSUB::eliminated(int i) const
  {
    if (orig2lp_[i] == -1) return true;
    else                   return false;
  }

inline int ABA_LPSUB::trueNCol() const
  {
    return ABA_LP::nCol();
  }

inline int ABA_LPSUB::trueNnz() const
  {
    return ABA_LP::nnz();
  }

inline double ABA_LPSUB::value() const
  {
    return ABA_LP::value() + valueAdd_;
  }

inline ABA_BUFFER<ABA_INFEASCON*> *ABA_LPSUB::infeasCon()
  {
    return &infeasCons_;
  }


#endif  // ABA_LPSUBH

/*! \class ABA_LPSUB
 *  \brief  class is derived from the class \a LP to implement the
 * linear programming relaxations of a subproblem. We require
 * this class as the ABA_CONSTRAINT/ABA_VARIABLE format of the
 * constraints/variables has to be transformed to the ABA_ROW/ABA_COLUMN format
 * required by the class \a LP. 
 */

