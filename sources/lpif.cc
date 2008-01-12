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
 *
 * \par
 *   The file \a lpif.cc contains all code fragments of the basic
 *   \ABACUS\ library which depend on the preprocessor definition of the
 *   supported LP solvers. Only this file needs to be recompiled
 *   if the LP solver definitions change.
 *
 * \par
 *   Currently the following definitions are supported:
 *
 * \par
 *   For OSI:
 *   ABACUS_LP_OSI.
 *
 * $Id: lpif.cc,v 2.7 2007/07/23 15:58:48 baumann Exp $
 */

#include "abacus/master.h"
#include "abacus/sub.h"

#ifdef ABACUS_LP_OSI
#include "abacus/lpmasterosi.h"
#include "abacus/lpsubosi.h"
#endif

//! The function ABA_SUB::generateLp().

ABA_LPSUB *ABA_SUB::generateLp()
  {
    switch (master_->defaultLpSolver()) {
#ifdef ABACUS_LP_OSI
       case ABA_MASTER::Cbc:
       case ABA_MASTER::Clp:
       case ABA_MASTER::CPLEX:
       case ABA_MASTER::DyLP:
       case ABA_MASTER::FortMP:
       case ABA_MASTER::GLPK:
       case ABA_MASTER::MOSEK:
       case ABA_MASTER::OSL:
       case ABA_MASTER::SoPlex:
       case ABA_MASTER::SYMPHONY:
       case ABA_MASTER::Vol:
       case ABA_MASTER::XPRESS_MP:
	  return new ABA_LPSUBOSI(master_, this);
#endif
       default:
	  master_->err() << "Error: ABACUS library not compiled for ";
	  master_->err() << "selected LP-Solver "<< ABA_MASTER::OSISOLVER_[master_->defaultLpSolver()] << "." << endl;
	  exit(Fatal);
    }
    return 0;           //!< to supress compiler warning
  }


//! The function ABA_MASTER::_createLpMasters().

void ABA_MASTER::_createLpMasters()
{
#ifdef ABACUS_LP_OSI
  lpMasterOsi_ = new ABA_LPMASTEROSI(this);
#endif
}

//! The function ABA_MASTER::_deleteLpMasters().

void ABA_MASTER::_deleteLpMasters()
{
#ifdef ABACUS_LP_OSI
  delete lpMasterOsi_;
#endif
}

//! The function ABA_MASTER::_initializeLpParameters().

void ABA_MASTER::_initializeLpParameters()
{
#ifdef ABACUS_LP_OSI
  lpMasterOsi_->initializeLpParameters();
#endif
}

//! The function ABA_MASTER::_setDefaultLpParameters().

void ABA_MASTER::_setDefaultLpParameters()
{
#ifdef ABACUS_LP_OSI
  lpMasterOsi_->setDefaultLpParameters();
#endif
}

//! The function ABA_MASTER::_printLpParameters().

void ABA_MASTER::_printLpParameters()
{
#ifdef ABACUS_LP_OSI
  lpMasterOsi_->printLpParameters();
#endif
}

//! The function ABA_MASTER::_outputLpStatistics().

void ABA_MASTER::_outputLpStatistics()
{
#ifdef ABACUS_LP_OSI
  lpMasterOsi_->outputLpStatistics();
#endif
}

