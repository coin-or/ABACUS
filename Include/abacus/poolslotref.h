/*!\file
 * \author Matthias Elf
 * \brief poolslotref
 *
 * As already explained in the class ABA_POOLSLOT we do not refer directly
 * to constraints/variables but store a pointer to a pool slot and
 * memorize the version number of the slot at initialization time of the
 * class ABA_POOLSLOTREF.
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

#ifndef ABA_POOLSLOTREF_H
#define ABA_POOLSLOTREF_H

#include "abacus/abacusroot.h"

#include <iostream>
class ABA_MASTER;
template<class BaseType, class CoType> 
class ABA_POOLSLOT;

template<class BaseType, class CoType>
class  ABA_POOLSLOTREF;

template<class BaseType, class CoType>
ostream &operator<< (ostream &out, const ABA_POOLSLOTREF<BaseType, CoType> &rhs);

template<class BaseType, class CoType>
class  ABA_POOLSLOTREF :  public ABA_ABACUSROOT {
    public:

/*! This constructor generates an object referencing to no pool slot.
 *
 * \param master A pointer to the corresponding master of the optimization.
 */
      ABA_POOLSLOTREF(ABA_MASTER *master);

/*! This constructor initializes the reference to a pool slot with a given slot.
 *
 *  Also the constraint/variable contained in this slot receives a
 *  message that a new references to it is created.
 *
 * \param slot The pool slot that is referenced now.
 */
      ABA_POOLSLOTREF(ABA_POOLSLOT<BaseType, CoType> *slot);

/*! \brief The copy constructor
 * may increments the reference counter of the
 * constraint/variable only if version number of the slot and
 * version number of the reference are equal, since otherwise
 * this is not a correct reference to \a slot_->conVar().
 *
 * \param rhs The pool slot that is copied in the initialization process.
 */
      ABA_POOLSLOTREF(const ABA_POOLSLOTREF<BaseType, CoType> &rhs);

/*! \brief The destructor
 * sends a message to the constraint that it
 * will no longer be referred from this place in the program.
 *
 * If the version number of the reference and the version number
 * of the slot do not equal, we must not decrement the
 * reference counter of \a slot_->conVar() because this is
 * not a correct reference to this constraint/variable.
 */ 
      ~ABA_POOLSLOTREF();

/*! \brief The output operator
 * writes the constraint/variable stored in the
 * referenced slot to an output stream.
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param rhs The reference to a pool slot being output.
 */
      friend ostream &operator<< <> (ostream &, const ABA_POOLSLOTREF &);

/*! \return A pointer to the constraint/variable
 * stored in the referenced slot if the version number of the slot
 * is equal to the version number at construction/initialization time
 * of this slot. Otherwise, it returns 0.
 */
      BaseType *conVar() const;

/*! \return The version number of the
 * constraint/variable stored in the referenced slot at construction
 * time of the reference to this slot.
 */
      unsigned long version() const;

/*! \return A pointer to the referenced slot.
 */
      ABA_POOLSLOT<BaseType, CoType> *slot() const;

/*! \brief This version of the function \a slot()
 * initializes the referenced pool slot.
 *
 * The function \a slot() may decrement the reference counter
 * of \a slot_->conVar() only if the version number of the reference
 * and the version number of the slot are equal since otherwise this
 * is not a valid reference.
 *
 * \param s The new slot that is referenced. This must not be a 0-pointer.
 */
      void slot(ABA_POOLSLOT<BaseType, CoType> *s);

    private:

      /*! A pointer to the corresponding master of the optimization.
       */
      ABA_MASTER *master_;

      /*! A pointer to the referenced pool slot.
       */
      ABA_POOLSLOT<BaseType, CoType> *slot_;

      /*! \brief The version number of the slot at 
       *  construction/initialization time of this reference.
       */
      unsigned long version_;
      void printDifferentVersionError() const;
      const ABA_POOLSLOTREF<BaseType, CoType>
                      &operator=(const ABA_POOLSLOTREF<BaseType, CoType> &rhs);
  };

#include "abacus/poolslotref.inc"

#endif  // ABA_POOLSLOTREF_H

/*! \class ABA_POOLSLOTREF
 *  \brief we do not refer directly
 * to constraints/variables but store a pointer to a pool slot and
 * memorize the version number of the slot at initialization time of the
 * class ABA_POOLSLOTREF.
 */

