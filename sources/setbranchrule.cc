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

#include "abacus/setbranchrule.h"
#include "abacus/master.h"
#include "abacus/sub.h"
#include "abacus/lpsub.h"

#ifdef ABACUS_PARALLEL
#include "abacus/message.h"
#endif

  ABA_SETBRANCHRULE::ABA_SETBRANCHRULE(ABA_MASTER *master, 
                                       int variable, 
                                       ABA_FSVARSTAT::STATUS status)
  :  
    ABA_BRANCHRULE(master),  
    variable_(variable),  
    status_(status)
  { }

  ABA_SETBRANCHRULE::~ABA_SETBRANCHRULE()
  { }

#ifdef ABACUS_PARALLEL

  ABA_SETBRANCHRULE::ABA_SETBRANCHRULE(const ABA_MASTER *master, ABA_MESSAGE &msg)
    :  
    ABA_BRANCHRULE(master, msg)
  {
    msg.unpack(variable_);
    status_ = (ABA_FSVARSTAT::STATUS) msg.unpackInt();
    msg.unpack(oldLpBound_);
  }

  void ABA_SETBRANCHRULE::pack(ABA_MESSAGE &msg) const
  {
    ABA_BRANCHRULE::pack(msg);
    msg.pack(variable_);
    msg.pack((int)status_);
    msg.pack(oldLpBound_);
  }

  int ABA_SETBRANCHRULE::classId() const
  {
    return SETBRANCHRULE_CLASSID;
  }

#endif

  ostream &operator<<(ostream &out, const ABA_SETBRANCHRULE &rhs)
  {
    return out << "x" << rhs.variable_ << " = " << (int) rhs.status_;
  }

  int ABA_SETBRANCHRULE::extract(ABA_SUB *sub)
  {
    if (sub->fsVarStat(variable_)->contradiction(status_))
      return 1;
    
    sub->fsVarStat(variable_)->status(status_);
    return 0;
  }

  void ABA_SETBRANCHRULE::extract(ABA_LPSUB *lp)
  {
    if (status_ == ABA_FSVARSTAT::SetToLowerBound) {
      oldLpBound_ = lp->uBound(variable_);
      lp->changeUBound(variable_, lp->lBound(variable_));
    }
    else {
      oldLpBound_ = lp->lBound(variable_);
      lp->changeLBound(variable_, lp->uBound(variable_));
    }
  }

  void ABA_SETBRANCHRULE::unExtract(ABA_LPSUB *lp)
  {
    if (status_ == ABA_FSVARSTAT::SetToLowerBound)
      lp->changeUBound(variable_, oldLpBound_);
    else
      lp->changeLBound(variable_, oldLpBound_);
  }

  bool ABA_SETBRANCHRULE::branchOnSetVar()
  {
    return true;
  }

  bool ABA_SETBRANCHRULE::setToUpperBound() const
  {
    if (status_ == ABA_FSVARSTAT::SetToUpperBound) return true;
    else                                       return false;
  }
