/*!\file
 * \author Matthias Elf
 * \brief candidates for fixing.
 *
 * Variables can be only fixed according to the reduced costs and
 * statuses of variables of the root of the remaining \bab\ tree.
 * However, if we store these values, we can repeat the fixing process
 * also in any other node of the enumeration tree when we find
 * a better global lower bound.
 *
 * \par
 * Possible candidates for fixing are all variables which have
 * the status \a AtLowerBound or \a AtUpperBound. We store all
 * these candidates together with their values in this class.
 *
 * \par
 * If we try to fix variables according to reduced cost criteria
 * in nodes which are not the root of the remaining \bac\ tree,
 * we always have to take the candidates and values from this class.
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

#ifndef ABA_FIXCAND_H
#define ABA_FIXCAND_H

#include "abacus/array.h"

class ABA_MASTER;
class ABA_SUB;
class ABA_FSVARSTAT;
class ABA_VARIABLE;
class ABA_CONSTRAINT;
template<class BaseType, class CoType> class ABA_CUTBUFFER;
template<class BaseType, class CoType> class ABA_POOLSLOTREF;


  class  ABA_FIXCAND :  public ABA_ABACUSROOT  { 
  friend class ABA_SUB;
  friend class ABA_MASTER;
    public: 

/*! The constructor.
 *
 *   \param master A pointer to the corresponding master of the optimization.
 */
      ABA_FIXCAND(ABA_MASTER *master);

//! The destructor.
      ~ABA_FIXCAND();

    private: 

/*! Memorizes suitable variables for fixing.
 *
 *   \param sub A pointer to the root node of the remaining \bac\ tree.
 */
      void saveCandidates(ABA_SUB *sub);

/*! Tries to fix as many candidates as possible.
 *
 *   The new variable status is both stored in the global variable status
 *   of the class ABA_MASTER and in the local variable status of ABA_SUB.
 *   Candidates which are fixed are removed from the candidate set.
 *
 *   \return 1 If contradictions to the variables statuses of \a sub are
 *                detected.
 *   \return 0 otherwise.
 *
 *   \param addVarBuffer Inactive variables which are fixed to a nonzero
 *                           value are added to \a addVarBuffer to be activated
 *                           in the next iteration.
 *
 *   We do not used the function ABA_MASTER::primalViolated() for checking of a
 *   variable can be fixed, because here we also have to be careful for
 *   integer objective function.
 */
      void fixByRedCost(ABA_CUTBUFFER<ABA_VARIABLE, ABA_CONSTRAINT> *addVarBuffer);

/*! Deletes all allocated memory of members.
 *
 *  The member pointers are set to 0 that multiple deletion cannot
 *  cause any error.
 */
      void deleteAll();

  /*! Allocates memory to store \a nCand candidates for fixing.
   */
      void allocate(int nCand);

      /*! A pointer to the corresponding master of the optimization.
       */
      ABA_MASTER             *master_;

      /*! The candidates for fixing.
       */
      ABA_BUFFER<ABA_POOLSLOTREF<ABA_VARIABLE, ABA_CONSTRAINT>*> *candidates_;

      /*! The fixing status of the candidates.
       */
      ABA_BUFFER<ABA_FSVARSTAT*>   *fsVarStat_;

      /*! The left hand side of the expression evaluated for fixing.
       */
      ABA_BUFFER<double>           *lhs_;
      ABA_FIXCAND(const ABA_FIXCAND &rhs);
      const ABA_FIXCAND &operator=(const ABA_FIXCAND &rhs);
  };
#endif  // ABA_FIXCAND_H


/*! \class ABA_FIXCAND
 *  \brief candidates for fixing
 */
