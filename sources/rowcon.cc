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

#include "abacus/rowcon.h"
#include "abacus/master.h"
#include "abacus/numvar.h"

  ABA_ROWCON::ABA_ROWCON(ABA_MASTER *master, 
                         const ABA_SUB *sub, 
                         ABA_CSENSE::SENSE sense, 
                         int nnz, 
                         const ABA_ARRAY<int> &support, 
                         const ABA_ARRAY<double> &coeff,
                         double rhs, 
                         bool dynamic, 
                         bool local,
                         bool liftable) 
  :  
    ABA_CONSTRAINT(master, sub, sense, rhs, dynamic, local, liftable),  
    row_(master, nnz, support, coeff, sense, rhs)
   { }

  ABA_ROWCON::ABA_ROWCON(ABA_MASTER *master, 
                         const ABA_SUB *sub, 
                         ABA_CSENSE::SENSE sense, 
                         int nnz,
                         int *support, 
                         double *coeff,
                         double rhs, 
                         bool dynamic, 
                         bool local,
                         bool liftable) 
  :  
    ABA_CONSTRAINT(master, sub, sense, rhs, dynamic, local, liftable),  
    row_(master, nnz, support, coeff, sense, rhs)
  { }

  ABA_ROWCON::~ABA_ROWCON()
  { }



  double ABA_ROWCON::coeff(ABA_VARIABLE *v)
  {
    ABA_NUMVAR *numVar = (ABA_NUMVAR *) v;
    return row_.origCoeff(numVar->number());
  }

  void ABA_ROWCON::print(ostream &out)
  {
    out << row_;
  }
