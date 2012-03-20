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
      char *_error=new char[255];
      sprintf(_error,"~ABA_POOLSLOT(): it is not allowed to destruct objects of class ABA_POOLSLOT with a constraint/variable with positive reference counter = %i .", conVar_->nReferences());
      THROW_PARAM(AlgorithmFailureException,afcPoolslot,_error);
    }

    delete conVar_;
  }

#include <limits.h>

  template<class BaseType, class CoType>
  void ABA_POOLSLOT<BaseType, CoType>::insert(BaseType *conVar)
  {
    if (conVar_ != 0) {
      const char *_error ="ABA_POOLSLOT::insert(): insertion failed, the slot is not void";
      THROW_PARAM(AlgorithmFailureException,afcPoolslot,_error);
    }

    if (version_ == ULONG_MAX) {
      const char *_error="ABA_POOLSLOT::insert(): insertion failed, maximum version number ULONG_MAX reached";
      THROW_PARAM(AlgorithmFailureException,afcPoolslot,_error);
    }

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



#endif   // ABA_POOLSLOT_INC
