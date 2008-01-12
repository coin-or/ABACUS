/*!\file
 * \author Matthias Elf
 * subproblem server.
 *
 * The class ABA_SUBSERVER is a multithreaded server for solving subproblems.
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


#ifndef ABA_SUBSERVER_H
#define ABA_SUBSERVER_H

#include "abacus/mtserver.h"


  class  ABA_SUBSERVER :  public ABA_MTSERVER  {
    public:
      ABA_SUBSERVER(ABA_MASTER *master, int port, int nThreads);

    protected:

//! Receives a subproblem and solves it.
      virtual bool svcMessage(ABA_MESSAGE& msg);
  };


inline ABA_SUBSERVER::ABA_SUBSERVER(ABA_MASTER *master, int port, int nThreads)
:
  ABA_MTSERVER(master, port, nThreads)
{}


#endif  // ABA_SUBSERVER_H
/*! \class ABA_SUBSERVER
 *  \brief class ABA_SUBSERVER is a multithreaded server for solving subproblems
 */

