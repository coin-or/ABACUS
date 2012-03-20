/*!\file
 * \author Matthias Elf
 * \brief doubly linked list.
 *
 * The following sections implement a template for a doubly linked
 * linear list. The implementation is very similar to the class
 * ABA_LIST.
 * However, we do not derive these classes from the respective classes of
 * ABA_LIST since this causes type conflicts.
 * (e.g. \a {item->pred_->succ_ = dListItem} would require explicit cast).
 * 
 * \par
 * Two classes are required for the representation of this data
 * structure. The first one ABA_DLISTITEM forms the basic building
 * block of the list storing an element and pointers to the
 * predecessor and successor in the list, the second one is the
 * ABA_LIST itself.
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


#ifndef ABA_DLIST_H
#define ABA_DLIST_H
#include <iostream>
using namespace std;

#include "abacus/abacusroot.h"
#include "abacus/global.h"
#include "abacus/dlistitem.h"

template<class Type>
class  ABA_DLIST;

template<class Type>
ostream &operator<< (ostream&, const ABA_DLIST<Type> &list);

  template<class Type> class  ABA_DLIST :  public ABA_ABACUSROOT  {
    public:

/*! The constructor for an empty list.

 *    \param glob A pointer to the corresponding global object.
 */
      ABA_DLIST(ABA_GLOBAL *glob);

/*! The destructor deallocates the memory of all items in the list.
 */
      ~ABA_DLIST();

/*! The output operator writes all elements of the \a list on an output stream.
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param list The list being output.
 */
      friend ostream& operator<< <> (ostream&, const ABA_DLIST<Type> &);

/*! Adds an element at the end of the list.
 *
 * \param elem The element being appended.
 */
      void append(const Type &elem);

/*! Assigns to \a elem the first element in the list and removes it from the list.
 *
 * \return 0 If the operation can be executed successfully.
 * \return 1 If the list is empty.
 *
 * \param elem If the list is nonemty, the first element is assigned
 *                to \a elem.
 */
      int extractHead(Type &elem);

/*! \brief If the list is non-empty, the function \a removeHead()
 * removes the head of the list.
 *
 * \return 0 If the list is non-empty before the function is called,
 * \return 1 otherwise.
 */
      int removeHead();

/*! The function \a remove() removes \a item from the list.
 */
      void remove(const Type &elem);

/*! \brief This version of the function \a remove()
 * scans the list for an item with element \a elem
 * beginning at the first element of the list.
 *
 * The first matching item is removed from the list.
 *
 * \param elem The element which should be removed.
 */
      void remove(ABA_DLISTITEM<Type> *item);

/*! Returns a pointer to the first item of the list.
 */
      ABA_DLISTITEM<Type>* first() const;

/*! Returns a pointer to the last item of the list.
 */
      ABA_DLISTITEM<Type>* last() const;

/*! \return true If no element is contained in the list,
 * \return false otherwise.
 */
      bool empty() const;

/*! Retrieves the first element of the list.
 *
 * \return 0 If the list is not empty,
 * \return 1 otherwise.
 *
 * \param elem Stores the first element of the list after the function
 *          call if the list is not empty.
 */
      int firstElem(Type& elem) const;

   private:


      /*! A pointer to corresponding global object.
       */     
     ABA_GLOBAL    *glob_;

     /*! A pointer to the first item of the list.
      */
     ABA_DLISTITEM<Type> *first_;

     /*! A pointer to the last item in the list.
      */
     ABA_DLISTITEM<Type> *last_;

     ABA_DLIST(const ABA_DLIST &rhs);
     const ABA_DLIST<Type>& operator=(const ABA_DLIST<Type>& rhs);
  };

#include "abacus/dlist.inc"

//! ABA_DLIST: iterators
/*! The iterator \a forAllDListElem assigns to \a {Type e} all elements in the
 * list beginning with the first element. The additional parameter
 * \a item has to be of type ABA_DLISTITEM<Type>*.
 * Deletions of elements in the list during the application of this
 * iterator can cause an error.
 * The operator \a ?: is used that \a item is only dereferenced if
 * \a item != 0. The expression 0 on the right side of \a :
 * is only a dummy that this expression is compilable.
 *
 * \param L The list that should be iterated (ABA_DLIST<Type>).
 * \param item An auxilliary pointer to a list item (ABA_DLISTITEM<Type> *).
 * \param e The elements in the list are assigned to this variable (\a Type).
 */
#define forAllDListElem(L, item, e) \
  for((item = (L).first()) ? (e = (item)->elem()) : 0; item !=0; \
    (item = (item)->succ()) ? (e = (item)->elem()) : 0)

#endif  // ABA_DLIST_H

/*! \class ABA_DLIST
 * \brief class ABA_DLIST implements a doubly linked linear list.
 * The list is implemented by a doubly linked list of ABA_DLISTITEMs.
 */
 

