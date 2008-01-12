/*!\file
 * \author Matthias Elf
 * \brief poolslot.
 *
 * Constraints or variables are not directly stored in a pool. But are
 * stored in a pool slot, which form again the basic building blocks of
 * a pool. The reason is that in order to save memory it can be necessary
 * that a constraint or variable in the pool has to be deleted although
 * it is still contained in the active formulation of an inactive subproblem.
 * Of course this deletion can be only done with constraints/variables which
 * can be regenerated or which are not required for the correctness of
 * the algorithm (e.g., for a cutting plane, but not for a variable or
 * constraint of the problem formulation of a general \mip).
 *
 * \par
 * Such that the deletion of a variable or constraint cannot cause a
 * run-time error, we store it in a pool slot. Together with the pointer
 * to the constraint/variable we store also its version number. The version
 * number is initially zero and incremented each time when a new item is
 * inserted in the pool slot. When we refer to a constraint/variable,
 * e.g., from the sets of active constraints or variables, then we
 * point to the slot and memorize the version number (class ABA_POOLSLOTREF),
 * when this reference has been set up. Later by comparing the version
 * number of ABA_POOLSLOTREF and the one of the corresponding ABA_POOLSLOT
 * we can check if still the constraint/variable is contained in the slot
 * which is supposed to be there. Usually, if the expected constraint/variable
 * is missing, it is ignored.
 *
 * \warning A ABA_POOLSLOT must not be deleted before the termination
 * of the optimization process, except that it can be guaranteed that
 * there is no reference to this slot from any other place of the program.
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

#ifndef ABA_POOLSLOT_H
#define ABA_POOLSLOT_H

#include "abacus/abacusroot.h"

class ABA_MASTER;
class ABA_SUB;
class ABA_CONSTRAINT;
class ABA_VARIABLE;
template<class BaseType, class CoType> class ABA_POOL;
template<class BaseType, class CoType> class ABA_POOLSLOTREF;
template<class BaseType, class CoType> class ABA_STANDARDPOOL;
template<class BaseType, class CoType> class ABA_NONDUPLPOOL;
template<class BaseType, class CoType> class ABA_CUTBUFFER;

#ifdef ABACUS_PARALLEL
class ABA_ID;
#endif
  template<class BaseType, class CoType> class  ABA_POOLSLOT :  public ABA_ABACUSROOT  {
  friend class ABA_POOLSLOTREF<BaseType,CoType>;
  friend class ABA_POOL<BaseType,CoType>;
  friend class ABA_STANDARDPOOL<BaseType,CoType>;
  friend class ABA_CUTBUFFER<BaseType,CoType>;

  friend class ABA_SUB;
  friend class ABA_POOLSLOTREF<ABA_CONSTRAINT, ABA_VARIABLE>;
  friend class ABA_POOLSLOTREF<ABA_VARIABLE, ABA_CONSTRAINT>;
  friend class ABA_POOL<ABA_CONSTRAINT, ABA_VARIABLE>;
  friend class ABA_POOL<ABA_VARIABLE, ABA_CONSTRAINT>;
  friend class ABA_STANDARDPOOL<ABA_CONSTRAINT, ABA_VARIABLE>;
  friend class ABA_STANDARDPOOL<ABA_VARIABLE, ABA_CONSTRAINT>;
  friend class ABA_NONDUPLPOOL<ABA_CONSTRAINT, ABA_VARIABLE>;
  friend class ABA_NONDUPLPOOL<ABA_VARIABLE, ABA_CONSTRAINT>;
  friend class ABA_CUTBUFFER<ABA_CONSTRAINT, ABA_VARIABLE>;
  friend class ABA_CUTBUFFER<ABA_VARIABLE, ABA_CONSTRAINT>;
    public:

/*! \brief The constructor
 * sets the version number to 1, if already
 * a constraint is inserted in this slot, otherwise it is set to 0.
 *
 *    \param master
 *      A pointer to the corresponding master of the optimization.
 *    \param pool The pool this slot belongs to.
 *    \param conVar The constraint/variable inserted in this slot
 *                      if not 0. The default value is 0.
 */
      ABA_POOLSLOT(ABA_MASTER *master, 
                   ABA_POOL<BaseType, CoType> *pool, 
                   BaseType *convar = 0);

/*! \brief The destructor for the poolslot
 * must not be called if there are references to its constraint/variable.
 */
      ~ABA_POOLSLOT();

/*! \return A pointer to the constraint/variable in the pool slot.
 */
      BaseType *conVar() const;
#ifdef ABACUS_PARALLEL

/*! \brief Returns the system wide identification
 * of the constraint/variable referenced by the ABA_POOLSLOT.
 *
 * \return The ABA_ID of the constraint/variable referenced by the ABA_POOLSLOT.
 */
      const ABA_ID &getIdentification(void) const;

/*! \brief Sets the system wide identification
 * of the constraint/variable referenced by the ABA_POOLSLOT and registers it
 * with the ABA_IDMAP of the pool.
 *
 * \param id The ABA_ID to be assigned.
 */
      void setIdentification(const ABA_ID &id);

/*! \brief Sets the system wide identification
 * of the constraint/variable referenced by the ABA_POOLSLOT to a new ABA_ID and
 * registers it with the ABA_IDMAP of the pool.
 */
      void setNewIdentification();
#endif

    private:

/*! Inserts a constraint/variable  in the slot, and updates the version number.
 *
 * If the slot still contains a constraint, the program stops.
 *
 * The constant \a ULONG_MAX is defined in the file {\tt limits.h}.
 *
 * \param conVar The constraint/variable that is inserted.
 */
      void insert(BaseType *convar);

/*! Tries to remove the item from the slot.
 *
 * This is possible if the function ABA_CONVAR::deletable() returns \a true.
 *
 * \return 0 If the constraint/variable in the slot could be deleted,
 * \return 1 otherwise.
 */
      int softDelete();

/*! Deletes the constraint/variable in the slot.
 *
 * \warning This function should be used very carefully.
 */
      void hardDelete();

/*! Removes the constraint contained in this ABA_POOLSLOT from its own ABA_POOL.
 */
      void removeConVarFromPool();

/*! \return The version number of the constraint/variable of the slot.
 */
      unsigned long version() const;

/*! \return A pointer to the corresponding master of the optimization.
 */
      ABA_MASTER *master();

      /*! A pointer to the corresponding master of the optimization.
       */
      ABA_MASTER *master_;

      /*! A pointer to the constraint/variable.
       */
      BaseType *conVar_;

      /*! The version of the constraint in the slot.
       */
      unsigned long version_;

      /*! A pointer to the corresponding pool.
       */
      ABA_POOL<BaseType, CoType> *pool_;
      ABA_POOLSLOT(const ABA_POOLSLOT<BaseType, CoType> &rhs);
      const ABA_POOLSLOT<BaseType, CoType> 
            &operator=(const ABA_POOLSLOT<BaseType, CoType> &rhs);
  };

#include "abacus/poolslot.inc"

#endif  // ABA_POOLSLOT_H


/*! \class ABA_POOLSLOT
 *  \brief Constraints or variables are not directly stored in a pool. But are
 * stored in a pool slot.
 */


