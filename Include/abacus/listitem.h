/*!\file
 * \author Matthias Elf
 *
 * \brief listitem.
 *
 * We call the basic building block of a linked list an
 * {\it item\/} that is implemented by the class ABA_LISTITEM.
 * A ABA_LISTITEM stores a copy of the inserted element and
 * a pointer to its successor.
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
 

#ifndef ABA_LISTITEM_H
#define ABA_LISTITEM_H

#include <iostream>
using namespace std;

#include "abacus/abacusroot.h"

template <class Type>
class  ABA_LISTITEM;

template <class Type>
ostream &operator<< (ostream &out, const ABA_LISTITEM<Type> &item);

template<class Type> class ABA_LIST;

  template<class Type> class  ABA_LISTITEM :  public ABA_ABACUSROOT  {
    friend class ABA_LIST<Type>;
    public:

/*! The constructor.
 *
 *    \param elem A copy of \a elem becomes the element of the list item.
 *    \param succ A pointer to the successor of the item in the list.
 */ 
      ABA_LISTITEM (const Type &elem, ABA_LISTITEM<Type> *succ);

/*! The output operator.
 * 
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param item The list item being output.
 */
      friend ostream& operator<< <> (ostream &, const ABA_LISTITEM<Type> &);


/*! \return The element of the item.
 */
      Type elem() const;

/*! \return The successor of the item in the list.
 */
      ABA_LISTITEM<Type> *succ() const;
    private:
      Type            elem_;
      ABA_LISTITEM<Type> *succ_;
  };

#include "abacus/listitem.inc"

#endif // ABA_LISTITEM_H
  
/*! \class ABA_LISTITEM
 * \brief We call the basic building block of a linked list an
 * {\it item\/} that is implemented by the class ABA_LISTITEM.
 *
 *  \param Type elem_ The element of the item.
 *      
 *  \param ABA_LISTITEM<Type> *succ_ A pointer to the successor of the item in 
 *                             the list.
 *                             The successor of the last item is \a 0.
 */

