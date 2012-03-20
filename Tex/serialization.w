@*1 ABA\_CSENSE.

@q m f Constructor @>
@ The message constructor creates the |ABA_CSENSE| from an |ABA_MESSAGE|.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object from which the sense is initialized.}
  \EndArg
@q m e a @>

@(csense.cc@>=
@q m b f @>
  ABA_CSENSE::ABA_CSENSE(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
@q m e f @>
  : @/
    glob_(glob)
  {
    sense_ = (SENSE) msg.unpackInt();
  }

@q m f pack @>
@ The function |pack()| packs the data of the sense in an |ABA_MESSAGE| object.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the sense is packed.}
  \EndArg
@q m e a @>

@(csense.cc@>=
@q m b f @>
  void ABA_CSENSE::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    msg.pack((int)sense_);
  }


@*1 ABA\_VARTYPE.

@q m f Constructor @>
@q m e t @>

@ The message constructor creates the type from an |ABA_MESSAGE|.
@q m e t @>

@q m b a @>
    \BeginArg
    \docarg{|msg|}{The message from which the type is initialized.}
    \EndArg
@q m e a @>

@(vartype.cc@>=
@q m b f @>
  ABA_VARTYPE::ABA_VARTYPE(ABA_MESSAGE &msg)
@q m e f @>
  {
    type_ = (TYPE) msg.unpackInt();
  }

@q m f pack @>
@ The function |pack()| packs the type in an |ABA_MESSAGE|.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the type is packed.}
  \EndArg
@q m e a @>

@(vartype.cc@>=
@q m b f @>
  void ABA_VARTYPE::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    msg.pack((int)type_);
  }


@*1 ABA\_FSVARSTAT.

@q m f Constructor @>
@q m e t @>

@ The message constructor creates the status from an |ABA_MESSAGE|.
@q m e t @>

@q m b a @>
    \BeginArg
    \docarg{|glob|}{A pointer to a global object.}
    \docarg{|msg|}{The message from which the object is initialized.}
    \EndArg
@q m e a @>

@(fsvarstat.cc@>=
@q m b f @>
  ABA_FSVARSTAT::ABA_FSVARSTAT(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
@q m e f @>
    : @/
    glob_(glob)
  {
    status_ = (STATUS) msg.unpackInt();
    msg.unpack(value_);
  }

@q m f pack @>
@ The function |pack()| packs the data of the status in an |ABA_MESSAGE|.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the status is packed.}
  \EndArg
@q m e a @>

@(fsvarstat.cc@>=
@q m b f @>
  void ABA_FSVARSTAT::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    msg.pack((int)status_);
    msg.pack(value_);
  }


@*1 ABA\_LPVARSTAT.

@q m f Constructor @>
@q m e t @>

@ The message constructor creates the |ABA_LPVARSTAT| from a message.
@q m e t @>

@q m b a @>
    \BeginArg
    \docarg{|glob|}{A pointer to a global object.}
    \docarg{|msg|}{The message from which the object is initialized.}
    \EndArg
@q m e a @>

@(lpvarstat.cc@>=
@q m b f @>
  ABA_LPVARSTAT::ABA_LPVARSTAT(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
@q m e f @>
    : @/
    glob_(glob)
  {
    status_ = (STATUS) msg.unpackInt();
  }


@q m f pack @>
@ The function |pack()| packs the data of the |ABA_LPVARSTAT| in a message.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the status is packed.}
  \EndArg
@q m e a @>

@(lpvarstat.cc@>=
@q m b f @>
  void ABA_LPVARSTAT::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    msg.pack((int)status_);
  }


@*1 ABA\_SLACKSTAT.

@q m f Constructor @>
@q m e t @>

@ The message constructor creates the status from a message.
@q m e t @>

@q m b a @>
    \BeginArg
    \docarg{|glob|}{A pointer to a global object.}
    \docarg{|msg|}{The message from which the object is initialized.}
    \EndArg
@q m e a @>

@(slackstat.cc@>=
@q m b f @>
  ABA_SLACKSTAT::ABA_SLACKSTAT(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
@q m e f @>
    : @/
    glob_(glob)
  {
    status_ = (STATUS) msg.unpackInt();
  }


@q m f pack @>
@ The function |pack()| packs the data of the status in a message.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the status is packed.}
  \EndArg
@q m e a @>

@(slackstat.cc@>=
@q m b f @>
  void ABA_SLACKSTAT::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    msg.pack((int)status_);
  }


@*1 ABA\_BRANCHRULE.

@(branchrule.h@>=
#define SETBRANCHRULE_CLASSID     9001            // preliminary
#define VALBRANCHRULE_CLASSID     9002            // preliminary
#define CONBRANCHRULE_CLASSID     9003            // preliminary
#define BOUNDBRANCHRULE_CLASSID   9004            // preliminary

@q m f Constructor @>
@q m e t @>

@ The message constructor creates the |ABA_BRANCHRULE| from an |ABA_MESSAGE|.

@q m e t @>

@q m b a @>
    \BeginArg
    \docarg
      {|master|}{A pointer to the corresponding master of the optimization.}
    \docarg
      {|msg|}{The message from which the object is initialized.}
    \EndArg
@q m e a @>

@(branchrule.cc@>=
@q m b f @>
  ABA_BRANCHRULE::ABA_BRANCHRULE(const ABA_MASTER *master, ABA_MESSAGE &msg)
@q m e f @>
    : @/
    master_(master)
  { }

@q m f pack @>
@ The function |pack()| packs the data of the |ABA_BRANCHRULE|
  in an |ABA_MESSAGE| object. This virtual function has to be redefined
  in a derived class if additional data should be packed. In this
  case the |pack()| function of the base class must be called
  by the |pack()| function of the derived class.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the |ABA_BRANCHRULE| is packed.}
  \EndArg
@q m e a @>

@(branchrule.cc@>=
@q m b f @>
  void ABA_BRANCHRULE::pack(ABA_MESSAGE &msg) const
@q m e f @>
  { }


@*1 ABA\_SETBRANCHRULE.

@q m f Constructor @>
@q m e t @>

@ The message constructor creates the |ABA_SETBRANCHRULE| from an |ABA_MESSAGE|.

@q m e t @>

@q m b a @>
    \BeginArg
    \docarg
      {|master|}{A pointer to the corresponding master of the optimization.}
    \docarg
      {|msg|}{The message from which the object is initialized.}
    \EndArg
@q m e a @>

@(setbranchrule.cc@>=
@q m b f @>
  ABA_SETBRANCHRULE::ABA_SETBRANCHRULE(const ABA_MASTER *master, ABA_MESSAGE &msg)
@q m e f @>
    : @/
    ABA_BRANCHRULE(master, msg)
  {
    msg.unpack(variable_);
    status_ = (ABA_FSVARSTAT::STATUS) msg.unpackInt();
    msg.unpack(oldLpBound_);
  }

@q m f pack @>
@ The virtual function |pack()| packs the data of the |ABA_SETBRANCHRULE|
  in an |ABA_MESSAGE| object. The |pack()| function of the base class
  has to be called before the local data is packed.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the |ABA_SETBRANCHRULE| is packed.}
  \EndArg
@q m e a @>

@(setbranchrule.cc@>=
@q m b f @>
  void ABA_SETBRANCHRULE::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    ABA_BRANCHRULE::pack(msg);
    msg.pack(variable_);
    msg.pack((int)status_);
    msg.pack(oldLpBound_);
  }

@q m f classId @>
@ The abstract virtual function |classId()| of |ABA_BRANCHRULE| has to be
  implemented in each derived subclass. It has to return an integer
  which identifies the class.

@q m b r @>
  \BeginReturn
  The class identification.
  \EndReturn
@q m e r @>

@(setbranchrule.cc@>=
@q m b f @>
  int ABA_SETBRANCHRULE::classId() const
@q m e f @>
  {
    return SETBRANCHRULE_CLASSID;
  }


@*1 ABA\_BOUNDBRANCHRULE.

@q m f Constructor @>
@q m e t @>

@ The message constructor creates the |ABA_BOUNDBRANCHRULE| from an |ABA_MESSAGE|.

@q m e t @>

@q m b a @>
    \BeginArg
    \docarg
      {|master|}{A pointer to the corresponding master of the optimization.}
    \docarg
      {|msg|}{The message from which the object is initialized.}
    \EndArg
@q m e a @>

@(boundbranchrule.cc@>=
@q m b f @>
  ABA_BOUNDBRANCHRULE::ABA_BOUNDBRANCHRULE(const ABA_MASTER *master, ABA_MESSAGE &msg)
@q m e f @>
    : @/
    ABA_BRANCHRULE(master, msg)
  {
    msg.unpack(variable_);
    msg.unpack(lBound_);
    msg.unpack(uBound_);
    msg.unpack(oldLpLBound_);
    msg.unpack(oldLpUBound_);
  }

@q m f pack @>
@ The virtual function |pack()| packs the data of the |ABA_BOUNDBRANCHRULE|
  in an |ABA_MESSAGE| object. The |pack()| function of the base class
  has to be called before the local data is packed.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the |ABA_BOUNDBRANCHRULE| is packed.}
  \EndArg
@q m e a @>

@(boundbranchrule.cc@>=
@q m b f @>
  void ABA_BOUNDBRANCHRULE::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    ABA_BRANCHRULE::pack(msg);
    msg.pack(variable_);
    msg.pack(lBound_);
    msg.pack(uBound_);
    msg.pack(oldLpLBound_);
    msg.pack(oldLpUBound_);
  }

@q m f classId @>
@ The abstract virtual function |classId()| of |ABA_BRANCHRULE| has to be
  implemented in each derived subclass. It has to return an integer
  which identifies the class.

@q m b r @>
  \BeginReturn
  The class identification.
  \EndReturn
@q m e r @>

@(boundbranchrule.cc@>=
@q m b f @>
  int ABA_BOUNDBRANCHRULE::classId() const
@q m e f @>
  {
    return BOUNDBRANCHRULE_CLASSID;
  }



@*1 ABA\_VALBRANCHRULE.

@q m f Constructor @>
@q m e t @>

@ The message constructor creates the |ABA_VALBRANCHRULE| from an |ABA_MESSAGE|.

@q m e t @>

@q m b a @>
    \BeginArg
    \docarg
      {|master|}{A pointer to the corresponding master of the optimization.}
    \docarg
      {|msg|}{The message from which the object is initialized.}
    \EndArg
@q m e a @>

@(valbranchrule.cc@>=
@q m b f @>
  ABA_VALBRANCHRULE::ABA_VALBRANCHRULE(const ABA_MASTER *master, ABA_MESSAGE &msg)
@q m e f @>
    : @/
    ABA_BRANCHRULE(master, msg)
  {
    msg.unpack(variable_);
    msg.unpack(value_);
    msg.unpack(oldLpLBound_);
    msg.unpack(oldLpUBound_);
  }

@q m f pack @>
@ The virtual function |pack()| packs the data of the |ABA_VALBRANCHRULE|
  in an |ABA_MESSAGE| object. The |pack()| function of the base class
  has to be called before the local data is packed.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the |ABA_VALBRANCHRULE| is packed.}
  \EndArg
@q m e a @>

@(valbranchrule.cc@>=
@q m b f @>
  void ABA_VALBRANCHRULE::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    ABA_BRANCHRULE::pack(msg);
    msg.pack(variable_);
    msg.pack(value_);
    msg.pack(oldLpLBound_);
    msg.pack(oldLpUBound_);
  }

@q m f classId @>
@ The abstract virtual function |classId()| of |ABA_BRANCHRULE| has to be
  implemented in each derived subclass. It has to return an integer
  which identifies the class.

@q m b r @>
  \BeginReturn
  The class identification.
  \EndReturn
@q m e r @>

@(valbranchrule.cc@>=
@q m b f @>
  int ABA_VALBRANCHRULE::classId() const
@q m e f @>
  {
    return VALBRANCHRULE_CLASSID;
  }


@*1 ABA\_CONBRANCHRULE.

@q m f Constructor @>
@q m e t @>

@ The message constructor creates the |ABA_CONBRANCHRULE| from an |ABA_MESSAGE|.

@q m e t @>

@q m b a @>
    \BeginArg
    \docarg
      {|master|}{A pointer to the corresponding master of the optimization.}
    \docarg
      {|msg|}{The message from which the object is initialized.}
    \EndArg
@q m e a @>

@(conbranchrule.cc@>=
@q m b f @>
  ABA_CONBRANCHRULE::ABA_CONBRANCHRULE(const ABA_MASTER *master, ABA_MESSAGE &msg)
@q m e f @>
    : @/
    ABA_BRANCHRULE(master, msg), @/
    poolSlotRef_(master)
  {
    ABA_ID id(msg);
    char needConstraint;

    ABA_POOL<ABA_CONSTRAINT, ABA_VARIABLE> *pool = (ABA_POOL<ABA_CONSTRAINT, ABA_VARIABLE>*)
      master_->parmaster()->getPool(id.index());
    ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE> *ps = pool->findSlot(id);
    if (ps) {
      poolSlotRef_.slot(ps);
      needConstraint=0;
    }
    else
      needConstraint=1;

    // msg.clear();
    msg.pack(needConstraint);
    msg.send();		// send flag if constraint is missing

    msg.receive();	// receive constraint if needed
    if (needConstraint) {
      int classId;
      msg.unpack(classId);
      ABA_CONSTRAINT *con = (ABA_CONSTRAINT*) master_->unpackConVar(msg, classId);
      if (debug(DEBUG_MESSAGE_CONVAR)) {
        master_->out() << "DEBUG_MESSAGE_CONVAR: Constraint " << id << " (classId=" << classId
                       << ") of ABA_CONBRANCHRULE received." << endl;
      }
      ABA_POOL<ABA_CONSTRAINT, ABA_VARIABLE> *pool = (ABA_POOL<ABA_CONSTRAINT, ABA_VARIABLE>*)
        master_->parmaster()->getPool(id.index());
      ps = pool->insert(con);
      if (ps == 0) {
        master_->err() << "ABA_CONBRANCHRULE::ABA_CONBRANCHRULE(): no room"
                          " to insert constraint into pool." << endl;
        exit(Fatal);
      }
      ps->setIdentification(id);
      poolSlotRef_.slot(ps);
    }
  }

@q m f pack @>
@ The virtual function |pack()| packs the data of the |ABA_CONBRANCHRULE|
  in an |ABA_MESSAGE| object. The |pack()| function of the base class
  has to be called before the local data is packed.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the |ABA_CONBRANCHRULE| is packed.}
  \EndArg
@q m e a @>

@(conbranchrule.cc@>=
@q m b f @>
  void ABA_CONBRANCHRULE::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    ABA_BRANCHRULE::pack(msg);
    if (poolSlotRef_.conVar() == 0) {		// removed conVar
      master_->err() << "ABA_CONBRANCHRULE::pack(): branching constraint not"
                        " available!" << endl;
      exit(Fatal);
    }

    ABA_POOLSLOT<ABA_CONSTRAINT, ABA_VARIABLE> *ps = poolSlotRef_.slot();
    if (!ps->getIdentification().isInitialized())
      ps->setNewIdentification();
    ps->getIdentification().pack(msg);

    msg.send();			// send |ABA_ID| of constraint

    msg.receive();		// receive flag if constraint is missing
    char needConstraint;
    msg.unpack(needConstraint);
    // msg.clear();

    if (needConstraint) {
      ABA_CONSTRAINT *con = poolSlotRef_.conVar();
      if (debug(DEBUG_MESSAGE_CONVAR)) {
        master_->out() << "DEBUG_MESSAGE_CONVAR: sending Constraint " << ps->getIdentification()
                       << " (classId=" << con->classId()
                       << ") of ABA_CONBRANCHRULE..." << endl;
      }
      msg.pack(con->classId());
      con->pack(msg);
    }
  }

@q m f classId @>
@ The abstract virtual function |classId()| of |ABA_BRANCHRULE| has to be
  implemented in each derived subclass. It has to return an integer
  which identifies the class.

@q m b a @>
  \BeginReturn
  The class identification.
  \EndReturn
@q m e a @>

@(conbranchrule.cc@>=
@q m b f @>
  int ABA_CONBRANCHRULE::classId() const
@q m e f @>
  {
    return CONBRANCHRULE_CLASSID;
  }


@*1 ABA\_POOL.

@q m f findSlot @>
@q m e t @>
@ The function |findSlot()| checks if a constraint/variable with the supplied
  |ABA_ID| exists in the pool.
@q m b r @>
  \BeginReturn
  A pointer to the poolslot of the constraint/variable if it exists in the
  pool, otherwise 0.
  \EndReturn
@q m e r @>

@(pool.inc@>=
  template<class BaseType, class CoType>
@q m b f @>
  ABA_POOLSLOT<BaseType, CoType> *ABA_POOL<BaseType, CoType>::findSlot(const ABA_ID &id) const
@q m e f @>
  {
    return identificationMap_->find(id);
  }

@q m f identificationMap @>
@q m e t @>
@ The function |identificationMap()|.

@q m b r @>
  \BeginReturn
  A pointer to the |ABA_IDMAP| of the |ABA_POOL| of this processor.
  \EndReturn
@q m e r @>

@(pool.inc@>=
  template<class BaseType, class CoType>
@q m b f @>
  ABA_IDMAP<ABA_POOLSLOT<BaseType, CoType> @= @> > *
    ABA_POOL<BaseType, CoType>::identificationMap() const
@q m e f @>
  {
    return identificationMap_;
  }


@*1 ABA\_POOLSLOT.

@q m f getIdentification @>
@ The function |getIdentification()| returns the system wide identification
  of the constraint/variable referenced by the |ABA_POOLSLOT|.
@q m e t @>

@q m b r @>
  \BeginReturn
  The |ABA_ID| of the  constraint/variable referenced by the |ABA_POOLSLOT|.
  \EndReturn
@q m e r @>

@(poolslot.inc@>=
  template<class BaseType, class CoType>
@q m b f @>
  const ABA_ID &ABA_POOLSLOT<BaseType, CoType>::getIdentification(void) const
@q m e f @>
  {
    if (conVar_ == 0) {
      master_->err() << "ABA_POOLSLOT::getIdentification() : no "
                        "constraint/variable available in this slot!" << endl;
      exit(Fatal);
    }
    return conVar_->identification_;
  }

@q m f setIdentification @>
@ The function |setIdentification()| sets the system wide identification
  of the constraint/variable referenced by the |ABA_POOLSLOT| and registers it
  with the |ABA_IDMAP| of the pool.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|id|}{The |ABA_ID| to be assigned.}
  \EndArg
@q m e a @>

@(poolslot.inc@>=
  template<class BaseType, class CoType>
@q m b f @>
  void ABA_POOLSLOT<BaseType, CoType>::setIdentification(const ABA_ID &id)
@q m e f @>
  {
    if (conVar_) {
      conVar_->identification_ = id;
      pool_->identificationMap()->insert(id, this);
    }
    else {
      master_->err() << "ABA_POOLSLOT::setIdentification() : no "
                        "constraint/variable available in this slot!" << endl;
      exit(Fatal);
    }
  }

@q m f setNewIdentification @>
@ The function |setNewIdentification()| sets the system wide identification
  of the constraint/variable referenced by the |ABA_POOLSLOT| to a new |ABA_ID| and
  registers it with the |ABA_IDMAP| of the pool.
@q m e t @>

@(poolslot.inc@>=
  template<class BaseType, class CoType>
@q m b f @>
  void ABA_POOLSLOT<BaseType, CoType>::setNewIdentification()
@q m e f @>
  {
    if (conVar_)
      pool_->identificationMap()->insertWithNewId(conVar_->identification_, this);
    else {
      master_->err() << "ABA_POOLSLOT::setNewIdentification() : no "
                        "constraint/variable available in this slot!" << endl;
      exit(Fatal);
    }
  }


@*1 ABA\_CONVAR.
@(convar.h@>=
#define NUMCON_CLASSID  8001            // preliminary
#define ROWCON_CLASSID  8002            // preliminary
#define SROWCON_CLASSID 8003            // preliminary
#define NUMVAR_CLASSID  8004            // preliminary
#define COLVAR_CLASSID  8005            // preliminary

@q m f Constructor @>
@q m e t @>

@ The message constructor creates the constraint/variable from an |ABA_MESSAGE|.

  A constraint/variable can only be constructed by the message constructor,
  if the associated subproblem of the constraint/variable already exists
  in the memory of the receiving processor. This is verified using the |ABA_ID| of
  the subproblem. The |sub_| pointer of the constraint/variable is then set
  to this subproblem.
@q m e t @>

@q m b a @>
    \BeginArg
    \docarg
      {|master|}{A pointer to the corresponding master of the optimization.}
    \docarg
      {|msg|}{The message from which the object is initialized.}
    \EndArg
@q m e a @>

@(convar.cc@>=
@q m b f @>
  ABA_CONVAR::ABA_CONVAR(const ABA_MASTER *master, ABA_MESSAGE &msg)
@q m e f @>
    : @/
    master_(master), @/
    sub_(0), @/
    expanded_(false), @/
    nReferences_(0), @/
    nActive_(0), @/
    nLocks_(0) @/
  {
    int bits = msg.unpackInt();

    assert((bits&1)==0);             // |sub_| must be 0
    dynamic_  = ((bits&2)?true:false);
    local_    = ((bits&4)?true:false);
  }


@q m f pack @>
@ The function |pack()| packs the data of the constraint/variable
  in an |ABA_MESSAGE| object. This virtual function has to be redefined
  in a derived class if additional data should be packed. In this
  case the |pack()| function of the base class must be called
  by the |pack()| function of the derived class.

  The constraint/variable must be packed and unpacked in compressed
  format. Note, that it is not required for the constraint/variable
  to be in compressed format at the time when it is packed!
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the constraint/variable
                 is packed.}
  \EndArg
@q m e a @>

@(convar.cc@>=
@q m b f @>
  void ABA_CONVAR::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    msg.pack( ((sub_&&local_)?1:0) + 2*dynamic_ + 4*local_);

    if (sub_&&local_) {
      master_->err() << "ABA_CONVAR::pack(): constraint/variable must "
                        "be globally valid." << endl;
      exit(Fatal);
    }
  }



@*1 ABA\_ROW.

@q m f Constructor @>
@q m e t @>
@ The message constructor creates the row from an |ABA_MESSAGE|.

@q m b a @>
    \BeginArg
    \docarg
      {|glob|}{A pointer to the corresponding global object.}
    \docarg
      {|msg|}{The message from which the object is initialized.}
    \EndArg
@q m e a @>

@(row.cc@>=
@q m b f @>
  ABA_ROW::ABA_ROW(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
@q m e f @>
    : @/
    ABA_SPARVEC(glob, msg), @/
    sense_(glob, msg)
  {
    msg.unpack(rhs_);
  }


@q m f pack @>
@ The function |pack()| packs the data of the row in an |ABA_MESSAGE| object.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the row is packed.}
  \EndArg
@q m e a @>

@(row.cc@>=
@q m b f @>
  void ABA_ROW::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    ABA_SPARVEC::pack(msg);
    sense_.pack(msg);
    msg.pack(rhs_);
  }


@*1 ABA\_COLUMN.

@q m f Constructor @>
@q m e t @>
@ The message constructor creates the column from an |ABA_MESSAGE|.

@q m b a @>
    \BeginArg
    \docarg
      {|glob|}{A pointer to the corresponding global object.}
    \docarg
      {|msg|}{The message from which the object is initialized.}
    \EndArg
@q m e a @>

@(column.cc@>=
@q m b f @>
  ABA_COLUMN::ABA_COLUMN(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
@q m e f @>
    : @/
    ABA_SPARVEC(glob, msg)
  {
    msg.unpack(obj_);
    msg.unpack(lBound_);
    msg.unpack(uBound_);
  }


@q m f pack @>
@ The function |pack()| packs the data of the column in an |ABA_MESSAGE| object.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the column is packed.}
  \EndArg
@q m e a @>

@(column.cc@>=
@q m b f @>
  void ABA_COLUMN::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    ABA_SPARVEC::pack(msg);
    msg.pack(obj_);
    msg.pack(lBound_);
    msg.pack(uBound_);
  }


@*1 ABA\_NUMCON.

@q m f Constructor @>
@ The message constructor creates the constraint from an |ABA_MESSAGE|.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object from which the constraint is initialized.}
  \EndArg
@q m e a @>

@(numcon.cc@>=
@q m b f @>
  ABA_NUMCON::ABA_NUMCON(const ABA_MASTER *master, ABA_MESSAGE &msg)
@q m e f @>
  : @/
    ABA_CONSTRAINT(master, msg)
  {
    msg.unpack(number_);
  }

@q m f pack @>
@ The function |pack()| packs the data of the constraint in an
  |ABA_MESSAGE| object.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the constraint is packed.}
  \EndArg
@q m e a @>

@(numcon.cc@>=
@q m b f @>
  void ABA_NUMCON::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    ABA_CONSTRAINT::pack(msg);
    msg.pack(number_);
  }

@q m f classId @>
@ The function |classId()| returns an integer which identifies the class.

@q m b r @>
  \BeginReturn
  The class identification.
  \EndReturn
@q m e r @>

@(numcon.cc@>=
@q m b f @>
  int ABA_NUMCON::classId() const
@q m e f @>
  {
    return NUMCON_CLASSID;
  }


@*1 ABA\_ROWCON.


@q m f Constructor @>
@ The message constructor creates the constraint from an |ABA_MESSAGE|.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object from which the constraint is initialized.}
  \EndArg
@q m e a @>

@(rowcon.cc@>=
@q m b f @>
  ABA_ROWCON::ABA_ROWCON(const ABA_MASTER *master, ABA_MESSAGE &msg)
@q m e f @>
  : @/
    ABA_CONSTRAINT(master, msg), @/
    row_(master, msg)
  {  }

@q m f pack @>
@ The function |pack()| packs the data of the constraint in an
  |ABA_MESSAGE| object.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the constraint is packed.}
  \EndArg
@q m e a @>

@(rowcon.cc@>=
@q m b f @>
  void ABA_ROWCON::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    ABA_CONSTRAINT::pack(msg);
    row_.pack(msg);
  }

@q m f classId @>
@ The function |classId()| returns an integer which identifies the class.

@q m b r @>
  \BeginReturn
  The class identification.
  \EndReturn
@q m e r @>

@(rowcon.cc@>=
@q m b f @>
  int ABA_ROWCON::classId() const
@q m e f @>
  {
    return ROWCON_CLASSID;
  }


@*1 ABA\_NUMVAR.


@q m f Constructor @>
@ The message constructor creates the variable from an |ABA_MESSAGE|.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object from which the variable is initialized.}
  \EndArg
@q m e a @>

@(numvar.cc@>=
@q m b f @>
  ABA_NUMVAR::ABA_NUMVAR(const ABA_MASTER *master, ABA_MESSAGE &msg)
@q m e f @>
  : @/
    ABA_VARIABLE(master, msg)
  {
    msg.unpack(number_);
  }

@q m f pack @>
@ The function |pack()| packs the data of the variable in an
  |ABA_MESSAGE| object.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the variable is packed.}
  \EndArg
@q m e a @>

@(numvar.cc@>=
@q m b f @>
  void ABA_NUMVAR::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    ABA_VARIABLE::pack(msg);
    msg.pack(number_);
  }

@q m f classId @>
@ The function |classId()| returns an integer which identifies the class.

@q m b r @>
  \BeginReturn
  The class identification.
  \EndReturn
@q m e r @>

@(numvar.cc@>=
@q m b f @>
  int ABA_NUMVAR::classId() const
@q m e f @>
  {
    return NUMVAR_CLASSID;
  }


@*1 ABA\_SROWCON.


@q m f Constructor @>
@ The message constructor creates the constraint from an |ABA_MESSAGE|.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object from which the constraint is initialized.}
  \EndArg
@q m e a @>

@(srowcon.cc@>=
@q m b f @>
  ABA_SROWCON::ABA_SROWCON(const ABA_MASTER *master, ABA_MESSAGE &msg)
@q m e f @>
  : @/
    ABA_ROWCON(master, msg)
  {  }

@q m f pack @>
@ The function |pack()| packs the data of the constraint in an
  |ABA_MESSAGE| object.

  This function needs not to be implemented here because it just calls the
  |pack()| function of the base class. It is provided for the convenience
  of the user only.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the constraint is packed.}
  \EndArg
@q m e a @>

@(srowcon.cc@>=
@q m b f @>
  void ABA_SROWCON::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    ABA_ROWCON::pack(msg);
  }

@q m f classId @>
@ The function |classId()| returns an integer which identifies the class.

@q m b r @>
  \BeginReturn
  The class identification.
  \EndReturn
@q m e r @>

@(srowcon.cc@>=
@q m b f @>
  int ABA_SROWCON::classId() const
@q m e f @>
  {
    return SROWCON_CLASSID;
  }


@*1 ABA\_COLVAR.


@q m f Constructor @>
@ The message constructor creates the variable from an |ABA_MESSAGE|.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object from which the variable is initialized.}
  \EndArg
@q m e a @>

@(colvar.cc@>=
@q m b f @>
  ABA_COLVAR::ABA_COLVAR(const ABA_MASTER *master, ABA_MESSAGE &msg)
@q m e f @>
  : @/
    ABA_VARIABLE(master, msg), @/
    column_(master, msg)
  {  }

@q m f pack @>
@ The function |pack()| packs the data of the variable in an
  |ABA_MESSAGE| object.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the variable is packed.}
  \EndArg
@q m e a @>

@(colvar.cc@>=
@q m b f @>
  void ABA_COLVAR::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    ABA_VARIABLE::pack(msg);
    column_.pack(msg);
  }

@q m f classId @>
@ The function |classId()| returns an integer which identifies the class.

@q m b r @>
  \BeginReturn
  The class identification.
  \EndReturn
@q m e r @>

@(colvar.cc@>=
@q m b f @>
  int ABA_COLVAR::classId() const
@q m e f @>
  {
    return COLVAR_CLASSID;
  }


@*1 ABA\_ACTIVE.


@q m f Constructor @>
@ The message constructor creates the object from an |ABA_MESSAGE|.
  The |ABA_POOLSLOTREF| references to the constraints/variables which are
  locally available are initialized. The indexes of the missing
  constraints/variables are collected in an |ABA_BUFFER|.

  To complete the initialization of the object the function
  |unpackNeeded()| has to be called afterwards.
  
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg
    {|msg|}
    {The |ABA_MESSAGE| object from which the object is constructed.}
  \docarg
    {|idBuffer|}
    {Returns a pointer to an |ABA_BUFFER| containing the IDs
     of all referenced constraints/variables. |*idBuffer| is allocated in
     this function and has to be freed by the caller.}
  \docarg
    {|needed|}
    {Returns a pointer to an |ABA_BUFFER| containing the indexes of the
     constraints/variables not locally available. |*needed| is allocated in
     this function and has to be freed by the caller.}
  \EndArg
@q m e a @>

@(active.inc@>=
  template<class BaseType, class CoType>
@q m b f @>
  ABA_ACTIVE<BaseType, CoType>::ABA_ACTIVE(const ABA_MASTER *master, ABA_MESSAGE &msg,
                                   ABA_BUFFER<ABA_ID> **idBuffer, ABA_BUFFER<int> **needed)
@q m e f @>
  : @/
    master_(master), @/
    n_(msg.unpackInt()), @/
    active_(master, msg.unpackInt()), @/
    redundantAge_(master, msg)
  {
    *idBuffer = new @[ABA_BUFFER<ABA_ID>(master, n_)@];
    *needed   = new @[ABA_BUFFER<int>(master, n_)@];

    for (int i = 0; i < n_; i++) {
      ABA_ID id(msg);
      (*idBuffer)->push(id);
      if (id.isInitialized()) {
        ABA_POOL<BaseType, CoType> *pool = (ABA_POOL<BaseType, CoType>*)
          master_->parmaster()->getPool(id.index());
        ABA_POOLSLOT<BaseType, CoType> *ps = pool->findSlot(id);
        if (ps)
          active_[i] = new @[ABA_POOLSLOTREF<BaseType, CoType>(ps)@];
        else {
          (*needed)->push(i);
          active_[i] = 0;		// will be initialized in unpackNeeded()
        }
      }
      else				// removed conVar
        active_[i] = 0;
    }
  }

@q m f unpackNeeded @>
@ The function |unpackNeeded()| unpacks the constraints/variables
  which were packed by the function |packNeeded()|.
  The corresponding entries of the |active_| array are initialized.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg
    {|msg|}
    {The |ABA_MESSAGE| object from which the data is unpacked.}
  \docarg
    {|idBuffer|}
    {A |ABA_BUFFER| with the IDs of all referenced constraints/variables.}
  \docarg
    {|needed|}
    {A |ABA_BUFFER| with the indexes of the constraints/variables
     to be unpacked.}
  \EndArg
@q m e a @>

@(active.inc@>=
  template<class BaseType, class CoType>
@q m b f @>
  void ABA_ACTIVE<BaseType, CoType>::unpackNeeded(ABA_MESSAGE &msg,
                                              const ABA_BUFFER<ABA_ID> &idBuffer,
                                              const ABA_BUFFER<int> &needed)
@q m e f @>
  {
    for (int i = 0; i < needed.number(); i++) {
      int classId;
      msg.unpack(classId);
      BaseType *cv = (BaseType*) master_->unpackConVar(msg, classId);
      const ABA_ID &id = idBuffer[needed[i]];
      if (debug(DEBUG_MESSAGE_CONVAR)) {
        master_->out() << "DEBUG_MESSAGE_CONVAR: Constraint/Variable " << id
                       << " (classId=" << classId << ") received." << endl;
      }
      ABA_POOL<BaseType, CoType> *pool = (ABA_POOL<BaseType, CoType>*)
        master_->parmaster()->getPool(id.index());
      ABA_POOLSLOT<BaseType, CoType> *ps = pool->insert(cv);
      if (ps == 0) {
        master_->err() << "ABA_ACTIVE::ABA_ACTIVE(): no room to insert constraint"
                          " into pool." << endl;
        exit(Fatal);
      }
      ps->setIdentification(id);
      active_[needed[i]] = new @[ABA_POOLSLOTREF<BaseType, CoType>(ps)@];
    }
  }

@q m f pack @>
@ The function |pack()| packs the size information of the object and
  the IDs of the referenced constraints/variables in an |ABA_MESSAGE| object.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the IDs are packed.}
  \EndArg
@q m e a @>

@(active.inc@>=
  template<class BaseType, class CoType>
@q m b f @>
  void ABA_ACTIVE<BaseType, CoType>::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    msg.pack(n_);
    msg.pack(active_.size());
    redundantAge_.pack(msg, n_);

    ABA_ID uninitializedId;
    for (int i = 0; i < n_; i++) {
      if (active_[i]->conVar() == 0)	// removed conVar
        uninitializedId.pack(msg);
      else {
        ABA_POOLSLOT<BaseType, CoType> *ps = active_[i]->slot();
        if (!ps->getIdentification().isInitialized())
          ps->setNewIdentification();
        ps->getIdentification().pack(msg);
      }
    }
  }

@q m f packNeeded @>
@ The function |packNeeded()| packs the constraints/variables whose
  indexes are stored in an |ABA_BUFFER|.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the data is packed.}
  \docarg{|needed|}{The indexes of the constraints/variables which
                    should be packed.}
  \EndArg
@q m e a @>

@(active.inc@>=
  template<class BaseType, class CoType>
@q m b f @>
  void ABA_ACTIVE<BaseType, CoType>::packNeeded(ABA_MESSAGE &msg,
                                            const ABA_BUFFER<int> &needed) const
@q m e f @>
  {
    for (int i = 0; i< needed.number(); i++) {
      BaseType *cv = active_[needed[i]]->conVar();
      if (debug(DEBUG_MESSAGE_CONVAR)) {
        master_->out() << "DEBUG_MESSAGE_CONVAR: sending Constraint/Variable "
                       << active_[needed[i]]->slot()->getIdentification()
                       << " (classId=" << cv->classId() << ")..." << endl;
      }
      msg.pack(cv->classId());
      cv->pack(msg);
    }
  }


@*1 ABA\_OPENSUB.


@ The function |terminate()| terminates the function |select()|.

@(opensub.cc@>=
  void ABA_OPENSUB::terminate()
  {
    terminate_ = true;
    while (!hasTerminated_)
      cond_.signal();
  }
 
@q m f getSubproblemWithBound @>
@q m e t @>
@ The function |getSubproblemWithBound()| extracts the best subproblem
  of the list of open subproblems if its  bound is
  at least as good as the given value.
@q m e t @>

@q m b a @> 
  \BeginArg
  \docarg{bound}{The requested dual bound}
  \EndArg
@q m e a @>

@q m b r @> 
  \BeginReturn
  A pointer to such a subproblem or 0.
  \EndReturn
@q m e r @>

@(opensub.cc@>=
@q m b f @>
  ABA_SUB* ABA_OPENSUB::getSubproblemWithBound(double bound)
@q m e f @>
{
  mutex_.acquire();

  ABA_DLISTITEM<ABA_SUB*> *item;
  ABA_SUB *s;

  forAllDListElem(list_, item, s) {
    if (s->status() == ABA_SUB::Dormant) {
      s->newDormantRound();
      if (s->nDormantRounds() < master_->minDormantRounds())
        continue;
    }

    // is the bound of s good enough?
    if (master_->optSense()->max()) {
      if (s->dualBound() >= bound)
        break;
    }
    else {
      if (s->dualBound() <= bound)
        break;
    }
  }

  if (item==0) {
    mutex_.release();
    return 0;
  }

  // subproblem found
  assert(s == item->elem());
  list_.remove(item);

  updateDualBound();
  mutex_.release();

  master_->parmaster()->newOpenSubCount(n_, dualBound_);
  return s;
}



@ The function |select()| selects a subproblem according to the
  strategy in |master| and removes it from the list
  of open subproblems. 

  The function |select()| scans the list of open subproblems, and selects
  the subproblem with highest priority from the set of open subproblems.
  Dormant subproblems are ignored if possible.

  \BeginReturn
  The selected subproblem.
  If the set of open subproblems is empty, 0 is returned.
  \EndReturn

@(opensub.cc@>=
  ABA_SUB* ABA_OPENSUB::select()
  {
    mutex_.acquire();

    while (!terminate_) {

      // perform load balancing if needed
      mutex_.release();
      for (int i=0; i<3; i++)
        if (master_->parmaster()->balance())
          break;
      mutex_.acquire();

      // select the next subproblem to optimize
      ABA_DLISTITEM<ABA_SUB*> *minItem = list_.first();
      ABA_DLISTITEM<ABA_SUB*> *item;
      ABA_SUB *s;

      forAllDListElem(list_, item, s) {
        if (s->status() == ABA_SUB::Dormant) {
          s->newDormantRound();
          if (s->nDormantRounds() < master_->minDormantRounds())
            continue;
        }
        if (master_->enumerationStrategy(s, minItem->elem()) > 0)
          minItem = item;
      }

      if (minItem != 0) {                 // subproblem found
        ABA_SUB* min = minItem->elem();
        list_.remove(minItem);

       updateDualBound();

        mutex_.release();

        master_->parmaster()->newOpenSubCount(n_, dualBound_);
        return min;
      }

      // host zero should poll for termination
      if (master_->parmaster()->isHostZero()) {
        ABA_NOTIFICATION msg(master_, 0);
        msg.pack(ABA_NOTIFYSERVER::TriggerTerminationCheckTag);
        msg.send();
        ACE_Time_Value abstime(0,400000);
        abstime += ACE_OS::gettimeofday();
        master_->parmaster()->startIdleTime();
        cond_.wait(&abstime);                // wait at most 0.4 s
        master_->parmaster()->stopIdleTime();
      }
      else {               // the other hosts should wait until signaled
        master_->parmaster()->startIdleTime();
        cond_.wait();
        master_->parmaster()->stopIdleTime();
      }
    }

    mutex_.release();

    hasTerminated_ = true;     // terminate |ABA_OPENSUB::terminate()|

    return 0;
  }


@*1 ABA\_SPARVEC.


@q m f Constructor @>
@q m e t @>
@ The message constructor creates the sparse vector from an |ABA_MESSAGE|.

@q m b a @>
    \BeginArg
    \docarg
      {|glob|}{A pointer to the corresponding global object.}
    \docarg
      {|msg|}{The message from which the object is initialized.}
    \EndArg
@q m e a @>

@(sparvec.cc@>=
@q m b f @>
  ABA_SPARVEC::ABA_SPARVEC(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
@q m e f @>
    : @/
    glob_(glob)
  {
    msg.unpack(size_);
    msg.unpack(nnz_);
    msg.unpack(reallocFac_);

    if (size_) {
      support_ = new int[size_];
      coeff_   = new double[size_];

      msg.unpack(support_, nnz_);
      msg.unpack(coeff_, nnz_);
    }
    else {
      support_ = 0;
      coeff_   = 0;
    }
  }


@q m f pack @>
@ The function |pack()| packs the data of the sparse vector
  in an |ABA_MESSAGE| object.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the sparse vector
                 is packed.}
  \EndArg
@q m e a @>

@(sparvec.cc@>=
@q m b f @>
  void ABA_SPARVEC::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    msg.pack(size_);
    msg.pack(nnz_);
    msg.pack(reallocFac_);

    if (size_) {
      msg.pack(support_, nnz_);
      msg.pack(coeff_, nnz_);
    }
  }


@*1 ABA\_STRING.

@q m f Constructor @>
@ The message constructor creates the |ABA_STRING| from an |ABA_MESSAGE|.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object from which the string is initialized.}
  \EndArg
@q m e a @>

@(string.cc@>=
@q m b f @>
  ABA_STRING::ABA_STRING(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
@q m e f @>
  : @/
    glob_(glob), @/
    string_(0)
  {
    unpack(msg);
  }

@q m f unpack @>
@ The function |unpack()| unpacks the string from an |ABA_MESSAGE| object.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object from which the string is unpacked.}
  \EndArg
@q m e a @>

@(string.cc@>=
@q m b f @>
  void ABA_STRING::unpack(ABA_MESSAGE &msg)
@q m e f @>
  {
    int size;
    msg.unpack(size);
    delete [] string_;
    string_ = new char[size + 1];
    msg.unpack(string_, size);
    string_[size] = '\0';
  }

@q m f pack @>
@ The function |pack()| packs the string in an |ABA_MESSAGE| object.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the string is packed.}
  \EndArg
@q m e a @>

@(string.cc@>=
@q m b f @>
  void ABA_STRING::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    int size = ::strlen(string_);
    msg.pack(size);
    msg.pack(string_, size);
  }


@*1 ABA\_ARRAY.

@q m f Constructor @>
@q m e t @>

@ The message constructor creates the |ABA_ARRAY| from an |ABA_MESSAGE|.

@q m b a @>
    \BeginArg
    \docarg{|glob|}{A pointer to the corresponding global object.}
    \docarg{|msg|}{The message from which the object is initialized.}
    \EndArg
@q m e a @>

@(array.inc@>=
  template <class Type>
@q m b f @>
  ABA_ARRAY<Type>::ABA_ARRAY(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
@q m e f @>
    : @/
    glob_(glob), @/
    n_(0), @/
    a_(0)
  {
    glob_->err() << "ABA_ARRAY::ABA_ARRAY() : An ABA_ARRAY of some type coudn't"
                    " be received. You have to implement a template"
                    " specialization of the message constructor for"
                    " that type!" << endl;
    exit(Fatal);
  }

@q m f pack @>
@ The function |pack()| packs all elements of the array in an |ABA_MESSAGE| object.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the array is packed.}
  \EndArg
@q m e a @>

@(array.inc@>=
  template<class Type>
@q m b f @>
  void ABA_ARRAY<Type>::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    pack(msg, n_);
  }

@q m f pack @>
@ This version of the function |pack()| packs a limited number of elements of the
  array in an |ABA_MESSAGE| object, only.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the array is packed.}
  \docarg{|nPacked|}{The number of elements to be packed.}
  \EndArg
@q m e a @>

@(array.inc@>=
  template<class Type>
@q m b f @>
  void ABA_ARRAY<Type>::pack(ABA_MESSAGE &msg, int nPacked) const
@q m e f @>
  {
    glob_->err() << "ABA_ARRAY::pack() : An ABA_ARRAY of some type coudn't"
                    " be sent. You first have to implement a template"
                    " specialization of the pack() template function"
                    " for that type!" << endl;
    exit(Fatal);
  }


@*1 ABA\_BUFFER.

@q m f Constructor @>
@q m e t @>

@ The message constructor creates the |ABA_BUFFER| from an |ABA_MESSAGE|.

@q m b a @>
    \BeginArg
    \docarg{|glob|}{A pointer to the corresponding global object.}
    \docarg{|msg|}{The message from which the object is initialized.}
    \EndArg
@q m e a @>

@(buffer.inc@>=
  template <class Type>
@q m b f @>
  ABA_BUFFER<Type>::ABA_BUFFER(const ABA_GLOBAL *glob, ABA_MESSAGE &msg)
@q m e f @>
  : @/
    glob_(glob)
  {
    glob_->err() << "ABA_BUFFER::ABA_BUFFER() : A ABA_BUFFER of some type coudn't"
                    " be received. You have to implement a template"
                    " specialization of the message constructor for"
                    "  that type!" << endl;
    exit(Fatal);
  }

@q m f pack @>
@ The function |pack()| packs the data of the buffer in an |ABA_MESSAGE| object.
@q m e t @>

@q m b a @>
  \BeginArg
  \docarg{|msg|}{The |ABA_MESSAGE| object in which the buffer is packed.}
  \EndArg
@q m e a @>

@(buffer.inc@>=
  template<class Type>
@q m b f @>
  void ABA_BUFFER<Type>::pack(ABA_MESSAGE &msg) const
@q m e f @>
  {
    glob_->err() << "ABA_BUFFER::pack() : A ABA_BUFFER of some type coudn't"
                    " be sent. You first have to implement a template"
                    " specialization of the pack() template function"
                    " for that type!" << endl;
    exit(Fatal);
  }


@*1 ABA\_HASH.

@ This is a hash function for elements of the class |ABA_ID|.
  A different random number associated with each processor is used to shuffle
  identical sequence numbers to different locations in the hash table.

@(hash.inc@>=
  template <class KeyType, class ItemType>
  int ABA_HASH<KeyType, ItemType>::hf(const ABA_ID &id)
  {
    const int rand[64] = {
      0x08fa735c, 0x465969d0, 0x66a657f4, 0x144d2cf9,
      0x32b20675, 0x7d86036c, 0x3bcd2f61, 0x30421197,
      0x272d9013, 0x1d3bf099, 0x1bd38ed1, 0x57abc10e,
      0x7e62fbf6, 0x0b9bf7ad, 0x15bd99d9, 0x451a0198,
      0x73b3a879, 0x325eeb8a, 0x1dbb0b7c, 0x5bec0be6,
      0x2e78432e, 0x2e2ceea6, 0x55177a1a, 0x7b31a98f,
      0x54d04dd5, 0x547bd0d0, 0x1d12c33a, 0x16fb478f,
      0x687e3120, 0x4a047b2e, 0x649e29fb, 0x1c36b5ae,
      0x3a9e8db8, 0x6488c827, 0x5b6315fa, 0x60b4e7c1,
      0x5c116177, 0x336ead28, 0x7dcdd34c, 0x41b4bb6e,
      0x3f7aeaa3, 0x687cf590, 0x19469807, 0x56a508f0,
      0x179ed4c4, 0x06e73a00, 0x007da2a3, 0x41e5ac24,
      0x0585b479, 0x5b1cf529, 0x285b5b9a, 0x3bbaea37,
      0x7c84f882, 0x081c97ba, 0x6df23bc6, 0x1f655ecb,
      0x291ac2ac, 0x7598ef40, 0x5b8235b8, 0x25ccaa59,
      0x65a52132, 0x2cf89028, 0x1d05cf45, 0x32e86c2b
    };

    return (rand[id.proc() & 63] ^ id.sequence()) % size_;
  }


