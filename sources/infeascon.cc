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

#include "abacus/infeascon.h"
#include "abacus/convar.h"
#include "abacus/variable.h"
#include "abacus/constraint.h"
#include "abacus/master.h"

  ABA_INFEASCON::ABA_INFEASCON(ABA_MASTER *master, ABA_CONSTRAINT *con, INFEAS inf) 
  :  
    master_(master),  
    constraint_(con),  
    infeas_(inf)
  {  }

  bool ABA_INFEASCON::goodVar(ABA_VARIABLE *v)
  {
    double eps = master_->machineEps();
    
#ifdef ABACUS_NO_BOOL
    bool pos;
    bool neg;
 
    if (v->uBound() > eps)  pos = true;
    else                    pos = false;
    if (v->lBound() < -eps) neg = true;
    else                    neg = false;
#else
    bool pos = v->uBound() > eps;
    bool neg = v->lBound() < -eps;
#endif
    double c = constraint_->coeff(v);
    
    if (infeas_ == TooSmall) {
      if (c > eps && pos)  return true;
      if (c < -eps && neg) return true;
      return false;
    }
    else if (infeas_ == TooLarge) {
      if (c > eps && neg)  return true;
      if (c < -eps && pos) return true;
      return false;
    }
    else {
      master_->err() << "ABA_INFEASCON::goodVar(): constraint is feasible" << endl;
      exit(Fatal);
      return false;  //!< to suppress compiler warnings
    }
  }
