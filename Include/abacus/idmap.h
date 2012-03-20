/*!\file
 * \author Matthias Elf
 *
 * \brief identification map.
 *
 * The class ABA_IDMAP<Type> implements a map between elements of ABA_ID
 * and pointers to objects of the class \a Type. Objects beeing inserted
 * are not copied, only a pointer to the object is stored in the map.
 * Insertion and deletion of map elements need constant time in the average,
 * since in the implementation a hashtable is used.
 *
 * \par
 * It is possible to create aliases of ABA_ID elements which are linked
 * in a ringlist. If an object is removed from the map all ABA_ID elements
 * referencing this object will be removed efficiently.
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


#ifndef ABA_IDMAP_H
#define ABA_IDMAP_H
#include <ace/Synch.h>

#include "abacus/abacusroot.h"
#include "abacus/id.h"

class ABA_MASTER;

template <class Type>
class  ABA_IDMAP;

template <class Type>
ostream &operator<< (ostream& out, const ABA_IDMAP<Type> &idmap);

  template <class Type> class  ABA_IDMAP :  public ABA_ABACUSROOT  {
    public:

/*! The constructor.
 *
 * \param master A pointer to the corresponding master object.
 * \param size The size of the hashtable used by the map.
 * \param pool The index of the pool, if the ABA_ID identifies a
 *                 constraint/variable.
 */
      ABA_IDMAP(ABA_MASTER *master, int size, int index);

//! The destructor.
      ~ABA_IDMAP();

/*! The output operator writes the ABA_IDMAP to the stream \a out.
 *
 *  \return A reference to the output stream.
 *
 * \param out The output stream.
 * \param idmap The ABA_IDMAP being output.
 */
      friend ostream& operator<< <>(ostream& , const ABA_IDMAP<Type> &);

/*! Looks up an object in the map by its ABA_ID.
 *
 * \return A pointer to the object with the given ABA_ID, or a 0-pointer if there
 * is no such object.
 *
 * \param id The ABA_ID of the object beeing looked up.
 */
      Type *find(const ABA_ID &id);

/*! \param id The identification of the object.
 * \param obj A pointer to the object beeing inserted.
 */
      void insert(const ABA_ID &id, const Type *obj);

/*! Inserts the object after having assigned a new ABA_ID to it.
 *
 * The new ABA_ID consists of the processor number
 * and a sequence number which is incremented each time this function is called.
 *
 * \param id The new identification is assigned to \a id.
 * \param obj A pointer to the object beeing inserted.
 */
      void insertWithNewId(ABA_ID &id, const Type *obj);
      void insertAlias(const ABA_ID &id, const ABA_ID &aliasId);

/*! \return 0 If the ABA_ID was successfully removed.
 * \return 1 If there is no such ABA_ID in the map.
 *
 * \param id The identification beeing removed.
 */
      int  remove(const ABA_ID &id);
      unsigned long sequence() const;
      int proc() const;
      int index() const;

    private:
      ABA_MASTER *master_;
      ABA_HASH<ABA_ID, const Type*> map_;
      unsigned long   sequence_;
      int             proc_;
      int             index_;
      ACE_Thread_Mutex mp_;

  };

#include "abacus/idmap.inc"

template<class Type>
inline unsigned long ABA_IDMAP<Type>::sequence() const { return sequence_; };
template<class Type>
inline int ABA_IDMAP<Type>::proc() const { return proc_; };
template<class Type>
inline int ABA_IDMAP<Type>::index() const { return index_; };

#endif  // !ABA_IDMAP_H

/*! \class ABA_IDMAP
 *  \brief class ABA_IDMAP<Type> implements a map between elements of ABA_ID
 * and pointers to objects of the class \a Type.
 */

