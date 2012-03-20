/*!\file
 * \author Matthias Elf
 * \brief infeasible constraints.
 *
 * If a constraint is transformed from its pool to the row format
 * it may turn out that the constraint is infeasible since variables
 * are fixed or set such that all nonzero coefficients of the left
 * hand side are eliminated and the right hand side has to be updated.
 * The enumeration \a INFEAS indicates if the
 * constraint's left hand side, which is implicitly zero,
 * is either \a TooLarge, \a Feasible, or \a TooSmall.
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


#ifndef ABA_INFEASCON_H
#define ABA_INFEASCON_H

#include "abacus/abacusroot.h"

class ABA_CONSTRAINT;
class ABA_VARIABLE;
class ABA_MASTER;

  class  ABA_INFEASCON :  public ABA_ABACUSROOT  {
    public:

    /*! The different ways of infeasibility of a constraint.
     *
     * \param TooSmall The left hand side is too small for the right hand side.
     * \param Feasible The constraint is not infeasible.
     * \param TooLarge The left hand side is too large for the right hand side.
     */
      enum INFEAS {TooSmall = -1, Feasible, TooLarge};

/*! The constructor.
 *
 *   \param master A pointer to the corresponding master of the optimization.
 *   \param con The infeasible constraint.
 *   \param inf The way of infeasibility.
*/      
      ABA_INFEASCON(ABA_MASTER *master, ABA_CONSTRAINT *con, INFEAS inf);

/*! \return A pointer to the infeasible constraint.
*/
      ABA_CONSTRAINT *constraint() const;

/*! \return The way of infeasibility of the constraint.
*/
      INFEAS infeas() const;

/*! \return true If the variable \a v might reduce the infeasibility,
 *   \return false otherwise.
 *
 *   \param v A variable for which we test if its addition might reduce
 *                infeasibility.
*/
      bool goodVar(ABA_VARIABLE *v);

    private:

      /*! A pointer to the corresponding master of the optimization.
       */
      ABA_MASTER *master_;

      /*! A pointer to the infeasible constraint.
       */
      ABA_CONSTRAINT *constraint_;

      /*! The way of infeasibility.
       */
      INFEAS infeas_;
  };


inline ABA_CONSTRAINT *ABA_INFEASCON::constraint() const
  {
    return constraint_;
  }

inline ABA_INFEASCON::INFEAS ABA_INFEASCON::infeas() const
  {
    return infeas_;
  }


#endif  // ABA_INFEASCON_H

/*! \class ABA_INFEASCON
 *  \brief If a constraint is transformed from its pool to the row format
 * it may turn out that the constraint is infeasible since variables
 * are fixed or set such that all nonzero coefficients of the left
 * hand side are eliminated and the right hand side has to be updated.
 */

