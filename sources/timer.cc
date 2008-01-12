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

#include "abacus/timer.h"
#include "abacus/string.h"
#include "abacus/global.h"
 
  ABA_TIMER::ABA_TIMER(ABA_GLOBAL *glob) :  
    glob_(glob),  
    totalTime_(0),  
    running_(false)
    { }

  ABA_TIMER::ABA_TIMER(ABA_GLOBAL *glob, long centiSeconds) :  
    glob_(glob),  
    totalTime_(centiSeconds),  
    running_(false)
    { }
 
  ABA_TIMER::~ABA_TIMER()
  { }

  ostream& operator<<(ostream& out, const ABA_TIMER& rhs)
  {
    long centiSeconds = rhs.centiSeconds();

    long sec  = centiSeconds/100;
    long mSec = centiSeconds - 100*sec;
    long rSec = sec%60;
    long min  = sec/60;
    long rMin = min%60;
    
    out << min/60 << ":";
    if(rMin < 10) out << '0';
    out << rMin << ':';
    if(rSec < 10) out << '0';
    out << rSec << '.';
    if (mSec < 10) out << '0';
    out << mSec;
    return out;
  }

  void ABA_TIMER::start(bool reset)
  {
    if (reset)
      totalTime_ = 0;
    else
      if (running_) {
        glob_->err() << "ABA_TIMER::start(): you cannot start a running timer." << endl;
        exit(Fatal);
      }
    running_   = true;
    startTime_ = theTime();
  }

  void ABA_TIMER::stop()
  {
    if(!running_) {
      glob_->err() << "ABA_TIMER::stop : you cannot stop a non-running timer." << endl;
      exit(Fatal);
    }
    
    totalTime_ += theTime() - startTime_;
    running_   = false;

  }

  void ABA_TIMER::reset()
  {
    running_   = false;
    totalTime_ = 0;
  }

  bool ABA_TIMER::running() const
  {
    return running_;
  }

  long ABA_TIMER::centiSeconds() const
  {
    long cs;
    
    if(running_) cs = totalTime_ + theTime() - startTime_;
    else         cs = totalTime_;

    return cs;
  }

  long ABA_TIMER::seconds() const
  {
    return centiSeconds()/100;
  }

  long ABA_TIMER::minutes() const
  {
    return centiSeconds()/6000;
  }

  long ABA_TIMER::hours() const
  {
    return seconds()/3600;
  }

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

  bool ABA_TIMER::exceeds(const ABA_STRING &maxTime) const
  {
    long maxSecs;  //!< \a maxTime in seconds
    
  // convert \a maxTime in seconds
  long s  = 0;   //!< seconds in \a maxTime
  long m  = 0;   //!< minutes in \a maxTime
  long h  = 0;   //!< hours in \a maxTime
  ABA_STRING buf = maxTime;  //!< copy of \a maxTime which is modified during the conversion
  
// begin at the end of \a maxTime and read the seconds  
/* We start at the end of \a buf and go left until a \a ':' is found
 *   or we have reached the first character of the string. The string
 *   on the right side of the current position are the seconds.
 */  
  int pos = buf.size() - 1;
  while (pos >= 0 && buf[pos] != ':') pos--;
  s = buf.ascii2int(pos + 1);

// proceed with the minutes 
/* If we have not reached the beginning of \a buf we replace the \a ':'
 *   by \a '\0' to mark the new end of the string. We go left
 *   until we reach the beginning of the string or the next \a ':'.
 *   The string on the right side of the current position are the
 *   minutes.
 */  
  if(pos >= 0) {
    buf[pos] = '\0';
    pos--;
    while (pos >= 0 && buf[pos] != ':') pos--;
    m = buf.ascii2int(pos + 1);

  }

// proceed with the hours
/* If we still have not reached the beginning of the string, we again replace
 *   the \a ':' by a \a '\0' to mark the new end of the string. If there
 *   are still characters on the left side from the current position,
 *   then this string is the hours.
 */  
  if (pos >= 0) {
    buf[pos] = '\0';
    if (--pos >= 0) h = buf.ascii2int(0);
  }

  
  maxSecs = s + 60*m + 3600*h;

    if (seconds() >= maxSecs) return true;
    else                      return false;
  }

  void ABA_TIMER::addCentiSeconds(long centiSeconds)
  {
#ifdef ABACUS_PARALLEL
    mutex_.acquire();
#endif
    totalTime_ += centiSeconds;
#ifdef ABACUS_PARALLEL
    mutex_.release();
#endif
  }
