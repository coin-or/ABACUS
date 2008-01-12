/*!\file
 * \author Matthias Elf
 * \brief set.
 *
 * This class implements a data structure for collections of dynamic disjoint
 * sets of integers. Each set has a unique representative being one
 * member of the set. We provide the operations generation of a
 * set with one element, union of sets, and the determination
 * of the set some element is currently contained in.
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


#ifndef ABA_SET_H
#define ABA_SET_H

#include "abacus/array.h"
class  ABA_SET :  public ABA_ABACUSROOT  {
    public:

/*! The constructor.
 *
 *       \param glob A pointer to the corresponding global object.
 *       \param size Only integers between 0 and \a size-1 can be inserted
 *                      in the set.
 */
      ABA_SET(ABA_GLOBAL *glob, int size);

//! The destructor.
      virtual ~ABA_SET();

/*! Creates a set storing only one element and adds it to the collection of sets.
 *
 *   \param x The single element of the new set.
 */
      void makeSet(int x);

/*! Unites the two sets which contain \a x and \a y, respectively. 
 *   
 *   This operation may only be performed if
 *   both \a x and \a y have earlier been added to the collection of sets
 *   by the function \a makeSet(). 
 *
 * \par
 *   We do not use the heuristic  attaching the smaller subtree
 *   to the bigger one, since we want to guarantee that the representative
 *   of \a x is always the representative of the two united sets. 
 *
 *   \return true If both sets have been disjoint before the function call,
 *   \return false otherwise.
 *
 *   \param x An element of the first set of the union operation.
 *   \param y An element in the second set of the union operation.
 */
      bool unionSets(int x, int y);

/*! Finds the representative of the set containing \a x.
 *
 *   This operation may be only performed if
 *   \a x has been earlier added to the collection of sets
 *   by the function \a makeSet().
 *
 *  \par
 *   A path compression is performed, i.e., all nodes of the
 *   tree on the path from \a x to the root are directly attached to the
 *   root of the tree.
 *
 *   \return The representative of the set containing \a x.
 *
 *   \param x An element of the searched set.
 */
      int findSet(int x);

    protected:

      /*! A pointer to the corresponding global object.
       */
      ABA_GLOBAL *glob_;

      /*! \brief The collection of sets is implemented by a collection of trees.
       *  \a parent\_[i] is the parent of node \a i in the tree representing
       *  the set containing \a i. If \a i is the root of a tree then
       *  \a parent\_[i] is \a i itself.
       */
      ABA_ARRAY<int>  parent_;
    };

#endif // ABA_SET_H


/*! \class ABA_SET
 *  \brief class implements a data structure for collections of dynamic disjoint
 * sets of integers
 */

