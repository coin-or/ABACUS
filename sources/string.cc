/*!\file
 *  We also include the file \a <string.h> that we can use the string
 *  manipulation functions from the C-library.
 * 
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "abacus/string.h"
#include "abacus/global.h"

#ifdef ABACUS_PARALLEL
#include "abacus/message.h"
#endif

  ABA_STRING::ABA_STRING(ABA_GLOBAL *glob, const char *cString) 
  :  
    glob_(glob)
  {
    string_ = new char[strlen(cString) + 1];
    ::strcpy(string_, cString);
  }

  ABA_STRING::ABA_STRING(ABA_GLOBAL *glob, const char *cString, int index) 
  :  
    glob_(glob)
  {
    string_ = new char[strlen(cString) + 11];
    sprintf(string_, "%s%d", cString, index);
  }

  ABA_STRING::ABA_STRING(const ABA_STRING &rhs)
  :  
    glob_(rhs.glob_)
  {
    string_ = new char[rhs.size() + 1];
    ::strcpy(string_, rhs.string_);
  }

  ABA_STRING::~ABA_STRING()
  {
    delete [] string_;
  }


#ifdef ABACUS_PARALLEL

  ABA_STRING::ABA_STRING(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
  :  
    glob_(glob),  
    string_(0)
  {
    unpack(msg);
  }

  void ABA_STRING::unpack(ABA_MESSAGE &msg)
  {
    int size;
    msg.unpack(size);
    delete [] string_;
    string_ = new char[size + 1];
    msg.unpack(string_, size);
    string_[size] = '\0';
  }

  void ABA_STRING::pack(ABA_MESSAGE &msg) const
  {
    int size = ::strlen(string_);
    msg.pack(size);
    msg.pack(string_, size);
  }

#endif

  const ABA_STRING& ABA_STRING::operator=(const ABA_STRING &rhs)
  {
    if (&rhs == this) return *this;

    if(size() != rhs.size()) {
      delete [] string_;
      string_ = new char[rhs.size() + 1];
    }

    ::strcpy(string_, rhs.string_);

    return *this;
  }

  const ABA_STRING& ABA_STRING::operator=(const char *rhs)
  {
    if(size() != ((int) strlen(rhs))) {
      delete [] string_;
      string_ = new char[strlen(rhs) + 1];
    }

    ::strcpy(string_, rhs);

    return *this;
  }

  int operator==(const ABA_STRING &lhs, const ABA_STRING &rhs)
  {
    return strcmp(lhs.string_, rhs.string_) ? 0 : 1;
  }

  int operator==(const ABA_STRING &lhs, const char *rhs)
  {
    return strcmp(lhs.string_, rhs) ? 0 : 1;
  }

  int operator!=(const ABA_STRING &lhs, const ABA_STRING &rhs)
  {
    return strcmp(lhs.string_, rhs.string_) ? 1 : 0;
  }

  int operator!=(const ABA_STRING &lhs, const char *rhs)
  {
    return strcmp(lhs.string_, rhs) ? 1 : 0;
  }

  ostream& operator<<(ostream &out, const ABA_STRING &rhs)
  {
     return out << rhs.string_;
  }

  char& ABA_STRING::operator[](int i)
  {
#ifdef ABACUSSAFE
    rangeCheck(i);
#endif
    return string_[i];
  }

  const char& ABA_STRING::operator[](int i) const
  {
#ifdef ABACUSSAFE
    rangeCheck(i);
#endif
    return string_[i];
  }

  int ABA_STRING::size() const
  {
    return ::strlen(string_);
  }

  int ABA_STRING::ascii2int(int i) const
  {
    return atoi(string_ + i);
  }

  double ABA_STRING::ascii2double() const
  {
    return atof(string_);
  }

  unsigned int ABA_STRING::ascii2unsignedint() const
  {
    return (unsigned int) strtoul(string_, NULL, 10);
  }

  bool ABA_STRING::ascii2bool() const
  {
    if (!strcmp(string_, "true")) return true;
    if (!strcmp(string_, "false")) return false;
    else {
      glob_->err() << "ABA_STRING::ascii2bool(): string neither true nor false, ";
      glob_->err() << "but " << string_ << "." << endl;
      exit(Fatal);
      return false;  //!< to suppress compiler warnings
    }
  }

  bool ABA_STRING::ending(const char *end) const
  {
    int l1 = ::strlen(string_);
    int l2 = ::strlen(end);

    if (l1 < l2) return false;

    if (!strcmp(string_+l1-l2, end)) return true;
    else                             return false;
    
  }

  char *ABA_STRING::string() const
  {
    return string_;
  }
 
  void ABA_STRING::rangeCheck(int i) const
  {
    if (i < 0 || i >= size()) {
      glob_->err() << "ABA_STRING::operator[" << i << "]: not in valid range ";
      glob_->err() << "[0," << size() - 1 << "]" << endl;
      exit (Fatal);
    }
  }
