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
#include "abacus/exceptions.h"

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
  }

ABA_CONVAR::~ABA_CONVAR()
  {
#ifdef ABACUSSAFE
    if (nActive_) {
      char *_error=new char[255];
      sprintf(_error,"ABA_CONVAR::~ABA_CONVAR(): constraint/variable still active: \ncounter = %i",nActive_);
      THROW_PARAM(AlgorithmFailureException,afcConvar,_error);
    }
    
    if (nLocks_) {
	char *_error=new char[255];
	sprintf(_error,"ABA_CONVAR::~ABA_CONVAR(): constraint/variable has still %i locks",nLocks_);
      THROW_PARAM(AlgorithmFailureException,afcConvar,_error);
	}
#endif    
  }



void ABA_CONVAR::deactivate()
  {
#ifdef ABACUSSAFE
    if (nActive_ == 0) {
      char *_error="ABA_CONVAR::deactivate(): constraint/variable already non-active";
      THROW_PARAM(AlgorithmFailureException,afcConvar,_error);
    }
#endif
    --nActive_;
  }

void ABA_CONVAR::removeReference()
  {
    if(--nReferences_ < 0) {
      const char *_error="ABA_CONVAR::removeReference : reference counter negative";
      THROW_PARAM(AlgorithmFailureException,afcConvar,_error);
    }
  }

void ABA_CONVAR::unlock()
  {
#ifdef ABACUSSAFE
    if (nLocks_ == 0) {
      char *_error="ABA_CONVAR::unlock(): constraint/variable not locked.";
      THROW_PARAM(AlgorithmFailureException,afcConvar,_error);
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
    const char *_error="ABA_CONVAR::hashKey() must be defined in derived class.";
    THROW_PARAM(AlgorithmFailureException,afcConvar,_error);
    return 0;
  }

  const char *ABA_CONVAR::name()
  {
    const char *_error="ABA_CONVAR::name() must be defined in derived class.";
    THROW_PARAM(AlgorithmFailureException,afcConvar,_error);
    return 0;
  }

  double  ABA_CONVAR::rank()
  {
    return 0;
  }

  bool ABA_CONVAR::equal(ABA_CONVAR *cv)
  {
    const char *_error="ABA_CONVAR::equal() must be defined in derived class.";
    THROW_PARAM(AlgorithmFailureException,afcConvar,_error);
    return false;
  }

