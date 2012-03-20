/*!\file
 * \author Matthias Elf
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

#ifndef ABA_STANDARDPOOL_INC
#define ABA_STANDARDPOOL_INC

#include "abacus/cutbuffer.h"
#include "abacus/master.h"
#include "abacus/poolslot.h"
#include "abacus/constraint.h"
#include "abacus/variable.h"
#include "abacus/sub.h"
#include "abacus/bheap.h"

#include <iostream>
using namespace std;


  template<class BaseType, class CoType>
  ABA_STANDARDPOOL<BaseType, CoType>::ABA_STANDARDPOOL(ABA_MASTER *master, 
                                                       int size, 
                                                       bool autoRealloc)
  :  
    ABA_POOL<BaseType, CoType>(master),  
    pool_(master, size),  
    freeSlots_(master),  
    autoRealloc_(autoRealloc)
  {
    for (int i = 0; i < size; i++) {
      pool_[i] = new ABA_POOLSLOT<BaseType, CoType>(master, this);
      freeSlots_.appendTail(pool_[i]);
    }
  }

  template<class BaseType, class CoType>
  ABA_STANDARDPOOL<BaseType, CoType>::~ABA_STANDARDPOOL()
  {
    const int s = size();

    for (int i = 0; i < s; i++) delete pool_[i];
  }



  template<class BaseType, class CoType>
  ostream &operator<<(ostream &out, const ABA_STANDARDPOOL<BaseType, CoType> &rhs)
  {
    const int s = rhs.size();

    for (int i = 0; i < s; i++)
      if (rhs.pool_[i]->conVar()) {
        out << i << ": ";
        rhs.pool_[i]->conVar()->print(out);
        out << endl;
      }

    return out;
  }

  template<class BaseType, class CoType>
  ABA_POOLSLOT<BaseType, CoType> * ABA_STANDARDPOOL<BaseType, CoType>::insert(
                                                               BaseType *cv)
  {
    ABA_POOLSLOT<BaseType, CoType>* slot = getSlot();
    if (slot == 0) {
      if(cleanup() == 0) {
        if (autoRealloc_)
          increase((int) (size()*1.1 + 1));
        else {
          if (removeNonActive(size()/10 + 1) == 0)
            return 0;
        }
      }
      slot = getSlot();
    }

    slot->insert(cv);
  ++ABA_POOL<BaseType, CoType>::number_;
    return slot;
  }

  template<class BaseType, class CoType>
  void ABA_STANDARDPOOL<BaseType, CoType>::increase(int size)
  {
    int oldSize = pool_.size();

    if (size < oldSize) {
      const char *_error= "ABA_STANDARDPOOL::increase(): the pool size cannot be decreased.";
      THROW_PARAM(AlgorithmFailureException,afcStandardPool,_error);
    }

    pool_.realloc(size);

    for(int i = oldSize; i < size; i++) {
      pool_[i] = new ABA_POOLSLOT<BaseType, CoType>(ABA_POOL<BaseType, CoType>::master_, this);
      freeSlots_.appendTail(pool_[i]);
    }
  }

  template<class BaseType, class CoType>
  int ABA_STANDARDPOOL<BaseType, CoType>::cleanup()
  {
    int nDeleted = 0;

    for(int i = 0; i < ABA_POOL<BaseType, CoType>::number(); i++) 
      {
      if(softDeleteConVar(pool_[i]) == 0)
        {
         nDeleted++;
	 // consider the case that a slot has been deleted although it was empty
         // in softDeleteConVar(), number_ was decreased by 1
         if (i != ABA_POOL<BaseType, CoType>::number())
           {
            //exchange the current slot with the last slot of the pool
            ABA_POOLSLOT<BaseType, CoType> *CMslot = pool_[i];
            pool_[i] = pool_[ABA_POOL<BaseType, CoType>::number()];
            pool_[ABA_POOL<BaseType, CoType>::number()] = CMslot;
            i--; // decrease i in order to consider the new current slot
           }
        }
     }

    ABA_POOL<BaseType, CoType>::master_->out() << "ABA_STANDARDPOOL::cleanup(): " << nDeleted << " items removed." << endl;
    return nDeleted;
    
  }

  template<class BaseType, class CoType>
  int ABA_STANDARDPOOL<BaseType, CoType>::removeNonActive(int maxRemove)
  {
    //! prepare the heap storing the candidates
    ABA_BUFFER<int> elems(ABA_POOL<BaseType, CoType>::master_, size());
    ABA_BUFFER<int> keys(ABA_POOL<BaseType, CoType>::master_, size());
    BaseType *cv;

    const int s = size();

    for (int i = 0; i < s; i++) {
      cv = pool_[i]->conVar();
      if (cv && !cv->active() && !cv->locked()) {
        elems.push(i);
	keys.push(cv->nReferences());
      }
    }

    ABA_BHEAP<int, int> candidates(ABA_POOL<BaseType, CoType>::master_, elems, keys);

    //! remove the items with minimal reference counter from the pool
    /*!  Only those items in the pool are candidates which are neither active nor
     *  locked.
     */
    int nRemoved = 0;
    int c;
    
    while(nRemoved < maxRemove && !candidates.empty()) {
      c = candidates.extractMin();  
      hardDeleteConVar(pool_[c]);
      nRemoved++;
    }

    ABA_POOL<BaseType, CoType>::master_->out() << nRemoved << " inactive items removed from pool." << endl;
  
    return nRemoved;

  }

  template<class BaseType, class CoType>
  inline int ABA_STANDARDPOOL<BaseType, CoType>::size() const
  {
    return pool_.size();
  }

  template<class BaseType, class CoType>
  inline ABA_POOLSLOT<BaseType, CoType> *ABA_STANDARDPOOL<BaseType, CoType>::slot(int i)
  {
    return pool_[i];
  }

  template<class BaseType, class CoType>
  inline ABA_POOLSLOT<BaseType, CoType>* ABA_STANDARDPOOL<BaseType, CoType>::getSlot()
  {
    ABA_POOLSLOT<BaseType, CoType> *slot;
    
    if (freeSlots_.extractHead(slot)) return 0;
    return slot;
  }

  template<class BaseType, class CoType>
  void ABA_STANDARDPOOL<BaseType, CoType>::putSlot(ABA_POOLSLOT<BaseType, CoType> *slot)
  {
    if (slot->conVar()) {
      const char *_error="ABA_STANDARDPOOL::putSlot(): you cannot put a non-void slot.";
      THROW_PARAM(AlgorithmFailureException,afcStandardPool,_error);
    }
    freeSlots_.appendHead(slot);
  }

  template<class BaseType, class CoType>
  int ABA_STANDARDPOOL<BaseType, CoType>::separate(
                                          double *z,
                                          ABA_ACTIVE<CoType, BaseType> *active,
                                          ABA_SUB *sub, 
                                          ABA_CUTBUFFER<BaseType, CoType> *cutBuffer,
                                          double minAbsViolation,
                                          int ranking)
  {
    BaseType *cv;
    double    violation;
    int     oldSep = cutBuffer->number(); 

    ABA_POOL<BaseType, CoType>::master_->out() << "ABA_STANDARDPOOL::separate(): ";
    ABA_POOL<BaseType, CoType>::master_->out() << "size = " << size() << " n = " << ABA_POOL<BaseType, CoType>::number_;

    ABA_POOLSLOT<BaseType, CoType> *slot;
    const int s = size();

    for (int i = 0; i < s; i++) {
      slot = pool_[i];
      cv = slot->conVar();
      if (cv && !cv->active() && (cv->global() ||  cv->valid(sub))) 
        if (cv->violated(active, z, &violation) 
            && fabs(violation) > minAbsViolation) {
          if (ranking == 0) {
            if (cutBuffer->insert(slot, true))
              break;
          }
          else if (ranking == 1) {
            if (cutBuffer->insert(slot, true, violation))
              break;
          }
          else if (ranking == 2) {
            if (cutBuffer->insert(slot, true, fabs(violation)))
              break;
          }
	  else if (ranking == 3) {
            if (cutBuffer->insert(slot, true, cv->rank()))
              break;
          }
        }
    }

    ABA_POOL<BaseType, CoType>::master_->out() << " generated = " << cutBuffer->number() - oldSep << endl;
    return cutBuffer->number() - oldSep;
  }

#endif   // ABA_STANDARDPOOL_INC
