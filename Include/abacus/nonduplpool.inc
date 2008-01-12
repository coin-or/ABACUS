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

#ifndef ABA_NONDUPLPOOL_INC
#define ABA_NONDUPLPOOL_INC

#include "abacus/nonduplpool.h"
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
  inline ABA_NONDUPLPOOL<BaseType, CoType>::ABA_NONDUPLPOOL(ABA_MASTER *master, 
                                                     int size, 
                                                     bool autoRealloc)
  :  
    ABA_STANDARDPOOL<BaseType, CoType>(master, size, autoRealloc),  
    hash_(ABA_POOL<BaseType, CoType>::master_, size),  
    nDuplications_(0)
  { }

  template<class BaseType, class CoType>
  ABA_NONDUPLPOOL<BaseType, CoType>::~ABA_NONDUPLPOOL()
  { 
#ifdef ABACUSSAFE
      ABA_POOL<BaseType, CoType>::master_->out() << "Number of duplicated constraints: " <<
          nDuplications_ << endl;
#endif
  }

  template<class BaseType, class CoType>
  ABA_POOLSLOT<BaseType, CoType> * ABA_NONDUPLPOOL<BaseType, CoType>::insert(
                                                               BaseType *cv)
  {
    
    ABA_POOLSLOT<BaseType, CoType>* slot;

    slot = present(cv);
    if (slot == 0) {
      slot = ABA_STANDARDPOOL<BaseType, CoType>::insert(cv);
      if (slot) 
        hash_.insert(cv->hashKey(), slot);
    }
    else {
      delete cv;
      nDuplications_++;
    }
    return slot;
  }

  template<class BaseType, class CoType>
  ABA_POOLSLOT<BaseType, CoType> *ABA_NONDUPLPOOL<BaseType, CoType>::present(
                                                                  BaseType *cv)
  {
    int key = cv->hashKey();

    ABA_POOLSLOT<BaseType, CoType> **cand = hash_.initializeIteration(key);
    while(cand) {
      if (cv->equal((*cand)->conVar())) {
	return *cand;
      }cand = hash_.next(key);
    }
    return 0;
  }

  template<class BaseType, class CoType>
  void ABA_NONDUPLPOOL<BaseType, CoType>::increase(int size)
  {
    ABA_STANDARDPOOL<BaseType, CoType>::increase(size);
    hash_.resize(size);
  }

  template<class BaseType, class CoType>
  int ABA_NONDUPLPOOL<BaseType, CoType>::softDeleteConVar(ABA_POOLSLOT<BaseType, CoType> *slot)
  {
    int key = slot->conVar()->hashKey();

    if (ABA_POOL<BaseType, CoType>::softDeleteConVar(slot) == 0) {
      if (hash_.remove(key, slot)) {
        ABA_POOL<BaseType, CoType>::master_->err() << "ABA_NONDUPLPOOL::softDeleteCon(): slot not ";
        ABA_POOL<BaseType, CoType>::master_->err() << "found in hash table." << endl;
        exit(ABA_ABACUSROOT::Fatal);
      }
      return 0;
    }
    return 1;
  }

  template<class BaseType, class CoType>
  void ABA_NONDUPLPOOL<BaseType, CoType>::hardDeleteConVar(
                                              ABA_POOLSLOT<BaseType, CoType> *slot)
  {
    if (hash_.remove(slot->conVar()->hashKey(), slot)) {
      ABA_POOL<BaseType, CoType>::master_->err() << "ABA_NONDUPLPOOL::hardDeleteConVar(): constraint ";
      ABA_POOL<BaseType, CoType>::master_->err() << "not found in hash table." << endl;
      exit(ABA_ABACUSROOT::Fatal);
    }
    ABA_POOL<BaseType, CoType>::hardDeleteConVar(slot);
  }

  template<class BaseType, class CoType>
  void ABA_NONDUPLPOOL<BaseType, CoType>::statistics(int &nDuplications, 
                                                     int &nCollisions) const
  {
    nDuplications = nDuplications_;
    nCollisions   = hash_.nCollisions();
  }
    

#endif   // ABA_NONDUPLPOOL_INC
