/*!\file
 * \author Matthias Elf
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

#ifndef ABA_DLISTITEM_INC
#define ABA_DLISTITEM_INC

#include <iostream>
using namespace std;

  template <class Type>
  inline ABA_DLISTITEM<Type>::ABA_DLISTITEM (const Type &elem, ABA_DLISTITEM<Type> *pred, 
                                      ABA_DLISTITEM<Type> *succ) 
  : 
    elem_(elem), 
    pred_(pred), 
    succ_(succ)
    {  }

  template <class Type>
  ostream& operator<<(ostream &out, const ABA_DLISTITEM<Type> &item)
  {
    out << item.elem_;
    return out;
  }

  template<class Type>
  inline Type ABA_DLISTITEM<Type>::elem() const
  {
    return elem_;
  }

  template<class Type>
  inline ABA_DLISTITEM<Type>* ABA_DLISTITEM<Type>::succ() const
  {
    return succ_;
  }

  template<class Type>
  inline ABA_DLISTITEM<Type>* ABA_DLISTITEM<Type>::pred() const 
  {
    return pred_;
  }

#endif   // ABA_DLISTITEM_INC
