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

#include "abacus/global.h"
#include "abacus/fsvarstat.h"

#ifdef ABACUS_PARALLEL
#include "abacus/message.h"
#endif

  ABA_FSVARSTAT::ABA_FSVARSTAT(ABA_GLOBAL *glob, STATUS status) 
   :  
    glob_(glob),  
    status_(status),  
    value_(0.0)
  {
    if (status == Fixed || status == Set) {
      glob_->err() << "ABA_FSVARSTAT::ABA_FSVARSTAT(): ";
      glob_->err() << "value to set/fix missing" << endl;
      exit (Fatal);
    }
  }

  ABA_FSVARSTAT::ABA_FSVARSTAT(ABA_GLOBAL *glob, STATUS status, double value) 
  :  
    glob_(glob),  
    status_(status),  
    value_(value)
  {
    if (status != Fixed && status != Set) {
      glob_->err() << "ABA_FSVARSTAT::ABA_FSVARSTAT(): wrong status for this constructor" << endl;
      exit (Fatal);
    }
  }

  ABA_FSVARSTAT::ABA_FSVARSTAT(ABA_FSVARSTAT *fsVarStat) 
  :  
    glob_(fsVarStat->glob_),  
    status_(fsVarStat->status_),  
    value_(fsVarStat->value_)
  {
  }

#ifdef ABACUS_PARALLEL

  ABA_FSVARSTAT::ABA_FSVARSTAT(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
    :  
    glob_(glob)
  {
    status_ = (STATUS) msg.unpackInt();
    msg.unpack(value_);
  }

  void ABA_FSVARSTAT::pack(ABA_MESSAGE &msg) const
  {
    msg.pack((int)status_);
    msg.pack(value_);
  }

#endif

  ostream &operator<<(ostream& out, const ABA_FSVARSTAT &rhs)
  {
    switch (rhs.status_) {
      case ABA_FSVARSTAT::Free:              out << "Free";
                                      break;
      case ABA_FSVARSTAT::SetToLowerBound:   out << "SetToLowerBound";
                                      break;
      case ABA_FSVARSTAT::Set:               out << "Set to " << rhs.value_;
                                      break;
      case ABA_FSVARSTAT::SetToUpperBound:   out << "SetToUpperBound";
                                      break;
      case ABA_FSVARSTAT::FixedToLowerBound: out << "FixedToLowerBound";
                                      break;
      case ABA_FSVARSTAT::Fixed:             out << "Fixed to "<< rhs.value_;
                                      break;
      case ABA_FSVARSTAT::FixedToUpperBound: out << "FixedToUpperBound";
                                       break;
      default: rhs.glob_->err() << "unknown status" << endl;
               rhs.exit(rhs.Fatal);
               break;
    }
    return out;
  }

  bool ABA_FSVARSTAT::fixed() const
  {
    switch (status_) {
      case FixedToLowerBound:
      case FixedToUpperBound:
      case Fixed:             return true;
      default:                return false;
    }
  }

  bool ABA_FSVARSTAT::set() const
  {
    switch (status_) {
      case SetToLowerBound:
      case SetToUpperBound:
      case Set:             return true;
      default:              return false;
    }
  }

  bool ABA_FSVARSTAT::contradiction(ABA_FSVARSTAT *fsVarStat) const
  {
    STATUS status = fsVarStat->status();
    
    switch (status) {
      case Set:
      case Fixed: return contradiction(status, fsVarStat->value());
      default   : return contradiction(status);
    }
  }

  bool ABA_FSVARSTAT::contradiction(STATUS status, double value) const
  {

    switch (status_) {
      case SetToLowerBound:
      case FixedToLowerBound: switch (status) {
                                case SetToUpperBound:
                                case FixedToUpperBound:
                                case Set:
                                case Fixed:               return true;
                                
                                default:                  return false;
      }

      case SetToUpperBound:
      case FixedToUpperBound: switch (status) {
                                case SetToLowerBound:
                                case FixedToLowerBound:
                                case Set:
                                case Fixed:               return true;
                                
                                default:                  return false;
      }
      case Fixed:
      case Set:  switch (status) {
                   case Fixed:
                   case Set: if (glob_->equal(value_, value)) return true;
                             else                             return false;
                   default:  break;
      }
      default: return false;
    }
  }

