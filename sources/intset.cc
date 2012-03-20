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
 * \par
 *   The set is implemented by the arrays \a uninitializedMap_[] and
 *   \a position_[]. The following holds for each element e stored in
 *   the set.
 *
 *  \arg a) \f$0 \le \a uninitializedMap_[e] < \a count_|\f$
 *
 *  \arg b) \f$position_[uninitializedMap_[e]] = e\f$
 *
 *   It is not required to initialize the arrays.
 */
#include "abacus/intset.h"

  ABA_INTSET::ABA_INTSET()
  :
    uninitializedMap_(0),  
    position_(0),  
    count_(0)
  {
  }

  ABA_INTSET::~ABA_INTSET()
  {
    delete[] uninitializedMap_;
    delete[] position_;
  }

  void ABA_INTSET::initialize(int n)
  {
    delete[] uninitializedMap_;
    delete[] position_;
    uninitializedMap_ = new int[n];
    position_ = new int[n];
    count_ = 0;
  }

  bool ABA_INTSET::exists(int elem) const
  {
    int i = uninitializedMap_[elem];
    return (unsigned)i < (unsigned)count_ && position_[i]==elem;
  }

  void ABA_INTSET::insert(int elem)
  {
    int i = uninitializedMap_[elem];
    if ( (unsigned)i < (unsigned)count_ && position_[i]==elem)
      return;
    //!< elem is not contained in the set. It is inserted by the next two lines.
    uninitializedMap_[elem] = count_;
    position_[count_++] = elem;
  }

  void ABA_INTSET::remove(int elem)
  {
    int i = uninitializedMap_[elem];
    if ( (unsigned)i < (unsigned)count_ && position_[i]==elem) {
      // elem is contained in the set. We have to remove it.
      if (--count_ != i) {
        int help = position_[count_];
        position_[i] = help;
        uninitializedMap_[help] = i;
      }
    }
  }

  int ABA_INTSET::count() const
  {
    return count_;
  }

  int ABA_INTSET::elem(int index) const
  {
    return position_[index];
  }

