/*!\file
 * \author Matthias Elf
 * \brief open subproblems.
 *
 *  During a \bab\ algorithm a set of open subproblems has to be maintained.
 *  New subproblems are inserted
 *  in this set after a branching step, or when a subproblem
 *  becomes dormant. A subproblem is extracted from this
 *  list if it becomes the active subproblem which is optimized.
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


#ifndef ABA_OPENSUB_H
#define ABA_OPENSUB_H


#include "abacus/dlist.h"


class ABA_MASTER;
class ABA_SUB;

  class  ABA_OPENSUB :  public ABA_ABACUSROOT  {
  friend class ABA_SUB;
  friend class ABA_MASTER;
    public:

/*! \brief The constructor
 *   does not initialize the member \a dualBound_ since
 *   this can only be done if we know the sense of the objective function
 *   which is normally unknown when the constructor of the class ABA_MASTER 
 *   is called which again calls this constructor.
 *
 *   \param master A pointer to the corresponding master of the optimization.
 */
      ABA_OPENSUB(ABA_MASTER *master);

/*! \return The current number of open subproblems contained in this set.
 */
      int number() const;

/*! \return true If there is no subproblem in the set of open subproblems,
 *   \return false otherwise.
*/
      bool empty() const;

/*! \return The value of the dual bound of all subproblems in the list.
*/
      double  dualBound() const;

    private:

/*! \brief Selects a subproblem according to the
 *   strategy in \a master and removes it from the list
 *   of open subproblems. 
 *
 *   The function \a select() scans the list of open subproblems, and selects
 *   the subproblem with highest priority from the set of open subproblems.
 *   Dormant subproblems are ignored if possible.
 *
 *   \return The selected subproblem.
 *   If the set of open subproblems is empty, 0 is returned.
 */
      ABA_SUB *select();

/*! Adds a subproblem to the set of open subproblems.
 *
 *   \param sub The subproblem that is inserted.
 */
      void insert(ABA_SUB *sub);

/*! Removes subproblem from the set of open subproblems.
 *
 *   \param sub The subproblem that is removed.
 */
      void remove(ABA_SUB *sub);

/*! Removes all elements from the set of opens subproblems.
 */
      void prune();

/*! \brief Updates the member \a dualBound_ 
 *  according to the dual bounds of the subproblems contained in this set.
 */
      void updateDualBound();

      /*! A pointer to corresponding master of the optimization.
       */
      ABA_MASTER *master_;

      /*! The doubly linked list storing the open subproblems.
       */
      ABA_DLIST<ABA_SUB*> list_;

      /*! The  number of open subproblems.
       */
      int           n_;

      /*! The dual bound of all open subproblems.
       */
      double        dualBound_;
      ABA_OPENSUB(const ABA_OPENSUB &rhs);
      const ABA_OPENSUB &operator=(const ABA_OPENSUB &rhs);
  };


inline int ABA_OPENSUB::number() const
  {
    return n_;
  }

inline bool ABA_OPENSUB::empty() const
  {
    return list_.empty();
  }


#endif  // ABA_OPENSUB_H

/*! \class ABA_OPENSUB
 *  \brief New subproblems are inserted
 *  in this set after a branching step, or when a subproblem
 *  becomes dormant. A subproblem is extracted from this
 *  list if it becomes the active subproblem which is optimized.
 */

