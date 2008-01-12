/*!\file
 * \author Matthias Elf
 *
 * \brief branchrule.
 *
 * Branching should be very flexible within such a framework.
 * Therefore in a branching step each generated subproblem
 * receives an object of the class ABA_BRANCHRULE. When the subproblem
 * is activated, it copies the active variables, their bounds and statuses,
 * and the active constraints from the father, and then modifies
 * the subproblem according to the branching rule.
 *
 *\par
 * This class is an abstract base class for all branching rules within
 * this framework. We provide by non-abstract derived classes standard branching
 * schemes for setting a binary variable to its lower or upper
 * bound (ABA_SETBRANCHRULE),
 * for setting an integer variable to a certain value (ABA_VALBRANCHRULE),
 * for changing the bounds of an integer variable (ABA_CONBRANCHRULE), and
 * for adding a branching constraint (ABA_CONBRANCHRULE).
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

#ifndef ABA_BRANCHRULE_H
#define ABA_BRANCHRULE_H

#include "abacus/array.h"

class ABA_MASTER;
class ABA_SUB;
class ABA_LPSUB;

#ifdef ABACUS_PARALLEL
#define SETBRANCHRULE_CLASSID   9001            // preliminary
#define VALBRANCHRULE_CLASSID   9002            // preliminary
#define CONBRANCHRULE_CLASSID   9003            // preliminary
#define BOUNDBRANCHRULE_CLASSID 9004            // preliminary
#endif

  class  ABA_BRANCHRULE :  public ABA_ABACUSROOT  { 
    public: 

/*! The constructor.
 *
 * \param master A pointer to the corresponding master of the optimization.
 */
      ABA_BRANCHRULE(ABA_MASTER *master);

/*! The destructor.
 */
      virtual ~ABA_BRANCHRULE();
#ifdef ABACUS_PARALLEL

/*!The message constructor creates the ABA_BRANCHRULE from an ABA_MESSAGE.
 *
 * \param master A pointer to the corresponding master of the optimization.
 * \param msg The message from which the object is initialized.
 */
      ABA_BRANCHRULE(ABA_MASTER *master, ABA_MESSAGE &msg);

/*! Packs the data of the ABA_BRANCHRULE in an |ABA_MESSAGE| object. 
 *
 * This virtual function has to be redefined
 * in a derived class if additional data should be packed. In this
 * case the \a pack() function of the base class must be called
 * by the \a pack() function of the derived class.
 *
 * \param msg The ABA_MESSAGE object in which the ABA_BRANCHRULE is packed.
 */
      virtual void pack(ABA_MESSAGE &msg) const;
      virtual int classId() const = 0;
#endif

/*! Modifies a subproblem by setting the branching variable.
 *
 * \return 0 If the subproblem can be modified according to the branching rule.
 * \return 1 If a contradiction occurs.
 *
 * \param sub The subproblem being modified.
 */
      virtual int extract(ABA_SUB *sub) = 0;

/*! \brief Should modify the linear programming relaxation |lp| in
 * order to determine the quality of the branching rule in a linear
 * programming based branching rule selection. 
 *
 * The default implementation
 * does nothing except writing a warning to the error stream. If a
 * derived concrete branching rule should be used in LP-based branching
 * rule selection then this function has to be redefined.
 *
 * \param lp A pointer to a the linear programming relaxtion of a
 * a subproblem.
 */
      virtual void extract(ABA_LPSUB *lp);

/*! Should undo the modifictions of the linear programming relaxtion |lp|. 
 *
 * This function has to be redefined
 * in a derived class, if also extract(ABA_LPSUB*) is redefined there.
 *
 * \param lp A pointer to a the linear programming relaxtion of a
 * a subproblem.
 */
      virtual void unExtract(ABA_LPSUB *lp);

/*! \brief Should indicate if the branching
 * is performed by setting a binary variable. 
 *
 * This is only required as in the
 * current version of the GNU-compiler run time type information
 * is not satisfactorily implemented.
 *
 * \par
 * This function is currently required to determine global validity
 * of Gomory cuts for general \a s.
 *
 * \return The default implementation 
 * returns always false. This function must be redefined in the
 * class ABA_SETBRANCHRULE, where it has to return \a true.
 */
      virtual bool branchOnSetVar();

/*! \brief The function initialize is a virtual dummy function doing nothing.
 * It is called from the constructor of the subproblem and can be used
 * to perform initializations of the branching rule that can be only
 * done after the generation of the subproblem.
 *
 * \param sub A pointer to the subproblem that should be used for the
 *  initialization.}
 */
      virtual void initialize(ABA_SUB* sub);
    protected: 
      ABA_MASTER *master_;
  };

#endif  // !ABA_BRANCHRULE_H

/*! \class ABA_BRANCHRULE
 * \brief class is an abstract base class for all branching rules within
 * this framework.
 *
 *   \param ABA_MASTER *master_ A pointer to the corresponding master of the 
 *                        optimization.
 */


