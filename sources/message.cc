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

#include "abacus/message.h"

int ABA_MESSAGE::readit(ABA_MESSAGE *msg, char *buf, int len)
{
  //! receive and decode len1 (the length of the message)
  XDR xdr;
  int len1;
  char buf_len1[4];
  int r = msg->stream_.recv_n(buf_len1, 4, 0);
  if (r != 4) {
    ACE_ERROR((LM_ERROR, "%p\n", "Error in recv_n"));
    assert(0);
  }
  xdrmem_create(&xdr, buf_len1, 4, XDR_DECODE);
  xdr_int(&xdr, &len1);
  xdr_destroy(&xdr);

  assert(len1<=len);

  //! receive len1 bytes of data
  r = msg->stream_.recv_n(buf, len1, 0);
  if (r != len1) {
    ACE_ERROR((LM_ERROR, "%p\n", "Error in recv_n"));
    assert(0);
  }

  return r;
}

int ABA_MESSAGE::writeit(ABA_MESSAGE *msg, const char *buf, int len)
{
  //! encode and send the length
  char buf_len[4];
  XDR  xdr;
  xdrmem_create(&xdr, buf_len, 4, XDR_ENCODE);
  xdr_int(&xdr, &len);
  xdr_destroy(&xdr);
  int r = msg->stream_.send_n(buf_len, 4, 0);
  if (r != 4) {
    ACE_ERROR((LM_ERROR, "%p\n", "Error in send_n"));
    assert(0);
  }

  //! send the data
  r = msg->stream_.send_n(buf, len, 0);
  if (r != len) {
    ACE_ERROR((LM_ERROR, "%p\n", "Error in send_n"));
    assert(0);
  }

  return r;
}


  ABA_MESSAGE::ABA_MESSAGE()
  :
    ABA_MESSAGEBASE( (int (*)(void *, char *, int))readit,
                     (int (*)(void *, char *, int))writeit )
  {
  }

  ABA_MESSAGE::~ABA_MESSAGE()
  {
    stream_.close();
  }

