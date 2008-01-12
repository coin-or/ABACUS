/*!\file
 * \author Matthias Elf
 * \brief variable.
 *
 * Variables are one of the central items in a \lpbab\ algorithm.
 * This class forms the virtual base class for all possible variables
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

#ifndef ABA_VARIABLE_H
#define ABA_VARIABLE_H
#include <iostream>
using namespace std;

#include "abacus/convar.h"
#include "abacus/fsvarstat.h"
#include "abacus/vartype.h"

class ABA_MASTER;
class ABA_SUB;
class ABA_VARTYPE;
class ABA_COLUMN;
template<class BaseType, class CoType> class ABA_ACTIVE;

#ifdef ABACUS_PARALLEL
#include "abacus/message.h"
#endif


  class  ABA_VARIABLE :  public ABA_CONVAR  { 
    public: 

/*! The constructor.
 *
 *      \param master
 *       A pointer to the corresponding master of the optimization.
 *      \param sub
 *        A pointer to the subproblem associated with the variable. This
 *         can also be the 0-pointer.
 *      \param dynamic
 *       If this argument is \a true, then the variable can also be
 *        removed again from the set of active variables after it is
 *        added once.
 *      \param local
 *        If this argument is \a true, then the variable is only locally
 *         valid, otherwise it is globally valid. As a locally valid variable is
 *         always associated with a subproblem, the argument \a sub must
 *         not be 0 if \a local is \a true.
 *      \param
 *        obj The objective function coefficient.
 *      \param lBound
 *       The lower bound of the variable.
 *      \param uBound
 *       The upper bound of the variable.
 *      \param type
 *        The type of the variable.
 */
      ABA_VARIABLE(ABA_MASTER *master, const ABA_SUB *sub,
                   bool dynamic, bool local, 
                   double obj, double lBound, double uBound,
                   ABA_VARTYPE::TYPE type);

//! The destructor.
      virtual ~ABA_VARIABLE();
#ifdef ABACUS_PARALLEL

/*! \brief The message constructor
 *   creates the variable from an ABA_MESSAGE.
 *
 *   \param msg The ABA_MESSAGE object from which the variable is initialized.
 */
      ABA_VARIABLE(ABA_MASTER *master, ABA_MESSAGE &msg);

/*! Packs the data of the variable in an ABA_MESSAGE object.
 *
 *   This virtual function has to be redefined
 *   in a derived class if additional data should be packed. In this
 *   case the \a pack() function of the base class must be called
 *   by the \a pack() function of the derived class.
 *
 *   \param msg The ABA_MESSAGE object in which the variable is packed.
 */
      virtual void pack(ABA_MESSAGE &msg) const;
#endif


/*! \return The type of the variable.
 */
      ABA_VARTYPE::TYPE varType() const;

/*! \return true If the type of the variable is \a Integer or \a Binary,
 *   \return false otherwise.
 */
      bool discrete();

/*! \return true If the type of the variable is \a Binary,
 *   \return false otherwise.
 */
      bool binary();

/*! \return true If the type of the variable is \a Integer,
 *   \return false otherwise.
 */
      bool integer();

/*! \return The objective function coefficient.
 */
      virtual double obj();

/*! \return The upper bound of the variable.
 */
      double uBound() const;

/*! \brief This version of the function \a uBound() sets the upper bound of the variable.
 *
 *   \param newBound The new value of the upper bound.
 */ 
      void uBound(double newValue);

/*! \return The lower bound of the variable.
 */
      double lBound() const;

/*! \brief This version of the function \a lBound()
 *   sets the lower bound of the variable.
 *
 *   \param newBound The new value of the lower bound.
 */
      void lBound(double newValue);

/*! \return A pointer to the global status of fixing and setting of the variable.
 *   \note This is the global status of fixing/setting that might differ
 *   from the local status of fixing/setting a variable returned by
 *   the function ABA_SUB::fsVarStat().
 */
      ABA_FSVARSTAT *fsVarStat();

/*! \return true If the variable is globally
 *                   valid, or the subproblem \a sub is an ancestor in the
 *                   enumeration
 *                   tree of the subproblem associated with the variable,
 *   \return false otherwise.
 *
 *   \param sub The subproblem for which validity of the variable is
 *                  checked.
 */
      virtual bool valid(ABA_SUB *sub);

/*! \brief Computes the column \a col of the variable
 *   associated with the active constraints \a *actCon.
 *
 *   \note The upper and lower bound of the column are initialized
 *   with the global upper and lower bound of the variable.
 *   Therefore, an adaption with the local bounds might be required.
 *
 *   \return The number of nonzero entries in \a col.
 *
 *   \param actCon The constraints for which the column of the variable
 *                     should be computed.
 *   \param col Stores the column when the function terminates.
 */
      virtual int genColumn(ABA_ACTIVE<ABA_CONSTRAINT, ABA_VARIABLE> *actCon,
                            ABA_COLUMN &col);

/*! Computes the coefficient of the variable in the constraint \a con.
 *
 *   Per default the coefficient of a variable is
 *   computed indirectly via the coefficient of a constraint. Problem
 *   specific redefinitions might be required.
 *
 *   \return The coefficient of the variable in the constraint \a con.
 *
 *   \param con The constraint of which the coefficient should be computed.
 */
      virtual double coeff(ABA_CONSTRAINT *con);

/*! \brief Checks, if a variable does not price
 *   out correctly, i.e., if the reduced cost \a rc is positive for
 *   a maximization problem and negative for a minimization problem,
 *   respectively.
 *
 *   \return true If the variable does not price out correctly.
 *   \return false otherwise.
 *
 *   \param rc The reduced cost of the variable.
 */
      virtual bool violated(double rc) const;

/*! \brief This version of the function \a violated()
 *   checks if the variable
 *   does not price out correctly, i.e., if the reduced cost
 *   of the variable associated with the constraint set \a constraints
 *   and the dual variables \a y are positive for a maximization problem
 *   and negative for a minimization problem, respectively. 
 *
 *   \return true If the variable does not price out correctly.
 *   \return false otherwise.
 *
 *   \param constraints The constraints associated with the dual
 *                          variables \a y.
 *   \param y The dual variables of the constraint.
 *   \param r If \a r is
 *                not the 0-pointer, it will store the reduced cost after the
 *                function call. Per default \a r is 0.
 */
      virtual bool violated(ABA_ACTIVE<ABA_CONSTRAINT, ABA_VARIABLE> *constraints,
                            double *y, double *slack = 0);

/*! \brief Computes the reduced cost of the variable
 *   corresponding the constraint set \a actCon and the dual variables \a y.
 *
 *   Given the dual variables \f$y\f$, then the reduced cost of a variable
 *   with objective function coefficient \f$c_e\f$, column \f$a_{.e}\f$ are defined
 *   as \f$c_e - y^{\mathrm{T}} a_{.e}\f$.
 *
 *   \return The reduced cost of the variable.
 *
 *   \param actCon The constraints associated with the dual
 *                          variables \a y.
 *   \param y The dual variables of the constraint.
 */
      virtual double redCost(ABA_ACTIVE<ABA_CONSTRAINT, ABA_VARIABLE> *actCon, 
                             double *y);

/*! \brief An (inactive) discrete variable is considered as \a useful() if its
 *   activation might not produce only solutions worse than the
 *   best known feasible solution.
 *
 *   This is the same criterion for
 *   fixing inactive variables by reduced cost criteria.
 *
 *   \return true If the variable is considered as useful,
 *   \return false otherwise.
 *
 *      \param actCon The active constraints.
 *      \param y The dual variables of these constraints.
 *      \param lpVal The value of the linear program.
 */
      virtual bool useful(ABA_ACTIVE<ABA_CONSTRAINT, ABA_VARIABLE> *actCon, 
                          double *y,
                          double lpVal);

/*! \brief Writes the column of the variable
 *   corresponding to the \a constraints on the stream \a out.
 *
 *   \param out The output stream.
 *   \param constraints The constraints for which the column should be
 *                          written.
 */
      void printCol(ostream &out, 
                    ABA_ACTIVE<ABA_CONSTRAINT, ABA_VARIABLE> *constraints);

    protected: 

      /*! The global status of fixing and setting of the variable.
       */
      ABA_FSVARSTAT fsVarStat_;

      /*! The objective function coefficient of the variable.
       */
      double obj_;

      /*! The lower bound of the variable.
       */
      double lBound_;

      /*! The upper bound of the variable.
       */
      double uBound_;

      /*! The type of the variable.
       */
      ABA_VARTYPE type_;
  };


inline ABA_VARTYPE::TYPE ABA_VARIABLE::varType() const
  {
    return type_.type();
  }

inline bool ABA_VARIABLE::discrete()
  {
    return type_.discrete();
  }

inline bool ABA_VARIABLE::binary()
  {
    return type_.binary();
  }

inline bool ABA_VARIABLE::integer()
  {
    return type_.integer();
  }

inline double ABA_VARIABLE::lBound() const
  {
    return lBound_;
  }

inline void ABA_VARIABLE::lBound(double newBound)
  {
    lBound_ = newBound;
  }

inline double ABA_VARIABLE::uBound() const
  {
    return uBound_;
  }

inline void ABA_VARIABLE::uBound(double newBound)
  {
    uBound_ = newBound;
  }

inline ABA_FSVARSTAT *ABA_VARIABLE::fsVarStat()
  {
    return &fsVarStat_;
  }


#endif  // ABA_VARIABLE_H

/*! \class ABA_VARIABLE
 *   \brief class forms the virtual base class for all possible variables given in pool format
 */


