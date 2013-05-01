
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "game/gameConnection.h"
#include "math/mathIO.h"

#include "afx/afxChoreographer.h"
#include "afx/afxEffectron.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEffectronData::ewValidator
//
// When an effect is added using "addEffect", this validator intercepts the value
// and adds it to the dynamic effects list. 
//
void afxEffectronData::ewValidator::validateType(SimObject* object, void* typePtr)
{
  afxEffectronData* eff_data = dynamic_cast<afxEffectronData*>(object);
  afxEffectWrapperData** ew = (afxEffectWrapperData**)(typePtr);

  if (eff_data && ew)
  {
    eff_data->fx_list.push_back(*ew);
    *ew = 0;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class EffectronFinishStartupEvent : public SimEvent
{
public:
  void process(SimObject* obj) 
  { 
     afxEffectron* eff = dynamic_cast<afxEffectron*>(obj);
     if (eff)
       eff->finish_startup();
  }
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEffectronData

IMPLEMENT_CONSOLETYPE(afxEffectronData)
IMPLEMENT_GETDATATYPE(afxEffectronData)
IMPLEMENT_SETDATATYPE(afxEffectronData)
IMPLEMENT_CO_DATABLOCK_V1(afxEffectronData);

afxEffectronData::afxEffectronData()
{
  duration = 0.0f;
  n_loops = 1;

  // dummy entry holds effect-wrapper pointer while a special validator
  // grabs it and adds it to an appropriate effects list
  dummy_fx_entry = NULL;

  // marked true if datablock ids need to
  // be converted into pointers
  do_id_convert = false;
}

void afxEffectronData::reset()
{
  fx_list.clear();
}

#define myOffset(field) Offset(field, afxEffectronData)

void afxEffectronData::initPersistFields()
{
  Parent::initPersistFields();

#ifdef AFX_TGE_1_3
  Con::registerType("AfxEffectronData", TypeafxEffectronData, sizeof(afxEffectronData*),
                    REF_GETDATATYPE(afxEffectronData), REF_SETDATATYPE(afxEffectronData));
#endif

  addField("duration",    TypeF32,      myOffset(duration));
  addField("numLoops",    TypeS32,      myOffset(n_loops));

  // effect lists
  // for each of these, dummy_fx_entry is set and then a validator adds it to the appropriate effects list 
  addFieldV("addEffect",  TypeafxEffectWrapperDataPtr,  myOffset(dummy_fx_entry),  new ewValidator(0));


  //addProtectedField("effectList",  TypeafxEffectWrapperDataPtr,  myOffset(dummy_fx_entry),  
  //                  set_entry, get_entry, );
}

bool afxEffectronData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;
    
  return true;
}

void afxEffectronData::pack_fx(BitStream* stream, const afxEffectList& fx, bool packed)
{
  stream->writeInt(fx.size(), EFFECTS_PER_PHRASE_BITS);
  for (int i = 0; i < fx.size(); i++)
    writeDatablockID(stream, fx[i], packed);
}

void afxEffectronData::unpack_fx(BitStream* stream, afxEffectList& fx)
{
  fx.clear();
  S32 n_fx = stream->readInt(EFFECTS_PER_PHRASE_BITS);
  for (int i = 0; i < n_fx; i++)
    fx.push_back((afxEffectWrapperData*)readDatablockID(stream));
}

void afxEffectronData::packData(BitStream* stream)
{
	Parent::packData(stream);

  stream->write(duration);
  stream->write(n_loops);

  pack_fx(stream, fx_list, packed);
}

void afxEffectronData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  stream->read(&duration);
  stream->read(&n_loops);

  do_id_convert = true;
  unpack_fx(stream, fx_list);
}

bool afxEffectronData::preload(bool server, char errorBuffer[256])
{
  if (!Parent::preload(server, errorBuffer))
    return false;

  // Resolve objects transmitted from server
  if (!server) 
  {
    if (do_id_convert)
    {
      for (S32 i = 0; i < fx_list.size(); i++)
      {
        SimObjectId db_id = (SimObjectId) fx_list[i];
        if (db_id != 0)
        {
          // try to convert id to pointer
          if (!Sim::findObject(db_id, fx_list[i]))
          {
            Con::errorf(ConsoleLogEntry::General, 
              "afxEffectronData::preload() -- bad datablockId: 0x%x", 
              db_id);
          }
        }
      }
      do_id_convert = false;
    }
  }

  return true;
}

void afxEffectronData::gather_cons_defs(Vector<afxConstraintDef>& defs, afxEffectList& fx)
{
  for (int i = 0; i <  fx.size(); i++)
  {
    afxEffectWrapperData* ewd = fx[i];
    if (ewd)
    {
      if (ewd->pos_cons_def.isDefined())
        defs.push_back(ewd->pos_cons_def);
      if (ewd->orient_cons_def.isDefined())
        defs.push_back(ewd->orient_cons_def);
      if (ewd->aim_cons_def.isDefined())
        defs.push_back(ewd->aim_cons_def);
      if (ewd->life_cons_def.isDefined())
        defs.push_back(ewd->life_cons_def);
    }
  }
}

void afxEffectronData::gatherConstraintDefs(Vector<afxConstraintDef>& defs)
{
  gather_cons_defs(defs, fx_list);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

ConsoleMethod(afxEffectronData, reset, void, 2, 2, "reset()") 
{
  object->reset();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEffectron

IMPLEMENT_CO_NETOBJECT_V1(afxEffectron);

StringTableEntry  afxEffectron::CAMERA_CONS;
StringTableEntry  afxEffectron::LISTENER_CONS;

void afxEffectron::init()
{
  // setup static predefined constraint names
  if (CAMERA_CONS == 0)
  {
    CAMERA_CONS = StringTable->insert("camera");
    LISTENER_CONS = StringTable->insert("listener");
  }

  // afxEffectron is always in scope, however the effects used 
  // do their own scoping in that they will shut off if their 
  // position constraint leaves scope.
	mNetFlags.set(Ghostable | ScopeAlways);

	datablock = NULL;

  constraints_initialized = false;

  effect_state = (U8) INACTIVE_STATE;
  effect_elapsed = 0;

  // define named constraints
  constraint_mgr->defineConstraint(OBJECT_CONSTRAINT, CAMERA_CONS);
  constraint_mgr->defineConstraint(POINT_CONSTRAINT,  LISTENER_CONS);

  active_phrase = NULL;
  time_factor = 1.0f;

  marks_mask = 0;
}

afxEffectron::afxEffectron()
{
  init();
}

afxEffectron::~afxEffectron()
{
  delete active_phrase;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// STANDARD OVERLOADED METHODS //

bool afxEffectron::onNewDataBlock(GameBaseData* dptr)
{
  datablock = dynamic_cast<afxEffectronData*>(dptr);
  if (!datablock || !Parent::onNewDataBlock(dptr))
    return false;

  return true;
}

void afxEffectron::processTick(const Move* m)
{
	Parent::processTick(m);

  // don't process moves or client ticks
  if (m != 0 || isClientObject())
    return;

  process_server();
}

void afxEffectron::advanceTime(F32 dt)
{
  Parent::advanceTime(dt);

  process_client(dt);
}

bool afxEffectron::onAdd()
{
	if (!Parent::onAdd()) 
    return(false);

  /*
  if (isServerObject())
    init_constraints();
  */

	return(true);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

U32 afxEffectron::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
{
  U32 retMask = Parent::packUpdate(conn, mask, stream);
  
  // InitialUpdate
  if (stream->writeFlag(mask & InitialUpdateMask))
  {
    stream->write(time_factor);

    GameConnection* gconn = dynamic_cast<GameConnection*>(conn);
    bool zoned_in = (gconn) ? gconn->isZonedIn() : false;
    if (stream->writeFlag(zoned_in))
      pack_constraint_info(conn, stream);
  }

  // StateEvent or SyncEvent
  if (stream->writeFlag((mask & StateEventMask) || (mask & SyncEventMask)))
  {
    stream->write(marks_mask);
    stream->write(effect_state);
    stream->write(effect_elapsed);
  }

  // SyncEvent
  bool do_sync_event = ((mask & SyncEventMask) && !(mask & InitialUpdateMask));
  if (stream->writeFlag(do_sync_event))
  {
    pack_constraint_info(conn, stream);
  }
  
  return retMask;
}

//~~~~~~~~~~~~~~~~~~~~//

void afxEffectron::unpackUpdate(NetConnection * conn, BitStream * stream)
{
  Parent::unpackUpdate(conn, stream);
  
  bool initial_update = false;
  bool zoned_in = true;
  bool do_sync_event = false;
  U8 new_marks_mask = 0;
  U8 new_state = INACTIVE_STATE;
  F32 new_elapsed = 0;

  // InitialUpdate Only
  if (stream->readFlag())
  {    
    initial_update = true;

    stream->read(&time_factor);

    // if client is marked as fully zoned in
    if (zoned_in = stream->readFlag())
    {
      unpack_constraint_info(conn, stream);
      init_constraints();
    }
  }

  // StateEvent or SyncEvent
  // this state data is sent for both state-events and
  // sync-events
  if (stream->readFlag())
  {
    stream->read(&new_marks_mask);
    stream->read(&new_state);
    stream->read(&new_elapsed);

    marks_mask = new_marks_mask;
  }

  // SyncEvent
  do_sync_event = stream->readFlag();
  if (do_sync_event)
  {
    unpack_constraint_info(conn, stream);
    init_constraints();
  }

  //~~~~~~~~~~~~~~~~~~~~//

  if (!zoned_in)
    effect_state = LATE_STATE;

  // need to adjust state info to get all synced up with spell on server
  if (do_sync_event && !initial_update)
    sync_client(new_marks_mask, new_state, new_elapsed);
}

void afxEffectron::sync_with_clients()
{
  setMaskBits(SyncEventMask);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// private

bool afxEffectron::state_expired()
{
  afxPhrase* phrase = (effect_state == ACTIVE_STATE) ? active_phrase : NULL;

  if (phrase)
  {
    if (phrase->expired(effect_elapsed))
      return (!phrase->recycle(effect_elapsed));
    return false;
  }

  return true;
}

void afxEffectron::init_constraints()
{
  if (constraints_initialized)
  {
    //Con::printf("CONSTRAINTS ALREADY INITIALIZED");
    return;
  }

  Vector<afxConstraintDef> defs;
  datablock->gatherConstraintDefs(defs);

  constraint_mgr->initConstraintDefs(defs, isServerObject());

  if (isClientObject())
  {
    // find local camera
    GameConnection* conn = GameConnection::getConnectionToServer();
    if (conn)
    {  
      ShapeBase* camera = conn->getCameraObject();
      if (camera)
        constraint_mgr->setReferenceObject(CAMERA_CONS, camera);
    }
    // find local listener
    Point3F listener_pos;
    alxGetListenerPoint3F(AL_POSITION, &listener_pos);
    listener_cons_id = constraint_mgr->setReferencePoint(LISTENER_CONS, listener_pos);
  }

  constraints_initialized = true;
}

void afxEffectron::setup_active_fx()
{
  active_phrase = new afxPhrase(isServerObject(), true);

  if (active_phrase)
  {
    active_phrase->init(datablock->fx_list, datablock->duration, this, time_factor, 
                        datablock->n_loops);
  }
}

bool afxEffectron::cleanup_over()
{
  if (active_phrase && !active_phrase->isEmpty())
    return false;

  return true;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// private

void afxEffectron::process_server()
{
  effect_elapsed += TickSec;

  U8 pending_state = effect_state;

  // check for state changes
  switch (effect_state)
  {
  case INACTIVE_STATE:
    if (marks_mask & MARK_ACTIVATE)
      pending_state = ACTIVE_STATE;
    break;
  case ACTIVE_STATE:
    if (marks_mask & MARK_INTERRUPT)
      pending_state = CLEANUP_STATE;
    else if (marks_mask & MARK_SHUTDOWN)
      pending_state = CLEANUP_STATE;
    else if (state_expired())
      pending_state = CLEANUP_STATE;
    break;
  case CLEANUP_STATE:
    if (cleanup_over())
      pending_state = DONE_STATE;
    break;
  }

  if (effect_state != pending_state)
    change_state_s(pending_state);

  if (effect_state == INACTIVE_STATE)
    return;

  //--------------------------//

  // sample the constraints
  constraint_mgr->sample(TickSec, Platform::getVirtualMilliseconds());

  if (active_phrase)
    active_phrase->update(TickSec, effect_elapsed);
}

void afxEffectron::change_state_s(U8 pending_state)
{ 
  if (effect_state == pending_state)
    return;

  switch (effect_state)
  {
  case INACTIVE_STATE:
    break;
  case ACTIVE_STATE:
    leave_active_state_s();
    break;
  case CLEANUP_STATE:
    break;
  case DONE_STATE:
    break;
  }

  effect_state = pending_state;

  switch (pending_state)
  {
  case INACTIVE_STATE:
    break;
  case ACTIVE_STATE:
    enter_active_state_s();
    break;
  case CLEANUP_STATE:
    enter_cleanup_state_s();
    break;
  case DONE_STATE:
    enter_done_state_s();
    break;
  }
}

void afxEffectron::enter_done_state_s()
{ 
  postEvent(DEACTIVATE_EVENT);

  F32 done_time = effect_elapsed;

  if (active_phrase)
  {
    F32 phrase_done;
    if (active_phrase->isInfinite())
      phrase_done = effect_elapsed + active_phrase->calcAfterLife();
    else
      phrase_done = active_phrase->calcDoneTime();
    if (phrase_done > done_time)
      done_time = phrase_done;
  }

  F32 time_left = done_time - effect_elapsed;
  if (time_left < 0)
    time_left = 0;

  Sim::postEvent(this, new ObjectDeleteEvent, Sim::getCurrentTime() + time_left*1000 + 500);

  // CALL SCRIPT afxEffectronData::onDeactivate(%eff)
  Con::executef(datablock, 2, "onDeactivate", scriptThis());
}

void afxEffectron::enter_active_state_s()
{ 
  // stamp constraint-mgr starting time
  constraint_mgr->setStartTime(Platform::getVirtualMilliseconds());
  effect_elapsed = 0;

  setup_dynamic_constraints();

  // start casting effects
  setup_active_fx();
  if (active_phrase)
    active_phrase->start(effect_elapsed, effect_elapsed);
}

void afxEffectron::leave_active_state_s()
{ 
  if (active_phrase)
    active_phrase->stop(effect_elapsed);
}

void afxEffectron::enter_cleanup_state_s()
{ 
  postEvent(SHUTDOWN_EVENT);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// private

void afxEffectron::process_client(F32 dt)
{
  effect_elapsed += dt;

  U8 pending_state = effect_state;

  // check for state changes
  switch (effect_state)
  {
  case INACTIVE_STATE:
    if (marks_mask & MARK_ACTIVATE)
      pending_state = ACTIVE_STATE;
    break;
  case ACTIVE_STATE:
    if (marks_mask & MARK_INTERRUPT)
      pending_state = CLEANUP_STATE;
    else if (marks_mask & MARK_SHUTDOWN)
      pending_state = CLEANUP_STATE;
    else if (state_expired())
      pending_state = CLEANUP_STATE;
    break;
  case CLEANUP_STATE:
    if (cleanup_over())
      pending_state = DONE_STATE;
    break;
  }

  if (effect_state != pending_state)
    change_state_c(pending_state);

  if (effect_state == INACTIVE_STATE)
    return;

  //--------------------------//

  // update the listener constraint position
  if (!listener_cons_id.undefined())
  {
    Point3F listener_pos;
    alxGetListenerPoint3F(AL_POSITION, &listener_pos);
    constraint_mgr->setReferencePoint(listener_cons_id, listener_pos);
  }

  // sample the constraints
  constraint_mgr->sample(dt, Platform::getVirtualMilliseconds());

  // update active effects lists
  if (active_phrase)
    active_phrase->update(dt, effect_elapsed);
}

void afxEffectron::change_state_c(U8 pending_state)
{ 
  if (effect_state == pending_state)
    return;

  switch (effect_state)
  {
  case INACTIVE_STATE:
    break;
  case ACTIVE_STATE:
    leave_active_state_c();
    break;
  case CLEANUP_STATE:
    break;
  case DONE_STATE:
    break;
  }

  effect_state = pending_state;

  switch (pending_state)
  {
  case INACTIVE_STATE:
    break;
  case ACTIVE_STATE:
    enter_active_state_c(effect_elapsed);
    break;
  case CLEANUP_STATE:
    break;
  case DONE_STATE:
    break;
  }
}

void afxEffectron::enter_active_state_c(F32 starttime)
{ 
  // stamp constraint-mgr starting time
  constraint_mgr->setStartTime(Platform::getVirtualMilliseconds() - (U32)(effect_elapsed*1000));
  ///effect_elapsed = 0;

  setup_dynamic_constraints();

  setup_active_fx();
  if (active_phrase)
    active_phrase->start(starttime, effect_elapsed);
}

void afxEffectron::leave_active_state_c()
{ 
  if (active_phrase)
    active_phrase->stop(effect_elapsed);
}

void afxEffectron::sync_client(U16 marks, U8 state, F32 elapsed)
{
  //Con::printf("SYNC marks=%d old_state=%d state=%d elapsed=%g", 
  //            marks, effect_state, state, elapsed);

  if (effect_state != LATE_STATE)
    return;

  marks_mask = marks;

  // don't want to be started on late zoning clients
  if (!datablock->exec_on_new_clients)
  {
    effect_state = DONE_STATE;
  }

  // it looks like we're ghosting pretty late and
  // should just return to the inactive state.
  else if (marks & (MARK_INTERRUPT | MARK_DEACTIVATE | MARK_SHUTDOWN))
  {
    effect_state = DONE_STATE;
  }

  // it looks like we should be in the active state.
  else if (marks & MARK_ACTIVATE)
  {
    effect_state = ACTIVE_STATE;
    effect_elapsed = elapsed;
    enter_active_state_c(0.0);
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// public:

void afxEffectron::postEvent(U8 event) 
{ 
  setMaskBits(StateEventMask);
  
  switch (event)
  {
  case ACTIVATE_EVENT:
    marks_mask |= MARK_ACTIVATE;
    break;
  case SHUTDOWN_EVENT:
    marks_mask |= MARK_SHUTDOWN;
    break;
  case DEACTIVATE_EVENT:
    marks_mask |= MARK_DEACTIVATE;
    break;
  case INTERRUPT_EVENT:
    marks_mask |= MARK_INTERRUPT;
    break;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

void afxEffectron::finish_startup()
{
  // CALL SCRIPT afxEffectronData::onActivate(%eff)
  Con::executef(datablock, 2, "onActivate", scriptThis());

  init_constraints();

  postEvent(afxEffectron::ACTIVATE_EVENT);
}

// static
afxEffectron* 
afxEffectron::start_effect(afxEffectronData* datablock) 
{
  AssertFatal(datablock != NULL, "Datablock is missing.");

  // create a new effectron instance
  afxEffectron* eff = new afxEffectron();
  eff->setDataBlock(datablock);

  // register
  if (!eff->registerObject())
  {
    Con::errorf("afxEffectron: failed to register effect instance.");
    Sim::postEvent(eff, new ObjectDeleteEvent, Sim::getCurrentTime());
    return 0;
  }
  registerForCleanup(eff);

  // separating the final part of startup allows the calling script
  // to make certain types of calls on the returned effectron that  
  // need to happen prior to constraint initialization.
  Sim::postEvent(eff, new EffectronFinishStartupEvent, Sim::getCurrentTime());

  return eff;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// console functions

ConsoleMethod(afxEffectron, setTimeFactor, void, 3, 3, "(F32 factor)") 
{
  object->setTimeFactor(dAtof(argv[2]));
}

ConsoleMethod(afxEffectron, interrupt, void, 2, 2, "interrupt()") 
{
  object->postEvent(afxEffectron::INTERRUPT_EVENT);
}

ConsoleFunction(startEffectron, S32, 2, 5, "startEffectron(datablock, [constraint_object], [constraint_name], [extra])")
{
  afxEffectronData* datablock = dynamic_cast<afxEffectronData*>(Sim::findObject(argv[1]));
  if (!datablock)
  {
    Con::errorf("startEffectron() -- failed to find datablock [%s].", argv[1]);
    return 0;
  }

  afxEffectron* eff = afxEffectron::start_effect(datablock);
  if (!eff)
    return 0;

  if (argc > 3)
  {
    if (!eff->addConstraint(argv[2], argv[3]))
      Con::errorf("startEffectron() -- failed to find constraint object [%s].", argv[2]);
  }

  if (argc == 3 || argc == 5)
  {
    SimObject* extra = Sim::findObject(argv[argc-1]);
    if (extra)
      eff->setExtra(extra);
    else
      Con::errorf("startEffectron() -- failed to find extra object [%s].", argv[argc-1]);
  }

  return eff->getId();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//









