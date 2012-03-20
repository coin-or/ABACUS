/*!\file
 * \author Matthias Elf
 * \brief hash table.
 *
 * This data structure stores a set of items and provides as central
 * functions the insertion of a new item, the search for an item, and
 * the deletion of an item.
 *
 * \par
 * Each item is associated with a key. The set of all possible keys is
 * called the universe. A hash table has a fixed size \f$n\f$. A hash function
 * assigns to each key of the universe a number in \f$\{0,\dots, n-1\}\f$,
 * which we denote slot. If an item is inserted in the hash table,
 * then it is stored in the component of the array associated with its slot.
 * Usually, \a n is much smaller than the cardinality
 * of the universe. Hence, it can happen that two elements are mapped
 * to the same slot. This is called a collision. In order to resolve
 * collisions, each slot of the hash table does not store an item
 * explicitly, but is the start of a linear list storing all items
 * mapped to this slot.
 *
 * \par
 * This template implements a hash table where collisions are resolved
 * by chaining.
 * Currently hash functions for keys of type \a int and ABA_STRING
 * are implemented. If you want to use this data structure for other
 * types (e.g., \a YOURTYPE), you should derive a class from the class ABA_HASH
 * and define a hash function \a {int hf(YOURTYPE key)}.
 *
 * \par
 * The following sections implement two new classes. The class ABA_HASH contains
 * the hash table which consists of pointers to the class ABA_HASHITEM.
 * The class ABA_HASHITEM stores
 * an inserted element and its key and provides the a pointer to the next item
 * in the linked list.
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


#ifndef ABA_HASH_H
#define ABA_HASH_H

#include <iostream>
using namespace std;

class ABA_GLOBAL;
class ABA_STRING;

#include "abacus/abacusroot.h"
#include "abacus/exceptions.h"

template<class KeyType,class ItemType> class ABA_HASH;


template <class KeyType, class ItemType>
class  ABA_HASHITEM;

template <class KeyType, class ItemType>
class  ABA_HASH;

template <class KeyType, class ItemType>
ostream &operator<< (ostream &out, const ABA_HASHITEM<KeyType, ItemType> &rhs);

template <class KeyType, class ItemType>
ostream &operator<< (ostream &out, const ABA_HASH<KeyType, ItemType> &hash);

  template <class KeyType, class ItemType>
  class  ABA_HASHITEM :  public ABA_ABACUSROOT  { 
    friend class ABA_HASH<KeyType, ItemType>;

    public: 

/*! The constructor.
 *
 *     \param key The key of the item.
 *     \param item The value of the item.
 */
      ABA_HASHITEM(const KeyType &key, const ItemType &item);


/*! The output operator writes the key and the value of the item on the stream \a out.
 *
 * \return A reference to the output stream.
 */
      friend ostream &operator<< <> (ostream &,
                                     const ABA_HASHITEM<KeyType, ItemType> &);

/*! \brief Returns a pointer to the next hash-item stored in
 * the linked list corresponding to the slot of this item.
 */
      ABA_HASHITEM<KeyType, ItemType> *next();
    private: 
      KeyType                      key_;
      ItemType                     item_;
      ABA_HASHITEM<KeyType, ItemType> *next_;
  };


  template <class KeyType, class ItemType>
  class  ABA_HASH :  public ABA_ABACUSROOT  { 
    public:

/*! \brief Initializes each slot with a 0-pointer
 * to indicate that the linked list of hash items of this slot is empty.
 *
 *     \param glob A pointer to the corresponding global object.
 *     \param size The size of the hash table.
 */ 
      ABA_HASH(ABA_GLOBAL *glob, int size);

/*! \brief The destructor
 * deletes for each hash item by going through all non-empty lists of hash items.
 */
      ~ABA_HASH();

/*! \brief The output operator
 * writes row by row all elements stored
 * in the list associated with a slot on an output stream.
 *
 * The output of an empty slot is suppressed.
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param rhs The hash table being output.
 */
      friend ostream &operator<< <> (ostream &out, 
                                     const ABA_HASH<KeyType, ItemType> &hash);

/*! Adds an item to the hash table.
 *
 * The new item is inserted at the head of the list in the corresponding
 * slot. It is possible to insert several items with the same key into 
 * the hash table.
 *
 * \param key The key of the new item.
 * \param item The item being inserted.
 */
      void insert(const KeyType &newKey, const ItemType &newItem);

/*! \brief Performs a regular \a insert() if there is
 * no item with the same key in the hash table, otherwise the item is replaced
 * by the new item.
 *
 * \param key The key of the new item.
 * \param item The item being inserted.
 */
      void overWrite(const KeyType &newKey, const ItemType &newItem);

/*! Looks for an item in the hash table with a given key.
 *
 * \return A pointer to an item with the given key, or a 0-pointer if there
 * is no item with this key in the hash table. If there is more than
 * one item in the hash table with this key, a pointer to the first item found 
 * is returned.
 *
 * \param key The key of the searched item.
 */
      ItemType *find(const KeyType &key);

/*! \brief This version of the function \a find()
 * checks if a prespecified item
 * with a prespecified key is contained in the hash table.
 *
 * \return true If there is an element \a (key, item) in the hash table,
 * \return false otherwise.
 *
 * \param key The key of the item.
 * \param item The searched item.
 */
      bool find(const KeyType &key, const ItemType &item);

/*! @name
 * The functions \a initializeIteration() and \a next()
 * can be used to iterate through all items stored in the hash table having the same key.
 */
//@{
/*! The function \a initializeIteration() retrieves the first item.
 * 
 * \return A pointer to the first item found in the hash table having key \a key,
 * or 0 if there is no such item.
 *
 * \param key The key of the items through which we want to iterate.
 */
      ItemType *initializeIteration(const KeyType &key);

/*! \brief The function \a next() 
 * can be used to go to the next item in the hash table with key \a key. 
 *
 * Before the first call of \a next() for a certain
 * can the iteration has to be initialized by calling \a initializeItaration().
 *
 * \note The function \a next() gives you the next item having \a key key but
 * not the next item in the linked list starting in a slot of the hash table.
 *
 * \return A pointer to the next item having key \a key, or 0 if there is no more
 * item with this key in the hash table.
 *
 * \param key The key of the items through which we want to iterate.
 */
      ItemType *next(const KeyType &key);
//@}

/*! Removes the first item with a given key from the hash table.
 *
 * \return 0 If an item with the key is found.
 * \return 1 If there is no item with this key.
 *
 * \param key The key of the item that should be removed.
 */
      int remove(const KeyType &key);

/*! \brief This version of the function \a remove()
 *  removes the first item with
 * a given key and a prespecified element from the hash table.
 *
 * \return 0 If an item with the key is found.
 * \return 1 If there is no item with this key.
 *
 * \param key The key of the item that should be removed.
 * \param item The item which is searched.
 */
      int remove(const KeyType &key, const ItemType &item);

/*! \return The length of the hash table.
 */
      int size() const;

/*! \return The number of collisions which occurred
 * during all previous calls of the functions \a insert()
 * and \a overWrite().
 */
      int nCollisions() const;

/*! Can be used to change the size of the hash table.
 *
 * \param newSize The new size of the hash table (must be positive).
 */
      void resize(int newSize);

    private: 

/*! Computes the hash value of \a key.
 *
 * It must be overloaded for all key types, which are used together
 * with this template.
 *
 * This following version of \a hf() implements a Fibonacci hash function
 * for keys of type \a int.
 */
      int hf(int key);

/*! \brief This version of \a hf()
 * implements a Fibonacci hash function
 * for keys of type \a unsigned.
 */
      int hf(unsigned key);

/*! This is a hash function for character strings.
 *
 * It is taken from \ref{Knu93a}, page 300.
 */
      int hf(const ABA_STRING &string);

      /*! A pointer to the corresponding global object.
       */
      ABA_GLOBAL *glob_;

      /*! The hash table storing a linked list of hash items in each slot.
       *
       *  \a table_[i] is initialized with a 0-pointer in order to indicate that
       *  it is empty. The linked lists of each slot are terminated with
       *  a 0-pointer, too.
       */
      ABA_HASHITEM<KeyType, ItemType> **table_;

      /*! The length of the hash table.
       */
      int size_;

      /*! The number of collisions on calls of \a insert() and \a overWrite().
       */
      int nCollisions_;

      /*! An iterator for all items stored in a slot.
       *
       * This variable is
       *  initialized by calling \a initializeIteration() and incremented
       *  by the function \a next().
       */
      ABA_HASHITEM<KeyType, ItemType> *iter_;
      ABA_HASH(const ABA_HASH &rhs);
      ABA_HASH &operator=(const ABA_HASH &rhs);
  };




#include "abacus/hash.inc"

#endif  // ABA_HASH_H


/*! \class ABA_HASH
 *  \brief data structure stores a set of items and provides as central
 * functions the insertion of a new item, the search for an item, and
 * the deletion of an item.
 */

/*! \class ABA_HASHITEM
 *  \brief see also class ABA_HASH
 */
