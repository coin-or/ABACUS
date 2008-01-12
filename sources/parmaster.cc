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

#include "abacus/parmaster.h"
#include "abacus/master.h"
#include "abacus/pool.h"
#include "abacus/notifyserver.h"
#include "abacus/notification.h"
#include "abacus/subserver.h"
#include "abacus/balancer.h"
#include "abacus/opensub.h"

#include <ace/OS.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>

#include "abacus/debug.h"
int debugLevel_ = 0;		//!< currently global!
  
  ABA_PARMASTER::ABA_PARMASTER(ABA_MASTER *master) 
    :  
    master_(master),  
    hostId_(0),  
    hostCount_(0),  
    myHostname_(master),  
    hostname_(master, 1, 0),  
    connectTimeout_(10),  
    registeredPools_(master, 10, 0),  
    lastRegisteredPool_(-1),  
    hostDualBounds_(master),  
    notifyStreams_(0),  
    notify_(0),  
    openSubCount_(master, 1, 0),  
    openSubBest_(master),  
    terminationOk_(false),  
    hasTerminated_(false),  
    workCount_(0),  
    idCounter_(1),  
    subSentCount_(0),  
    subReceivedCount_(0),  
    idleCowTimeFirst_(master),  
    idleCowTimeMiddle_(master),  
    idleCowTimeLast_(master)
  {
    char help[100];
    if (ACE_OS::hostname(help, 100)==-1) {
      master->err() << "ABA_PARMASTER::ABA_PARMASTER(): can't determine my hostname." << endl;
      exit(Fatal);
    }
    myHostname_ = help;

    strcat(help,":");
    master_->out().setPrompt(help);
    master_->err().setPrompt(help);
  }

  ABA_PARMASTER::~ABA_PARMASTER()
  {
    if (debug(DEBUG_TERMINATION))
      master_->out() << "DEBUG_TERMINATION: terminating BALANCER..." << endl;
    ACE_Thread_Manager::instance()->cancel_task(balancer_);
    ACE_Thread_Manager::instance()->wait_task(balancer_);
    delete balancer_;

    if (debug(DEBUG_TERMINATION))
      master_->out() << "DEBUG_TERMINATION: terminating SUBSERVER..." << endl;
    ACE_Thread_Manager::instance()->cancel_task(subserver_);
    ACE_Thread_Manager::instance()->wait_task(subserver_);
    delete subserver_;

    if (debug(DEBUG_TERMINATION))
      master_->out() << "DEBUG_TERMINATION: terminating NOTIFICATIONSERVER..." << endl;

    ABA_BROADCAST bcast(master_, true);
    bcast.pack(ABA_NOTIFYSERVER::ShutdownTag);
    bcast.send();
    bcast.receiveReply();    //!< receive accumulated reply

    if (debug(DEBUG_TERMINATION))
      master_->out() << "DEBUG_TERMINATION: shutdown  broadcasted." << endl;

    ACE_Thread_Manager::instance()->wait_task(notify_);
    delete notify_;

    delete notifyStreams_;
  }

  void ABA_PARMASTER::initializeParameters()
  {
    master_->assignParameter(debugLevel_, "ParallelDebugLevel", 0, INT_MAX, 0);
    master_->assignParameter(connectTimeout_, "ParallelConnectTimeout", 1, INT_MAX, 10);
    master_->assignParameter(hostCount_, "ParallelHostCount", 1, INT_MAX);

    //! resize the arrays
    hostname_.realloc(hostCount_, 0);
    openSubCount_.realloc(hostCount_, 0);

    hostId_=-1;
    for (int i=0; i<hostCount_; i++) {
      char help[100];
      sprintf(help, "ParallelHostname_%d", i);
      hostname_[i] = new ABA_STRING(master_);
      master_->assignParameter(*(hostname_[i]), help);

      if (myHostname_ == hostname(i)) {
        if (hostId_>=0) {
          master_->err() << "ABA_PARMASTER::initializeParameters(): host "
                    << myHostname_ << " is included in .abacus more than once!"
                    << endl;
          exit(Fatal);
        }
        hostId_=i;
      }
    }

    if (hostId_==-1) {
      master_->err() << "ABA_PARMASTER::initializeParameters(): host " 
                    << myHostname_  << " is missing in .abacus!" << endl;
      exit(Fatal);
    }

    master_->assignParameter(notifyPort_, "ParallelNotifyPort", 1, INT_MAX);
    master_->assignParameter(subserverPort_, "ParallelSubproblemPort", 1, INT_MAX);
    master_->assignParameter(balancerPort_, "ParallelBalancerPort", 1, INT_MAX);
    master_->assignParameter(bestFirstTolerance_, "ParallelBestFirstTolerance", 0, 100, 0);

    hostDualBounds_.initialize(hostCount_, master_->optSense()->min());
    openSubBest_.initialize(hostCount_, master_->optSense()->min());


    static int once=0;
    if (once) return;
    once=1;
    //! run the following stuff only once

    //! create new notification server with \a hostCount_ threads
    notify_ = new ABA_NOTIFYSERVER(master_, notifyPort_, hostCount_);
    notify_->open(0);

    //! create notification streams and connect with server
    notifyStreams_ = new ACE_SOCK_Stream [hostCount_];
    for (int i=0; i<hostCount_; i++) {

      char *name = hostname(i).string();

      ACE_INET_Addr addr(notifyPort_, name);
      ACE_SOCK_Connector connector;

      while (connector.connect(notifyStreams_[i], addr) == -1) {
        if (debug(DEBUG_SOCKET))
          master_->out() << "DEBUG_SOCKET: server "
                         << name << ":" << notifyPort_
                         << " not available, retrying..." << endl;
        sleep(1);
      }
    }
    if (debug(DEBUG_SOCKET))
      master_->out() << "DEBUG_SOCKET: notification "
                     << "connections successfully established." << endl;

    //! create subproblem server
    subserver_ = new ABA_SUBSERVER(master_, subserverPort_, 1);
    subserver_->open(0);

    //! create balancing server
    balancer_ = new ABA_BALANCER(master_, balancerPort_, 1);
    balancer_->open(0);

  }

  void ABA_PARMASTER::setDefaultParameters()
  {
    master_->insertParameter("ParallelDebugLevel", "0");
  }

  void ABA_PARMASTER::printParameters()
  {
    master_->out() << endl << "Parallel Parameters:" << endl << endl;

    master_->out() << "  Debug Level                            : ";
    master_->out() << debugLevel_ << endl;
    master_->out() << "  Connect Timeout                        : ";
    master_->out() << connectTimeout_ << endl;
    master_->out() << "  Number of parallel hosts               : ";
    master_->out() << hostCount_ << endl;
    for (int i=0; i<hostCount_; i++) {
      master_->out() << "  Hostname " << setWidth(2) << i
                     << "                            : "
                     << hostname(i) << endl;
    }
    master_->out() << "  Notify Port                            : ";
    master_->out() << notifyPort_ << endl;
    master_->out() << "  Subproblem Server Port                 : ";
    master_->out() << subserverPort_ << endl;
    master_->out() << "  Balancer Port                          : ";
    master_->out() << balancerPort_ << endl;
    master_->out() << "  Best First Search Tolerance            : ";
    master_->out() << bestFirstTolerance_ << endl;
  }

  void ABA_PARMASTER::outputStatistics()
  {
    const int w = 6;

    master_->out() << endl;
    master_->out() << "Parallel Statistices" << endl << endl;
    master_->out() << "  Number of Subproblems sent        : ";
    master_->out() << setWidth(w) << subSentCount_ << endl;
    master_->out() << "  Number of Subproblems received    : ";
    master_->out() << setWidth(w) << subReceivedCount_ << endl;

    master_->out() << "  idle time before first subproblem : ";
    master_->out() << setWidth(w) << idleCowTimeFirst_ << endl;
    master_->out() << "  idle time during optimization     : ";
    master_->out() << setWidth(w) << idleCowTimeMiddle_ << endl;
    master_->out() << "  idle time after last subproblem   : ";
    master_->out() << setWidth(w) << idleCowTimeLast_ << endl;
  }

  int ABA_PARMASTER::registerPool(void *pool)
  {
    int n = registeredPools_.size();
    int i = lastRegisteredPool_;
    do {
      i++;
      if (i == n)
        i = 0;
      if (!registeredPools_[i]) {
        registeredPools_[i] = pool;
        lastRegisteredPool_ = i;
        return i;
      }
    }
    while (i != lastRegisteredPool_);

    registeredPools_.realloc(2*n);
    registeredPools_[n] = pool;
    registeredPools_.set(n+1, 2*n-1, 0);
    lastRegisteredPool_ = n;
    return n;
  }

  void ABA_PARMASTER::unregisterPool(int index)
  {
    registeredPools_[index] = 0;
  }

  void *ABA_PARMASTER::getPool(int index)
  {
    return registeredPools_[index];
  }

  void ABA_PARMASTER::connectService(int port, ABA_MESSAGE& msg, int destId)
  {
    char *name = hostname(destId).string();

    ACE_INET_Addr addr(port, name);
    ACE_SOCK_Connector connector;

    while (connector.connect(msg.stream(), addr) == -1) {
      if (debug(DEBUG_SOCKET))
        master_->out() << "DEBUG_SOCKET: server " << name << ":" << port
                       << " not available, retrying..." << endl;
      sleep(1);
    }
  }

  void ABA_PARMASTER::newHostDualBound(double x)
  {
    newHostDualBoundMutex_.acquire();

    if (hostDualBounds_.better(hostId_, x)) {

      hostDualBounds_.insert(hostId_, x);

      double newDual = hostDualBounds_.best();

      if (master_->betterDual(newDual))
        master_->dualBound(newDual);

      if (debug(DEBUG_NOTIFICATION))
        master_->out() << "DEBUG_NOTIFICATION: new dual bound " << x
                       << " broadcast by " << hostname() << endl;

      ABA_BROADCAST bcast(master_);

      bcast.pack(ABA_NOTIFYSERVER::NewHostDualBoundTag);
      bcast.pack(hostId_);
      bcast.pack(x);

      bcast.send();
    }

    newHostDualBoundMutex_.release();
  }

  void ABA_PARMASTER::newHostDualBound(ABA_MESSAGE &msg)
  {
    int id;
    double x;
    msg.unpack(id);
    msg.unpack(x);
    if (debug(DEBUG_NOTIFICATION))
      master_->out() << "DEBUG_NOTIFICATION: new dual bound " << x
                     << " received from " << hostname(id) << endl;

    newHostDualBoundMutex_.acquire();

    if (hostDualBounds_.better(id, x)) {

      hostDualBounds_.insert(id, x);

      double newDual = hostDualBounds_.best();

      if (master_->betterDual(newDual))
        master_->dualBound(newDual);
    }

    newHostDualBoundMutex_.release();
  }

  void ABA_PARMASTER::newPrimalBound(double x)
  {
    if (debug(DEBUG_NOTIFICATION))
      master_->out() << "new primal bound " << x << " broadcast by " 
                     << hostname() << endl;

    ABA_BROADCAST bcast(master_);

    bcast.pack(ABA_NOTIFYSERVER::NewPrimalBoundTag);
    bcast.pack(x);

    bcast.send();
  }

  void ABA_PARMASTER::newPrimalBound(ABA_MESSAGE &msg)
  {
    double x;
    msg.unpack(x);

    if (debug(DEBUG_NOTIFICATION))
      master_->out() << "DEBUG_NOTIFICATION: new primal bound " << x
                     << " received." << endl;

    newPrimalBoundMutex_.acquire();

    if (master_->betterPrimal(x))
      master_->primalBound(x, false);

    newPrimalBoundMutex_.release();
  }

  void ABA_PARMASTER::newOpenSubCount(int n, double best)
  {
    newOpenSubCountMutex_.acquire();

    if (n != openSubCount_[hostId_]) {
      openSubCount_[hostId_] = n;
      openSubBest_.insert(hostId_, best);

      //! trigger load balancing
      master_->openSub()->triggerSelect();

      if (debug(DEBUG_NOTIFICATION))
        master_->out() << "DEBUG_NOTIFICATION: " << hostname()
                       << " broadcasts: #subproblems=" << n
                       << ", best=" << best << endl;

      ABA_BROADCAST bcast(master_);

      bcast.pack(ABA_NOTIFYSERVER::NewOpenSubCountTag);
      bcast.pack(hostId_);
      bcast.pack(n);
      bcast.pack(best);

      bcast.send();
    }

    newOpenSubCountMutex_.release();
  }

  void ABA_PARMASTER::newOpenSubCount(ABA_MESSAGE &msg)
  {
    int id, n;
    double best;

    msg.unpack(id);
    msg.unpack(n);
    msg.unpack(best);

    if (debug(DEBUG_NOTIFICATION))
      master_->out() << "DEBUG_NOTIFICATION: received from "
                     << hostname(id) << ": #subproblems=" << n
                     << ", best=" << best << endl;

    newOpenSubCountMutex_.acquire();

    openSubCount_[id] = n;
    openSubBest_.insert(id, best);

    newOpenSubCountMutex_.release();

    //! trigger load balancing
    master_->openSub()->triggerSelect();
  }

  bool ABA_PARMASTER::balance()
  {
    int    host;
    double best = openSubBest_.best(&host);

    if (host<0)
      return false;

    if (host == hostId_)   //!< best subproblem is at this host
      return true;

    if (!master_->openSub()->empty()) {

      //! check whether the locally avalibale subproblem is good enough
      double total = master_->primalBound() - best;
      double dist  = master_->openSub()->dualBound() - best;

      if (master_->optSense()->max()) {
        total = - total;
        dist = - dist;
      }

      if (debug(DEBUG_BALANCER))
        master_->out() << "DEBUG_BALANCER: globalBest=" << best
                       << ", primal=" << master_->primalBound()
                       << ", localBest=" << master_->openSub()->dualBound()
                       << endl;

      if (total<=0)
        return false;

      if (dist * 100 <= total * bestFirstTolerance_)
        return true;
    }

    if (debug(DEBUG_BALANCER))
      master_->out() << "DEBUG_BALANCER: requesting subproblem with dual bound "
                     << best << " from " << hostname(host) << endl;

    ABA_MESSAGE msg;
    connectService(balancerPort_, msg, host);

    msg.pack(hostId_);    //!< the requesting host
    msg.pack(best);       //!< the requested dual bound
    msg.send();

    bool success;
    msg.receive();
    msg.unpack(success);

    if (debug(DEBUG_BALANCER))
      master_->out() << "DEBUG_BALANCER: " << hostname(host)
                     << (success ? " confirms to send " : " doesn't send ")
                     << "a subproblem." << endl;

    if (success) {
      ABA_SUB *sub = master_->unpackSub(msg);

      if (debug(DEBUG_BALANCER)) {
        master_->out() << "DEBUG_BALANCER: subproblem ";
        printId(sub->id());
        master_->out() << " received from " << hostname(host) << endl;
      }

      master_->openSub()->insert(sub);
    }
    
    return success;
  }

  void ABA_PARMASTER::terminationCheck()
  {
    assert(hostId_==0);

    if (debug(DEBUG_TERMINATION))
      master_->out() << "DEBUG_TERMINATION: terminationCheck() called" << endl;

    for (int i=0; i<hostCount_; i++)
      if (openSubCount_[i]!=0)
        return;                   //!< do not test for termination

    // if the termination check is already running we set a flag to
    // remember that we have to check again.
    if (terminationCheckMutex_.tryacquire()==-1) {
      terminationCheckAgain_ = true;
      return;
    }

    if (hasTerminated_) {
      terminationCheckMutex_.release();
      return;
    }

    do {
      terminationCheckAgain_ = false;

      if (debug(DEBUG_TERMINATION))
        master_->out() << "DEBUG_TERMINATION: Phase I started..." << endl;

      int idle;

      // \a terminationOk_ is set, if the processor is idle
      startTerminationCheck();

      {
        ABA_BROADCAST bcast(master_);
        bcast.pack(ABA_NOTIFYSERVER::TerminationCheckTag);
        bcast.send();
        bcast.receiveReply();    //!< receive accumulated dummy reply
    
        if (debug(DEBUG_TERMINATION))
          master_->out() << "DEBUG_TERMINATION: Phase II started..." << endl;

        bcast.pack(0);           //!< send some dummy data
        bcast.send();
        idle = bcast.receiveReply();    //!< receive accumulated reply
      }

      if (idle < hostCount_-1) {
        terminationMutex_.acquire();
        terminationOk_ = false;
        terminationMutex_.release();
      }

      if (debug(DEBUG_TERMINATION))
        master_->out() << "DEBUG_TERMINATION: terminationOk = " << terminationOk_ << endl;

    }
    while (terminationCheckAgain_ && !terminationOk_);

    if (terminationOk_)
      hasTerminated_ = true;

    terminationCheckMutex_.release();

    // should the system be terminated?
    if (terminationOk_) {
      terminate();

      ABA_BROADCAST bcast(master_);
      bcast.pack(ABA_NOTIFYSERVER::TerminateTag);
      bcast.send();
    }
  }

  void ABA_PARMASTER::terminationCheck(ABA_MESSAGE &msg)
  {
    if (debug(DEBUG_TERMINATION))
      master_->out() << "DEBUG_TERMINATION: handling phase I." << endl;

    // \a terminationOk_ is set, if the processor is idle
    startTerminationCheck();

    // send dummy reply for synchronisation
    msg.pack(false);
    msg.send();

    // \a terminateFlag_ is reset asynchronously when a subproblem is received

    // wait for phase II
    msg.receive();
    int dummy;
    msg.unpack(dummy);

    if (debug(DEBUG_TERMINATION))
      master_->out() << "DEBUG_TERMINATION: handling phase II, "
                     << "terminationOk = " << terminationOk_ << endl;

    // send reply
    msg.pack(terminationOk_);
    msg.send();
  }

  void ABA_PARMASTER::startTerminationCheck()
{
  terminationMutex_.acquire();
  if (workCount_==0)
    terminationOk_ = true;
  terminationMutex_.release();
}

void ABA_PARMASTER::incWorkCount()
{
  terminationMutex_.acquire();
  workCount_++;
  terminationOk_ = false;
  terminationMutex_.release();
}

void ABA_PARMASTER::decWorkCount()
{
  terminationMutex_.acquire();
  workCount_--;
  terminationMutex_.release();
  if (workCount_==0) {
    ABA_NOTIFICATION msg(master_, 0);
    msg.pack(ABA_NOTIFYSERVER::TriggerTerminationCheckTag);
    msg.send();
  }
}

void ABA_PARMASTER::terminate()
{
  if (debug(DEBUG_TERMINATION))
    master_->out() << "DEBUG_TERMINATION: Terminating all Threads..." << endl;

  master_->openSub()->terminate();

  if (debug(DEBUG_TERMINATION))
    master_->out() << "DEBUG_TERMINATION: finished." << endl;
}

  int ABA_PARMASTER::newId(int fatherId)
  {
    if (master_->vbcLog()!=ABA_MASTER::NoVbc) {
      if (hostId_==0) {
        idCounterMutex_.acquire();
        int id = idCounter_++;
        master_->treeInterfaceNewNode(fatherId, id);
        idCounterMutex_.release();
        return id;
      }
      // request unique Id from host 0
      ABA_NOTIFICATION msg(master_, 0);
      msg.pack(ABA_NOTIFYSERVER::NewIdTag);
      msg.pack(fatherId);
      msg.send();
      msg.receive();
      return msg.unpackInt();
    }
    else {
      idCounterMutex_.acquire();
      int id = idCounter_++;
      idCounterMutex_.release();
      return hostId_*1000000 + id;
    }
  }

  void ABA_PARMASTER::printId(int id)
  {
    if (master_->vbcLog()==ABA_MASTER::NoVbc)
      master_->out() << "(" << hostname(id/1000000) << "," << id%1000000 << ")";
    else
      master_->out() << id;
  }
