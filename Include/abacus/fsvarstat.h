/*!\file
 * \author Matthias Elf
 * \brief status of fixed and set variables.
 *
 * If a variable is fixed to a value, then this means that it keeps
 * this value ``forever''. If it is set, then the variable keeps the
 * value in the subproblem where the setting is performed and in the
 * subproblems of the subtree rooted at this subproblem.
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

#ifndef ABA_FSVARSTAT_H
#define ABA_FSVARSTAT_H
#include <iostream>
using namespace std;


#include "abacus/abacusroot.h"

class ABA_GLOBAL;


  class  ABA_FSVARSTAT :  public ABA_ABACUSROOT  { 
    public: 

/*! \brief The enumeration defining the different statuses of variables
 *         from the point of view of fixing and setting:
 *
 *    \param Free The variable is neither fixed nor set.
 *    \param SetToLowerBound The variable is set to its lower bound.
 *    \param Set The variable is set to a value which can be accessed
 *                   with the member function |value()|.
 *    \param SetToUpperbound The variable is set to its upper bound.
 *    \param FixedToLowerBound The variable is fixed to its lower bound.
 *    \param Fixed The variable is fixed to a value which can be accessed
 *                     with the member function |value()|.
 *    \param FixedToUpperBound The variable is fixed to its upper bound.
 */
      enum STATUS {Free, SetToLowerBound, Set, SetToUpperBound,
                   FixedToLowerBound, Fixed, FixedToUpperBound};

/*! \brief This constructor
 *  initializes  the status as \a Free.
 *
 *  \param glob A pointer to a global object.
 */

      ABA_FSVARSTAT(ABA_GLOBAL *glob);

/*! \brief This constructor
 *  initializes the status explicitely.
 *
 *   \param glob A pointer to a global object.
 *   \param status The initial status that must neither be \a Fixed nor \a Set.
 *                     For these two statuses the next constructor has to be
 *                     used.
 */
      ABA_FSVARSTAT(ABA_GLOBAL *glob, STATUS status);

/*! \brief This constructor
 *  initializes the status explicitely to \a Fixed or \a Set.
 *
 *   \param glob A pointer to a global object.
 *   \param status The initial status that must be \a Fixed or \a Set.
 *   \param value The value associated with the status \a Fixed or \a Set.
 */
      ABA_FSVARSTAT(ABA_GLOBAL *glob, STATUS status, double value);

/*! This constructor makes a copy.
 *
 *   \param fsVarStat The status is initialized with a copy of \a *fsVarStat.
 */
      ABA_FSVARSTAT(ABA_FSVARSTAT *fsVarStat);

/*! \brief The output operator writes the status and, if the status is
 *  \a Fixed or \a Set, also its value on an output stream.
 *
 *   \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param rhs The variable status being output.
 */
      friend ostream &operator<<(ostream& out, const ABA_FSVARSTAT &rhs);

/*! \return The status of fixing or setting.
 */
      STATUS status() const;

/*! \brief This version of the function \a status() 
 *  assigns a new status. 
 * 
 *  For specifying also a value in case of the statuses \a Fixed or \a Set the
 *  next version of this function can be use.
 *
 *   \param stat The new status.
 */
      void status(STATUS stat);

/*! \brief This version of the function \a status()
 *  can assign a new status also for the statuses \a Fixed and \a Set.
 *
 *   \param stat The new status.
 *   \param val A value associated with the new status.
 */
      void status(STATUS stat, double val);

/*! \brief A version of \a status()
 *  for assigning the status of an other object of the class ABA_FSVARSTAT.
 *
 *   \param stat A pointer to the object that status and value is copied.
 */
      void status(const ABA_FSVARSTAT *stat);

/*! \return The value of fixing or setting if the variable has status \a Fixed or \a Set.
 */
      double value() const;

/*! \brief This version of \a value()
 *  assigns a new value of fixing or setting.
 *
 *   \param val The new value.
 */
      void value(double val);

/*!  \return true If the status is \a FixedToLowerBound, \a Fixed, or
 *                   \a FixedToUpperBound,
 *   \return false otherwise.
 */
      bool fixed() const;

/*!  \return true If the status is \a SetToLowerBound, \a Set, or
 *                   \a SetToUpperBound,
 *   \return false otherwise.
 */
      bool set() const;

/*! \return false If the status is \a Free,
 *   \return true otherwise.
 */
      bool fixedOrSet() const;

/*! \brief We say there is a contradiction between two status if they are
 *  fixed/set to different bounds or values. However, two statuses are not
 *  contradiction if one of them is ``fixed'' and the other one is
 *  ``set'', if this fixing/setting refers to the same bound or value.
 *
 *   \return true If there is a contradiction between the status of this
 *                 object and \a fsVarStat,
 *   \return false otherwise.
 *
 *   \param fsVarStat A pointer to the status with which contradiction is
 *                        is tested.
 */
      bool contradiction(ABA_FSVARSTAT *fsVarStat) const;

/*! Another version of the function \a contradiction().
 *
 *   \return true If there is a contradiction between the status of this
 *                     object and (\a status, \a value),
 *   \return false otherwise.
 *
 *   \param status The status with which contradiction is checked.
 *   \param value The value with which contradiction is checked.
 *                    The default value of \a value is 0.
 */
      bool contradiction(STATUS status, double value = 0) const;

    private: 

      /*! A pointer to the corresponding global object.
       */
      ABA_GLOBAL *glob_;

      /*! The status of the variable.
       */
      STATUS      status_;

      /*! The value the variable is fixed/set to. 
       *
       *  This member is only used for the statuses \a Fixed and \a Set.
       */
      double      value_;
  };


inline ABA_FSVARSTAT::ABA_FSVARSTAT(ABA_GLOBAL *glob) 
  : 
    glob_(glob), 
    status_(Free)
  { }

inline ABA_FSVARSTAT::STATUS ABA_FSVARSTAT::status() const
  {
    return status_;
  }

inline void ABA_FSVARSTAT::status(STATUS stat)
  {
    status_ = stat;
  }

inline void ABA_FSVARSTAT::status(STATUS stat, double val)
  {
    status_ = stat;
    value_  = val;
  }

inline void ABA_FSVARSTAT::status(const ABA_FSVARSTAT *stat)
  {
    status_ = stat->status_;
    value_  = stat->value_;
  }

inline double ABA_FSVARSTAT::value() const
  {
    return value_;
  }

inline void ABA_FSVARSTAT::value(double val)
  {
    value_ = val;
  }

inline bool ABA_FSVARSTAT::fixedOrSet() const
  {
    if (status_ == Free) return false;
    else                 return true;
  }


#endif  // ABA_FSVARSTAT_H


/*! \class ABA_FSVARSTAT
 *  \brief status of fixed and set variables.
 */


