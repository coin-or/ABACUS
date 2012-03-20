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

#include "abacus/subserver.h"
#include "abacus/master.h"
#include "abacus/parmaster.h"
#include "abacus/sub.h"
#include "abacus/opensub.h"
#include "abacus/debug.h"

  bool ABA_SUBSERVER::svcMessage(ABA_MESSAGE& msg)
  {
    if (debug(DEBUG_SUBSERVER))
      master_->out() << "DEBUG_SUBSERVER: receiving a subproblem..." << endl;

    msg.receive();

    ABA_SUB* sub = master_->unpackSub(msg);

    master_->openSub()->insert(sub);

    if (debug(DEBUG_SUBSERVER)) {
      master_->out() << "DEBUG_SUBSERVER: subproblem ";
      master_->parmaster()->printId(sub->id());
      master_->out() << " received and inserted into openSub list." << endl;
    }

    return true;
  }
