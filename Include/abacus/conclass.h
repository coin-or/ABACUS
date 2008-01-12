/*!\file
 * \author Matthias Elf
 * \brief constraint classification.
 *
 * For the generation of knapsack cuts in \mip, a constraint
 * requires certain attributes about the types of its variables
 * and the structure of the constraint. A rather rudimentary
 * classification is implemented in the class ABA_CONCLASS.
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

#ifndef ABA_CONCLASS_H
#define ABA_CONCLASS_H
#include <iostream>
using namespace std;

#include "abacus/abacusroot.h"

class ABA_MASTER;

#ifdef ABACUS_PARALLEL
class ABA_MESSAGE;
#endif

  class  ABA_CONCLASS :  public ABA_ABACUSROOT  { 
    public: 

/*! The constructor initializes the constraint classification with
 *
 * \param master A pointer to the corresponding master of the optimization.
 * \param allVar Binary \a true if all variables are binary
 * \param trivial \a true if it is a bound or a variable upper bound
 * \param bound \atrue if the constraint is a bound of the variable
 * \param varBound \atrue if the constraint is a variable upper bound
 */
      ABA_CONCLASS(const ABA_MASTER *master, bool discrete, bool allVarBinary,
                   bool trivial, bool bound, bool varBound);
#ifdef ABACUS_PARALLEL

/*! \brief The message constructor creates the ABA_CONCLASS object from an ABA_MESSAGE.
 *
 *  \param master A pointer to the corresponding master of the optimization.
 *  \param msg The message from which the object is initialized.
 */
      ABA_CONCLASS (const ABA_MASTER *master, ABA_MESSAGE &msg);

/*! Packs the data of the ABA_CONCLASS in an ABA_MESSAGE object.
 *
 *\param msg The ABA_MESSAGE object in which the ABA_CONCLASS is packed.
 */
      void pack(ABA_MESSAGE &msg) const;
#endif

      friend ostream &operator<<(ostream &out, const ABA_CONCLASS &rhs);

/*! \brief Returns \a true if all variables with
 *  nonzero coefficients of the constraint are binary.
 */
      bool allVarBinary() const;

/*! \brief Returns \a true if the constraint is a
 *  bound or a variable upper bound.
 */
      bool trivial() const;

    private: 

      /*! A pointer to the corresponding master of the optimization.
       */
      const ABA_MASTER  *master_;
      bool  discrete_;

      /*! \a true if all variables are binary.
       */
      bool  allVarBinary_;

      /*! \a true if it is a bound or a variable lower/upper bound.
       */
      bool  trivial_;

      /*! \a true if the constraint is a bound of the variable.
       */
      bool  bound_;

      /*! \a true if the constraint is a variable lower/upper bound.
       */
      bool  varBound_;
  };


inline bool ABA_CONCLASS::allVarBinary() const
  {
    return allVarBinary_;
  }

inline bool ABA_CONCLASS::trivial() const
  {
    return trivial_;
  }


#endif  // !ABA_CONCLASS_H

/*! \class ABA_CONCLASS
 *  \brief A rather rudimentary classification for the generation of knapsack cuts 
 * is implemented in the class ABA_CONCLASS.
 */

