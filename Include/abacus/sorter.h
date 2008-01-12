/*!\file
 * \author Matthias Elf
 * \brief sorter.
 *
 * This class implements several functions for sorting arrays
 * according to increasing keys. We encapsulate these functions in
 * order to avoid name conflicts. Moreover, instead of local variables
 * in the sorting functions we can provide within the class
 * variables for swapping in order to speed up the sorting.
 *
 * \par
 * The sorting functions do not keep the elements of the array storing the
 * keys in place but resort it in parallel with the array storing the
 * items.
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

#ifndef ABA_SORTER_H
#define ABA_SORTER_H

#include "abacus/array.h"
  template <class ItemType, class KeyType>
  class  ABA_SORTER :  public ABA_ABACUSROOT  {
    public:

/*! The constructor.
 *
 *    \param glob A pointer to the corresponding global object.
 */
      ABA_SORTER (ABA_GLOBAL *glob);

/*! \brief Sorts the elements of an array of \a n items
 * according to their keys.
 *
 * This function is very efficient for
 * many practical applications. Yet, has a worst case running time
 * of \f$\hbox{\rm O}(n^2)\f$.
 *
 * \param n The number of elements being sorted.
 * \param items The items being sorted.
 * \param keys The keys of the sorted items.
 */
      void quickSort(int n, ABA_ARRAY<ItemType> &items, ABA_ARRAY<KeyType> &keys);

/*! Sorts an partial array.
 *
 * The function \a quickSort() uses the divide-and-conquer technique.
 * First the function \a partition() puts the small elements to the left
 * part and all big elements to the right part of the array being sorted.
 * More precisely, it holds then,
 * \a keys[i] <= keys[q] for all \a i in \a left, \dots \a q
 * and 
 * \a keys[q] < keys[i] for all \a i in \a q+1, \dots \a right.
 * Hence, it is sufficient to sort these two subarrays recursively.
 *
 * \param items The items being sorted.
 * \param keys The keys of the items.
 * \param left The first item in the partial array being sorted.
 * \param right The last item in the partial array being sorted.
 */
      void quickSort(ABA_ARRAY<ItemType> &items, ABA_ARRAY<KeyType> &keys,
                     int left, int right);

/*! Sorts an array of \a n items according to their keys.
 *
 * In many practical applications this
 * function is inferior to \a quickSort(), although it has the optimal
 * worst case running time of \f$\hbox{\rm O}(n\log n)\f$.
 *
 * \par
 * The function \a heapSort() generates a heap. This guarantees that the
 * largest element is stored in \a items[0]. So it is obvious
 * that if we want to sort the items by increasing keys, this element
 * will finally be stored in \a items[n-1]. Hence we swap the \a items
 * and \a keys of 0 and \a n-1 and restore the heap property for the
 * elements 0,\dots, \a n-2. This can be done by \a heapify() since
 * the subtree rooted at 1 and 2 are still heaps (the last element
 * is not considered anymore). This process is iterated until the
 * elements are sorted.
 *
 * \param n The number of items being sorted.
 * \param items The items being sorted.
 * \param keys The keys of the items.
 */
      void heapSort(int n, ABA_ARRAY<ItemType> &items, ABA_ARRAY<KeyType> &keys);

    private:

/*! \brief Returns a number \a q (\a{{left <= q <= right)}
 * and guarantees that all elements \a i with \a{key[i] <= key[q]}}
 * are stored in the left part of the array, i.e., in \a items[left],\dots ,
 * \a items[q], and all elements \a j with \a key[j] > key[q] are stored
 * in the right part of the array, i.e., in \a items[q+1], \f$\dots\f$, \a items[right].
 *
 *    \param items The items being sorted.
 *    \param keys The keys for sorting the items.
 *    \param left The left border of the partial array being considered.
 *    \param right The right border ot the partial array being considered.
 *
 * \par
 * First, we determine a pivot element \a k. The \a while
 * loop stops by returning \a r as soon as the elements are
 * partitioned in two subsets such all elements \a i in
 * \a left <= i <= r have a smaller key than the elements i with
 * \a r+1 <= right.
 *
 * \par
 * The \a do-loops  stop as soon as a pair of elements is
 * found violating the partition property. This pair of elements is
 * the swapped together with their keys.
 */
      int partition(ABA_ARRAY<ItemType> &items, ABA_ARRAY<KeyType> &keys,
                    int left, int right);
                    

/*! \brief Resorts the elements if \a items and \a keys
 * such that the heap property holds, i.e.,
 * \a keys[i] >= keys[2*i+1] and \a keys[i] >= keys[2*i+2].
 *
 *    \param n The number of elements of the following arrays.
 *    \param items The items being sorted.
 *    \param keys The keys for sorting the items.
 *
 * \par
 *  The function \a heapify() is called for each node of the tree which
 *  is not necessarily a leaf. First nodes on higher level in the tree
 *  processed.
 */
      void buildHeap(int n, ABA_ARRAY<ItemType> &items, ABA_ARRAY<KeyType> &keys);

/*! \brief Assumes that the heap property holds for the
 * subtrees rooted at the sons of \a root and restores the heap property
 * for the subtree rooted at \a root.
 *
 *    \param n The number of elements of the following arrays.
 *    \param items The items being sorted.
 *    \param keys The keys for sorting the items.
 *    \param root The index where the heaps property has to be restored.
 *
 * \par
 * The function \a heapify() checks if the heap property holds for \a root.
 * This is not the case if the \a largest element of \a l, \a r, and \a root
 * is not \a root. In this case the elements of \a root and \a largest
 * are swapped and we iterate. Otherwise, the heap property is restored.
 */
      void heapify(int n, ABA_ARRAY<ItemType> &items, ABA_ARRAY<KeyType> &keys, int root);

/*! \brief Is a debugging function and terminates
 *  the program with an error message if the elements of \a items
 *  are not sorted by increasing keys.
 *
 *    \param n The number of elements of the following arrays.
 *    \param items The items being sorted.
 *    \param keys The keys for sorting the items.
 */
      void check(int n, ABA_ARRAY<ItemType> &items, ABA_ARRAY<KeyType> &keys);

      /*! A pointer to the corresponding global object.
       */
      ABA_GLOBAL *glob_;

      /*! An auxiliary variable for swapping items.
       */    
      ItemType itemSwap_;

      /*! An auxiliary variable for swapping keys.
       */
      KeyType  keySwap_;      
    };

#include "abacus/sorter.inc"

#endif // ABA_SORTER_H


/*! \class ABA_SORTER
 *  \brief This class implements several functions for sorting arrays
 * according to increasing keys
 */
