/*!\file
 * \author Matthias Elf
 * \brief dictionary.
 *
 * The data structure dictionary is a collection of items with keys.
 * It provides the operations to insert pairs of keys and items and
 * to look up an item given some key.
 *
 * \par
 * We implement the dictionary by a hash table (class ABA_HASH).
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

#ifndef ABA_DICTIONARY_H
#define ABA_DICTIONARY_H
#include <iostream>
using namespace std;

#include "abacus/global.h"
#include "abacus/hash.h"


  template <class KeyType, class ItemType>
  class  ABA_DICTIONARY :  public ABA_ABACUSROOT  { 
    public: 

/*! The constructor.
 *
 *    \param glob A pointer to the corresponding global object.
 *    \param size The size of the hash table implementing the dictionary.
 */
      ABA_DICTIONARYABA_GLOBAL *glob, int size);

/*! \brief The output operator
 *  writes the hash table implementing the dictionary on an output stream.
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param rhs The hash table being output.
 */
      friend ostream &operator<<(ostream &out, 
                                 const ABA_DICTIONARY<KeyType, ItemType> &rhs);

/*! Adds the item together with a key to the dictionary.
 *
 * \param key The key of the new item.
 * \param item The new item.
 */
      void insert(const KeyType &key, const ItemType &item);

/*! \return A pointer to the item associated
 * with \a key in the ABA_DICTIONARY, or 0 if there is no such item.
 *
 * \param key The key of the searched item.
 */
      ItemType *lookUp(const KeyType &key);

    private:

/*! A pointer to the corresponding global object.
 */
      ABA_GLOBAL            *glob_;

/*! The hash table implementing the dictionary.
 */
      ABA_HASH<KeyType, ItemType> hash_;
      ABA_DICTIONARY(const ABA_DICTIONARY<KeyType, ItemType> &rhs);
      const ABA_DICTIONARY &operator=(const ABA_DICTIONARY<KeyType, ItemType> &rhs);
  };

#include "abacus/dictionary.inc"

#endif  // ABA_DICTIONARY_H


/*! \class ABA_DICTIONARY
 * \brief data structure dictionary is a collection of items with keys.
 * It provides the operations to insert pairs of keys and items and
 * to look up an item given some key.
 */
