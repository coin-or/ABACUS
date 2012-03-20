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

#include "abacus/numvar.h"
#include "abacus/master.h"

  ABA_NUMVAR::ABA_NUMVAR(ABA_MASTER *master, 
                         const ABA_SUB *sub, 
                         int number, 
                         bool dynamic,
                         bool local, 
                         double obj,
                         double lBound, 
                         double uBound, 
                         ABA_VARTYPE::TYPE type) 
  :  
    ABA_VARIABLE(master, sub, dynamic, local, obj, lBound, uBound, type),  
    number_(number) 
  { }

  ABA_NUMVAR::~ABA_NUMVAR()
  { }



  ostream &operator<<(ostream &out, const ABA_NUMVAR &rhs)
  {
    return out << '(' << rhs.number_  << ')' << endl;
  }
