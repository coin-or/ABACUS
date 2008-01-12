/*!\file
 * \author Matthias Elf
 * \brief constraint using row.
 *
 * Earlier we explained that we distinguish between the constraint
 * and the row format. We have seen already that a constraint is
 * transformed to the row format when it is added to the linear program.
 * However, for some constraints of certain optimization problems
 * the row format itself is the most suitable representation.
 * Therefore the class ABA_ROWCON implements constraints stored in the
 * class ABA_ROW.
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

#ifndef ABA_ROWCON_H
#define ABA_ROWCON_H

#include "abacus/row.h"
#include "abacus/constraint.h"

  class  ABA_ROWCON :  public ABA_CONSTRAINT  { 
    public: 

/*! The constructor.
 *
 *   \param master A pointer to the corresponding master of the optimization.
 *   \param sub A pointer to the subproblem associated with the
 *                    constraint. This can also be the 0-pointer.
 *   \param sense The sense of the constraint.
 *   \param nnz The number of nonzero elements of the constraint.
 *   \param support The array storing the variables with nonzero
 *                        coefficients.
 *   \param coeff The nonzero coefficients of the variables stored
 *                      in \a support.
 *   \param rhs The right hand side of the constraint.
 *   \param dynamic If this argument is \a true, then the constraint
 *                      can be removed from the active constraint set
 *                      during the cutting plane phase of the subproblem
 *                      optimization.
 *   \param local If this argument is \a true, then the constraint
 *                    is considered to be only locally valid. As a
 *                    locally valid constraint is associated with
 *                    a subproblem, \a sub must not be 0 if \a local is \a true.
 *   \param liftable If this argument is \a true, then a lifting
 *                       procedure must be available, i.e., that the
 *                       coefficients of variables which have not been
 *                       active at generation time of the constraint
 *                       can be computed.
 */
      ABA_ROWCON(ABA_MASTER *master, 
                 const ABA_SUB *sub, 
                 ABA_CSENSE::SENSE sense,
                 int nnz, 
                 const ABA_ARRAY<int> &support, 
                 const ABA_ARRAY<double> &coeff,
                 double rhs, 
                 bool dynamic, 
                 bool local,
                 bool liftable);

/*! \brief This constructor
 *   is equivalent to the previous constructor
 *   except that it uses C-style arrays for \a support and \a coeff.
 */
      ABA_ROWCON(ABA_MASTER *master, 
                 const ABA_SUB *sub, 
                 ABA_CSENSE::SENSE sense,
                 int nnz, 
                 int *support, 
                 double *coeff,
                 double rhs, 
                 bool dynamic, 
                 bool local,
                 bool liftable);

//! The destructor.
      virtual ~ABA_ROWCON();
#ifdef ABACUS_PARALLEL

/*! \brief The message constructor
 *   creates the constraint from an ABA_MESSAGE.
 *
 *   \param msg The ABA_MESSAGE object from which the constraint is initialized.
 */
      ABA_ROWCON(const ABA_MASTER *master, ABA_MESSAGE &msg);

/*! Packs the data of the constraint in an ABA_MESSAGE object.
 *
 *   \param msg The ABA_MESSAGE object in which the constraint is packed.
 */
      virtual void pack(ABA_MESSAGE &msg) const;

/*! Returns an integer which identifies the class.
 *
 *   \return The class identification.
 */
      virtual int classId() const;
#endif

/*! \brief Computes the coefficient of a variable which
 *   must be of type ABA_NUMVAR.
 *
 *   It redefines the virtual function \a coeff() of the base class ABA_CONSTRAINT.
 *
 *   \warning The worst case complexity of the call of this function is
 *   the number of nonzero elements of the constraint.
 *
 *   \return The coefficient of the variable \a v.
 *
 *   \param v The variable of which the coefficient is determined.
 */
      virtual double coeff(ABA_VARIABLE *v);

/*! Writes the row format of the constraint on an output stream.
 *
 *   It redefines the virtual function \a print() of the base class
 *   ABA_CONVAR.
 *
 *   \param out The output stream.
 */
      virtual void print(ostream &out);

/*! \return A pointer to the object of the class ABA_ROW representing the constraint.
 */
      ABA_ROW *row();

    protected: 

      /*! The representation of the constraint.
       */
      ABA_ROW row_;
  };


inline ABA_ROW *ABA_ROWCON::row()
  {
    return &row_;
  }


#endif  // ABA_ROWCON_H

/*! \class ABA_ROWCON
 *  \brief class ABA_ROWCON implements constraints stored in the
 * class ABA_ROW.
 */

