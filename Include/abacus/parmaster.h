/*!\file
 * \author Matthias Elf
 * \brief the parallel master.
 *
 * The class ABA_PARMASTER contains all data and operations specific to
 * the parallel ABACUS.
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


#ifndef ABA_PARMASTER_H
#define ABA_PARMASTER_H

#include "abacus/abacusroot.h"
#include "abacus/array.h"
#include "abacus/sub.h"
#include "abacus/idmap.h"
#include "abacus/message.h"
#include "abacus/broadcast.h"
#include "abacus/dualbound.h"

class ABA_MASTER;
class ABA_NOTIFYSERVER;
class ABA_SUBSERVER;
class ABA_BALANCER;
template <class Type> class ABA_IDMAP;

  class  ABA_PARMASTER :  public ABA_ABACUSROOT  {
    public:

/*! The constructor.
 *   
 *   \param master A pointer to the master object.
 */
      ABA_PARMASTER(ABA_MASTER *master);

//! The destructor.
      ~ABA_PARMASTER();

/*! Initializes the parameters specific to the parallel version of ABACUS.
 */
      void initializeParameters();

/*! \brief Sets default values of the
 *   parameters specific to the parallel version of ABACUS.
 */
      void setDefaultParameters();

/*! \brief Prints the settings of the 
 *   parameters specific to the parallel version of ABACUS.
 */
      void printParameters();

/*! Prints statistics specific to the parallel version of ABACUS.
 */
      void outputStatistics();
      int hostId() const;
      int hostCount() const;
      bool isHostZero() const;
      const ABA_STRING& hostname(int i) const;
      const ABA_STRING& hostname() const;

/*! Registers an ABA_POOL with the master.
 *
 *   \param pool A pointer to the ABA_POOL.
 *
 *   \return An integer identifying the pool.
 */
      int registerPool(void *pool);

/*! Unregisters a previously registered ABA_POOL.
 *
 *   \param index The index of the ABA_POOL which was returned by the function
 *                    \a registerPool() when the ABA_POOL was registered.
 */
      void unregisterPool(int index);

/*! \return A Pointer to the registered ABA_POOL.
 *
 *   \param index The index of the registered ABA_POOL.
 */
      void *getPool(int index);
      ABA_IDMAP<ABA_SUB> *subIdentificationMap() const;

/*! \param port The port number of the service to connect to.
 *   \param msg The ABA_MESSAGE where the connection will be
 *                     established into.
 */
      void connectService(int port, ABA_MESSAGE& msg, int destId);
      ACE_SOCK_Stream& notifyStream(int i);

/*! \brief Updates the dual bound of this host,
 *   i.e.~the minimum (maximum) dual bound of all subproblems of this host.
 *
 *   The new bound is broadcast to all other hosts if it has changed.
 *   The global dual bound stored in the master is also updated, if needed.
 */
      void newHostDualBound(double x);

/*! \brief This version of the function \a newHostDualBound()
 *   receives a new dual
 *   bound of another host from a message and updates the corresponding
 *   element in the array \a hostDualBounds_.
 *
 *   The global dual bound stored in the master is also updated, if needed.
 */
      void newHostDualBound(ABA_MESSAGE& msg);

/*! Broadcasts a new primal bound to all hosts.
 */
      void newPrimalBound(double x);

/*! \brief This version of the function \a newPrimalBound()
 *   receives a new primal bound.
 */
      void newPrimalBound(ABA_MESSAGE& msg);

/*! \brief Broadcasts the new number of open
 *   subproblems and the best bound of all subproblems in the openSub list
 *   of this host to all other hosts if it has changed.
 */
      void newOpenSubCount(int n, double best);

/*! \brief Receives the new number
 *   of open subproblems and the best bound of the openSub list
 *   broadcast by another host.
 */
      void newOpenSubCount(ABA_MESSAGE& msg);

/*! \brief Is called before a new subproblem is selected 
 *   for optimization to perform a loadbalancing step. 
 *
 *   \a bestFirstTolerance_ defines the allowed deviation of the dual bound
 *   of the best subproblem at this host compared to the global best dual
 *   bound.
 *   If the best locally available subproblem is not within the tolerance,
 *   the best subproblem available at some other host is requested for.
 *   The location of the best subproblem is determined by inspecting
 *   \a hostDualBounds_.
 *
 *   \return true if successful
 *   \return false otherwise.
 */
      bool balance();

/*! \brief Tests, if all hosts are idle and no subproblems are staying arround anywhere. 
 *
 *   This is done by a two phase
 *   protocol initiated by host 0. In the first phase host 0 broadcasts a state
 *   request to all other hosts. Each host sets its local flag \a terminationOk_
 *   if it is idle and want's to terminate. This local flag is cleared
 *   immediately if work (e.g. a subproblem) is received since that time.
 *   Then each host sends a reply with dummy data.
 *   After all hosts replies are received the second phase is started.
 *   Host 0 broadcasts a state request again. This time, each host replies the
 *   value of its local flag \a terminationOk_. The flag is true if no work
 *   (i.e. subproblem) was received since the first phase. If all hosts reply
 *   true the system is out of work and termination can be initiated.
 */
      void terminationCheck();

/*! \brief This version of the function \a terminationCheck()
 *   handles the termination test phases I and II.
 */
      void terminationCheck(ABA_MESSAGE& msg);

/*! Sets the local flag \a terminationOk_ if this host is idle.
 */
      void startTerminationCheck();

/*! Has to be called if the host got new work, e.g.~if a new subproblem is created. 
 *
 *   The local variable \a workCount_ is
 *   incremented and the flag \a terminationOk_ is cleared.
 */
      void incWorkCount();

/*! Has to be called if the host has finished some work, e.g.~if a new subproblem is created. 
 *
 *   The local variable
 *   \a workCount_ is decremented. If \a workCount_ is zero the processor is idle.
 */
      void decWorkCount();

/*! Terminates all threads.
 */
      void terminate();

/*! Requests a new ID for the subproblem.
 *
 *   If \a master_->vbcLog() is true a globally unique ID is requested from
 *   host 0 to get a continuous sequence of IDs.
 *   Otherwise the ID is generated by adding \a 1000000*hostId_ a
 *   local counter.
 */
      int newId(int fatherId);
      void incSubSentCount();
      void incSubReceivedCount();
      void startIdleTime();
      void stopIdleTime();
      void updateIdleTimers(bool first);

/*! \brief Prints an ID. If the IDs are not
 *   requested continuous (vbcLog!=NoVbc) a pair consisting of the
 *   nmae of the host which has generated the subproblem and the
 *   local number of the subproblem is output.
 */
      void printId(int id);

    private:
      ABA_MASTER            *master_;
      int                    hostId_;
      int                    hostCount_;
      ABA_STRING             myHostname_;
      ABA_ARRAY<ABA_STRING*> hostname_;
      int                    connectTimeout_;
      ABA_ARRAY<void*>       registeredPools_;
      int                    lastRegisteredPool_;
      ABA_DUALBOUND          hostDualBounds_;
      ACE_Thread_Mutex       newHostDualBoundMutex_;
      ACE_Thread_Mutex       newPrimalBoundMutex_;
      int                    notifyPort_;
      ACE_SOCK_Stream       *notifyStreams_;
      ABA_NOTIFYSERVER      *notify_;
      int                    subserverPort_;
      ABA_SUBSERVER         *subserver_;
      int                    balancerPort_;
      double                 bestFirstTolerance_;
      ABA_BALANCER          *balancer_;
      ABA_ARRAY<int>         openSubCount_;
      ABA_DUALBOUND          openSubBest_;
      ACE_Thread_Mutex       newOpenSubCountMutex_;
      ACE_Thread_Mutex       terminationCheckMutex_;
      volatile bool          terminationCheckAgain_;
      ACE_Thread_Mutex       terminationMutex_;
      volatile bool          terminationOk_;
      volatile bool          hasTerminated_;
      int                    workCount_;
      ACE_Thread_Mutex       idCounterMutex_;
      int                    idCounter_;
      int                    subSentCount_;
      int                    subReceivedCount_;
      ABA_COWTIMER           idleCowTimeFirst_;
      ABA_COWTIMER           idleCowTimeMiddle_;
      ABA_COWTIMER           idleCowTimeLast_;
  };

inline int ABA_PARMASTER::hostId() const {
  return hostId_;
}

inline int ABA_PARMASTER::hostCount() const {
  return hostCount_;
}

inline bool ABA_PARMASTER::isHostZero() const {
  return hostId_==0;
}

inline const ABA_STRING& ABA_PARMASTER::hostname(int i) const {
  return *(hostname_[i]);
}

inline const ABA_STRING& ABA_PARMASTER::hostname() const {
  return *(hostname_[hostId_]);
}

inline ACE_SOCK_Stream& ABA_PARMASTER::notifyStream(int i) {
  return notifyStreams_[i];
}

inline void ABA_PARMASTER::incSubSentCount() {
  subSentCount_++;
}

inline void ABA_PARMASTER::incSubReceivedCount() {
  subReceivedCount_++;
}
  
inline void ABA_PARMASTER::startIdleTime() {
  idleCowTimeLast_.start();
}

inline void ABA_PARMASTER::stopIdleTime() {
  idleCowTimeLast_.stop();
}

inline void ABA_PARMASTER::updateIdleTimers(bool first) {
  if (first)
    idleCowTimeFirst_.addCentiSeconds(idleCowTimeLast_.centiSeconds());
  else
    idleCowTimeMiddle_.addCentiSeconds(idleCowTimeLast_.centiSeconds());
  idleCowTimeLast_.reset();
}

#endif  // ABA_PARMASTER_H

/*! \class ABA_PARMASTER
 *  \brief Class ABA_PARMASTER contains all data and operations specific to
 * the parallel ABACUS
 */

