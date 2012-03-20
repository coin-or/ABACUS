/*!\file
 * \author Matthias Elf
 * \brief column.
 *
 * In the same way as the class ABA_ROW refines the class ABA_SPARVEC for the
 * representation of constraints in row format, the class ABA_COLUMN refines
 * ABA_SPARVEC for the representation of variables in column format.
 * This class should not be confused with the class ABA_VARIABLE for the
 * abstract representation of variables within the framework. Again, there
 * is a class ABA_COLVAR derived from ABA_VARIABLE having a member of type
 * ABA_COLUMN, but there are also other classes derived from ABA_VARIABLE.
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

#ifndef ABA_COLUMN_H
#define ABA_COLUMN_H

#include "abacus/sparvec.h"


  class  ABA_COLUMN :  public ABA_SPARVEC  { 
    public: 

/*! A constructor.
 *
 * \param glob A pointer to the corresponding global object.
 * \param obj {The objective function coefficient.
 * \param lb The lower bound.
 * \param ub The upper bound.
 * \param nnz The number of nonzero elements stored in the
 *                  arrays |s| and |c|.
 * \param s An array of the nonzero elements of the column.
 * \param c An array of the nonzero coefficients associated with the
 *                elements of |s|.
 */
      ABA_COLUMN(ABA_GLOBAL *glob, 
                 double obj, 
                 double lb, 
                 double ub,
                 int nnz, 
                 ABA_ARRAY<int> &s, 
                 ABA_ARRAY<double> &c);

/*! Another constructor generating an uninitialized column.
 *
 *\param glob A pointer to the corresponding global object.
 *\param maxNnz The maximal number of nonzero elements that
 *                  can be stored in the row.
 */
      ABA_COLUMN(ABA_GLOBAL *glob, int maxNnz);

/*! A constructor using a sparse vector for the initialization.
 *
 * \param glob  A pointer to the corresponding global object.
 * \param obj The objective function coefficient.
 * \param lb  The lower bound.
 * \param ub  The upper bound.
 * \param vec A sparse vector storing the support and the coefficients
 *                  of the column.
 */
      ABA_COLUMN(ABA_GLOBAL *glob, 
                 double obj, 
                 double lb, 
                 double ub, 
                 ABA_SPARVEC &vec);
      ~ABA_COLUMN();

/*!The output operator.
 *
 *\return A reference to the output stream.
 *
 *\param out The output stream.
 *\param rhs The column being output.
 */
      friend ostream& operator<<(ostream &out, const ABA_COLUMN &rhs);

/*! \return The objective function coefficient of the column.
 */
      double obj() const;

/*! \brief This version of the  function \a obj() 
 * sets the objective function coefficient of the column.
 *
 * \param c The new value of the objective function coefficient.
 */
      void obj(double c);

/*! \return The lower bound of the column.
 */
      double lBound()  const;

/*! \brief This version of the function \a lBound()
 * sets the lower bound of the column.
 *
 *\param l The new value of the lower bound.
 */
      void lBound(double l);

/*! \return The upper bound of the column.
 */
      double uBound()  const;

/*! \brief This version of the function \a uBound()
 * sets the upper bound of the column.
 *
 * \param u The new value of the upper bound.
 */
      void uBound(double u);

/*! \brief Is very similar to the assignment operator,
 * yet the columns do not have to be of equal size.
 * A reallocation is performed if required.
 *
 * \param col The column that is copied.
 */
      void copy(const ABA_COLUMN &col);

    private: 

      /*! The objective function coefficient of the column.
       */
      double obj_;

      /*! The lower bound of the column.
       */
      double lBound_;

      /*! The upper bound of the column.
       */
      double uBound_;
  };


inline double ABA_COLUMN::obj() const
  {
    return obj_;
  }

inline void ABA_COLUMN::obj(double c) 
  {
    obj_ = c;
  }

inline double ABA_COLUMN::lBound() const
  {
    return lBound_;
  }

inline void ABA_COLUMN::lBound(double l)
  {
    lBound_ = l;
  }

inline double ABA_COLUMN::uBound() const
  {
    return uBound_;
  }

inline void ABA_COLUMN::uBound(double u)
  {
    uBound_ = u;
  }


#endif  // ABA_COLUMN_H

/*! \class ABA_COLUMN
 * \brief class ABA_COLUMN refines
 * ABA_SPARVEC for the representation of variables in column format.
 */

