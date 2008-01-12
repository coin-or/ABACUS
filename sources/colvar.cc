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

#include "abacus/colvar.h"
#include "abacus/master.h"
#include "abacus/column.h"
#include "abacus/array.h"
#include "abacus/numcon.h"

ABA_COLVAR::ABA_COLVAR(ABA_MASTER *master, 
                         const ABA_SUB *sub, 
                         bool dynamic, 
                         bool local,
                         double lBound, 
                         double uBound,
                         ABA_VARTYPE::TYPE varType, 
                         double obj, 
                         int nnz, 
                         ABA_ARRAY<int> &support, 
                         ABA_ARRAY<double> &coeff)
  : 
    ABA_VARIABLE(master, sub, dynamic, local, obj, lBound, uBound, varType),
    column_(master, obj, lBound, uBound, nnz, support, coeff)
  { }

ABA_COLVAR::ABA_COLVAR(ABA_MASTER *master, 
                         const ABA_SUB *sub, 
                         bool dynamic, 
                         bool local,
                         double lBound, 
                         double uBound,
                         ABA_VARTYPE::TYPE varType, 
                         double obj, 
                         ABA_SPARVEC &vector) 
  : 
    ABA_VARIABLE(master, sub, dynamic, local, obj, lBound, uBound, varType),
    column_(master, obj, lBound, uBound, vector)
  { }
  
ABA_COLVAR::~ABA_COLVAR()
  { }


#ifdef ABACUS_PARALLEL

ABA_COLVAR::ABA_COLVAR(const ABA_MASTER *master, ABA_MESSAGE &msg)
  :
    ABA_VARIABLE(master, msg),
    column_(master, msg)
  {  }

void ABA_COLVAR::pack(ABA_MESSAGE &msg) const
  {
    ABA_VARIABLE::pack(msg);
    column_.pack(msg);
  }

int ABA_COLVAR::classId() const
  {
    return COLVAR_CLASSID;
  }

#endif

ostream &operator<<(ostream &out, const ABA_COLVAR &rhs)
  {
    return out << rhs.column_;
  }

void ABA_COLVAR::print(ostream &out)
  {
    out << *this;
  }

double ABA_COLVAR::coeff(ABA_CONSTRAINT *con)
  {
    return column_.origCoeff(((ABA_NUMCON*) con)->number());
  }

double ABA_COLVAR::coeff(int i)
  {
    return column_.origCoeff(i);
  }

ABA_COLUMN *ABA_COLVAR::column()
  {
    return &column_;
  }
