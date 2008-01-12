/*!\file
 * \author Matthias Elf
 * \brief branching rule for values.
 *
 * This class implements a branching rule for setting a variable to a certain
 * value.
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

#ifndef ABA_VALBRANCHRULE_H
#define ABA_VALBRANCHRULE_H

#include "abacus/branchrule.h"

#ifdef ABACUS_PARALLEL
class ABA_MESSAGE;
#endif

  class  ABA_VALBRANCHRULE :  public ABA_BRANCHRULE  { 
    public: 

/*! The constructor.
 *
 *    \param master The corresponding master of the optimization.
 *    \param variable The branching variable.
 *    \param value The value the branching variable is set to.
 */
      ABA_VALBRANCHRULE(ABA_MASTER *master, int variable, double value);

//! The destructor.
      virtual ~ABA_VALBRANCHRULE();
#ifdef ABACUS_PARALLEL

/*! \brief The message constructor
 *   creates the ABA_VALBRANCHRULE from an ABA_MESSAGE.
 *
 *     \param
 *       master A pointer to the corresponding master of the optimization.
 *     \param
 *       msg The message from which the object is initialized.
 */
      ABA_VALBRANCHRULE(ABA_MASTER *master, ABA_MESSAGE &msg);

/*! Packs the data of the ABA_VALBRANCHRULE in an ABA_MESSAGE object.
 *
 *   The \a pack() function of the base class
 *   has to be called before the local data is packed.
 *
 *   \param msg The ABA_MESSAGE object in which the ABA_VALBRANCHRULE is packed.
 */
      virtual void pack(ABA_MESSAGE &msg) const;

/*! \brief The abstract virtual function \a classId()
 *   of ABA_BRANCHRULE has to be implemented in each derived subclass.
 *
 *   It has to return an integer which identifies the class.
 *
 *   \return The class identification.
 */
      virtual int classId() const;
#endif

/*! \brief The output operator
 *   writes the branching variable together with its value to an output stream.
 *
 *   \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param rhs The branching rule being output.
 */
      friend ostream &operator<<(ostream &out, const ABA_VALBRANCHRULE &rhs);

/*! Modifies a subproblem by setting the branching variable.
 *
 *   \return 0 If the subproblem can be modified according to the
 *                branching rule.
 *   \return 1 If a contradiction occurs.
 *
 *   \param sub The subproblem being modified.
 */
      virtual int extract(ABA_SUB *sub);

/*! \brief The function \a extract()
 *   is overloaded to modify directly the linear
 *   programming relaxation. This required to evaluate the quality of
 *   a branching rule.
 */
      virtual void extract(ABA_LPSUB *lp);

//! The function \a unExtract().
      virtual void unExtract(ABA_LPSUB *lp);

/*! \return The number of the branching variable.
 */
      int variable() const;

/*! \return The value of the branching variable.
 */
      double value() const;
    private: 
      int    variable_;
      double value_;
      double oldLpLBound_;
      double oldLpUBound_;
  };


inline int ABA_VALBRANCHRULE::variable() const
  {
    return variable_;
  }

inline double ABA_VALBRANCHRULE::value() const
  {
    return value_;
  }


#endif  // ABA_VALBRANCHRULE_H


 /*! \class ABA_VALBRANCHRULE
 * \brief class implements a branching rule for setting a variable to a certain value.
 *
 *  \param int variable_ The branching variable.
 *
 *  \param double value_ The value the branching variable is set to.
 */

