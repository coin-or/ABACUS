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

#ifndef ABA_ARRAY_INC
#define ABA_ARRAY_INC


using std::endl;
template <class Type> 
  inline ABA_ARRAY<Type>::ABA_ARRAY(ABA_GLOBAL *glob, int size)
    : 
    glob_(glob),  
    n_(size)
  {
#ifdef ABACUSSAFE
    if (size < 0) {
      char *_error= "ABA_ARRAY::ABA_ARRAY(): cannot construct array with negative size";
      THROW_PARAM(AlgorithmFailureException,afcArray,_error);
    }
#endif  
    a_ = new Type[size];
}

  template <class Type>
  ABA_ARRAY<Type>::ABA_ARRAY(ABA_GLOBAL *glob, int size, Type init) 
    :  
     glob_(glob),  
     n_(size)
  {
#ifdef ABACUSSAFE
    if (size < 0) {
      char *_error = "ABA_ARRAY::ABA_ARRAY(): cannot construct array with negative size";
	THROW_PARAM(AlgorithmFailureException,afcArray,_error);
    }
#endif  

    a_ = new Type[size];
    set(init);
}

  template <class Type>
  ABA_ARRAY<Type>::ABA_ARRAY(ABA_GLOBAL *glob, const ABA_BUFFER<Type> &buf) 
  :  
  glob_(glob),  
  n_(buf.number())
  {
    a_ = new Type[n_] ;
    for (int i = 0; i < n_; i++) a_[i] = buf[i];
  }

  template <class Type> 
ABA_ARRAY<Type>::ABA_ARRAY(const ABA_ARRAY<Type> &rhs) 
  : 
  glob_(rhs.glob_),  
  n_(rhs.n_)
  {
    a_ = new Type[n_];
    for (int i = 0; i < n_; i++) a_[i] = rhs[i];
  }

  template <class Type> 
  inline ABA_ARRAY<Type>::~ABA_ARRAY()
  {
    delete [] a_;
  }


 template <class Type>
 const ABA_ARRAY<Type>& ABA_ARRAY<Type>::operator=(const ABA_ARRAY<Type>& rhs)
 {
 if (this == &rhs) return *this;

 // check if the dimensions of both arrays are equal
 /* If in an assignment operation of two arrays the dimensions of both
  * arrays are different we terminate the program.
  */
  if (n_ != rhs.n_) {
     char *_error=new char [255];	
     sprintf(_error,"ABA_ARRAY::operator= : dimensions of left and right hand side are different\n (%i!= %i)",n_,rhs.n_);
     THROW_PARAM(AlgorithmFailureException,afcArray,_error);
     }

 glob_ = rhs.glob_;
 n_    = rhs.n_;

 for (int i = 0; i < n_; i++) a_[i] = rhs[i];

 return *this;
 }

template <class Type>
  const ABA_ARRAY<Type>& ABA_ARRAY<Type>::operator=(const ABA_BUFFER<Type>& rhs)
  {
    if (n_ < rhs.size()) {
      char *_error="ABA_ARRAY::operator=(const ABA_BUFFER&): size of ABA_ARRAY too small.";
      THROW_PARAM(AlgorithmFailureException,afcArray,_error);
    }

    const int rhsNumber = rhs.number();

    for (int i = 0; i < rhsNumber; i++) a_[i] = rhs[i];

    return *this;
  
  }

  template <class Type>
  ostream& operator<<(ostream &out, const ABA_ARRAY<Type> &array)
  {
    const int s = array.size();

    for (int i = 0; i < s; i++) out << i << ": " << array[i] << endl;
    return out;
  }

  template <class Type>
inline  Type& ABA_ARRAY<Type>::operator[](int i)
  {
#ifdef ABACUSSAFE
    rangeCheck(i);
#endif
    
    return a_[i];
  }

  template <class Type>
inline  const Type& ABA_ARRAY<Type>::operator[](int i) const
  {

#ifdef ABACUSSAFE
    rangeCheck(i);
#endif  
  
    return a_[i];

  }

  template<class Type>
  void ABA_ARRAY<Type>::copy(const ABA_ARRAY<Type> &rhs)
  {
    copy(rhs, 0, rhs.size() - 1);
  }

  template<class Type>
  void ABA_ARRAY<Type>::copy(const ABA_ARRAY<Type> &rhs, int l, int r)
  {
    // exit if \a r < l
    if (r < l) {
      char *_error= new char[255];
	sprintf(_error,"ABA_ARRAY::copy() : r = %i  is less than r = %i",r,l);
        THROW_PARAM(AlgorithmFailureException,afcArray,_error);
    }

    // reallocate storage if necessary
    if (n_ < r-l+1) {
      delete [] a_;
      n_ = r-l+1;
      a_ = new Type[n_];
    }

    // copy the elements
    for (int i = 0; i <= r-l; i++) a_[i] = rhs[l + i];

  }

  template <class Type>
  void ABA_ARRAY<Type>::leftShift(ABA_BUFFER<int> &ind)
  {
    const int nInd = ind.number();

    if (nInd == 0) return;
  
    int i,j;
    int current = ind[0];
  
    // shift all items of the array up to the last element of \a ind to the left
    // perform a range check for the array for \a ind[0]
#ifdef ABACUSSAFE
    if(ind[0] < 0 || ind[0] >= n_) {
      char *_error="ABA_ARRAY:leftShift(): shift index not valid.";
      THROW_PARAM(AlgorithmFailureException,afcArray,_error);
    }
#endif      

    for (i = 0; i < nInd - 1; i++) {
	// perform a range check for the array for \a ind[i+1]
#ifdef ABACUSSAFE
        if(ind[i+1] < 0 || ind[i+1] >= n_) {
           char *_error=new char [255];
	   sprintf(_error,"ABA_ARRAY:leftShift(): shift index %i not valid.", i+1);
	   THROW_PARAM(AlgorithmFailureException,afcArray,_error);
        }
#endif      

	const int last = ind[i+1];
	for(j = ind[i]+1; j < last; j++)
	a_[current++] = a_[j];
    }

    // copy the rest of the array
    for (j = ind[nInd - 1] + 1; j < n_; j++)
    a_[current++] = a_[j];
  }

  template <class Type>
  void ABA_ARRAY<Type>::leftShift(ABA_ARRAY<bool> &remove)
  {
    ABA_BUFFER<int> removeIndex(glob_, n_);

    for (int i = 0; i < n_; i++)
      if (remove[i])
        removeIndex.push(i);
    
    leftShift(removeIndex);
  }
  
  template <class Type>
  void ABA_ARRAY<Type>::set(int l, int r, Type val)
  {
    for (int i = l; i <= r; i++) a_[i] = val;
  }

  template <class Type>
  void ABA_ARRAY<Type>::set(Type val)
  {
    set(0, n_ - 1, val);
  }

  template <class Type>
  void ABA_ARRAY<Type>::rangeCheck(int i) const
  {
    if (i < 0 || i >= n_) {
      char *_error=new char[255];
      sprintf(_error, "ABA_ARRAY:operator[] : index %i out of ranges [0, %i] ",i,n_-1);
      THROW_PARAM(AlgorithmFailureException,afcArray,_error); 
    }
  }

  template <class Type>
  inline int ABA_ARRAY<Type>::size() const
  {
    return n_;
  }

  template <class Type>
  void ABA_ARRAY<Type>::realloc(int newSize)
  {
    if (newSize != n_) {
      Type *newA = new Type[newSize];

      int length = n_ < newSize ? n_ : newSize;

      for (int i = 0; i < length; i++) newA[i] = a_[i];

      delete [] a_;

      a_ = newA;
      n_ = newSize;
    }
  }

  template <class Type>
  void ABA_ARRAY<Type>::realloc(int newSize, Type init)
  {
    if (newSize != n_) {
      delete [] a_;

      a_ = new Type[newSize];
      n_ = newSize;
    }

    set(init);
  }

#endif   // ABA_ARRAY_INC
