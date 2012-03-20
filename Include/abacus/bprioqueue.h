/*!\file
 * \author Matthias Elf
 *
 * \brief bounded priority queue.
 *
 * A priority queue is a data structure storing a set of elements.  
 * Each element has a key which must be an ordered data type.  
 * The most important operations are the insertion  
 * of an element, the determination of the element having the minimal  
 * key, and the deletion of the element having minimal key.  
 *
 * \par
 * Since the priority queue is implemented by a heap (class ABA_BHEAP)  
 * the insertion of a new element and the deletion of the minimal  
 * element require \f$\hbox{\rm O}(\log n)\f$ time if \f$n\f$ elements are  
 * stored in the priority queue. The element having minimal key  
 * can be determined in constant time.  
 *
 * \par
 * To provide an efficient implementation the priority queue is  
 * bounded, i.e., the maximal number of elements is an argument  
 * of the constructor. However, if required, later a reallocation  
 * can be performed.  
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


#ifndef ABA_BPRIOQUEUE_H
#define ABA_BPRIOQUEUE_H
#include "abacus/exceptions.h"
#include "abacus/bheap.h"

  template<class Type, class Key>
  class  ABA_BPRIOQUEUE :  public ABA_ABACUSROOT  {
    public:

/*! The constructor of an empty priority queue.
 *
 *    \param glob A pointer to the corresponding object.
 *    \param size The maximal number of elements the priority queue
 *                    can hold without reallocation.
 */
      ABA_BPRIOQUEUE(ABA_GLOBAL *glob, int size);

/*! Inserts an element in the priority queue.
 *
 * \param elem The element being inserted.
 * \param key The key of the element.
 */
      void insert(Type elem, Key key);

/*! Retrieves the element with minimal key from the priority queue.
 *
 * \return 0 If the priority queue is non-empty,
 * \return 1 otherwise.
 *
 * \param min If the priority queue is non-empty the minimal element is
 *              assigned to \a min.
 */
      int getMin(Type &min) const;

/*! Retrieves the key of the minimal element in the priority queue.
 *
 * \return 0 If the priority queue is non-empty,
 * \return 1 otherwise.
 *
 * \param minKey Holds after the call the key of the minimal element in
 *                   the priority queue, if the queue is non-emtpy.
 */
      int getMinKey(Key &minKey) const;

/*! \brief Extends the function \a getMin(min) in
 * the way that the minimal element is also removed from the priority queue.
 *
 * \return 0 If the priority queue is non-empty,
 * \return 1 otherwise.
 *
 * \param min If the priority queue is non-empty the minimal element is
 *                assigned to \a min.
 */
      int extractMin(Type &min);

/*! Makes the priority queue empty.
 */
      void clear();

/*! \return The maximal number of elements which can be stored in the priority queue.
 */
      int size() const;

/*! \return The number of elements stored in the priority queue.
 */
      int number() const;

/*! Increases the size of the priority queue. 
 *
 * It is not allowed to decrease the size of the priority queue.
 * In this case an error message is output and the program stops.
 *
 * \param newSize The new size of the priority queue.
 */
      void realloc(int newSize);

    private:

      /*! A pointer to the corresponding global object.
       */
      ABA_GLOBAL      *glob_;

      /*! The heap implementing the priority queue.
       */
      ABA_BHEAP<Type, Key>  heap_;
  };

#include "abacus/bprioqueue.inc"

#endif  // !ABA_BPRIOQUEUE_H

/*! \class ABA_BPRIOQUEUE
 * \brief Since the priority queue is implemented by a heap (class 
ABA_BHEAP)  
 * the insertion of a new element and the deletion of the minimal  
 * element require \f$\hbox{\rm O}(\log n)\f$ time if \f$n\f$ elements 
are  
 * stored in the priority queue.
 */

