/*!\file
 * \author Matthias Elf
 *
 * \brief linked list.
 *
 * The following sections implement a template for a linked linear
 * list. Two classes are required for the representation of this data
 * structure. The first one ABA_LISTITEM forms the basic building
 * block of the list storing an element and a pointer to the
 * next item of the list, the second one is the ABA_LIST itself.
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
   

#ifndef ABA_LIST_H
#define ABA_LIST_H
#include <iostream>
using namespace std;

#include "abacus/abacusroot.h"
#include "abacus/global.h"
#include "abacus/listitem.h"

#ifdef ABACUS_NEW_TEMPLATE_SYNTAX
template <class Type>
class ABA_LIST;

template <class Type>
ostream &operator<< (ostream&, const ABA_LIST<Type> &list);
#endif

//! class ABA_LIST
  template<class Type> class ABA_LIST : public ABA_ABACUSROOT {
    friend class ABA_LISTITEM<Type>;
    public:

/*! The constructor initializes the list with the empty list.
 *
 * This is done by
 * assigning \a first_ and \a last_ to the \a 0-pointer.
 *
 *     \param glob A pointer to the corresponding global object.
 */
      ABA_LIST(const ABA_GLOBAL *glob);

/*! The destructor deallocates the memory of all items in the list.
 */
      ~ABA_LIST();
#ifdef ABACUS_NEW_TEMPLATE_SYNTAX

/*! The output operator writes all items of the list on an output stream.
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param list The list being output.
 */
      friend ostream& operator<< <> (ostream&, const ABA_LIST<Type> &);
#else

/*! The output operator writes all items of the list on an output stream.
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param list The list being output.
 */
      friend ostream& operator<< (ostream&, const ABA_LIST<Type> &list);
#endif

/*! Adds an element at the front of the list.
 *
 * \param elem The element being appended.
 */
      void appendHead(const Type &elem);

/*! Adds an element at the end of the list.
 *
 * \param elem The element being appended.
 */
      void appendTail(const Type &elem);

/*! Assigns to \a elem the first element in the list and removes it from the list.
 *
 * \return 0 If the operation can be be executed successfully.
 * \return 1 If the list is empty.
 *
 * \param elem If the list is nonemty, the first element is assigned
 *      to \a elem.
 */
      int extractHead(Type &elem);

/*! \brief Assign \a elem the first element as the
 * function \a extractHead() but does not remove this element from the list.
 *
 * \return 0 If the operation can be be executed successfully.
 * \return 1 If the list is empty.
 *
 * \param elem If the list is nonemty, the first element is assigned
 *                 to \a elem.
 */
      int firstElem(Type& elem) const;

/*! \return true If no element is contained in the list,
 * \return false otherwise.
 */
      bool empty() const;
      
   private:

/*! Returns a pointer to the first item in the list.
 */
      ABA_LISTITEM<Type>* first() const;

/*! Returns a pointer to the last item in the list.
 */
      ABA_LISTITEM<Type>* last() const;

/*! \brief This version of the function \a appendHead()
 *  adds \a item at the front of the list.
 */
     void appendHead(ABA_LISTITEM<Type> *item);

/*! \brief This version of the function \a appendTail()
 *  adds \a item at the end of the list.
 */
     void appendTail(ABA_LISTITEM<Type> *item);

     /*! A pointer to the corresponding global object.
      */
     const ABA_GLOBAL   *glob_;

     /*! A pointer to the first item of the list.
      */
     ABA_LISTITEM<Type> *first_;

     /* A pointer to last item of the list.
      */
     ABA_LISTITEM<Type> *last_;
     ABA_LIST(const ABA_LIST &rhs);
     const ABA_LIST<Type>& operator=(const ABA_LIST<Type>& rhs);
  };

#include "abacus/list.inc"

//! ABA_LIST: iterators
/*! The iterator \a forAllListElem assigns to \a Type e all elements in the
 * list beginning with the first element. 
 * Deletions of elements in the list during the application of this
 * iterator can cause an error.
 * The operator \a ?: is used that \a item is only dereferenced if
 * \a item != 0. The expression 0 on the right side of \a :
 * is only a dummy that this expression is compilable.
 *
 *
 * \param L The list that should be iterated (ABA_LIST<Type>).
 * \param item An auxilliary pointer to a list item 
 *                 (ABA_LISTITEM<Type> *).
 * \param e The elements in the list are assigned to this
 *              variable (\a Type).
 
 */
#define forAllListElem(L, item, e) \
  for((item = (L).first()) ? (e = (item)->elem()) : 0; item !=0; \
    (item = (item)->succ()) ? (e = (item)->elem()) : 0)

#endif  // ABA_LIST_H

/*! \class ABA_LIST
 *  \brief sections implement a template for a linked linear
 * list. Two classes are required for the representation: ABA_LISTITEM and ABA_LIST.
 */

