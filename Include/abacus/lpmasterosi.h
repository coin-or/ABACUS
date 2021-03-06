/*!\file
 * \author Frank Baumann
 * \brief the osi master.
 *
 *  An instance of the class ABA_LPMASTEROSI is used to store all
 *  OSI specific parameters and global data. The master instance of
 *  ABA_MASTER keeps a pointer to an instance of this class.
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

#ifndef ABA_LPMASTEROSI_H
#define ABA_LPMASTEROSI_H
#include "abacus/lpmaster.h"
#include "abacus/string.h"

  class  ABA_LPMASTEROSI :  public ABA_LPMASTER  {
    friend class ABA_OSIIF;
    public:

/*! The constructor.
 *
 *      \param master The master of the optimization.
*/
      ABA_LPMASTEROSI(ABA_MASTER *master);

      //! The destructor.
      virtual ~ABA_LPMASTEROSI();


/*! Initializes the LP solver specific Parameters.
*/
      virtual void initializeLpParameters();

/*! Sets default values of the LP solver specific Parameters.
*/
      virtual void setDefaultLpParameters();

/*! Prints the settings of the LP solver specific Parameters.
*/
      virtual void printLpParameters();

/*! Prints LP solver specific Statistics.
*/
      virtual void outputLpStatistics();

    private:
      
  };
#endif  // ABA_LPMASTEROSI_H
