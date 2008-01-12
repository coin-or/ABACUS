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

#include "abacus/mtserver.h"
#include "abacus/parmaster.h"
#include "abacus/master.h"

#include <ace/SOCK_Stream.h>
#include <ace/INET_Addr.h>
 
  ABA_MTSERVER::ABA_MTSERVER(ABA_MASTER *master, int port, int nThreads) 
    : 
    master_(master),
    port_(port),
    nThreads_(nThreads)
  {
  }

  ABA_MTSERVER::~ABA_MTSERVER()
  {
  }

  int ABA_MTSERVER::open(void*)
  {
    ACE_INET_Addr addr(port_);

    if (acceptor_.open(addr, 1) == -1) {
      master_->err() << "ABA_MTSERVER::open(): listening on port " << port_
                     << " failed!" << endl;
      exit(Fatal);
    }

    activate(THR_NEW_LWP, nThreads_);

    return 0;
  }

  int ABA_MTSERVER::svc()
  {
    ACE_Time_Value timeout(1);
    while (ACE_Thread_Manager::instance()->testcancel(ACE_Thread::self())==0) {
      ABA_MESSAGE   msg;
      ACE_INET_Addr addr;
      int ret = acceptor_.accept(msg.stream(), &addr, &timeout);
      if (ret==0)
        if (!svcMessage(msg))
          break;
    }

    return 0;
  }

  bool ABA\_MTSERVER::svcMessage(ABA\_MESSAGE\& msg)
