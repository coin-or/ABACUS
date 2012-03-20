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

#ifndef ABA_POOLSLOTREF_INC
#define ABA_POOLSLOTREF_INC

#include "abacus/poolslotref.h"
#include "abacus/master.h"
#include "abacus/constraint.h"
#include "abacus/variable.h"
#include "abacus/poolslot.h"

  template<class BaseType, class CoType>
  inline ABA_POOLSLOTREF<BaseType, CoType>::ABA_POOLSLOTREF(ABA_MASTER *master)
  :  
    master_(master),  
    slot_(0),  
    version_(0)
  {  }

  template<class BaseType, class CoType>
  ABA_POOLSLOTREF<BaseType, CoType>::ABA_POOLSLOTREF(
                                         ABA_POOLSLOT<BaseType, CoType> *slot)
  :  
    master_(slot->master()),  
    slot_(slot),  
    version_(slot->version())
    {
      ABA_CONVAR *cv = slot_->conVar();

      if(cv) cv->addReference();
    }

  template<class BaseType, class CoType>
  ABA_POOLSLOTREF<BaseType, CoType>::ABA_POOLSLOTREF(const ABA_POOLSLOTREF &rhs) 
  :  
    master_(rhs.master_),  
    slot_(rhs.slot_),  
    version_(rhs.version_)
    {
      ABA_CONVAR *cv = slot_->conVar();

      if (version_ == slot_->version() && cv)
        cv->addReference();
    }

  template<class BaseType, class CoType>
  ABA_POOLSLOTREF<BaseType, CoType>::~ABA_POOLSLOTREF()
  {
    if(slot_) {
      ABA_CONVAR *cv = slot_->conVar();
      if (cv && version_ == slot_->version())
        cv->removeReference();
    }
  }

  template <class BaseType, class CoType>
  ostream &operator<<(ostream &out, const ABA_POOLSLOTREF<BaseType, CoType> &rhs)
  {
    if (rhs.conVar()) rhs.conVar()->print(out);
    return out;
  }

  template<class BaseType, class CoType>
  inline BaseType* ABA_POOLSLOTREF<BaseType, CoType>::conVar() const
  {
    if(version_ == slot_->version()) return slot_->conVar();
    printDifferentVersionError();
    
    return 0;
  }

  template<class BaseType, class CoType>
  void ABA_POOLSLOTREF<BaseType, CoType>::printDifferentVersionError() const
  {
     ABA_POOLSLOTREF<BaseType, CoType>::master_->out() << "ABA_POOLSLOTREF::con: Version of reference to slot " << version_;
    ABA_POOLSLOTREF<BaseType, CoType>::master_->out() << " and version of slot " << slot_->version() << " differ." << endl;
  }

  template<class BaseType, class CoType>
  inline unsigned long ABA_POOLSLOTREF<BaseType, CoType>::version() const
  {
    return version_;
  }

  template<class BaseType, class CoType>
  inline ABA_POOLSLOT<BaseType, CoType>* ABA_POOLSLOTREF<BaseType, CoType>::slot() const 
  {
    return slot_;
  }

  template<class BaseType, class CoType>
  void ABA_POOLSLOTREF<BaseType, CoType>::slot(ABA_POOLSLOT<BaseType, CoType> *s)
  {
    ABA_CONVAR *cv;

    if(slot_) {
      cv = slot_->conVar();
      if (cv && version_ == slot_->version())
        cv->removeReference();
    }

    slot_    = s;
    version_ = s->version();
    cv       = slot_->conVar();
    if(cv) cv->addReference();
  }

#endif // ABA_POOLSLOTREF_INC
