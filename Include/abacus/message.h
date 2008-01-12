/*!\file
 * \author Matthias Elf
 * \brief point to point message.
 *
 * The ABA_MESSAGE class implements a message buffer for sending arbitrary
 * objects over a communication stream. It fully encapsulates the underlaying
 * XDR stream which provides machine independant data representation.
 * The information needed to recreate an object has to be packed in the
 * message buffer by \a pack() functions. Afterwards the message
 * has to be sent by \a send().
 * The receiver calls \a receive() and then \a unpack() to
 * obtain the data. We have chosen message buffers to provide a
 * mechanism for sending objects of derived subclasses and sending data of
 * variable size. Large objects are sent in message chunks.
 * The user has to provide the member functions \a pack() and \a unpack()
 * in derived classes as well as a constructor which constructs an object
 * of that class from a message buffer.
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

#ifndef ABA_MESSAGE_H
#define ABA_MESSAGE_H
#include "abacus/messagebase.h"
#include <ace/SOCK_Stream.h>
  class  ABA_MESSAGE : public ABA_MESSAGEBASE { 
    public: 

/*! The constructor. 
 *  
 *  A message buffer encapsulates a bidirectional communication
 *  stream. The underlaying \a ACE\_SOCK\_Stream has to be initialized later by
 *  using the member function |stream()|.
 */
      ABA_MESSAGE();

//! The destructor.
      ~ABA_MESSAGE();
      ACE_SOCK_Stream& stream();

    protected: 

/*! @name 
 *  Static helper functions to read/write data from/to the stream
 */
//@{
      static int readit(ABA_MESSAGE *msg, char *buf, int len);
      static int writeit(ABA_MESSAGE *msg, const char *buf, int len);
//@}
      ACE_SOCK_Stream  stream_;
  };

inline ACE_SOCK_Stream& ABA_MESSAGE::stream()
{
  return stream_;
}


#endif  // ABA_MESSAGE_H

/*! \class ABA_MESSAGE
 *  \brief  ABA_MESSAGE class implements a message buffer for sending arbitrary
 * objects over a communication stream. It fully encapsulates the underlaying
 * XDR stream which provides machine independant data representation.
 */

