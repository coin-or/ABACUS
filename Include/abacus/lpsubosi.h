/*!\file
 * \author Frank Baumann
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
#ifndef ABA_LPSUBOSI_H
#define ABA_LPSUBOSI_H
#include "abacus/lpsub.h"
#include "abacus/osiif.h"
class ABA_MASTER;

  class  ABA_LPSUBOSI :  public ABA_LPSUB, public ABA_OSIIF  {
    public:

/*! \brief The constructor 
 *   calls the function \a initialize() of the base
 *   classABA_LPSUB, which sets up the
 *   linear program and passes the data to the LP-solver.
 *
 *     \param master A pointer to the corresponding master of the optimization.
 *     \param sub The subproblem of which the LP-relaxation is solved.
 */
      ABA_LPSUBOSI(ABA_MASTER *master, ABA_SUB *sub);

//! The destructor.
      virtual ~ABA_LPSUBOSI();
    private: 
      ABA_LPSUBOSI(const ABA_LPSUBOSI &rhs);
      const ABA_LPSUBOSI &operator=(const ABA_LPSUBOSI &rhs);
  };
#endif
