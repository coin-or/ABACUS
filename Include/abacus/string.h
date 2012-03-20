/*!\file
 * \author Matthias Elf
 * \brief string.
 *
 * The class ABA_STRING implements are very simple class for the
 * representation of character strings.
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

#ifndef ABA_STRING_H
#define ABA_STRING_H
#include <iostream>
using namespace std;

#include "abacus/abacusroot.h"

class ABA_GLOBAL;

  class  ABA_STRING :  public ABA_ABACUSROOT  {
    public:

/*! The constructor.
 *
 *   \param glob A pointer to the corresponding global object.
 *   \param cString The initializing string, by default the empty string.
 */
      ABA_STRING(ABA_GLOBAL *glob, const char* cString = "");

/*! A constructor building a string from a string  and an integer.
 *
 *   This constructor is especially useful for building variable
 *   or constraint names like \a con18.
 *
 *   \param glob A pointer to the corresponding global object.
 *   \param cString The initializing string.
 *   \param index The integer value appending to the \a cString
 *    		      (must be less than \a MAX_INT).
 */
      ABA_STRING(ABA_GLOBAL *glob, const char* cString, int index);

/*! The copy constructor.
 *
 *   \param rhs The string that is copied.
 */
      ABA_STRING(const ABA_STRING &rhs);

//! The destructor.
      ~ABA_STRING();

/*! \brief The assignment operator
 *   makes a copy of the right hand side and reallocates memory if required.
 *
 *   \return A reference to the object.
 *
 *   \param rhs The right hand side of the assignment.
 */
      const ABA_STRING& operator=(const ABA_STRING &rhs);

/*! \brief The assignment operator
 *   is overloaded for character strings.
 */
      const ABA_STRING& operator=(const char *rhs);

/*! The comparison operator.
 *
 *   \note the C-library function \a strcmp() returns 0 if both strings equal.
 *
 *   \return 0 If both strings are not equal,
 *   \return 1 otherwise.
 *
 *   \param lhs The left hand side of the comparison.
 *   \param rhs The right hand side of the comparison.
 */
      friend int operator==(const ABA_STRING &lhs, const ABA_STRING &rhs);

/*! \brief The comparison operator
 *   is overloaded for character strings on the right hand side.
 */
      friend int operator==(const ABA_STRING &lhs, const char *rhs);

/*! The not-equal operator.
 *
 *   \note the C-library function \a strcmp() returns 0 if both strings equal.
 *
 *   \return 0 If both strings are equal,
 *   \return 1 otherwise.
 *
 *   \param lhs The left hand side of the comparison.
 *   \param rhs The right hand side of the comparison.
 */
      friend int operator!=(const ABA_STRING &lhs, const ABA_STRING &rhs);

/*! \brief The not-equal operator
 *   is overloaded for character strings on the right hand side.
 */
      friend int operator!=(const ABA_STRING &lhs, const char *rhs);

/*! The output operator.
 *
 *   \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param rhs The string being output.
 */
      friend ostream& operator<<(ostream &out, const ABA_STRING &rhs);

/*! \brief With the subscript operator
 *   a single character of the string can be accessed or modified.  
 *
 *   If the class is compiled with the preprocessor flag \a -DABACUSSAFE,
 *   then a range check is performed.
 *
 *   \return A reference to the \a i-th character of
 *   the string. 
 *
 *   \param i The number of the character that should be accessed or
 *                modified. The first character has number 0.
 */
      char& operator[](int i);

/*! \brief The subscript operator
 *   is overloaded for constant use.
 */
      const char& operator[](int i) const;

/*! \return The length of the string, not including the \a '\0'
 *   terminating the string.
 */
      int size() const;

/*! Is very similar to the function \a atoi() from \a <string.h>.
 *
 *   It converts the substring starting
 *   at component \a i and ending in the first following component
 *   with \a '\0' to an integer.
 *   \a ascii2int(0) converts the complete string.
 *
 *   \return The string converted to an integer value.
 *
 *   \param i The number of the character at which the conversion should
 *                start. The default value of \a i is 0.
 */
      int ascii2int(int i = 0) const;

/*! The function \a ascii2unsignedint()
 *   converts the string to an \a unsigned int value.
 *
 *   \return The string converted to an unsigned integer.
 */
      unsigned int ascii2unsignedint() const;

/*! \brief Emulates the function \a atof() of the
 *   standard C library and converts the string to a floating point number.
 *
 *   \return The string converted to a floating point number
 */
      double ascii2double() const;

/*! Converts the string to a boolean value.
 *
 *   This is only possible for the strings \a "true" and \a "false".
 *
 *   \return The string converted to \a true or \a false.
 */
      bool ascii2bool() const;

/*! \return true If the string ends with the string \a end,
 *   \return false otherwise.
 *
 *   \param end The string with which the ending of the string is compared.
 */
      bool ending(const char *end) const;

/*! \return The \a char* representing the string to make it accessible for 
 *   C-functions.
 */
      char *string() const;

    private:

/*! \brief Terminates the program with an
 *   error message if \a i is not the position of a character of
 *   the string.
 *  
 *   The \a '\0' at the end of the string is not a valid
 *   character in this sense.
 */ 
      void rangeCheck(int i) const;

      /*! A pointer to the corresponding master of the optimization.
       */
      ABA_GLOBAL *glob_;

      /*! An array storing the character of the string. This
       *  array must be terminated with a \a '\0'.
       */
      char   *string_;
  };
#endif  // ABA_STRING_H

/*! \class ABA_STRING
 *   \brief class ABA_STRING implements are very simple class for the
 * representation of character strings.
 */

