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
 * \brief The generation of external templates for the GNU-Compiler.
 *
 * \par
 * The templates of the message constructor and the ABA_ARRAY<T>::pack() function have
 * to be specialized for all array types which should be sent or received.
 *
 * \par
 * The library currently implements the following template specializations:
 */


#include "abacus/array.h"
#include "abacus/array.inc"

#ifdef ABACUS_TEMPLATES_EXPLICIT
#include "array.eti"
#endif


#ifdef ABACUS_PARALLEL

ABA_ARRAY<int>::ABA_ARRAY(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
    : glob_(glob)
  {
    msg.unpack(n_);
    a_ = new int[n_];
    msg.unpack(a_, msg.unpackInt());
  }

ABA_ARRAY<double>::ABA_ARRAY(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
    : glob_(glob)
  {
    msg.unpack(n_);
    a_ = new double[n_];
    msg.unpack(a_, msg.unpackInt());
  }

ABA_ARRAY<ABA_FSVARSTAT*>::ABA_ARRAY(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
    : glob_(glob)
  {
    msg.unpack(n_);
    a_ = new (ABA_FSVARSTAT*)[n_];
    int nPacked;
    msg.unpack(nPacked);
    for (int i = 0; i < nPacked; i++)
      a_[i] = new ABA_FSVARSTAT(glob, msg);
  }

ABA_ARRAY<ABA_LPVARSTAT*>::ABA_ARRAY(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
    : glob_(glob)
  {
    msg.unpack(n_);
    a_ = new (ABA_LPVARSTAT*)[n_];
    int nPacked;
    msg.unpack(nPacked);
    for (int i = 0; i < nPacked; i++)
      a_[i] = new ABA_LPVARSTAT(glob, msg);
  }

ABA_ARRAY<ABA_SLACKSTAT*>::ABA_ARRAY(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
    : glob_(glob)
  {
    msg.unpack(n_);
    a_ = new (ABA_SLACKSTAT*)[n_];
    int nPacked;
    msg.unpack(nPacked);
    for (int i = 0; i < nPacked; i++)
      a_[i] = new ABA_SLACKSTAT(glob, msg);
  }

void ABA_ARRAY<int>::pack(ABA_MESSAGE &msg, int nPacked) const
  {
    msg.pack(n_);
    msg.pack(nPacked);
    msg.pack(a_, nPacked);
  }

void ABA_ARRAY<double>::pack(ABA_MESSAGE &msg, int nPacked) const
  {
    msg.pack(n_);
    msg.pack(nPacked);
    msg.pack(a_, nPacked);
  }

void ABA_ARRAY<ABA_FSVARSTAT*>::pack(ABA_MESSAGE &msg, int nPacked) const
  {
    msg.pack(n_);
    msg.pack(nPacked);
    for (int i = 0; i < nPacked; i++)
      a_[i]->pack(msg);
  }

void ABA_ARRAY<ABA_LPVARSTAT*>::pack(ABA_MESSAGE &msg, int nPacked) const
  {
    msg.pack(n_);
    msg.pack(nPacked);
    for (int i = 0; i < nPacked; i++)
      a_[i]->pack(msg);
  }

void ABA_ARRAY<ABA_SLACKSTAT*>::pack(ABA_MESSAGE &msg, int nPacked) const
  {
    msg.pack(n_);
    msg.pack(nPacked);
    for (int i = 0; i < nPacked; i++)
      a_[i]->pack(msg);
  }

#endif
