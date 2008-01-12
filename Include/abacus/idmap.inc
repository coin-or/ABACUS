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

#include "abacus/master.h"
#include "abacus/id.h"
#include "abacus/hash.h"

template <class Type> 
  ABA_IDMAP<Type>::ABA_IDMAP(ABA_MASTER *master, int size, int index)
    :
    master_(master),  
    map_(master, size),  
    sequence_(1),  
    index_(index)
  {
    proc_ = master->parmaster()->hostId();
  }

  template <class Type> 
  ABA_IDMAP<Type>::~ABA_IDMAP()
  {
  }

  template <class Type>
  ostream& operator<<(ostream &out, const ABA_IDMAP<Type> &idmap)
  {
    idmap.mp_.acquire();

    out << "ABA_IDMAP: proc=" << idmap.proc_
        << ", sequence=" << idmap.sequence_
        << endl;
    out << idmap.map_;

    idmap.mp_.release();
    return out;
  }

  template<class Type>
  Type *ABA_IDMAP<Type>::find(const ABA_ID &id)
  {
    mp_.acquire();

    const Type **ptr = map_.find(id);

    mp_.release();
    return ptr ? (Type*)(*ptr) : 0;
  }

  template<class Type>
  void ABA_IDMAP<Type>::insert(const ABA_ID &id, const Type *obj)
  {
    mp_.acquire();

#ifdef ABACUSSAFE
    if (map_.find(id)) {
      master_->err() << "ABA_IDMAP::insert(): tried to insert ABA_ID "
                     << id << " more than once.";
      exit(Fatal);
    }
#endif

    map_.insert(id, obj);

    mp_.release();
  }

extern "C" {
#include <limits.h>	//!< ULONG_MAX
}
  template<class Type>
  void ABA_IDMAP<Type>::insertWithNewId(ABA_ID &id, const Type *obj)
  {
    mp_.acquire();

    if (sequence_ == ULONG_MAX) {
      master_->err() << "ABA_IDMAP::insertWithNewId(): insertion failed, "
                        "maximum sequence number ULONG_MAX = "
                     << ULONG_MAX << " reached";
      exit (Fatal);
    }
    id.initialize(sequence_++, proc_, index_);

#ifdef ABACUSSAFE
    if (map_.find(id)) {
      master_->err() << "ABA_IDMAP::insertWithNewId(): tried to insert ABA_ID "
                     << id << " more than once.";
      exit(Fatal);
    }
#endif

    map_.insert(id, obj);
    mp_.release();
  }

  template<class Type>
  int ABA_IDMAP<Type>::remove(const ABA_ID &id)
  {
    mp_.acquire();
    int status = map_.remove(id);
    mp_.release();
    return status;
  }
