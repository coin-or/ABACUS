/*!\file
 * \author Matthias Elf
 * \brief standard pool.
 *
 * This class is derived from the class ABA_POOL and provides a very
 * simple implementation of a pool which is sufficient for a large
 * class of applications. The pool slots are stored in an array
 * and the set of free slots is managed by a linear list.
 *
 * \par
 * A standard pool can be static or dynamic. A static standard pool
 * has a fixed size, whereas a dynamic standard pool is automatically
 * enlarged by ten percent if it is full and an item is inserted.
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
#ifndef ABA_STANDARDPOOL_H
#define ABA_STANDARDPOOL_H

#include "abacus/list.h"
#include "abacus/pool.h"

#ifdef ABACUS_PARALLEL
class ABA_MESSAGE;
#endif

#ifdef ABACUS_NEW_TEMPLATE_SYNTAX
template<class BaseType, class CoType>
class ABA_STANDARDPOOL;

template<class BaseType, class CoType>
ostream& operator<< (ostream &out, const ABA_STANDARDPOOL<BaseType,CoType> &rhs);
#endif

template<class BaseType, class CoType> 
		    class ABA_STANDARDPOOL:public ABA_POOL<BaseType,CoType> {
public:
  
/*! The constructor for an empty pool.
 *
 * All slots are inserted in the linked list of free slots.
 *
 * \param master A pointer to the corresponding master of the optimization.
 * \param size The maximal number of items which can be inserted in
 *                     the pool without reallocation.
 * \param autoRealloc If this argument is \a true an automatic
 *                            reallocation is performed if the pool is
 *                            full.
 */
  ABA_STANDARDPOOL(ABA_MASTER*master,int size,bool autoRealloc= false);

/*! \brief The destructor
 * deletes all slots. The destructor of a pool slot 
 * deletes then also the respective constraint or variable.
 */
  virtual~ABA_STANDARDPOOL();
#ifdef ABACUS_PARALLEL

/*! \brief The message constructor
 *  creates the ABA_STANDARDPOOL from an ABA_MESSAGE.
 *
 * \param master A pointer to the corresponding master of the optimization.
 * \param msg The message from which the object is initialized.
 */
  ABA_STANDARDPOOL(const ABA_MASTER*master,ABA_MESSAGE&msg);

/*! Packs all data of the pool in an ABA_MESSAGE object.
 *
 * \param msg The ABA_MESSAGE object in which the pool is packed.
 */
  void pack(ABA_MESSAGE&msg)const;
#endif
#ifdef ABACUS_NEW_TEMPLATE_SYNTAX

/*! \brief The output operator
 * calls the output operator of each item of a non-void pool slot.
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param rhs The pool being output.
 */
  friend ostream& operator<< <> (ostream &,const ABA_STANDARDPOOL &);
#else
  
/*! \brief The output operator
 * calls the output operator of each item of a non-void pool slot.
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param rhs The pool being output.
 */
  friend ostream& operator<<(ostream& out,const ABA_STANDARDPOOL& rhs);
#endif
  

/*! Tries to insert a constraint/variable in the pool.
 *
 * If there is no free slot available, we try to
 * generate free slots by removing redundant items, i.e., items
 * which have no reference to them.
 * If this fails, we either perform
 * an automatic reallocation of the pool or remove non-active items.
 *
 * \return A pointer to the pool slot where the item has been
 * inserted, or 0 if the insertion failed.
 *
 * \param cv The constraint/variable being inserted.
 */
  virtual ABA_POOLSLOT<BaseType,CoType> *insert(BaseType*cv);

/*! Enlarges the pool to store.
 * 
 * To avoid fatal errors we do not allow
 * decreasing the size of the pool.
 *
 * \param size The new size of the pool.
 */
  virtual void increase(int size);

/*! \brief Scans the pool, removes all deletable items,
 * i.e., those items without having references, and
 * adds the corresponding slots to the list of free slots.
 *
 * \return The number of ``cleaned'' slots.
 */
  int cleanup();

/*! \return The maximal number of constraints/variables
 * that can be inserted in the pool.
 */
  int size()const;

/*! \return A pointer to the \a i-th slot in the pool.
 *
 * \param i The number of the slot being accessed.
 */
  ABA_POOLSLOT<BaseType,CoType> *slot(int i);

/*! \brief Checks if a pair of a vector and an active
 * constraint/variable set violates any item in the pool. If the pool is 
 * a constraint pool, then the vector is an LP-solution and the active set
 * the set
 * of active variables. Otherwise, if the pool is a variable pool, then
 * the vector stores the values of the dual variables and the active set
 * the associated active constraints.
 *
 * \par
 * Before a constraint or variable is generated
 * we check if it is valid for the subproblem \a sub.
 *
 * \par
 * The function defines the pure virtual function of the base class ABA_POOL.
 *
 * \par
 * This is a very simple version of the pool separation. Future versions
 * might scan a priority queue of the available constraints until
 * a limited number of constraints is tested or separated.
 *
 * \return The number of violated items. 
 *
 * \param z The vector for which violation is checked.
 * \param active The constraint/variable set associated with \a z.
 * \param sub The subproblem for which validity of the violated
 *               item is required.
 * \param cutBuffer The violated constraints/variables are added to this
 *                      buffer.
 * \param minAbsViolation A violated constraint/variable is only
 *                            added to the \a cutBuffer if the absolute
 *                            value of its violation is at least
 *                            \a minAbsViolation.
 *                            The default value is \a 0.001.
 * \param ranking If 1, the violation is associated with a rank of
 *                    item in the buffer, if 2 the absolute violation is
 *                    used, if 3 the function ABA_CONVAR::rank() is used,
 *		      if 0 no rank is associated with the item.
 */
  virtual int separate(double*x,
		       ABA_ACTIVE<CoType,BaseType> *active,
		       ABA_SUB*sub,
		       ABA_CUTBUFFER<BaseType,CoType> *cutBuffer,
		       double minAbsViolation= 0.001,
		       int ranking= 0);
		    protected:
  

/*! Tries to remove at most \a maxRemove  inactive items from the pool.
 *
 * A minimum heap of the items with the reference counter as 
 * key is built up and items are removed in this order.
 */
  int removeNonActive(int maxRemove);

/*! \brief Returns a free slot, or 0 if no free slot 
 * is available. A returned slot is removed from
 * the list of free slots.
 *
 * This function defines the pure virtual function of the base class ABA_POOL.
 */
  virtual ABA_POOLSLOT<BaseType,CoType> *getSlot();

/*! Inserts the \a slot in the list of free slots.
 *
 * It is an error to insert a slot which is not empty.
 * 
 * This function defines the pure virtual function of the base class ABA_POOL.
 */
  virtual void putSlot(ABA_POOLSLOT<BaseType,CoType> *slot);
  
  /*! The array with the pool slots.
   */
  ABA_ARRAY<ABA_POOLSLOT<BaseType,CoType> *> pool_;

  /*! The linked lists of unused slots.
   */
  ABA_LIST<ABA_POOLSLOT<BaseType,CoType> *> freeSlots_;

  /*! If the pool becomes full and this member is
   *  \a true, then an automatic reallocation is
   *  performed.
   */
  bool autoRealloc_;
  
  
private:
  ABA_STANDARDPOOL(const ABA_STANDARDPOOL&rhs);
  const ABA_STANDARDPOOL&operator= (const ABA_STANDARDPOOL&rhs);
};



#include "abacus/standardpool.inc"

#endif // ABA_STANDARDPOOL_H

/*! \class ABA_STANDARDPOOL
 *  \brief class provides a very simple implementation of a pool which is sufficient for a large
 * class of applications. pool slots stored in array, set of free slots is managed by a linear list
 */
