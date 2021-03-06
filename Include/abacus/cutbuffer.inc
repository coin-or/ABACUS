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

#ifndef ABA_CUTBUFFER_INC
#define ABA_CUTBUFFER_INC

#include "abacus/poolslotref.h"
#include "abacus/poolslot.h"
#include "abacus/cutbuffer.h"
#include "abacus/master.h"
#include "abacus/variable.h"
#include "abacus/constraint.h"
#include "abacus/sorter.h"

  template<class BaseType, class CoType> 
  ABA_CUTBUFFER<BaseType, CoType>::ABA_CUTBUFFER(ABA_MASTER *master, int size) 
  :  
    master_(master),  
    n_(0),  
    psRef_(master, size),  
    keepInPool_(master, size),  
    rank_(master, size),  
    ranking_(true)
  { }

  template<class BaseType, class CoType>
  ABA_CUTBUFFER<BaseType, CoType>::~ABA_CUTBUFFER()
  {
    for (int i = 0; i < n_; i++) {
      psRef_[i]->conVar()->unlock();
      delete psRef_[i];
    }
  }

  template<class BaseType, class CoType>
  inline int ABA_CUTBUFFER<BaseType, CoType>::size() const
  {
    return psRef_.size();
  }

  template<class BaseType, class CoType>
  inline int ABA_CUTBUFFER<BaseType, CoType>::number() const
  {
    return n_;
  }

  template<class BaseType, class CoType>
  inline int ABA_CUTBUFFER<BaseType, CoType>::space() const
  {
    return size() - n_;
  }

  template<class BaseType, class CoType>
  inline ABA_POOLSLOT<BaseType, CoType> * ABA_CUTBUFFER<BaseType, CoType>::slot(int i)
  {
    return psRef_[i]->slot();
  }

  template<class BaseType, class CoType>
  int ABA_CUTBUFFER<BaseType, CoType>::insert(ABA_POOLSLOT<BaseType, CoType> *slot, 
                                              bool keepInPool)
  {
    if (n_ == size())
      return 1;
    else {
      psRef_[n_]      = new ABA_POOLSLOTREF<BaseType, CoType>(slot);
      keepInPool_[n_] = keepInPool;
      ranking_        = false;
      slot->conVar()->lock();
      ++n_;
      return 0;
    }
  }

  template<class BaseType, class CoType>
  int ABA_CUTBUFFER<BaseType, CoType>::insert(ABA_POOLSLOT<BaseType, CoType> *slot, 
                                              bool keepInPool, 
                                              double rank)
  {
    if (n_ == size())
      return 1;
    else {
      psRef_[n_]      = new ABA_POOLSLOTREF<BaseType, CoType>(slot);
      keepInPool_[n_] = keepInPool;
      rank_[n_]       = rank;
      ++n_;
      slot->conVar()->lock();
      return 0;
    }
  }

  template<class BaseType, class CoType>
  void ABA_CUTBUFFER<BaseType, CoType>::remove(ABA_BUFFER<int> &index)
  {

    ABA_POOLSLOT<BaseType, CoType> *ps;
    ABA_POOLSLOTREF<BaseType, CoType> *psr;

    const int nIndex = index.number();
    
    for (int i = 0; i < nIndex; i++) {
      psr = psRef_[index[i]];
      psr->conVar()->unlock();
      ps = psr->slot();
      delete psr;
      if (ps->conVar()->deletable())
        ps->removeConVarFromPool();
    }
    psRef_.leftShift(index);
    keepInPool_.leftShift(index);
    rank_.leftShift(index);

    n_ -= nIndex;
  }

  template<class BaseType, class CoType>
  void ABA_CUTBUFFER<BaseType, CoType>::sort(int threshold)
  {
    if (ranking_) {
      if (n_ > threshold) {
        // sort the buffered items
	ABA_SORTER<int, double> sorter(master_);
	ABA_ARRAY<int>          index(master_, n_);
	ABA_ARRAY<double>       keys(master_, n_);

	for (int i = 0; i < n_; i++) {
	  index[i] = i;
	  keys[i]  = -rank_[i];
	}
          
      sorter.quickSort(n_, index, keys);

        // reorder the buffered items
	ABA_ARRAY<ABA_POOLSLOTREF<BaseType, CoType>*> psRefSorted(master_, n_);
	ABA_ARRAY<bool> keepInPoolSorted(master_, n_);

#ifdef ABACUS_NO_FOR_SCOPE
        for (i = 0; i < n_; i++) {
#else
	for (int i = 0; i < n_; i++) {
#endif
	  psRefSorted[i]      = psRef_[index[i]];
	  keepInPoolSorted[i] = keepInPool_[index[i]];
	}

#ifdef ABACUS_NO_FOR_SCOPE
        for (i = 0; i < n_; i++) {
#else
	for (int i = 0; i < n_; i++) {
#endif
	  psRef_[i]      = psRefSorted[i];
	  keepInPool_[i] = keepInPoolSorted[i];
	}

	ABA_CUTBUFFER<BaseType, CoType>::master_->out(1) << "items ranked: accepted in " << -keys[0] << " ... ";
	ABA_CUTBUFFER<BaseType, CoType>::master_->out()  << -keys[threshold - 1] << ", rejected in ";
	ABA_CUTBUFFER<BaseType, CoType>::master_->out()  << -keys[threshold] << " ... " << -keys[n_ - 1] << endl;

      }
      else
        ABA_CUTBUFFER<BaseType, CoType>::master_->out(1) << "not enough items, no ranking required" << endl;
    }
    else
      ABA_CUTBUFFER<BaseType, CoType>::master_->out(1) << "ranking of buffered items not possible" << endl;
  }

  template<class BaseType, class CoType>
  void ABA_CUTBUFFER<BaseType, CoType>::extract(int max,
                                            ABA_BUFFER<ABA_POOLSLOT<BaseType, CoType>*> &newSlots)
  {
    // unlock the buffered items
    for (int i = 0; i < n_; i++)
      psRef_[i]->conVar()->unlock();

    // determine the number of items to extract
    int nExtract;

    if (n_ < max) nExtract = n_;
    else          nExtract = max;

    // delete the nonextracted items  
    /* We have to check if the constraint/variable can be deleted, because the
     * pool slot might be shared with another constraint/variable that is not 
     * deleted.
     *
     * The deletion of the extracted items must be performed before the
     * deletion of the non-extracted ones. Otherwise if a \a NONDUPLPOOL
     * is used, it can happen that a constraint is removed from the pool 
     * that is the duplicate of an extracted one. 
     */
     ABA_POOLSLOT<BaseType, CoType> *s;

#ifdef ABACUS_NO_FOR_SCOPE  
     for (i = nExtract; i < n_; i++)
#else
     for (int i = nExtract; i < n_; i++)
#endif
       if (!keepInPool_[i]) {
         s = psRef_[i]->slot();
	 delete psRef_[i];
	 if (s->conVar()->deletable())
           s->removeConVarFromPool();
       }
       else delete psRef_[i];

       n_ = 0;

       // extract the items
#ifdef ABACUS_NO_FOR_SCOPE
       for (i = 0; i < nExtract; i++) {
#else
       for (int i = 0; i < nExtract; i++) {
#endif
	 newSlots.push(psRef_[i]->slot());
	 delete psRef_[i];
       }

      // allow ranking in next iteration
      ranking_ = true;

    }
  
  
  
      
#endif // ABA_CUTBUFFER_INC
