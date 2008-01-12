/*!\file
 * \author Matthias Elf
 * \brief sense of optimization.
 *
 * We can either minimize or maximize the objective function.
 * We encapsulate this information in a class since it is required
 * in various classes, e.g., in the master of the \bab\ algorithm and
 * in the linear program.
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

#ifndef ABA_OPTSENSE_H
#define ABA_OPTSENSE_H

#include <iostream>
using namespace std;

#include "abacus/abacusroot.h"

  class  ABA_OPTSENSE :  public ABA_ABACUSROOT  { 
    public: 

    /*! The enumeration defining the sense of optimization.
     *
     *  \param Min Minimization problem.
     *  \param Max Maximization problem.
     *  \param Unknown Unknown optimization sense, required to recognize
     *                 uninitialized object.
     */
      enum SENSE {Min, Max, Unknown};

/*! The constructor initializes the optimization sense.
 *
 *   \param s The sense of the optimization. The default value is \a Unknown.
 */
      ABA_OPTSENSE(SENSE s = Unknown);

/*! \brief The output operator
 *    writes the optimization sense on an output stream
 *    in the form {\tt maximize}, {\tt minimize}, or
 *   {\tt unknown}.
 *
 *   \return The output stream.
 *
 *   \param out The output stream.
 *   \param rhs The sense being output.
 */
      friend ostream &operator<<(ostream& out, const ABA_OPTSENSE &rhs);

/*! \brief This version of the function \a sense()
 *   sets the optimization sense.
 *
 *   \param s The new sense of the optimization.
 */
      void sense(SENSE s);
   
/*! \return The sense of the optimization.
 */
      SENSE sense() const;

/*! \return true If it is minimization problem,
 *   \return false otherwise.
 */
      bool min() const;

/*! \return true If it is maximization problem,
 *   \return false otherwise.
 */
      bool max() const;

/*! \return true If the optimization sense  is unknown,
 *   \return false otherwise.
 */
      bool unknown() const;
    private:

      /*! The optimization sense.
       */
      SENSE sense_;  
  };


inline ABA_OPTSENSE::ABA_OPTSENSE(SENSE s) 
  : 
    sense_(s)
   { }

inline ABA_OPTSENSE::SENSE ABA_OPTSENSE::sense() const
  {
    return sense_;
  }

inline void ABA_OPTSENSE::sense(SENSE s)
  {
    sense_ = s;
  }

inline bool ABA_OPTSENSE::min() const
  {
    return sense_ == Min ? true : false;
  }

inline bool ABA_OPTSENSE::max() const
  {
    return sense_ == Max ? true : false;
  }

inline bool ABA_OPTSENSE::unknown() const
  {
    return sense_ == Unknown ? true : false;
  }


#endif  // ABA_OPTSENSE_H


/*! \class ABA_OPTSENSE
 *  \brief We can either minimize or maximize the objective function.
 * We encapsulate this information in a class since it is required
 * in various classes
 */

