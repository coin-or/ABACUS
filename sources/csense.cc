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

#include "abacus/csense.h"
#include "abacus/global.h"

#ifdef ABACUS_PARALLEL
#include "abacus/message.h"
#endif

  ABA_CSENSE::ABA_CSENSE(ABA_GLOBAL *glob) 
  :  
    glob_(glob)
  { }

  ABA_CSENSE::ABA_CSENSE(ABA_GLOBAL *glob, SENSE s) 
  :  
    glob_(glob),  
    sense_(s)
  { }

  ABA_CSENSE::ABA_CSENSE(ABA_GLOBAL *glob, char s) 
  :  
    glob_(glob)
  {
    switch (s) {
      case 'l':
      case 'L': sense_ = Less;
                break;
      case 'e':
      case 'E': sense_ = Equal;
                break;
      case 'g':
      case 'G': sense_ = Greater;
                break;
      default: glob_->err() << "ABA_CSENSE::ABA_CSENSE(): unknown argument " << s << endl;
               exit(Fatal);
               break;
    }
  }


#ifdef ABACUS_PARALLEL

  ABA_CSENSE::ABA_CSENSE(ABA_GLOBAL *glob, ABA_MESSAGE &msg)
  :  
    glob_(glob)
  {
    sense_ = (SENSE) msg.unpackInt();
  }

  void ABA_CSENSE::pack(ABA_MESSAGE &msg) const
  {
    msg.pack((int)sense_);
  }

#endif

  ostream &operator<<(ostream &out, const ABA_CSENSE &rhs)
  {
    switch (rhs.sense_) {
      case ABA_CSENSE::Less:    out << "<=";
                            break;
      case ABA_CSENSE::Equal:   out << '=';
                            break;
      case ABA_CSENSE::Greater: out << ">=";
                            break;
    }
    return out;
  }

  void ABA_CSENSE::sense(char s)
  {
    switch (s) {
      case 'l':
      case 'L': sense_ = Less;
                break;
      case 'e':
      case 'E': sense_ = Equal;
                break;
      case 'g':
      case 'G': sense_ = Greater;
                break;
      default: glob_->err() << "ABA_CSENSE::sense(): unknown argument " << s << "." << endl;
               exit(Fatal);
               break;
    }
  }
