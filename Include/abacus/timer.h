/*!\file
 * \author Matthias Elf
 * \brief timer.
 *
 *  This class implements a base class for timers measuring the CPU time
 *  (class ABA_CPUTIMER) and the wall-clock time (class ABA_COWTIMER).
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

#ifndef ABA_TIMER_H
#define ABA_TIMER_H
#include <iostream>
using namespace std;

#include "abacus/abacusroot.h"


class ABA_STRING;
class ABA_GLOBAL;

  class  ABA_TIMER :  public ABA_ABACUSROOT  {
    public:

/*! \brief The constructor
 *   for a timer with a pointer to the global object \a glob.
 *
 *   After the application of the constructor the timer is not running,
 *   i.e., to measure time it has to be started explicitly.
 */ 
      ABA_TIMER(ABA_GLOBAL *glob);

/*! \brief This constructor
 *   initializes the total time of the timer
 *   with \a centiSeconds and
 *   the pointer to the corresponding global object \a glob.
 *   The timer is not running, too.
 */
      ABA_TIMER(ABA_GLOBAL *glob, long centiSeconds);

//! The destructor.
      virtual ~ABA_TIMER();

/*! \brief The output operator
 *   writes the time in the format
 *   {\tt hours:minutes:seconds.seconds/100} on an output stream.
 *
 *   After the time has been divided in hours, minutes and seconds
 *   we have to take care that an additional leading zero is output
 *   if minutes or seconds have a value less than ten.
 *
 *   \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param rhs The timer being output.
 */
      friend ostream& operator<<(ostream& out, const ABA_TIMER& rhs);

/*! \brief The timer is started with the function \a start().
 *   For safety starting a running timer is an error.
 *
 *   \param reset If this flag is set to true, the timer is reset before
 *                   it is started (default=false)}
 */
      void start(bool reset = false);

/*! \brief Stops the timer and adds the difference between
 *   the current time and the starting time to the total time.
 *
 *   Stopping a non-running timer is an error.
 */
      void stop();

/*! Stops the timer and sets the \a totalTime to 0.
 */
      void reset();

/*! \return true If the timer is running,
 *   \return false otherwise.
 */
      bool running() const;

/*! \return The currently
 *   spent time in  \f$1\over100\f$-seconds. It is
 *   not necessary to stop the timer to get the correct time.
 */
      long centiSeconds() const;

/*! \return The currently spent time in seconds. It is
 *   not necessary to stop the timer to get the correct time.
 *   The result is rounded down to the next integer value.
 */
      long seconds() const;

/*! \return The currently spent time in minutes. It is
 *   not necessary to stop the timer to get the correct time.
 *   The result is rounded down to the next integer value.
 */
      long minutes() const;

/*! \return The currently spent time in hours. It is
 *   not necessary to stop the timer to get the correct time.
 *   The result is rounded down to the next integer value.
 */
      long hours() const;

/*! \return true If the currently spent time exceeds \a maxTime,
 *   \return false otherwise.
 *
 *   \param maxTime A string of the form
 *     \a [[h:]m:]s, where \a h are the hours, \a m the minutes, and \a s the
 *     seconds. Hours and minutes are optional. \a h can be an arbitrary
 *     nonnegative integer, \a s and \a m have to be integers in \f$\{0,\dots,59\}\f$.
 *     If \a m or \a s are less than 10, then a leading 0 is allowed
 *     (e.g. 3:05:09).
 */
      bool exceeds(const ABA_STRING &maxTime) const;

/*! \param centiSeconds The number of centiseconds to be added.
 */
      void addCentiSeconds(long centiSeconds);

    protected:

/*!  \brief Is required for measuring the time difference
 *   between the time of the call and some base point (e.g., the program
 *   start).
 *
 *   It is a pure virtual function because in derived classes
 *   different implementation for elapsed time and CPU time are required.
 *
 *   \return The time since some base point (e.g., the program start for the
 *      cpu time) in \f$1\over100\f$ seconds.
 */


/*! We measure time according to the following principle.
 *
 *  The pure virtual function \a theTime() returns the CPU time
 *  since the start of the program for the class CPUABA_TIMER
 *  or the elapsed time since some point in the past for the class
 *  ABA_COWTIMER. When
 *  the timer is started \a startTime_ is initialized with the a value
 *  returned by \a theTime() and when it is stopped the difference
 *  between \a theTime() and \a startTime_ is added to the total time.
 */
      virtual long theTime() const = 0;

      /*! A pointer to the corresponding global object.
       */
      ABA_GLOBAL *glob_;

    private:

      /*! The start time of the timer in \f$1\over100\f$-seconds.
       */
      long startTime_;

      /*! The total time in \f$1\over100\f$-seconds.
       */
      long totalTime_;

      /*! \a true, if the timer is running.
       */
      bool running_;
  };

#endif  // ABA_TIMER_H


/*! \class ABA_TIMER
 *  \brief class implements a base class for timers measuring the CPU time and the wall-clock time 
 */
