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

#ifndef ABA_RING_INC
#define ABA_RING_INC

  template <class Type>
  inline ABA_RING<Type>::ABA_RING(ABA_GLOBAL *glob, int size)
  :  
    glob_(glob),  
    ring_(glob, size),  
    head_(0),  
    filled_(false)
  { }
 
  template <class Type> 
  inline ABA_RING<Type>::~ABA_RING()
  {
  }

  template <class Type>
  ostream &operator<<(ostream &out, const ABA_RING<Type> &rhs)
  {
    
    if(rhs.filled_) {
      const int s = rhs.size();

      for(int i = rhs.head_; i < s; i++)
        out << rhs.ring_[i] << " ";
    }

    for (int i = 0; i < rhs.head_; i++)
        out << rhs.ring_[i] << " ";

    out << endl;

    return out;
  }

  template <class Type>
  inline Type& ABA_RING<Type>::operator[](int i)
  {
    return ring_[i];
  }

  template <class Type>
  inline const Type& ABA_RING<Type>::operator[](int i) const
  {
    return ring_[i];
  }

  template <class Type>
  void ABA_RING<Type>::insert(Type elem)
  {
    ring_[head_] = elem;

    if (++head_ == size()) {
      if (!filled_) filled_ = true;
      head_ = 0;
    }
  }

  template <class Type>
  inline void ABA_RING<Type>::clear()
  {
    head_ = 0;
    filled_ = false;
  }

  template <class Type>
  inline int ABA_RING<Type>::size() const
  {
    return ring_.size();
  }

  template <class Type>
  inline int ABA_RING<Type>::number() const
  {
    if (filled_) 
      return size();
    else
      return head_;
  }

  template <class Type>
  inline Type ABA_RING<Type>::oldest() const
  {
    if(filled_) return ring_[head_];
    else return ring_[0];
  }

  template <class Type>
  inline int ABA_RING<Type>::oldestIndex() const
  {
    if(filled_) return head_;
    else return 0;
  }

  template <class Type>
  inline Type ABA_RING<Type>::newest() const
  {
    if (head_)  return ring_[head_ - 1];
    else return ring_[size() - 1];
  }

  template <class Type>
  inline int ABA_RING<Type>::newestIndex() const
  {
    if (head_)  return head_ - 1;
    else return size() - 1;
  }

  template <class Type>
  int ABA_RING<Type>::previous(int i, Type &p) const
  {
    int j = head_ - 1 - i;

    if (j >= 0) {
      p = ring_[j];
      return 0;
    }
    else if (filled_) {
      p = ring_[size() + j];
      return 0;
    }
    else return 1;
  }

  template <class Type>
  inline bool ABA_RING<Type>::empty() const
  {
#ifdef ABACUS_NO_BOOL
    if (head_ || filled_) return false;
    else                  return true;
#else
    return !(head_ || filled_);
#endif
  }

  template <class Type>
  inline bool ABA_RING<Type>::filled() const
  {
    return filled_;
  }

  template <class Type>
  void ABA_RING<Type>::realloc(int newSize)
  {
    ABA_ARRAY<Type> tmp = ring_;
    int oldSize = size();
    int oldHead = head_;
    int i;

    
    ring_.realloc(newSize);

    if(newSize > oldSize) {
      // increase ring
      /* If the ring is increased yet has not been filled, nothing has to be
       * done. Otherwise, the elements of the old ring are copied in the correct
       * order.
       */ 
      if (filled_) {
        head_ = 0;
	for(i = oldHead; i < oldSize; i++) ring_[head_++] = tmp[i];
	for(i = 0; i < oldHead; i++) ring_[head_++] = tmp[i];
	filled_ = false;
      }

    }
    else {
      // decrease ring
      /* If the ring is decreased and it is filled, then we copy the elements of the
       * old ring in the correct order. If the ring is not filled we only have
       * to copy the elements if by decreasing the ring elements are removed.
       */
      if (filled_) {
        for (head_ = size() - 1, i = oldHead - 1; head_ >= 0; head_--, i--) {
	  if (i < 0) i = oldSize - 1;
	  ring_[head_] = tmp[i];
        }
        head_ = 0;
      }
      else if (oldHead > size()) {
        for (head_ = size() - 1, i = oldHead - 1; head_ >= 0; --head_, --i)
          ring_[head_] = tmp[i];
        head_ = 0;
        filled_ = true;
      }

    
    }
  }


#endif   // ABA_RING_INC
