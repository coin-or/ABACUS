/*!\file
 * \author Matthias Elf
 * \brief colvar.
 *
 * Some optimization problems, in particular column generation problems, are
 * better described from a variable point of view instead of a constraint
 * point of view. For such context we provide the class ABA_COLVAR which
 * similar to the class ABA_ROWCON stores the nonzero coefficient explicitly
 * in an object of the class ABA_COLUMN.
 *
 * \par
 * The constraint class which is associated with this variables class
 * is the class ABA_NUMCON which identifies constraints only by a unique
 * integer number. ABA_NUMCON is an abstract class.
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

#ifndef ABA_COLVAR_H
#define ABA_COLVAR_H

#include "abacus/variable.h"
#include "abacus/vartype.h"
#include "abacus/column.h"

class ABA_SPARVEC;
  class  ABA_COLVAR :  public ABA_VARIABLE  { 
    public: 

/*! The constructor.
 *
 *\param master A pointer to the corresponding master of the optimization.
 *\param sub A pointer to the subproblem associated with the
 *                 variable. This can be also the 0-pointer.
 *\param dynamic If this argument is \a true, then the variable
 *                         can be removed from the active variable set
 *                         during the subproblem
 *                         optimization.
 *\param local If this argument is \a true, then the constraint
 *                     is considered to be only locally valid. As a local
 *                     variable is associated with a subproblem, \a sub
 *                     must not be 0 if local is \a true.
 *\param lBound The lower bound of the variable.
 *\param uBound The upper bound of the variable.
 *\param varType The type of the variable.
 *\param obj The objective function coefficient of the variable.
 *\param nnz The number of nonzero elements of the variable.
 *\param support The array storing the constraints with the nonzero
 *                        coefficients.
 *\param coeff The nonzero coefficients of the constraints stored
 *                      in \a support.
 */
      ABA_COLVAR(ABA_MASTER *master, 
                 const ABA_SUB *sub, 
                 bool dynamic,  
                 bool local,
                 double lBound, 
                 double uBound,
                 ABA_VARTYPE::TYPE varType, 
                 double obj, 
                 int nnz, 
                 ABA_ARRAY<int> &support, 
                 ABA_ARRAY<double> &coeff);

/*! \brief A constructor 
 * substituting \a nnz, \a support, and \a coeff of the previous
 * constructor by an object of the class ABA_SPARVEC.
 */
      ABA_COLVAR(ABA_MASTER *master, 
                 const ABA_SUB *sub, 
                 bool dynamic,  
                 bool local,
                 double lBound, 
                 double uBound,
                 ABA_VARTYPE::TYPE varType, 
                 double obj, ABA_SPARVEC &vector);

/*! The destructor.
 */
      virtual ~ABA_COLVAR();
#ifdef ABACUS_PARALLEL

/*! The message constructor creates the variable from an ABA_MESSAGE.
 *
 *\param msg The ABA_MESSAGE object from which the variable is initialized.
 */
      ABA_COLVAR(ABA_MASTER *master, ABA_MESSAGE &msg);

/* Packs the data of the variable in an ABA_MESSAGE object.
 *
 *\param msg The ABA_MESSAGE object in which the variable is packed.
 */
      virtual void pack(ABA_MESSAGE &msg) const;

/*! Returns an integer which identifies the class.
 *
 *\return The class identification.
 */
      virtual int classId() const;
#endif

/*!The output operator writes the column representing the variable to an output stream.
 *
 *\return A reference to the output stream.
 *
 *\param out The output stream.
 *\param rhs The variable being output.
 */
      friend ostream &operator<<(ostream &out, const ABA_COLVAR &rhs);

/*! Writes the column representing the variable to an output stream.
 *
 * It redefines the virtual function \a print() of the base class ABA_CONVAR.
 *
 *\param out The output stream.
 */
      virtual void print(ostream &out);

/*! \return The coefficient of the constraint \a con.
 *
 *\param con The constraint of which the coefficient is computed.
 *               This must be a pointer to the class ABA_NUMCON.
 */
      virtual double coeff(ABA_CONSTRAINT *con);

/*! This version of the function \a coeff() computes the coefficient of a constraint with a given number.
 *
 *\return The coefficient of constraint \a i.
 *
 *\param i The number of the constraint.
 */
      double coeff(int i);

/*! \return A pointer to the column representing the variable.
 */
      ABA_COLUMN *column();

    protected:
      ABA_COLUMN   column_;
  };
#endif  // ABA_COLVAR_H

/*! \class ABA_COLVAR
 * \brief Some optimization problems, in particular column generation problems, are
 * better described from a variable point of view instead of a constraint
 * point of view. For such context we provide the class ABA_COLVAR.
 *
 * \param ABA_COLUMN  column_ The column representing the variable.
 */

