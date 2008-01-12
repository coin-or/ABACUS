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

#ifdef ABACUS_PARALLEL
#include "abacus/parmaster.h"
#include "abacus/message.h"
#include "abacus/pool.h"
#include "abacus/poolslot.h"
#include "abacus/debug.h"
#endif

ABA_CONBRANCHRULE::ABA_CONBRANCHRULE(
                         ABA_MASTER *master,
                         ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE> *poolSlot) 
  : 
    ABA_BRANCHRULE(master),
    poolSlotRef_(poolSlot)
  { }

ABA_CONBRANCHRULE::~ABA_CONBRANCHRULE()
  { }

#ifdef ABACUS_PARALLEL

ABA_CONBRANCHRULE::ABA_CONBRANCHRULE(const ABA_MASTER *master, ABA_MESSAGE &msg)
    :
    ABA_BRANCHRULE(master, msg),
    poolSlotRef_(master)
  {
    ABA_ID id(msg);
    char needConstraint;

    ABA_POOL<ABA_CONSTRAINT, ABA_VARIABLE> *pool = (ABA_POOL<ABA_CONSTRAINT, ABA_VARIABLE>*)
      master_->parmaster()->getPool(id.index());
    ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE> *ps = pool->findSlot(id);
    if (ps) {
      poolSlotRef_.slot(ps);
      needConstraint=0;
    }
    else
      needConstraint=1;

    // msg.clear();
    msg.pack(needConstraint);
    msg.send();         //!< send flag if constraint is missing

    msg.receive();      //!< receive constraint if needed
    if (needConstraint) {
      int classId;
      msg.unpack(classId);
      ABA_CONSTRAINT *con = (ABA_CONSTRAINT*) master_->unpackConVar(msg, classId);
      if (debug(DEBUG_MESSAGE_CONVAR)) {
        master_->out() << "DEBUG_MESSAGE_CONVAR: Constraint " << id << " (classId=" << classId
                       << ") of ABA_CONBRANCHRULE received." << endl;
      }
      ABA_POOL<ABA_CONSTRAINT, ABA_VARIABLE> *pool = (ABA_POOL<ABA_CONSTRAINT, ABA_VARIABLE>*)
        master_->parmaster()->getPool(id.index());
      ps = pool->insert(con);
      if (ps == 0) {
        master_->err() << "ABA_CONBRANCHRULE::ABA_CONBRANCHRULE(): no room"
                          " to insert constraint into pool." << endl;
        exit(Fatal);
      }
      ps->setIdentification(id);
      poolSlotRef_.slot(ps);
    }
  }

void ABA_CONBRANCHRULE::pack(ABA_MESSAGE &msg) const
  {
    ABA_BRANCHRULE::pack(msg);
    if (poolSlotRef_.conVar() == 0) {           //!< removed conVar
      master_->err() << "ABA_CONBRANCHRULE::pack(): branching constraint not"
                        " available!" << endl;
      exit(Fatal);
    }

    ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE> *ps = poolSlotRef_.slot();
    if (!ps->getIdentification().isInitialized())
      ps->setNewIdentification();
    ps->getIdentification().pack(msg);

    msg.send();                 //!< send ABA_ID of constraint

    msg.receive();              //!< receive flag if constraint is missing
    char needConstraint;
    msg.unpack(needConstraint);
    // msg.clear();

    if (needConstraint) {
      ABA_CONSTRAINT *con = poolSlotRef_.conVar();
      if (debug(DEBUG_MESSAGE_CONVAR)) {
        master_->out() << "DEBUG_MESSAGE_CONVAR: sending Constraint " << ps->getIdentification()
                       << " (classId=" << con->classId()
                       << ") of ABA_CONBRANCHRULE..." << endl;
      }
      msg.pack(con->classId());
      con->pack(msg);
    }
  }

int ABA_CONBRANCHRULE::classId() const
  {
    return CONBRANCHRULE_CLASSID;
  }

#endif

ostream &operator<<(ostream &out, const ABA_CONBRANCHRULE &rhs)
  {
    return out << rhs.poolSlotRef_;
  }

int ABA_CONBRANCHRULE::extract(ABA_SUB *sub)
  {

    if (poolSlotRef_.conVar() == 0) {
      master_->err() << "ABA_CONBRANCHRULE::extract(): branching constraint not available" << endl;
      exit(Fatal);
    }

    if (sub->addBranchingConstraint(poolSlotRef_.slot())) {
      master_->err() << "ABA_CONBRANCHRULE::extract(): addition of branching constaint ";
      master_->err() << "to subproblem failed." << endl;
      exit(Fatal);
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
      master_->err() << "ABA_CONBRANCHRULE::initialize(): ";
      master_->err() << "branching constraint not available" << endl;
      exit(Fatal);
    }

    poolSlotRef_.conVar()->sub(sub);
  }

  ABA_CONSTRAINT *ABA_CONBRANCHRULE::constraint()
  {
    return poolSlotRef_.conVar();
  }
