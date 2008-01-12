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

#ifndef ABA_BUFFER_INC
#define ABA_BUFFER_INC

#ifdef ABACUS_PARALLEL
#include "abacus/message.h"
#endif

template <class Type> 
inline ABA_BUFFER<Type>::ABA_BUFFER(ABA_GLOBAL *glob, int size) 
  :
  glob_(glob),
  size_(size),
  n_(0)

{ 
  buf_ = new Type[size];
}

template <class Type>
ABA_BUFFER<Type>::ABA_BUFFER(const ABA_BUFFER<Type> &rhs) 
  :  
  glob_(rhs.glob_),  
  size_(rhs.size_),  
  n_(rhs.n_)
{
  buf_ = new Type[size_] ;
  for (int i = 0; i < n_; i++) buf_[i] = rhs.buf_[i];
}

template <class Type> 
inline ABA_BUFFER<Type>::~ABA_BUFFER()
{
  delete [] buf_;
}

#ifdef ABACUS_PARALLEL

  template <class Type>
  ABA_BUFFER<Type>::ABA_BUFFER(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
  :  
    glob_(glob)
  {
    glob_->err() << "ABA_BUFFER::ABA_BUFFER() : A ABA_BUFFER of some type coudn't"
                    " be received. You have to implement a template"
                    " specialization of the message constructor for"
                    "  that type!" << endl;
    exit(Fatal);
  }

  template<class Type>
  void ABA_BUFFER<Type>::pack(ABA_MESSAGE &msg) const
  {
    glob_->err() << "ABA_BUFFER::pack() : A ABA_BUFFER of some type coudn't"
                    " be sent. You first have to implement a template"
                    " specialization of the pack() template function"
                    " for that type!" << endl;
    exit(Fatal);
  }

#endif

template <class Type>
const ABA_BUFFER<Type>& ABA_BUFFER<Type>::operator=(const ABA_BUFFER<Type>& rhs)
{
  if (this != &rhs) {
    // check if the dimension of both buffers are equal
    /* If in an assignment operation of two buffers the dimension of both
     * buffers do not equal we terminate the program.
     */
     if (size_ != rhs.size_) {
       glob_->err() << "ABA_BUFFER::operator= : dimension of left and right hand side ";
       glob_->err() << "do not equal (" << size_ << " != " << rhs.size_ << ")" << endl;
       exit(Fatal);
     }

    // assign the elements of ABA_BUFFER
    glob_ = rhs.glob_;
    size_ = rhs.size_;
    n_    = rhs.n_;

    for (int i = 0; i < n_; i++) buf_[i] = rhs.buf_[i];

  }

  return *this;
}

template <class Type>
ostream& operator<<(ostream &out, const ABA_BUFFER<Type> &buffer)
{
  for (int i = 0; i < buffer.number(); i++)
    out << i << ": " << buffer[i] << endl;
  return out;
}

template <class Type>
inline Type& ABA_BUFFER<Type>::operator[](int i)
{
  // \a Buffer: body of operator \a []
  /* Only access to already buffered elements is permitted.
   */ 
#ifdef ABACUSSAFE
  if(i < 0 || i >= n_) {
    glob_->err() << "ABA_BUFFER::operator[" << i << "]: only ";
    glob_->err() << n_ << " elements buffered." << endl;
    exit(Fatal);
  }
#endif

  return buf_[i];

}

template <class Type>
inline const Type& ABA_BUFFER<Type>::operator[](int i) const
{
  // \a Buffer: body of operator \a []
  /* Only access to already buffered elements is permitted.
   */ 
#ifdef ABACUSSAFE
  if(i < 0 || i >= n_) {
    glob_->err() << "ABA_BUFFER::operator[" << i << "]: only ";
    glob_->err() << n_ << " elements buffered." << endl;
    exit(Fatal);
  }
#endif

  return buf_[i];

}

template <class Type>
inline int ABA_BUFFER<Type>::size() const
{
  return size_;
}

template <class Type>
inline int ABA_BUFFER<Type>::number() const
{
  return n_;
}

template <class Type>
inline bool ABA_BUFFER<Type>::full() const
{
  return n_ == size() ? true : false;
}

template <class Type>
inline bool ABA_BUFFER<Type>::empty() const
{
  return n_ == 0 ? true : false;
}

template <class Type>
inline void ABA_BUFFER<Type>::push(Type item)
{
#ifdef ABACUSSAFE
  if(full()) {
   glob_->err() << "ABA_BUFFER::push(): buffer is full, " << n_ << " elements buffered" << endl;
   exit(Fatal);
  }
#endif

  buf_[n_++] = item;
}

template <class Type>
inline Type ABA_BUFFER<Type>::pop()
{
#ifdef ABACUSSAFE
  if(empty()) {
    glob_->err() << "ABA_BUFFER::pop(): buffer is empty." << endl;
    exit(Fatal);
  }
#endif

  return buf_[--n_];
}

template <class Type>
inline void ABA_BUFFER<Type>::clear()
{
  n_ = 0;
}

template <class Type>
void ABA_BUFFER<Type>::leftShift(ABA_BUFFER<int> &ind)
{
  const int nInd = ind.number();

  if (nInd == 0) return;
  
  int i,j;
  int current = ind[0];

  //! shift all items up to the lass element of \a ind to the left
  //! perform a range check for \a ind[0]
#ifdef ABACUSSAFE
  if(ind[0] < 0 || ind[0] >= n_) {
    glob_->err() << "ABA_BUFFER:leftShift(): shift index 0 (";
    glob_->err() << ind[0] << ") not valid" << endl;
    exit(Fatal);
  }
#endif      

  for (i = 0; i < nInd - 1; i++) {
    //! perform a range check for \a ind[i+1]
#ifdef ABACUSSAFE
    if(ind[i+1] < 0 || ind[i+1] >= n_) {
      glob_->err() << "ABA_BUFFER:leftShift(): shift index " << i+1;
      glob_->err() << " (" << ind[i+1] << ") not valid" << endl;
      exit(Fatal);
    }
#endif      

    const int last = ind[i+1];
    for(j = ind[i]+1; j < last; j++)
      buf_[current++] = buf_[j];
  }

  //! copy the rest of the buffer
  for (j = ind[nInd - 1]+1; j < n_; j++)
    buf_[current++] = buf_[j];

  n_ -= nInd;

}

template <class Type>
void ABA_BUFFER<Type>::realloc(int newSize)
{

  Type *newBuf = new Type[newSize];
  int   newN;

  if (n_ < newSize) newN = n_;
  else              newN = newSize;

  for (int i = 0; i < newN; i++) newBuf[i] = buf_[i];

  delete [] buf_;

  buf_   = newBuf;
  size_  = newSize;
  n_     = newN;
}

#endif   //!ABA_BUFFER_INC
