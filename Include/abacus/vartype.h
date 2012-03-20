/*!\file
 * \author Matthias Elf
 * \brief vartype.
 *
 * Variables can be of three different types: \a Continuous,
 * \a Integer or \a Binary. We distinguish \a Integer and \a Binary variables
 * since some operations are performed differently (e.g., branching).
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

#ifndef ABA_VARTYPE_H
#define ABA_VARTYPE_H
#include <iostream>
using namespace std;

#include "abacus/abacusroot.h"


  class  ABA_VARTYPE :  public ABA_ABACUSROOT  { 
    public: 

    /*! The enumeration with the different variable types.
     *
     * \param Continuous A continuous variable.
     * \param Integer A general integer variable.
     * \param Binary A variable having value 0 or 1.
     */
      enum TYPE {Continuous, Integer, Binary};

/*! \brief The default constructor
 *   lets the type of the variable uninitialized.
 */
      ABA_VARTYPE();

/*! This constructor initializes the variable type.
 *
 *   \param t The variable type.
 */
      ABA_VARTYPE(TYPE t);

/*! \brief The output operator
 *   writes the variable type to an output stream in
 *   the format {\tt Continuous}, {\tt Integer}, or {\tt Binary}.
 *
 *   \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param rhs The variable type being output.
 */
      friend ostream &operator<<(ostream &out, const ABA_VARTYPE &rhs);

/*! \return The type of the variable.
 */
      TYPE type() const;

/*! \brief This version of the function \a type() sets the variable type.
 *
 *   \param t The new type of the variable.
 */
      void type(TYPE t);

/*! \return true If the type of the variable is \a Integer or \a Binary,
 *   \return false otherwise.
 */
      bool discrete() const;

/*! \return true If the type of the variable \a Binary,
 *   \return false otherwise.
 */
      bool binary() const;

/*! \return true If the type of the variable is \a Integer,
 *   \return false otherwise.
 */
      bool integer() const;

    private: 

      /*! The type of the variable.
       */
      TYPE type_;
  };


inline ABA_VARTYPE::ABA_VARTYPE() 
  
  { }

inline ABA_VARTYPE::ABA_VARTYPE(TYPE t) 
  : 
    type_(t)
  { }

inline ABA_VARTYPE::TYPE ABA_VARTYPE::type() const
  {
    return type_;
  }

inline void ABA_VARTYPE::type(TYPE t)
  {
    type_ = t;
  }

inline bool ABA_VARTYPE::discrete() const
  {
    if (type_ == Continuous) return false;
    else                     return true;
  }

inline bool ABA_VARTYPE::binary() const
  {
    if (type_ == Binary) return true;
    else                 return false;
  }


#endif  // ABA_VARTYPE_H


/*! \class ABA_VARTYPE
 * \brief Variables can be of three different types: \a Continuous,
 * \a Integer or \a Binary. We distinguish \a Integer and \a Binary variables
 * since some operations are performed differently (e.g., branching).
 */

