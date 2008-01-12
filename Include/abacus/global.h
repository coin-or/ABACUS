/*!\file
 * \author Matthias Elf
 * \brief global.
 *
 * This class stores global data (e.g., a zero tolerance,
 * an output stream, a table with system parameters)
 * und functions operating with this data. For each application
 * there is usually one global object and almost every object
 * in this system has a pointer to an associated global object
 * or a pointer to an object of a class derived from ABA_GLOBAL
 * (e.g., ABA_MASTER).
 *
 * \par
 * Like the class ABA_ABACUSROOT, the class ABA_GLOBAL helps us to avoid
 * names with global scope.
 *
 * \par
 * We assume that a set of parameters is associated with every application.
 * These parameters can either be given by \ABACUS system classes or can
 * be user defined. The class ABA_GLOBAL contains a table for the storage
 * of the literal values of the parameters. Moreover, functions are
 * provided for assigning the literal valued parameters to specific types.
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


#ifndef ABA_GLOBAL_H
#define ABA_GLOBAL_H

#include "abacus/abacusroot.h"
#include "abacus/ostream.h"
#include "abacus/hash.h"
#include "abacus/string.h"

  class  ABA_GLOBAL :  public ABA_ABACUSROOT  { 
    public:

/*! \brief The constructor 
 *  initializes our filtered output and error stream
 *   with the standard output stream \a cout  and the standard error stream
 *   \a cerr.
 *
 *   \param eps The zero-tolerance used within all member functions of
 *                    objects which have a pointer to this global object
 *                    (default value \a 1.0e-4).
 *   \param machineEps The machine dependent zero tolerance
 *                            (default value \a 1.0e-7).
 *   \param infinity All values greater than \a infinity are
 *                         regarded as ``infinite big'', all values less than
 *                         \a -infinity are regarded as ``infinite small''
 *                         (default value \a 1.0e32).
 *                         Please note that this value might be different from 
 *                         the value the LP-solver uses internally.
 *                         You should make sure that the value used here is 
 *                         always greater than or equal to the value used 
 *                         by the solver.
 */
      ABA_GLOBAL(double eps = 1.0e-4, double machineEps = 1.0e-7,
                 double infinity = 1.0e32);

//! The destructor.
      virtual ~ABA_GLOBAL();

/*! The output operator writes some of the data members to an ouput stream.
 *
 *  \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param rhs The object being output.
 */
      friend ostream &operator<<(ostream &out, const ABA_GLOBAL &rhs);

/*! \brief Returns a reference to the output stream
 *   associated with this global object after writing \a nTab (default value 0)
 *   tabulators on this stream. This tabulator is not the normal
 *   tabulator but consists of four blanks.
 *
 *  \return A reference to the global output stream.
 *
 *   \param nTab The number of tabulators which should be written to the
 *                   global output stream. The default value is 0.
 */
      virtual ABA_OSTREAM& out(int nTab = 0);

/*! \brief Behaves like the function \a out() except that
 *   the global error stream is used instead of the global output stream.
 *
 *   \return A reference to the global error stream.
 *
 *   \param nTab The number of tabulators which should be written to the
 *                   global error stream. The default value is 0.
 */
      virtual ABA_OSTREAM& err(int nTab = 0);

/*! \return The zero tolerance.
 */
      double eps() const;

/*! \brief This version of the function \a eps()
 *  sets the zero tolerance.
 *
 *   \param e The new value of the zero tolerance.
 */
      void eps(double e);

/*! Provides a machine dependent zero tolerance.
 *
 *   The machine dependent zero tolerance is used, e.g., to test if
 *   a floating point value is 0.
 *   This value is usually less than \a eps(), which provides, e.g.,
 *   a safety tolerance if a constraint is violated.
 *
 *   \return The machine dependent zero tolerance.
 */
      double machineEps() const;

/*! \brief This version of the function \a machineEps()
 *  sets the machine dependent zero tolerance.
 *
 *   \param e The new value of the machine dependent zero tolerance.
 */
      void machineEps(double e);

/*! \brief Provides a floating point value of 
 *   ``infinite'' size. Especially, we assume that \a -infinity() is
 *   the lower and \a infinity() is the upper bound of an unbounded variable
 *   in the linear program. 
 *
 *   \return A very large floating point number. The default value of \a infinity() is
 *   \a 1.0e32.
 */
      double infinity() const;

/*! \brief This version of the function \a infinity()
 *  sets the ``infinite value''.
 *  Please note that this value might be different from the value the LP-solver 
 *  uses internally.
 *  You should make sure that the value used here is always greater than or
 *  equal to the value used by the solver.
 *  
 *   \param x The new value representing ``infinity''.
 */
      void infinity(double x);

/*! \return true If \a x is regarded as ``infinite'' large,
 *   \return false otherwise.
 *
 *   \param x The value compared with ``infinity''.
 */
      bool isInfinity(double x) const;

/*! \return true If \a x is regarded as infinite small;}
 *   \return false otherwise.
 *
 *   \param x The value compared with ``minus infinity''.
 */
      bool isMinusInfinity(double x) const;

/*! \return true If the absolute difference of \a x and \a y is less than
 *                   the machine dependent zero tolerance,
 *   \return false otherwise.
 *
 *   \param x The first value being compared.
 *   \param y The second value being compared.
 */
      bool equal(double x, double y) const;

/*! \return true If the value \a x differs at most by the machine
 *                   dependent zero tolerance from an integer value,
 *   \return false otherwise.
 */
      bool isInteger(double x) const;

/*! \return true If the value \a x differs at most by \a eps
 *                   from an integer value,
 *   \return false otherwise.
 */
      bool isInteger(double x, double eps) const;

/*! \brief Displays the string {\tt ENTER>}
 *   on the global output stream and waits for a character on
 *   the input stream \a in, e.g., a keystroke if \a in == cin.
 *
 *  \return The character read from the input stream.
 *
 *   \param in The input stream the character should be read from.
 */
      virtual char enter(istream &in);

/*! \brief Opens the parameter file \a fileName,
 *   reads all parameters, and inserts them in the parameter table.
 *
 *   A parameter file may have at most 1024 characters per line.
 *
 *   \param fileName The name of the parameter file.
 */
      void readParameters(const char *fileName);

/*! Inserts a parameter in the parameter table.
 *
 *  If the parameter already is in the table, the value is overwritten.
 *
 *   \param name The name of the parameter.
 *   \param value The literal value of the parameter.
 */
      void insertParameter(const char *name, const char *value);

/*! Searches for the parameter \a name in the parameter table. 
 *
 *   This function is overloaded for different types of the argument \a parameter.
 *   See also the functions \a assignParameter and \a findParameter with 
 *   enhanced functionality. 
 *
 *   \return 0 If the parameter is found,
 *   \return 1 otherwise.
 *
 *   \param name The name of the parameter.
 *   \param parameter The variable \a parameter receives the value of the
 *                       parameter, if the function returns 1, otherwise it
 *                       is undefined.
 *
 */
      int  getParameter(const char *name, int &param);
      int  getParameter(const char *name, unsigned int &param); 
      int  getParameter(const char *name, double &param);
      int  getParameter(const char *name, ABA_STRING &param);
      int  getParameter(const char *name, bool &param);
      int  getParameter(const char *name, char &param);

/*! Searches for the parameter \a name in the parameter table.
 *  
 *   If no parameter \a name is found and no default value
 *   of the parameter is given, the program terminates with an error messages.
 *   The program terminates also with an error message if the value of a 
 *   parameter is not within a specified feasible region.
 *   Depending on the type of the parameter, a feasible region can be an 
 *   interval (specified by \a minVal and \a maxVal) or can be given by
 *   a set of feasible settings (given by a number \a nFeasible and a pointer
 *   \a feasible to the feasible values. 
 *
 *  \par
 *   This function is overloaded in two ways. First, this function is defined 
 *   for different types of  the argument \a parameter,  second, for each such 
 *   type we have both versions, with and without a default value of
 *   the parameter.
 *
 *   \param param The variable \a parameter receives the value of the
 *                       parameter.
 *   \param name The name of the parameter.
 *   \param minVal The  value of the parameter is considered as infeasible
 *                      if it is less than \a minVal.
 *   \param maxVal The  value of the parameter is considered as infeasible
 *                      if it is larger than \a maxVal.
 */
      void  assignParameter(int &param, const char *name, 
                            int minVal, int maxVal);

//! See \ref{ABA_GLOBAL::assignParameter} for description.
      void  assignParameter(unsigned &param, const char *name, 
                            unsigned minVal,unsigned maxVal);

//! See \ref{ABA_GLOBAL::assignParameter} for description.
      void  assignParameter(double &param, const char *name, 
                            double minVal, double maxVal);

//! See \ref{ABA_GLOBAL::assignParameter} for description.
      void  assignParameter(bool &param, const char *name);

/*! See \ref{ABA_GLOBAL::assignParameter} for description.
 *
 *   \param param The variable \a parameter receives the value of the
 *                       parameter.
 *   \param name The name of the parameter.
 *   \param nFeasible The number of feasible settings. If \a nFeasible is
 *                        equal to zero, then all values are allowed. 0
 *                        is the default value.
 *   \param feasible If \a nFeasible is greater zero, the this are
 *        the settings for the parameter to be considered as
 *                         feasible. Must be an array of \a nFeasible strings.
 */
      void  assignParameter(ABA_STRING &param, const char *name, 
                            unsigned nFeasible=0, const char *feasible[]=0);
/*! See \ref{ABA_GLOBAL::assignParameter} for description.
 *
 *   \param param The variable \a param receives the value of the
 *                       parameter.
 *   \param name The name of the parameter.
 *   \param feasible A string consisting of all feasible characters. 
 *            If \a feasible is zero, then all characters are allowed.
 */
      void  assignParameter(char &param, const char *name, 
                            const char *feasible=0);

/*! See \ref{ABA_GLOBAL::assignParameter} for description.
 *
 *   \param parameter The variable \a parameter receives the value of the
 *                       parameter.
 *   \param name The name of the parameter.
 *   \param minVal The  value of the parameter is considered as infeasible
 *                      if it is less than \a minVal.
 *   \param maxVal The  value of the parameter is considered as infeasible
 *                      if it is larger than \a maxVal.
 *   \param defVal  The default value that is used when the paramter is 
 *                        not found in the parameter table.
 */
      void  assignParameter(int &param, const char *name, 
                            int minVal, int maxVal, int defVal);

//! See \ref{ABA_GLOBAL::assignParameter} for description.
      void  assignParameter(unsigned &param, const char *name, 
                            unsigned minVal,unsigned maxVal, unsigned defVal);

//! See \ref{ABA_GLOBAL::assignParameter} for description.
      void  assignParameter(double &param, const char *name, 
                            double minVal, double maxVal, double defVal);

//! See \ref{ABA_GLOBAL::assignParameter} for description.
      void  assignParameter(bool &param, const char *name, bool defVal);

/*! See \ref{ABA_GLOBAL::assignParameter} for description.
 *
 *   \param parameter The variable \a parameter receives the value of the
 *                       parameter.
 *   \param name The name of the parameter.
 *   \param nFeasible The number of feasible settings. If \a nFeasible is
 *                        equal to zero, then all settings are allowed.
 *   \param feasible The settings for the parameter to be considered as
 *                         feasible. Must be an array of \a nFeasible strings.
 *   \param defVal The default value that is used when the paramter is 
 *                        not found in the parameter table.
 */
      void  assignParameter(ABA_STRING &param, const char *name, 
                            unsigned nFeasible, const char *feasible[], 
                            const char *defVal);

/*! See \ref{ABA_GLOBAL::assignParameter} for description.
 *
 *   \param param The variable \a param receives the value of the
 *                       parameter.
 *   \param name The name of the parameter.
 *   \param feasible A string containing all feasible settings. 
 *    If \a feasible is zero, then all settings are allowed.  
 *     \param defVal The default value that is used when the paramter is 
 *                        not found in the parameter table.
 */
      void  assignParameter(char &param, const char *name, 
                            const char *feasible, char defVal);

/*! Searches for the parameter \a name in the parameter table.
 *
 *   If no parameter \a name is found 
 *   the program terminates with an error messages.
 *   The program terminates also with an error message if the value of a 
 *   parameter is not within a given list of  feasible settings.
 *   This function is overloaded and can be used for different types of 
 *   parameters such as integer valued, char valued and string parameters.
 *
 *   \return The index of the matched feasible setting.
 *
 *   \param name The name of the parameter.
 *   \param nFeasible The number of feasible settings.
 *   \param feasible The settings for the parameter to be considered as
 *                         feasible. Must be an array of \a nFeasible strings.
 *
 */
      int findParameter(const char *name,
                        unsigned nFeasible, const int *feasible);

//! See \ref{ABA_GLOBAL::findParameter} for description.
      int findParameter(const char *name,
                        unsigned nFeasible, const char *feasible[]);

//! See \ref{ABA_GLOBAL::findParameter} for description.
      int findParameter(const char *name,const char *feasible);
                                                        
    private: 

      /*! The global output stream.
       */
      /*mutable*/ ABA_OSTREAM   out_;

      /*! The global error stream.
       */
      /*mutable*/ ABA_OSTREAM   err_;

      /*! A zero tolerance.
       */
      double eps_;

      /*! \brief The machine dependent zero tolerance, which
       *  is used to , e.g., to test if a floating point value is 0.
       *
       *  This value is usually less than \a eps_, which represents, e.g.,
       *  a safety tolerance if a constraint is violated.
       */
      double machineEps_;

      /*! An ``infinite'' big number.
       */
      double infinity_;

      /*! A string used as tabulator in the functions \a out() and \a err().
       */
      char  *tab_;
      ABA_HASH<ABA_STRING, ABA_STRING> paramTable_;
      ABA_GLOBAL(const ABA_GLOBAL &rhs);
      const ABA_GLOBAL &operator=(const ABA_GLOBAL &rhs);
  };


inline double ABA_GLOBAL::eps() const
  {
    return eps_;
  }

inline void ABA_GLOBAL::eps(double e)
  {
    eps_ = e;
  }

inline double ABA_GLOBAL::machineEps() const
  {
    return  machineEps_;
  }

inline void ABA_GLOBAL::machineEps(double e)
  {
    machineEps_ = e;
  }

inline double ABA_GLOBAL::infinity() const
  {
    return infinity_;
  }

inline void ABA_GLOBAL::infinity(double x)
  {
    infinity_ = x;
  }

inline bool ABA_GLOBAL::isInfinity(double x) const
  {
#ifdef ABACUS_NO_BOOL
    if (x >= infinity_) return true;
    else                return false;
#else
    return x >= infinity_;
#endif
  }

inline bool ABA_GLOBAL::isMinusInfinity(double x) const
  {
#ifdef ABACUS_NO_BOOL
    if (x <= -infinity_) return true;
    else                 return false;
#else
    return x <= -infinity_;
#endif
  }

inline bool ABA_GLOBAL::equal(double x, double y) const
  {
    if (fabs(x-y) < machineEps_) return true;
    else                         return false;
  }

inline bool ABA_GLOBAL::isInteger(double x) const
  {
    return isInteger(x, machineEps_);
  }


#endif  // ABA_GLOBAL_H


/*! \class ABA_GLOBAL
 *  \brief class stores global data (e.g., a zero tolerance,
 * an output stream, a table with system parameters)
 * und functions operating with this data.
 */
