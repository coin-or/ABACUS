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

#ifndef ABA_SORTER_INC
#define ABA_SORTER_INC

  template <class ItemType, class KeyType> 
  ABA_SORTER<ItemType, KeyType>::ABA_SORTER(ABA_GLOBAL *glob)
  :  
    glob_(glob)
  { }

  template <class ItemType, class KeyType> 
  void ABA_SORTER<ItemType, KeyType>::quickSort(int n, 
                                                ABA_ARRAY<ItemType> &items, 
                                                ABA_ARRAY<KeyType> &keys)
  {
    quickSort(items, keys, 0, n - 1);
  }

  template <class ItemType, class KeyType> 
  void ABA_SORTER<ItemType, KeyType>::quickSort(ABA_ARRAY<ItemType> &items, 
                                                ABA_ARRAY<KeyType> &keys, 
                                                int left, 
                                                int right)
  {
    if (left < right) {
      int q = partition(items, keys, left, right); 
                                              
      quickSort(items, keys, left, q);
      quickSort(items, keys, q+1, right);
    }
  }  

  template <class ItemType, class KeyType> 
  int ABA_SORTER<ItemType, KeyType>::
  partition(ABA_ARRAY<ItemType> &items, ABA_ARRAY<KeyType> &keys, 
            int left, 
            int right)
  {

    KeyType  k = keys[left];
    
    int l = left  - 1;
    int r = right + 1;

    while (1) {

      do r--; while(k < keys[r]);
      do l++; while(keys[l] < k);

      if(l < r) {
        //! swap \a l and \a r
	itemSwap_ = items[l];
	items[l] = items[r];
	items[r] = itemSwap_;
  
	keySwap_  = keys[l];
	keys[l]  = keys[r];
	keys[r]  = keySwap_;

      }  
      else
        return r;
    }
  }

  template <class ItemType, class KeyType> 
  void ABA_SORTER<ItemType, KeyType>::heapSort(int n, 
                                               ABA_ARRAY<ItemType> &items, 
                                               ABA_ARRAY<KeyType> &keys)
  {
    buildHeap(n, items, keys);

    for (int i = n - 1; i > 0; i--) {
      //! swap elements \a 0 and \a i
      itemSwap_ = items[0];
      items[0]  = items[i];
      items[i]  = itemSwap_;

      keySwap_ = keys[0];
      keys[0]  = keys[i];
      keys[i]  = keySwap_;

      heapify(i, items, keys, 0);
    }
  }

  template <class ItemType, class KeyType> 
  void ABA_SORTER<ItemType, KeyType>::
  buildHeap(int n, ABA_ARRAY<ItemType> &items, ABA_ARRAY<KeyType> &keys)
  {
    for (int i = n/2 - 1; i >= 0; i--)
      heapify(n, items, keys, i);
  }

  template <class ItemType, class KeyType> 
  void ABA_SORTER<ItemType, KeyType>::
  heapify(int n, ABA_ARRAY<ItemType> &items, ABA_ARRAY<KeyType> &keys, int root)
  {
    int l, r, largest;

    int lRoot = root;

    while (1) {
      l = 2*lRoot + 1;
      r = l + 1;

      //! determine the \a largest element of \a l, \a r, and \a lRoot
      if (l < n && keys[l] > keys[lRoot])
        largest = l;
      else
        largest = lRoot;
    
      if (r < n && keys[r] > keys[largest])
        largest = r;


      if (largest != lRoot) {
        //! swap the elements of \a largest and \a lRoot
        itemSwap_      = items[largest];
        items[largest] = items[lRoot];
        items[lRoot]   = itemSwap_;
 
        keySwap_      = keys[largest];
        keys[largest] = keys[lRoot];
        keys[lRoot]   = keySwap_;

        lRoot = largest;
      }
      else break;
    }
  }

  template <class ItemType, class KeyType> 
  void ABA_SORTER<ItemType, KeyType>::        
  check(int n, ABA_ARRAY<ItemType> &items, ABA_ARRAY<KeyType> &keys)
  {
    for (int i = 0; i < n - 1; i++)
      if (keys[i] > keys[i + 1]) {
        glob_->err() << "ABA_SORTER::check(): error" << endl;
        glob_->err() << "keys[" << i << "] = " << keys[i];
        glob_->err() << " > keys[" << i + 1 << "] = " << keys[i+1] << "." << endl;
        exit(Fatal);
      }
  }
    

#endif   // ABA_SORTER_INC
