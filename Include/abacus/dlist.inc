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

#ifndef ABA_DLIST_INC
#define ABA_DLIST_INC
    
  template<class Type>
  inline ABA_DLIST<Type>::ABA_DLIST(ABA_GLOBAL *glob)
  :  
    glob_(glob),  
    first_(0),  
    last_(0)
  {  }


  template<class Type>
  ABA_DLIST<Type>::~ABA_DLIST()
  {
     ABA_DLISTITEM<Type> *item = first_;
     ABA_DLISTITEM<Type> *del;
    
     while (item) {
       del = item;
       item = item->succ_;
       delete del;
     }
  }

  template <class Type>
  ostream& operator<<(ostream &out, const ABA_DLIST<Type> &list)
  {
    ABA_DLISTITEM<Type> *item = list.first_;

    while (item != 0) {
      out << *item << " ";
      item = item->succ();
    }
    out << endl;
    return out;
  }

  template<class Type> 
  void ABA_DLIST<Type>::append(const Type &elem)
  {
    ABA_DLISTITEM<Type> *item = new ABA_DLISTITEM<Type>(elem, last_, 0);
    if (first_ == 0) first_ = item;
    else last_->succ_ = item;
    last_ = item;
  }

  template <class Type>
  int ABA_DLIST<Type>::extractHead(Type &elem)
  {
    int status;
    if ((status = firstElem(elem))) return status;
    
    ABA_DLISTITEM<Type> *second = first_->succ_;

    delete first_;
    first_ = second;
    // FIX fixes bug reported by Christophe Meyer
    if (first_ == 0) last_ = 0;

    return status;
  }

  template <class Type>
  int ABA_DLIST<Type>::removeHead()
  {
    if (first_) {
      ABA_DLISTITEM<Type> *second = first_->succ_;

      delete first_;
      first_ = second;
      // FIX fixes bug reported by Christophe Meyer
      if (first_ == 0) last_ = 0;
      return 0;
    }
    else return 1;
  }

  template <class Type>
  void ABA_DLIST<Type>::remove(ABA_DLISTITEM<Type> *item)
  {
    if (item == 0) return;

    //! update the predecessor

    if (first_ == item) {
      first_ = item->succ_;
      if(first_ != 0) first_->pred_ = 0;
    }
    else
     item->pred_->succ_ = item->succ_;

    //! update the successor
  
    if (last_ == item) {
      last_ = item->pred_;
      if (last_ != 0) last_->succ_ = 0;
    }
    else
      item->succ_->pred_ = item->pred_;
    
    delete item;
  }

  template<class Type>
  void ABA_DLIST<Type>::remove(const Type &elem)
  {
    ABA_DLISTITEM<Type> *item = first_;

    while (item != 0) {
      if (item->elem_ == elem) {
        remove(item);
        return;
      }
      item = item->succ_;
    }
  }

  template<class Type>
  inline ABA_DLISTITEM<Type>* ABA_DLIST<Type>::first() const
  {
    return first_;
  }

  template<class Type>
  inline ABA_DLISTITEM<Type>* ABA_DLIST<Type>::last() const
  {
    return last_;
  }

  template<class Type>
  inline bool ABA_DLIST<Type>::empty() const
  {
    return first_ == 0 ? true : false;
  }

  template <class Type>
  inline int ABA_DLIST<Type>::firstElem(Type &elem) const
  {
    if (empty()) return 1;
    elem = first()->elem();
    return 0;
  }
  
#endif   // ABA_DLIST_INC
