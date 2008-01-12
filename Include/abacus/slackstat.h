/*!\file
 * \author Matthias Elf
 * \brief status of slack variables
 *
 * As for the structural variables the simplex method also assigns
 * a unique status to each slack variable. A slack variable can
 * be a basic or a nonbasic variable. If it is a nonbasic
 * variable, then we distinguish if the slack variable has value
 * zero or nonzero.
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

#ifndef ABA_SLACKSTAT_H
#define ABA_SLACKSTAT_H
#include <iostream>
using namespace std;


#include "abacus/abacusroot.h"

class ABA_GLOBAL;

#ifdef ABACUS_PARALLEL
class ABA_MESSAGE;
#endif

  class  ABA_SLACKSTAT :  public ABA_ABACUSROOT  { 
    public: 

    /*! The different statuses of a slack variable:
     *
     * \param Basic The slack variable belongs to the basis.
     * \param NonBasicZero The slack variable does not belong to the basis
     *                     and has value zero.
     * \param NonBasicNonZero The slack variable does not belong to the
     *                        basis and has a nonzero value.
     * \param Unknown The status of the slack variable is not known
     *                since no linear program with the corresponding
     *		      constraint has been solved.
     */
      enum STATUS {Basic, NonBasicZero, NonBasicNonZero, Unknown};

/*! \brief This constructor 
 *   initializes the status as \a Unknown.
 *
 *   \param glob A pointer to the corresponding global object.
 */
      ABA_SLACKSTAT(const ABA_GLOBAL *glob);

/*!  A constructor with initialization.
 *
 *   \param glob A pointer to the corresponding global object.
 *   \param status The slack variable receives the status \a status.
 */
      ABA_SLACKSTAT(const ABA_GLOBAL *glob, STATUS status);
#ifdef ABACUS_PARALLEL

/*! The message constructor creates the status from a message.
 *
 *     \param glob A pointer to a global object.
 *     \param msg The message from which the object is initialized.
 */
      ABA_SLACKSTAT(const ABA_GLOBAL *glob, ABA_MESSAGE &msg);

/*! Packs the data of the status in a message.
 *
 *   \param msg The ABA_MESSAGE object in which the status is packed.
 */
      void pack(ABA_MESSAGE &msg) const;
#endif

/*! \brief The output operator
 *   writes the status to an output stream in the
 *   format {\tt Basic}, {\tt NonBasicZero}, {\tt Non\-Basic\-Non\-Zero}, or
 *   {\tt Unknown}.
 *
 *   \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param rhs The status being output.
 */
      friend ostream  &operator<<(ostream& out, const ABA_SLACKSTAT &rhs);

/*! \return The status of the slack variable.
 */
      STATUS status() const;

/*! \brief This version of the function \a status()
 *   sets the status of the slack  variable.
 *
 *   \param stat The new status of the slack variable.
 */
      void status(STATUS stat);

/*! \brief This version of the function \a status()
 *   sets the status to the one of \a *stat.
 *
 *   \param stat The status of the slack variable is set to \a *stat.
 */
      void status(const ABA_SLACKSTAT *stat);

    private: 

      /*! A pointer to the corresponding global object.
       */
      const ABA_GLOBAL *glob_;

      /*! The status of the slack variable.
       */
      STATUS      status_;
  };


inline ABA_SLACKSTAT::ABA_SLACKSTAT(const ABA_GLOBAL *glob) 
  : 
    glob_(glob), 
    status_(Unknown)
  { }

inline ABA_SLACKSTAT::ABA_SLACKSTAT(const ABA_GLOBAL *glob, STATUS status) 
  : 
    glob_(glob), 
    status_(status)
  { }

inline ABA_SLACKSTAT::STATUS ABA_SLACKSTAT::status() const
  {
    return status_;
  }

inline void ABA_SLACKSTAT::status(STATUS stat)
  {
    status_ = stat;
  }

inline void ABA_SLACKSTAT::status(const ABA_SLACKSTAT *stat)
  {
    status_ = stat->status_;
  }


#endif  // ABA_SLACKSTAT_H

/*! \class ABA_SLACKSTAT
 *  \brief As for the structural variables the simplex method also assigns
 * a unique status to each slack variable. A slack variable can
 * be a basic or a nonbasic variable
 */
