/*!\file
 * \author Matthias Elf
 * \brief debug levels.
 *
 * The header file debug.h defines some debug level masks. The current
 * debug level is stored in the global variable \a debugLevel_.
 * It is a bitwise or combination of the debug level masks.
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

#ifndef ABA_DEBUG_H
#define ABA_DEBUG_H
#define DEBUG_MESSAGE_CONVAR      1
#define DEBUG_MESSAGE_SUB         2
#define DEBUG_NOTIFICATION        4
#define DEBUG_BALANCER            8
#define DEBUG_TERMINATION         16
#define DEBUG_SOCKET              32
#define DEBUG_SEPARATE            64
#define DEBUG_SUBSERVER           128
extern int debugLevel_;

/*! The macro \a debug
 *  evaluates to true if at least one bit of the requested
 *  debug level is set in the global debug level.
 */
#define debug(level) (debugLevel_ & (level))

#endif	// !ABA_DEBUG_H
