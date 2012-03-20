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

#include "abacus/dualbound.h"
#include "abacus/global.h"

  ABA_DUALBOUND::ABA_DUALBOUND(ABA_GLOBAL *glob)
  :  
    glob_(glob),  
    bounds_(0)
  {
  }

  ABA_DUALBOUND::~ABA_DUALBOUND()
  {
    delete[] bounds_;
  }

  void ABA_DUALBOUND::initialize(int n, bool minIsBest)
  {
    minIsBest_ = minIsBest;
    set_.initialize(n);
    delete[] bounds_;
    bounds_ = new double[n];
    updateBestAndWorst();
  }

  void ABA_DUALBOUND::insert(int i, double d)
  {
    if (set_.exists(i) && (bounds_[i]==best_ || bounds_[i]==worst_)) {
      bounds_[i] = d;
      updateBestAndWorst();
    }
    else {
      set_.insert(i);
      bounds_[i] = d;
      if (minIsBest_) {
        if (d < best_) { best_ = d; bestIndex_ = i; }
        if (d > worst_) worst_ = d;
      }
      else {
        if (d > best_) { best_ = d; bestIndex_ = i; }
        if (d < worst_) worst_ = d;
      }
    }
  }

  void ABA_DUALBOUND::remove(int i)
  {
    if (set_.exists(i)) {
      set_.remove(i);
      if (bounds_[i]==best_ || bounds_[i]==worst_)
        updateBestAndWorst();
    }
  }

  bool ABA_DUALBOUND::better(int i, double d) const
  {
    if (set_.exists(i)) {
      // test, if the new bound is better than the old one
      double oldBound = bounds_[i];

      if (minIsBest_) {
        if (d <= oldBound)
          return false;
      }
      else {
        if (d >= oldBound)
          return false;
      }
    }
    return true;
  }

  double ABA\_DUALBOUND::best(int *index) const

  double ABA_DUALBOUND::best(double d) const
  {
    if (minIsBest_)
      return d < best_ ? d : best_;
    else
      return d > best_ ? d : best_;
  }

  double ABA\_DUALBOUND::worst() const

  void ABA_DUALBOUND::updateBestAndWorst()
  {
    bestIndex_ = -1;
    if (minIsBest_) {
      int count = set_.count();
      best_ = glob_->infinity();
      worst_ = -glob_->infinity();
      for (int i = 0; i < count; i++) {
        double d = bounds_[set_.elem(i)];
        if (d < best_) { best_ = d; bestIndex_ = set_.elem(i); }
        if (d > worst_) worst_ = d;
      }
    }
    else {
      int count = set_.count();
      best_ = -glob_->infinity();
      worst_ = glob_->infinity();
      for (int i = 0; i < count; i++) {
        double d = bounds_[set_.elem(i)];
        if (d > best_) { best_ = d; bestIndex_ = set_.elem(i); }
        if (d < worst_) worst_ = d;
      }
    }
  }

