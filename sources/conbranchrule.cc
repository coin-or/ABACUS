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
 *
 */

#include "abacus/conbranchrule.h"
#include "abacus/active.h"
#include "abacus/master.h"
#include "abacus/sub.h"
#include "abacus/lpsub.h"
#include "abacus/exceptions.h"

ABA_CONBRANCHRULE::ABA_CONBRANCHRULE(
                         ABA_MASTER *master,
                         ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE> *poolSlot) 
  : 
    ABA_BRANCHRULE(master),
    poolSlotRef_(poolSlot)
  { }

ABA_CONBRANCHRULE::~ABA_CONBRANCHRULE()
  { }


ostream &operator<<(ostream &out, const ABA_CONBRANCHRULE &rhs)
  {
    return out << rhs.poolSlotRef_;
  }

int ABA_CONBRANCHRULE::extract(ABA_SUB *sub)
  {

    if (poolSlotRef_.conVar() == 0) {
      const char *_error="ABA_CONBRANCHRULE::extract(): branching constraint not available";
      THROW_PARAM(AlgorithmFailureException,afcConBranchRule,_error);
    }

    if (sub->addBranchingConstraint(poolSlotRef_.slot())) {
      const char *_error="ABA_CONBRANCHRULE::extract(): addition of branching constaint\nto subproblem failed.";
	THROW_PARAM(AlgorithmFailureException,afcConBranchRule,_error);
    }

    return 0;
  }

void ABA_CONBRANCHRULE::extract(ABA_LPSUB *lp)
  {
    ABA_BUFFER<ABA_CONSTRAINT*> newCon(master_, 1);

    newCon.push(poolSlotRef_.conVar());
    lp->addCons(newCon);
  }

void ABA_CONBRANCHRULE::unExtract(ABA_LPSUB *lp)
  {
    ABA_BUFFER<int> remove(master_, 1);

    remove.push(lp->nRow() - 1);

    // pivot the slack variable associated with the removed row in
    
  int status = lp->pivotSlackVariableIn(remove);

  if (status) {
    master_->err() << "WARNING: ";
    master_->err() << "ABA_CONBRANCHRULE::unExtract(): pivoting in ";
    master_->err() << "slack variable failed." << endl;
  }

lp->removeCons(remove);
}

  void ABA_CONBRANCHRULE::initialize(ABA_SUB* sub)
  {
    if (poolSlotRef_.conVar() == 0) {
      const char *_error="ABA_CONBRANCHRULE::initialize(): branching constraint not available";
      THROW_PARAM(AlgorithmFailureException,afcConBranchRule,_error);
    }

    poolSlotRef_.conVar()->sub(sub);
  }

  ABA_CONSTRAINT *ABA_CONBRANCHRULE::constraint()
  {
    return poolSlotRef_.conVar();
  }
