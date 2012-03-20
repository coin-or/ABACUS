/*!\file
 * \author Matthias Elf
 * \brief output stream.
 *
 * This class implements an output stream which can be turned on and off
 * at run time, i.e., if the output stream is turned off, then
 * no messages written by the operator \a << reach the associated ``real''
 * output stream. 
 * Moreover, the output is also optionally written to a log file.
 * The class is derived from the class \a ostream and reimplements the
 * \a << operator for all fundamental types, strings of the form
 * \a{const char *}, pointers to manipulators functions, and some classes
 * of \ABACUS.
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

#ifndef ABA_OSTREAM_H
#define ABA_OSTREAM_H
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

class ABA_OSTREAM;
ABA_OSTREAM& endl(ABA_OSTREAM &o);
ABA_OSTREAM& flush(ABA_OSTREAM &o);
using ::endl;

#include "abacus/abacusroot.h"
class ABA_STRING;
class ABA_HISTORY;
class ABA_TIMER;
class ABA_LP;
class ABA_LPVARSTAT;
class ABA_CSENSE;
typedef ABA_OSTREAM&(*ABA_OSTREAM_MANIP)(ABA_OSTREAM&);

class  ABA_OSTREAM:public ostream,public ABA_ABACUSROOT{
    public:

/*! The constructor turns the output on and associates it with a ``real'' stream.
 *
 *   \param out The ``real'' stream (usually \a cout or \a cerr.)}
 *   \param logStreamName If \a logStreamName is not 0, 
 *                            then the output also directed to a log-file
 *                            with this name. 
 *                            The default value of \a logStreamName is 0.
 */
      ABA_OSTREAM(ostream &out, const char *logStreamName = 0);

//! The destructor.
      ~ABA_OSTREAM();

/*! output operator \a << 
 *   \brief Reimplementation for all fundamental types, for
 *   \a const char *, and for some other classes listed below.
 *   
 *   If the output is turned on the
 *   operator of the base class \a ostream is called. If also the
 *   output to the logfile is turned on, we write the same message also
 *   to the log-file.  
 *
 *   return A reference to the output stream.
 *
 *   \param o The item being output.
 */
      ABA_OSTREAM&operator<<(char o);
      ABA_OSTREAM&operator<<(unsigned char o);
      ABA_OSTREAM&operator<<(signed char o);
      ABA_OSTREAM&operator<<(short o);
      ABA_OSTREAM&operator<<(unsigned short o);
      ABA_OSTREAM&operator<<(int o);
      ABA_OSTREAM&operator<<(unsigned int o);
      ABA_OSTREAM&operator<<(long o);
      ABA_OSTREAM&operator<<(unsigned long o);
      ABA_OSTREAM&operator<<(float o);
      ABA_OSTREAM&operator<<(double o);
      ABA_OSTREAM&operator<<(const char *o);
       ABA_OSTREAM& operator<<(ABA_OSTREAM& (*pf)(ABA_OSTREAM&)) { return pf( *this ); }

/*! \brief A manipulator
 *   is a function having as argument a reference to an
 *   ABA_OSTREAM and returning an ABA_OSTREAM. 
 *
 *   Manipulators are used that
 *   we can call, e.g., the function \a endl(o) by just writing its
 *   name omitting brackets and the function argument.
 *
 *     \return A reference to the output stream.
 *
 *     \param m An output stream manipulator.
 */
      ABA_OSTREAM&operator<<(const ABA_STRING &o);
      ABA_OSTREAM&operator<<(const ABA_TIMER &o);
      ABA_OSTREAM&operator<<(const ABA_HISTORY &o);
      ABA_OSTREAM&operator<<(const ABA_LPVARSTAT &o);
      ABA_OSTREAM&operator<<(const ABA_CSENSE &o);
      ABA_OSTREAM&operator<<(const ABA_LP &o);

/*! Turns the output off.
 */
      void off();

/*! Turns the output on.
 */
      void on();

/*! Turns the output to the logfile on.
 */
      void logOn();

/*! \brief This version of \a logOn()
 *   turns the output to the logfile on and sets the log-file to \a logStreamName.
 *
 *   \param logStreamName The name of the log-file.
 */
      void logOn(const char *logStreamName);

/*! Turns the output to the logfile off.
 */
      void logOff();

/*! \return true If the output is turned on,
 *   \return false otherwise.
 */
      bool isOn() const;

/*! \return true If the output to the logfile is turned on,
 *   \return false otherwise.
 */
      bool isLogOn() const;

/*! \return A pointer to the stream associated with the log-file.
 */
      ofstream* log() const;


/*! \brief Can be used to set the format flags
 *   of the output stream and the log file similar to the function 
 *   \a ios::set() of the iostream library.
 *
 *   For a documentation of all
 *   possible flags we refer to the documentation of the GNU \CPLUSPLUS/
 *   iostream Library.
 *
 *   \param flag The flag being set.
 */
      void setFormatFlag(fmtflags flag);

/*! \brief Flushes the output and the log stream buffers
 *   of the stream \a o. This function can be called via the
 *   manipulator \a o << flush;.
 *
 *   \return A reference to the output stream.
 *
 *   \param o An output stream.
 */ 
      friend ABA_OSTREAM& flush(ABA_OSTREAM &o);

/*! Writes an end of line to the output and log-file of the stream \a o and flushes both stream buffers.
 *
 *   This function can be called via the manipulator \a o << endl;.
 *
 *   \return A reference to the output stream.
 *
 *   \param o An output stream.
 */ 
      friend ABA_OSTREAM& endl(ABA_OSTREAM &o);

/*! Sets the width of the field for the next output operation on the log and the output stream.
 *
 *   In most cases the manipulator \a setWith is more convenient to use.
 *
 *   \return A reference to the output stream.
 *
 *   \param o An output stream.
 *   \param w The width of the field.
 */
      friend ABA_OSTREAM& _setWidth(ABA_OSTREAM &o, int w);

/*! Sets the precision for the output stream.
 *
 *   In most cases the manipulator \a setPrecision is more convenient to use.
 *
 *   \return A reference to the output stream.
 *
 *   \param o An output stream.
 *   \param p The precision.
 */
      friend ABA_OSTREAM& _setPrecision(ABA_OSTREAM &o, int p);
      
    private:

      /*! \brief The ``real'' stream associated with our output stream 
       *  (usually \a cout or \a cerr).
       */
      ostream  &out_;

      /*! \brief If \a true, then output is written to the stream \a out_,
       *         otherwise it is suppressed.
       */
      bool      on_;

      /*! \brief If \a true_, then output is also written to the
       *         log stream \a *log.
       */
      bool      logOn_;

      /*! A pointer to a stream associated with the log file.
       */
      ofstream *log_;
  };

//! the manipulator class for manipulators with one argument
/*! The following class is adapted from [Str93] for the class ABA_OSTREAM.
 */
  class ABA_OSTREAM_MANIP_INT {
    public:

/*! \brief The Constructor for a manipulator
 *   of the class ABA_OSTREAM with an integer argument.
 *
 *   \param f A pointer to the manipulating function.
 *   \param i The argument of the manipulating function.
 */
      ABA_OSTREAM_MANIP_INT(ABA_OSTREAM &(*f)(ABA_OSTREAM &o, int i), int i);

/*! The output operator of the maninpulator class calls the manipulating function.
 *
 *   \return A pointer to the ouput stream.
 *
 *   \param o An output stream}
 *   \param m A manipulator object.
 */
      friend ABA_OSTREAM &operator<<(ABA_OSTREAM &o, const ABA_OSTREAM_MANIP_INT &m);

    private:
      int i_;
      ABA_OSTREAM &(*f_)(ABA_OSTREAM &o, int i);
  };

/*! @name 
 * manipulator functions with one argument
 * The following function provide us manipulator having the same functionality
 *  as the functions \a setw() and \a setprecision() for normal output streams.
 */
//@{
/*! \brief Can be used for output streams of the class 
 *   ABA_OSTREAM as the function \a setw for the class \a ostream, e.g.:
 *
 *   \a master\_->out() << setw(10) << x << endl;
 *
 *   \return A manipulator object with the function \a \_setWidth().
 *
 *   \param p The width of the output field.
 */
  ABA_OSTREAM_MANIP_INT setWidth(int w);

/*! \brief Can be used for output streams of the class 
 *   ABA_OSTREAM in the same way as the function \a setprecision 
 *   for the class \a ostream, e.g.:
 *
 *   \a master\_->out() << setprecision(10) << x << endl;
 *
 *   \return A manipulator object with the function \a \_setPrecision().
 *
 *   \param p The precision for the output stream.
 */
  ABA_OSTREAM_MANIP_INT setPrecision(int p);
//@}

#endif // ABA_OSTREAM_H

/*! \class ABA_OSTREAM
 * \brief Class implements an output stream which can be turned on and off
 * at run time, i.e., if the output stream is turned off, then
 * no messages written by the operator \a << reach the associated ``real''
 * output stream.
 */

