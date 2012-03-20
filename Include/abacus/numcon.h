/*!\file
 * \author Matthias Elf
 * \brief constraint defined by a number.
 *
 * Like the class ABA_NUMVAR for variables we provide the class ABA_NUMCON
 * for constraints which are uniquely defined by an integer number.
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

#ifndef ABA_NUMCON_H
#define ABA_NUMCON_H

#include "abacus/constraint.h"

  class  ABA_NUMCON :  public ABA_CONSTRAINT  { 
    public: 

/*! The constructor.
 *
 *   \param master A pointer to the corresponding master of the optimization.
 *   \param sub A pointer to the subproblem associated with the
 *                    constraint. This can be also the 0-pointer.
 *   \param sense The sense of the constraint.
 *   \param dynamic If this argument is \a true, then the constraint
 *                           can be removed from the active constraint set
 *                           during the cutting plane phase of the subproblem
 *                           optimization.
 *   \param local If this argument is \a true, then the constraint
 *                        is considered to be only locally valid. As a local
 *                        constraint is associated with a subproblem, \a sub
 *                        must not be 0 if \a local is \a true.
 *       
 *   \param liftable If this argument is \a true, then a lifting
 *                           procedure must be available, i.e., that the
 *                           coefficients of variables which have not been
 *                           active at generation time of the constraint
 *                           can be computed.
 *   \param number The identification number of the constraint.
 *   \param rhs The right hand side of the constraint.
 */
      ABA_NUMCON(ABA_MASTER *master, 
                 const ABA_SUB *sub, 
                 ABA_CSENSE::SENSE sense, 
                 bool dynamic, 
                 bool local, 
                 bool liftable, 
                 int number, 
                 double rhs);

//! The destructor.
      virtual ~ABA_NUMCON();

/*! The output operator writes the identification number and the right hand side to an output stream.
 *
 *   \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param rhs The variable being output.
 */    
      friend ostream &operator<<(ostream &out, const ABA_NUMCON &rhs);

/*! \return The coefficient of the variable \a v.
 *
 *   \param v The variable of which the coefficient is determined.
 *                It must point to an object of the class ABA_COLVAR.
 */
      virtual double coeff(ABA_VARIABLE *v);

/*! Writes the row format of the constraint on  an output stream.
 *
 *   It redefines the virtual function \a print() of the base class
 *   ABA_CONVAR.
 *
 *   \param out The output stream.
 */
      virtual void   print(ostream &out);

/*! \return Returns the identification number of the constraint.
 */
      int number() const;

    private: 
      int    number_;
  };


inline int ABA_NUMCON::number () const
  {
    return number_;
  }


#endif  // ABA_NUMCON_H


/*! \class ABA_NUMCON
 * \brief Like the class ABA_NUMVAR for variables we provide the class ABA_NUMCON
 * for constraints which are uniquely defined by an integer number.
 *
 * \param int number_ The identification number of the constraint.
 */

