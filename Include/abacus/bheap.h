/*!\file
 * \author Matthias Elf
 * 
 * A heap is the representation of a binary tree by an array \a a.
 *
 * \par
 * The root of the tree is associated with \a a[0]. If a node
 * corresponds to \a a[i], then its left son corresponds 
 * to \a a[2*i+1] and its right son to \a a[2*i+2]. This implicit 
 * binary tree is completely filled except possibly its highest 
 * level. Every item in the heap has to fulfil the heap 
 * property, i.e., its key has to be less than or equal than the 
 * keys of both sons. 
 *
 * \par
 * This template class implements a heap with a fixed maximal size, 
 * however a reallocation can be performed if required. 
 *
 * \par
 * The operations \a insert(), 
 * \a extractMin() require \f$\hbox{\rm O}(\log n)\f$ running time if 
 * \f$n\f$ elements are 
 * contained in the heap. The operation \a getMin() can even be 
 * executed in constant time. 
 * A heap can also be constructed from an \a ABA_BUFFER of \f$n\f$ elements 
 * which requires a running time of \f$\hbox{\rm O}(n)\f$ only. 
 *
 * \par
 * The order of the elements in the heap is given by keys 
 * which are inserted together with each element of the heap. 
 * The class \a Key must be from an ordered type. 
 * Given two objects \a k1 and \a k2 of type \a Key then 
 * \a k1 has higher priority if the expression \a k1 < k2 holds. 
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


#ifndef ABA_BHEAP_H
#define ABA_BHEAP_H

#include "abacus/array.h"

template<class Type, class Key> 
class ABA_BHEAP;

template<class Type,class Key>
ostream&operator<< (ostream& out, const ABA_BHEAP<Type, Key>& rhs);

  template<class Type, class Key> class  ABA_BHEAP :  public ABA_ABACUSROOT  {
    public: 

/*! A constructor.
 *
 *    \param glob A pointer to the corresponding global object.
 *    \param size The maximal number of elements which can be stored.
 */
      ABA_BHEAP(ABA_GLOBAL *glob, int size);

/*! A constructor with initialization.
 *
 * The heap is initialized from
 * an ABA_BUFFER of elements and a corresponding ABA_BUFFER of keys.
 * The running time is \f$\hbox{\rm O}(n)\f$ for \f$n\f$ elements.
 *
 *    \param glob A pointer to the corresponding global object.
 *    \param elem A ABA_BUFFER wich contains the elements.
 *    \param elem A ABA_BUFFER wich contains the keys.
 */
      ABA_BHEAP(ABA_GLOBAL *glob, 
                const ABA_BUFFER<Type> &elems, 
                const ABA_BUFFER<Key> &keys);

/*! The output operator
 * writes the elements of the heap together with their keys on an output stream.
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param rhs The heap being output.
 */
      friend ostream& operator<< <> (ostream&, const ABA_BHEAP<Type, Key>&);

/*! Inserts an item with a key  into the heap.
 *
 * It is a fatal error to perform this operation if the
 * heap is full.
 * If the
 * precompiler flag \a -DABACUSSAFE is set, we check if the
 * heap is not already full.
 *
 * \param elem The element being inserted into the heap.
 * \param key The key of this element.
 */
      void insert(Type elem, Key key);

/*! \return The minimum element of the heap. This operation must not be performed
 * if the heap is empty.
 * Since the heap property holds the element having minimal key is
 * always stored in \a heap_[0].
 */
      Type getMin() const;

/*! \return The key of the minimum element of the heap. 
 * This operation must not be performed
 * if the heap is empty.
 * Since the heap property holds the element having minimal key is
 * always stored in \a heap_[0] and its key in \a key_[0].
 */
      Key getMinKey() const;

/*! Accesses and removes the minimum element from the heap.
 *
 * The minimum element is stored in the root of the tree, i.e.,
 * in \a heap_[0]. If the heap does not become empty by removing the
 * minimal element, we move the last element stored in \a heap_
 * to the root (\a heap_[0]). Whereas this operation can destroy
 * the heap property, the two subtrees rooted at nodes 1 and 2
 * still satisfy the heap property. Hence calling \a heapify(0)
 * will restore the overall heap property.
 *
 * \return The minimum element of the heap.
 */
      Type extractMin();


    /*! Empties the heap.
     */
      void clear();

/*! \return The maximal number of elements which can be stored in the heap.
 */
      int size() const;

/*! \return The number of elements in the heap.
 */
      int number() const;

/*! \return true If there are no elements in the heap,
 * \return false otherwise.
 */
      bool empty() const;

/*! Changes the size of the heap.
 *
 * \param newSize The new maximal number of elements in the heap.
 */
      void realloc(int newSize);

/*! Stops with an error message if the heap properties are violated. 
 *
 * This function is used for debugging
 * this class.
 */
      void check() const;

    private: 

/*! Returns the index of the left son of node \a i.
 */
      int  leftSon(int i) const;

/*! Returns the index of the right son of node \a i.
 */
      int  rightSon(int i) const;

/*! Returns the  index of the father of element \a i.
 */
      int  father(int i) const;

/*! Is the central function to maintain the heap property. 
 * 
 * The function assumes that the two trees rooted at \a left(i) and
 * \a right(i) fulfil already the heap property and restores the
 * heap property of the (sub-) tree rooted at \a i.
 */
      void heapify(int i);

      ABA_GLOBAL *glob_;
      ABA_ARRAY<Type>  heap_;
      ABA_ARRAY<Key>   keys_;
      int          n_;
  };

#include "abacus/bheap.inc" 

#endif  // !ABA_BHEAP_H


/*! \class ABA_BHEAP
 *  \brief This template class implements a heap with a fixed maximal size, 
 * however a reallocation can be performed if required.
 */



