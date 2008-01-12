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
 * $Id: tailoff.cc,v 2.5 2007/07/19 08:03:41 baumann Exp $
 */

#include "abacus/tailoff.h"
#include "abacus/master.h"

#include <math.h>

  ABA_TAILOFF::ABA_TAILOFF(ABA_MASTER *master) 
  :  
    master_(master)  
  {
    if (master->tailOffNLp() > 0)
      lpHistory_ = new ABA_RING<double>(master, master->tailOffNLp());
    else
      lpHistory_ = 0;
  }

// alternative tailoff constructor
// The parameter NLp determines the size of the buffer
  ABA_TAILOFF::ABA_TAILOFF(ABA_MASTER *master, int NLp) 
  :  
    master_(master)  
  {
    if (NLp > 0)
      lpHistory_ = new ABA_RING<double>(master, NLp);
    else
      lpHistory_ = 0;
  }

  ABA_TAILOFF::~ABA_TAILOFF()
  {
    delete lpHistory_;
  }

  ostream &operator<<(ostream &out, const ABA_TAILOFF &rhs)
  {
    out << "LP-history:" << endl;
    if (rhs.lpHistory_) 
      out << *(rhs.lpHistory_);
    else
      out << "no LP-history available" << endl;

    return out;
  }

  void ABA_TAILOFF::update(double value)
  {
    if (lpHistory_) lpHistory_->insert(value);
  }

  void ABA_TAILOFF::reset()
  {
    if (lpHistory_) lpHistory_->clear();
  }

  bool ABA_TAILOFF::tailOff() const
  {
    if (!lpHistory_) return false;
    
    if (!lpHistory_->filled()) return false;  //!< not enough iterations

    if (fabs((lpHistory_->oldest() - lpHistory_->newest())*100.0
               /lpHistory_->oldest())
             < master_->tailOffPercent()) return true;
    else return false;
  }

  int ABA_TAILOFF::diff(int nLps, double &d) const
  {
    double oldVal;
    if (lpHistory_->previous(nLps, oldVal))
      return 1;

    double lastVal = lpHistory_->newest();

    d = fabs((lastVal - oldVal)*100.0/oldVal);

    return 0;
  }
