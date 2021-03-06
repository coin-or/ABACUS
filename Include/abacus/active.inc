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

#ifndef ABA_ACTIVE_INC
#define ABA_ACTIVE_INC

#include "abacus/master.h"
#include "abacus/active.h"
#include "abacus/poolslotref.h"
#include "abacus/sparvec.h"
#include "abacus/convar.h"
#include "abacus/poolslot.h"

#ifdef ABACUS_PARALLEL
#include "abacus/message.h"
#include "abacus/constraint.h"
#include "abacus/variable.h"
#include "abacus/debug.h"
#endif	 
			
  template<class BaseType, class CoType> 
  ABA_ACTIVE<BaseType, CoType>::ABA_ACTIVE(ABA_MASTER *master, int max)
  :  
    master_(master), 
    n_(0), 
    active_(master, max),  
    redundantAge_(master, max, 0)
  { }

  template <class BaseType, class CoType>
  ABA_ACTIVE<BaseType, CoType>::ABA_ACTIVE(ABA_MASTER *master,
                                           ABA_ACTIVE<BaseType, CoType> *a,
                                           int max)
    :  
    master_(master), 
    n_(0), 
    active_(master, max),  
    redundantAge_(master, max, 0)
  {
    n_ = max < a->number() ? max : a->number();
    
    for (int i = 0; i < n_; i++)
      active_[i] = new ABA_POOLSLOTREF<BaseType, CoType>(*(a->active_[i]));
  }

  template <class BaseType, class CoType>
  ABA_ACTIVE<BaseType, CoType>::ABA_ACTIVE(const ABA_ACTIVE<BaseType, CoType> &rhs)
  :  
    master_(rhs.master_),  
    n_(rhs.n_),  
    active_(rhs.master_, rhs.max()),  
    redundantAge_(master_, rhs.max(), 0)
  {
    for (int i = 0; i < n_; i++) {
      active_[i] = new ABA_POOLSLOTREF<BaseType, CoType>(*(rhs.active_[i]));
      redundantAge_[i] = rhs.redundantAge_[i];
    }
  }

  template <class BaseType, class CoType>
  ABA_ACTIVE<BaseType, CoType>::~ABA_ACTIVE()
  {
    for (int i = 0; i < n_; i++) 
      delete active_[i];
  }

#ifdef ABACUS_PARALLEL

  template<class BaseType, class CoType>
  ABA_ACTIVE<BaseType, CoType>::ABA_ACTIVE(const ABA_MASTER *master, ABA_MESSAGE &msg,
                                   ABA_BUFFER<ABA_ID> **idBuffer, ABA_BUFFER<int> **needed)
  :  
    master_(master),  
    n_(msg.unpackInt()),  
    active_(master, msg.unpackInt()),  
    redundantAge_(master, msg)
  {
    *idBuffer = new ABA_BUFFER<ABA_ID>(master, n_);
    *needed   = new ABA_BUFFER<int>(master, n_);

    for (int i = 0; i < n_; i++) {
      ABA_ID id(msg);
      (*idBuffer)->push(id);
      if (id.isInitialized()) {
        ABA_POOL<BaseType, CoType> *pool = (ABA_POOL<BaseType, CoType>*)
          master_->parmaster()->getPool(id.index());
        ABA_POOLSLOT<BaseType, CoType> *ps = pool->findSlot(id);
        if (ps)
          active_[i] = new ABA_POOLSLOTREF<BaseType, CoType>(ps);
        else {
          (*needed)->push(i);
          active_[i] = 0;		// will be initialized in unpackNeeded()
        }
      }
      else				// removed conVar
        active_[i] = 0;
    }
  }

  template<class BaseType, class CoType>
  void ABA_ACTIVE<BaseType, CoType>::unpackNeeded(ABA_MESSAGE &msg,
                                              const ABA_BUFFER<ABA_ID> &idBuffer,
                                              const ABA_BUFFER<int> &needed)
  {
    for (int i = 0; i < needed.number(); i++) {
      int classId;
      msg.unpack(classId);
      BaseType *cv = (BaseType*) master_->unpackConVar(msg, classId);
      const ABA_ID &id = idBuffer[needed[i]];
      if (debug(DEBUG_MESSAGE_CONVAR)) {
        master_->out() << "DEBUG_MESSAGE_CONVAR: Constraint/Variable " << id
                       << " (classId=" << classId << ") received." << endl;
      }
      ABA_POOL<BaseType, CoType> *pool = (ABA_POOL<BaseType, CoType>*)
        master_->parmaster()->getPool(id.index());
      ABA_POOLSLOT<BaseType, CoType> *ps = pool->insert(cv);
      if (ps == 0) {
        master_->err() << "ABA_ACTIVE::ABA_ACTIVE(): no room to insert constraint"
                          " into pool." << endl;
        exit(Fatal);
      }
      ps->setIdentification(id);
      active_[needed[i]] = new ABA_POOLSLOTREF<BaseType, CoType>(ps);
    }
  }

  template<class BaseType, class CoType>
  void ABA_ACTIVE<BaseType, CoType>::pack(ABA_MESSAGE &msg) const
  {
    msg.pack(n_);
    msg.pack(active_.size());
    redundantAge_.pack(msg, n_);

    ABA_ID uninitializedId;
    for (int i = 0; i < n_; i++) {
      if (active_[i]->conVar() == 0)	// removed conVar
        uninitializedId.pack(msg);
      else {
        ABA_POOLSLOT<BaseType, CoType> *ps = active_[i]->slot();
        if (!ps->getIdentification().isInitialized())
          ps->setNewIdentification();
        ps->getIdentification().pack(msg);
      }
    }
  }

  template<class BaseType, class CoType>
  void ABA_ACTIVE<BaseType, CoType>::packNeeded(ABA_MESSAGE &msg,
                                            const ABA_BUFFER<int> &needed) const
  {
    for (int i = 0; i< needed.number(); i++) {
      BaseType *cv = active_[needed[i]]->conVar();
      if (debug(DEBUG_MESSAGE_CONVAR)) {
        master_->out() << "DEBUG_MESSAGE_CONVAR: sending Constraint/Variable "
                       << active_[needed[i]]->slot()->getIdentification()
                       << " (classId=" << cv->classId() << ")..." << endl;
      }
      msg.pack(cv->classId());
      cv->pack(msg);
    }
  }

#endif

  template <class BaseType, class CoType>
  ostream &operator<<(ostream &out, const ABA_ACTIVE<BaseType, CoType> &rhs)
  {
    BaseType *cv;
    for (int i = 0; i < rhs.n_; i++) {
      out << i << ": ";
      if (cv = rhs.active_[i]->conVar()) 
        cv->print(out);
      else
        out << "void" << endl;
    }
    return out;
  }

  template <class BaseType, class CoType>
  inline int ABA_ACTIVE<BaseType, CoType>::number() const
  {
    return n_;
  }

  template <class BaseType, class CoType>
  inline int ABA_ACTIVE<BaseType, CoType>::max() const
  {
    return active_.size();
  }

  template <class BaseType, class CoType>
  BaseType*  ABA_ACTIVE<BaseType, CoType>::operator[](int i) 
  {
#ifdef ABACUSSAFE
    if (i > n_) {
      master_->err() << "ABA_ACTIVE::operator[] : no active item in slot " << i << "." << endl;
      exit(Fatal);
    }
#endif

    if (active_[i]) return active_[i]->conVar();
    else            return 0;
  }

  template <class BaseType, class CoType>
  inline ABA_POOLSLOTREF<BaseType, CoType> *  ABA_ACTIVE<BaseType, CoType>::poolSlotRef(int i) 
  {
    return active_[i];
  }

  template <class BaseType, class CoType>
  void ABA_ACTIVE<BaseType, CoType>::insert(ABA_POOLSLOT<BaseType, CoType> *ps)
  {
#ifdef ABACUSSAFE  
    if (n_ == max()) {
      master_->err() << "ABA_ACTIVE::insert(): buffer is full" << endl;
      exit (Fatal);
    }
#endif
   
    active_[n_] = new ABA_POOLSLOTREF<BaseType, CoType>(ps);
    redundantAge_[n_] = 0;
    n_++;
  }

  template <class BaseType, class CoType>
  void ABA_ACTIVE<BaseType, CoType>::insert(
                             ABA_BUFFER<ABA_POOLSLOT<BaseType, CoType> *> &ps)
  {
    const int nPs = ps.number();

    for(int i = 0; i < nPs; i++)
      insert(ps[i]);
  }

  template <class BaseType, class CoType>
  void ABA_ACTIVE<BaseType, CoType>::remove(ABA_BUFFER<int> &del)
  {
    const int nDel = del.number();

    for(int i = 0; i < nDel; i++)
      delete active_[del[i]];
    active_.leftShift(del);
    redundantAge_.leftShift(del);
    n_ -= nDel;
  }

  template <class BaseType, class CoType>
  void ABA_ACTIVE<BaseType, CoType>::realloc(int newSize)
  {
    active_.realloc(newSize);
    redundantAge_.realloc(newSize);
  }

  template <class BaseType, class CoType>
  inline int ABA_ACTIVE<BaseType, CoType>::redundantAge(int i) const
  {
    return redundantAge_[i];
  }

  template <class BaseType, class CoType>
  inline void ABA_ACTIVE<BaseType, CoType>::incrementRedundantAge(int i)
  {
    redundantAge_[i]++;
  }

  template <class BaseType, class CoType>
  inline void ABA_ACTIVE<BaseType, CoType>::resetRedundantAge(int i)
  {
    redundantAge_[i] = 0;
  }

#endif   // ABA_ACTIVE_INC
