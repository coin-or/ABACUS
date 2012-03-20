/*!\file
 * \author Matthias Elf
 * \brief separator.
 *
 * This abstract template class can be used to implement a separation
 * routine. 
 * Using this class 
 * is not mandatory, because separation can be implemented directly
 * in ABA_SUB::pricing() and ABA_SUB::separate(). However,
 * this class facilitates encapsulation of the code and provides
 * functions for checking for duplication of generated constraints/variables.
 *
 * \par 
 * If constraints are generated in the separation, then the \a BaseType
 * must be ABA_CONSTRAINT and the \a CoType must be ABA_VARIABLE,
 * if variables are generated in separation this is vice versa.
 *
 * \par
 * The user has to derive its specific separtor class in which the
 * separation algorithm should be implemented in function \a separate(). 
 * If a cutting plane is found,
 * the function \a cutFound should be called.
 *
 * \par
 * The generated constraints/variables can be obtained by the member
 * function \a cutBuffer(). The return value of that function then can
 * serve as parameter to the functions ABA_SUB::addCons() and ABA_SUB::addVars().
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
 
#ifndef ABA_SEPARATOR_H
#define ABA_SEPARATOR_H

#include "abacus/hash.h"
#include "abacus/buffer.h" 
#include "abacus/lpsolution.h"
#include "abacus/nonduplpool.h"

class ABA_ABACUSROOT;


  enum ABA_SEPARATOR_CUTFOUND {Added, Duplication, Full};
  template <class BaseType, class CoType>
  class  ABA_SEPARATOR :  public ABA_ABACUSROOT  {
    public:

/*! The constructor.
 *
 * \param master A pointer to the corresponding master of the optimization.
 * \param lpSolution The LP solution to be separated.
 * \param maxGen The maximal number of cutting planes which are stored.
 * \param nonDuplications If this flag is set, then the same
 * constraint/variable is stored at most  once in the buffer. In this case
 * for constraints/variables the virtual member functions
 * \a name(), \a hashKey(), and \a equal() of the base class ABA_CONVAR have to be
 * defined. Using these three functions, we check in the function \a cutFound
 * if a constraint or variable is already stored in the buffer.
 * \param sendConstraint In the parallel version this parameter determines 
 *	      if the constraints should be sent to their corresponding stores.
 */
      ABA_SEPARATOR(ABA_LPSOLUTION<CoType,BaseType> *lpSolution, 
		    bool nonDuplications, int maxGen=300 
		    );

//! The destructor.
      virtual ~ABA_SEPARATOR();

/*! \brief This function
 * has to be redefined and should implement the separation routine.
 */
      virtual void separate()=0;

/*! \brief The function \a cutFound(BaseType *cv) 
 * passes a cut (constraint or variable) to the buffer.
 *
 * If the buffer is full or the  cut already exists, the cut is deleted.
 *
 * \return ABA\_SEPARATOR\_CUTFOUND::Added, if the cut is added to the buffer;
 * \return ABA\_SEPARATOR\_CUTFOUND::Duplication, if the cut is already in the buffer;
 * \return ABA\_SEPARATOR\_CUTFOUND::Full, if the buffer is full.
 *
 * \param cv A pointer to a new constraint/variable found by the
 *  separation algorithm.
 */
      ABA_SEPARATOR_CUTFOUND cutFound(BaseType *);

/*! \return The function returns true if the separation should be terminated.
 * In the default implementation, this is the case if \a maxGen constraints/variables 
 * are in the cutBuffer.
 */
      virtual bool terminateSeparation() 
       {return(nGen()>=maxGen());};

/*! \return The buffer with the generated constraints/variable.
 */
      ABA_BUFFER<BaseType *> &cutBuffer();

/*! \return The number of generated cutting planes.
 */
      int nGen() const;

/*! \return The number of duplicated constraints/variables which are discarded.
 */
      int nDuplications() const;

/*! \return The number of collisions in the hash table.
 */
      int nCollisions() const;

/*! \return The maximal number of generated cutting planes.
 */
      int maxGen() const;

/*! \return The absolute value for considering a constraint/variable as violated.
 */
      double minAbsViolation() const;

//! Set a new value for \a minAbsViolation.
      void minAbsViolation(double minAbsVio)
      {minAbsViolation_=minAbsVio;};

//! The lpSolution to be separated.
  ABA_LPSOLUTION<CoType, BaseType> *lpSolution() 
       {return lpSol_;};

/*! \brief If the separator
 * checks for duplication of cuts, the test is also done for
 * constraints/variables that are in the pool passed as argument.
 *
 * This can be useful if already cuts are generated
 * by performing constraint pool separation 
 * of this pool.
 */  
       void watchNonDuplPool(ABA_NONDUPLPOOL<BaseType, CoType> *pool)
      {pool_=pool;};
  protected: 

/*! \return The function checks if a constraint/variable that is equivalent
 * to \a cv according to the function ABA_CONVAR::equal() is already stored in
 * the buffer by using the hashtable.
 *
 * \param cv A pointer to a constraint/variable for which it should
 *               be checked if an equivalent item is already contained
 *               in the buffer.
 */
  bool find(BaseType *);
  ABA_MASTER *master_;
  ABA_LPSOLUTION<CoType, BaseType> *lpSol_;
  private: 
  double minAbsViolation_;
  ABA_BUFFER<BaseType*> newCons_;
  ABA_HASH<unsigned, BaseType*> *hash_;
  int nDuplications_;
  bool sendConstraints_;
  ABA_NONDUPLPOOL<BaseType, CoType> *pool_;   
  ABA_SEPARATOR(const ABA_SEPARATOR<BaseType, CoType> &rhs);
  const ABA_SEPARATOR<BaseType, CoType> 
                              &operator=(const ABA_SEPARATOR<BaseType, CoType> & rhs);
  };

#include "abacus/separator.inc"

/*! \class ABA_SEPARATOR
 *
 * \param 
 *   ABA_MASTER *master\_ A pointer to the corresponding master of the optimization.
 *
 * \param 
 *   ABA_LPSOLUTION<CoType, BaseType> *lpSol\_ The LP solution to be separated.
 */

#endif  // ABA_SEPARATOR_H
/*! \class ABA_SEPARATOR
 *  \brief  abstract template class can be used to implement a separation
 * routine
 */

