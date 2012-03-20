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
#include "abacus/constraint.h"
#include "abacus/row.h"
#include "abacus/conclass.h"
#include "abacus/active.h"
#include "abacus/exceptions.h"
#include <math.h>

ABA_CONSTRAINT::ABA_CONSTRAINT(ABA_MASTER *master, 
                                 const ABA_SUB *sub, 
                                 ABA_CSENSE::SENSE sense,
                                 double rhs, 
                                 bool dynamic,
                                 bool local, 
                                 bool liftable) 
    :  
    ABA_CONVAR(master, sub, dynamic, local),  
    sense_(master, sense),  
    rhs_(rhs),  
    conClass_(0),  
    liftable_(liftable)
  {
    if (local && sub == 0) {
      const char *_error="ABA_CONSTRAINT::ABA_CONSTRAINT(): subtree of local item must not be 0";
      THROW_PARAM(AlgorithmFailureException,afcConstraint,_error);
    }
  }

ABA_CONSTRAINT::ABA_CONSTRAINT (ABA_MASTER *master) 
  :  
    ABA_CONVAR(master, 0, true, true),  
    sense_(master),  
    conClass_(0)
  { }

ABA_CONSTRAINT::ABA_CONSTRAINT(const ABA_CONSTRAINT &rhs) 
    :  
    ABA_CONVAR(rhs),  
    sense_(rhs.sense_),  
    rhs_(rhs.rhs_),
    conClass_(0),
    liftable_(rhs.liftable_)
  {
    if (rhs.conClass_) conClass_ = new ABA_CONCLASS(*(rhs.conClass_));
  }

ABA_CONSTRAINT::~ABA_CONSTRAINT()
  {
    delete conClass_;
  }



double ABA_CONSTRAINT::rhs()
  {
    return rhs_;
  }

bool ABA_CONSTRAINT::valid(ABA_SUB *sub)
  {
#ifdef ABACUSSAFE
    if (local_ && sub == 0) {
      char *_error="ABA_CONSTRAINT::valid(): cannot evaluate for locally valid constraint with 0-pointer to subproblem.";
      THROW_PARAM(AlgorithmFailureException,afcConstraint,_error);
    }
#endif

    if (!local_ || sub_->ancestor(sub)) return true;
    else                                return false;
  }

int ABA_CONSTRAINT::genRow(ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *var,
                             ABA_ROW &row)
  {
    double eps      = master_->machineEps();
    double minusEps = -eps;
    int    n        = var->number();
    double c;

    expand();
    
    for (int e = 0; e < n; e++) {
      c = coeff((*var)[e]);
      if (c > eps || c < minusEps) row.insert(e, c);
    }
    
    row.rhs(rhs());
    row.sense(sense_);
    compress();
    return row.nnz();
  }

double ABA_CONSTRAINT::slack(ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *variables,
                               double *x)
  {
    double eps      = master_->machineEps();
    double minusEps = -eps;
    double c;
    double xi;
    double lhs = 0.0;
    int    n   = variables->number();

    expand();
    
    for (int i = 0; i < n; i++) {
      xi = x[i];
      if (xi > eps || xi < minusEps) {
        c = coeff((*variables)[i]);
        if (c > eps || c < minusEps)
          lhs += c * xi;
      }
    }

    compress();

    return rhs() - lhs;
  }

bool ABA_CONSTRAINT::violated(ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *variables,
                                double *x, 
                                double *sl)
  {
    double s = slack(variables, x);

    if (sl) *sl = s;
    
    return violated(s);
  }

bool ABA_CONSTRAINT::violated(double slack) const
  {
    switch (sense_.sense()) {
      case ABA_CSENSE::Equal:
        if (fabs(slack) > master_->eps()) return true;
        else                              return false;
      case ABA_CSENSE::Less:
        if (slack < -master_->eps()) return true;
        else                         return false;
      case ABA_CSENSE::Greater:
        if (slack > master_->eps()) return true;
        else                        return false;
      default: const char *_error="ABA_CONSTRAINT::violated(): unknown sense";
	       THROW_PARAM(AlgorithmFailureException,afcConstraint,_error);
               return false;  //!< to suppress compiler warning
    }
  }

ABA_INFEASCON::INFEAS ABA_CONSTRAINT::voidLhsViolated(double newRhs) const
  {
    switch (sense_.sense()) {
      case ABA_CSENSE::Equal:
        if(newRhs > master_->eps())  return ABA_INFEASCON::TooLarge;
        if(newRhs < -master_->eps()) return ABA_INFEASCON::TooSmall;
        else                      return ABA_INFEASCON::Feasible;
      case ABA_CSENSE::Less:
        return newRhs < -master_->eps() ? ABA_INFEASCON::TooLarge : ABA_INFEASCON::Feasible;
      case ABA_CSENSE::Greater:
        return   newRhs > master_->eps() ? ABA_INFEASCON::TooSmall : ABA_INFEASCON::Feasible;
      default: const char *_error="ABA_CONSTRAINT::voidLhsViolated(): unknown sense";
               THROW_PARAM(AlgorithmFailureException,afcConstraint,_error);
               return ABA_INFEASCON::Feasible;  //!< to suppress compiler warning
    }
  }

void ABA_CONSTRAINT::printRow(ostream &out, 
                                ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *var)
  {
    ABA_ROW row(master_, var->number());

    genRow(var, row);

    out << row;
  }

double ABA_CONSTRAINT::distance(double *x, 
                                  ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *actVar)
  {

    ABA_ROW a(master_, actVar->number());

    int nnz = genRow(actVar, a);

    double ax = 0.0;

    for (int i = 0; i < nnz; i++)
      ax += a.coeff(i) * x[a.support(i)];

    return fabs((rhs() - ax)/a.norm());

  }

ABA_CONCLASS *ABA_CONSTRAINT::classification(ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *var)
  {
    if (conClass_ == 0 || var) {
      if (var == 0) {
        const char *_error="ABA_CONSTRAINT::classification(): Fatal error.\nNeither classification nor variable set specified.";
        THROW_PARAM(AlgorithmFailureException,afcConstraint,_error);
      }
      conClass_ = classify(var);
    }
    return conClass_;
  }

ABA_CONCLASS *ABA_CONSTRAINT::classify(ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *var)
  {
    return 0;
  }


