
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "game/gameConnection.h"
#include "game/player.h"
#include "math/mathIO.h"
#include "sceneGraph/sceneGraph.h"
#include "terrain/waterBlock.h"

#include "afx/afxChoreographer.h"
#include "afx/afxPhrase.h"
#include "afx/afxMagicSpell.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

/*
DefineConsoleType( TypeDatablockDummy )

ConsoleType( SimObjectPtr, TypeDatablockDummy, sizeof(SimObject*) )

ConsoleSetType( TypeDatablockDummy )
{
  if (argc == 1)
  {
    SimObject **obj = (SimObject **)dptr;
    *obj = Sim::findObject(argv[0]);
  }
  else
    Con::printf("(TypeDatablockDummy) Cannot set multiple args to a single S32.");
}

ConsoleGetType( TypeDatablockDummy )
{
  return 0;
}
*/

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxMagicSpellData::ewValidator
//
// When any of the effect list fields (addCastingEffect, etc.) are set, this validator
// intercepts the value and adds it to the appropriate effects list. One validator is
// created for each effect list and an id is used to identify which list to add the effect
// to. 
//
void afxMagicSpellData::ewValidator::validateType(SimObject* object, void* typePtr)
{
  afxMagicSpellData* spelldata = dynamic_cast<afxMagicSpellData*>(object);
  afxEffectWrapperData** ew = (afxEffectWrapperData**)(typePtr);

  if (spelldata && ew)
  {
    switch (id)
    {
    case CASTING_PHRASE:
      spelldata->casting_fx_list.push_back(*ew);
      break;
    case LAUNCH_PHRASE:
      spelldata->launch_fx_list.push_back(*ew);
      break;
    case DELIVERY_PHRASE:
      spelldata->delivery_fx_list.push_back(*ew);
      break;
    case IMPACT_PHRASE:
      spelldata->impact_fx_list.push_back(*ew);
      break;
    case LINGER_PHRASE:
      spelldata->linger_fx_list.push_back(*ew);
      break;
    }
    *ew = 0;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class SpellFinishStartupEvent : public SimEvent
{
public:
  void process(SimObject* obj) 
  { 
     afxMagicSpell* spell = dynamic_cast<afxMagicSpell*>(obj);
     if (spell)
       spell->finish_startup();
  }
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxMagicSpellData

IMPLEMENT_CONSOLETYPE(afxMagicSpellData)
IMPLEMENT_GETDATATYPE(afxMagicSpellData)
IMPLEMENT_SETDATATYPE(afxMagicSpellData)
IMPLEMENT_CO_DATABLOCK_V1(afxMagicSpellData);

afxMagicSpellData::afxMagicSpellData()
{
  casting_dur = 0.0f;
  delivery_dur = 0.0f;
  linger_dur = 0.0f;

  n_casting_loops = 1;
  n_delivery_loops = 1;
  n_linger_loops = 1;

  extra_casting_time = 0.0f;
  extra_delivery_time = 0.0f;
  extra_linger_time = 0.0f;

  // interrupt flags 
  do_move_interrupts = true;
  move_interrupt_speed = 2.0f;

  // delivers projectile spells
  missile = 0;
  launch_on_server_signal = false;
  primary_target_types = PlayerObjectType;

  // dummy entry holds effect-wrapper pointer while a special validator
  // grabs it and adds it to an appropriate effects list
  dummy_fx_entry = NULL;

  // marked true if datablock ids need to
  // be converted into pointers
  do_id_convert = false;
}

void afxMagicSpellData::reset()
{
  casting_fx_list.clear();
  launch_fx_list.clear();
  delivery_fx_list.clear();
  impact_fx_list.clear();
  linger_fx_list.clear();
}

#define myOffset(field) Offset(field, afxMagicSpellData)

void afxMagicSpellData::initPersistFields()
{
  Parent::initPersistFields();

  // spell phase timing
  addField("castingDur",            TypeF32,        myOffset(casting_dur));
  addField("deliveryDur",           TypeF32,        myOffset(delivery_dur));
  addField("lingerDur",             TypeF32,        myOffset(linger_dur));
  //
  addField("numCastingLoops",       TypeS32,        myOffset(n_casting_loops));
  addField("numDeliveryLoops",      TypeS32,        myOffset(n_delivery_loops));
  addField("numLingerLoops",        TypeS32,        myOffset(n_linger_loops));
  //
  addField("extraCastingTime",      TypeF32,        myOffset(extra_casting_time));
  addField("extraDeliveryTime",     TypeF32,        myOffset(extra_delivery_time));
  addField("extraLingerTime",       TypeF32,        myOffset(extra_linger_time));

  // interrupt flags
  addField("allowMovementInterrupts", TypeBool,     myOffset(do_move_interrupts));
  addField("movementInterruptSpeed",  TypeF32,      myOffset(move_interrupt_speed));

  // delivers projectile spells
  addField("missile",               TypeafxMagicMissileDataPtr, myOffset(missile));
  addField("launchOnServerSignal",  TypeBool,                   myOffset(launch_on_server_signal));
  addField("primaryTargetTypes",    TypeS32,                    myOffset(primary_target_types));

  // effect lists
  // for each of these, dummy_fx_entry is set and then a validator adds it to the appropriate effects list 
  
  addFieldV("addCastingEffect",   TypeafxEffectWrapperDataPtr,    myOffset(dummy_fx_entry),  new ewValidator(CASTING_PHRASE));
  addFieldV("addLaunchEffect",    TypeafxEffectWrapperDataPtr,    myOffset(dummy_fx_entry),  new ewValidator(LAUNCH_PHRASE));
  addFieldV("addDeliveryEffect",  TypeafxEffectWrapperDataPtr,    myOffset(dummy_fx_entry),  new ewValidator(DELIVERY_PHRASE));
  addFieldV("addImpactEffect",    TypeafxEffectWrapperDataPtr,    myOffset(dummy_fx_entry),  new ewValidator(IMPACT_PHRASE));
  addFieldV("addLingerEffect",    TypeafxEffectWrapperDataPtr,    myOffset(dummy_fx_entry),  new ewValidator(LINGER_PHRASE));
}

bool afxMagicSpellData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;

  if (missile != NULL && delivery_dur == 0.0)
    delivery_dur = -1;
  
  return true;
}

void afxMagicSpellData::pack_fx(BitStream* stream, const afxEffectList& fx, bool packed)
{
  stream->writeInt(fx.size(), EFFECTS_PER_PHRASE_BITS);
  for (int i = 0; i < fx.size(); i++)
    writeDatablockID(stream, fx[i], packed);
}

void afxMagicSpellData::unpack_fx(BitStream* stream, afxEffectList& fx)
{
  fx.clear();
  S32 n_fx = stream->readInt(EFFECTS_PER_PHRASE_BITS);
  for (int i = 0; i < n_fx; i++)
    fx.push_back((afxEffectWrapperData*)readDatablockID(stream));
}

void afxMagicSpellData::packData(BitStream* stream)
{
	Parent::packData(stream);

  stream->write(casting_dur);
  stream->write(delivery_dur);
  stream->write(linger_dur);
  //
  stream->write(n_casting_loops);
  stream->write(n_delivery_loops);
  stream->write(n_linger_loops);
  //
  stream->write(extra_casting_time);
  stream->write(extra_delivery_time);
  stream->write(extra_linger_time);

  stream->writeFlag(do_move_interrupts);
  stream->write(move_interrupt_speed);

  writeDatablockID(stream, missile, packed);
  stream->write(launch_on_server_signal);
  stream->write(primary_target_types);

  pack_fx(stream, casting_fx_list, packed);
  pack_fx(stream, launch_fx_list, packed);
  pack_fx(stream, delivery_fx_list, packed);
  pack_fx(stream, impact_fx_list, packed);
  pack_fx(stream, linger_fx_list, packed);
}

void afxMagicSpellData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  stream->read(&casting_dur);
  stream->read(&delivery_dur);
  stream->read(&linger_dur);
  //
  stream->read(&n_casting_loops);
  stream->read(&n_delivery_loops);
  stream->read(&n_linger_loops);
  //
  stream->read(&extra_casting_time);
  stream->read(&extra_delivery_time);
  stream->read(&extra_linger_time);

  do_move_interrupts = stream->readFlag();
  stream->read(&move_interrupt_speed);

  missile = (afxMagicMissileData*) readDatablockID(stream);
  stream->read(&launch_on_server_signal);
  stream->read(&primary_target_types);

  do_id_convert = true;
  unpack_fx(stream, casting_fx_list);
  unpack_fx(stream, launch_fx_list);
  unpack_fx(stream, delivery_fx_list);
  unpack_fx(stream, impact_fx_list);
  unpack_fx(stream, linger_fx_list);
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
          "afxMagicSpellData::preload() -- bad datablockId: 0x%x (%s)", 
          db_id, tag);
      }
    }
  }
}

bool afxMagicSpellData::preload(bool server, char errorBuffer[256])
{
  if (!Parent::preload(server, errorBuffer))
    return false;

  // Resolve objects transmitted from server
  if (!server) 
  {
    if (do_id_convert)
    {
      SimObjectId missile_id = (SimObjectId)missile;
      if (missile_id != 0)
      {
        // try to convert id to pointer
        if (!Sim::findObject(missile_id, missile))
        {
          Con::errorf(ConsoleLogEntry::General,
            "afxMagicSpellData::preload() -- bad datablockId: 0x%x (missile)",
            missile_id);
        }
      }
      expand_fx_list(casting_fx_list, "casting");
      expand_fx_list(launch_fx_list, "launch");
      expand_fx_list(delivery_fx_list, "delivery");
      expand_fx_list(impact_fx_list, "impact");
      expand_fx_list(linger_fx_list, "linger");
      do_id_convert = false;
    }
  }

  return true;
}

void afxMagicSpellData::gatherConstraintDefs(Vector<afxConstraintDef>& defs)
{
  gather_cons_defs(defs, casting_fx_list);
  gather_cons_defs(defs, launch_fx_list);
  gather_cons_defs(defs, delivery_fx_list);
  gather_cons_defs(defs, impact_fx_list);
  gather_cons_defs(defs, linger_fx_list);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

ConsoleMethod(afxMagicSpellData, reset, void, 2, 2, "reset()") 
{
  object->reset();
}

ConsoleMethod(afxMagicSpellData, addCastingEffect, void, 3, 3, "(afxEffectWrapperData)") 
{
  afxEffectWrapperData* ew = dynamic_cast<afxEffectWrapperData*>(Sim::findObject(argv[2]));
  if (!ew) 
  {
    Con::errorf(ConsoleLogEntry::General, 
                "afxMagicSpellData::addCastingEffect() -- "
                "failed to find afxEffectWrapperData [%s].",
                argv[1]);
    return;
  }
  
  object->casting_fx_list.push_back(ew);
}

ConsoleMethod(afxMagicSpellData, addLaunchEffect, void, 3, 3, "(afxEffectWrapperData)") 
{
  afxEffectWrapperData* ew = dynamic_cast<afxEffectWrapperData*>(Sim::findObject(argv[2]));
  if (!ew) 
  {
    Con::errorf(ConsoleLogEntry::General, 
                "afxMagicSpellData::addLaunchEffect() -- "
                "failed to find afxEffectWrapperData [%s].",
                argv[1]);
    return;
  }
  
  object->launch_fx_list.push_back(ew);
}

ConsoleMethod(afxMagicSpellData, addDeliveryEffect, void, 3, 3, "(afxEffectWrapperData)") 
{
  afxEffectWrapperData* ew = dynamic_cast<afxEffectWrapperData*>(Sim::findObject(argv[2]));
  if (!ew) 
  {
    Con::errorf(ConsoleLogEntry::General, 
                "afxMagicSpellData::addDeliveryEffect() -- "
                "failed to find afxEffectWrapperData [%s].",
                argv[1]);
    return;
  }
  
  object->delivery_fx_list.push_back(ew);
}

ConsoleMethod(afxMagicSpellData, addImpactEffect, void, 3, 3, "(afxEffectWrapperData)") 
{
  afxEffectWrapperData* ew = dynamic_cast<afxEffectWrapperData*>(Sim::findObject(argv[2]));
  if (!ew) 
  {
    Con::errorf(ConsoleLogEntry::General, 
                "afxMagicSpellData::addImpactEffect() -- "
                "failed to find afxEffectWrapperData [%s].",
                argv[1]);
    return;
  }
  
  object->impact_fx_list.push_back(ew);
}

ConsoleMethod(afxMagicSpellData, addLingerEffect, void, 3, 3, "(afxEffectWrapperData)") 
{
  afxEffectWrapperData* ew = dynamic_cast<afxEffectWrapperData*>(Sim::findObject(argv[2]));
  if (!ew) 
  {
    Con::errorf(ConsoleLogEntry::General, 
                "afxMagicSpellData::addLingerEffect() -- "
                "failed to find afxEffectWrapperData [%s].",
                argv[1]);
    return;
  }
  
  object->linger_fx_list.push_back(ew);
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxMagicSpell

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// CastingPhrase_C
//    Subclass of afxPhrase for the client casting phrase. 
//    This subclass adds handling of the casting progress
//    bar in cases where the caster is the client's control
//    object.
//

class CastingPhrase_C : public afxPhrase
{
  typedef afxPhrase Parent;
  ShapeBase*    caster;
  bool          notify_castbar;
  F32           castbar_progress;
public:
  /*C*/         CastingPhrase_C(ShapeBase* caster, bool notify_castbar);
  virtual void  start(F32 startstamp, F32 timestamp);
  virtual void  update(F32 dt, F32 timestamp);
  virtual void  stop(F32 timestamp);
  virtual void  interrupt(F32 timestamp);
};

CastingPhrase_C::CastingPhrase_C(ShapeBase* c, bool notify) 
  : afxPhrase(false, true)
{
  caster = c;
  notify_castbar = notify;
  castbar_progress = 0.0f;
}

void CastingPhrase_C::start(F32 startstamp, F32 timestamp)
{
  Parent::start(startstamp, timestamp); //START
  if (notify_castbar)
  {
    castbar_progress = 0.0f;
    Con::executef(1, "onCastingStart");
  }
}

void CastingPhrase_C::update(F32 dt, F32 timestamp)
{
  Parent::update(dt, timestamp);

  if (!notify_castbar)
    return;

  if (dur > 0 && n_loops > 0)
  {
    F32 nfrac = (timestamp - starttime)/(dur*n_loops);
    if (nfrac - castbar_progress > 1.0f/200.0f)
    {
      castbar_progress = (nfrac < 1.0f) ? nfrac : 1.0f;
      Con::executef(2, "onCastingProgressUpdate", Con::getFloatArg(castbar_progress));
    }
  }
}

void CastingPhrase_C::stop(F32 timestamp)
{
  Parent::stop(timestamp);
  if (notify_castbar)
  {
    Con::executef(1, "onCastingEnd");
    notify_castbar = false;
  }
}

void CastingPhrase_C::interrupt(F32 timestamp)
{
  Parent::interrupt(timestamp);
  if (notify_castbar)
  {
    Con::executef(1, "onCastingEnd");
    notify_castbar = false;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// some enum to name converters for debugging purposes

static char* name_from_state(U8 s)
{
  switch (s)
  {
  case afxMagicSpell::INACTIVE_STATE:
    return "inactive";
  case afxMagicSpell::CASTING_STATE:
    return "casting";
  case afxMagicSpell::DELIVERY_STATE:
    return "delivery";
  case afxMagicSpell::LINGER_STATE:
    return "linger";
  case afxMagicSpell::CLEANUP_STATE:
    return "cleanup";
  case afxMagicSpell::DONE_STATE:
    return "done";
  }

  return "unknown";
}

static char* name_from_event(U8 e)
{
  switch (e)
  {
  case afxMagicSpell::NULL_EVENT:
    return "null";
  case afxMagicSpell::ACTIVATE_EVENT:
    return "activate";
  case afxMagicSpell::LAUNCH_EVENT:
    return "launch";
  case afxMagicSpell::IMPACT_EVENT:
    return "impact";
  case afxMagicSpell::SHUTDOWN_EVENT:
    return "shutdown";
  case afxMagicSpell::DEACTIVATE_EVENT:
    return "deactivate";
  case afxMagicSpell::INTERRUPT_PHASE_EVENT:
    return "interrupt_phase";
  case afxMagicSpell::INTERRUPT_SPELL_EVENT:
    return "interrupt_spell";
  }

  return "unknown";
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxMagicSpell

IMPLEMENT_CO_NETOBJECT_V1(afxMagicSpell);

// static
StringTableEntry  afxMagicSpell::CASTER_CONS;
StringTableEntry  afxMagicSpell::TARGET_CONS;
StringTableEntry  afxMagicSpell::MISSILE_CONS;
StringTableEntry  afxMagicSpell::CAMERA_CONS;
StringTableEntry  afxMagicSpell::LISTENER_CONS;
StringTableEntry  afxMagicSpell::IMPACT_POINT_CONS;
StringTableEntry  afxMagicSpell::IMPACTED_OBJECT_CONS;

void afxMagicSpell::init()
{
  // setup static predefined constraint names
  if (CASTER_CONS == 0)
  {
    CASTER_CONS = StringTable->insert("caster");
    TARGET_CONS = StringTable->insert("target");
    MISSILE_CONS = StringTable->insert("missile");
    CAMERA_CONS = StringTable->insert("camera");
    LISTENER_CONS = StringTable->insert("listener");
    IMPACT_POINT_CONS = StringTable->insert("impactPoint");
    IMPACTED_OBJECT_CONS = StringTable->insert("impactedObject");
  }

  // afxMagicSpell is always in scope, however effects 
  // do their own scoping in that they will shut off if 
  // their position constraint leaves scope. 
  //
  //   note -- ghosting is delayed until constraint 
  //           initialization is done.
  //
  //mNetFlags.set(Ghostable | ScopeAlways);
  mNetFlags.clear(Ghostable | ScopeAlways);

  datablock = NULL;

  caster = NULL;
  target = NULL;

  caster_scope_id = 0;
  target_scope_id = 0;
  target_is_shape = false;

  constraints_initialized = false;

  spell_state = (U8) INACTIVE_STATE;
  spell_elapsed = 0;

  // define named constraints
  constraint_mgr->defineConstraint(OBJECT_CONSTRAINT,  CASTER_CONS);
  constraint_mgr->defineConstraint(OBJECT_CONSTRAINT,  TARGET_CONS);
  constraint_mgr->defineConstraint(OBJECT_CONSTRAINT, MISSILE_CONS);
  constraint_mgr->defineConstraint(OBJECT_CONSTRAINT, CAMERA_CONS);
  constraint_mgr->defineConstraint(POINT_CONSTRAINT,  LISTENER_CONS);
  constraint_mgr->defineConstraint(POINT_CONSTRAINT,  IMPACT_POINT_CONS);
  constraint_mgr->defineConstraint(OBJECT_CONSTRAINT,  IMPACTED_OBJECT_CONS);

  for (S32 i = 0; i < NUM_PHRASES; i++)
  {
    phrases[i] = NULL;
    tfactors[i] = 1.0f;
  }

  notify_castbar = false;
  overall_time_factor = 1.0f;

  marks_mask = 0;

  missile = NULL;
  impacted_obj = NULL;
  impact_pos.zero();
  impact_norm.set(0,0,1);
  impacted_scope_id = 0;
  impacted_is_shape = false;
}

afxMagicSpell::afxMagicSpell()
{
  init();
}

afxMagicSpell::afxMagicSpell(ShapeBase* caster, SceneObject* target)
{
  init();

	this->caster = caster;
  if (caster)
  {
    deleteNotify(caster);
    processAfter(caster);
  }

	this->target = target;
  if (target)
    deleteNotify(target);
}

afxMagicSpell::~afxMagicSpell()
{
  for (S32 i = 0; i < NUM_PHRASES; i++)
  {
    if (phrases[i])
    {
      phrases[i]->interrupt(spell_elapsed);
      delete phrases[i];
    }
  }

  if (missile)
    missile->deleteObject();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// STANDARD OVERLOADED METHODS //

bool afxMagicSpell::onNewDataBlock(GameBaseData* dptr)
{
  datablock = dynamic_cast<afxMagicSpellData*>(dptr);
  if (!datablock || !Parent::onNewDataBlock(dptr))
    return false;

  return true;
}

void afxMagicSpell::processTick(const Move* m)
{
	Parent::processTick(m);

  // don't process moves or client ticks
  if (m != 0 || isClientObject())
    return;

  process_server();
}

void afxMagicSpell::advanceTime(F32 dt)
{
  Parent::advanceTime(dt);

  process_client(dt);
}

bool afxMagicSpell::onAdd()
{
	if (!Parent::onAdd()) 
    return(false);

  ////if (isServerObject())
  ////  init_constraints();

	return(true);
}

void afxMagicSpell::onRemove()
{
	Parent::onRemove();
}

void afxMagicSpell::onDeleteNotify(SimObject* obj)
{
  // caster deleted?
  ShapeBase* shape = dynamic_cast<ShapeBase*>(obj);
  if (shape == caster)
  {
    clearProcessAfter();
    caster = NULL;
    caster_scope_id = 0;
  }

  // target deleted?
  SceneObject* scene_obj = dynamic_cast<SceneObject*>(obj);
  if (scene_obj == target)
  {
    target = NULL;
    target_scope_id = 0;
    target_is_shape = false;
  }

  // impacted_obj deleted?
  if (scene_obj == impacted_obj)
  {
    impacted_obj = NULL;
    impacted_scope_id = 0;
    impacted_is_shape = false;
  }

  // missile deleted?
  afxMagicMissile* missile = dynamic_cast<afxMagicMissile*>(obj);
  if (missile != NULL && missile == this->missile)
  {
    this->missile = NULL;
  }

  // something else
  Parent::onDeleteNotify(obj);
}

// static
void afxMagicSpell::initPersistFields()
{
	Parent::initPersistFields();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

void afxMagicSpell::pack_constraint_info(NetConnection* conn, BitStream* stream)
{
  // pack caster's ghost index or scope id if not yet ghosted
  if (stream->writeFlag(caster != NULL))
  {
    S32 ghost_idx = conn->getGhostIndex(caster); 
    if (stream->writeFlag(ghost_idx != -1))
      stream->writeRangedU32(U32(ghost_idx), 0, NetConnection::MaxGhostCount);
    else
    {
      if (stream->writeFlag(caster->getScopeId() > 0))
        stream->writeInt(caster->getScopeId(), NetObject::SCOPE_ID_BITS);
    }
  }
  
  // pack target's ghost index or scope id if not yet ghosted
  if (stream->writeFlag(target != NULL))
  {
    S32 ghost_idx = conn->getGhostIndex(target);
    if (stream->writeFlag(ghost_idx != -1))
      stream->writeRangedU32(U32(ghost_idx), 0, NetConnection::MaxGhostCount);
    else
    {
      if (stream->writeFlag(target->getScopeId() > 0))
      {
        stream->writeInt(target->getScopeId(), NetObject::SCOPE_ID_BITS);
        stream->writeFlag(dynamic_cast<ShapeBase*>(target) != NULL); // is shape?
      }
    }
  }
  
  Parent::pack_constraint_info(conn, stream);
}

void afxMagicSpell::unpack_constraint_info(NetConnection* conn, BitStream* stream)
{
  caster = NULL;
  caster_scope_id = 0;
  if (stream->readFlag()) // has caster 
  {
    if (stream->readFlag()) // has ghost_idx
    {
      S32 ghost_idx = stream->readRangedU32(0, NetConnection::MaxGhostCount);
      caster = dynamic_cast<ShapeBase*>(conn->resolveGhost(ghost_idx));
      if (caster)
      {
        deleteNotify(caster);
        processAfter(caster);
      }
    }
    else
    {
      if (stream->readFlag()) // has scope_id (is always a shape)
        caster_scope_id = stream->readInt(NetObject::SCOPE_ID_BITS);
    }
  }

  target = NULL;
  target_scope_id = 0;
  target_is_shape = false;
  if (stream->readFlag()) // has target
  {
    if (stream->readFlag()) // has ghost_idx
    {
      S32 ghost_idx = stream->readRangedU32(0, NetConnection::MaxGhostCount);
      target = dynamic_cast<SceneObject*>(conn->resolveGhost(ghost_idx));
      if (target)
        deleteNotify(target);
    }
    else
    {
      if (stream->readFlag()) // has scope_id
      {
        target_scope_id = stream->readInt(NetObject::SCOPE_ID_BITS);
        target_is_shape = stream->readFlag(); // is shape?
      }
    }
  }
  
  Parent::unpack_constraint_info(conn, stream);
}

U32 afxMagicSpell::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
{
  U32 retMask = Parent::packUpdate(conn, mask, stream);

  // InitialUpdate
  if (stream->writeFlag(mask & InitialUpdateMask)) 
  {
    // pack initial exec conditions
    stream->write(exec_conds_mask);

    // flag if this client owns the spellcaster
    bool client_owns_caster = is_caster_client(caster, dynamic_cast<GameConnection*>(conn));
    stream->writeFlag(client_owns_caster);

    // pack per-phrase time-factor values
    for (S32 i = 0; i < NUM_PHRASES; i++)
      stream->write(tfactors[i]);

    // flag if this conn is zoned-in yet
    bool zoned_in = client_owns_caster;
    if (!zoned_in)
    {
      GameConnection* gconn = dynamic_cast<GameConnection*>(conn);
      zoned_in = (gconn) ? gconn->isZonedIn() : false;
    }
    if (stream->writeFlag(zoned_in))
      pack_constraint_info(conn, stream);
  }

  // StateEvent or SyncEvent
  if (stream->writeFlag((mask & StateEventMask) || (mask & SyncEventMask)))
  {
    stream->write(marks_mask);
    stream->write(spell_state);
    stream->write(state_elapsed());
    stream->write(spell_elapsed);
  }

  // SyncEvent
  if (stream->writeFlag((mask & SyncEventMask) && !(mask & InitialUpdateMask)))
  {
    pack_constraint_info(conn, stream);
  }

  // LaunchEvent
  if (stream->writeFlag((mask & LaunchEventMask) && (marks_mask & MARK_LAUNCH) && missile))
  {
    F32 vel; Point3F vel_vec;
    missile->getStartingVelocityValues(vel, vel_vec);
    // pack launch vector and velocity
    stream->write(vel);
    mathWrite(*stream, vel_vec);
  }

  // ImpactEvent
  if (stream->writeFlag(((mask & ImpactEventMask) || (mask & SyncEventMask)) && (marks_mask & MARK_IMPACT)))
  {
    // pack impact objects's ghost index or scope id if not yet ghosted
    if (stream->writeFlag(impacted_obj != NULL))
    {
      S32 ghost_idx = conn->getGhostIndex(impacted_obj);
      if (stream->writeFlag(ghost_idx != -1))
        stream->writeRangedU32(U32(ghost_idx), 0, NetConnection::MaxGhostCount);
      else
      {
        if (stream->writeFlag(impacted_obj->getScopeId() > 0))
        {
          stream->writeInt(impacted_obj->getScopeId(), NetObject::SCOPE_ID_BITS);
          stream->writeFlag(dynamic_cast<ShapeBase*>(impacted_obj) != NULL);
        }
      }
    }

    // pack impact position and normal
    mathWrite(*stream, impact_pos);
    mathWrite(*stream, impact_norm);
    stream->write(exec_conds_mask);

    ShapeBase* temp_shape;
    stream->writeFlag(caster != 0 && caster->getDamageState() == ShapeBase::Enabled);
    temp_shape = dynamic_cast<ShapeBase*>(target);
    stream->writeFlag(temp_shape != 0 && temp_shape->getDamageState() == ShapeBase::Enabled);
    temp_shape = dynamic_cast<ShapeBase*>(impacted_obj);
    stream->writeFlag(temp_shape != 0 && temp_shape->getDamageState() == ShapeBase::Enabled);
  }

  return retMask;
}

//~~~~~~~~~~~~~~~~~~~~//

void afxMagicSpell::unpackUpdate(NetConnection * conn, BitStream * stream)
{
  Parent::unpackUpdate(conn, stream);
  
  bool initial_update = false;
  bool zoned_in = true;
  bool do_sync_event = false;
  U16 new_marks_mask = 0;
  U8 new_spell_state = INACTIVE_STATE;
  F32 new_state_elapsed = 0;
  F32 new_spell_elapsed = 0;;

  // InitialUpdate
  if (stream->readFlag())
  {
    initial_update = true;

    // unpack initial exec conditions
    stream->read(&exec_conds_mask);
    
    // if this is controlling client for the caster,
    // enable castbar updates
    bool client_owns_caster = stream->readFlag();
    if (client_owns_caster)
      notify_castbar = true;

    // unpack per-phrase time-factor values
    for (S32 i = 0; i < NUM_PHRASES; i++)
      stream->read(&tfactors[i]);

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
    stream->read(&new_spell_state);
    stream->read(&new_state_elapsed);
    stream->read(&new_spell_elapsed);
    marks_mask = new_marks_mask;
  }

  // SyncEvent
  if (do_sync_event = stream->readFlag())
  {
    unpack_constraint_info(conn, stream);
    init_constraints();
  }

  // LaunchEvent
  if (stream->readFlag())
  {
    F32 vel; Point3F vel_vec;
    stream->read(&vel);
    mathRead(*stream, &vel_vec);
    if (missile)
    {
      missile->setStartingVelocity(vel);
      missile->setStartingVelocityVector(vel_vec);
    }
  }

  // ImpactEvent
  if (stream->readFlag())
  {
    if (impacted_obj)
      clearNotify(impacted_obj);
    impacted_obj = NULL;
    impacted_scope_id = 0;
    impacted_is_shape = false;
    if (stream->readFlag()) // is impacted_obj
    {
      if (stream->readFlag()) // is ghost_idx
      {
        S32 ghost_idx = stream->readRangedU32(0, NetConnection::MaxGhostCount);
        impacted_obj = dynamic_cast<SceneObject*>(conn->resolveGhost(ghost_idx));
        if (impacted_obj)
          deleteNotify(impacted_obj);
      }
      else
      {
        if (stream->readFlag()) // has scope_id
        {
          impacted_scope_id = stream->readInt(NetObject::SCOPE_ID_BITS);
          impacted_is_shape = stream->readFlag(); // is shape?
        }
      }
    }

    mathRead(*stream, &impact_pos);
    mathRead(*stream, &impact_norm);
    stream->read(&exec_conds_mask);

    bool caster_alive = stream->readFlag();
    bool target_alive = stream->readFlag();
    bool impacted_alive = stream->readFlag();

    afxConstraint* cons;
    if (cons = constraint_mgr->getConstraint(caster_cons_id))
      cons->setLivingState(caster_alive);
    if (cons = constraint_mgr->getConstraint(target_cons_id))
      cons->setLivingState(target_alive);
    if (cons = constraint_mgr->getConstraint(impacted_cons_id))
      cons->setLivingState(impacted_alive);
  }

  //~~~~~~~~~~~~~~~~~~~~//

  if (!zoned_in)
    spell_state = LATE_STATE;

  // need to adjust state info to get all synced up with spell on server
  if (do_sync_event && !initial_update)
    sync_client(new_marks_mask, new_spell_state, new_state_elapsed, new_spell_elapsed);
}

void afxMagicSpell::sync_with_clients()
{
  setMaskBits(SyncEventMask);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// private

bool afxMagicSpell::state_expired()
{
  afxPhrase* phrase = NULL;

  switch (spell_state)
  {
  case CASTING_STATE:
    phrase = phrases[CASTING_PHRASE];
    break;
  case DELIVERY_STATE:
    phrase = phrases[DELIVERY_PHRASE];
    break;
  case LINGER_STATE:
    phrase = phrases[LINGER_PHRASE];
    break;
  }

  if (phrase)
  {
    if (phrase->expired(spell_elapsed))
      return (!phrase->recycle(spell_elapsed));
    return false;
  }

  return true;
}

F32 afxMagicSpell::state_elapsed()
{
  afxPhrase* phrase = NULL;

  switch (spell_state)
  {
  case CASTING_STATE:
    phrase = phrases[CASTING_PHRASE];
    break;
  case DELIVERY_STATE:
    phrase = phrases[DELIVERY_PHRASE];
    break;
  case LINGER_STATE:
    phrase = phrases[LINGER_PHRASE];
    break;
  }

  return (phrase) ? phrase->elapsed(spell_elapsed) : 0.0f;
}

void afxMagicSpell::init_constraints()
{
  if (constraints_initialized)
  {
    //Con::printf("CONSTRAINTS ALREADY INITIALIZED");
    return;
  }

  Vector<afxConstraintDef> defs;
  datablock->gatherConstraintDefs(defs);

  constraint_mgr->initConstraintDefs(defs, isServerObject());

  if (isServerObject())
  {
    caster_cons_id = constraint_mgr->setReferenceObject(CASTER_CONS, caster);
    target_cons_id = constraint_mgr->setReferenceObject(TARGET_CONS, target);
    if (caster->isScopeable())
      constraint_mgr->addScopeableObject(caster);
    if (target && target->isScopeable())
      constraint_mgr->addScopeableObject(target);

    // find local camera
    GameConnection* conn = caster->getControllingClient();
    if (conn)
    {  
      ShapeBase* camera = conn->getCameraObject();
      if (camera)
        constraint_mgr->setReferenceObject(CAMERA_CONS, camera);
    }
  }
  else
  {
    if (caster)
      caster_cons_id = constraint_mgr->setReferenceObject(CASTER_CONS, caster); 
    else if (caster_scope_id > 0)
      caster_cons_id = constraint_mgr->setReferenceObjectByScopeId(CASTER_CONS, caster_scope_id, true); 

    if (target)
      target_cons_id = constraint_mgr->setReferenceObject(TARGET_CONS, target); 
    else if (target_scope_id > 0)
      target_cons_id = constraint_mgr->setReferenceObjectByScopeId(TARGET_CONS, target_scope_id, target_is_shape);

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

  constraint_mgr->adjustProcessOrdering(this);

  constraints_initialized = true;

  if (isServerObject())
  {
     mNetFlags.set(Ghostable);
     setScopeAlways();
  }
}

void afxMagicSpell::setup_casting_fx()
{
  if (isServerObject())
    phrases[CASTING_PHRASE] = new afxPhrase(isServerObject(), true);
  else
    phrases[CASTING_PHRASE] = new CastingPhrase_C(caster, notify_castbar);

  if (phrases[CASTING_PHRASE])
    phrases[CASTING_PHRASE]->init(datablock->casting_fx_list, datablock->casting_dur, this, 
                                  tfactors[CASTING_PHRASE], datablock->n_casting_loops,
                                  datablock->extra_casting_time);
}

void afxMagicSpell::setup_launch_fx()
{
  phrases[LAUNCH_PHRASE] = new afxPhrase(isServerObject(), false);
  if (phrases[LAUNCH_PHRASE])
    phrases[LAUNCH_PHRASE]->init(datablock->launch_fx_list, -1, this, 
                                 tfactors[LAUNCH_PHRASE], 1);
}

void afxMagicSpell::setup_delivery_fx()
{
  phrases[DELIVERY_PHRASE] = new afxPhrase(isServerObject(), true);
  if (phrases[DELIVERY_PHRASE])
  {
    phrases[DELIVERY_PHRASE]->init(datablock->delivery_fx_list, datablock->delivery_dur, this, 
                                   tfactors[DELIVERY_PHRASE], datablock->n_delivery_loops,
                                   datablock->extra_delivery_time);
  }
}

void afxMagicSpell::setup_impact_fx()
{
  phrases[IMPACT_PHRASE] = new afxPhrase(isServerObject(), false);
  if (phrases[IMPACT_PHRASE])
  {
    phrases[IMPACT_PHRASE]->init(datablock->impact_fx_list, -1, this, 
                                 tfactors[IMPACT_PHRASE], 1);
  }
}

void afxMagicSpell::setup_linger_fx()
{
  phrases[LINGER_PHRASE] = new afxPhrase(isServerObject(), true);
  if (phrases[LINGER_PHRASE])
    phrases[LINGER_PHRASE]->init(datablock->linger_fx_list, datablock->linger_dur, this, 
                                 tfactors[LINGER_PHRASE], datablock->n_linger_loops,
                                 datablock->extra_linger_time);
} 

bool afxMagicSpell::cleanup_over()
{
  for (S32 i = 0; i < NUM_PHRASES; i++)
    if (phrases[i] && !phrases[i]->isEmpty())
      return false;

  return true;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// private
//
// MISSILE STUFF
//

void afxMagicSpell::init_missile_s(afxMagicMissileData* mm_db)
{
  if (missile)
    clearNotify(missile);

  // create the missile
  missile = new afxMagicMissile(true, false);
  missile->setDataBlock(mm_db);
  missile->setChoreographer(this);
  if (!missile->registerObject())
  {
    Con::errorf("afxMagicSpell: failed to register missile instance.");
    delete missile;
    missile = NULL;
  }

  if (missile)
  {
    deleteNotify(missile);
    registerForCleanup(missile);
  }
}

void afxMagicSpell::launch_missile_s()
{
  if (missile)
  {
    constraint_mgr->setReferenceObject(MISSILE_CONS, missile);
    missile->launch();
  }
}

void afxMagicSpell::init_missile_c(afxMagicMissileData* mm_db)
{
  if (missile)
    clearNotify(missile);

  // create the missile
  missile = new afxMagicMissile(false, true);
  missile->setDataBlock(mm_db);
  missile->setChoreographer(this);
  if (!missile->registerObject())
  {
    Con::errorf("afxMagicSpell: failed to register missile instance.");
    delete missile;
    missile = NULL;
  }

  if (missile)
  {
    deleteNotify(missile);
    registerForCleanup(missile);
  }
}

void afxMagicSpell::launch_missile_c()
{
  if (missile)
  {
    constraint_mgr->setReferenceObject(MISSILE_CONS, missile);
    missile->launch();
  }
}

bool afxMagicSpell::is_impact_in_water(SceneObject* obj, const Point3F& p)
{
#if !defined(BROKEN_SPLASH_CODE)
  Point3F point = (obj) ? obj->getPosition() : p;

  SimpleQueryList sql;
  gServerSceneGraph->getWaterObjectList(sql);
  
  for (U32 i = 0; i < sql.mList.size(); i++)
  {
    WaterBlock* pBlock = dynamic_cast<WaterBlock*>(sql.mList[i]);
    if (pBlock && pBlock->isPointSubmergedSimple(point))
      return true;
  }
#endif

  return false;
}

void afxMagicSpell::impactNotify(const Point3F& p, const Point3F& n, SceneObject* obj)
{
  if (isClientObject())
    return;

  ///impact_time_ms = spell_elapsed_ms;
  if (impacted_obj)
      clearNotify(impacted_obj);
  impacted_obj = obj;
  impact_pos = p;
  impact_norm = n;

  if (impacted_obj != NULL)
  {
    deleteNotify(impacted_obj);
    exec_conds_mask |= IMPACTED_SOMETHING;
    if (impacted_obj == target)
      exec_conds_mask |= IMPACTED_TARGET;
    if (impacted_obj->getType() & datablock->primary_target_types)
      exec_conds_mask |= IMPACTED_PRIMARY;
  }

  if (is_impact_in_water(obj, p))
    exec_conds_mask |= IMPACT_IN_WATER;

  postSpellEvent(IMPACT_EVENT);

  if (missile)
    clearNotify(missile);
  missile = NULL;
}

void afxMagicSpell::executeScriptEvent(const char* method, afxConstraint* cons, 
                                        const MatrixF& xfm, const char* data)
{
  SceneObject* cons_obj = (cons) ? cons->getObject() : NULL;

  char *arg_buf = Con::getArgBuffer(256);
  Point3F pos;
  xfm.getColumn(3,&pos);
  AngAxisF aa(xfm);
  dSprintf(arg_buf,256,"%g %g %g %g %g %g %g",
           pos.x, pos.y, pos.z,
           aa.axis.x, aa.axis.y, aa.axis.z, aa.angle);

  // CALL SCRIPT afxChoreographerData::method(%spell, %caster, %constraint, %transform, %data)
  Con::executef(datablock, 6, method, 
                scriptThis(),
                (caster) ? caster->scriptThis() : "", 
                (cons_obj) ? cons_obj->scriptThis() : "",
                arg_buf,
                data);
}

void afxMagicSpell::inflictDamage(const char * label, const char* flavor, SimObjectId target_id,
                                   F32 amount, U8 n, F32 ad_amount, F32 radius, Point3F pos, F32 impulse)
{ 
 // Con::printf("INFLICT-DAMAGE label=%s flav=%s id=%d amt=%g n=%d rad=%g pos=(%g %g %g) imp=%g",
 //             label, flavor, target_id, amount, n, radius, pos.x, pos.y, pos.z, impulse);

  // CALL SCRIPT afxMagicSpellData::onDamage()
  //    onDamage(%spell, %label, %type, %damaged_obj, %amount, %count, %pos, %ad_amount, 
  //             %radius, %impulse)
  char *posArg = Con::getArgBuffer(64);
  dSprintf(posArg, 64, "%f %f %f", pos.x, pos.y, pos.z);
  Con::executef(datablock, 11, "onDamage", 
                                  scriptThis(), 
                                  label,
                                  flavor,
                                  Con::getIntArg(target_id),
                                  Con::getFloatArg(amount), 
                                  Con::getIntArg(n),
                                  posArg,
                                  Con::getFloatArg(ad_amount), 
                                  Con::getFloatArg(radius),
                                  Con::getFloatArg(impulse));
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// private

void afxMagicSpell::process_server()
{
  spell_elapsed += TickSec;

  U8 pending_state = spell_state;

  // check for state changes
  switch (spell_state)
  {

  case INACTIVE_STATE:
    if (marks_mask & MARK_ACTIVATE)
      pending_state = CASTING_STATE;
    break;

  case CASTING_STATE:
    if (datablock->do_move_interrupts && is_caster_moving())
    {
      displayScreenMessage(caster, "SPELL INTERRUPTED.");
      postSpellEvent(INTERRUPT_SPELL_EVENT);
    }
    if (marks_mask & MARK_INTERRUPT_CASTING)
      pending_state = CLEANUP_STATE;
    else if (marks_mask & MARK_END_CASTING)
      pending_state = DELIVERY_STATE;
    else if (marks_mask & MARK_LAUNCH)
      pending_state = DELIVERY_STATE;
    else if (state_expired())
      pending_state = DELIVERY_STATE;
    break;

  case DELIVERY_STATE:
    if (marks_mask & MARK_INTERRUPT_DELIVERY)
      pending_state = CLEANUP_STATE;
    else if (marks_mask & MARK_END_DELIVERY)
      pending_state = LINGER_STATE;
    else if (marks_mask & MARK_IMPACT)
      pending_state = LINGER_STATE;
    else if (state_expired())
      pending_state = LINGER_STATE;
    break;

  case LINGER_STATE:
    if (marks_mask & MARK_INTERRUPT_LINGER)
      pending_state = CLEANUP_STATE;
    else if (marks_mask & MARK_END_LINGER)
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

  if (spell_state != pending_state)
    change_state_s(pending_state);

  if (spell_state == INACTIVE_STATE)
    return;

  //--------------------------//

  // sample the constraints
  constraint_mgr->sample(TickSec, Platform::getVirtualMilliseconds());

  for (S32 i = 0; i < NUM_PHRASES; i++)
    if (phrases[i])
      phrases[i]->update(TickSec, spell_elapsed);
}

void afxMagicSpell::change_state_s(U8 pending_state)
{ 
  if (spell_state == pending_state)
    return;

  // LEAVING THIS STATE
  switch (spell_state)
  {
  case INACTIVE_STATE:
    break;
  case CASTING_STATE:
    leave_casting_state_s();
    break;
  case DELIVERY_STATE:
    leave_delivery_state_s();
    break;
  case LINGER_STATE:
    leave_linger_state_s();
    break;
  case CLEANUP_STATE:
    break;
  case DONE_STATE:
    break;
  }

  spell_state = pending_state;

  // ENTERING THIS STATE
  switch (pending_state)
  {
  case INACTIVE_STATE:
    break;
  case CASTING_STATE:
    enter_casting_state_s();
    break;
  case DELIVERY_STATE:
    enter_delivery_state_s();
    break;
  case LINGER_STATE:
    enter_linger_state_s();
    break;
  case CLEANUP_STATE:
    break;
  case DONE_STATE:
    enter_done_state_s();
    break;
  }
}

void afxMagicSpell::enter_done_state_s()
{ 
  postSpellEvent(DEACTIVATE_EVENT);

  if (marks_mask & MARK_INTERRUPTS)
  {
    Sim::postEvent(this, new ObjectDeleteEvent, Sim::getCurrentTime() + 500);
  }
  else
  {
    F32 done_time = spell_elapsed;

    for (S32 i = 0; i < NUM_PHRASES; i++)
      if (phrases[i])
      {
        F32 phrase_done;
        if (phrases[i]->willStop() && phrases[i]->isInfinite())
          phrase_done = spell_elapsed + phrases[i]->calcAfterLife();
        else
          phrase_done = phrases[i]->calcDoneTime();
        if (phrase_done > done_time)
          done_time = phrase_done;
      }

    F32 time_left = done_time - spell_elapsed;
    if (time_left < 0)
      time_left = 0;

    Sim::postEvent(this, new ObjectDeleteEvent, Sim::getCurrentTime() + time_left*1000 + 500);
  }

  // CALL SCRIPT afxMagicSpellData::onDeactivate(%spell)
  Con::executef(datablock, 2, "onDeactivate", scriptThis());
}

void afxMagicSpell::enter_casting_state_s()
{ 
  // note - onActivate() is called in cast_spell() instead of here to make sure any
  // new time-factor settings resolve before they are sent off to the clients.

  // stamp constraint-mgr starting time and reset spell timer
  constraint_mgr->setStartTime(Platform::getVirtualMilliseconds());
  spell_elapsed = 0;

  setup_dynamic_constraints();

  // start casting effects
  setup_casting_fx();
  if (phrases[CASTING_PHRASE])
    phrases[CASTING_PHRASE]->start(spell_elapsed, spell_elapsed);

  // initialize missile
  if (datablock->missile)
    init_missile_s(datablock->missile);
}

void afxMagicSpell::leave_casting_state_s()
{ 
  if (phrases[CASTING_PHRASE])
  {
    if (marks_mask & MARK_INTERRUPT_CASTING)
    {
      //Con::printf("INTERRUPT CASTING (S)");
      phrases[CASTING_PHRASE]->interrupt(spell_elapsed);
    }
    else
    {
      //Con::printf("LEAVING CASTING (S)");
      phrases[CASTING_PHRASE]->stop(spell_elapsed);
    }
  }

  if (marks_mask & MARK_INTERRUPT_CASTING)
  {
    // CALL SCRIPT afxMagicSpellData::onInterrupt(%spell)
    Con::executef(datablock, 3, "onInterrupt", scriptThis(), (caster) ? caster->scriptThis() : "");
  }
}

void afxMagicSpell::enter_delivery_state_s() 
{ 
  // CALL SCRIPT afxMagicSpellData::onLaunch(%spell, %caster, %target, %missile)
  Con::executef(datablock, 5, "onLaunch", scriptThis(), (caster) ? caster->scriptThis() : "", 
               (target) ? target->scriptThis() : "", (missile) ? missile->scriptThis() : "");

  if (datablock->launch_on_server_signal)
    postSpellEvent(LAUNCH_EVENT);

  if (missile)
    launch_missile_s();

  // start launch effects
  setup_launch_fx();
  if (phrases[LAUNCH_PHRASE])
    phrases[LAUNCH_PHRASE]->start(spell_elapsed, spell_elapsed); //START

  // start delivery effects
  setup_delivery_fx();
  if (phrases[DELIVERY_PHRASE])
    phrases[DELIVERY_PHRASE]->start(spell_elapsed, spell_elapsed); //START
}
 
void afxMagicSpell::leave_delivery_state_s() 
{ 
  if (phrases[DELIVERY_PHRASE])
  {
    if (marks_mask & MARK_INTERRUPT_DELIVERY)
    {
      //Con::printf("INTERRUPT DELIVERY (S)");
      phrases[DELIVERY_PHRASE]->interrupt(spell_elapsed);
    }
    else
    {
      //Con::printf("LEAVING DELIVERY (S)");
      phrases[DELIVERY_PHRASE]->stop(spell_elapsed);
    }
  }

  if (!missile && !(marks_mask & MARK_IMPACT))
  {  
    if (target)
    {
      Point3F p = afxMagicSpell::getShapeImpactPos(target);
      Point3F n = Point3F(0,0,1);
      impactNotify(p, n, target);
    }
    else
    {
      Point3F p = Point3F(0,0,0);
      Point3F n = Point3F(0,0,1);
      impactNotify(p, n, 0);
    }
  }
}

void afxMagicSpell::enter_linger_state_s() 
{ 
  if (impacted_obj)
  {
    impacted_cons_id = constraint_mgr->setReferenceObject(IMPACTED_OBJECT_CONS, impacted_obj);
    if (impacted_obj->isScopeable())
      constraint_mgr->addScopeableObject(impacted_obj);
  }
  else
    constraint_mgr->setReferencePoint(IMPACTED_OBJECT_CONS, impact_pos, impact_norm);
  constraint_mgr->setReferencePoint(IMPACT_POINT_CONS, impact_pos, impact_norm);
  constraint_mgr->setReferenceObject(MISSILE_CONS, 0);

  // start impact effects
  setup_impact_fx();
  if (phrases[IMPACT_PHRASE])
    phrases[IMPACT_PHRASE]->start(spell_elapsed, spell_elapsed); //START

  // start linger effects
  setup_linger_fx();
  if (phrases[LINGER_PHRASE])
    phrases[LINGER_PHRASE]->start(spell_elapsed, spell_elapsed); //START

  char pos_buf[128];
  dSprintf(pos_buf, sizeof(pos_buf), "%g %g %g", impact_pos.x, impact_pos.y, impact_pos.z);

  char norm_buf[128];
  dSprintf(norm_buf, sizeof(norm_buf), "%g %g %g", impact_norm.x, impact_norm.y, impact_norm.z);

  // CALL SCRIPT afxMagicSpellData::onImpact(%spell, %caster, %impactedObj, %impactedPos, %impactedNorm)
  Con::executef(datablock, 6, "onImpact", 
                scriptThis(), 
                (caster) ? caster->scriptThis():"",
                (impacted_obj) ? impacted_obj->scriptThis():"",
                pos_buf,
                norm_buf);
}

void afxMagicSpell::leave_linger_state_s() 
{
  if (phrases[LINGER_PHRASE])
  {
    if (marks_mask & MARK_INTERRUPT_LINGER)
    {
      //Con::printf("INTERRUPT LINGER (S)");
      phrases[LINGER_PHRASE]->interrupt(spell_elapsed);
    }
    else
    {
      //Con::printf("LEAVING LINGER (S)");
      phrases[LINGER_PHRASE]->stop(spell_elapsed);
    }
  }
}



//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// private

void afxMagicSpell::process_client(F32 dt)
{
  spell_elapsed += dt; //SPELL_ELAPSED

  U8 pending_state = spell_state;

  // check for state changes
  switch (spell_state)
  {
  case INACTIVE_STATE:
    if (marks_mask & MARK_ACTIVATE)
      pending_state = CASTING_STATE;
    break;
  case CASTING_STATE:
    if (marks_mask & MARK_INTERRUPT_CASTING)
      pending_state = CLEANUP_STATE;
    else if (marks_mask & MARK_END_CASTING)
      pending_state = DELIVERY_STATE;
    else if (datablock->launch_on_server_signal)
    {
      if (marks_mask & MARK_LAUNCH)
        pending_state = DELIVERY_STATE;
    }
    else
    {
      if (state_expired())
        pending_state = DELIVERY_STATE;
    }
    break;
  case DELIVERY_STATE:
    if (marks_mask & MARK_INTERRUPT_DELIVERY)
      pending_state = CLEANUP_STATE;
    else if (marks_mask & MARK_END_DELIVERY)
      pending_state = LINGER_STATE;
    else if (marks_mask & MARK_IMPACT)
      pending_state = LINGER_STATE;
    else if (state_expired())
      ;//pending_state = LINGER_STATE;
    break;
  case LINGER_STATE:
    if (marks_mask & MARK_INTERRUPT_LINGER)
      pending_state = CLEANUP_STATE;
    else if (marks_mask & MARK_END_LINGER)
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

  if (spell_state != pending_state)
    change_state_c(pending_state);

  if (spell_state == INACTIVE_STATE)
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
      phrases[i]->update(dt, spell_elapsed);
}

void afxMagicSpell::change_state_c(U8 pending_state)
{ 
  if (spell_state == pending_state)
    return;

  // LEAVING THIS STATE
  switch (spell_state)
  {
  case INACTIVE_STATE:
    break;
  case CASTING_STATE:
    leave_casting_state_c();
    break;
  case DELIVERY_STATE:
    leave_delivery_state_c();
    break;
  case LINGER_STATE:
    leave_linger_state_c();
    break;
  case CLEANUP_STATE:
    break;
  case DONE_STATE:
    break;
  }

  spell_state = pending_state;

  // ENTERING THIS STATE
  switch (pending_state)
  {
  case INACTIVE_STATE:
    break;
  case CASTING_STATE:
    enter_casting_state_c(spell_elapsed);
    break;
  case DELIVERY_STATE:
    enter_delivery_state_c(spell_elapsed);
    break;
  case LINGER_STATE:
    enter_linger_state_c(spell_elapsed);
    break;
  case CLEANUP_STATE:
    break;
  case DONE_STATE:
    break;
  }
}

void afxMagicSpell::enter_casting_state_c(F32 starttime)
{ 
  // stamp constraint-mgr starting time
  constraint_mgr->setStartTime(Platform::getVirtualMilliseconds() - (U32)(spell_elapsed*1000));
  //spell_elapsed = 0; //SPELL_ELAPSED

  setup_dynamic_constraints();

  // start casting effects and castbar
  setup_casting_fx();
  if (phrases[CASTING_PHRASE])
    phrases[CASTING_PHRASE]->start(starttime, spell_elapsed); //START

  // initialize missile
  if (datablock->missile)
    init_missile_c(datablock->missile);
}

void afxMagicSpell::leave_casting_state_c()
{ 
  if (phrases[CASTING_PHRASE])
  {
    if (marks_mask & MARK_INTERRUPT_CASTING)
    {
      //Con::printf("INTERRUPT CASTING (C)");
      phrases[CASTING_PHRASE]->interrupt(spell_elapsed);
    }
    else
    {
      //Con::printf("LEAVING CASTING (C)");
      phrases[CASTING_PHRASE]->stop(spell_elapsed);
    }
  }
}

void afxMagicSpell::enter_delivery_state_c(F32 starttime) 
{ 
  launch_missile_c();

  setup_launch_fx();
  if (phrases[LAUNCH_PHRASE])
    phrases[LAUNCH_PHRASE]->start(starttime, spell_elapsed); //START

  setup_delivery_fx();
  if (phrases[DELIVERY_PHRASE])
    phrases[DELIVERY_PHRASE]->start(starttime, spell_elapsed); //START
}

void afxMagicSpell::leave_delivery_state_c() 
{ 
  if (missile)
  {
    clearNotify(missile);
    missile->deleteObject();
    missile = NULL;
  }

  if (phrases[DELIVERY_PHRASE])
  {
    if (marks_mask & MARK_INTERRUPT_DELIVERY)
    {
      //Con::printf("INTERRUPT DELIVERY (C)");
      phrases[DELIVERY_PHRASE]->interrupt(spell_elapsed);
    }
    else
    {
      //Con::printf("LEAVING DELIVERY (C)");
      phrases[DELIVERY_PHRASE]->stop(spell_elapsed);
    }
  }
}

void afxMagicSpell::enter_linger_state_c(F32 starttime) 
{ 
  if (impacted_obj)
    impacted_cons_id = constraint_mgr->setReferenceObject(IMPACTED_OBJECT_CONS, impacted_obj);
  else if (impacted_scope_id > 0)
    impacted_cons_id = constraint_mgr->setReferenceObjectByScopeId(IMPACTED_OBJECT_CONS, impacted_scope_id, impacted_is_shape);
  else
    constraint_mgr->setReferencePoint(IMPACTED_OBJECT_CONS, impact_pos, impact_norm);
  constraint_mgr->setReferencePoint(IMPACT_POINT_CONS, impact_pos, impact_norm);
  constraint_mgr->setReferenceObject(MISSILE_CONS, 0);

  setup_impact_fx(); 
  if (phrases[IMPACT_PHRASE])
    phrases[IMPACT_PHRASE]->start(starttime, spell_elapsed); //START

  setup_linger_fx();
  if (phrases[LINGER_PHRASE])
  {
    phrases[LINGER_PHRASE]->start(starttime, spell_elapsed); //START
  }
}

void afxMagicSpell::leave_linger_state_c() 
{ 
  if (phrases[LINGER_PHRASE])
  {
    if (marks_mask & MARK_INTERRUPT_LINGER)
    {
      //Con::printf("INTERRUPT LINGER (C)");
      phrases[LINGER_PHRASE]->interrupt(spell_elapsed);
    }
    else
    {
      //Con::printf("LEAVING LINGER (C)");
      phrases[LINGER_PHRASE]->stop(spell_elapsed);
    }
  }
}

void afxMagicSpell::sync_client(U16 marks, U8 state, F32 elapsed, F32 spell_elapsed)
{
  //Con::printf("SYNC marks=%d old_state=%s state=%s elapsed=%g spell_elapsed=%g", 
  //            marks, name_from_state(spell_state), name_from_state(state), elapsed, 
  //            spell_elapsed);

  if (spell_state != LATE_STATE)
    return;

  marks_mask = marks;

  // don't want to be started on late zoning clients
  if (!datablock->exec_on_new_clients)
  {
    spell_state = DONE_STATE;
  }

  // it looks like we're ghosting pretty late and
  // should just return to the inactive state.
  else if ((marks & (MARK_INTERRUPTS | MARK_DEACTIVATE | MARK_SHUTDOWN)) ||
           (((marks & MARK_IMPACT) || (marks & MARK_END_DELIVERY)) && (marks & MARK_END_LINGER)))
  {
    spell_state = DONE_STATE;
  }

  // it looks like we should be in the linger state.
  else if ((marks & MARK_IMPACT) || 
           (((marks & MARK_LAUNCH) || (marks & MARK_END_CASTING)) && (marks & MARK_END_DELIVERY)))
  {
    spell_state = LINGER_STATE;
    this->spell_elapsed = spell_elapsed;
    enter_linger_state_c(spell_elapsed-elapsed);
  }

  // it looks like we should be in the delivery state.
  else if ((marks & MARK_LAUNCH) || (marks & MARK_END_CASTING))
  {
    spell_state = DELIVERY_STATE;
    this->spell_elapsed = spell_elapsed;
    enter_delivery_state_c(spell_elapsed-elapsed);
  }

  // it looks like we should be in the casting state.
  else if (marks & MARK_ACTIVATE)
  {
    spell_state = CASTING_STATE; //SPELL_STATE
    this->spell_elapsed = spell_elapsed;
    enter_casting_state_c(spell_elapsed-elapsed);
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// public:

void afxMagicSpell::postSpellEvent(U8 event) 
{ 
  setMaskBits(StateEventMask);

  switch (event)
  {
  case ACTIVATE_EVENT:
    marks_mask |= MARK_ACTIVATE;
    break;
  case LAUNCH_EVENT:
    marks_mask |= MARK_LAUNCH;
    setMaskBits(LaunchEventMask);
    break;
  case IMPACT_EVENT:
    marks_mask |= MARK_IMPACT;
    setMaskBits(ImpactEventMask);
    break;
  case SHUTDOWN_EVENT:
    marks_mask |= MARK_SHUTDOWN;
    break;
  case DEACTIVATE_EVENT:
    marks_mask |= MARK_DEACTIVATE;
    break;
  case INTERRUPT_PHASE_EVENT:
    if (spell_state == CASTING_STATE)
      marks_mask |= MARK_END_CASTING;
    else if (spell_state == DELIVERY_STATE)
      marks_mask |= MARK_END_DELIVERY;
    else if (spell_state == LINGER_STATE)
      marks_mask |= MARK_END_LINGER;
    break;
  case INTERRUPT_SPELL_EVENT:
    if (spell_state == CASTING_STATE)
      marks_mask |= MARK_INTERRUPT_CASTING;
    else if (spell_state == DELIVERY_STATE)
      marks_mask |= MARK_INTERRUPT_DELIVERY;
    else if (spell_state == LINGER_STATE)
      marks_mask |= MARK_INTERRUPT_LINGER;
    break;
  }
}

void afxMagicSpell::resolveTimeFactors()
{ 
  for (S32 i = 0; i < NUM_PHRASES; i++)
    tfactors[i] *= overall_time_factor;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

void afxMagicSpell::finish_startup()
{
#if !defined(BROKEN_POINT_IN_WATER)
  // test if caster is in water
  Point3F pos = caster->getPosition();
  if (caster->pointInWater(pos))
    exec_conds_mask |= CASTER_IN_WATER;
#endif

  // CALL SCRIPT afxMagicSpellData::onActivate(%spell, %caster, %target)
  ////Con::executef(datablock, 4, "onActivate", scriptThis(), 
  ////              (caster) ? caster->scriptThis() : "", (target) ? target->scriptThis() : "");
  resolveTimeFactors();

  init_constraints();

  postSpellEvent(afxMagicSpell::ACTIVATE_EVENT);
}

// static
afxMagicSpell* 
afxMagicSpell::cast_spell(afxMagicSpellData* datablock, ShapeBase* caster, SceneObject* target)
{
  AssertFatal(datablock != NULL, "Datablock is missing.");
  AssertFatal(caster != NULL, "Caster is missing.");

  /*
  Con::printf("SPELL STATS %s", datablock->getName());
  Con::printf("    casting = %d", datablock->casting_fx_list.size());
  Con::printf("    launch = %d", datablock->launch_fx_list.size());
  Con::printf("    delivery = %d", datablock->delivery_fx_list.size());
  Con::printf("    impact = %d", datablock->impact_fx_list.size());
  Con::printf("    linger = %d", datablock->linger_fx_list.size());
  Con::printf("    total = %d", datablock->casting_fx_list.size() +
                                datablock->launch_fx_list.size() +
                                datablock->delivery_fx_list.size() +
                                datablock->impact_fx_list.size() +
                                datablock->linger_fx_list.size());
   */

  // create a new spell instance
  afxMagicSpell* spell = new afxMagicSpell(caster, target);
  spell->setDataBlock(datablock);

  // register
  if (!spell->registerObject())
  {
    Con::errorf("afxMagicSpell: failed to register spell instance.");
    Sim::postEvent(spell, new ObjectDeleteEvent, Sim::getCurrentTime());
    return 0;
  }
  registerForCleanup(spell);

  // separating the final part of startup allows the calling script
  // to make certain types of calls on the returned spell that need 
  // to happen prior to object registration.
  Sim::postEvent(spell, new SpellFinishStartupEvent, Sim::getCurrentTime());

  // CALL SCRIPT afxMagicSpellData::onActivate(%spell, %caster, %target)
  Con::executef(datablock, 4, "onActivate", spell->scriptThis(), 
                (caster) ? caster->scriptThis() : "", (target) ? target->scriptThis() : "");

  return spell;
}

void afxMagicSpell::displayScreenMessage(ShapeBase* caster, const char* msg)
{
  if (!caster)
    return;

  GameConnection* client = caster->getControllingClient();
  if (client)
    Con::executef(4, "DisplayScreenMessage", client->getIdString(), msg);
}

Point3F afxMagicSpell::getShapeImpactPos(SceneObject* obj)
{
  Point3F pos = obj->getRenderPosition();
  if (obj->getType() & CorpseObjectType)
    pos.z += 0.5f;
  else
    pos.z += (obj->getObjBox().len_z()/2);
  return pos;
}

void afxMagicSpell::restoreObject(SceneObject* obj) 
{ 
  if (obj->getScopeId() == caster_scope_id && dynamic_cast<ShapeBase*>(obj) != NULL)
  {
    caster_scope_id = 0;
    caster = (ShapeBase*)obj;
    deleteNotify(caster);
    processAfter(caster);
  }

  if (obj->getScopeId() == target_scope_id)
  {
    target_scope_id = 0;
    target = obj;
    deleteNotify(target);
  }

  if (obj->getScopeId() == impacted_scope_id)
  {
    impacted_scope_id = 0;
    impacted_obj = obj;
    deleteNotify(impacted_obj);
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// console methods/functions

ConsoleMethod(afxMagicSpell, getCaster, S32, 2, 2, "getCaster();" ) 
{
  ShapeBase* caster = object->getCaster();
  return (caster) ? caster->getId() : -1;
}

ConsoleMethod(afxMagicSpell, getTarget, S32, 2, 2, "getTarget();" ) 
{
  SceneObject* target = object->getTarget();
  return (target) ? target->getId() : -1;
}

ConsoleMethod(afxMagicSpell, getMissile, S32, 2, 2, "getMissile();" ) 
{
  afxMagicMissile* missile = object->getMissile();
  return (missile) ? missile->getId() : -1;
}

ConsoleMethod(afxMagicSpell, getImpactedObject, S32, 2, 2, "getImpactedObject();" ) 
{
  SceneObject* imp_obj = object->getImpactedObject();
  return (imp_obj) ? imp_obj->getId() : -1;
}

ConsoleMethod(afxMagicSpell, setTimeFactor, void, 3, 4, "(F32 factor) or (string phase, F32 factor)") 
{
  if (argc == 3)
    object->setTimeFactor(dAtof(argv[2]));
  else
  {
    if (dStricmp(argv[2], "overall") == 0)
      object->setTimeFactor(dAtof(argv[3]));
    else if (dStricmp(argv[2], "casting") == 0)
      object->setTimeFactor(afxMagicSpell::CASTING_PHRASE, dAtof(argv[3]));
    else if (dStricmp(argv[2], "launch") == 0)
      object->setTimeFactor(afxMagicSpell::LAUNCH_PHRASE, dAtof(argv[3]));
    else if (dStricmp(argv[2], "delivery") == 0)
      object->setTimeFactor(afxMagicSpell::DELIVERY_PHRASE, dAtof(argv[3]));
    else if (dStricmp(argv[2], "impact") == 0)
      object->setTimeFactor(afxMagicSpell::IMPACT_PHRASE, dAtof(argv[3]));
    else if (dStricmp(argv[2], "linger") == 0)
      object->setTimeFactor(afxMagicSpell::LINGER_PHRASE, dAtof(argv[3]));
    else
      Con::errorf("afxMagicSpell::setTimeFactor() -- unknown spell phrase [%s].", argv[2]);
  }
}

ConsoleMethod(afxMagicSpell, interruptStage, void, 2, 2, "interruptStage()") 
{
  object->postSpellEvent(afxMagicSpell::INTERRUPT_PHASE_EVENT);
}

ConsoleMethod(afxMagicSpell, interrupt, void, 2, 2, "interrupt()") 
{
  object->postSpellEvent(afxMagicSpell::INTERRUPT_SPELL_EVENT);
}

ConsoleFunction(castSpell, S32, 4, 5, "castSpell(spell_datablock, caster, target, [extra])")
{
  afxMagicSpellData* datablock = dynamic_cast<afxMagicSpellData*>(Sim::findObject(argv[1]));
  if (!datablock)
  {
    Con::errorf("castSpell() -- failed to find spell datablock [%s].", argv[1]);
    return 0;
  }

  ShapeBase* caster = dynamic_cast<ShapeBase*>(Sim::findObject(argv[2]));
  if (!caster)
  {
    Con::errorf("castSpell() -- failed to find spellcaster [%s].", argv[2]);
    return 0;
  }

  // target is optional (depends on spell)
  SceneObject* target = dynamic_cast<SceneObject*>(Sim::findObject(argv[3]));

  SimObject* extra = 0;
  if (argc > 4 && argv[argc-1][0] != '\0')
  {
    SimObject* extra = Sim::findObject(argv[argc-1]);
    if (!extra)
      Con::errorf("castSpell() -- failed to find extra object [%s].", argv[argc-1]);
  }

  // note -- we must examine all arguments prior to calling cast_spell because
  // it calls Con::executef() which will overwrite the argument array.
  afxMagicSpell* spell = afxMagicSpell::cast_spell(datablock, caster, target);
  if (!spell)
    return 0;

  if (extra)
    spell->setExtra(extra);

  return spell->getId();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

// deprecated

ConsoleMethod(afxMagicSpell, interruptSpellStage, void, 2, 2, "interruptSpellStage()") 
{
  object->postSpellEvent(afxMagicSpell::INTERRUPT_PHASE_EVENT);
}

ConsoleMethod(afxMagicSpell, interruptSpell, void, 2, 2, "interruptSpell()") 
{
  object->postSpellEvent(afxMagicSpell::INTERRUPT_SPELL_EVENT);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
