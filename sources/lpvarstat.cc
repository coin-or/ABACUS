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

#include "abacus/lpvarstat.h"
#include "abacus/global.h"

#ifdef ABACUS_PARALLEL
#include "abacus/message.h"
#endif

#ifdef ABACUS_PARALLEL

  ABA_LPVARSTAT::ABA_LPVARSTAT(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
    :  
    glob_(glob)
  {
    status_ = (STATUS) msg.unpackInt();
  }

  void ABA_LPVARSTAT::pack(ABA_MESSAGE &msg) const
  {
    msg.pack((int)status_);
  }

#endif

  ostream &operator<<(ostream& out, const ABA_LPVARSTAT &rhs)
  {
    switch (rhs.status_) {
      case ABA_LPVARSTAT::AtLowerBound:  out << "AtLowerBound";
                                     break;
      case ABA_LPVARSTAT::Basic:         out << "Basic";
                                     break;
      case ABA_LPVARSTAT::AtUpperBound:  out << "AtUpperBound";
                                     break;
      case ABA_LPVARSTAT::NonBasicFree:  out << "NonBasicFree";
                                     break;
      case ABA_LPVARSTAT::Eliminated:    out << "Eliminated";
                                     break;                          
      case ABA_LPVARSTAT::Unknown:       out << "Unknown";
                                     break;
      default:                       rhs.glob_->err() << "Unknown status" << endl;
                                     rhs.exit(rhs.Fatal);
    }
    return out;
  }
