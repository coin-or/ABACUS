/*!\file
 * \author Matthias Elf
 *
 * This class is the base class of all other classes of ABACUS. 
 * By embedding an enumeration and some useful functions in this class 
 * we can avoid a global scope of these names. 
 *
 * \par
 * For Compilers that to not provide the type \a bool so far, we add this 
 * type here. 
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

#ifndef ABA_ABACUSROOT_H
#define ABA_ABACUSROOT_H

// check if preprocessor definitions are set correctly
/* We have to split some of the checks since otherwise the lines get to
 *  long.
 */
// is a compiler defined?
#if !(defined ABACUS_COMPILER_GCC33 || defined ABACUS_COMPILER_GCC41 || \
      defined ABACUS_COMPILER_GCC34 || defined ABACUS_COMPILER_GCC43 || \
      defined ABACUS_COMPILER_SUN   || defined ABACUS_COMPILER_GCC44 || \
      defined ABACUS_COMPILER_GCC)
#error Preprocessor definition for compiler is missing!
#endif






class ABA_ABACUSROOT {
  public:
/*! The destructor 
 * is only implemented since it should be virtual function.
 */
    virtual ~ABA_ABACUSROOT();


    /*! \brief This enumeration defines the codes used be the function \a exit().
     *
     * \param Ok The program terminates without error.
     * \param Fatal A severe error occurred leading to an immediate 
     *              termination of the program.
     */
    enum EXITCODES{Ok, Fatal};

    /*! \brief terminates the program and returns \a code
     * to the environment from which the program was called.
     *
     * \par
     * We overload the function \a exit() that in a debugger a break point can be
     * easily set within this function in order to investigate the
     * error. We also observed that for some reason it can be impossible
     * to set a break point within a template. Here this function \a exit()
     * was quite helpful during the debugging process.
     *
     * \par
     * Exception handling could substitute many calls to the function \a exit().
     * However, in version 2.6.3 of the GNU \CPLUSPLUS/ compiler only a prototypical
     * implementation of exception handling is available. As soon as a
     * the GNU compiler provides a stable implementation of exception handling
     * we will use this technique in future releases of this software.
     
     * \param code The exit code given to the environment.
     */
    virtual void exit(enum EXITCODES code) const;

    /*! \brief converts a boolean variable to the strings \a "on" and \a "off".
     *
     * \return \a "on" if \a value is \a true
     * \return \a "off" otherwise
     * \param value The boolean variable being converted.  
     */
    const char *onOff(bool value);

    /*! \return The absolute value of the fractional
     * part of the value \a x. E.g., it holds \a fracPart(2.33) == 0.33 and
     * \a fracPart(-1.77) == 0.77.
     *
     * \param x The value of which the fractional part is computed.
     */
    double fracPart(double x) const;
};
/*! \class ABA_ABACUSROOT
 *  \brief base class of all other classes of ABACUS
 */


#endif  //ABA_ABACUSROOT_H



#define ABACUS_VERSION 301
#define ABACUS_VERSION_STRING "3.0.1"



