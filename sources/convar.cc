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

#include "abacus/master.h"
#include "abacus/sub.h"
#include "abacus/convar.h"
#include "abacus/active.h"

#ifdef ABACUS_PARALLEL
#include "abacus/message.h"
#include "abacus/idmap.h"
#endif

ABA_CONVAR::ABA_CONVAR(ABA_MASTER *master, const ABA_SUB *sub, bool dynamic, bool local) 
    :  
    master_(master),  
    sub_(sub),  
    expanded_(false),  
    nReferences_(0),  
    dynamic_(dynamic),  
    nActive_(0),  
    nLocks_(0),  
    local_(local)
  {
#ifdef ABACUS_PARALLEL
    if (local) {
      master_->err() << "ABA_CONVAR::ABA_CONVAR(): local constraints/variables"
                     << " are not suported in the parallel version of ABACUS."
                     << endl;
      exit(Fatal);
    }
#endif
  }

ABA_CONVAR::~ABA_CONVAR()
  {
#ifdef ABACUSSAFE
    if (nActive_) {
      master_->err() << "ABA_CONVAR::~ABA_CONVAR(): constraint/variable still active: ";
      master_->err() << "counter = " << nActive_ << endl;
      exit(Fatal);
    }
    
    if (nLocks_) {
      master_->err() << "ABA_CONVAR::~ABA_CONVAR(): constraint/variable has still ";
      master_->err() << nLocks_ << " locks." << endl;
      exit(Fatal);
    }
#endif    
  }


#ifdef ABACUS_PARALLEL

  ABA_CONVAR::ABA_CONVAR(const ABA_MASTER *master, ABA_MESSAGE &msg)
    :  
    master_(master),  
    sub_(0),  
    expanded_(false),  
    nReferences_(0),  
    nActive_(0),  
    nLocks_(0)  
  {
    int bits = msg.unpackInt();

    assert((bits&1)==0);             //!< \a sub_ must be 0
    dynamic_  = ((bits&2)?true:false);
    local_    = ((bits&4)?true:false);
  }

  void ABA_CONVAR::pack(ABA_MESSAGE &msg) const
  {
    msg.pack( ((sub_&&local_)?1:0) + 2*dynamic_ + 4*local_);

    if (sub_&&local_) {
      master_->err() << "ABA_CONVAR::pack(): constraint/variable must "
                        "be globally valid." << endl;
      exit(Fatal);
    }
  }

#endif

void ABA_CONVAR::deactivate()
  {
#ifdef ABACUSSAFE
    if (nActive_ == 0) {
      master_->err() << "ABA_CONVAR::deactivate(): constraint/variable already non-active" << endl;
      exit(Fatal);
    }
#endif
    --nActive_;
  }

void ABA_CONVAR::removeReference()
  {
    if(--nReferences_ < 0) {
      master_->err() << "ABA_CONVAR::removeReference : reference counter negative" << endl;
      exit(Fatal);
    }
  }

void ABA_CONVAR::unlock()
  {
#ifdef ABACUSSAFE
    if (nLocks_ == 0) {
      master_->err() << "ABA_CONVAR::unlock(): constraint/variable not locked." << endl;
      exit(Fatal);
    }
#endif

    --nLocks_;
  }

bool ABA_CONVAR::deletable() const
  {
    if (nReferences_ || nLocks_) return false;
    else                         return true;
  }


bool ABA_CONVAR::dynamic() const
{
  return dynamic_;
}
  

  void ABA_CONVAR::_expand()
  {
    if(expanded_) {
      master_->err() << "WARNING: ABA_CONVAR::_expand(): ";
      master_->err() << "constraint already expanded" << endl;
      return;
    }
    expand();
    expanded_ = true;
  }

  void ABA_CONVAR::_compress()
  {
    if(!expanded_) {
      master_->err() << "WARNING: ABA_CONVAR::_compress(): ";
      master_->err() << "constraint already compressed" << endl;
      return;
    }
    compress();
    expanded_ = false;
  }
 
  void ABA_CONVAR::expand()
  { }

  void ABA_CONVAR::compress()
  { }

  void ABA_CONVAR::print(ostream &out)
  {
    out << "ABA_CONVAR::print() is only a dummy." << endl;
  }

  unsigned ABA_CONVAR::hashKey()
  {
    master_->err() << "ABA_CONVAR::hashKey() must be defined in derived ";
    master_->err() << "class." << endl;
    exit(Fatal);
    return 0;
  }

  const char *ABA_CONVAR::name()
  {
    master_->err() << "ABA_CONVAR::name() must be defined in derived ";
    master_->err() << "class." << endl;
    exit(Fatal);
    return 0;
  }

  double  ABA_CONVAR::rank()
  {
    return 0;
  }

  bool ABA_CONVAR::equal(ABA_CONVAR *cv)
  {
    master_->err() << "ABA_CONVAR::equal() must be defined in derived ";
    master_->err() << "class." << endl;
    exit(Fatal);
    return false;
  }

