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

#ifndef ABA_POOL_INC
#define ABA_POOL_INC

#include "abacus/pool.h"
#include "abacus/poolslot.h"
#include "abacus/master.h"

#ifdef ABACUS_PARALLEL
#include "abacus/idmap.h"
#endif

  template<class BaseType, class CoType>
  inline ABA_POOL<BaseType, CoType>::ABA_POOL(ABA_MASTER *master) 
  :  
    master_(master),  
    number_(0)
  {
#ifdef ABACUS_PARALLEL
    index_ = master_->parmaster()->registerPool(this);
    identificationMap_ = new 
      ABA_IDMAP<ABA_POOLSLOT<BaseType, CoType>  >(master, 1000, index_);
#endif
  }

  template<class BaseType, class CoType>
  inline ABA_POOL<BaseType, CoType>::~ABA_POOL()
  {
#ifdef ABACUS_PARALLEL
    delete identificationMap_;
    master_->parmaster()->unregisterPool(index_);
#endif
  }

  template<class BaseType, class CoType>
  void ABA_POOL<BaseType, CoType>::removeConVar(ABA_POOLSLOT<BaseType, CoType> *slot)
  {
    if (softDeleteConVar(slot)) {
      ABA_POOL<BaseType, CoType>::master_->err() << "removeConVar(): removing constraint from slot failed" << endl;
      exit (Fatal);
    }
  }

  template<class BaseType, class CoType>
  int ABA_POOL<BaseType, CoType>::softDeleteConVar(
                                         ABA_POOLSLOT<BaseType, CoType> *slot)
  {
    if (slot->softDelete() == 0) {
      putSlot(slot);
      --number_;
      return 0;
    }
    else return 1;
  }

 template<class BaseType, class CoType>
  void ABA_POOL<BaseType, CoType>::hardDeleteConVar(ABA_POOLSLOT<BaseType, CoType> 
                                                    *slot)
  {
    --number_;
    slot->hardDelete();
    putSlot(slot);
  }

  template<class BaseType, class CoType>
  inline int ABA_POOL<BaseType, CoType>::number() const
  {
    return number_;
  }


#ifdef ABACUS_PARALLEL

  template<class BaseType, class CoType>
  ABA_POOLSLOT<BaseType, CoType> *ABA_POOL<BaseType, CoType>::findSlot(const ABA_ID &id) const
  {
    return identificationMap_->find(id);
  }

  template<class BaseType, class CoType>
  ABA_IDMAP<ABA_POOLSLOT<BaseType, CoType> > *
    ABA_POOL<BaseType, CoType>::identificationMap() const
  {
    return identificationMap_;
  }

#endif

#endif   // ABA_POOL_INC
