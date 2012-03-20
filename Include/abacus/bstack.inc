/*!\file
 * \author Matthias Elf
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

#ifndef ABA_BSTACK_INC
#define ABA_BSTACK_INC

template <class Type> 
ABA_BSTACK<Type>::ABA_BSTACK(ABA_GLOBAL *glob, int size)
:  
  glob_(glob),  
  stack_(glob, size),  
  tos_(0)
{ }

template <class Type>
ostream& operator<<(ostream &out, const ABA_BSTACK<Type> &rhs)
{
  for (int i = 0; i < rhs.tos(); i++) 
    out << i << " " << rhs.stack_[i] << endl;
  return out;
}

template <class Type>
inline int ABA_BSTACK<Type>::size() const
{
  return stack_.size();
}

template <class Type>
inline int ABA_BSTACK<Type>::tos() const
{
  return tos_;
}

template <class Type>
inline bool ABA_BSTACK<Type>::empty() const
{
  return tos_ ? false : true;
}


template <class Type>
inline bool ABA_BSTACK<Type>::full() const
{
  return tos_ == size() ? true : false;
}

template <class Type>
void ABA_BSTACK<Type>::push(Type item)
{
  //! check if the stack is full
#ifdef ABACUSSAFE
  if (full()) {
    char *_error="ABA_BSTACK::push: stack is full";
    THROW_PARAM(AlgorithmFailureException,afcBstack,_error);
  }
#endif

  stack_[tos_++] = item;
}

template <class Type>
Type ABA_BSTACK<Type>::top() const
{

  //! check if stack is empty
#ifdef ABACUSSAFE
  if(empty()) {
    char *_error="ABA_BSTACK::top/pop: cannot perform operation on empty stack.";
    THROW_PARAM(AlgorithmFailureException,afcBstack,_error);
  }
#endif

  return stack_[tos_-1];
}

template <class Type>
Type ABA_BSTACK<Type>::pop()
{
  //! check if stack is empty
#ifdef ABACUSSAFE
  if(empty()) {
    char *_error="ABA_BSTACK::top/pop: cannot perform operation on empty stack.";
    THROW_PARAM(AlgorithmFailureException,afcBstack,_error);
  }
#endif

  return stack_[--tos_];
 
}

template <class Type>
void ABA_BSTACK<Type>::realloc(int newSize)
{
  stack_.realloc(newSize);
  if (newSize < tos_) tos_ = newSize;
}

#endif   //!ABA_BSTACK_INC
