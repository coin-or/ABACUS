/*!\file
 * \author Matthias Elf
 * \brief solution history
 *
 * This class implements the storage of the solution history. Each time
 * when a better feasible solution or globally valid dual bound 
 * is found, it should be memorized in this class.
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

#ifndef ABA_HISTORY_H
#define ABA_HISTORY_H
#include <iomanip>
using namespace std;

#include "abacus/array.h"

class ABA_MASTER;

  class  ABA_HISTORY :  public ABA_ABACUSROOT  {
    public: 

/*! The constructor initializes a history table with 100 possible entries.
 *
 *   If this number is exceeded an automatic reallocation is performed.
 *
 *   \param master A pointer to the corresponding master of the optimization.
*/
      ABA_HISTORY(ABA_MASTER *master);

//! The destructor.
      virtual ~ABA_HISTORY();

/*! The output operator.
 *
 *   \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param rhs The solution history being output.
*/      friend ostream& operator<<(ostream& out, const ABA_HISTORY &rhs);

/*! \brief Adds an additional line to the history table,
 *  primal bound, dual bound, and the time are taken from the
 *  corresponding master object. 
 *  The history table is automatically reallocated if necessary.
 *  
 *  Usually an explicit call to this function from an application class
 *  is not required since \a update() is automatically called if
 *  a new global primal or dual bound is found.
 */
      void update();

    private:

/*! Returns the length of the history table.
 */
      int size() const;

//! The function \a realloc() enlarges the history table by 100 components.
      void realloc();

      /*! A pointer to corresponding master of the optimization.
       */
      ABA_MASTER   *master_;

      /*! The array storing the value of the best primal solution.
       */
      ABA_ARRAY<double>  primalBound_;

      /*! The array storing the value of the best dual solution.
       */
      ABA_ARRAY<double>  dualBound_;

      /*! The CPU time in seconds, when the entry in the table was made.
       */
      ABA_ARRAY<long>    time_;

      /*! The number of entries in the history table.
       */
      int            n_;
  };


inline int ABA_HISTORY::size() const
  {
    return primalBound_.size();
  }


#endif  // ABA_HISTORY_H


/*! \class ABA_HISTORY
 *  \brief class implements the storage of the solution history. 
 */


