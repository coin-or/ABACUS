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

#include "abacus/srowcon.h"
#include "abacus/master.h"
#include "abacus/numvar.h"
#include "abacus/active.h"

  ABA_SROWCON::ABA_SROWCON(ABA_MASTER *master, 
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
    ABA_ROWCON(master, sub, sense, nnz, support, coeff, rhs, dynamic, local,
           liftable)
  { }

  ABA_SROWCON::ABA_SROWCON(ABA_MASTER *master, 
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
    ABA_ROWCON(master, sub, sense, nnz, support, coeff, rhs, dynamic, local,
           liftable)
  { }

  ABA_SROWCON::~ABA_SROWCON()
  { }



  int ABA_SROWCON::genRow(ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *var, 
                          ABA_ROW &row)
  {
    row.copy(row_);

    return row_.nnz();
  }

  double ABA_SROWCON::slack(ABA_ACTIVE<ABA_VARIABLE, ABA_CONSTRAINT> *variables, 
                            double *x)
  {

    int nnz = row_.nnz();

    double lhs = 0.0;

    for (int i = 0; i < nnz; i++)
      lhs += x[row_.support(i)] * row_.coeff(i);

    return rhs() - lhs;
  }
