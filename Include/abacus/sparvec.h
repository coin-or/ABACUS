/*!\file
 * \author Matthias Elf
 * \brief sparse vector.
 *
 * If the number of components of a vector having nonzero coefficients
 * is small (sparse), then it is more adequate to store only the
 * number of these components together with the nonzero coefficients.
 *
 * Since other classes, e.g., the class ABA_RO  are derived from this
 * class, all data members are protected in order to provide efficient
 * access also in these derived classes.
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


#ifndef ABA_SPARVEC_H
#define ABA_SPARVEC_H
#include <stdlib.h>

#include "abacus/array.h"
#include "abacus/buffer.h"


  class  ABA_SPARVEC :  public ABA_ABACUSROOT  {
    public:

/*! The constructor for an empty sparse vector.  
 *
 *  \param glob A pointer to the corresponding global object.  
 *  \param size The maximal number of
 *                  nonzeros of the sparse vector (without reallocation).
 *  \param reallocFac The reallocation factor (in percent of the
 *     original size), which is used in a default reallocation if a variable
 *     is inserted when the sparse vector is already full. Its default value
 *     is 10.  
 *
 * If no memory for \a support_ and \a coeff_ is
 * allocated then an automatic allocation will be performed when the
 * function \a insert() is called the first time.
 */
      ABA_SPARVEC(ABA_GLOBAL *glob, 
                  int size, 
                  double reallocFac = 10.0);

/*! \brief A constructor
 *   with initialization of the support and coefficients of the sparse vector.
 *
 *   The minimum value of \a size and \a s.size
 *   is the number of nonzeros of the sparse vector.
 *
 *       \param glob A pointer to the corresponding global object.
 *       \param size The maximal number of nonzeros (without reallocation).
 *       \param s An array storing the support of the sparse vector, i.e.,
 *                   the elements for which a (normally nonzero) coefficient
 *                   is given in \a c.
 *      \param c An array storing the coefficients of the support elements
 *                   given in \a s. This array must have at least
 *                   the length of the minimum of \a size and \a s.size().
 *       \param reallocFac The reallocation factor (in percent of the original
 *                     size), which is used in a default reallocation if
 *                     a variable is inserted when the sparse vector is already
 *                     full. Its default value is 10.
 *
 *   If \a size is 0, then also no elements are copied in the \a for-loop
 *   since \a nnz_ will be also 0.
 */
      ABA_SPARVEC(ABA_GLOBAL *glob, 
                  int size, 
                  const ABA_ARRAY<int> &s, 
                  const ABA_ARRAY<double> &c, 
                  double reallocFac = 10.0);

/*! \brief This constructor
 *   is equivalent to the previous one except that it is
 *   using C-style arrays for the initialization of the sparse vector.
 */
      ABA_SPARVEC(ABA_GLOBAL *glob, 
                  int size, 
                  int *s, 
                  double *c, 
                  double reallocFac = 10.0);

/*! The copy constructor.
 *
 *   \param rhs The sparse vector that is copied.
 */
      ABA_SPARVEC(const ABA_SPARVEC& rhs);

//! The destructor.
      ~ABA_SPARVEC();

/*! \brief The assignment operator
 *   requires that the left hand and the
 *   right hand side have the same length (otherwise use the function
 *   \a copy()). 
 *
 *   \return A reference to the left hand side.
 *
 *   \param rhs The right hand side of the assignment.
 */
      const ABA_SPARVEC& operator=(const ABA_SPARVEC& rhs);

/*! \brief The output operator
 *    writes the elements of the support 
 *    and their coefficients line by line on an output stream.
 *
 *   \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param rhs The sparse vector being output.
 */
      friend ostream& operator<<(ostream& out, const ABA_SPARVEC& rhs);

/*! \brief A range check is performed if the function is compiled with
 *   \a -DABACUSSAFE.
 *
 *  \return The support of the \a i-th nonzero element.
 *
 *   \param i The number of the nonzero element.
 */
      int support(int i) const; 

/*! \brief A range check is performed if the function is compiled with
 *   \a -DABACUSSAFE.
 *
 *   \return The coefficient of the \a i-th nonzero element.
 *
 *   \param i The number of the nonzero element.
 */
      double coeff(int i) const;

/*! \return The coefficient having support \a i.
 *
 *   \param i The number of the original coefficient.
 */
      double origCoeff(int i) const;

/*! Adds a new support/coefficient pair to the vector.
 *
 *   If necessary a reallocation
 *   of the member data is performed automatically.
 *
 *   \param s The new support.
 *   \param c The new coefficient.
 */
      void insert(int s, double c);

/*! Deletes the elements listed in a buffer from the sparse vector.
 *
 *   The numbers of indices in this buffer must be upward sorted.
 *   The elements before the first element in the buffer are unchanged.
 *   Then the elements
 *   which are not deleted are shifted left in the arrays.
 *
 *   \param del The numbers of the elements removed from the sparse vector.
 */
      void leftShift(ABA_BUFFER<int> &del);

/*! \brief Is very similar to the assignment operator,
 *   yet the size of the two vectors need not be equal and only
 *   the support, the coefficients, and the number of nonzeros is copied.
 *   A reallocation is performed if required.
 *
 *   \param vec The sparse vector that is copied.
 */
      void copy (const ABA_SPARVEC &vec);

/*! Removes all nonzeros from the sparse vector.
 */
      void clear();

/*! Replaces the index of the support by new names.
 *
 *   \param newName The new names (support) of the elements of the sparse
 *                      vector. The array \a newName must have at least a length
 *                      equal to the
 *                      maximal element in the support of the sparse vector.
 */
      void rename(ABA_ARRAY<int> &newName);

/*! \return The maximal length of the sparse vector.
 */
      int size() const;

/*! \return The number of nonzero elements.
 *   This is not necessarily the correct number of nonzeros, yet the
 *   number of coefficient/support pairs, which are stored.
 *   Some of these pairs may have a zero coefficient.
 */
      int nnz() const;

/*! \return The Euclidean norm of the sparse vector.
 */
      double norm();

/*! Increases the size of the sparse vector by \a reallocFac_ percent of the original size. 
 *
 *   This function is
 *   called if an automatic reallocation takes place.
 */
      void realloc();

/*! \brief This other version of \a realloc()
 *   reallocates the sparse vector to a given length. 
 *   
 *   It is an error to decrease
 *   size below the current number of nonzeros.
 *
 *   \param newSize The new maximal number of nonzeroes that can be
 *                      stored in the sparse vector.
 */
      void realloc(int newSize);
    protected:

/*! \brief Terminates the program with an error message if \a i is negative
 *   or greater or equal than the number of nonzero elements.
 *
 *   If the class ABA_SPARVEC is compiled with the flag \a -DABACUSSAFE,
 *   then before each access operation on element \a i of the sparse
 *   vector the function \a rangeCheck() is called.
 *
 *   \param i An integer that should be checked if it is in the range of
 *                the sparse vector.
 */
      void rangeCheck(int i) const;

      /*! A pointer to the corresponding global object.
       */
      ABA_GLOBAL *glob_;

      /*! \brief The maximal number of nonzero coefficients which can
       *         be stored without reallocation.
       */
      int size_;

      /*! The number of stored elements (``nonzeros'').
       */
      int nnz_;

      /*! \brief If a new element is inserted but the sparse
       *  vector is full, then its size is increased
       *  by \a reallocFac_ percent.
       */
      double reallocFac_;

      /*! The array storing the nonzero variables.
       */
      int *support_;

      /*! The array storing the corresponding nonzero coefficients.
       */
      double *coeff_;
  };


inline int ABA_SPARVEC::support(int i) const
  {
#ifdef ABACUSSAFE
    rangeCheck(i);
#endif    
    return support_[i];
  }

inline double ABA_SPARVEC::coeff(int i) const
  {
#ifdef ABACUSSAFE
    rangeCheck(i);
#endif    
    return coeff_[i];
  }

inline void ABA_SPARVEC::insert(int s, double c)
  {

    if (nnz_ == size_) realloc();

    support_[nnz_] = s;
    coeff_[nnz_]   = c;
    ++nnz_;

  }

inline void ABA_SPARVEC::clear()
  {
    nnz_ = 0;
  }

inline int ABA_SPARVEC::size() const
  {
    return size_;
  }

inline int ABA_SPARVEC::nnz() const
  {
    return nnz_;
  }


#endif  // ABA_SPARVEC_H

/*! \class ABA_SPARVEC
 *  \brief Since other classes, e.g., the class ABA_RO  are derived from this
 * class, all data members are protected in order to provide efficient
 * access also in these derived classes.
 */

