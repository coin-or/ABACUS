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

#include "abacus/id.h"
#include "abacus/message.h"

  ABA_ID::ABA_ID()
  :  
    sequence_(0)
  {
  }

  ABA_ID::ABA_ID(ABA_MESSAGE &msg)
  {
    msg.unpack(sequence_);
    msg.unpack(proc_);
    msg.unpack(index_);
  }

  void ABA_ID::pack(ABA_MESSAGE &msg) const
  {
    msg.pack(sequence_);
    msg.pack(proc_);
    msg.pack(index_);
  }

  ostream& operator<<(ostream &out, const ABA_ID &id)
  {
    if (id.sequence_ == 0)
      return out << "(uninitialized id)";
    else
      return out << '(' << id.sequence_ << ',' << id.proc_ << ','
                 << id.index_ << ')';
  }

  int operator==(const ABA_ID &lhs, const ABA_ID &rhs)
  {
    return lhs.sequence_ == rhs.sequence_ && lhs.proc_ == rhs.proc_ &&
           lhs.index_ == rhs.index_;
  }

  void ABA_ID::initialize(unsigned long sequence, int proc, int index)
  {
    sequence_ = sequence;
    proc_ = proc;
    index_ = index;
  }

  void ABA_ID::uninitialize()
  {
    sequence_ = 0;
  }

  bool ABA_ID::isInitialized() const
  {
    return sequence_ != 0;
  }



