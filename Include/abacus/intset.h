/*!\file
 * \author Matthias Elf
 * \brief integer set.
 *
 * The class ABA_INTSET implements the abstract datatype for storing
 * a subset of the set \f$\{0,\dots,n-1\}\f$. All operations of this class
 * need constant time only. The storage requirement is \f$O(n)\f$.
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

#ifndef ABA_INTSET_H
#define ABA_INTSET_H
  class  ABA_INTSET  { 
    public: 

/*! The constructor. An empty set is created. 
 * 
 *   \f$O(n)\f$ bytes of memory are
 *   allocated for the internal representation of the data structure.
 *
 *   \param n The created object can store subsets of \f$\{0,\dots,n-1\}\f$.
*/
      ABA_INTSET();

/*! The destructor deletes the allocated memory.
 */
      ~ABA_INTSET();

/*! An empty set is initialized.
 * 
 *   \f$O(n)\f$ bytes of memory are
 *   allocated for the internal representation of the data structure.
 *
 *   \param n The initialized object can store subsets of \f$\{0,\dots,n-1\}\f$.
*/
      void initialize(int n);

/*! Tests if an integer is contained in the set.
 */
      bool exists(int elem) const;

/*! Inserts an element in the set.
 *
 *  The element is not inserted twice if it is already contained in the set.
 *
 *   \param elem The element to be inserted.
*/
      void insert(int elem);

/*! Removes an element from the set if it is contained in it.
 *
 *   \param elem The element to be removed.
*/
      void remove(int elem);

/*! \brief This function and the function \a elem() can be used
 *   to iterate over the elements of the set.
 *
 *   \return The number of elements contained in the set.
*/
      int  count() const;

/*! Is used to access the elements contained in the set.
 *
 *   \param index The index of the element to be returned. This must
 *                    be a non negative integer less than \a number()|.
 *
 *   \return The element referenced by the given index.
*/
      int  elem(int index) const;

    private: 
      int *uninitializedMap_;
      int *position_;
      int count_;
  };
#endif	// ABA_INTSET_H


/*! \class ABA_INTSET
 *  \brief class ABA_INTSET implements the abstract datatype for storing
 * a subset of the set \f$\{0,\dots,n-1\}\f$.
 */

