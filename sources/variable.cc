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

#include "abacus/variable.h"
#include "abacus/master.h"
#include "abacus/sub.h"
#include "abacus/constraint.h"
#include "abacus/vartype.h"
#include "abacus/column.h"
#include "abacus/active.h"
#include "abacus/exceptions.h"
#include <math.h>

  ABA_VARIABLE::ABA_VARIABLE(ABA_MASTER *master, 
                             const ABA_SUB *sub, 
                             bool dynamic, 
                             bool local,
                             double obj, 
                             double lBound,
                             double uBound,
                             ABA_VARTYPE::TYPE type) 
  :  
    ABA_CONVAR(master, sub, dynamic, local),  
    fsVarStat_(master),  
    obj_(obj),  
    lBound_(lBound),  
    uBound_(uBound),  
    type_(type)
  { }

  ABA_VARIABLE::~ABA_VARIABLE()
  { }



  double ABA_VARIABLE::obj()
  {
    return obj_;
  }

  bool ABA_VARIABLE::valid(ABA_SUB *sub)
  {
#ifdef ABACUSSAFE
    if (local_ && sub == 0) {
      char *_error="ABA_VARIABLE::valid(): cannot evaluate for locally valid variable with 0-pointer to subproblem.";
      THROW_PARAM(AlgorithmFailureException,afcVariable,_error);
    }
#endif

    if (!local_ || sub->ancestor(sub_)) return true;
    else                                return false;
  }
  
  double ABA_VARIABLE::coeff(ABA_CONSTRAINT *con)
  {
    return con->coeff(this);
  }

  int ABA_VARIABLE::genColumn(ABA_ACTIVE<ABA_CONSTRAINT, ABA_VARIABLE> *actCon, 
                              ABA_COLUMN &col)
  {
    double eps      = master_->machineEps();
    double minusEps = -eps;
    double co;
    int    n        = actCon->number();

    expand();
    
    for (int i = 0; i < n; i++) {
      co = (*actCon)[i]->coeff(this);
      if (co > eps || co < minusEps) col.insert(i,co);
    }

    col.obj(obj());
    col.lBound(lBound());
    col.uBound(uBound());

    compress();
    
    return col.nnz();
      
  }

  bool ABA_VARIABLE::violated(double rc) const
  {
    if (master_->optSense()->max()) {
      return rc > master_->eps();
    }
    else {
      return rc < -master_->eps();
    }
  }

  bool ABA_VARIABLE::violated(ABA_ACTIVE<ABA_CONSTRAINT, ABA_VARIABLE> *constraints,
                              double *y, 
                              double *r)
  {
    double rc = redCost(constraints, y);
    
    if (r) *r = rc;
    
    return violated(rc);
  }

  double ABA_VARIABLE::redCost(ABA_ACTIVE<ABA_CONSTRAINT, ABA_VARIABLE> *actCon, 
                               double *y)
  {
    double c;
    double eps = master_->machineEps();
    double minusEps = -eps;
    double rc = obj();
    int    n  = actCon->number();

    expand();

    for (int i = 0; i < n; i++) {
      c = (*actCon)[i]->coeff(this);
      if (c > eps || c < minusEps) 
        rc -= y[i] * c;
    }

    compress();
    
    return rc;
  }

  bool ABA_VARIABLE::useful(ABA_ACTIVE<ABA_CONSTRAINT, ABA_VARIABLE> *actCon, 
                            double *y, 
                            double lpVal)
  {
    if (!discrete()) return true;
    
    double rc = redCost(actCon, y);

    if (master_->optSense()->max()) {
      if (lpVal + rc > master_->primalBound()) return true;
      else                                 return false;
    }
    else {
      if (lpVal + rc < master_->primalBound()) return true;
      else                                 return false;
    }
  }

  void ABA_VARIABLE::printCol(ostream &out,
                              ABA_ACTIVE<ABA_CONSTRAINT, ABA_VARIABLE> *constraints)
  {
    ABA_COLUMN col(master_, constraints->number());

    genColumn(constraints, col);

    out << col;
  }
