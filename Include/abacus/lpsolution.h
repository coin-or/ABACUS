/*!\file
 * \author Matthias Elf
 * \brief lpsolution.
 *
 * This template class implements an LP solution. This class is
 * necessary when using the class ABA_SEPARATOR for separation.
 *
 * \par 
 * If constraints are to be generated in the separation then the \a BaseType
 * must be ABA_VARIABLE and the \a CoType must be ABA_CONSTRAINT.
 * In this case an objects of that class stores the primal variables of 
 * a linear program.
 * Otherwise, if variables are to be generated, then \a BaseType
 * must be ABA_CONSTRAINT and the \a CoType must be ABA_VARIABLE.
 * In this case an objects of that class stores the dual variables of 
 * a linear program.
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

#ifndef ABA_LPSOLUTION_H
#define ABA_LPSOLUTION_H
#include "abacus/array.h"

class ABA_MASTER;
class ABA_SUB;
template<class BaseType, class CoType> class ABA_ACTIVE;
template<class CoType,class  BaseType> class ABA_SEPARATOR;

#ifdef ABACUS_NEW_TEMPLATE_SYNTAX
template <class BaseType, class CoType>
class  ABA_LPSOLUTION;

template <class BaseType, class CoType>
ostream &operator<< (ostream &out, const ABA_LPSOLUTION<BaseType, CoType> &rhs);
#endif

  template <class BaseType, class CoType>
  class  ABA_LPSOLUTION :  public ABA_ABACUSROOT  {
    friend class ABA_SEPARATOR< CoType, BaseType>;
    public:

/*! The constructor.
 *
 *   \param sub A pointer to the subproblem in which the LP
 * solution is generated.
 *   \param primalVariables True if ABA_LPSOLUTION 
 *       contains the primal variables.
 *	In this case \a BaseType must be
 *	ABA_VARIABLE.
 *	If \a primaVariables is false, then \a BaseType must be ABA_CONSTRAINT.
 *   \param active 
 *       The active variables/constraints that are associated with the
 *  	LP solution.
 *	The default argument is 0.
 *	Then the set of active
 *	variables/constraints is not stored, but is assumed to be
 *	fixed and known.
 */
      ABA_LPSOLUTION(ABA_SUB *sub, bool primalVariables, ABA_ACTIVE<BaseType, CoType> *active);

/*! The constructor.
 *
 *   \param master A pointer to ABA_MASTER.
 */
      ABA_LPSOLUTION(ABA_MASTER *master);

/*! The copy constructor.
 *
 * \param rhs The LP solution that is copied.
 */
      ABA_LPSOLUTION(const ABA_LPSOLUTION<BaseType, CoType> &rhs);

//! The destructor.
      ~ABA_LPSOLUTION();
#ifdef ABACUS_NEW_TEMPLATE_SYNTAX

/*! The output operator writes the lpsolution to an output stream. 
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param rhs The lpsolution being output.
 */
      friend ostream &operator<< <> (ostream &, 
                                 const ABA_LPSOLUTION<BaseType, CoType> &);
#else

/*! The output operator writes the lpsolution to an output stream. 
 *
 * \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param rhs The lpsolution being output.
 */
      friend ostream &operator<<(ostream &out, 
                                 const ABA_LPSOLUTION<BaseType, CoType> &rhs);
#endif

/*! \return  The number of variables (if \a BaseType is ABA_VARIABLE) or
 *  the number of constraints (if \a BaseType is ABA_CONSTRAINT), resp.
 */
      int nVarCon() const;

/*! \return The primal/dual variables of the LP solution.
 */
      double *zVal();

/*! \return The active variables/constraints.
 */
      ABA_ACTIVE<BaseType, CoType> *active();

/*! \return The Id of the subproblem in which the LP solution was generated.
 */
      int idSub() const;

/*! \return The Id of the LP in which the LP solution was generated.
 */
      int idLp() const;
      #ifdef ABACUS_PARALLEL

/*! \return The processor which on which the LP solution was generated.
 */
      int idProc() const;
      #endif


    protected: 
      ABA_MASTER *master_;
      int nVarCon_;
      int idSub_;	
      int idLp_;	
      #ifdef ABACUS_PARALLEL
      int idProc_;	
      #endif
      ABA_ARRAY<double>  zVal_;
      ABA_ACTIVE<BaseType, CoType> *active_;
    private:
      const ABA_LPSOLUTION<BaseType, CoType> 
                              &operator=(const ABA_LPSOLUTION<BaseType, CoType> & rhs);
  };

/*! \class ABA_LPSOLUTION
 *
 * \param
 *   ABA_MASTER *master_
 *   A pointer to the corresponding master of the optimization.
 *
 * \param
 *   int nVarCon_
 *   The number of variables/constraints.
 *
 * \param
 *   int idSub_
 *   The Id of the subproblem in which the LP solution was generated.
 *
 * \param
 *   int idLp_
 *   The Id of the LP in which the LP solution was generated.
 *
 * \param
 *   ABA_ARRAY<double> * zVal_
 *   The primal/dual variables of the LP solution.
 *
 * \param
 *   ABA_ACTIVE<BaseType, CoType> *active_
 *   The active variables/constraints.
 */

#include "abacus/lpsolution.inc"

#endif  // ABA_LPSOLUTION_H


/*! \class ABA_LPSOLUTION
 * \brief template class implements an LP solution. This class is
 * necessary when using the class ABA_SEPARATOR for separation.
 */

