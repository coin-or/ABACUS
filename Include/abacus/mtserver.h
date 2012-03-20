/*!\file
 * \author Matthias Elf
 * \brief multithreaded server.
 *
 * The class ABA_MTSERVER is an abstract base class of a multithreaded server,
 * which listens on a port for messages of type ABA_MESSAGE. The pure
 * virtual function \a svcMessage must be defined in a subclass.
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


#ifndef ABA_MTSERVER_H
#define ABA_MTSERVER_H

#include "abacus/abacusroot.h"
#include "abacus/message.h"

#include <ace/Task.h>
#include <ace/SOCK_Acceptor.h>

class ABA_MASTER;


  class  ABA_MTSERVER :  public ACE_Task<ACE_MT_SYNCH>, public ABA_ABACUSROOT  {
    public/

/*! The constructor.
 *
 *        \param master A pointer to the master object.
 *        \param port The port number on which the server is listening.
 *        \param nThreads The number of threads.
 */ 
      ABA_MTSERVER(ABA_MASTER *master, int port, int nThreads);

//! The destructor.
      ~ABA_MTSERVER();

/*!  This function begins listening for connections on \a port_. 
 *   \a nThreads_ new threads are started
 *   which all will all accept connections on this port.
 */
      int open (void*);
      int close (u_long);
 
/*! The entry point of all threads.
 */
      int svc();

    protected:
      ABA_MASTER *master_;
      int         port_;
      int         nThreads_;
      ACE_SOCK_Acceptor acceptor_;

/*! \brief The pure virtual function \a svcMessage
 *  has to be defined in a subclass.
 * 
 *   This function must receive the message and service the connection. When
 *   this function returns the connection will be closed.
 */
      virtual bool svcMessage(ABA_MESSAGE& msg) = 0;
  };


inline int ABA_MTSERVER::close(u_long) { return 0; }

#endif  // ABA_MTSERVER_H
/*! \class ABA_MTSERVER
 *  \brief class ABA_MTSERVER is an abstract base class of a multithreaded server,
 * which listens on a port for messages of type ABA_MESSAGE. The pure
 * virtual function \a svcMessage must be defined in a subclass.
 */

