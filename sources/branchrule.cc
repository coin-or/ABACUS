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

#include "abacus/branchrule.h"
#include "abacus/master.h"


ABA_BRANCHRULE::ABA_BRANCHRULE(ABA_MASTER *master) 
  : master_(master)
  { }

ABA_BRANCHRULE::~ABA_BRANCHRULE()
  { }


bool ABA_BRANCHRULE::branchOnSetVar()
  {
    return false;
  }
  
void ABA_BRANCHRULE::extract(ABA_LPSUB *lp)
  {
    master_->err() << "WARNING:" << endl;
    master_->err() << "  virtual dummy function extract() of base class ";
    master_->err() << "ABA_BRANCHRULE is called." << endl;
    master_->err() << "  Redefinition in derived class is missing." << endl;
  }

void ABA_BRANCHRULE::unExtract(ABA_LPSUB *lp)
  {
    master_->err() << "WARNING:" << endl;
    master_->err() << "  virtual dummy function unExtract() of base class ";
    master_->err() << "ABA_BRANCHRULE is called." << endl;
    master_->err() << "  Redefinition in derived class is missing." << endl;
  }

void ABA_BRANCHRULE::initialize(ABA_SUB* sub)
  {}

