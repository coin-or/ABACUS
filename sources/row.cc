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

#include "abacus/row.h"
#include "abacus/global.h"


  ABA_ROW::ABA_ROW(ABA_GLOBAL *glob, 
                   int nnz,
                   const ABA_ARRAY<int> &s, 
                   const ABA_ARRAY<double> &c, 
                   const ABA_CSENSE sense, 
                   double r)
  :  
    ABA_SPARVEC(glob, nnz, s, c),  
    sense_(sense),  
   rhs_(r)
  {  }

  ABA_ROW::ABA_ROW(ABA_GLOBAL *glob, 
                   int nnz,
                   const ABA_ARRAY<int> &s, 
                   const ABA_ARRAY<double> &c, 
                   const ABA_CSENSE::SENSE sense, 
                   double r)
  :  
    ABA_SPARVEC(glob, nnz, s, c),  
    sense_(glob, sense),  
    rhs_(r)
  {  }

  ABA_ROW::ABA_ROW(ABA_GLOBAL *glob, int nnz,
                   int *s, double *c,
                   ABA_CSENSE::SENSE sense, double r) 
  :  
    ABA_SPARVEC(glob, nnz, s, c),  
    sense_(glob, sense),  
    rhs_(r)
  { }
                
  ABA_ROW::ABA_ROW(ABA_GLOBAL *glob, int size) 
  :  
    ABA_SPARVEC(glob, size),  
    sense_(glob)
  { }

  ABA_ROW::~ABA_ROW()
  {}
    

  ostream &operator<<(ostream& out, const ABA_ROW &rhs)
  {
    int    s;
    double c;
    char   sign;
    double eps = rhs.glob_->machineEps();
    const int rhsNnz = rhs.nnz();

    for (int i = 0; i < rhsNnz; i++) {
      s = rhs.support(i);
      c = rhs.coeff(i);

      if (c < 0.0) {
        sign = '-';
        c = -c;
      }
      else sign = '+';

      if (i > 0 || sign == '-')  // do not print first \a '+' of row
        out << sign << ' ';
      if (c < 1.0 - eps || 1.0 + eps < c)  // do not print coefficient 1
        out << c << ' ';
      out << 'x' << s << ' ';   

      if (i && !(i % 10)) out << endl;
    }

    return out << rhs.sense_ << ' ' << rhs.rhs();
  }

  void ABA_ROW::copy(const ABA_ROW &row)
  {
    sense_ = row.sense_;
    rhs_   = row.rhs_;

    ABA_SPARVEC::copy(row);
  }

  void ABA_ROW::delInd(ABA_BUFFER<int> &buf, double rhsDelta)
  {
    leftShift(buf);
    rhs_ -= rhsDelta;
  }

