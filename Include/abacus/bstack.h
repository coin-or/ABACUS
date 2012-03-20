/*!\file
 * \author Matthias Elf
 *
 * \brief bounded stack.
 *
 * A stack is a data structure storing a set of elements. Following
 * the last-in first-out (LIFO) principle the access to or the deletion
 * of an element is restricted to the most recently inserted
 * element.
 *
 * \par
 * In order to provide an efficient implementation this stack  
 * is ``bounded'', i.e., the number of elements which can be  
 * inserted is limited. However, a reallocation can be  
 * performed if required.  
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

#ifndef ABA_BSTACK_H
#define ABA_BSTACK_H

#include "abacus/array.h"
#include "abacus/exceptions.h"
template<class Type>
class  ABA_BSTACK;

template<class Type> 
ostream& operator<< (ostream& out, const ABA_BSTACK<Type> &rhs);


  template <class Type> class  ABA_BSTACK :  public ABA_ABACUSROOT  {
    public:

/*! The constructor initializes an empty stack.
 *
 *     \param glob A pointer to the corresponding global object.
 *     \param size The maximal number of elements the stack can store.
 */
      ABA_BSTACK(ABA_GLOBAL *glob, int size);

/*! \brief The output operator writes the numbers of all stacked elements
 * and the elements line by line on an output stream.
 * 
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param rhs The stack being output.
 */
      friend ostream& operator<< <> (ostream&, const ABA_BSTACK<Type> &);

/*! \return The maximal number of elements which can be inserted into the stack.
 */
      int size() const;

/*! \return The top of the stack, i.e., the number
 * of the next free component of the stack. 
 * This is also the number of elements currently contained in the stack
 * since the first element is inserted in position 0.
 */
      int tos() const;

/*! \return true If there is no element in the stack,
 * \return false otherwise.
 */
      bool empty() const;

/*! \return true If the maximal number of elements
 *                has been inserted in the stack,
 * \return false otherwise.
 */
      bool full() const;

/*! Adds an element to the stack.
 *
 * It is a fatal error to insert an element if the stack is full.
 * If this function is compiled with \a -DABACUSSAFE, then the program
 * terminates if this error occurs.
 *
 * \param item The element added to the stack.
 */
      void push(Type item);

/*! Accesses the last element pushed on the stack without removing it.
 *
 * It is an error to perform this operation on an empty stack.
 * If this function is compiled with \a -DABACUSSAFE, then the program
 * terminates if this error occurs.
 *
 * \return The last element pushed on the stack.
 */
      Type top() const;

/*! \brief Accesses like \a top() the last element pushed on the
 * stack and removes in addition this item from the stack.
 *
 * It is an error to
 * perform this operation on an empty stack.
 * If this function is compiled with \a -DABACUSSAFE, then the program
 * terminates if this error occurs.
 *
 * \return The last element pushed on the stack.
 */
      Type pop();

/*! Changes the maximal number of elements of the stack.
 *
 * \param newSize The new maximal number of elements on the stack.
 *                    If \a newSize is less than the current number of 
 *                    elements in the stack, then the \a newSize oldest
 *                    element are contained in the stack after the
 *                    reallocation.
 */
      void realloc (int newSize);

    private:

      /*! A pointer to the corresponding global object.
       */
      ABA_GLOBAL *glob_;

      /*! The array storing the elements of the stack.
       */
      ABA_ARRAY<Type>  stack_;

      /*! The top of stack (next free component).
       */
      int          tos_;
    };

#include "abacus/bstack.inc"

#endif // ABA_BSTACK_H

/*! \class ABA_BSTACK
 *  \brief a set of elements, following the last-in first-out (LIFO) 
principle the access to or the deletion
 * of an element is restricted to the most recently inserted
 * element.
 */


