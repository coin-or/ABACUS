/*!\file
 * \author Matthias Elf
 *
 * \brief branching rule for constraints.
 *
 * This class implements the branching by adding a constraint to the
 * set of active constraints.
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

#ifndef ABA_CONBRANCHRULE_H
#define ABA_CONBRANCHRULE_H

#include "abacus/branchrule.h"
#include "abacus/poolslotref.h"
#include "abacus/constraint.h"
#include "abacus/variable.h"

#ifdef ABACUS_PARALLEL
class ABA_MESSAGE;
#endif

  class  ABA_CONBRANCHRULE :  public ABA_BRANCHRULE  { 
    public: 

/*! The constructor.
 *
 * \note The subproblem associated with the branching constraint will be 
 * modified in the constructor of the subproblem generated with this 
 * branching rule such that later the check for local validity of the 
 * branching constraint is performed correcly.
 *
 * \param master A pointer to the corresponding master of the optimization.
 * \param poolSlot A pointer to the pool slot of the branching constraint.
 */
      ABA_CONBRANCHRULE(ABA_MASTER *master,
                        ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE> *poolSlot);

/*! The destructor.
 */
      virtual ~ABA_CONBRANCHRULE();
#ifdef ABACUS_PARALLEL

/*! The message constructor 
 * creates the ABA_CONBRANCHRULE from an ABA_MESSAGE.
 *
 *  \param master A pointer to the corresponding master of the optimization.
 *  \param msg The message from which the object is initialized.
 */
      ABA_CONBRANCHRULE(ABA_MASTER *master, ABA_MESSAGE &msg);

/*! \brief Packs the data of the ABA_CONBRANCHRULE
 * in an ABA_MESSAGE object. The \a pack() function of the base class
 * has to be called before the local data is packed.
 *
 * \param msg The ABA_MESSAGE object in which the ABA_CONBRANCHRULE is packed.
 */
      virtual void pack(ABA_MESSAGE &msg) const;

/*!\brief Has to be implemented in each derived subclass. It has to return an integer
 * which identifies the class.
 *
 * \return The class identification.
 */
      virtual int classId() const;
#endif

/*! The output operator writes the branching constraint on an output stream.
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param rhs The branch rule being output.
 */
      friend ostream &operator<<(ostream &out, const ABA_CONBRANCHRULE &rhs);

/*! Adds the branching constraint to the subproblem.
 * 
 * Instead of adding it directly to the set of active constraints it is
 * added to the cut buffer. 
 *
 * \return Always 0, since there cannot be a contractiction.
 *
 * \param sub The subproblem being modified.
 */
      virtual int extract(ABA_SUB *sub);

/*! \brief The function \a extract() 
 * is overloaded to modify directly the linear
 * programming relaxation. 
 *
 * This required to evaluate the quality of
 * a branching rule.
 */
      virtual void extract(ABA_LPSUB *lp);
      virtual void unExtract(ABA_LPSUB *lp);

/*! \brief Redefines the virtual function of the base 
 * class ABA_BRANCHRULE in order to initialize the subproblem associated with
 * the branching constraint.
 *
 *\param sub A pointer to the subproblem that is associated with
 *               the branching constraint.
 */
      virtual void initialize(ABA_SUB *sub);

/*! \return A pointer to the branching constraint or a 0-pointer, if this
 * constraint is not available.
 */
      ABA_CONSTRAINT *constraint();
    private: 
      ABA_POOLSLOTREF<ABA_CONSTRAINT, ABA_VARIABLE> poolSlotRef_;
      
      const ABA_CONBRANCHRULE &operator=(const ABA_CONBRANCHRULE &rhs);
  };
#endif  //ABA_CONBRANCHRULE_H

/*! \class ABA_CONBRANCHRULE
 * \brief class implements the branching by adding a constraint to the
 * set of active constraints.
 *
 * \param ABA_POOLSLOTREF poolSlotRef_ A reference to the pool
 *                                     slot of the branching constraints.
 */  

