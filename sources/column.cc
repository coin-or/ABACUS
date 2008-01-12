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

#include "abacus/column.h"
#include "abacus/global.h"

#ifdef ABACUS_PARALLEL
#include "abacus/message.h"
#endif

ABA_COLUMN::ABA_COLUMN(ABA_GLOBAL *glob, 
                         double obj, 
                         double lb, 
                         double ub,
                         int nnz, 
                         ABA_ARRAY<int> &s, 
                         ABA_ARRAY<double> &c)
  :  
    ABA_SPARVEC(glob, nnz, s, c),  
    obj_(obj),  
    lBound_(lb),  
    uBound_(ub)
  {  }

ABA_COLUMN::ABA_COLUMN(ABA_GLOBAL *glob, int maxNnz) 
  :  
    ABA_SPARVEC(glob, maxNnz)
  { }

ABA_COLUMN::ABA_COLUMN(ABA_GLOBAL *glob, 
                         double obj, 
                         double lb, 
                         double ub,
                         ABA_SPARVEC &vec) 
  :  
    ABA_SPARVEC(vec),  
    obj_(obj),  
    lBound_(lb),  
    uBound_(ub)
  { }

/*! The destructor.
 */
ABA_COLUMN::~ABA_COLUMN()
  { }
  

#ifdef ABACUS_PARALLEL

ABA_COLUMN::ABA_COLUMN(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
    :  
    ABA_SPARVEC(glob, msg)
  {
    msg.unpack(obj_);
    msg.unpack(lBound_);
    msg.unpack(uBound_);
  }

void ABA_COLUMN::pack(ABA_MESSAGE &msg) const
  {
    ABA_SPARVEC::pack(msg);
    msg.pack(obj_);
    msg.pack(lBound_);
    msg.pack(uBound_);
  }

#endif

ostream &operator<<(ostream &out, const ABA_COLUMN &rhs)
  {
    out << "objective function coefficient: " << rhs.obj() << endl;
    out << "bounds: " << rhs.lBound_ << " <= x <= " << rhs.uBound_ << endl;
    out << "nonzero rows of column :" << endl;

    for (int i = 0; i < rhs.nnz_; i++)
      out << 'r' << rhs.support_[i] << ": " << rhs.coeff_[i] << endl;

    return out;
  }




void ABA_COLUMN::copy(const ABA_COLUMN &col)
  {
    ABA_SPARVEC::copy(col);
 
    obj_    = col.obj_;
    lBound_ = col.lBound_;
    uBound_ = col.uBound_;
  }
