/*!\file
 * \author Matthias Elf
 * \brief the lp master.
 *
 * The class ABA_LPMASTER is an abstract base class. A LP solver
 * specific master class has to be derived from this class.
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

#ifndef ABA_LPMASTER_H
#define ABA_LPMASTER_H

#include "abacus/cputimer.h"

class ABA_MASTER;

  class  ABA_LPMASTER :  public ABA_ABACUSROOT  {
    public:
      ABA_LPMASTER(ABA_MASTER *master) : master_(master) {}
      virtual ~ABA_LPMASTER() {}
      virtual void initializeLpParameters() = 0;
      virtual void setDefaultLpParameters() = 0;
      virtual void printLpParameters() = 0;
      virtual void outputLpStatistics() = 0;

    protected:
      ABA_MASTER *master_;
  };
#endif  // ABA_LPMASTER_H

/*! \class ABA_LPMASTER
 *  \brief The class ABA_LPMASTER is an abstract base class. A LP solver
 * specific master class has to be derived from this class.
 */

