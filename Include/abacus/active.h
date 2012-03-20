/*!\file
 * \author Matthias Elf
 *
 * This template class implements the sets of active constraints and variables 
 * which are associated with each subproblem. Note, also an inactive 
 * subproblem can have an active set of constraints and variables, e.g., 
 * the sets with which its unprocessed sons in the enumeration tree 
 * are initialized.  
 *
 * \par
 * If an active set of constraints is instantiated then the \a BaseType
 * should be ABA_CONSTRAINT and the \a CoType should be ABA_VARIABLE,  
 * for an active set of variables this is vice versa.  
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

#ifndef ABA_ACTIVE_H
#define ABA_ACTIVE_H
#include "abacus/array.h"
#include "abacus/exceptions.h"
class ABA_MASTER;
class ABA_SPARVEC;

template<class BaseType, class CoType> class ABA_POOLSLOT;
template<class BaseType, class CoType> class ABA_POOLSLOTREF;

template<class BaseType,class CoType>
class ABA_ACTIVE;

template<class BaseType,class CoType>
ostream&operator<< (ostream &out, const ABA_ACTIVE<BaseType, CoType> &rhs);

template <class BaseType, class CoType>
		   class  ABA_ACTIVE : public ABA_ABACUSROOT  {
		   public:
  
  /*! The constructor.
   *
   *  \param master A pointer to the corresponding master of the optimization.
   *  \param max The maximal number of active constraints/variables.
   */
  ABA_ACTIVE(ABA_MASTER *master, int max);

/*! \brief In addition to the previous constructor, this constructor
 * initializes the active set.
 *
 *   \param master A pointer to the corresponding master of the optimization.
 *   \param a At most \a max active constraints/variables are taken
 *                from this set.
 *   \param max The maximal number of active constraints/variables.
 */
  ABA_ACTIVE(ABA_MASTER *master, ABA_ACTIVE *a, int max);

/*! The copy constructor.
 *
 * \param rhs The active set that is copied.
 */
  ABA_ACTIVE(const ABA_ACTIVE<BaseType, CoType> &rhs);

//! The destructor.
  ~ABA_ACTIVE();

/*! \brief The output operator
 * writes all active constraints and variables
 * to an output stream. 
 *
 * If an associated pool slot is void, or the item
 * is newer than the one we refer to, then \a "void" is output.
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param rhs The active set being output.
 */
  friend ostream &operator<< <> (ostream &, 
				 const ABA_ACTIVE<BaseType, CoType> &);

/*! \return The current number of active items.
 */
  int number() const;

/*! \return The maximum number of storable active items (without reallocation).
 */
  int max() const;

/*! The operator \a [].
 *
 * \return A pointer to the \a i-th active item or
 * \return 0 if this item has been removed in the meantime.
 *
 * \param i The number of the active item.
 */
  BaseType* operator[](int i);

/*! \return The \a i-th entry in the ABA_ARRAY \a active.
 *
 * \param i The number of the active item.
 */
  ABA_POOLSLOTREF<BaseType, CoType>* poolSlotRef(int i);

/*! Adds a constraint/variable to the active items.
 *
 * \param ps The pool slot storing the constraint/variable being added.
 */
  void insert(ABA_POOLSLOT<BaseType, CoType> *ps);

/*! \brief Is overloaded that also several items
 * can be added at the same time.
 *
 * \param ps The buffer storing the pool slots of all constraints/variables
 *               that are added.
 */
  void insert(ABA_BUFFER<ABA_POOLSLOT<BaseType, CoType> *> &ps);

/*! \brief Removes items from the list of active items.
 *
 * \param del The numbers of the items that should be removed. These
 *                numbers must be upward sorted.
 */
  void remove(ABA_BUFFER<int> &del);

/*! \brief Changes the maximum number of 
 *  active items which can be stored in an object of this class.
 *
 * \param newSize The new maximal number of active items.
 */
  void realloc(int newSize);

/*! \return The number of iterations a constraint/variable is already redundant.
 */
  int redundantAge(int i) const;

/*! \brief Increments the number of
 * iterations the item \a i is already redundant by 1.
 *
 * \param i The index of the constraint/variable.
 */
  void incrementRedundantAge(int i);

/*! \briefSets the number of iterations item \a i is redundant to 0.
 *
 * \param i The index of the constraint/variable.
 */
  void resetRedundantAge(int i);
  
		   private: 
  /*! A pointer to corresponding master of the optimization.
   */
  ABA_MASTER     *master_;
  /*! The number of active items.
   */
  int                  n_;

  /*! The array storing references to the pool slots of the active items.
   */
  ABA_ARRAY<ABA_POOLSLOTREF<BaseType, CoType> *>  active_;

  /*! The number of iterations a constraint is already redundant.
   */
  ABA_ARRAY<int>                                  redundantAge_;
  const ABA_ACTIVE<BaseType, CoType> 
    &operator=(const ABA_ACTIVE<BaseType, CoType> & rhs);
};


#include "abacus/active.inc"

#endif  // ABA_ACTIVE_H

/*! \class ABA_ACTIVE
 *  \brief template class implements the sets of act
ive constraints and variables which are associated w
ith each subproblem
 */


