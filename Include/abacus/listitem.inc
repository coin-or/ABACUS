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

#ifndef ABA_LISTITEM_INC
#define ABA_LISTITEM_INC

  template <class Type>
  inline ABA_LISTITEM<Type>::ABA_LISTITEM(const Type &elem, ABA_LISTITEM<Type> *succ) 
  : 
    elem_(elem), 
    succ_(succ)
    {  }

  template <class Type>
  ostream& operator<<(ostream &out, const ABA_LISTITEM<Type> &item)
  {
    out << item.elem_;
    return out;
  }

  template<class Type>
  inline Type ABA_LISTITEM<Type>::elem() const
  {
    return elem_;
  }

  template<class Type>
  inline ABA_LISTITEM<Type> * ABA_LISTITEM<Type>::succ() const
  {
    return succ_;
  }


#endif   // ABA_LISTITEM_INC
