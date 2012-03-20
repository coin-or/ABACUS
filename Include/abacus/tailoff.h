/*!\file
 * \author Matthias Elf
 * \brief tailing off manager.
 * 
 * During the cutting plane phase of the optimization of a single
 * subproblem it can be quite often observed that during the first
 * iterations a significant decrease of the optimum value of the
 * LP occurs, yet, this decrease becomes smaller and
 * smaller in later iterations. This effect is called
 * {\it tailing off\/} (\cite{PR91}).
 * Experimental results show that it might
 * be better to branch, although violated constraints can still
 * be generated, than to continue the cutting plane phase.
 * This class stores the history of the values of the last
 * LP-solutions and implements all functions to control this
 * tailing-off effect.
 * The parameters are taken from the associated master.
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
 *
 * $Id: tailoff.h,v 2.4 2008-10-27 11:02:25 baumann Exp $
 */

#ifndef ABA_TAILOFF_H
#define ABA_TAILOFF_H

#include "abacus/ring.h"

class ABA_MASTER;
class ABA_SUB;
  class  ABA_TAILOFF :  public ABA_ABACUSROOT  { 
    friend class ABA_SUB;
    public: 

/*! \brief The constructor
 *  takes the length of the tailing off history from ABA_MASTER::tailOffNLp().
 *
 *   \param master A pointer to the corresponding master of the optimization.
 */
      ABA_TAILOFF(ABA_MASTER *master);

/*! \brief An alternative constructor
 *  takes the length of the tailing off history from the parameter NLp.
 *
 *   \param master A pointer to the corresponding master of the optimization.
 *   \param NLp The length of the tailing off history.
 */
      ABA_TAILOFF(ABA_MASTER *master, int NLp);

//! The destructor.
      ~ABA_TAILOFF();

/*! \brief The output operator
 *   writes the memorized LP-values on an output stream.
 *
 *   \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param rhs The tailing-off manager  being output.
 */
      friend ostream &operator<<(ostream &out, const ABA_TAILOFF &rhs);


/*! Checks if there is a tailing-off effect.
 *
 *   We assume a tailing-off effect if during the last ABA_MASTER::tailOffNLps()
 *   iterations of the cutting plane algorithms
 *   the dual bound changed at most ABA_MASTER::tailOffPercent() percent.
 *
 *   \return true If a tailing off effect is observed,
 *   \return false otherwise.
 */
      virtual bool tailOff() const;

/*! \brief Can be used to retrieve the difference between
 *   the last and a previous LP-solution in percent.
 *
 *   \return 0 If the difference could be computed, i.e., the old 
 *                LP-value \a nLPs before the last one is store in the
 *                history,
 *   \return 1 otherwise.
 *
 *   \param nLps The number of LPs before the last solved linear program
 *                   with which the last solved LP-value should be compared.
 *   \param d Contains the absolute difference bewteen the value of the 
 *                last solved
 *                linear program and the value of the linear program solved
 *                \a nLPs before in percent relative to the older value.
 */
      int diff(int nLps, double &d) const;

    protected: 

/*! A new LP-solution value can be stored by calling the function \a update(). 
 *
 *   This update should be performed after every solution
 *   of an LP in the cutting plane generation phase of the subproblem
 *   optimization process.
 *
 *   \param value The LP-solution value.
 */
      void update(double value);

/*! Clears the solution history.
 *
 *   This function
 *   should be called if variables are added, because
 *   normally the solution value of the LP-relaxation gets worse
 *   after the addition of variables. Such a
 *   change could falsely indicate a tailing-off effect if the
 *   history of LP-values is not reset.
 */
      void reset();

      /*! A pointer to the corresponding master of the optimization.
       */
      ABA_MASTER   *master_;

      /*! The LP-values considered in the tailing off analysis.
       */
      ABA_RING<double> *lpHistory_;
  };
#endif  // ABA_TAILOFF_H

 
 
/*! \class ABA_TAILOFF
 *  \brief This class stores the history of the values of the last 
  * LP-solutions and implements all functions to control 
  * tailing-off effect.
  */

