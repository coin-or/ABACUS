/*!\file
 * \author Matthias Elf
 * \brief standard pool without constraint duplication.
 *
 * The class ABA_NONDUPLPOOL provides an ABA_STANDARDPOOL with the additional feature
 * that the same constraint is at most stored once in the pool. For constraints
 * and variables inserted in this pool the virtual member functions
 * \a name(), \a hashKey(), and \a equal() of the base class ABA_CONVAR have to be
 * defined. Using these three functions, we check at insertation time
 * if a constraint or variable is already stored in the pool.
 *
 * \par
 * The implementation is unsafe in the sense that the data structure for 
 * registering a constraint is corrupted if a constraint is removed directly
 * from the pool slot without using a function of this class.
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


#ifndef ABA_NONDUPLPOOL_H
#define ABA_NONDUPLPOOL_H

#include "abacus/global.h"
#include "abacus/hash.h"
#include "abacus/standardpool.h"
#include "abacus/exceptions.h"

  template<class BaseType, class CoType>
  class  ABA_NONDUPLPOOL :  public ABA_STANDARDPOOL<BaseType, CoType>  { 
    public: 

/*! The constructor for an empty pool.
 *
 * \param master A pointer to the corresponding master of the optimization.
 * \param size The maximal number of items which can be inserted in
 *                     the pool without reallocation.
 * \param autoRealloc If this argument is \a true an automatic
 *                            reallocation is performed if the pool is
 *                            full.
 */
      ABA_NONDUPLPOOL(ABA_MASTER *master, int size, bool autoRealloc = false);


//! The destructor.
      virtual ~ABA_NONDUPLPOOL();

/*! \brief Before the function \a insert()
 * tries to insert a constraint/variable
 * in the pool, it checks if the constraint/variable is already contained in the
 * pool. If the constraint/variable \a cv is contained in the pool, 
 * it is deleted.
 *
 * \return A pointer to the pool slot where the item has been
 * inserted, or a pointer to the pool slot if the item is already contained
 * in the pool, or 0 if the insertion failed.
 *
 * \param cv The constraint/variable being inserted.
 */
      virtual ABA_POOLSLOT<BaseType, CoType> *insert(BaseType *cv);

/*! Checks if a constraint/variables is already  contained in the pool.
 *
 * \return A pointer to the pool slot storing a constraint/variable that is equivalent
 * to \a cv according to the function ABA_CONVAR::equal(). If there is no
 * such constraint/variable 0 is returned.
 *
 * \param cv A pointer to a constraint/variable for which it should
 *               be checked if an equivalent item is already contained
 *               in the pool.
 */
      virtual ABA_POOLSLOT<BaseType, CoType> *present(BaseType *cv);

/*! Enlarges the pool to store. 
 *
 * To avoid fatal errors we do not allow
 * decreasing the size of the pool.
 * This function redefines the virtual function of the base class 
 * ABA_STANDARDPOOL because we have to reallocate the hash table.
 *
 * \param size The new size of the pool.
 */
      virtual void increase(int size);

/*! \brief Determines the number of constraints that 
 * have not been inserted into the pool, because an equivalent was already
 * present.
 *
 * Also the number of collisions in the hash table is computed.
 * If this number is high, it might indicate that your hash function is not
 * chosen very well.
 *
 * \param nDuplications The number of constraints that have not been 
 *                      inserted
 *                      into the pool because an equivalent one was already
 *                      present.
 * \param nCollisions The number of collisions in the hash table.
 */
      void statistics(int &nDuplications, int &nCollisions) const;
    private: 

/*! \brief Has to be redefined because the
 * slot has to be removed from the hash table if the
 * constraint/variable can be deleted.
 *
 * \return 0 If the constraint/variable could be deleted.
 * \return 1 otherwise.
 *
 * \param slot A pointer to the pool slot from wich the constraint/variable
 *                 should be deleted.
 */
      virtual int softDeleteConVar(ABA_POOLSLOT<BaseType, CoType> *slot);

/*! Has to be redefined because the pool slot has to be removed from the hash table.
 *
 * \param slot A pointer to the pool slot from wich the constraint/variable
 *                 should be deleted.
 */
      virtual void hardDeleteConVar(ABA_POOLSLOT<BaseType, CoType> *slot);
      ABA_HASH<unsigned, ABA_POOLSLOT<BaseType, CoType>*> hash_;
      int nDuplications_;
      ABA_NONDUPLPOOL(const ABA_NONDUPLPOOL &rhs);
      const ABA_NONDUPLPOOL &operator=(const ABA_NONDUPLPOOL &rhs);
  };

#include "abacus/nonduplpool.inc"

#endif  // ABA_NONDUPLPOOL_H




/*! \class ABA_NONDUPLPOOL
 * \brief class ABA_NONDUPLPOOL provides an ABA_STANDARDPOOL with the additional feature
 * that the same constraint is at most stored once in the pool. For constraints
 * and variables inserted in this pool the virtual member functions
 * \a name(), \a hashKey(), and \a equal() of the base class ABA_CONVAR have to be
 * defined
 *
 *\param hash_ A hash table for a fast access to the pool slot storing
 *                                      a constraint/variable.
 *
 *\param nDuplications_ The number of insertions of constraints/variables
 *                          that were rejected since the constraint/variable is 
 *                          stored already in the pool.
 */ 

