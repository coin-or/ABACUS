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
 * 
 * \brief The generation of external templates with the GNU-compiler.
 *
 * \par
 * The templates of the message constructor and the |pack()| function have
 * to be specialized for all buffer types which should be sent or received.
 *
 * \par
 * The library currently implements the following template specializations:
 */

#include "abacus/buffer.h"
#include "abacus/buffer.inc"

#ifdef ABACUS_TEMPLATES_EXPLICIT
#include "buffer.eti"
#endif

#ifdef ABACUS_PARALLEL

ABA_BUFFER<int>::ABA_BUFFER(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
    : glob_(glob)
  {
    msg.unpack(size_);
    msg.unpack(n_);
    buf_ = new int[size_];
    msg.unpack(buf_, n_);
  }



void ABA_BUFFER<int>::pack(ABA_MESSAGE &msg) const
  {
    msg.pack(size_);
    msg.pack(n_);
    msg.pack(buf_,n_);
  }

#endif
