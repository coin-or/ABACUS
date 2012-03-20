/*!\file
 * \author Matthias Elf
 * \brief cutbuffer.
 *
 * This template class implements a buffer for constraints and variables which are
 * generated during the cutting plane or column generation phase.
 * There are two reasons why constraints/variables are buffered instead
 * of being added immediately. First, the set of active constraints/variables
 * should not be falsified during the cut/variable generation. Second,
 * optionally a rank can be assigned to each buffered item. Then
 * not all, but only the best items according to this rank are actually
 * added.
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

#ifndef ABA_CUTBUFFER_H
#define ABA_CUTBUFFER_H

#include "abacus/array.h"

class ABA_MASTER;
template<class BaseType, class CoType> class ABA_POOLSLOT;
template<class BaseType, class CoType> class ABA_POOLSLOTREF;

  template<class BaseType, class CoType>
  class ABA_CUTBUFFER : public ABA_ABACUSROOT  { 
    friend class ABA_SUB;
    public: 

/*! The constructor.
 *
 *   \param master A pointer to the corresponding master of the optimization.
 *   \param size The maximal number of constraints/variables which
 *                     can be buffered.
 */
      ABA_CUTBUFFER(ABA_MASTER *master, int size);

/*! The destructor.
 *
 * If there are still items buffered when this object is destructed then
 * we have to unset the locks of the buffered items. This can happen if
 * in the feasibility test constraints are generated but for some reason
 * (e.g., due to tailing off) the optimization of the subproblem is 
 * terminated.
 */
      ~ABA_CUTBUFFER();

/*! \return The maximal number of items that can be buffered.
 */
      int size() const;

/*! \return The number of buffered items.
 */
      int number() const;
    

/*! \return The number of items which can still be inserted into the buffer.
 */
      int space() const;

/*! Adds a slot to the buffer.
 *
 * The member \a ranking_ has to be set to \a false, because since no rank
 * is added together with this item a ranking of all items is impossible.
 * Such that newly generated items cannot be removed immediately in
 * a cleaning up process of the pool we set a lock which will be
 * removed in the function \a extract().
 *
 * \return 0 If the item can be inserted.
 * \return 1 If the buffer is already full.
 *
 * \param slot The inserted pool slot.
 * \param keepInPool If the flag \a keepInPool is \a true, 
 *                      then the item stored in the
 *                       \a slot is not removed from the pool, even if it is 
 *                       discarded in \a extract(). Items regenerated from a 
 *                       pool should always have this flag set to \a true.
 */
      int insert(ABA_POOLSLOT<BaseType, CoType> *slot, bool keepInPool);

/*! \brief In addition to the previous version of the function \a insert()
 * this version also adds a rank to the item
 * such that all buffered items can be sorted with the function
 * \a sort().
 *
 * \return 0 If the item can be inserted.
 * \return 1 If the buffer is already full.
 *
 * \param rank A rank associated with the constraint/variable.
 */
      int insert(ABA_POOLSLOT<BaseType, CoType> *slot, bool keepInPool, 
                 double rank);

/*! Removes the specified elements from the buffer.
 *
 * \param index The numbers of the elements which should be removed.
 */
      void remove(ABA_BUFFER<int> &index);

/*! \return A pointer to the \a i-th ABA_POOLSLOT that is buffered.
 */
      ABA_POOLSLOT<BaseType, CoType> *slot(int i);

    private: 

/*! Takes the first \a max items from the buffer and clears the buffer. 
 *
 * Constraints or variables stored in slots which
 * are not extracted are also removed from their pools if
 * \a keepInPool has not been set to \a true at insertion time.
 *
 * \param max The maximal number of extracted items.
 * \param newSlots The extracted items are inserted into this buffer.
 */
      void extract(int max, ABA_BUFFER<ABA_POOLSLOT<BaseType, CoType>*> &newSlots);

/*! Sorts the items according to their ranks.
 *
 * \param threshold Only if more than \a threshold items are buffered,
 *                   the sorting is performed.
 */
      void sort(int threshold);


/*! A pointer to the corresponding master of the optimization.
 */
      ABA_MASTER *master_;

/*! The number of buffered items.
 */
      int     n_;

/*! References to the pool slots of the buffered constraints/variables.
 */
      ABA_ARRAY<ABA_POOLSLOTREF<BaseType, CoType>*> psRef_;

/*! \brief If \a keepInPool_[i] is \a true for a buffered constraint/variables,
 *  then it is not removed from its pool although it might be
 *  discarded in \a extract().
 */
      ABA_ARRAY<bool>         keepInPool_;

/*! This array stores optionally the rank of the buffered items.
 */
      ABA_ARRAY<double>       rank_;

/*! \brief This flag is \a true if a rank for each buffered item is available.
 *   As soon as an item without rank is inserted it becomes \a false.
 */
      bool                ranking_;
      ABA_CUTBUFFER(const ABA_CUTBUFFER<BaseType, CoType> &rhs);
      const ABA_CUTBUFFER<BaseType, CoType> 
            &operator=(const ABA_CUTBUFFER<BaseType, CoType> &rhs);
  };

#include "abacus/cutbuffer.inc"

#endif  // !ABA_CUTBUFFER_H

/*! \class ABA_CUTBUFFER
 *  \brief template class implements a buffer for constraints and variables which are
 * generated during the cutting plane or column generation phase.
 */


