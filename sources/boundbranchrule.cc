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

#include "abacus/boundbranchrule.h"
#include "abacus/master.h"
#include "abacus/sub.h"
#include "abacus/fsvarstat.h"
#include "abacus/lpsub.h"

#ifdef ABACUS_PARALLEL
#include "abacus/message.h"
#endif

ABA_BOUNDBRANCHRULE::ABA_BOUNDBRANCHRULE(ABA_MASTER *master, 
                                           int variable,
                                           double lBound, 
                                           double uBound) 
  : 
    ABA_BRANCHRULE(master),
    variable_(variable),
    lBound_(lBound),
    uBound_(uBound)
  { }

ABA_BOUNDBRANCHRULE::~ABA_BOUNDBRANCHRULE()
  { }

#ifdef ABACUS_PARALLEL
ABA_BOUNDBRANCHRULE::ABA_BOUNDBRANCHRULE(const ABA_MASTER *master, ABA_MESSAGE &msg)
    : 
    ABA_BRANCHRULE(master, msg)
  {
    msg.unpack(variable_);
    msg.unpack(lBound_);
    msg.unpack(uBound_);
    msg.unpack(oldLpLBound_);
    msg.unpack(oldLpUBound_);
  }

void ABA_BOUNDBRANCHRULE::pack(ABA_MESSAGE &msg) const
  {
    ABA_BRANCHRULE::pack(msg);
    msg.pack(variable_);
    msg.pack(lBound_);
    msg.pack(uBound_);
    msg.pack(oldLpLBound_);
    msg.pack(oldLpUBound_);
  }

int ABA_BOUNDBRANCHRULE::classId() const
  {
    return BOUNDBRANCHRULE_CLASSID;
  }


#endif

ostream &operator<<(ostream &out, const ABA_BOUNDBRANCHRULE &rhs)
  {
    return out << rhs.lBound_ << " <= x" << rhs.variable_ << " <= " << rhs.uBound_;
  }

int ABA_BOUNDBRANCHRULE::extract(ABA_SUB *sub)
  {

    if (sub->fsVarStat(variable_)->fixedOrSet()) return 1;

    sub->lBound(variable_, lBound_);
    sub->uBound(variable_, uBound_);
    
    return 0;
  }

void ABA_BOUNDBRANCHRULE::extract(ABA_LPSUB *lp)
  {
     oldLpLBound_ = lp->lBound(variable_);
     oldLpUBound_ = lp->uBound(variable_);

     lp->changeLBound(variable_, lBound_);
     lp->changeUBound(variable_, uBound_);
  }

void ABA_BOUNDBRANCHRULE::unExtract(ABA_LPSUB *lp)
  {
    lp->changeLBound(variable_, oldLpLBound_);
    lp->changeUBound(variable_, oldLpUBound_);
  }
