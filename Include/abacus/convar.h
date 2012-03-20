/*!\file
 * \author Matthias Elf
 *
 * \brief constraints and variables.
 *
 * ABA_CONVAR is the common base class for constraints and variables, which
 * are implemented in the derived classes ABA_CONSTRAINT and ABA_VARIABLE,
 * respectively.
 * It might seem a bit strange to implement a common base class for
 * these two objects. Besides several technical reasons, there is
 * linear programming duality which motivates this point of view.
 * E.g., the separation problem for the primal problem is equivalent
 * to the pricing problem for the dual problem.
 *
 * \par
 * ABA_CONVAR is {\bf not} the base class for constraints and variables
 * as they are used in the interface to the linear programming solver.
 * There are the classes ABA_ROW and ABA_COLUMN for this purpose. ABA_CONVAR is
 * the father of a class hierarchy for abstract constraints and
 * variables which are used in the \bab\ algorithm.
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


#ifndef ABA_CONVAR_H
#define ABA_CONVAR_H

#include "abacus/abacusroot.h"

#include <iostream>
using namespace std;

class ABA_MASTER;
class ABA_SUB;
class ABA_VARIABLE;
class ABA_CONSTRAINT;
// class ostream;

template<class BaseType, class CoType> class ABA_POOLSLOT;
template<class BaseType, class CoType> class ABA_POOLSLOTREF;
template<class BaseType, class CoType> class ABA_STANDARDPOOL;
template<class BaseType, class CoType> class ABA_CUTBUFFER;

class  ABA_CONVAR : public ABA_ABACUSROOT  {
  friend class ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE>;
  friend class ABA_POOLSLOT<ABA_VARIABLE, ABA_CONSTRAINT>;
  friend class ABA_POOLSLOTREF<ABA_CONSTRAINT, ABA_VARIABLE>;
  friend class ABA_POOLSLOTREF<ABA_VARIABLE, ABA_CONSTRAINT>;
  friend class ABA_STANDARDPOOL<ABA_CONSTRAINT, ABA_VARIABLE>;
  friend class ABA_STANDARDPOOL<ABA_VARIABLE, ABA_CONSTRAINT>;
  friend class ABA_CUTBUFFER<ABA_CONSTRAINT, ABA_VARIABLE>;
  friend class ABA_CUTBUFFER<ABA_VARIABLE, ABA_CONSTRAINT>;
  friend class ABA_SUB;
  public:

/*! The constructor.
 *
 *     \param master A pointer to the corresponding master of the optimization.
 *     \param sub A pointer the subproblem the constraint/variable is associated with.
 *        If the item is not associated with any subproblem, then this
 *        can also be the 0-pointer.
 *     \param dynamic If this paramument is \a true, then the constraint/variable can also be
 *        removed again from the set of active constraints/variables after it is
 *        added once.
 *     \param local If \a local is \a true, then the constraint/variable is only locally
 *        valid.
*/
    ABA_CONVAR (ABA_MASTER *master, const ABA_SUB *sub, bool dynamic, bool local);

/*! The destructor.
 */
    virtual ~ABA_CONVAR();

/*! Checks if the constraint/variable is active in at least one active subproblem. 
 *
 *   In the parallel implementation
 *   this can be more than one subproblem when multithreading occurs.
 *   Only those subproblems are taken into account which are related to the
 *   pool in which the constraint/variable is stored.
 *
 *
 *   \return true If the constraint/variable is active,
 *   \return false otherwise.
 */
    bool active() const;

/*! \return true If the constraint/variable  is only locally valid,
 * \return false otherwise.
 */
    bool local() const;

/*! \return true If the constraint/variable is globally valid,
 *   \return false otherwise.
 */
    bool global() const;

/*! \return true If the constraint/variable can be also removed from the
 *                   set of active constraints/var\-i\-a\-bles
 *                   after it has been activated,
 *   \return false otherwise.
 */
    virtual bool dynamic() const;

/*! @name 
 *   Constraints/Variables often have to be stored in a format different
 *   from the format used in the linear program. One reason is to save
 *   memory and the other reason is that if constraints and/or variable
 *   sets are dynamic, then we require a format to compute the coefficients
 *   of later activated variables/constraints.
 *
 * \par
 *   The disadvantage of such a constraint format is that the computation
 *   of a single constraint coefficient can be very time consuming. Often
 *   it cannot be done in constant time. Hence we provide a mechanism
 *   which converts a constraint/variable to a format enabling efficient
 *   computation of coefficients. The following functions provide this
 *   feature.
 */
//@{
/*! \return true If the expanded format of a constraint/variable is available,
 *   \return false otherwise.
 */
    bool expanded() const;

/*! The default implementation of the function \a expand() is void.
 *   It should be redefined in derived classes.
 */
    virtual void expand();

/*! Also the default implementation of the function \a compress() is void.
 *  It should be redefined in derived classes.
 */
    virtual void compress();

/*! Returns \a true if the constraint/variable can be destructed. 
 *
 * This is per default only possible if the
 * reference counter is 0 and no lock is set.
 * The function is declared virtual such
 * that problem specific implementations are possible.
 */
    virtual bool deletable() const;

/*! The function writes the constraint/variable on the stream \a out.
 *   
 *   This function is used since the output operator cannot be declared
 *   virtual. The default implementation writes
 *   \a "ABA_CONVAR::print() is only a dummy." on the stream \a out.
 *   We do not declare this function pure virtual since it is not really
 *   required, mainly only for debugging. In this case a constraint/variable
 *   specific redefinition is strongly recommended.
 *
 *   Normally, the implementation \a out << *this should be sufficient.
 *
 *   \param out The output stream.
 */
    virtual void print(ostream &out);
//@}

/*! \return A pointer to the subproblem
 *   associated with the constraint/variable. Note, this can also
 *   be the 0-pointer.
 */
    const ABA_SUB *sub() const;

/*! \brief This version of the function \a sub() 
 * associates a new subproblem with the constraint/variable.
 *
 *   \param sub The new subproblem associated with the constraint/variable.
 */
    void sub(ABA_SUB *sub);

/*! \brief Should provide a key for the constraint/variable 
 *   that can be used to insert it into a hash table.
 *   
 *   As usual for hashing, it is not required that any two items have
 *   different keys.
 *
 * \par
 *   This function is required if the constraint/variable is stored in a
 *   pool of the class ABA_NONDUPLPOOL.
 *
 * \par
 *   The default implementation shows a warning and calls \a exit().
 *   This function is not a pure virtual function because in the default
 *   version of \ABACUS\ it is not required.
 *
 * \par
 *   We do not use \a double as result type because typical problems in
 *   floating point arithmetic might give slightly different hash keys
 *   for two constraints that are equal from a mathematical point of
 *   view.
 *
 * \par
 *   The redundant return statement is required to suppress compiler warnings.
 *
 *   \return An integer providing a hash key for the constraint/variable.
*/
    virtual unsigned hashKey();

/*! Should return the name of the constraint/variable. 
 *
 *   This function is required to emulate a simple
 *   run time type information (RTTI) that is still missing in \GPLUSPLUS/.
 *   This function will be removed as soon as RTTI is supported sufficiently.
 *
 * \par
 *   A user must take care that for each redefined version of this
 *   function in a derived class a unique name is returned. Otherwise
 *   fatal run time errors can occur. Therefore, we recommend to return
 *   always the name of the class.
 *
 * \par
 *   This function is required if the constraint/variable is stored in a
 *   pool of the class ABA_NONDUPLPOOL.
 *
 * \par
 *   The default implementation shows a warning and calls \a exit().
 *   This function is not a pure virtual function because in the default
 *   version of \ABACUS\ it is not required.
 *
 * \par
 *   The redundant return statement is required to suppress compiler warnings.
 *
 * \return The name of the constraint/variable.
 */
    virtual const char *name();

/*! \brief Should compare if the constraint/variable
 *   is identical (in a mathematical sense) with the constraint/variable
 *   \a cv. 
 *
 *   Using RTTI or its emulation provided by the function \a name()|
 *   it is sufficient to implement this functions for
 *   constraints/variables of the same type.
 *
 * \par
 *   This function is required if the constraint/variable is stored in a
 *   pool of the class ABA_NONDUPLPOOL.
 *
 * \pat
 *   The default implementation shows a warning and calls \a exit().
 *   This function is not a pure virtual function because in the default
 *   version of \ABACUS\ it is not required.
 *
 * \par
 *   The redundant return statement is required to suppress compiler warnings.
 *
 *   \return true If the constraint/variable represented by this object
 *                   represents the same item as the constraint/variable
 *                   \a cv,
 *   \return false otherwise.
 *
 *   \param cv The constraint/variable that should be compared with
 *                 this object.
*/
    virtual bool equal(ABA_CONVAR *cv);

/*! \brief The function should return a rank associated with the constraint/variable.
 *  The default implementation returns 0.
 *
 * \return The rank of the constraint/variable.
*/
    virtual double rank();
  protected:

    /*! A pointer to the corresponding master of the optimization.
     */
    ABA_MASTER *master_;

    /*! \brief A pointer to the subproblem associated with the constraint/variable.
     *  This may be also the 0-pointer.
     */
    const ABA_SUB *sub_;

    /*! \a true, if expanded version of constraint/variables available.
     */
    bool expanded_;

    /*! \brief The number of references to the pool slot
     *  the constraint is stored ABA_POOLSLOTREF.
     */
    int nReferences_;

    /*! \brief If this member is \a true then the constraint/variable
     * can be also removed from the active formulation
     * after it is added the first time.
     * For constraints/variables which should be 
     * never removed from the active formulation
     * this member should be set to \a false.
     */
    bool dynamic_;

    /*! \brief The number of active subproblems of which
     *  the constraint/variable belongs to the set of
     *  active constraints/variables.
     *
     *  This value is always 0 after construction and
     *  has to be set and reset during the subproblem
     *  optimization. This member is mainly used to accelerate
     *  pool separation and to control that the same variable
     *  is not multiply included into a set of active variables.
     */
    int nActive_;

    /*! The number of locks which have been set on the constraint/variable.
     */
    int nLocks_;

    /*! \brief This flag is \a true if the constraint/variable is only locally valid,
     *  otherwise it is false.
     */
    bool local_;

  private:


/*! Tries to generate the expanded format of the constraint/variable. 
 *
 * This will be only possible if the virtual
 *   function \a expand() is redefined for the specific constraint/variable.
 */
    void _expand();


/*! Removes the expanded format of the constraint/variable. 
 *
 * This will be only possible if the virtual
 *   function \a compress() is redefined for the specific constraint/variable.
 */
    void _compress();

/*! \brief Must be called if the constraint/variable
 * is added to the active formulation of an active subproblem.
 *
 * This function is only called within member functions of the class ABA_SUB.
 */

    void activate();

/*! \brief Is the counterpart to \a activate()
 * and is also called within members of the class ABA_SUB to indicate
 * that the constraint/variable does not belong any more to the active
 * formulation of an active subproblem.
 */
    void deactivate();

/*! \brief Returns the number of references to the
 * pool slot ABA_POOLSLOTREF storing this constraint/variable.
 *
 * We require the bookkeeping of the references in order to determine
 * if a constraint/variable can be deleted without causing any harm.
 */
    int nReferences() const;

/*! \brief Indicates that there is a new reference to the
 * pool slot storing this constraint/variable. 
 *
 * The function is only
 * called from members of the class ABA_POOLSLOTREF.
 */

    void addReference();

/*! \brief Is the counterpart of the function
 * \a addReference()
 *  and indicates the removal of a reference to this constraint. 
 *
 * It is only called from members of the class ABA_POOLSLOTREF.
 */
    void removeReference();

/*! @name
 *   If a constraint/variable has just been separated and added to
 *   the buffer of currently separated constraints/variables, then
 *   this item should not be removed before the buffer is emptied
 *   at the beginning of the next iteration. Hence, we provide a
 *   locking mechanism for constraints/variables by the following
 *   three functions.
 */
//@{ 
/*! \return \a true if at least one lock is set on the constraint/variable
 * \return \a false otherwise.
 */
    bool locked() const;

/*! \brief Adds an additional lock to the constraint/variable.
 */
    void lock();

/*! Removes one lock from the constraint/variable.
 */
    void unlock();
//@}


};

inline bool ABA_CONVAR::active() const
  {
    if (nActive_) return true;
    else          return false;
  }

inline void ABA_CONVAR::activate()
  {
    ++nActive_;
  }

inline int ABA_CONVAR::nReferences() const
  {
    return nReferences_;
  }

inline void ABA_CONVAR::addReference()
  {
    ++nReferences_;
  }

inline bool ABA_CONVAR::locked() const
  {
    if (nLocks_) return true;
    else         return false;
  }

inline void ABA_CONVAR::lock()
  {
    ++nLocks_;
  }

inline bool ABA_CONVAR::local() const
  {
    return local_;
  }

inline bool ABA_CONVAR::global() const
  {
    return !local_;
  }

inline const ABA_SUB *ABA_CONVAR::sub() const
  {
    return sub_;
  }

inline void ABA_CONVAR::sub(ABA_SUB *sub)
  {
    sub_ = sub;
  }

inline bool ABA_CONVAR::expanded() const
  {
    return expanded_;
  }


#endif  // !ABA_CONVAR_H

/*! \class ABA_CONVAR
 * \brief ABA_CONVAR is the common base class for constraints and variables, which
 * are implemented in the derived classes ABA_CONSTRAINT and ABA_VARIABLE,
 * respectively.
 */


