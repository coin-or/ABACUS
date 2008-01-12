/*!\file
 * \author Matthias Elf
 *
 * \brief This class implements a branching rule for modifying the lower and
 * the upper bound of a variable.
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

#ifndef ABA_BOUNDBRANCHRULE_H
#define ABA_BOUNDBRANCHRULE_H

#include "abacus/branchrule.h"

#ifdef ABACUS_PARALLEL
class MESSAGE;
#endif
  class  ABA_BOUNDBRANCHRULE :  public ABA_BRANCHRULE  { 
    public: 

/*! The constructor.
 *
 * \param master A pointer to the corresponding master of the optimization.
 * \param variable The branching variable.
 * \param lBound The lower bound of the branching variable.
 * \param uBound The upper bound of the branching variable.
 */
      ABA_BOUNDBRANCHRULE(ABA_MASTER *master, int variable,
                          double lBound, double uBound);

/*! The destructor.
 */
      virtual ~ABA_BOUNDBRANCHRULE();
#ifdef ABACUS_PARALLEL

/*! The message constructor creates the ABA_BOUNDBRANCHRULE from an ABA_MESSAGE.
 *
 * \param master A pointer to the corresponding master of the optimization.
 * \param msg The message from which the object is initialized.
 */
      ABA_BOUNDBRANCHRULE(ABA_MASTER *master, ABA_MESSAGE &msg);

/*! \brief The virtual function \a pack() packs the data of the ABA_BOUNDBRANCHRULE
 *  in an ABA_MESSAGE object. The \a pack() function of the base class
 *  has to be called before the local data is packed.
 *
 *  \param msg The ABA_MESSAGE object in which the ABA_BOUNDBRANCHRULE is packed.
 */
      virtual void pack(ABA_MESSAGE &msg) const;

/*! \brief The abstract virtual function \a classId() of ABA_BRANCHRULE 
 * has to be implemented in each derived subclass. It has to return an integer
 * which identifies the class.
 *
 * \return The class identification.
 */
      virtual int classId() const;
#endif

/*! \brief The output operator 
 * writes the branching variable together with its
 * lower and upper bound to an output stream.
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param rhs The branch rule being output.
 */
      friend ostream &operator<<(ostream &out, const ABA_BOUNDBRANCHRULE &rhs);
      
/*! \brief Modifies a
 * subproblem by changing the lower and the upper bound of the
 * branching variable.
 *
 * \return 0 If the subproblem is successfully modified.
 * \return 1 If the modification causes a contradiction.
 *
 * \param sub The subproblem being modified.
 */
      virtual int extract(ABA_SUB *sub);

/*! \brief Is overloaded to modify directly the linear
 *  programming relaxation. 
 *
 * This required to evaluate the quality of
 * a branching rule.
 */
      virtual void extract(ABA_LPSUB *lp);
      virtual void unExtract(ABA_LPSUB *lp);

/*! \return The number of the branching variable.
 */
      int variable() const;

/*! \return The lower bound of the branching variable.
 */
      double lBound() const;

/*! \return The upper bound of the branching variable.
 */
      double uBound() const;

    private: 
      int    variable_;
      double lBound_;
      double uBound_;
      double oldLpLBound_;
      double oldLpUBound_;
  };


inline int ABA_BOUNDBRANCHRULE::variable() const
  {
    return variable_;
  }

inline double ABA_BOUNDBRANCHRULE::lBound() const
  {
    return lBound_;
  }

inline double ABA_BOUNDBRANCHRULE::uBound() const
  {
    return uBound_;
  }


#endif  // !ABA_BOUNDBRANCHRULE_H

/*! \class ABA_BOUNDBRANCHRULE
 *  \brief class implements a branching rule for modifying the lower and
 *  the upper bound of a variable.
 *
 *  \param int variable_ The branching variable.
 *
 *  \param double lBound_ The lower bound of the branching variable.
 *
 *  \param double uBound_ The upper bound of the branching variable.
 */

