/*!\file
 * \author Matthias Elf
 * \brief row.
 *
 * This class refines its base class ABA_SPARVEC for the
 * representation of constraints in the row format.
 * This class plays an essential role in the interface with the LP-solver.
 *
 * \par
 * This class should not be confused with the class ABA_CONSTRAINT, which
 * is an abstract class for the representation of constraints within
 * the framework.
 * Moreover, the class ABA_ROWCON derived from the class ABA_CONSTRAINT provides
 * a constraint representation in row format, but there are also other
 * representations of constraints.
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


#ifndef ABA_ROW_H
#define ABA_ROW_H

#include "abacus/sparvec.h"
#include "abacus/csense.h"
  class  ABA_ROW :  public ABA_SPARVEC  {
    public:

/*! A constructor.
 *
 *   \param glob A pointer to the corresponding global object.
 *   \param nnz The number of nonzero elements of the row.
 *   \param s The array storing the nonzero elements.
 *   \param c The array storing the nonzero coefficients of the
 *                     elements of \a s.
 *   \param sense The sense of the row.
 *   \param r The right hand side of the row.
 */
      ABA_ROW(ABA_GLOBAL *glob, 
              int nnz, 
              const ABA_ARRAY<int> &s, 
              const ABA_ARRAY<double> &c, 
              const ABA_CSENSE sense, double r);

/*! \brief This is an equivalent constructor
 *   using ABA_CSENSE::SENSE instead
 *   of an object of the class \a SENSE to initialize the sense of the
 *   constraint.
 */
      ABA_ROW(ABA_GLOBAL *glob, 
              int nnz, 
              const ABA_ARRAY<int> &s, 
              const ABA_ARRAY<double> &c, 
              const ABA_CSENSE::SENSE sense, double r);

/*! \brief This is also an equivalent constructor
 *   except that \a s and \a c are C-style arrays.
 */
      ABA_ROW(ABA_GLOBAL *glob, 
              int nnz, 
              int *s, 
              double *c, 
              ABA_CSENSE::SENSE sense, 
              double r);

/*! A constructor without initialization of the nonzeros of the row.
 *
 *   \param glob A pointer to the corresponding global object.
 *   \param size The maximal numbers of nonzeros.
 */
      ABA_ROW(ABA_GLOBAL *glob, int size);

//! The destructor.
      ~ABA_ROW();

/*! \brief The output operator
 *   writes the row on an output stream in format like {\tt -2.5 x1 + 3 x3 <= 7}.
 *  
 *   Only variables with nonzero coefficients are output.
 *   The output operator does neither output a \a '+' before the first
 *   coefficient of a row, if it is positive, nor outputs coefficients
 *   with absolute value 1.
 *
 *   \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param rhs The row being output.
 */
      friend ostream &operator<<(ostream& out, const ABA_ROW &rhs);

/*! \return The right hand side stored in the row format.
 */ 
      double rhs() const;

/*! \brief This version of \a rhs()
 *   sets the right hand side of the row.
 *
 *   \param r The new value of the right hand side.
 */
      void rhs(double r);

/*! \return A pointer to the sense of the row.
 */
      ABA_CSENSE *sense();

/*! \brief This version of \a sense() 
 *  sets the sense of the row.
 *
 *   \param s The new sense of the row.
 */
      void sense(ABA_CSENSE &s);

/*! \brief And another version of \a sense()
 *   to set the sense of the row.
 *
 *   \param s The new sense of the row.
 */
      void sense(ABA_CSENSE::SENSE s);

/*! \brief Behaves like an assignment operator,
 *   however, the maximal number of the elements of this row
 *   only has to be at least the number of nonzeros of \a row.
 *
 *   \param row The row that is copied.
 */
      void copy(const ABA_ROW &row);

/*! \brief Removes the indices listed in \a buf
 *   from the support of the row and subtracts \a rhsDelta from
 *   its right hand side.
 *
 *   \param buf The components being removed from the row.
 *   \param rhsDelta The correction of the right hand side of the row.
 */
      void delInd(ABA_BUFFER<int> &buf, double rhsDelta);

    protected:

      /*! The sense of the row.
       */
      ABA_CSENSE sense_;

      /*! The right hand side of the row.
       */
      double rhs_;
  };


inline double ABA_ROW::rhs() const
  {
    return rhs_;
  }

inline void  ABA_ROW::rhs(double r) 
  {
    rhs_ = r;
  }

inline ABA_CSENSE *ABA_ROW::sense()
  {
    return &sense_;
  }

inline void ABA_ROW::sense(ABA_CSENSE &s)
  {
    sense_ = s;
  }

inline void ABA_ROW::sense(ABA_CSENSE::SENSE s)
  {
    sense_.sense(s);
  }


#endif  // ABA_ROW_H

/*! \class ABA_ROW
 *  \brief class refines its base class ABA_SPARVEC for the
 * representation of constraints in the row format
 */

