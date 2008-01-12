/*!\file
 * \author Matthias Elf
 * \brief status of variables.
 *
 * After the solution of a linear program by the simplex method
 * each variable receives a status indicating if the variable
 * is contained in the basis of the optimal solution, or is
 * nonbasic and has a value equal to its lower or upper bound,
 * or is a free variable not contained in the basis. We extend
 * this notion since later in the interface from a cutting plane
 * algorithm to the linear program variables might be eliminated.
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

#ifndef ABA_LPVARSTAT_H
#define ABA_LPVARSTAT_H
#include <iostream>
using namespace std;

#include "abacus/abacusroot.h"

class ABA_GLOBAL;

#ifdef ABACUS_PARALLEL
class ABA_MESSAGE;
#endif

  class ABA_LPVARSTAT :  public ABA_ABACUSROOT  { 
    public: 

    /*! \brief The enumeration of the statuses a variable gets from the linear program
     *         solver:
     *
     * \param AtLowerBound The variable is at its lower bound, but not
     *                     in the basis.
     * \param Basic The variable is in the basis.
     * \param AtUpperBound The variable is at its upper bound , but not
     *                     in the basis.
     * \param NonBasicFree The variable is unbounded and not in the
     *                     basis.
     * \param Eliminated The variable has been removed by our
     *                   preprocessor in the class ABA_LPSUB. So, it
     *                   is not present in the LP-solver.
     * \param Unknown The LP-status of the variable is unknown since no
     *                LP has been solved. This status is also assigned
     *                to variables which are fixed or set, yet still
     *                contained in the \a LP to avoid a wrong setting
     *                or fixing  by reduced costs.
     */ 
      enum STATUS {AtLowerBound, Basic, AtUpperBound, NonBasicFree,
                   Eliminated, Unknown}; 


/*! This constructor initializes the status as \a Unknown.
 *
 *   \param glob A pointer to the corresponding global object.
 */
      ABA_LPVARSTAT(ABA_GLOBAL *glob);

/*! This constructor initializes the ABA_LPVARSTAT.
 *
 *   \param glob A pointer to the corresponding global object.
 *   \param status The initial status.
 */
      ABA_LPVARSTAT(ABA_GLOBAL *glob, STATUS status);
    
/*! This constructor make a copy of \a *lpVarStat.
 *
 *   \param lpVarStat A copy of this object is made.
 */

      ABA_LPVARSTAT(ABA_LPVARSTAT *lpVarStat);
#ifdef ABACUS_PARALLEL

/*! The message constructor creates the ABA_LPVARSTAT from a message.
 *
 *     \param glob A pointer to a global object.
 *     \param msg The message from which the object is initialized.
 */
      ABA_LPVARSTAT(const ABA_GLOBAL *glob, ABA_MESSAGE &msg);

/*! The function \a pack() packs the data of the ABA_LPVARSTAT in a message.
 *
 *   \param msg The ABA_MESSAGE object in which the status is packed.
 */
      void pack(ABA_MESSAGE &msg) const;
#endif

/*! \brief The output operator
 *   writes the \a STATUS to an output stream
 *   in the form {\tt AtLowerBound}, {\tt Basic}, {\tt AtUpper\-Bound},
 *   {\tt NonBasicFree}, {\tt Eliminated}, {\tt Unknown}.
 *
 *   \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param rhs The status being output.
 */
      friend ostream &operator<<(ostream& out, const ABA_LPVARSTAT &rhs);

/*! \return The LP-status.
 */
      STATUS status() const;

/*! \brief This version of \a status()
 *   sets the status.
 *
 *   \param stat The new LP-status.
 */
      void status(STATUS stat);

/*! \brief Another version of the function \a status()
 *   for setting the status.
 *
 *   \param stat The new LP-status.
 */
      void status(const ABA_LPVARSTAT *stat);

/*!  \return true If the variable status is \a AtUpperBound or \a AtLowerBound,
 *   \return false otherwise.
 */
      bool atBound() const;

/*!  \return true If the status is \a Basic,
 *   \return false otherwise.
 */
      bool basic() const;

    private: 

      /*! A pointer to the corresponding global object.
       */
      ABA_GLOBAL *glob_;

      /*! The LP-status.
       */
      STATUS status_;
  };


inline ABA_LPVARSTAT::ABA_LPVARSTAT(ABA_GLOBAL *glob) 
  : 
    glob_(glob), 
    status_(Unknown)
  { }

inline ABA_LPVARSTAT::ABA_LPVARSTAT(ABA_GLOBAL *glob, STATUS status) 
  : 
    glob_(glob), 
    status_(status)
  { }

inline ABA_LPVARSTAT::ABA_LPVARSTAT(ABA_LPVARSTAT *lpVarStat) 
  : 
    glob_(lpVarStat->glob_), 
    status_(lpVarStat->status_)
  { }

inline ABA_LPVARSTAT::STATUS ABA_LPVARSTAT::status() const
  {
    return status_;
  }

inline void ABA_LPVARSTAT::status(STATUS stat)
  {
    status_ = stat;
  }

inline void ABA_LPVARSTAT::status(const ABA_LPVARSTAT *stat)
  {
    status_ = stat->status_;
  }

inline bool ABA_LPVARSTAT::atBound() const
  {
    if (status_ == AtLowerBound || status_ == AtUpperBound) return true;
    else                                                    return false;
  }

inline bool ABA_LPVARSTAT::basic() const
  {
    return status_ == Basic ? true : false;
  }


#endif  // ABA_LPVARSTAT_H

/*! \class ABA_LPVARSTAT
 * \brief After the solution of a linear program by the simplex method
 * each variable receives a status indicating if the variable
 * is contained in the basis of the optimal solution, or is
 * nonbasic and has a value equal to its lower or upper bound,
 * or is a free variable not contained in the basis.
 */

