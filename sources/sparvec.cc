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
  
#include "abacus/sparvec.h"

#include <math.h>

#ifdef ABACUS_PARALLEL
#include "abacus/message.h"
#endif

  ABA_SPARVEC::ABA_SPARVEC(ABA_GLOBAL *glob, 
                           int size, 
                           double reallocFac) 
    :  
    glob_(glob),  
    size_(size),  
    nnz_(0),  
    reallocFac_(reallocFac)

    {
      if (size) {
        support_ = new int[size];
        coeff_   = new double[size];
      }
      else {
        support_ = 0;
        coeff_   = 0;
      }
    }

  ABA_SPARVEC::ABA_SPARVEC(ABA_GLOBAL *glob, 
                           int size, 
                           const ABA_ARRAY<int> &s,
                           const ABA_ARRAY<double> &c, 
                           double reallocFac) 
    :  
    glob_(glob),  
    size_(size),  
    reallocFac_(reallocFac)

  {
    if (size ) {
      support_ = new int[size];
      coeff_   = new double[size];
    }
    else {
      support_ = 0;
      coeff_   = 0;
    }
      
    nnz_ = size < s.size() ? size : s.size();
    
    for(int i = 0; i < nnz_; i++) {
      support_[i] = s[i];
      coeff_[i]   = c[i];
    }

  }

  ABA_SPARVEC::ABA_SPARVEC(ABA_GLOBAL *glob, 
                           int nnz, 
                           int *s, 
                           double *c, 
                           double reallocFac)
   :  
    glob_(glob),  
    size_(nnz),  
    nnz_(nnz),  
    reallocFac_(reallocFac)

  {
    if (nnz) {
      support_ = new int[nnz];
      coeff_   = new double[nnz];
    }
    else {
      support_ = 0;
      coeff_   = 0;
    }
      
    for(int i = 0; i < nnz; i++) {
      support_[i] = s[i];
      coeff_[i]   = c[i];
    }

  }

  ABA_SPARVEC::ABA_SPARVEC(const ABA_SPARVEC& rhs) 
    :  
    glob_(rhs.glob_),  
    size_(rhs.size_),  
    nnz_(rhs.nnz_),  
    reallocFac_(rhs.reallocFac_)

  {
    if (size_) {
      support_ = new int[size_];
      coeff_   = new double[size_];
      
      for(int i = 0; i < nnz_; i++) {
        support_[i] = rhs.support_[i];
        coeff_[i]   = rhs.coeff_[i];
      }
    }
    else {
      support_ = 0;
      coeff_   = 0;
    }
  }

  ABA_SPARVEC::~ABA_SPARVEC()
  {
    delete [] support_;
    delete [] coeff_;
  }
  

#ifdef ABACUS_PARALLEL

  ABA_SPARVEC::ABA_SPARVEC(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
    :  
    glob_(glob)
  {
    msg.unpack(size_);
    msg.unpack(nnz_);
    msg.unpack(reallocFac_);

    if (size_) {
      support_ = new int[size_];
      coeff_   = new double[size_];

      msg.unpack(support_, nnz_);
      msg.unpack(coeff_, nnz_);
    }
    else {
      support_ = 0;
      coeff_   = 0;
    }
  }

  void ABA_SPARVEC::pack(ABA_MESSAGE &msg) const
  {
    msg.pack(size_);
    msg.pack(nnz_);
    msg.pack(reallocFac_);

    if (size_) {
      msg.pack(support_, nnz_);
      msg.pack(coeff_, nnz_);
    }
  }

#endif

  const ABA_SPARVEC& ABA_SPARVEC::operator=(const ABA_SPARVEC& rhs)
  {
    if (this == &rhs)
      return *this;
      
    if(size_ != rhs.size_) {
      glob_->err() << "ABA_SPARVEC::operator= : length of operands are different ";
      glob_->err() << "(" << size_ << " != " << rhs.size_ << ")." << endl;
      exit(Fatal);
    }

    for(int i = 0; i < rhs.nnz_; i++) {
      support_[i] = rhs.support_[i];
      coeff_[i]   = rhs.coeff_[i];
    }

    nnz_ = rhs.nnz_;
    glob_ = rhs.glob_;
    return *this;
  }

ostream& operator<<(ostream &out, const ABA_SPARVEC &rhs)
{
  for (int i = 0; i < rhs.nnz_; i++)
    out << rhs.support_[i] << " " << rhs.coeff_[i] << endl;
  return out;
}

  double ABA_SPARVEC::origCoeff(int i) const
  {
#ifdef TEST_SPARVEC_ORDER
    { int k;
    for (k = 1; k < nnz_; k++)
      if (support_[k-1]>support_[k])
        break;
    if (k==nnz_)
      cout << "ABA_SPARVEC::origCoeff vector is in order.\n";
    else
      cout << "ABA_SPARVEC::origCoeff vector is not in order.\n";
    }
#endif
    for (int k = 0; k < nnz_; k++) 
      if (support_[k] == i) return coeff_[k];
    return 0.0;
  }

  void ABA_SPARVEC::leftShift(ABA_BUFFER<int> &del)
  {
    const int nDel = del.number();

    if (nDel == 0) return;
    
    int i,j;
    int current = del[0];

    // shift all elements left of the last deleted element to the left side
    /* All elements in the arrays between the removed elements \a del[i] and
     *   del[i+1] are shifted left in the inner loop.
     */  
    for (i = 0; i < nDel - 1; i++) {

#ifdef ABACUSSAFE
      if(del[i] < 0 || del[i] >= nnz_) {
	glob_->err() << "ABA_BUFFER:leftShift(): shift index " << i << " not valid." << endl;
	exit(Fatal);
      }
#endif      

      const int last = del[i+1];

      for(j = del[i]+1; j < last; j++) {
	support_[current] = support_[j];
	coeff_[current]   = coeff_[j];
	++current;
      }
    }

    // shift all elements right of the last deleted element to the left side
#ifdef ABACUSSAFE
    if(del[nDel -1] < 0 || del[nDel - 1] >= nnz_) {
      glob_->err() << "ABA_BUFFER:leftShift(): shift index " << i << " not valid." << endl;
      exit(Fatal);
    }
#endif      

    for (j = del[nDel - 1] + 1; j < nnz_; j++) {
      support_[current] = support_[j];
      coeff_[current]   = coeff_[j];
      ++current;
    }


    nnz_ -= nDel;
  }

  void ABA_SPARVEC::copy(const ABA_SPARVEC &vec)
  {
    if(size_ < vec.nnz())
      realloc(vec.nnz());

    nnz_ = vec.nnz();

    for(int i = 0; i < nnz_; i++) {
      support_[i] = vec.support(i);
      coeff_[i]   = vec.coeff(i);
    }
  }

  void ABA_SPARVEC::rename(ABA_ARRAY<int> &newName)
  {
    for(int i = 0; i < nnz_; i++)
      support_[i] = newName[support_[i]];
  }

  double ABA_SPARVEC::norm()
  {
    double v = 0.0;

    for (int i = 0; i < nnz_; i++)
      v += coeff_[i]*coeff_[i];

    return sqrt(v);
  }

  void ABA_SPARVEC::realloc()
  {
    realloc((int) ((1.0 + reallocFac_/100.0) * size_) + 1);
  }

  void ABA_SPARVEC::realloc(int newSize)
  {
    if (newSize < nnz_) {
      glob_->err() << "ABA_SPARVEC::realloc(" << newSize << "):" << endl;
      glob_->err() << "  length of vector becomes less than number of nonzeros ";
      glob_->err() << nnz_ << "." << endl;
      exit (Fatal);
    }
    
    int    *newSupport = new int[newSize];
    double *newCoeff    = new double[newSize];

    for (int i = 0; i < nnz_; i++) {
      newSupport[i] = support_[i];
      newCoeff[i]   = coeff_[i];
    }

    delete [] support_;
    delete [] coeff_;

    support_ = newSupport;
    coeff_   = newCoeff;
    size_    = newSize;
  }

  void ABA_SPARVEC::rangeCheck(int i) const
  {
    if (i < 0 || i >= nnz_) {
      glob_->err() << "ABA_SPARVEC::rangeCheck(): index " << i;
      glob_->err() << " out of ranges of nonzeros [0," << nnz_-1 << "]." << endl;
      exit(Fatal);
    }
  }
