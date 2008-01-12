/*!\file
 * \author Matthias Elf
 * \brief variable identified by a number.
 *
 *  This class is derived from the class ABA_VARIABLE and implements
 *  a variable which is uniquely defined by a number.
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

#ifndef ABA_NUMVAR_H
#define ABA_NUMVAR_H

#include "abacus/variable.h"

  class  ABA_NUMVAR :  public ABA_VARIABLE  { 
    public: 

/*! The constructor.
 *
 *      \param
 *        master 
 *        A pointer to the corresponding master of the optimization.
 *      \param
 *         sub 
 *         A pointer to the subproblem associated with variable. This
 *         can also be the 0-pointer.
 *  
 *      \param
 *        number 
 *        The number of the column associated with the variable.      
 *      \param
 *        dynamic 
 *        If this argument is \a true, then the variable can also be
 *        removed again from the set of active variables after it is
 *        added once.
 *      \param
 *         local 
 *         If this argument is \a true, then the variable is only locally
 *         valid, otherwise it is globally valid. As a locally valid variable
 *         is associated with a subproblem, \a sub must not be 0, if \a local
 *         is \a true.
 * 
 *      \param
 *         obj 
 *         The objective function coefficient of the variable.
 *      \param
 *         lBound 
 *         The lower bound of the variable.
 *      \param
 *         uBound 
 *         The upper Bound of the variable.
 *      \param
 *         type 
 *         The type of the variable.
 */
      ABA_NUMVAR(ABA_MASTER *master, 
                 const ABA_SUB *sub, 
                 int number, 
                 bool dynamic, 
                 bool local,
                 double obj, 
                 double lBound, 
                 double uBound, 
                 ABA_VARTYPE::TYPE type);

//! The destructor.
      virtual ~ABA_NUMVAR();
#ifdef ABACUS_PARALLEL

/*! The message constructor creates the variable from an ABA_MESSAGE.
 *
 *   \param msg  The ABA_MESSAGE object from which the variable is initialized.
 */
      ABA_NUMVAR(const ABA_MASTER *master, ABA_MESSAGE &msg);

/*! Packs the data of the variable in an ABA_MESSAGE object.
 *
 *   \param msg  The ABA_MESSAGE object in which the variable is packed.
 */
      virtual void pack(ABA_MESSAGE &msg) const;

/*! Returns an integer which identifies the class.
 *
 *   \return The class identification.
 */
      virtual int classId() const;
#endif

/*! Writes the number of the variable to an output stream.
 *
 *   \return A reference to the output stream.
 *
 *   \param out  The output stream.
 *   \param rhs  The variable being output.
 */
      friend ostream &operator<<(ostream &out, const ABA_NUMVAR &rhs);

/*! \return The number of the variable.
 */
      int number() const;
    protected: 


      /*! The identification number of the variable. 
       */
      int number_;
  };


inline int ABA_NUMVAR::number () const
  {
    return number_;
  }


#endif  // ABA_NUMVAR_H

/*! \class ABA_NUMVAR
 *  \brief class is derived from the class ABA_VARIABLE and implements
 *  a variable which is uniquely defined by a number
 */
