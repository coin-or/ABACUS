/*!\file
 * \author Matthias Elf
 *
 * Every constraint and variable has to to be stored in a pool. This class
 * implements an abstract template class for a pool, which can be
 * used to store objects of the class ABA_VARIABLE or of the class ABA_CONSTRAINT.
 * A constraint or variable is not directly stored in the pool, but in
 * an ABA_POOLSLOT. Hence, a pool is a collection of pool slots.
 *
 * \par
 * A pool has two template arguments: the \a BaseType and the \a CoType.
 * Only two scenarios make sense in the current context. For a pool
 * storing constraints the \a BaseType is ABA_CONSTRAINT and the \a CoType
 * is ABA_VARIABLE. For a pool storing variables the \a BaseType is
 * ABA_VARIABLE and the corresponding \a CoType is ABA_CONSTRAINT.
 *
 * \par
 * The class ABA_POOL is an abstract class from which concrete classes
 * have to be derived, implementing the data structures for the storage
 * of pool slots. We provide already in the class ABA_STANDARDPOOL
 * a simple but convenient implementation of a pool.
 * We refer to all constraints and variables via the class ABA_POOLSLOTREF.
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

#ifndef ABA_POOL_H
#define ABA_POOL_H

#include "abacus/array.h"

class ABA_MASTER;
class ABA_SUB;
template<class BaseType, class CoType> class ABA_ACTIVE;
template<class BaseType, class CoType> class ABA_CUTBUFFER;
template<class BaseType, class CoType> class ABA_POOLSLOT;


  template<class BaseType, class CoType>
  class  ABA_POOL :  public ABA_ABACUSROOT  { 
    public: 
      enum RANKING {NO_RANK, RANK, ABS_RANK};


/*! The constructor initializes an empty pool.
 *
 * \param master A pointer to the corresponding master of the optimization.
 */ 
      ABA_POOL(ABA_MASTER *master);

//! The destructor.
      virtual ~ABA_POOL();
      virtual ABA_POOLSLOT<BaseType, CoType> *insert(BaseType *cv) = 0;

/*! \brief Removes the constraint/variable stored in
 * a pool slot and adds the slot to the list of free slots.
 *
 * \param slot The pool slot from which the constraint/variable is removed.
 */
      void removeConVar(ABA_POOLSLOT<BaseType, CoType> *slot);

/*! \return The current number of items in the pool.
 */
      int number() const;
      virtual int separate(double *z, 
                           ABA_ACTIVE<CoType, BaseType> *active, 
                           ABA_SUB *sub,
                           ABA_CUTBUFFER<BaseType, CoType> *cutBuffer,
                           double minAbsViolation = 0.001,
                           int ranking = 0) = 0;

   protected: 

/*! \brief Removes the constraint/variable stored
 * in the pool slot \a slot from the pool if the constraint/variable can be
 * deleted.
 *
 * If the constraint/variable can be removed the slot is added to the
 * set of free slots.
 *
 * \return 0 If the constraint/variable could be deleted.
 * \return 1 otherwise.
 *
 * \param slot A pointer to the pool slot from wich the constraint/variable
 *                 should be deleted.
 */
      virtual int softDeleteConVar(ABA_POOLSLOT<BaseType, CoType> *slot);

/*! \brief Removes a constraint/variable from the
 * pool and adds the slot to the set of free slots.
 *
 * \param slot A pointer to the pool slot from wich the constraint/variable
 *                 should be deleted.
 */
      virtual void hardDeleteConVar(ABA_POOLSLOT<BaseType, CoType> *slot);
      virtual ABA_POOLSLOT<BaseType, CoType> *getSlot() = 0;
      virtual void putSlot(ABA_POOLSLOT<BaseType, CoType> *slot) = 0;
      ABA_MASTER *master_;
      int number_;
  };

#include "abacus/pool.inc"

#endif  // ABA_POOL_H

/*! \class ABA_POOL
 * \brief The public enumerations of ABA_POOL
 */

/*!
 * \enum RANKING {NO_RANK, RANK, ABS_RANK}
 *     The enumeration \a RANKING indicates how the rank of a 
 *      constraint/variable in a pool separation is determined.
 *      This enumeration is not used at the moment because
 *        we cannot used the enumeration as the type of a function
 *        parameter in a derived class.
 *      \param NO_RANK No rank is computed.
 *      \param RANK The violation computed by the function
 *                      \a violated() of the classes ABA_CONSTRAINT or ABA_VARIABLE
 *                      is used as rank.
 *      \param ABS\_RANK The absolute value of the violation is taken as rank.
 *
 * \param ABA_MASTER *master_
 *   A pointer to the corresponding master of the optimization.
 *
 * \param int number_ The current number of constraints in the pool.
 */

/*! \fn separate()
 *  \brief checks if a pair of a vector and an active
 *  constraint/variable set violates any item in the pool. 
 *
 * If the pool is 
 * a constraint pool, then the vector is an LP-solution and the active set
 * is the set
 * of active variables. Otherwise, if the pool is a variable pool, then
 * the vector contains the dual variables and the active set
 * is the set of associated active constraints.
 *
 * \return The number  of violated items. 
 *
 * \param z The vector for which violation is checked.
 * \param active The constraint/variable set associated with \a z.
 * \param sub The subproblem for which validity of the violated
 *                item is required.
 * \param cutBuffer The violated constraints/variables are added to this
 *                      buffer.
 * \param minAbsViolation A violated constraint/variable is only
 *                            added to the \a cutBuffer if the absolute
 *                            value of its violation is at least
 *                            \a minAbsViolation.
 *                            The default value is \a 0.001.
 * \param ranking If 1, the violation is associated with a rank of
 *                    item in the buffer, if 2 the absolute violation is
 *                    used, if 0 no rank is associated with the item.
 */

/*!\fn insert()
 * tries to insert a constraint/variable in the pool. 
 *
 * \return A pointer to the pool slot where the item has been
 * inserted, or 0 if the insertion failed.
 *
 * \param cv The constraint/variable being inserted.
 */

/*! \fn getSlot()
 *  tries to find a free slot in the pool.
 *
 * This function is protected since it should be only used
 * by \a insert(). The data structure managing the free poolslots
 * can be individually defined for each derived pool class.
 *
 * \return A pointer to a
 * free ABA_POOLSLOT where a constraint/variable can be inserted.
 * If no pool slot is available \a getSlot() returns 0.
 */

/*! \fn putSlot()
 *  makes an ABA_POOLSLOT again available for later calls of \a getSlot().
 *
 *  If somebody else refers to this
 *  constraint the function should abort with an error message.
 *
 * \param slot The slot made available for further use.
 */
 
