/*!\file
 * \author Matthias Elf
 * \brief constraint.
 *
 * Constraints are one of the central items in a \lpbab\ algorithm.
 * This class forms the virtual base class for all possible constraints
 * given in pool format and is derived from the common base class ABA_CONVAR of
 * all constraints and variables.
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


#ifndef ABA_CONSTRAINT_H
#define ABA_CONSTRAINT_H

#include "abacus/convar.h"
#include "abacus/array.h"
#include "abacus/infeascon.h"
#include "abacus/csense.h"

class ABA_ROW;
class ABA_MASTER;
class ABA_VARIABLE;
class ABA_CONCLASS;
class ABA_LPSUB;

template<class BaseType, class CoType> class ABA_ACTIVE;

class  ABA_CONSTRAINT :  public ABA_CONVAR  {
  friend class ABA_LPSUB;
  public:

/*! The constructor.
 *
 *    \param master A pointer to the corresponding master of the optimization.
 *    \param sub A pointer to the subproblem associated with the
 *                   constraint. This can be also the 0-pointer.
 *    \param sense The sense of the constraint.
 *    \param rhs The right hand side of the constraint.
 *    \param dynamic If this paramument is \a true, then the constraint
 *                        can be removed from the active constraint set
 *                        during the cutting plane phase of the subproblem
 *                        optimization.
 *    \param local If this paramument is \a true, then the constraint
 *                     is considered to be only locally valid. In this case
 *                     the paramument sub must not be 0 as each locally
 *                     valid constraint is associated with a subproblem.
 *    }
 *    \param liftable If this paramument is \a true, then a lifting
 *                        procedure must be available, i.e., that the
 *                        coefficients of variables which have not been
 *                        active at generation time of the constraint
 *                        can be computed.
 */
    ABA_CONSTRAINT (ABA_MASTER *master, const ABA_SUB *sub,
                    ABA_CSENSE::SENSE sense, double rhs,
                    bool dynamic, bool local, bool liftable);

/*! The following constructor initializes an empty constraint. 
 *
 * This constructor is, e.g., useful if parallel separation is applied. 
 * In this case the constraint can be constructed and receive later its 
 * data by message passing.
 *
 *    \param master A pointer to the corresponding master of the optimization.
 */
    ABA_CONSTRAINT (ABA_MASTER *master);

/*! The copy constructor.
 *
 *   \param rhs The constraint being copied.
 */
    ABA_CONSTRAINT(const ABA_CONSTRAINT &rhs); 

/*! The destructor.
 */
    virtual ~ABA_CONSTRAINT();

/*! \return A pointer to the sense of the constraint.
 */
    ABA_CSENSE *sense();


    /*! \param v A pointer to a variable.
     *
     *  \return   The coefficient of the variable \a v in the constraint.
     */
    virtual double coeff(ABA_VARIABLE *v) = 0;

/*! \return The right hand side of the constraint.
 */
    virtual double rhs();

/*! Checks if the constraint is liftable, 
 *
 * i.e., if
 * the coefficients of variables inactive at generation time of the constraint
 * can be computed later.
 *
 * \return true If the constraint can be lifted,
 * \return false otherwise.
 */
    bool liftable() const;
  
/*! Checks if the constraint is valid for the subproblem sub. 
 *
 * Per default, this is the case
 * if the constraint is globally valid, or the subproblem associated
 * with the constraint is an ancestor of the subproblem sub in the
 * enumeration tree.
 *
 *\return true If the constraint is valid for the subproblem sub,
 *\return false otherwise.
 *
 *\param sub The subproblem for which the validity is checked.
 */
    virtual bool valid(ABA_SUB *sub);

/*! Generates the row format of the constraint associated with the variable set \a var.
 *
 * This function is declared virtual since faster constraint specific
 * implementations might be desirable.
 *
 * \par
 * All nonzero coefficients are added to the row format. Before
 * we generate the coefficients we try to expand the constraint,
 * afterwards it is compressed again.
 *
 * \return The number of nonzero elements in the row format \a row.
 *
 *\param var The variable set for which the row format should be
 *                 computed.
 *\param row Stores the row format after calling this function.
 */
    virtual int genRow(ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *var, 
                       ABA_ROW &row);

/*! \brief Computes the slack of the vector \a x associated
 *  with the variable set \a variables.
 *
 * \return The slack induced by the vector \a x.
 *
 *\param variables The variable set associated with the vector \a x.
 *\param x The values of the variables.
 */
    virtual double slack(ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *variables,
                         double *x);

/*! Checks if a constraint is violated by a vector \a x associated with a variable set.
 *
 * \return true If the constraint is violated,
 * \return false otherwise.
 *
 * \param variables The variables associated with the vector \a x.
 * \param x The vector for which the violation is checked.
 * \param sl If \a sl is nonzero, then \a *sl will store the value of the
 *                violation, i.e., the slack.
 */
    virtual bool violated(ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *variables,
                          double *x, double *sl = 0);

/*! \brief This version of function \a violated() 
 * checks for the violation given the slack of a vector.
 *
 * \return true If the constraint
 *                  is an equation and the \a slack is nonzero, or 
 *                  if the constraint is a \f$\le\f$-inequality and the slack 
 *                  is negative, or the constraint is
 *                  a \f$\ge\f$-inequality and the slack is positive,
 * \return false otherwise.
 *
 * \param slack The slack of a vector.
 */
    virtual bool violated(double slack) const;

/*! \brief Writes the row format of the constraint
 *  associated with the variable set \a var on an output stream.
 *
 * \param out The output stream.
 * \param var The variables for which the row format should be written.
 */
    void printRow(ostream &out, ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *var);

/*! The distance of a point \f$\overline{x}\f$ and a hyperplane
 * \f$a^T x = \beta\f$ can be computed in the following way:
 * Let \f$y\f$ be the intersection of the hyperplane \f$a^T x = \beta\f$
 * and the line defined by \f$\overline{x}\f$ and the vector \f$a\f$.
 * Then the distance \f$d\f$ is the length of the vector \f$||\overline{x} - y||\f$.
 *
 * \return The Euclidean distance of the vector
 * \a x associated with the variable set \a actVar to the hyperplane
 * induced by the constraint.
 *
 * \param x The point for which the distance should be computed.
 * \param actVar The variables associated with \a x.
 */
    virtual double distance(double *x, 
                            ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *actVar);

    ABA_CONSTRAINT *duplicate()
    {return 0;};

/*! Returns a pointer to the classification of the constraint. 
 *
 * If no classification is available then
 * we try to classify the constraint. In this case \a var must not be
 * a 0-pointer. Per default \a var is 0.
 *
 * \par
 * A constraint classification can only be generated if the function
 * \a classify() is redefined in a derived class.
 */
    ABA_CONCLASS *classification(ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *var = 0);
  protected:

/*! \brief Can be called if after variable
 * elimination the left hand side of the constraint has become void
 * and the right hand side has been adapted to \a newRhs. 
 *
 * Then this function
 * checks if the constraint is violated.
 *
 * \return {TooLparame or TooSmall} If the value \a 
 * newRhs violates the sense of the constraint, i.e,
 * it is \a < / \a > / != 0 and the sense of the constraint is \a >= / \a <= / \a =,
 * \return Feasible otherwise.
 *
 * \param newRhs The right hand side of the constraint after the
 *                  elimination of the variables.
 */
    virtual ABA_INFEASCON::INFEAS voidLhsViolated(double newRhs) const;

/*! \brief The default implementation of the function \a classify() 
 * returns a 0 pointer.
 */
    virtual ABA_CONCLASS *classify(ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *var);

    /*! The sense of the constraint.
     */
    ABA_CSENSE sense_;

    /*! The right hand side of the constraint.
     */
    double rhs_;
    ABA_CONCLASS *conClass_;

    /*! \brief This member is  \a true if also coefficients of
     *         variables which have been inactive at generation time can be
     *         computed, \a false otherwise.
     */
    bool liftable_;

  private:
    const ABA_CONSTRAINT &operator=(const ABA_CONSTRAINT &rhs);
};


inline ABA_CSENSE *ABA_CONSTRAINT::sense()
  {
    return &sense_;
  }

inline bool ABA_CONSTRAINT::liftable() const
  {
    return liftable_;
  }


#endif  // ABA_CONSTRAINT_H

/*! \class ABA_CONSTRAINT
 * \brief class forms the virtual base class for all possible constraints
 * given in pool format and is derived from the common base class ABA_CONVAR of
 * all constraints and variables.
 */
