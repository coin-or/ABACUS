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

#include "abacus/numcon.h"
#include "abacus/master.h"
#include "abacus/colvar.h"

  ABA_NUMCON::ABA_NUMCON(ABA_MASTER *master, 
                         const ABA_SUB *sub, 
                         ABA_CSENSE::SENSE sense, 
                         bool dynamic,
                         bool local, 
                         bool liftable,
                         int number, 
                         double rhs) 
  :  
   ABA_CONSTRAINT(master, sub,  sense, rhs, dynamic, local, liftable),  
   number_(number)
  { }

  ABA_NUMCON::~ABA_NUMCON()
  { }


#ifdef ABACUS_PARALLEL

  ABA_NUMCON::ABA_NUMCON(const ABA_MASTER *master, ABA_MESSAGE &msg)
  :  
    ABA_CONSTRAINT(master, msg)
  {
    msg.unpack(number_);
  }

  void ABA_NUMCON::pack(ABA_MESSAGE &msg) const
  {
    ABA_CONSTRAINT::pack(msg);
    msg.pack(number_);
  }

  int ABA_NUMCON::classId() const
  {
    return NUMCON_CLASSID;
  }

#endif

  ostream &operator<<(ostream &out, const ABA_NUMCON &rhs)
  {
    return out << "number = " << rhs.number_ << "  rhs = " << rhs.rhs_ << endl;
  }

  double ABA_NUMCON::coeff(ABA_VARIABLE *v)
  {
    ABA_COLVAR *colvar = (ABA_COLVAR *) v;

    return colvar->coeff(number_);
  }

  void ABA_NUMCON::print(ostream &out)
  {
    out << *this;
  }
