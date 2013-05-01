
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "game/gameConnection.h"
#include "math/mathIO.h"

#include "afx/afxChoreographer.h"
#include "afx/afxSelectron.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxSelectronData::ewValidator
//
// When an effect is added using "addEffect", this validator intercepts the value
// and adds it to the dynamic effects list. 
//
void afxSelectronData::ewValidator::validateType(SimObject* object, void* typePtr)
{
  afxSelectronData* sele_data = dynamic_cast<afxSelectronData*>(object);
  afxEffectWrapperData** ew = (afxEffectWrapperData**)(typePtr);

  if (sele_data && ew)
  {
    switch (id)
    {
    case MAIN_PHRASE:
      sele_data->main_fx_list.push_back(*ew);
      break;
    case SELECT_PHRASE:
      sele_data->select_fx_list.push_back(*ew);
      break;
    case DESELECT_PHRASE:
      sele_data->deselect_fx_list.push_back(*ew);
      break;
    }
    *ew = 0;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class SelectronFinishStartupEvent : public SimEvent
{
public:
  void process(SimObject* obj) 
  { 
     afxSelectron* selectron = dynamic_cast<afxSelectron*>(obj);
     if (selectron)
       selectron->finish_startup();
  }
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxSelectronData

IMPLEMENT_CONSOLETYPE(afxSelectronData)
IMPLEMENT_GETDATATYPE(afxSelectronData)
IMPLEMENT_SETDATATYPE(afxSelectronData)
IMPLEMENT_CO_DATABLOCK_V1(afxSelectronData);

afxSelectronData::afxSelectronData()
{
  main_dur = 0.0f;
  select_dur = 0.0f;
  deselect_dur = 0.0f;

  n_main_loops = 1;
  n_select_loops = 1;
  n_deselect_loops = 1;

  registered = false;

  obj_type_style = 0;
  obj_type_mask = 0;

  // dummy entry holds effect-wrapper pointer while a special validator
  // grabs it and adds it to an appropriate effects list
  dummy_fx_entry = NULL;

  // marked true if datablock ids need to
  // be converted into pointers
  do_id_convert = false;
}

afxSelectronData::~afxSelectronData()
{
  if (registered)
    arcaneFX::unregisterSelectronData(this);
}

void afxSelectronData::reset()
{
  main_fx_list.clear();
  select_fx_list.clear();
  deselect_fx_list.clear();
}

#define myOffset(field) Offset(field, afxSelectronData)

void afxSelectronData::initPersistFields()
{
  Parent::initPersistFields();

#ifdef AFX_TGE_1_3
  Con::registerType("AfxSelectronData", TypeafxSelectronData, sizeof(afxSelectronData*),
                    REF_GETDATATYPE(afxSelectronData), REF_SETDATATYPE(afxSelectronData));
#endif

  addField("mainDur",                   TypeF32,    myOffset(main_dur));
  addField("selectDur",                 TypeF32,    myOffset(select_dur));
  addField("deselectDur",               TypeF32,    myOffset(deselect_dur));
  addField("mainRepeats",               TypeS32,    myOffset(n_main_loops));
  addField("selectRepeats",             TypeS32,    myOffset(n_select_loops));
  addField("deselectRepeats",           TypeS32,    myOffset(n_deselect_loops));
  addField("selectionTypeMask",         TypeS32,    myOffset(obj_type_mask));
  addField("selectionTypeStyle",        TypeS8,     myOffset(obj_type_style));

  // effect lists
  // for each of these, dummy_fx_entry is set and then a validator adds it to the appropriate effects list 
  addFieldV("addMainEffect",      TypeafxEffectWrapperDataPtr,  myOffset(dummy_fx_entry),  new ewValidator(MAIN_PHRASE));
  addFieldV("addSelectEffect",    TypeafxEffectWrapperDataPtr,  myOffset(dummy_fx_entry),  new ewValidator(SELECT_PHRASE));
  addFieldV("addDeselectEffect",  TypeafxEffectWrapperDataPtr,  myOffset(dummy_fx_entry),  new ewValidator(DESELECT_PHRASE));

  // deprecated
  addField("numMainLoops",      TypeS32,      myOffset(n_main_loops));
  addField("numSelectLoops",    TypeS32,      myOffset(n_select_loops));
  addField("numDeselectLoops",  TypeS32,      myOffset(n_deselect_loops));
}

bool afxSelectronData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;
    
  return true;
}

void afxSelectronData::pack_fx(BitStream* stream, const afxEffectList& fx, bool packed)
{
  stream->writeInt(fx.size(), EFFECTS_PER_PHRASE_BITS);
  for (int i = 0; i < fx.size(); i++)
    writeDatablockID(stream, fx[i], packed);
}

void afxSelectronData::unpack_fx(BitStream* stream, afxEffectList& fx)
{
  fx.clear();
  S32 n_fx = stream->readInt(EFFECTS_PER_PHRASE_BITS);
  for (int i = 0; i < n_fx; i++)
    fx.push_back((afxEffectWrapperData*)readDatablockID(stream));
}

void afxSelectronData::packData(BitStream* stream)
{
	Parent::packData(stream);

  stream->write(main_dur);
  stream->write(select_dur);
  stream->write(deselect_dur);
  stream->write(n_main_loops);
  stream->write(n_select_loops);
  stream->write(n_deselect_loops);
  stream->write(obj_type_style);
  stream->write(obj_type_mask);

  pack_fx(stream, main_fx_list, packed);
  pack_fx(stream, select_fx_list, packed);
  pack_fx(stream, deselect_fx_list, packed);
}

void afxSelectronData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  stream->read(&main_dur);
  stream->read(&select_dur);
  stream->read(&deselect_dur);
  stream->read(&n_main_loops);
  stream->read(&n_select_loops);
  stream->read(&n_deselect_loops);
  stream->read(&obj_type_style);
  stream->read(&obj_type_mask);

  do_id_convert = true;
  unpack_fx(stream, main_fx_list);
  unpack_fx(stream, select_fx_list);
  unpack_fx(stream, deselect_fx_list);
}

inline void expand_fx_list(afxEffectList& fx_list, const char* tag)
{
  for (S32 i = 0; i < fx_list.size(); i++)
  {
    SimObjectId db_id = (SimObjectId)fx_list[i];
    if (db_id != 0)
    {
      // try to convert id to pointer
      if (!Sim::findObject(db_id, fx_list[i]))
      {
        Con::errorf(ConsoleLogEntry::General, 
          "afxSelectronData::preload() -- bad datablockId: 0x%x (%s)", 
          db_id, tag);
      }
    }
  }
}

bool afxSelectronData::preload(bool server, char errorBuffer[256])
{
  if (!Parent::preload(server, errorBuffer))
    return false;

  // Resolve objects transmitted from server
  if (!server) 
  {
    if (do_id_convert)
    {
      expand_fx_list(main_fx_list, "main");
      expand_fx_list(select_fx_list, "select");
      expand_fx_list(deselect_fx_list, "deselect");
      do_id_convert = false;
    }

    // this where a selectron registers itself with the rest of AFX
    if (!registered)
    {
      arcaneFX::registerSelectronData(this);
      registered = true;
    }
  }

  return true;
}

void afxSelectronData::gather_cons_defs(Vector<afxConstraintDef>& defs, afxEffectList& fx)
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

void afxSelectronData::gatherConstraintDefs(Vector<afxConstraintDef>& defs)
{
  gather_cons_defs(defs, main_fx_list);
  gather_cons_defs(defs, select_fx_list);
  gather_cons_defs(defs, deselect_fx_list);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

ConsoleMethod(afxSelectronData, reset, void, 2, 2, "reset()") 
{
  object->reset();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxSelectron

IMPLEMENT_CO_NETOBJECT_V1(afxSelectron);

StringTableEntry  afxSelectron::CAMERA_CONS;
StringTableEntry  afxSelectron::LISTENER_CONS;

void afxSelectron::init()
{
  client_only = true;
  mNetFlags.clear(Ghostable | ScopeAlways);
  mNetFlags.set(IsGhost);

  // setup static predefined constraint names
  if (CAMERA_CONS == 0)
  {
    CAMERA_CONS = StringTable->insert("camera");
    LISTENER_CONS = StringTable->insert("listener");
  }

	datablock = NULL;

  constraints_initialized = false;

  effect_state = (U8) INACTIVE_STATE;
  effect_elapsed = 0;

  // define named constraints
  constraint_mgr->defineConstraint(OBJECT_CONSTRAINT, CAMERA_CONS);
  constraint_mgr->defineConstraint(POINT_CONSTRAINT,  LISTENER_CONS);

  for (S32 i = 0; i < NUM_PHRASES; i++)
  {
    phrases[i] = NULL;
    //tfactors[i] = 1.0f;
  }

  //active_phrase = NULL;
  time_factor = 1.0f;

  marks_mask = 0;
}

afxSelectron::afxSelectron()
{
  init();
}

afxSelectron::~afxSelectron()
{
  for (S32 i = 0; i < NUM_PHRASES; i++)
  {
    if (phrases[i])
    {
      phrases[i]->interrupt(effect_elapsed);
      delete phrases[i];
    }
  }

  //delete active_phrase;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// STANDARD OVERLOADED METHODS //

bool afxSelectron::onNewDataBlock(GameBaseData* dptr)
{
  datablock = dynamic_cast<afxSelectronData*>(dptr);
  if (!datablock || !Parent::onNewDataBlock(dptr))
    return false;

  return true;
}

void afxSelectron::processTick(const Move* m)
{
	Parent::processTick(m);

  // don't process moves or client ticks
  if (m != 0 || isClientObject())
    return;

  process_server();
}

void afxSelectron::advanceTime(F32 dt)
{
  Parent::advanceTime(dt);

  process_client(dt);
}

bool afxSelectron::onAdd()
{
  NetConnection* conn = NetConnection::getConnectionToServer();
  if (!conn || !Parent::onAdd())
    return false;

  conn->addObject(this);

  return(true);
}

void afxSelectron::onRemove()
{
  getContainer()->removeObject(this);
  Parent::onRemove();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

U32 afxSelectron::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
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

void afxSelectron::unpackUpdate(NetConnection * conn, BitStream * stream)
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

void afxSelectron::sync_with_clients()
{
  setMaskBits(SyncEventMask);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// private

bool afxSelectron::state_expired()
{
  afxPhrase* phrase = (effect_state == ACTIVE_STATE) ? phrases[MAIN_PHRASE] : NULL;

  if (phrase)
  {
    if (phrase->expired(effect_elapsed))
      return (!phrase->recycle(effect_elapsed));
    return false;
  }

  return true;
}

void afxSelectron::init_constraints()
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

void afxSelectron::setup_main_fx()
{
  phrases[MAIN_PHRASE] = new afxPhrase(isServerObject(), true);

  if (phrases[MAIN_PHRASE])
    phrases[MAIN_PHRASE]->init(datablock->main_fx_list, datablock->main_dur, this, time_factor, 
                               datablock->n_main_loops);
}

void afxSelectron::setup_select_fx()
{
  phrases[SELECT_PHRASE] = new afxPhrase(isServerObject(), true);

  if (phrases[SELECT_PHRASE])
    phrases[SELECT_PHRASE]->init(datablock->select_fx_list, -1, this, time_factor, 1);
}

void afxSelectron::setup_deselect_fx()
{
  phrases[DESELECT_PHRASE] = new afxPhrase(isServerObject(), true);

  if (phrases[DESELECT_PHRASE])
    phrases[DESELECT_PHRASE]->init(datablock->deselect_fx_list, -1, this, time_factor, 1);
}

bool afxSelectron::cleanup_over()
{
  for (S32 i = 0; i < NUM_PHRASES; i++)
    if (phrases[i] && !phrases[i]->isEmpty())
      return false;

  return true;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// private

void afxSelectron::process_server()
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

  for (S32 i = 0; i < NUM_PHRASES; i++)
    if (phrases[i])
      phrases[i]->update(TickSec, effect_elapsed);
}

void afxSelectron::change_state_s(U8 pending_state)
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

void afxSelectron::enter_done_state_s()
{ 
  postEvent(DEACTIVATE_EVENT);

  F32 done_time = effect_elapsed;

  for (S32 i = 0; i < NUM_PHRASES; i++)
  {
    if (phrases[i])
    {
      F32 phrase_done;
      if (phrases[i]->willStop() && phrases[i]->isInfinite())
        phrase_done = effect_elapsed + phrases[i]->calcAfterLife();
      else
        phrase_done = phrases[i]->calcDoneTime();
      if (phrase_done > done_time)
        done_time = phrase_done;
    }
  }

  F32 time_left = done_time - effect_elapsed;
  if (time_left < 0)
    time_left = 0;

  Sim::postEvent(this, new ObjectDeleteEvent, Sim::getCurrentTime() + time_left*1000 + 500);

  // CALL SCRIPT afxSelectronData::onDeactivate(%sele)
  Con::executef(datablock, 2, "onDeactivate", scriptThis());
}

void afxSelectron::enter_active_state_s()
{ 
  // stamp constraint-mgr starting time
  constraint_mgr->setStartTime(Platform::getVirtualMilliseconds());
  effect_elapsed = 0;

  setup_dynamic_constraints();

  // start casting effects
  setup_main_fx();
  if (phrases[MAIN_PHRASE])
    phrases[MAIN_PHRASE]->start(effect_elapsed, effect_elapsed);

  setup_select_fx();
  if (phrases[SELECT_PHRASE])
    phrases[SELECT_PHRASE]->start(effect_elapsed, effect_elapsed);
}

void afxSelectron::leave_active_state_s()
{ 
  if (phrases[MAIN_PHRASE])
    phrases[MAIN_PHRASE]->stop(effect_elapsed);
}

void afxSelectron::enter_cleanup_state_s()
{ 
  // start deselect effects
  setup_deselect_fx();
  if (phrases[SELECT_PHRASE])
    phrases[SELECT_PHRASE]->interrupt(effect_elapsed);
  if (phrases[DESELECT_PHRASE])
    phrases[DESELECT_PHRASE]->start(effect_elapsed, effect_elapsed);

  postEvent(SHUTDOWN_EVENT);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// private

void afxSelectron::process_client(F32 dt)
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
  for (S32 i = 0; i < NUM_PHRASES; i++)
    if (phrases[i])
      phrases[i]->update(dt, effect_elapsed);

}

void afxSelectron::change_state_c(U8 pending_state)
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
    enter_cleanup_state_c();
    break;
  case DONE_STATE:
    enter_done_state_c();
    break;
  }
}

void afxSelectron::enter_active_state_c(F32 starttime)
{ 
  // stamp constraint-mgr starting time
  constraint_mgr->setStartTime(Platform::getVirtualMilliseconds() - (U32)(effect_elapsed*1000));
  ///effect_elapsed = 0;

  setup_dynamic_constraints();

  setup_main_fx();
  if (phrases[MAIN_PHRASE])
    phrases[MAIN_PHRASE]->start(starttime, effect_elapsed);

  setup_select_fx();
  if (phrases[SELECT_PHRASE])
    phrases[SELECT_PHRASE]->start(starttime, effect_elapsed);
}

void afxSelectron::leave_active_state_c()
{ 
  if (phrases[MAIN_PHRASE])
  {
    //if (marks_mask & MARK_INTERRUPT)
    //  active_phrase->interrupt(effect_elapsed);
    //else
      phrases[MAIN_PHRASE]->stop(effect_elapsed);
  }
}

void afxSelectron::enter_cleanup_state_c()
{ 
  if (!client_only)
    return;

  // start deselect effects
  setup_deselect_fx();
  if (phrases[DESELECT_PHRASE])
    phrases[DESELECT_PHRASE]->start(effect_elapsed, effect_elapsed);

  postEvent(SHUTDOWN_EVENT);
}

void afxSelectron::enter_done_state_c()
{ 
  if (!client_only)
    return;

  postEvent(DEACTIVATE_EVENT);

  F32 done_time = effect_elapsed;

  for (S32 i = 0; i < NUM_PHRASES; i++)
  {
    if (phrases[i])
    {
      F32 phrase_done;
      if (phrases[i]->willStop() && phrases[i]->isInfinite())
        phrase_done = effect_elapsed + phrases[i]->calcAfterLife();
      else
        phrase_done = phrases[i]->calcDoneTime();
      if (phrase_done > done_time)
        done_time = phrase_done;
    }
  }

  F32 time_left = done_time - effect_elapsed;
  if (time_left < 0)
    time_left = 0;

  Sim::postEvent(this, new ObjectDeleteEvent, Sim::getCurrentTime() + time_left*1000 + 500);

  // CALL SCRIPT afxMagicSpellData::onDeactivate(%spell)
  Con::executef(datablock, 2, "onDeactivate", scriptThis());
}

void afxSelectron::sync_client(U16 marks, U8 state, F32 elapsed)
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

void afxSelectron::postEvent(U8 event) 
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

void afxSelectron::finish_startup()
{
  // CALL SCRIPT afxSelectronData::onActivate(%selectron)
  Con::executef(datablock, 2, "onActivate", scriptThis());
  
  init_constraints();
  // resolve time-factors here

  postEvent(afxSelectron::ACTIVATE_EVENT);
}

// static
afxSelectron* 
afxSelectron::start_selectron(SceneObject* picked, U8 subcode) 
{
  AssertFatal(picked != NULL, "Selected object is missing.");

  afxSelectronData* datablock = arcaneFX::findSelectronData(picked->getType(), subcode);
  if (!datablock)
  {
    Con::errorf("startSelectron() -- failed to match object-type (%x/%d) to a selection-effect.",
                picked->getType(), subcode);
    return 0;
  }

  // create a new selectron instance
  afxSelectron* selectron = new afxSelectron();
  selectron->setDataBlock(datablock);

  // register
  if (!selectron->registerObject())
  {
    Con::errorf("afxSelectron: failed to register effect instance.");
    Sim::postEvent(selectron, new ObjectDeleteEvent, Sim::getCurrentTime());
    return 0;
  }

  // separating the final part of startup allows the calling script
  // to make certain types of calls on the returned selectron that need 
  // to happen prior to constraint initialization.
  Sim::postEvent(selectron, new SelectronFinishStartupEvent, Sim::getCurrentTime());

  return selectron;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// console functions

ConsoleMethod(afxSelectron, setTimeFactor, void, 3, 3, "(F32 factor)") 
{
  object->setTimeFactor(dAtof(argv[2]));
}

ConsoleMethod(afxSelectron, interrupt, void, 2, 2, "interrupt()") 
{
  object->postEvent(afxSelectron::INTERRUPT_EVENT);
}

ConsoleMethod(afxSelectron, stopSelectron, void, 2, 2, "stopSelectron()") 
{
  object->postEvent(afxSelectron::INTERRUPT_EVENT);
}

ConsoleFunction(startSelectron, S32, 2, 4, "startSelectron(selected_object, [subcode], [extra])")
{
  SceneObject* picked = dynamic_cast<SceneObject*>(Sim::findObject(argv[1]));
  if (!picked)
  {
    Con::errorf("startSelectron() -- failed to find selected object [%s].", argv[1]);
    return 0;
  }

  U8 subcode = (argc > 2) ? dAtoi(argv[2]) : 0;

  afxSelectron* selectron = afxSelectron::start_selectron(picked, subcode);
  if (!selectron)
    return 0;

  if (argc > 3)
  {
    SimObject* extra = Sim::findObject(argv[argc-1]);
    if (extra)
      selectron->setExtra(extra);
    else
      Con::errorf("startSelectron() -- failed to find extra object [%s].", argv[argc-1]);
  }

  return selectron->getId();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//









