/*!\file
 * \author Matthias Elf
 * \brief dual bound.
 *
 * The class ABA_DUALBOUND implements the abstract datatype for holding
 * up to \f$n\f$ dual bounds \f$\{d_1,\dots,d_n\}\f$. A dual bound \f$d_i\f$ can be
 * inserted or removed. The best bound (minimum or maximum) can be queried.
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

#ifndef ABA_DUALBOUND_H
#define ABA_DUALBOUND_H
#include "abacus/intset.h"
class ABA_GLOBAL;
  class  ABA_DUALBOUND  { 
    public: 

/*! The constructor.
 *
 *  \param glob A pointer to the corresponding global object.
 */
      ABA_DUALBOUND(ABA_GLOBAL *glob);

//! The destructor deletes the allocated memory.
      ~ABA_DUALBOUND();

/*! This function initializes the set of dual bounds.
 *
 *  \param n The object can hold up to n dual bounds.
 *  \param minIsBest If this parameter is true the
 *  function \a best() returns the minimum of the dual bounds,
 *  otherwise it returns the maximum.
 */
      void initialize(int n, bool minIsBest);

/*! Inserts a dual bound in the set.
 *
 *  If a bound with the same index already exists in the set
 *  the value of the bound is updated.
 *
 *  \param i The index of the dual bound (0..n-1).
 *  \param d The dual bound.
 */
      void insert(int i, double d);

/*! Removes a dual bound from the set.
 *
 *  \param i The index of the dual bound (0..n-1).
 */
      void remove(int i);

/*! \param i The index of the dual bound (0..n-1).
 *  \param d The value of the dual bound to be tested.
 *
 *  \return true if \a d is a new dual bound or \a d is better than the
 *                  previous dual bound at index \a i|,}
 *  \return false otherwise.
 */
      bool better(int i, double d) const;

/*! Returns the best dual bound (minimum or maximum) of all dual bounds in the set.
 *
 *  \param index An pointer to an integer to which the index of the
 *                 best dual bound should be stored. This is an optional
 *                 parameter.
 *
 *  \return The best dual bound.
 */
      double best(int *index = 0) const;

/*! \brief This version of the function \a best()
 *  returns the best dual bound (minimum
 *  or maximum) of all dual bounds in the set and a specified dual bound.
 *
 *  \param d An additional dual bound.
 *
 *  \return The best dual bound.
 */
      double best(double d) const;

/*! Returns the worst dual bound (minimum or maximum) of all dual bounds in the set.
 *
 *  \return The worst dual bound.
 */
      double worst() const;

    private: 

/*! Updates the variables \a best_|, \a worst_ and \a bestIndex_.
 */
      void       updateBestAndWorst();
      ABA_GLOBAL *glob_;
      ABA_INTSET set_;
      double     *bounds_;
      double     best_;
      int        bestIndex_;
      double     worst_;
      bool       minIsBest_;
  };

inline double ABA_DUALBOUND::best(int *index) const
{
  if (index)
    *index = bestIndex_;
  return best_;
}

inline double ABA_DUALBOUND::worst() const
{
  return worst_;
}

#endif	// ABA_DUALBOUND_H


/*! \class ABA_DUALBOUND
 *  \brief class ABA_DUALBOUND implements the abstract datatype for holding
 * up to \f$n\f$ dual bounds \f$\{d_1,\dots,d_n\}\f$. A dual bound \f$d_i\f$ can be
 * inserted or removed.
 */
