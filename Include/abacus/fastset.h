/*!\file
 * \author Matthias Elf
 * \brief fastset.
 *
 * This class is derived from the class ABA_SET and holds
 * for each set a rank which approximates the logarithm of the tree size
 * representing the set and is also an upper bound for the height of this
 * tree. In a union operation the tree with smaller rank is attached to the
 * tree with larger rank.
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


#ifndef ABA_FASTSET_H
#define ABA_FASTSET_H

#include "abacus/set.h"

  class  ABA_FASTSET :  public ABA_SET  {
    public:

/*! The constructor.
 *
 *   At the beginning each possible set receives the rank 0.
 *
 *       \param glob A pointer to the corresponding global object.
 *       \param size Only integers between 0 and \a size-1 can be inserted
 *                      in the set.
 */
      ABA_FASTSET (ABA_GLOBAL *glob, int size);

/*! Unites the sets \a x and \a y.
 *
 *   It differs from the function \a unionSets() of the base class ABA_SET
 *   such that the tree with smaller rank is attached to the one with larger
 *   rank. Therefore, \a x is no more guaranteed to be the representative of the
 *   joint set.
 *
 *   \return true If both sets have been disjoint before the function call,
 *   \return false otherwise.
 *
 *   \param x An element of the first set of the union operation.
 *   \param y An element in the second set of the union operation.
 */ 
      bool unionSets(int x, int y);
     
    private:
      ABA_ARRAY<int> rank_;
  };
#endif // ABA_FASTSET_H


/*! \class ABA_FASTSET
 * \brief class is derived from the class ABA_SET and holds
 * for each set a rank which approximates the logarithm of the tree size
 * representing the set and is also an upper bound for the height of this
 * tree.
 *
 *  \param rank_ The rank of each set.
 */

