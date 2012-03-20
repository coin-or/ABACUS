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

#ifndef ABA_HASH_INC
#define ABA_HASH_INC

#include "abacus/string.h"

#include <math.h>

  template <class KeyType, class ItemType>
  inline ABA_HASHITEM<KeyType, ItemType>::ABA_HASHITEM(
         const KeyType &key, 
         const ItemType &item) :  
    key_(key),  
    item_(item),  
    next_(0)
  {  }

  template <class KeyType, class ItemType>
  ostream &operator<<(ostream &out, const ABA_HASHITEM<KeyType, ItemType> &rhs)
  {
    return out << '(' << rhs.key_ << ',' << rhs.item_ << ')';
  }

  template <class KeyType, class ItemType>
  inline ABA_HASHITEM<KeyType, ItemType> * ABA_HASHITEM<KeyType, 
     ItemType>::next()
  {
    return next_;
  }

  template <class KeyType, class ItemType>
  ABA_HASH<KeyType, ItemType>::ABA_HASH(ABA_GLOBAL *glob, int size)
  :  
    glob_(glob),  
    size_(size),  
    nCollisions_(0),  
    iter_(0)
  {  

    table_ = new ABA_HASHITEM<KeyType, ItemType>* [size];

    for (int i = 0; i < size; i++) table_[i] = 0;
  }

  template <class KeyType, class ItemType>
  ABA_HASH<KeyType, ItemType>::~ABA_HASH()
  {
    ABA_HASHITEM<KeyType, ItemType> *h1;
    ABA_HASHITEM<KeyType, ItemType> *h2;
    int i;
    
    for (i = 0; i < size_; i++) {
      if((h1 = table_[i])) 
        while (h1) {
          h2 = h1->next_;
          delete h1;
          h1 = h2;
        }
    }
    delete [] table_;
  }

  template <class KeyType, class ItemType>
  ostream &operator<<(ostream &out, const ABA_HASH<KeyType, ItemType> &hash)
  {

    ABA_HASHITEM<KeyType, ItemType> *h;
    const int s = hash.size();
    
    for (int i = 0; i < s; i++) {
      h = hash.table_[i];
      if (h) {
        out << i << ':';
        while(h) {
          out << *h << ' ';
          h = h->next();
        }  
        out << endl;
      }  
    }  
    return out;
  }

  template <class KeyType, class ItemType>
  void ABA_HASH<KeyType, ItemType>::insert(const KeyType &key, 
                                           const ItemType &item)
  {
    ABA_HASHITEM<KeyType, ItemType> *h = new ABA_HASHITEM<KeyType, ItemType>(key, item);
    int slotNum = hf(key);

    if (table_[slotNum]) ++nCollisions_;
    h->next_ = table_[slotNum];
    table_[slotNum] = h;

  }

  template <class KeyType, class ItemType>
  void ABA_HASH<KeyType, ItemType>::overWrite(const KeyType &key, 
                                              const ItemType &item)
  {
    //! overWrite(): find the slot of the \a key
    int slotNum = hf(key);
    if (table_[slotNum]) ++nCollisions_;

    ABA_HASHITEM<KeyType, ItemType> *h = table_[slotNum];

    //! find the \a key and the \a item in the list of the slot
    /*! As soon as we find the \a item we can overwrite it and return.
     */
    while (h) {
      if (h->key_ == key) {
        h->item_ = item;
	return;
      }
      h = h->next_;
    }

    //! if the search is not successful, perform a normal insertion
    h               = new ABA_HASHITEM<KeyType, ItemType>(key, item);
    h->next_        = table_[slotNum];
    table_[slotNum] = h;

  }

  template <class KeyType, class ItemType>
  ItemType * ABA_HASH<KeyType, ItemType>::find(const KeyType &key)
  {
    ABA_HASHITEM<KeyType, ItemType> *slot;

    slot = table_[hf(key)];

    while (slot) {
      if (key == slot->key_) return &(slot->item_);
      slot = slot->next_;
    }
    return 0;
  }

  template <class KeyType, class ItemType>
  bool ABA_HASH<KeyType, ItemType>::find (const KeyType &key, const ItemType &item)
  {
    ABA_HASHITEM<KeyType, ItemType> *slot;

    slot = table_[hf(key)];

    while (slot) {
      if (key == slot->key_ && slot->item_ == item) return true;
      slot = slot->next_;
    }
    return false;
  }

  template <class KeyType, class ItemType>
  ItemType *ABA_HASH<KeyType, ItemType>::initializeIteration(const KeyType &key)
  {
    iter_ = table_[hf(key)];
    while (iter_) {
      if (key == iter_->key_) return &(iter_->item_);
      iter_ = iter_->next_;
    }
    return 0;
  }

  template <class KeyType, class ItemType>
  ItemType *ABA_HASH<KeyType, ItemType>::next(const KeyType &key)
  {
    if (iter_ == 0) return 0;
    iter_ = iter_->next_;

    while (iter_) {
      if (key == iter_->key_) return &(iter_->item_);
      iter_ = iter_->next();
    }
    return 0;
  }

  template <class KeyType, class ItemType>
  int ABA_HASH<KeyType, ItemType>::remove(const KeyType &key)
  {
    // remove(): find the slot and return if it is empty
    ABA_HASHITEM<KeyType, ItemType> *h1;
    ABA_HASHITEM<KeyType, ItemType> *h2;
    int slotNum = hf(key);

    h1 = table_[slotNum];
    if (h1 == 0)
       return 1;

    // check if the first item is being removed
    if (h1->key_ == key) {
      table_[slotNum] = h1->next_;
      delete h1;
      return 0;
    }

    // otherwise, go through the linked list
    while ((h2 = h1->next_)) {
      if (h2->key_ == key) {
        h1->next_ = h2->next_;
	delete h2;
	return 0;
      }
      h1 = h2;
    }
    return 1;
  }

  template <class KeyType, class ItemType>
  int ABA_HASH<KeyType, ItemType>::remove(const KeyType &key, const ItemType &item)
  {
    // remove(): find the slot and return if it is empty
    ABA_HASHITEM<KeyType, ItemType> *h1;
    ABA_HASHITEM<KeyType, ItemType> *h2;
    int slotNum = hf(key);

    h1 = table_[slotNum];
    if (h1 == 0)
      return 1;

    // check \a key and \a item of the head of the list
    if (h1->key_ == key && h1->item_ == item) {
      table_[slotNum] = h1->next_;
      delete h1;
      return 0;
    }

    // check \a key and \a item of the other elements of the list
    while ((h2 = h1->next_)) {
      if (h2->key_ == key && h2->item_ == item) {
        h1->next_ = h2->next_;
        delete h2;
        return 0;
      }
      h1 = h2;
    }
    return 1;

  }

  template <class KeyType, class ItemType>
  inline int ABA_HASH<KeyType, ItemType>::size() const
  {
    return size_;
  }

  template <class KeyType, class ItemType>
  inline int ABA_HASH<KeyType, ItemType>::nCollisions() const
  {
    return nCollisions_;
  }
  
  template <class KeyType, class ItemType>
  inline int ABA_HASH<KeyType, ItemType>::hf(int key)
  {
    if (key < 0) key = -key;

    double x = key*0.6180339887;

    return (int) (size()*(x-floor(x)));
  }

  template <class KeyType, class ItemType>
  inline int ABA_HASH<KeyType, ItemType>::hf(unsigned key)
  {
    double x = key*0.6180339887;

    return (int) (size()*fmod(x, 1.0));
  }

  template <class KeyType, class ItemType>
  int ABA_HASH<KeyType, ItemType>::hf(const ABA_STRING &string)
  {

    const int prime = 516595003;
    const int mult  = 314159;
    const int s     = string.size();
    int h = 0;
    for (int i = 0; i < s; i++) {
      h += (h ^ (h >> 1)) + mult * (unsigned char) string[i];
      while (h >= prime) h -= prime;
    }

    return h % s;
  }


  template <class KeyType, class ItemType>
  void ABA_HASH<KeyType, ItemType>::resize(int newSize)
  {
    // check the value of \a newSize
    if (newSize <= 0) {
      char *_error= "ABA_HASH::resize(): new size of hash table must be positive.";
      THROW_PARAM(AlgorithmFailureException,afcHash,_error);
    }

    // allocate a new hash table
    /* We have to set the entries of the new hash table to 0 that we 
     * can insert the items in the linear lists of the slots in a simple way later.
     */
    ABA_HASHITEM<KeyType, ItemType> **newTable;

    newTable = new ABA_HASHITEM<KeyType, ItemType>* [newSize];

    for (int i = 0; i < newSize; i++) 
      newTable[i] = 0;

    // insert all elements of the old table into the new table
    /* We cannot make copies of slots of the old hash tables but have to reinsert
     * all elements into the new hash table since the hash function might have
     * changed. For efficieny we move each hash item to the new slot.
     *
     * We replace already here the old size with the new size of the hash table,
     * because we need the hash function according to the new size.
     */
    int oldSize = size_;
    size_       = newSize;

    for (int i = 0; i < oldSize; i++) 
    if (table_[i]) {
      // move the items to the corresponding new slots
      ABA_HASHITEM<KeyType, ItemType> *current = table_[i];
      ABA_HASHITEM<KeyType, ItemType> *next;

      while (current) {
        int slotNum = hf(current->key_);
	next = current->next_;

	current->next_    = newTable[slotNum];
	newTable[slotNum] = current;
	current           = next;
      }

    }

    // replace the old table by the new one
    delete table_;
    table_ = newTable;

  }

#endif   // ABA_HASH_INC

