/*!\file
 * \author Matthias Elf
 * \brief identification of objects.
 *
 * An instance of the class ABA_ID is used for identification of
 * the same object stored multiple times on different processors. It
 * consists of a processor number, a sequence number, and optionally
 * an index of a pool if a constraint/variable is identified.
 * This class is used by the class ABA_IDMAP.
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

#ifndef ABA_ID_H
#define ABA_ID_H
#include <iostream>
using namespace std;

#include "abacus/abacusroot.h"

class ABA_MESSAGE;
  class  ABA_ID :  public ABA_ABACUSROOT  { 
    public: 

/*! The constructor.
 *
 *  \a sequence_ will be set to 0 indicating that the ABA_ID is not initialized.
*/
      ABA_ID();

/*! The message constructor.
 *
 *     \param msg The message from which the object is initialized.
*/
      ABA_ID(ABA_MESSAGE &msg);

/*! Packs the ABA_ID in an ABA_MESSAGE object.
 *
 *   \param msg The ABA_MESSAGE object in which the ABA_ID is packed.
*/
      void pack(ABA_MESSAGE &msg) const;

/*! The output operator.
 *
 *   \return A reference to the output stream.
 *
 *   \param out The output stream.
 *   \param id The ABA_ID being output.
*/
      friend ostream &operator<<(ostream &out, const ABA_ID &id);

/*! The comparison operator.
 *
 *   \return 0 If both ABA_ID objects are not equal,
 *   \return 1 otherwise.
 *
 *   \param lhs The left hand side of the comparison.
 *   \param rhs The right hand side of the comparison.
*/
      friend int operator==(const ABA_ID &lhs, const ABA_ID &rhs);

/*! Sets the processor number and the sequence number.
 *
 *   \param sequence The sequence number of the ABA_ID.
 *   \param proc The processor number of the ABA_ID.
 *   \param index If a constraint/variable is identified, then this
 *                   optional parameter determines the index of the pool
 *                   in which the constraint/variable is stored.
*/
      void initialize(unsigned long sequence, int proc, int index);

/*! Sets the ABA_ID to the uninitialized state.
 */
      void uninitialize();

/*! Tests if the ABA_ID was initialized.
 *
 *   \return false If the ABA_ID is not initialized.
 *   \return true If the ABA_ID is initialized.
*/
      bool isInitialized() const;
      unsigned long sequence() const;
      int proc() const;
      int index() const;

    private:
      unsigned long sequence_;
      short         proc_;
      short         index_;
  };
inline unsigned long ABA_ID::sequence() const { return sequence_; }
inline int ABA_ID::proc() const { return proc_; }
inline int ABA_ID::index() const { return index_; }
#endif  // ABA_ID_H


/*! \class ABA_ID
 *  \brief instance of the class ABA_ID is used for identification of
 * the same object stored multiple times on different processors.
 */

