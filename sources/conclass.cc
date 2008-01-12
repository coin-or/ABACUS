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

#include "abacus/conclass.h"
#include "abacus/master.h"

#ifdef ABACUS_PARALLEL
#include "abacus/message.h"
#endif

ABA_CONCLASS::ABA_CONCLASS(const ABA_MASTER *master, bool discrete,
                     bool allVarBinary, bool trivial,
                     bool bound, bool varBound) :  
    master_(master),  
    discrete_(discrete),  
    allVarBinary_(allVarBinary),  
    trivial_(trivial),  
    bound_(bound),  
    varBound_(varBound)
  { }


#ifdef ABACUS_PARALLEL

ABA_CONCLASS::ABA_CONCLASS(const ABA_MASTER *master, ABA_MESSAGE &msg)
    :  
    master_(master)
  {
    int bits = msg.unpackInt();
    discrete_     = ((bits&1)?true:false);
    allVarBinary_ = ((bits&2)?true:false);
    trivial_      = ((bits&4)?true:false);
    bound_        = ((bits&8)?true:false);
    varBound_     = ((bits&16)?true:false);
  }

void ABA_CONCLASS::pack(ABA_MESSAGE &msg) const
  {
    msg.pack( discrete_ + 2*allVarBinary_ + 4*trivial_ +
              8*bound_ + 16*varBound_ );
  }

#endif

ostream &operator<<(ostream &out, const ABA_CONCLASS &rhs)
  {
    bool classified = false;

    if (rhs.discrete_) {
      out << "discrete/";
      classified = true;
    }
    if (rhs.allVarBinary_) {
      out << "allVarBinary/";
      classified = true;
    }
    if (rhs.trivial_) {
      out << "trivial/";
      classified = true;
    }
    if (rhs.bound_) {
      out << "bound/";
      classified = true;
    }
    if (rhs.varBound_) {
      out << "variable bound/";
      classified = true;
    }

    if (!classified) out << "no classification ";
    
    return out;
  }
