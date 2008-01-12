/*!\file
 * \author Matthias Elf
 * \brief wall-clock timer.
 *
 * This class derived from ABA_TIMER implements a timer measuring
 * the elpased time (clock-of-the-wall time) of parts of the program.
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

#ifndef ABA_COWTIMER_H
#define ABA_COWTIMER_H

#include "abacus/timer.h"
  class  ABA_COWTIMER :  public ABA_TIMER  {
    public:

/*! \brief After the application of the constructor 
 * the timer is not running, i.e., to measure time it has to be started explicitly.
 *
 *   We initialize base time with the current time, such that later we
 *   can convert the time to \f$1\over100\f$ seconds without arithmetic overflow.
 *   The function \a time() is defined in the standard \CEE/-library.
 *
 *   \param glob A pointer to a global object.
 */
      ABA_COWTIMER(ABA_GLOBAL *glob);

/*! This constructor initializes the total time of the timer.
 *   
 * The timer is not running, too.
 *
 *   \param glob A pointer to a global object.
 *   \param centiSeconds The initial value of the timer in \f$1\over100\f$ seconds.
 */
      ABA_COWTIMER(ABA_GLOBAL *glob, long secs);

/*! The destructor.
*/
      virtual ~ABA_COWTIMER();

    private:

/*! \brief Returns the wall clock time since the initialization
 *  of the timer in \f$1\over100\f$ seconds.
 *   
 * This function redefines the pure virtual function of the base class ABA_TIMER.
 *
 * \par
 *   The function \a theTime() uses the function \a times(), which returns
 *   the elapsed real time in clock ticks.
 *
 */
      virtual long theTime() const;

      /*! \brief Stores the result of a call to the function 
       *         \a time(NULL) at construction time. 
       *
       *  We require this member such that we can return the time
       *  in centiseconds correctly in the function \a theTime(). Otherwise,
       *  an arithmetic overflow can occur.
       */
      long baseTime_;
  };
#endif  // ABA_COWTIMER_H

/*! \class ABA_COWTIMER
 *  \brief class derived from ABA_TIMER implements a timer measuring
 * the elpased time (clock-of-the-wall time) of parts of the program.
 */

