/*!\file
 * \author Matthias Elf
 *
 * \brief csense.
 *
 * The most important objects in a cutting plane algorithm are
 * constraints, which can be equations (\a Equal) or
 * inequalities with the sense \f$\le\f$ (\a Less) or the sense \f$\ge\f$
 * (\a Greater). We implement the sense of optimization as a class
 * since we require it both in the classes ABA_CONSTRAINT and
 * ABA_ROW.
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

#ifndef ABA_CSENSE_H
#define ABA_CSENSE_H
#include <iostream>
using namespace std;

#include "abacus/abacusroot.h"

class ABA_GLOBAL;

  class  ABA_CSENSE : public ABA_ABACUSROOT  { 
    public: 

/*! \param Less \f$\le\f$
 *  \param Equal \f$=\f$
 *  \param Greater \f$\ge\f$ 
 */
      enum SENSE {Less, Equal, Greater};

/*! If the default constructor is used, the sense is undefined.
 *
 *       \param glob A pointer to the corresponding global object.
 */
      ABA_CSENSE(ABA_GLOBAL *glob);
   
/*! This constructor initializes the sense.
 *
 *  \param glob A pointer to the corresponding global object.
 *  \param s The sense.
 */
      ABA_CSENSE(ABA_GLOBAL *glob, SENSE s);

/*! \brief With this constructor
 *  the sense of the constraint can also be initialized with a single letter.
 *
 *  \param glob A pointer to the corresponding global object.
 *  \param s A character representing the sense:
 *               {\tt E} or {\tt e} stand
 *               for \a Equal, {\tt G} and {\tt g} stand for \a Greater, and
 *               {\tt L} or {\tt l} stand for \a Less.
 */
      ABA_CSENSE(ABA_GLOBAL *glob, char s);

/*! \brief The output operator 
 * writes the sense on an output stream in the form \a <=, \a =, or \a >=.
 *
 *  \return The output stream.
 *
 *  \param out The output stream.
 *  \param rhs The sense being output.
 */
      friend ostream& operator<<(ostream &out, const ABA_CSENSE &rhs);

/*! \brief The default assignment operator
 * is overloaded such that also the
 *  enumeration \a SENSE can be used on the right hand side.
 *
 *  \return A reference to the sense.
 *
 *  \param rhs The new sense.
 */
      const ABA_CSENSE &operator=(SENSE rhs);

 /*! \return The sense of the constraint.
  */

      SENSE sense() const;

 /*! \brief This overloaded version of \a sense() 
  * changes the sense of the constraint.
  *
  *  \param s The new sense.
  */

      void sense(SENSE s);

 /*! \brief The sense can also be changed by a character as in the
  *  constructor ABA_CSENSE(ABA_GLOBAL *glob, char s).
  *
  *  \param s The new sense.
  */
      void sense(char s);

    private: 
      ABA_GLOBAL *glob_;

/*! Stores the sense of a constraint.
 */
      SENSE            sense_;
  };


inline const ABA_CSENSE &ABA_CSENSE::operator=(SENSE rhs)
  {
    sense_ = rhs;
    return *this;
  }

inline ABA_CSENSE::SENSE ABA_CSENSE::sense() const
  {
    return sense_;
  }

inline void ABA_CSENSE::sense(SENSE s) 
  {
    sense_ = s;
  }  


#endif  // ABA_CSENSE_H

/*! \class ABA_CSENSE
 *  \brief we implement the sense of optimization as a class
 * since we require it both in the classes ABA_CONSTRAINT and
 * ABA_ROW.
 */

