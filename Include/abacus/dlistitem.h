/*!\file
 * \author Matthias Elf
 * \brief item of doubly linked list.
 * 
 * We call the basic building block of a doubly linked list an
 * {\it item\/}, which is implemented by the class ABA_DLISTITEM.
 * A ABA_DLISTITEM stores a copy of the inserted element and
 * has pointers to its predecessor and its successor.
 * The predecessor of the first and the successor of the last item are \a 0.
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


#ifndef ABA_DLISTITEM_H
#define ABA_DLISTITEM_H

#include "abacus/abacusroot.h"


#ifdef ABACUS_NEW_TEMPLATE_SYNTAX
template<class Type>
class ABA_DLISTITEM;

template<class Type>
ostream &operator<< (ostream &out, const ABA_DLISTITEM<Type> &item);
#endif

template<class Type> class ABA_DLIST;
// class ostream;

  template<class Type> class  ABA_DLISTITEM :  public ABA_ABACUSROOT  {
    friend class ABA_DLIST<Type>;
    public:

/*! The constructor.
 *
 *    \param elem The element of the item.
 *    \param pred A pointer to the previous item in the list.
 *    \param succ A pointer to the next item in the list.
 */
      ABA_DLISTITEM (const Type &elem, 
                     ABA_DLISTITEM<Type> *pred, 
                     ABA_DLISTITEM<Type> *succ);
#ifdef ABACUS_NEW_TEMPLATE_SYNTAX

/*! The output operator.
 * 
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param item The list item being output.
 */
      friend ostream& operator<< <> (ostream &, const ABA_DLISTITEM<Type> &);
#else
   
/*! The output operator.
 * 
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param item The list item being output.
 */
      friend ostream& operator<< (ostream &out, const ABA_DLISTITEM<Type> &item);
#endif

/*! \return The element stored in the item.
 */
      Type elem() const;

/*! \return A pointer to the successor of the item in the list.
 */
      ABA_DLISTITEM<Type> *succ() const;

/*! \return A pointer to the predecessor of the item in the list.
 */
      ABA_DLISTITEM<Type> *pred() const;

    private:
      Type elem_;
      ABA_DLISTITEM<Type> *pred_;
      ABA_DLISTITEM<Type> *succ_;
  };

#include "abacus/dlistitem.inc"

#endif  // ABA_DLISTITEM_H

/*! \class ABA_DLISTITEM
  * \brief A ABA_DLISTITEM stores a copy of the inserted element and
 * has pointers to its predecessor and its successor.
 * \param Type elem_ The element stored in the item.
 *
 * \param ABA_DLISTITEM<Type> *pred_ A pointer to predecessor of the item in the list.
 *
 * \param ABA_DLISTITEM<Type> *succ_ A pointer to the successor of the item in the list.
 */


