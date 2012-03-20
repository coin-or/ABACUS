/*!\file
 * \author Matthias Elf
 *
 * One of the basic classes is a template for arrays. It can be used like 
 * a ``normal'' C-style array, yet has some additional nice features, 
 * especially we do not have to care for the allocation and deallocation 
 * of memory. The first index of an array is 0 as usual in \CPLUSPLUS/. 
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

#ifndef ABA_ARRAY_H
#define ABA_ARRAY_H
#include <iostream>
using namespace std;

#include "abacus/global.h"
#include "abacus/buffer.h"
#include "abacus/exceptions.h"

template<class Type> 
class  ABA_ARRAY;
template<class Type> 
ostream& operator<< (ostream& out, const ABA_ARRAY<Type> &array);

  template <class Type> class  ABA_ARRAY : public ABA_ABACUSROOT  {
    public:

/*! A constructor without initialization.
 *
 * \param glob A pointer to the corresponding global object.
 * \param size The length of the array.
 */
      ABA_ARRAY(ABA_GLOBAL *glob, int size);

/*! A constructor with initialization.
 *
 *    \param glob A pointer to the corresponding global object.
 *    \param size The length of the array.
 *    \param init The initial value of all elements of the array.
 */
      ABA_ARRAY(ABA_GLOBAL *glob, int size, Type init);

/*! A constructor.
 *
 *    \param glob A pointer to the corresponding global object.
 *    \param buf The array receives the length of this buffer and
 *                   all buffered elements are copied to the array.
 */
      ABA_ARRAY(ABA_GLOBAL *glob, const ABA_BUFFER<Type> &buf);

/*! The copy constructor.
 *
 * \param rhs The array being copied.
 */
      ABA_ARRAY(const ABA_ARRAY<Type> &rhs);

//! The destructor.
      ~ABA_ARRAY();

/*! The assignment operator can only be used for arrays with equal length.
 *
 * \return A reference to the array on the left hand side.
 *
 * \param rhs The array being assigned.
 */
      const ABA_ARRAY<Type>& operator=(const ABA_ARRAY<Type>& rhs);

/*! \brief To assign an object of the class ABA_BUFFER to an object of the class
 * ABA_ARRAY the size of the left hand side must be at least the size
 * of \a rhs. Then all buffered elements of \a rhs are copied.
 *
 * \return A reference to the array on the left hand side.
 *
 * \param rhs The buffer being assigned.
 */
      const ABA_ARRAY<Type>& operator=(const ABA_BUFFER<Type>& rhs);

/*! \brief The output operator writes first the number of the element and a
 * \a ':' followed by the value of the element line by line to the stream
 * \a out.
 *
 *  \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param array The array being output.
 */
      friend ostream& operator<< <> (ostream&, const ABA_ARRAY<Type> &);

/*! The operator \a [].
 *
 * \return The \a i-th element of the array.
 *
 * \param i The element being accessed.
 */
      Type& operator[](int i);

/*! The operator \a [] 
 *  is overloaded for constant use.
 */
      const Type& operator[](int i) const;

/*! Copies all elements of \a rhs. 
 *
 * The difference
 * to the operator \a = is that also copying between arrays of different
 * size is allowed. If necessary the array on the left hand side is
 * reallocated.
 *
 * \param rhs The array being copied.
 */
      void copy(const ABA_ARRAY<Type>& rhs);

/*! \brief This version of the function \a copy() copies the elements
 * \a rhs[l], \a rhs[l+1],\dots, \a rhs[r] into the components
 * \a 0,\dots,r-l of the array.
 *
 * If the size of the array is smaller than \a r-l+1 storage is reallocated.
 *
 * \param rhs The array that is partially copied.
 * \param l The first element being copied.
 * \param r the last element being copied.
 */
      void copy(const ABA_ARRAY<Type>& rhs, int l, int r);

/*! \brief Removes the components listed
 * in \a ind by shifting the remaining components to the
 * left. 
 *
 * Memory management of the removed components must be
 * carefully implemented by the user of this function to avoid
 * memory leaks.
 *
 * \param ind The compenents being removed from the array.
 */
      void leftShift(ABA_BUFFER<int> &ind);

/*! \brief This version of the function \a leftShift()
 * removes all components \a i
 * with \a marked[i]==true from the array by shifting the other 
 * components to the left.
 *
 * \param remove The marked components are removed from the array.
 */
      void leftShift(ABA_ARRAY<bool> &remove);

/*! Assigns the same value to a subset of the components of the array.
 *
 * \param l The first component the value is assigned.
 * \param r The last component the value is assigned.
 * \param val The new value of these components.
 */
      void set(int l, int r, Type val);

/*! \brief This version of the function \a set()
 * initializes all components of the array with the same value.
 *
 * \param val The new value of all components.
 */
      void set(Type val);

/*! \return The length of the array.
 */
      int  size() const;

/*! \brief The length of an array can be changed with the function \a realloc().
 * If the array is enlarged all elements of the old array are copied
 * and the values of the additional new elements are undefined. If the
 * array is shortened only the first \a newSize elements are copied.
 *
 * \param newSize The new length of the array.
 */
      void realloc(int newSize);

/*! \brief Is overloaded such that also
 * an initialization with a new value of the elements of the array after
 * reallocation is possible.
 *
 * \param newSize The new length of the array.
 * \param init The new value of all components of the array.
 */
      void realloc(int newSize, Type init);

    private:

/*! \brief Stops the program with an error message
 * if the index \a i is not within the bounds of the array.
 */
      void rangeCheck(int i) const;
      /*! A pointer to the corresponding global object.
       */
      ABA_GLOBAL *glob_;

      /*! The length of the array.
       */
      int         n_;

      /*! The \CEE/-style array storing the elements of the \a ABA_ARRAY. 
       */
      Type       *a_;
  };

#include "abacus/array.inc"

#endif  // !ABA_ARRAY_H

/*! \class ABA_ARRAY
 *  \brief it is a template for arrays. It can be used like 
 *      a ``normal'' C-style array
 */


