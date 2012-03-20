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


  ABA_OPENSUB::ABA_OPENSUB(ABA_MASTER *master)
  :  
  master_(master),  
  list_(master),  
  n_(0)
 {  }

  void ABA_OPENSUB::insert(ABA_SUB *sub)
  {
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
  }


  ABA_SUB* ABA_OPENSUB::select()
  {
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
  }

  void ABA_OPENSUB::remove(ABA_SUB* sub)
  {
    if (n_) {
      list_.remove(sub);
      --n_;
      updateDualBound();
    }
  }

  void ABA_OPENSUB::prune()
  {
    while (!list_.removeHead());
    n_ = 0;
  }

  double ABA_OPENSUB::dualBound() const
  {
    double ret;
    if (empty()) {
      if (master_->optSense()->max()) ret = -master_->infinity();
      else                            ret = master_->infinity();
    }
    else
      ret = dualBound_;
    return ret;
  }

  void ABA_OPENSUB::updateDualBound()
  {

    ABA_DLISTITEM<ABA_SUB*> *item;  //!< to scan the list of subproblems
    ABA_SUB              *s;

    if (master_->optSense()->max()) {
      dualBound_ = -master_->infinity();
      
      forAllDListElem(list_, item, s) {
        if (s->dualBound() > dualBound_)
          dualBound_ = s->dualBound();
      }
    }
    else {
      dualBound_ = master_->infinity();
      
      forAllDListElem(list_, item, s) {
        if (s->dualBound() < dualBound_)
          dualBound_ = s->dualBound();
      }
    }
  }
