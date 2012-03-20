/*!\file
 * \author Matthias Elf
 * \brief branching rule for setting.
 *
 * This class implements a branching rule for setting a binary variable
 * to its lower or upper bound.
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

#ifndef ABA_SETBRANCHRULE_H
#define ABA_SETBRANCHRULE_H

#include "abacus/branchrule.h"
#include "abacus/fsvarstat.h"


  class  ABA_SETBRANCHRULE :  public ABA_BRANCHRULE  { 
    public: 

/*! The constructor.
 *
 *    \param master A pointer to the corresponding master of the optimization.
 *    \param variable The branching variable.
 *    \param status The status the variable is set to
 *                    (SetToLowerBound or \a SetToUpperBound).
 */
      ABA_SETBRANCHRULE(ABA_MASTER *master, int variable, 
                        ABA_FSVARSTAT::STATUS status);

//! The destructor.
      virtual ~ABA_SETBRANCHRULE();

/*! \brief The output operator
 *   writes the number of the branching variable and its status on an output stream.
 *
 *   \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param rhs The branching rule being output.
 */
      friend ostream &operator<<(ostream &out, const ABA_SETBRANCHRULE &rhs);
      

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
 *   is overloaded to modify directly the linear programming relaxation.
 *
 *   This required to evaluate the quality of
 *   a branching rule with linear programming methods. The changes
 *   have to be undone with the function \a unextract() before the next linear
 *   program is solved.
 *
 *   \param lp A pointer to the linear programming relaxation of a 
 *                 subproblem.
 */
      virtual void extract(ABA_LPSUB *lp);

//! The function \a unExtract().
      virtual void unExtract(ABA_LPSUB *lp);

/*! \brief Redefines the virtual function
 *   of the base class ABA_BRANCHRULE as this branching rule is setting
 *   a binary variable.
 *
 *   \return Always \a true.
 */
      virtual bool branchOnSetVar();

/*! \return true If the branching variable is set to the upper bound,
 *   \return false otherwise.
 */
      bool setToUpperBound() const;

/*! \return The number of the branching variable.
 */
      int variable() const;

    private: 
      int                   variable_;
      ABA_FSVARSTAT::STATUS status_;
      double                oldLpBound_;
  };



inline int ABA_SETBRANCHRULE::variable() const
  {
    return variable_;
  }


#endif  // ABA_SETBRANCHRULE_H

/*! \class ABA_SETBRANCHRULE
 * \brief The data members of the class ABA_SETBRANCHRULE.
 *
 *  \param int variable_ The branching variable.
 *
 *  \param ABA_FSVARSTAT::STATUS status_ The status of the 
 *  branching variable (\a SetToLowerBound or \a SetToUpperBound).
 *
 *  \param double oldLpbound_ The bound of the branching 
 *  variable in the linear program,
 *  before it is temporarily modified for testing the quality of
 *  this branching rule.
 */
