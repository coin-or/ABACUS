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

#ifndef ABA_POOLSLOT_INC
#define ABA_POOLSLOT_INC

#include "abacus/poolslot.h"
#include "abacus/pool.h"
#include "abacus/master.h"
#include "abacus/constraint.h"
#include "abacus/variable.h"

#ifdef ABACUS_PARALLEL
#include "abacus/id.h"
#include "abacus/idmap.h"
#endif

  template<class BaseType, class CoType> 
  inline ABA_POOLSLOT<BaseType, CoType>::ABA_POOLSLOT(ABA_MASTER *master, 
                                               ABA_POOL<BaseType, CoType> *pool, 
                                               BaseType *conVar) 
  :  
    master_(master),  
    conVar_(conVar),  
    pool_(pool)
  {
    if (conVar) version_ = 1;
    else        version_ = 0;
  }

  template<class BaseType, class CoType>
  ABA_POOLSLOT<BaseType, CoType>::~ABA_POOLSLOT()
  {
    if (conVar_ && conVar_->nReferences()) {
      ABA_POOLSLOT<BaseType, CoType>::master_->err() << "~ABA_POOLSLOT(): it is not allowed to destruct objects of ";
      ABA_POOLSLOT<BaseType, CoType>::master_->err() << "class ABA_POOLSLOT with a constraint/variable with ";
      ABA_POOLSLOT<BaseType, CoType>::master_->err() << "positive reference counter = " << conVar_->nReferences() << "." << endl;
      exit(Fatal);
    }

    delete conVar_;
  }

#include <limits.h>

  template<class BaseType, class CoType>
  void ABA_POOLSLOT<BaseType, CoType>::insert(BaseType *conVar)
  {
    if (conVar_ != 0) {
      ABA_POOLSLOT<BaseType, CoType>::master_->err() << "ABA_POOLSLOT::insert(): insertion failed, the slot is not void" << endl;
      exit (Fatal);
    }

    if (version_ == ULONG_MAX) {
      ABA_POOLSLOT<BaseType, CoType>::master_->err() << "ABA_POOLSLOT::insert(): insertion failed, ";
      ABA_POOLSLOT<BaseType, CoType>::master_->err() << "maximum version number ULONG_MAX = " << ULONG_MAX;
      ABA_POOLSLOT<BaseType, CoType>::master_->err() << " reached";
      exit (Fatal);
    }

#ifdef ABACUS_PARALLEL
    if (conVar->identification_.isInitialized()) {
      master_->err() << "ABA_POOLSLOT::insert(): the constraint/variable"
                        " must not have an initialized ABA_ID!" << endl;
      exit (Fatal);
    }
#endif
    conVar_ = conVar;
    ++version_;
  }

  template<class BaseType, class CoType>
  int ABA_POOLSLOT<BaseType, CoType>::softDelete()
  {
    if (conVar_ == 0)                  return 0;
    if (conVar_->deletable() == false) return 1;
    hardDelete();
    return 0;
  }

  template<class BaseType, class CoType>
  inline void ABA_POOLSLOT<BaseType, CoType>::hardDelete()
  {
#ifdef ABACUS_PARALLEL
    if (conVar_ && conVar_->identification_.isInitialized()) {
      pool_->identificationMap()->remove(conVar_->identification_);
    }
#endif
    delete conVar_;
    conVar_ = 0;
  }

  template<class BaseType, class CoType>
  inline void ABA_POOLSLOT<BaseType, CoType>::removeConVarFromPool()
  {
    pool_->removeConVar(this);
  }

  template<class BaseType, class CoType>
  inline BaseType * ABA_POOLSLOT<BaseType, CoType>::conVar() const
  {
    return conVar_;
  }

  template<class BaseType, class CoType>
  inline unsigned long ABA_POOLSLOT<BaseType, CoType>::version() const
  {
    return version_;
  }

  template<class BaseType, class CoType>
  inline ABA_MASTER * ABA_POOLSLOT<BaseType, CoType>::master()
  {
    return master_;
  }


#ifdef ABACUS_PARALLEL

  template<class BaseType, class CoType>
  const ABA_ID &ABA_POOLSLOT<BaseType, CoType>::getIdentification(void) const
  {
    if (conVar_ == 0) {
      master_->err() << "ABA_POOLSLOT::getIdentification() : no "
                        "constraint/variable available in this slot!" << endl;
      exit(Fatal);
    }
    return conVar_->identification_;
  }

  template<class BaseType, class CoType>
  void ABA_POOLSLOT<BaseType, CoType>::setIdentification(const ABA_ID &id)
  {
    if (conVar_) {
      conVar_->identification_ = id;
      pool_->identificationMap()->insert(id, this);
    }
    else {
      master_->err() << "ABA_POOLSLOT::setIdentification() : no "
                        "constraint/variable available in this slot!" << endl;
      exit(Fatal);
    }
  }

  template<class BaseType, class CoType>
  void ABA_POOLSLOT<BaseType, CoType>::setNewIdentification()
  {
    if (conVar_)
      pool_->identificationMap()->insertWithNewId(conVar_->identification_, this);
    else {
      master_->err() << "ABA_POOLSLOT::setNewIdentification() : no "
                        "constraint/variable available in this slot!" << endl;
      exit(Fatal);
    }
  }

#endif

#endif   // ABA_POOLSLOT_INC
