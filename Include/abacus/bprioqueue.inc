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

#ifndef ABA_BPRIOQUEUE_INC
#define ABA_BPRIOQUEUE_INC
  template<class Type, class Key>
  ABA_BPRIOQUEUE<Type, Key>::ABA_BPRIOQUEUE(ABA_GLOBAL *glob, int size) 
  :  
    glob_(glob),  
    heap_(glob, size)
  { }

  template<class Type, class Key>
  void ABA_BPRIOQUEUE<Type, Key>::insert(Type elem, Key key)
  {
    heap_.insert(elem, key);
  }

  template<class Type, class Key>
  int ABA_BPRIOQUEUE<Type, Key>::getMin(Type &min) const
  {
    //! ABA_BPRIOQUEUE: is the priority queue empty?
    if (heap_.empty()) return 1;

    min = heap_.getMin();
    return 0;
  }

  template<class Type, class Key>
  int ABA_BPRIOQUEUE<Type, Key>::getMinKey(Key &minKey) const
  {
    //! ABA_BPRIOQUEUE: is the priority queue empty?
    if (heap_.empty()) return 1;

    minKey = heap_.getMinKey();
    return 0;
  }

  template<class Type, class Key>
  int ABA_BPRIOQUEUE<Type, Key>::extractMin(Type& min)
  {
    //! ABA_BPRIOQUEUE: is the priority queue empty?
    if (heap_.empty()) return 1;

    min = heap_.extractMin();
    return 0;
  }

  template<class Type, class Key>
  void ABA_BPRIOQUEUE<Type, Key>::clear()
  {
    heap_.clear();
  }

  template<class Type, class Key>
  inline int ABA_BPRIOQUEUE<Type, Key>::size() const
  {
    return heap_.size();
  }

  template<class Type, class Key>
  inline int ABA_BPRIOQUEUE<Type, Key>::number() const
  {
    return heap_.number();
  }

  template<class Type, class Key>
  void ABA_BPRIOQUEUE<Type, Key>::realloc(int newSize)
  {
    if (newSize < size()) {
      char *_error="ABA_BPRIOQUEUE::realloc : priority queue cannot be decreased";
	THROW_PARAM(AlgorithmFailureException,afcBPrioQueue,_error);
    }

    heap_.realloc(newSize);
  }

#endif   //ABA_BPRIOQUEUE_INC
