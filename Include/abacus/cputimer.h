/*!\file
 * \author Matthias Elf
 * \brief cpu timer.
 *
 * This class derived from ABA_TIMER implements a timer measuring
 * the cpu time of parts of a program.
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

#ifndef ABA_CPUTIMER_H
#define ABA_CPUTIMER_H

#include "abacus/timer.h"

  class  ABA_CPUTIMER : public ABA_TIMER  {
    public:

/*! \brief After the application of the constructor the timer is not running,
 *  i.e., to measure time it has to be started explicitly.
 *
 *  \param glob A pointer to a global object.
 */
      ABA_CPUTIMER(ABA_GLOBAL *glob);
   
/*! This constructor initializes the total time of the timer.
 *
 *  The timer is not running, too.
 *
 *  \param glob A pointer to a global object.
 *  \param centiSeconds The intial value of the total time in
 *                          \f$1\over 100\f$ seconds.
 */
      ABA_CPUTIMER(ABA_GLOBAL *glob, long centiSeconds);

//! The destructor.
      virtual ~ABA_CPUTIMER();

    private:
      static long clk_tck_;

/*!  Returns the used cpu time in \f$1\over100\f$ seconds since the start of the program.
 *
 *   This function redefines the pure virtual function of the base
 *   class ABA_TIMER.
 *
 * \par
 *   Since \a CLOCKS_PER_SEC can be 1000000 
 *   the standard library function \a clock() returns negative values
 *   after about
 *   than 35 minutes. Hence we measure the cpu time with the function
 *   \a times which is common on \UNIX/ systems, although not defined
 *   in the \CEE/-ANSI-standard.
 *
*/
      virtual long theTime() const;
  };
#endif  // ABA_CPUTIMER_H

/*! \class ABA_CPUTIMER
 * \brief This class derived from ABA_TIMER implements a timer measuring
 * the cpu time of parts of a program.
 *
 */  
