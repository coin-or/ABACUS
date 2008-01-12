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

#include "abacus/fixcand.h"
#include "abacus/master.h"
#include "abacus/sub.h"
#include "abacus/lpsub.h"
#include "abacus/fsvarstat.h"
#include "abacus/lpvarstat.h"
#include "abacus/variable.h"
#include "abacus/constraint.h"
#include "abacus/poolslotref.h"
#include "abacus/cutbuffer.h"

  ABA_FIXCAND::ABA_FIXCAND(ABA_MASTER *master) 
 :  
    master_(master),  
    candidates_(0),  
    fsVarStat_(0),  
    lhs_(0)
  { }

  ABA_FIXCAND::~ABA_FIXCAND()
  {
    deleteAll();
  }

  void ABA_FIXCAND::saveCandidates(ABA_SUB *sub)
  {
// count the candidates and allocate memory
/* All variables which have status \a AtUpperBound or \a AtLowerBound
 *  are candidates. Only discrete variables can be candidates. To avoid
 *  memory leaks we delete the memory used to store older candidate sets
 *  before we allocate the necessary memory.
 */  
  int nCand = 0;  // number of candidates
    
  const int nVar = sub->nVar();

  for (int i = 0; i < nVar; i++) 
    if (sub->lpVarStat(i)->atBound() && sub->variable(i)->discrete()) nCand++;

  deleteAll();
  allocate(nCand);
    
// collect the candidates
/* For each candidate we memorize the left hand side of the condition
 *  tested for fixing. Then later we only have to compare if this
 *  value is greater than the primal bound for minimization problems or
 *  less than the primal bound for maximization problems, respectively.
 */
  ABA_LPSUB *lp = sub->lp();  

#ifdef ABACUS_NO_FOR_SCOPE
  for (i = 0; i < nVar; i++)
#else
  for (int i = 0; i < nVar; i++)
#endif
    if (sub->lpVarStat(i)->atBound() && sub->variable(i)->discrete()) {
      candidates_->push(
           new ABA_POOLSLOTREF<ABA_VARIABLE, 
                              ABA_CONSTRAINT>(*(sub->actVar()->poolSlotRef(i))));
      
      if (sub->lpVarStat(i)->status() == ABA_LPVARSTAT::AtLowerBound) {
        lhs_->push(lp->value() + lp->reco(i));
        fsVarStat_->push(new ABA_FSVARSTAT(master_, 
                                               ABA_FSVARSTAT::FixedToLowerBound));
      }
      else {
        lhs_->push(lp->value() - lp->reco(i));
        fsVarStat_->push(new ABA_FSVARSTAT(master_, 
                                               ABA_FSVARSTAT::FixedToUpperBound));
      }
    }
  }

  void ABA_FIXCAND::fixByRedCost(ABA_CUTBUFFER<ABA_VARIABLE, ABA_CONSTRAINT> 
                                                                      *addVarBuffer)
  {
    if (candidates_ == 0) return;

    master_->out() << endl;
    master_->out() << "Fixing Variables by Reduced Costs:      " << flush;

    const int nCandidates = candidates_->number();

    ABA_BUFFER<int> fixed(master_, nCandidates);  // fixed variables
    ABA_VARIABLE    *v;                                // variable being fixed

    for (int i = 0; i < nCandidates; i++)
      if ((master_->optSense()->max() && 
           ((*lhs_)[i] + master_->eps() < master_->primalBound())) ||
          (master_->optSense()->min() &&
           ((*lhs_)[i] - master_->eps() > master_->primalBound()))) {
        v = (ABA_VARIABLE *) (*candidates_)[i]->conVar();
        if (v) {
          if (!v->fsVarStat()->fixed())    
            master_->newFixed(1);
          v->fsVarStat()->status((*fsVarStat_)[i]);
          
// should a fixed inactive variable be activated?
/* If an inactive variable is fixed to a value different from 0, then
 *  we activate it.
 */  
  if (!v->active()) {
    switch (v->fsVarStat()->status()) {
      case ABA_FSVARSTAT::FixedToLowerBound:
        if (fabs(v->lBound()) > master_->eps())
          addVarBuffer->insert((*candidates_)[i]->slot(), true);
        break;
    
      case ABA_FSVARSTAT::FixedToUpperBound:
        if (fabs(v->uBound()) > master_->eps())
          addVarBuffer->insert((*candidates_)[i]->slot(), true);
        break;
                       
      case ABA_FSVARSTAT::Fixed:
        if (fabs(v->fsVarStat()->value()) > master_->eps())
          addVarBuffer->insert((*candidates_)[i]->slot(), true);
        break;
                       
      default: master_->err() << "ABA_FIXCAND::fixByRedCost(): ";
               master_->err() << "activated variable not fixed" << endl;
               exit(Fatal);
    }
  }

          fixed.push(i);
  }
  }
// remove fixed candidates
/* We delete allocated memory of \a fsVarStat_ for the fixed variables
 *  und remove the fixed variables from the three buffers.
 */  
  const int nFixed = fixed.number();

#ifdef ABACUS_NO_FOR_SCOPE
  for (i = 0; i < nFixed; i++) {
#else
  for (int i = 0; i < nFixed; i++) {
#endif
    delete (*candidates_)[fixed[i]];
    delete (*fsVarStat_)[fixed[i]];
  }

  candidates_->leftShift(fixed);
  fsVarStat_->leftShift(fixed);
  lhs_->leftShift(fixed);

    master_->out(1) << fixed.number() << " variables fixed" << endl;

  }

  void ABA_FIXCAND::deleteAll()
  {
    int i;

    if (candidates_) {
      const int nCandidates = candidates_->number();

      for (i = 0; i < nCandidates; i++) delete (*candidates_)[i];
      delete candidates_;
      candidates_ = 0;
    }
      
    if (fsVarStat_) {
      const int nFsVarStat = fsVarStat_->number();
      for (i = 0; i < nFsVarStat; i++) delete (*fsVarStat_)[i];
      delete fsVarStat_;
      fsVarStat_ = 0;
    }

    if (lhs_) {
      delete lhs_;
      lhs_ = 0;
    }
  }

  void ABA_FIXCAND::allocate(int nCand)
  {
    candidates_ = new ABA_BUFFER<ABA_POOLSLOTREF<ABA_VARIABLE, 
                                                ABA_CONSTRAINT>*>(master_, nCand);
    fsVarStat_  = new ABA_BUFFER<ABA_FSVARSTAT*>(master_, nCand);
    lhs_        = new ABA_BUFFER<double>(master_, nCand);
  }
  
