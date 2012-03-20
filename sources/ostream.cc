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

#include "abacus/ostream.h"
#include "abacus/string.h"
#include "abacus/history.h"
#include "abacus/timer.h"
#include "abacus/lpvarstat.h"
#include "abacus/row.h"
#include "abacus/constraint.h"
#include "abacus/lp.h"
#include "abacus/exceptions.h"

  ABA_OSTREAM::ABA_OSTREAM(ostream &out, const char *logStreamName) 
  :  
    ostream(out.rdbuf()),
    out_(*this),  
    on_(true),  
    log_(0)
  {
    if (logStreamName == 0) logOn_ = false;
    else {
      logOn_ = true;
      log_ = new   ofstream(logStreamName)  ;

      if (!(*log_)) {
        char *_error=new char[255];
	sprintf(_error, "ABA_OSTREAM::ABA_OSTREAM(): opening log-file %s failed", logStreamName);
	THROW_PARAM(AlgorithmFailureException,afcOstream,_error);
      }
    }
  }

  ABA_OSTREAM::~ABA_OSTREAM()
  {
    delete log_;
  }

 ABA_OSTREAM& ABA_OSTREAM::operator<<(char o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;

  }


  ABA_OSTREAM& ABA_OSTREAM::operator<<(unsigned char o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;
  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(signed char o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;
  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(short o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;
  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(unsigned short o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;
  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(int o) 
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;
  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(unsigned int o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;

  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(long o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;

  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(unsigned long o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;

  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(float o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;

  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(double o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;
  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(const char *o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;
  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(const ABA_STRING &o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;
  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(const ABA_TIMER &o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;
  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(const ABA_HISTORY &o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;
  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(const ABA_LP &o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;
  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(const ABA_LPVARSTAT &o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;
  }

  ABA_OSTREAM& ABA_OSTREAM::operator<<(const ABA_CSENSE &o)
  {
    // body of the output operator 
    if(on_) out_ << o;
    if(logOn_) *log_ << o;
    return *this;
  }



/*
  ABA_OSTREAM& ABA_OSTREAM::operator<<(ABA_OSTREAM_MANIP m)
  {
    return (*m)(*this);
  }
*/
  void ABA_OSTREAM::off()
  {
    on_ = false;
  }

  void ABA_OSTREAM::on()
  {
    on_ = true;
  }

  void ABA_OSTREAM::logOn()
  {
    if(log_ == 0) {
      const char *_error="ABA_OSTREAM::logOn: logfile not initialized, cannot be turned on.";
      THROW_PARAM(AlgorithmFailureException,afcOstream,_error);
    }
    logOn_ = true;
  }

  void ABA_OSTREAM::logOn(const char *logStreamName)
  {
    delete log_;
    
    log_ = new  ofstream(logStreamName)  ;

    if (!(*log_)) {
      char *_error = new char [255];
      sprintf(_error,"ABA_OSTREAM::ABA_OSTREAM(): opening log-file %s failed.", logStreamName);
      THROW_PARAM(AlgorithmFailureException,afcOstream,_error);
    }

    logOn();
  }

  void ABA_OSTREAM::logOff()
  {
    logOn_ = false;
  }

  ofstream* ABA_OSTREAM::log() const
  {
    return log_;
  }

  void ABA_OSTREAM::setFormatFlag(fmtflags flag)
  {
    out_.setf(flag);
    if (log_) log_->setf(flag);
  }

  bool ABA_OSTREAM::isOn() const
  {
    return on_;
  }

  bool ABA_OSTREAM::isLogOn() const
  {
    return logOn_;
  }

 
  ABA_OSTREAM& flush(ABA_OSTREAM &o)
  {
    if (o.on_) o.out_ << flush;
    if(o.logOn_) *(o.log_) <<flush;
    return o;
  }
 
  ABA_OSTREAM& endl(ABA_OSTREAM &o)
  {
    o << '\n';
    if (o.on_) o.out_ << flush;
    if(o.logOn_) *(o.log_) << flush;
    return o;
  }
  
  ABA_OSTREAM& _setWidth(ABA_OSTREAM &o, int w)
  {
    if (o.on_) o.out_ << setw(w);
    if(o.logOn_) *(o.log_) << setw(w);
    return o;
  }
  
  ABA_OSTREAM& _setPrecision(ABA_OSTREAM &o, int p)
  {
    if (o.on_) o.out_ << setprecision(p);
    if(o.logOn_) *(o.log_) << setprecision(p);
    return o;
  }

  ABA_OSTREAM_MANIP_INT::ABA_OSTREAM_MANIP_INT(ABA_OSTREAM &(*f)(ABA_OSTREAM &o, int  i), 
                                       int i) :  
    i_(i),  
    f_(f)
  {}

  ABA_OSTREAM &operator<<(ABA_OSTREAM &o, const ABA_OSTREAM_MANIP_INT &m)
  {
    return (*(m.f_))(o, m.i_);
  }

  ABA_OSTREAM_MANIP_INT setWidth(int p)
  {
    return ABA_OSTREAM_MANIP_INT(&_setWidth, p);
  }

  ABA_OSTREAM_MANIP_INT setPrecision(int p)
  {
    return ABA_OSTREAM_MANIP_INT(&_setPrecision, p);
  }
