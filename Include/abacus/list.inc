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

#ifndef ABA_LIST_INC
#define ABA_LIST_INC

  template<class Type>
  inline ABA_LIST<Type>::ABA_LIST(const ABA_GLOBAL *glob) 
  :  
    glob_(glob),  
    first_(0),  
    last_(0)
  {  }

  template<class Type>
  ABA_LIST<Type>::~ABA_LIST()
  {
     ABA_LISTITEM<Type> *item = first_;
     ABA_LISTITEM<Type> *del;
    
     while (item) {
       del = item;
       item = item->succ_;
       delete del;
     }
  }

  template <class Type>
  ostream& operator<<(ostream &out, const ABA_LIST<Type> &list)
  {
    ABA_LISTITEM<Type> *item = list.first_;

    while (item != 0) {
      out << *item << " ";
      item = item->succ();
    }
    out << endl;
    return out;
  }

  template<class Type>
  inline void ABA_LIST<Type>::appendHead(const Type &elem)
  {
    appendHead(new ABA_LISTITEM<Type>(elem, first_));
  }
 
  template<class Type>
  inline void ABA_LIST<Type>::appendHead(ABA_LISTITEM<Type> *item)
  {
    first_ = item;
    if (last_ == 0) last_ = item;
  }
 
  template<class Type>
  inline void ABA_LIST<Type>::appendTail(const Type &elem)
  {
    appendTail(new ABA_LISTITEM<Type>(elem, 0));
  }

  template<class Type>
  inline void ABA_LIST<Type>::appendTail(ABA_LISTITEM<Type> *item)
  {
    if (first_ == 0) first_ = item;
    else last_->succ_ = item;
    last_ = item;
  }

  template <class Type>
  int ABA_LIST<Type>::extractHead(Type &elem)
  {
    int status;
    if ((status = firstElem(elem))) return status;
    
    ABA_LISTITEM<Type> *second = first_->succ_;

    delete first_;
    first_ = second;
    // FIX fixes bug reported by Christophe Meyer
    if (first_ == 0) last_ = 0;

    return status;
  }

  template <class Type>
  inline int ABA_LIST<Type>::firstElem(Type &elem) const
  {
    if (empty()) return 1;
    elem = first()->elem();
    return 0;
  }

  template<class Type>
  inline ABA_LISTITEM<Type> * ABA_LIST<Type>::first() const
  {
    return first_;
  }

  template<class Type>
  inline ABA_LISTITEM<Type> * ABA_LIST<Type>::last() const
  {
    return last_;
  }
  
  template<class Type>
  inline bool ABA_LIST<Type>::empty() const
  {
    return first_ == 0 ? true : false;
  }

#endif   // ABA_LIST_INC
