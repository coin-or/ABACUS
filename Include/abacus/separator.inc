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

#ifndef ABA_SEPARATOR_INC
#define ABA_SEPARATOR_INC

#include "abacus/separator.h"
#include "abacus/lpsolution.h"
#include "abacus/constraint.h"
#include "abacus/variable.h"
#include "abacus/hash.h"
#include "abacus/master.h"

  template <class BaseType, class CoType> 
  ABA_SEPARATOR<BaseType, CoType>::ABA_SEPARATOR(ABA_LPSOLUTION<CoType,
    BaseType> *lpSolution, bool testDuplications, int maxGen
	  )

  :  
  master_(lpSolution->master_), 
  lpSol_(lpSolution), 
  minAbsViolation_(master_->eps()), 
  newCons_(master_,maxGen), 
  hash_(0),
  nDuplications_(0),
  pool_(0)
  { 
  if(testDuplications)
    hash_=new ABA_HASH<unsigned, BaseType *>((ABA_GLOBAL*)master_, 3*maxGen);
}

  template <class BaseType, class CoType>
  ABA_SEPARATOR<BaseType, CoType>::~ABA_SEPARATOR()
  {
    delete hash_;
  }

  template <class BaseType, class CoType>
  ABA_SEPARATOR_CUTFOUND 
  ABA_SEPARATOR<BaseType, CoType>::cutFound(BaseType *cv)
  {
  if(newCons_.full()) {
    delete cv;
    return Full;
  }

  if(pool_&&pool_->present(cv)) {
    delete cv;
    nDuplications_++;
    return Duplication;
  }   

  if(hash_&&find(cv)) {
    delete cv;
    nDuplications_++;
    return Duplication;
  }
   

  newCons_.push(cv);
  if(hash_)
    hash_->insert(cv->hashKey(),cv);
  return Added;
  }

  template <class BaseType, class CoType>
   ABA_BUFFER<BaseType *> &ABA_SEPARATOR<BaseType, CoType>::cutBuffer() 
  {
    return newCons_;
  }

  template <class BaseType, class CoType>
  int ABA_SEPARATOR<BaseType, CoType>::nGen() const
  {
  return newCons_.number();
  }

  template <class BaseType, class CoType>
  int ABA_SEPARATOR<BaseType, CoType>::nDuplications() const
  {
  return nDuplications_;
  }

  template <class BaseType, class CoType>
  int ABA_SEPARATOR<BaseType, CoType>::nCollisions() const
  {
  if(!hash_)
    return 0;   
  return hash_->nCollisions();
  }

  template <class BaseType, class CoType>
  int ABA_SEPARATOR<BaseType, CoType>::maxGen() const
  {
  return newCons_.size();
  }

  /*
  virtual void separate();

  virtual bool terminateSeparation();
  */
 
  template <class BaseType, class CoType>
  bool ABA_SEPARATOR<BaseType, CoType>::find(BaseType *cv) 
  {
   int key = cv->hashKey();

   BaseType **cand = hash_->initializeIteration(key);

   while(cand) {
      if (cv->equal(*cand)) return true;
      cand = hash_->next(key);
    }
    return false;
 }

  template <class BaseType, class CoType>
  double ABA_SEPARATOR<BaseType, CoType>::minAbsViolation() const
  {
    return minAbsViolation_;
  }

  //template <class BaseType, class CoType>
  //void ABA_SEPARATOR<BaseType, CoType>::minAbsViolation(double minAbsVio);
 
  //template <class BaseType, class CoType>
  //ABA_LPSOLUTION<CoType, BaseType> * ABA_SEPARATOR<BaseType, CoType>::lpSolution();

  //template <class BaseType, class CoType>
  //void ABA_SEPARATOR<BaseType, CoType>::watchNonDuplPool(ABA_NONDUPLPOOL<BaseType, CoType> *pool);

#endif   // ABA_SEPERATOR_INC



