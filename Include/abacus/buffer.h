/*!\file
 * \author Matthias Elf
 * \brief buffer.
 *
 * Often we  need a data structure for buffering information. This class
 * implements such a buffer by an array and storing the number of already
 * buffered elements. If the initial size of the buffer turns out to be
 * too small, then the buffer can be reallocated.
 *
 * \par
 * In fact, this class could be derived from the class ABA_ARRAY. However,
 * due to a bug in that release of the GNU compiler which we used
 * when we developed this class we could not derive this class from
 * the class ABA_ARRAY.
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

#ifndef ABA_BUFFER_H
#define ABA_BUFFER_H
#include <stdlib.h>
#include <iostream>
using namespace std;
#include "abacus/exceptions.h"
#include "abacus/global.h"


template<class Type>
class  ABA_BUFFER;
 
template<class Type> 
ostream& operator<< (ostream& out, const ABA_BUFFER<Type> &buffer);

  template <class Type> class  ABA_BUFFER :  public ABA_ABACUSROOT  {
    public:

/*! The constructor generates an empty buffer.
 *
 * \param glob The corresponding global object.
 * \param size The size of the buffer.
 */
      ABA_BUFFER(ABA_GLOBAL *glob, int size);

/*! The copy constructor.
 *
 * \param rhs The buffer being copied.
 */
      ABA_BUFFER(const ABA_BUFFER<Type> &rhs);

//! The destructor.
      ~ABA_BUFFER();

/*! The assignment operator is only allowed between buffers having equal size.
 *
 * \return A reference to the buffer on the left hand side of the assignment operator.
 *
 * \param rhs The buffer being assigned.
 */
      const ABA_BUFFER<Type>& operator=(const ABA_BUFFER<Type>& rhs);

/*! \brief The output operator
 * writes all buffered elements line by line
 * to an output stream in the format {\it number\/}{\tt : \a  }{\it value\/}.
 *
 * \return A reference to the stream the buffer is written to.
 *
 * \param out The output stream.
 * \param buffer The buffer being output.
 */
      friend ostream& operator<< <> (ostream& , const ABA_BUFFER<Type> &);

/*! The operator \a [] can be used to access an element of the buffer. 
 *
 * It is only allowed to access buffered elements.
 * Otherwise, if the function is compiled with \a -DABACUSSAFE
 * the program stops with an error message.
 *
 * \return The \a i-th element of the buffer.
 *
 * \param i The number of the component which should be returned.
 */
      Type& operator[](int i);

/*! The operator \a [] is overloaded that it can be also used to access elements of constant buffers.
 */
      const Type& operator[](int i) const;

/*! \return The maximal number of elements which can be stored in the buffer.
 */
      int size() const;

/*! \return The number of buffered elements.
 */
      int number() const;

/*! \return true If no more elements can be inserted into the buffer,
 * \return false otherwise.
 */
      bool full() const;

/*! \return true If no items are buffered,
 * \return false otherwise.
 */
      bool empty() const;

/*! Inserts an item into the buffer.
 *
 * It is a fatal error to perform this operation if the buffer is full.
 *
 * In this case the program stops with an
 * error message if this function is compiled with \a -DABACUSSAFE.
 *
 * \param item The item that should be inserted into the buffer.
 */
      void push(Type item);

/*! Removes and returns the last inserted item from the buffer.
 *
 * It is a fatal error to perform this operation
 * on an empty buffer. 
 *
 * In this case the program stops with an
 * error message if this function is compiled with \a -DABACUSSAFE.
 *
 * \return The last item that has been inserted into the buffer.
 */ 
      Type pop();

/*! Sets the number of buffered items to 0 such that the buffer is empty.
 */
      void clear();

/*! \brief Removes the components listed
 * in the buffer \a ind by shifting the remaining components to the
 * left. 
 *
 * The values stored in \a ind have to be upward sorted.
 * Memory management of the removed components must be
 * carefully implemented by the user of this function to avoid
 * memory leaks. 
 *
 * If this function is compiled with \a -DABACUSSAFE
 * then it is checked if each value of \a ind is in the
 * range 0,\dots, \a number()-1.
 *
 * \param ind The numbers of the components being removed.
 */
      void leftShift(ABA_BUFFER<int> &ind);

/*! \brief The length of a buffer can be changed with the function \a realloc().
 * If the size of the buffer is increased all buffered elements are
 * copied. If the size is decreased the number of buffered elements
 * is updated if necessary.
 *
 * \param newSize The new length of the buffer.
 */
      void realloc (int newSize);

    private:

      /*! A pointer to the corresponding global object.
       */
      ABA_GLOBAL *glob_;

      /*! The maximal number of elements which can be stored in the buffer.
       */
      int         size_;

      /*! The number of buffered elements.
       */
      int         n_;

      /*! The \CEE/-style array storing the buffered elements.
       */
      Type       *buf_;
  };

#include "abacus/buffer.inc"

#endif  // ABA_BUFFER_H


/*! \class ABA_BUFFER
 * \brief class implements a buffer by an array and storing the number of already
 * buffered elements.
 */


