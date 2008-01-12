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

#include "abacus/opensub.h"
#include "abacus/sub.h"
#include "abacus/master.h"
#include "abacus/optsense.h"

#ifdef ABACUS_PARALLEL
#include "abacus/parmaster.h"
#include "abacus/notifyserver.h"
#include "abacus/notification.h"
#endif

  ABA_OPENSUB::ABA_OPENSUB(ABA_MASTER *master)
  :  
  master_(master),  
  list_(master),  
  n_(0)
#ifdef ABACUS_PARALLEL
  ,  
  cond_(mutex_),  
  terminate_(false),  
  hasTerminated_(false)
#endif
 {  }

  void ABA_OPENSUB::insert(ABA_SUB *sub)
  {
#ifdef ABACUS_PARALLEL
    mutex_.acquire();
#endif
    // update the dual bound of all subproblems in the \a list_
    if (empty())
      dualBound_ = sub->dualBound();
    else {
      if (master_->optSense()->max()) {
	if (sub->dualBound() > dualBound_)
	  dualBound_ = sub->dualBound();
      }
      else
	if (sub->dualBound() < dualBound_)
	  dualBound_ = sub->dualBound();
    }

    list_.append(sub);
    ++n_;
#ifdef ABACUS_PARALLEL
    cond_.signal();
    mutex_.release();
    master_->parmaster()->newOpenSubCount(n_, dualBound_);
#endif
  }

#ifdef ABACUS_PARALLEL

  void ABA_OPENSUB::terminate()
  {
    terminate_ = true;
    while (!hasTerminated_)
      cond_.signal();
  }
 
  ABA_SUB* ABA_OPENSUB::getSubproblemWithBound(double bound)
{
  mutex_.acquire();

  ABA_DLISTITEM<ABA_SUB*> *item;
  ABA_SUB *s;

  forAllDListElem(list_, item, s) {
    if (s->status() == ABA_SUB::Dormant) {
      s->newDormantRound();
      if (s->nDormantRounds() < master_->minDormantRounds())
        continue;
    }

    // is the bound of s good enough?
    if (master_->optSense()->max()) {
      if (s->dualBound() >= bound)
        break;
    }
    else {
      if (s->dualBound() <= bound)
        break;
    }
  }

  if (item==0) {
    mutex_.release();
    return 0;
  }

  //! subproblem found
  assert(s == item->elem());
  list_.remove(item);

  updateDualBound();
  mutex_.release();

  master_->parmaster()->newOpenSubCount(n_, dualBound_);
  return s;
}

#endif

  ABA_SUB* ABA_OPENSUB::select()
  {
#ifdef ABACUS_PARALLEL
    mutex_.acquire();

    while (!terminate_) {

      // perform load balancing if needed
      mutex_.release();
      for (int i=0; i<3; i++)
        if (master_->parmaster()->balance())
          break;
      mutex_.acquire();

      // select the next subproblem to optimize
      ABA_DLISTITEM<ABA_SUB*> *minItem = list_.first();
      ABA_DLISTITEM<ABA_SUB*> *item;
      ABA_SUB *s;

      forAllDListElem(list_, item, s) {
        if (s->status() == ABA_SUB::Dormant) {
          s->newDormantRound();
          if (s->nDormantRounds() < master_->minDormantRounds())
            continue;
        }
        if (master_->enumerationStrategy(s, minItem->elem()) > 0)
          minItem = item;
      }

      if (minItem != 0) {                 //!< subproblem found
        ABA_SUB* min = minItem->elem();
        list_.remove(minItem);

        updateDualBound();

        mutex_.release();

        master_->parmaster()->newOpenSubCount(n_, dualBound_);
        return min;
      }

      // host zero should poll for termination
      if (master_->parmaster()->isHostZero()) {
        ABA_NOTIFICATION msg(master_, 0);
        msg.pack(ABA_NOTIFYSERVER::TriggerTerminationCheckTag);
        msg.send();
        ACE_Time_Value abstime(0,400000);
        abstime += ACE_OS::gettimeofday();
        master_->parmaster()->startIdleTime();
        cond_.wait(&abstime);                //!< wait at most 0.4 s
        master_->parmaster()->stopIdleTime();
      }
      else {               //!< the other hosts should wait until signaled
        master_->parmaster()->startIdleTime();
        cond_.wait();
        master_->parmaster()->stopIdleTime();
      }
    }

    mutex_.release();

    hasTerminated_ = true;     //!< terminate ABA_OPENSUB::terminate() 

    return 0;

#else
    ABA_DLISTITEM<ABA_SUB*> *minItem = list_.first();
    ABA_DLISTITEM<ABA_SUB*> *item;
    ABA_SUB *s;

    forAllDListElem(list_, item, s) {
      if (s->status() == ABA_SUB::Dormant) {
        s->newDormantRound();
        if (s->nDormantRounds() < master_->minDormantRounds())
          continue;
      }
      if (master_->enumerationStrategy(s, minItem->elem()) > 0)
        minItem = item;
    }

    if (minItem == 0) return 0;

    ABA_SUB* min = minItem->elem();
    list_.remove(minItem);

    --n_;
    
    updateDualBound();

    return min;
#endif
  }

  void ABA_OPENSUB::remove(ABA_SUB* sub)
  {
#ifdef ABACUS_PARALLEL
    mutex_.acquire();
#endif
    if (n_) {
      list_.remove(sub);
      --n_;
      updateDualBound();
    }
#ifdef ABACUS_PARALLEL
    mutex_.release();
    master_->parmaster()->newOpenSubCount(n_, dualBound_);
#endif
  }

  void ABA_OPENSUB::prune()
  {
#ifdef ABACUS_PARALLEL
    mutex_.acquire();
#endif
    while (!list_.removeHead());
    n_ = 0;
#ifdef ABACUS_PARALLEL
    mutex_.release();
    master_->parmaster()->newOpenSubCount(n_, dualBound_);
#endif
  }

  double ABA_OPENSUB::dualBound() const
  {
#ifdef ABACUS_PARALLEL
    mutex_.acquire();
#endif
    double ret;
    if (empty()) {
      if (master_->optSense()->max()) ret = -master_->infinity();
      else                            ret = master_->infinity();
    }
    else
      ret = dualBound_;
#ifdef ABACUS_PARALLEL
    mutex_.release();
#endif
    return ret;
  }

  void ABA_OPENSUB::updateDualBound()
  {

    ABA_DLISTITEM<ABA_SUB*> *item;  //!< to scan the list of subproblems
    ABA_SUB              *s;

#ifdef ABACUS_PARALLEL
    n_ = 0;
#endif
    if (master_->optSense()->max()) {
      dualBound_ = -master_->infinity();
      
      forAllDListElem(list_, item, s) {
#ifdef ABACUS_PARALLEL
        n_++;
#endif
        if (s->dualBound() > dualBound_)
          dualBound_ = s->dualBound();
      }
    }
    else {
      dualBound_ = master_->infinity();
      
      forAllDListElem(list_, item, s) {
#ifdef ABACUS_PARALLEL
        n_++;
#endif
        if (s->dualBound() < dualBound_)
          dualBound_ = s->dualBound();
      }
    }
  }
