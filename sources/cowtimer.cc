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

#include "abacus/cowtimer.h"

#include <sys/times.h>
#include <limits.h>

  ABA_COWTIMER::ABA_COWTIMER(ABA_GLOBAL *glob) 
  :  
    ABA_TIMER(glob)
  {
    struct tms now;
    baseTime_ = (long)times(&now);
  }

  ABA_COWTIMER::ABA_COWTIMER(ABA_GLOBAL *glob, long centiSeconds) 
  :  
    ABA_TIMER(glob, centiSeconds)
  {
    struct tms now;
    baseTime_ = (long)times(&now);
  }

  ABA_COWTIMER::~ABA_COWTIMER()
  { }
  
  long ABA_COWTIMER::theTime() const
  {
    struct tms now;
    return (long) times(&now) - baseTime_;
  }
