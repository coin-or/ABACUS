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


  template<class BaseType, class CoType>
  inline ABA_POOL<BaseType, CoType>::ABA_POOL(ABA_MASTER *master) 
  :  
    master_(master),  
    number_(0)
  {
  }

  template<class BaseType, class CoType>
  inline ABA_POOL<BaseType, CoType>::~ABA_POOL()
  {
  }

  template<class BaseType, class CoType>
  void ABA_POOL<BaseType, CoType>::removeConVar(ABA_POOLSLOT<BaseType, CoType> *slot)
  {
    if (softDeleteConVar(slot)) {
      const char *_error="removeConVar(): removing constraint from slot failed";
      THROW_PARAM(AlgorithmFailureException,afcPool,_error); 
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



#endif   // ABA_POOL_INC
