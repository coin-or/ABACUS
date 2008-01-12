/*!\file
 * \author Matthias Elf
 * \brief ring.
 *
 * The template ABA_RING implements a bounded circular list with
 * the property that if the list is full and an element is inserted
 * the oldest element of the ring is removed. With this implementation
 * single elements cannot be removed, but the whole ABA_RING can be cleared.
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


#ifndef ABA_RING_H
#define ABA_RING_H

#include "abacus/array.h"

#ifdef ABACUS_NEW_TEMPLATE_SYNTAX
template <class Type>
class  ABA_RING;

template <class Type>
ostream &operator<< (ostream &out, const ABA_RING<Type> &ring);
#endif

  template <class Type> class  ABA_RING :  public ABA_ABACUSROOT  {
    public:

/*! The constructor.
 *
 *    \param glob A pointer to the corresponding global object.
 *    \param size The length of the ring.
 */
      ABA_RING(ABA_GLOBAL *glob, int size);

//! The destructor.
      virtual ~ABA_RING();
#ifdef ABACUS_NEW_TEMPLATE_SYNTAX

/*! \brief The output operator
 * writes the elements of the ring to an output
 * stream starting with the oldest element in the ring.
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param rhs The ring being output.
 */
      friend ostream &operator<< <> (ostream &, const ABA_RING<Type> &);
#else

/*!\brief The output operator
 * writes the elements of the ring to an output
 * stream starting with the oldest element in the ring.
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param rhs The ring being output.
 */
      friend ostream &operator<< (ostream &out, const ABA_RING<Type> &ring);
#endif

/*! \return The \a i-th element of the ring. The operation is undefined if no
 * element has been inserted in the \a i-th position so far.
 *
 * \param i The element being accessed.
 */
      Type& operator[](int i);

/*! \brief The operator \a []
 * is overloaded for constant use.
 */
      const Type& operator[](int i) const;

/*! Inserts a new element into the ring.
 *
 * If the ring is already full, this operation overwrites the oldest
 * element in the ring.
 *
 * \param elem The element being inserted.
 */
      void insert(Type elem);

/*! Empties the ring.
 */
      void clear();

/*! \return The size of the ring.
 */      
      int  size() const; 

/*! \return The current number of elements in the ring.
 */
      int  number() const;

/*! \return The oldest element in the ring.
 * \return The result is undefined, if the ring is empty.
 */
      Type oldest() const;

/*! \return The index of the oldest element in the ring.
 * \return The result is undefined, if the ring is empty.
 */
      int  oldestIndex() const;

/*! \return The newest element in the ring.
 * \return The result is undefined if the ring is empty.
 */
      Type newest() const;

/*! \return The index of the newest element in the ring.
 * \return The result is undefined if the ring is empty.
 */
      int  newestIndex() const;

/*! Can be used to access any element between the oldest and newest inserted element.
 *
 * \return 0 If there are enough elements in the ring such that
 *              the element \a i entries before the newest one could be
 *              accessed,
 * \return 1 otherwise.
 *
 * \param i The element \a i elements before the newest element is
 *              retrieved. If \a i is 0, then the function retrieves the
 *              newest element.
 * \param p Contains the \a i-th element before the newest one in
 *              a successful call.
 */
      int  previous(int i, Type &p) const;

/*! \return true If no element is contained in the ring,
 * \return false otherwise.
 */
      bool empty() const;

/*! \return true If the ABA_RING is completely filled up,
 * \return false otherwise.
 */
      bool filled() const;

/*! Changes the length of the ring.
 *
 * \param newSize The new length of the ring.
 * If the ring decreases below the current number of elements in the
 * ring, then the \a newSize newest elements stay in the ring.
 */
      void realloc(int newSize);

    private:

      /*! A pointer to the corresponding global object.
       */
      ABA_GLOBAL  *glob_;

      /*! {An array storing the elements of the ring.
       */
      ABA_ARRAY<Type>  ring_;

      /*! The position in the array \a ring_ where the next element will be inserted.
       */
      int          head_;

      /*! This member becomes \a true if ring is completely filled up.
       */
      bool         filled_;
  };

#include "abacus/ring.inc"

#endif  // ABA_RING_H

/*! \class ABA_RING
 *  \brief template ABA_RING implements a bounded circular list with
 * the property that if the list is full and an element is inserted
 * the oldest element of the ring is removed
 */
