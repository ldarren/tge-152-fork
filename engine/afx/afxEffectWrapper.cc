 
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/consoleTypes.h"
#include "console/simBase.h"
#include "game/gameBase.h"
#include "math/mathIO.h"

#include "afx/ce/afxComponentEffect.h"
#include "afx/afxResidueMgr.h"
#include "afx/afxChoreographer.h"
#include "afx/afxConstraint.h"
#include "afx/xm/afxXfmMod.h"
#include "afx/afxEffectWrapper.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEffectWrapperData

IMPLEMENT_CONSOLETYPE(afxEffectWrapperData)
IMPLEMENT_GETDATATYPE(afxEffectWrapperData)
IMPLEMENT_SETDATATYPE(afxEffectWrapperData)
IMPLEMENT_CO_DATABLOCK_V1(afxEffectWrapperData);

DECLARE_CONSOLETYPE(SimDataBlock)
IMPLEMENT_CONSOLETYPE(SimDataBlock)
IMPLEMENT_GETDATATYPE(SimDataBlock)
IMPLEMENT_SETDATATYPE(SimDataBlock)

afxEffectWrapperData::afxEffectWrapperData()
{
  effect_name = ST_NULLSTRING;
  effect_data = 0;
  effect_desc = 0;
  data_ID = 0;
  use_as_cons_obj = false;
  use_ghost_as_cons_obj = false;

  // constraint data
  cons_spec = ST_NULLSTRING;
  pos_cons_spec = ST_NULLSTRING;
  orient_cons_spec = ST_NULLSTRING;
  aim_cons_spec = StringTable->insert("camera");
  life_cons_spec = ST_NULLSTRING;

  // conditional execution flags
  ranking_range.set(0,255);
  lod_range.set(0,255);
  life_conds = 0;
  for (S32 i = 0; i < MAX_CONDITION_STATES; i++)
  { 
    exec_cond_on_bits[i] = 0;
    exec_cond_off_bits[i] = 0;
    exec_cond_bitmasks[i] = 0;
  }

  timing.lifetime = -1;

  user_fade_out_time = 0.0;

  is_looping = false;
  n_loops = 0;
  loop_gap_time = 0.0f;

  ignore_time_factor = false;
  propagate_time_factor = false;

  // residue settings

  // scaling factors
  rate_factor = 1.0f;
  scale_factor = 1.0f;

  dMemset(xfm_modifiers, 0, sizeof(xfm_modifiers));

  forced_bbox.min.set(1,1,1);
  forced_bbox.max.set(-1,-1,-1);

  update_forced_bbox = false;

  // marked true if datablock ids need to
  // be converted into pointers
  do_id_convert = false;

  sort_priority = 0;
  direction.set(0,1,0);
  speed = 0.0f;
  mass = 1.0f;
}

#define myOffset(field) Offset(field, afxEffectWrapperData)

void afxEffectWrapperData::initPersistFields()
{
  Parent::initPersistFields();

  // the wrapped effect
  addField("effect",            TypeSimDataBlockPtr,  myOffset(effect_data));
  addField("effectName",        TypeString,           myOffset(effect_name));

  // constraints
  addField("constraint",              TypeString,   myOffset(cons_spec));
  addField("posConstraint",           TypeString,   myOffset(pos_cons_spec));
  addField("posConstraint2",          TypeString,   myOffset(aim_cons_spec));
  addField("orientConstraint",        TypeString,   myOffset(orient_cons_spec));
  addField("lifeConstraint",          TypeString,   myOffset(life_cons_spec));
  //
  addField("isConstraintSrc",         TypeBool,     myOffset(use_as_cons_obj));
  addField("ghostIsConstraintSrc",    TypeBool,     myOffset(use_ghost_as_cons_obj));

  addField("delay",             TypeF32,          myOffset(timing.delay));
  addField("lifetime",          TypeF32,          myOffset(timing.lifetime));
  addField("fadeInTime",        TypeF32,          myOffset(timing.fade_in_time));
  addField("residueLifetime",   TypeF32,          myOffset(timing.residue_lifetime));
  addField("fadeOutTime",       TypeF32,          myOffset(user_fade_out_time));

  addField("rateFactor",        TypeF32,          myOffset(rate_factor));
  addField("scaleFactor",       TypeF32,          myOffset(scale_factor));

  addField("isLooping",         TypeBool,         myOffset(is_looping));
  addField("loopCount",         TypeS32,          myOffset(n_loops));
  addField("loopGapTime",       TypeF32,          myOffset(loop_gap_time));

  addField("ignoreTimeFactor",    TypeBool,       myOffset(ignore_time_factor));
  addField("propagateTimeFactor", TypeBool,       myOffset(propagate_time_factor));

  addField("rankingRange",          TypeByteRange,    myOffset(ranking_range));
  addField("levelOfDetailRange",    TypeByteRange,    myOffset(lod_range));
  addField("lifeConditions",        TypeS32,      myOffset(life_conds));
  addField("execConditions",        TypeS32,      myOffset(exec_cond_on_bits),  MAX_CONDITION_STATES);
  addField("execOffConditions",     TypeS32,      myOffset(exec_cond_off_bits), MAX_CONDITION_STATES);

  addField("xfmModifiers",      TypeafxXM_BaseDataPtr, myOffset(xfm_modifiers),  MAX_XFM_MODIFIERS);

  addField("forcedBBox",        TypeBox3F,        myOffset(forced_bbox));
  addField("updateForcedBBox",  TypeBool,         myOffset(update_forced_bbox));

  addField("sortPriority",      TypeS8,           myOffset(sort_priority));
  addField("direction",         TypePoint3F,        myOffset(direction));
  addField("speed",             TypeF32,            myOffset(speed));
  addField("mass",              TypeF32,            myOffset(mass));

  // Conditional Execution Flags
  Con::setIntVariable("$afx::DISABLED", DISABLED);
  Con::setIntVariable("$afx::ENABLED", ENABLED);
  Con::setIntVariable("$afx::FAILING", FAILING);
  Con::setIntVariable("$afx::DEAD", DEAD);
  Con::setIntVariable("$afx::ALIVE", ALIVE);
  Con::setIntVariable("$afx::DYING", DYING);
}

bool afxEffectWrapperData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;

  if (!effect_data)
  {
    if (!Sim::findObject((SimObjectId)data_ID, effect_data))
    {
      Con::errorf("afxEffectWrapperData::onAdd() -- bad datablockId: 0x%x", data_ID);
      return false;
    }
  }

  if (effect_data)
  {
    if (!afxEffectAdapterDesc::identifyEffect(this))
    {
      Con::errorf("afxEffectWrapperData::onAdd() -- unknown effect type.");
      return false;
    }
  }

  // parse the constraint specifications
  bool runs_on_s = runsOnServer();
  bool runs_on_c = runsOnClient();
  cons_def.parseSpec(cons_spec, runs_on_s, runs_on_c);
  pos_cons_def.parseSpec(pos_cons_spec, runs_on_s, runs_on_c);
  orient_cons_def.parseSpec(orient_cons_spec, runs_on_s, runs_on_c);
  aim_cons_def.parseSpec(aim_cons_spec, runs_on_s, runs_on_c);
  life_cons_def.parseSpec(life_cons_spec, runs_on_s, runs_on_c);
  if (cons_def.isDefined())
  {
    pos_cons_def = cons_def;
    if (!orient_cons_def.isDefined())
      orient_cons_def = cons_def;
  }

  // figure out if fade-out is for effect of residue
  if (timing.residue_lifetime > 0)
  {
    timing.residue_fadetime = user_fade_out_time;
    timing.fade_out_time = 0.0f;
  }
  else
  {
    timing.residue_fadetime = 0.0f;
    timing.fade_out_time = user_fade_out_time;
  }

  // adjust fade-in time
  if (timing.lifetime >= 0)
  {
    timing.fade_in_time = getMin(timing.lifetime, timing.fade_in_time);
  }

  // adjust exec-conditions
  for (S32 i = 0; i < MAX_CONDITION_STATES; i++)
    exec_cond_bitmasks[i] = exec_cond_on_bits[i] | exec_cond_off_bits[i];

  return true;
}

void afxEffectWrapperData::packData(BitStream* stream)
{
  Parent::packData(stream);

  writeDatablockID(stream, effect_data, packed);

  stream->writeString(effect_name);

  stream->writeString(cons_spec);
  stream->writeString(pos_cons_spec);
  stream->writeString(orient_cons_spec);
  stream->writeString(aim_cons_spec);
  stream->writeString(life_cons_spec);
  //
  stream->write(use_as_cons_obj);
  //stream->write(use_ghost_as_cons_obj);

  stream->write(ranking_range.low);
  stream->write(ranking_range.high);
  stream->write(lod_range.low);
  stream->write(lod_range.high);

  for (S32 i = 0; i < MAX_CONDITION_STATES; i++)
  {
    stream->write(exec_cond_on_bits[i]);
    stream->write(exec_cond_off_bits[i]);
  }
  stream->write(life_conds);
  stream->write(timing.delay);
  stream->write(timing.lifetime);
  stream->write(timing.fade_in_time);
  stream->write(user_fade_out_time);
  stream->write(is_looping);
  stream->write(n_loops);
  stream->write(loop_gap_time);
  stream->write(ignore_time_factor);
  stream->write(propagate_time_factor);
  stream->write(timing.residue_lifetime);
  stream->write(rate_factor);
  stream->write(scale_factor);

  // modifiers
  pack_mods(stream, xfm_modifiers, packed);

  mathWrite(*stream, forced_bbox);
  stream->write(update_forced_bbox);

  stream->write(sort_priority);
  mathWrite(*stream, direction);
  stream->write(speed);
  stream->write(mass);
}

void afxEffectWrapperData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  data_ID = readDatablockID(stream);

  effect_name = stream->readSTString();

  cons_spec = stream->readSTString();
  pos_cons_spec = stream->readSTString();
  orient_cons_spec = stream->readSTString();
  aim_cons_spec = stream->readSTString();
  life_cons_spec = stream->readSTString();
  //
  stream->read(&use_as_cons_obj);
  //stream->read(&use_ghost_as_cons_obj);

  stream->read(&ranking_range.low);
  stream->read(&ranking_range.high);
  stream->read(&lod_range.low);
  stream->read(&lod_range.high);

  for (S32 i = 0; i < MAX_CONDITION_STATES; i++)
  {
    stream->read(&exec_cond_on_bits[i]);
    stream->read(&exec_cond_off_bits[i]);
  }
  stream->read(&life_conds);
  stream->read(&timing.delay);
  stream->read(&timing.lifetime);
  stream->read(&timing.fade_in_time);
  stream->read(&user_fade_out_time);
  stream->read(&is_looping);
  stream->read(&n_loops);
  stream->read(&loop_gap_time);
  stream->read(&ignore_time_factor);
  stream->read(&propagate_time_factor);
  stream->read(&timing.residue_lifetime);
  stream->read(&rate_factor);
  stream->read(&scale_factor);

  // modifiers
  do_id_convert = true;
  unpack_mods(stream, xfm_modifiers);

  mathRead(*stream, &forced_bbox);
  stream->read(&update_forced_bbox);

  stream->read(&sort_priority);
  mathRead(*stream, &direction);
  stream->read(&speed);
  stream->read(&mass);
}

/* static*/ 
S32 num_modifiers(afxXM_BaseData* mods[])
{
  S32 n_mods = 0;
  for (int i = 0; i < afxEffectDefs::MAX_XFM_MODIFIERS; i++)
  {
    if (mods[i])
    {
      if (i != n_mods)
      {
        mods[n_mods] = mods[i];
        mods[i] = 0;
      }
      n_mods++;
    }
  }

  return n_mods;
}

void afxEffectWrapperData::gather_cons_defs(Vector<afxConstraintDef>& defs)
{
  if (pos_cons_def.isDefined())
    defs.push_back(pos_cons_def);
  if (orient_cons_def.isDefined())
    defs.push_back(orient_cons_def);
  if (aim_cons_def.isDefined())
    defs.push_back(aim_cons_def);
  if (life_cons_def.isDefined())
    defs.push_back(life_cons_def);

  afxComponentEffectData* ce_data = dynamic_cast<afxComponentEffectData*>(effect_data);
  if (ce_data)
    ce_data->gather_cons_defs(defs);
}

void afxEffectWrapperData::pack_mods(BitStream* stream, afxXM_BaseData* mods[], bool packed)
{
  S32 n_mods = num_modifiers(mods);
  stream->writeInt(n_mods, 6);
  for (int i = 0; i < n_mods; i++)
    writeDatablockID(stream, mods[i], packed);
}

void afxEffectWrapperData::unpack_mods(BitStream* stream, afxXM_BaseData* mods[])
{
  S32 n_mods = stream->readInt(6);
  for (int i = 0; i < n_mods; i++)
    mods[i] = (afxXM_BaseData*) readDatablockID(stream);
}

bool afxEffectWrapperData::preload(bool server, char errorBuffer[256])
{
  if (!Parent::preload(server, errorBuffer))
    return false;
  
  // Resolve objects transmitted from server
  if (!server) 
  {
    if (do_id_convert)
    {
      for (int i = 0; i < MAX_XFM_MODIFIERS; i++)
      {
        SimObjectId db_id = (SimObjectId)xfm_modifiers[i];
        if (db_id != 0)
        {
          // try to convert id to pointer
          if (!Sim::findObject(db_id, xfm_modifiers[i]))
          {
            Con::errorf("afxEffectWrapperData::preload() -- bad datablockId: 0x%x (xfm_modifiers[%d])",
              db_id, i);
          }
        }
        do_id_convert = false;
      }
    }
  }
  
  return true;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEffectWrapper

IMPLEMENT_CONOBJECT(afxEffectWrapper);

afxEffectWrapper::afxEffectWrapper()
{
  choreographer = 0;
  datablock = 0;
  cons_mgr = 0;

  cond_alive = true;
  elapsed = 0;
  life_end = 0;
  life_elapsed = 0;
  stopped = false;
  n_updates = 0;
  fade_value = 1.0f;
  fade_in_end = 0.0;
  fade_out_start = 0.0f;
  in_scope = true;
  is_aborted = false;
  do_fades = false;
  full_lifetime = 0;

  time_factor = 1.0f;
  prop_time_factor = 1.0f;

  live_scale_factor = 1.0f;
  live_fade_factor = 1.0f;

  //tested_exec_conds = 3;

  dMemset(xfm_modifiers, 0, sizeof(xfm_modifiers));
}

afxEffectWrapper::~afxEffectWrapper()
{
  for (S32 i = 0; i < MAX_XFM_MODIFIERS; i++)
    if (xfm_modifiers[i])
      delete xfm_modifiers[i];

  if (datablock && datablock->effect_name != ST_NULLSTRING)
  {
    choreographer->removeNamedEffect(this);
    if (datablock->use_as_cons_obj && !effect_cons_id.undefined())
      cons_mgr->setReferenceEffect(effect_cons_id, 0);
  }
}

#undef myOffset
#define myOffset(field) Offset(field, afxEffectWrapper)

void afxEffectWrapper::initPersistFields()
{
  Parent::initPersistFields();

  addField("liveScaleFactor",     TypeF32,    myOffset(live_scale_factor));
  addField("liveFadeFactor",      TypeF32,    myOffset(live_fade_factor));
}

void afxEffectWrapper::ew_init(afxChoreographer*     choreographer, 
                               afxEffectWrapperData* datablock, 
                               afxConstraintMgr*     cons_mgr,
                               F32                   time_factor)
{
  AssertFatal(choreographer != NULL, "Choreographer is missing.");
  AssertFatal(datablock != NULL, "Datablock is missing.");
  AssertFatal(cons_mgr != NULL, "Constraint manager is missing.");

  this->choreographer = choreographer;
  this->datablock = datablock;
  this->cons_mgr = cons_mgr;
  ea_set_datablock(datablock->effect_data);

  ew_timing = datablock->timing;

  pos_cons_id = cons_mgr->getConstraintId(datablock->pos_cons_def);
  orient_cons_id = cons_mgr->getConstraintId(datablock->orient_cons_def);
  aim_cons_id = cons_mgr->getConstraintId(datablock->aim_cons_def);
  life_cons_id = cons_mgr->getConstraintId(datablock->life_cons_def);

  this->time_factor = (datablock->ignore_time_factor) ? 1.0f : time_factor;

  if (datablock->propagate_time_factor)
    prop_time_factor = time_factor;

  for (int i = 0; i < MAX_XFM_MODIFIERS && datablock->xfm_modifiers[i] != 0; i++)
    xfm_modifiers[i] = datablock->xfm_modifiers[i]->create(this, choreographer->isServerObject());

  if (datablock->effect_name != ST_NULLSTRING)
  {
    assignName(datablock->effect_name);
    choreographer->addNamedEffect(this);
    if (datablock->use_as_cons_obj)
      effect_cons_id = cons_mgr->setReferenceEffect(datablock->effect_name, this);
  }
}

void afxEffectWrapper::prestart() 
{
  // modify timing values by time_factor
  if (ew_timing.lifetime > 0)
    ew_timing.lifetime *= time_factor;
  ew_timing.delay *= time_factor;
  ew_timing.fade_in_time *= time_factor;
  ew_timing.fade_out_time *= time_factor;

  if (ew_timing.lifetime < 0)
  {
    full_lifetime = INFINITE_LIFETIME;
    life_end = INFINITE_LIFETIME;
  }
  else
  {
    full_lifetime = ew_timing.lifetime + ew_timing.fade_out_time;
    life_end = ew_timing.delay + ew_timing.lifetime;
  }

  if ((ew_timing.fade_in_time + ew_timing.fade_out_time) > 0.0f)
  {
    fade_in_end = ew_timing.delay + ew_timing.fade_in_time;
    if (full_lifetime == INFINITE_LIFETIME)
      fade_out_start = INFINITE_LIFETIME;
    else
      fade_out_start = ew_timing.delay + ew_timing.lifetime;
    do_fades = true;
  }
}

bool afxEffectWrapper::start(F32 timestamp) 
{ 
  afxConstraint* life_constraint = getLifeConstraint();
  if (life_constraint)
    cond_alive = life_constraint->getLivingState();

  elapsed = timestamp; 

  for (S32 i = 0; i < MAX_XFM_MODIFIERS; i++)
  {
    if (!datablock->xfm_modifiers[i])
      break;
    else
      xfm_modifiers[i]->start(timestamp);
  }

  if (!ea_start())
  {
    Con::errorf("afxEffectWrapper::start() -- effect %s failed to start.", datablock->getName());
    return false;
  }

  update(0.0f);
  return !isAborted();
}

bool afxEffectWrapper::test_life_conds()
{
  afxConstraint* life_constraint = getLifeConstraint();
  if (!life_constraint || datablock->life_conds == 0)
    return true;

  S32 now_state = life_constraint->getDamageState();
  if ((datablock->life_conds & DEAD) != 0 && now_state == ShapeBase::Disabled)
    return true;
  if ((datablock->life_conds & ALIVE) != 0 && now_state == ShapeBase::Enabled)
    return true;
  if ((datablock->life_conds & DYING) != 0)
    return (cond_alive && now_state == ShapeBase::Disabled);

  return false;
}

bool afxEffectWrapper::update(F32 dt) 
{ 
  elapsed += dt; 

  // life_elapsed won't exceed full_lifetime
  life_elapsed = getMin(elapsed - ew_timing.delay, full_lifetime);

  // update() returns early if elapsed is outside of active timing range 
  //     (delay <= elapsed <= delay+lifetime)
  // note: execution is always allowed beyond this point at least once, 
  //       even if elapsed exceeds the lifetime.
  if (elapsed < ew_timing.delay)
  {
    setScopeStatus(false);
    return false;
  }
  
  if (!datablock->requiresStop() && ew_timing.lifetime < 0)
  {
    F32 afterlife = elapsed - ew_timing.delay;
    if (afterlife > 1.0f || ((afterlife > 0.0f) && (n_updates > 0)))
    {
      setScopeStatus(ew_timing.residue_lifetime > 0.0f);
      return false;
    }
  }
  else
  {
    F32 afterlife = elapsed - (full_lifetime + ew_timing.delay);
    if (afterlife > 1.0f || ((afterlife > 0.0f) && (n_updates > 0)))
    {
      setScopeStatus(ew_timing.residue_lifetime > 0.0f);
      return false;
    }
  }

  // first time here, test if required conditions for effect are met
  if (n_updates == 0)
  {
    if (!test_life_conds())
    {
      elapsed = full_lifetime + ew_timing.delay;
      setScopeStatus(false);
      n_updates++;
      return false;
    }
  }

  setScopeStatus(true);
  n_updates++;


  // calculate current fade value if enabled
  if (do_fades)
  {
    if (ew_timing.fade_in_time > 0 && elapsed <= fade_in_end)
      fade_value = mClampF((elapsed-ew_timing.delay)/ew_timing.fade_in_time, 0.0f, 1.0f);
    else if (elapsed > fade_out_start)
    {
      if (ew_timing.fade_out_time == 0)
        fade_value = 0.0f;
      else
        fade_value = mClampF(1.0f-(elapsed-fade_out_start)/ew_timing.fade_out_time, 0.0f, 1.0f);
    }
    else
      fade_value = 1.0f;
  }

  // DEAL WITH CONSTRAINTS

  // CONS_POS is the initial constrained position
  afxConstraint* pos_constraint = getPosConstraint();
  Point3F CONS_POS;
  if (pos_constraint)
  {
    bool valid = pos_constraint->getPosition(CONS_POS, datablock->pos_cons_def.history_time);
    setScopeStatus(valid);
  }
  else
  {
    getUnconstrainedPosition(CONS_POS);
  }

  // CONS_XFM is the initial constrained orientation
  afxConstraint* orient_constraint = getOrientConstraint();
  MatrixF CONS_XFM;  
  if (orient_constraint) 
  {
    orient_constraint->getTransform(CONS_XFM, datablock->pos_cons_def.history_time);
  }
  else
  {
    getUnconstrainedTransform(CONS_XFM);
  }

  // CONS_AIM is the initial aim constraint position
  afxConstraint* aim_constraint = getAimConstraint();
  Point3F CONS_AIM;
  if (aim_constraint)
    aim_constraint->getPosition(CONS_AIM, datablock->pos_cons_def.history_time);
  else
    CONS_AIM.zero();

  Point3F SCALE;
  SCALE.set(datablock->scale_factor, datablock->scale_factor, datablock->scale_factor);

  // apply modifiers
  for (int i = 0; i < MAX_XFM_MODIFIERS; i++)
  {
    if (!xfm_modifiers[i])
      break;
    else
      xfm_modifiers[i]->update(dt, life_elapsed, CONS_POS, CONS_XFM, CONS_AIM, SCALE);
  }

  // final pos/orient is determined
  updated_xfm = CONS_XFM;  
  updated_pos = CONS_POS;
  updated_aim = CONS_AIM;
  updated_xfm.setPosition(updated_pos);
  updated_scale = SCALE;

  //if (pos_constraint->getTriggerState(1,false))
  //  Con::printf("TRIGGERED");

  if (!ea_update(dt))
  {
    is_aborted = true;
    Con::errorf("afxEffectWrapper::update() -- effect %s ended unexpectedly.", datablock->getName());
  }

  return true;
}

void afxEffectWrapper::stop() 
{ 
  if (!datablock->requiresStop())
    return;

  stopped = true; 

  // this resets full_lifetime so it starts to shrink or fade
  if (full_lifetime == INFINITE_LIFETIME)
  {
    full_lifetime = (elapsed - ew_timing.delay) + afterStopTime();
    life_end = elapsed; 
    if (ew_timing.fade_out_time > 0)
      fade_out_start = elapsed;
  }
}

void afxEffectWrapper::cleanup(bool was_stopped)
{ 
  ea_finish(was_stopped);
  if (!effect_cons_id.undefined())
  {
    cons_mgr->setReferenceEffect(effect_cons_id, 0);
    effect_cons_id = afxConstraintID();
  }
}

void afxEffectWrapper::setScopeStatus(bool in_scope)
{ 
  if (this->in_scope != in_scope)
  {
    this->in_scope = in_scope;
    ea_set_scope_status(in_scope);
  }
}

bool afxEffectWrapper::isDone() 
{ 
  if (!datablock->is_looping)
    return (elapsed >= (life_end + ew_timing.fade_out_time));

  return false;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// static 
afxEffectWrapper* afxEffectWrapper::ew_create(afxChoreographer*      choreographer, 
                                              afxEffectWrapperData*  datablock, 
                                              afxConstraintMgr*      cons_mgr,
                                              F32                    time_factor)
{
  afxEffectWrapper* adapter = datablock->effect_desc->create();

  if (adapter)
    adapter->ew_init(choreographer, datablock, cons_mgr, time_factor); 

  return adapter;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

Vector<afxEffectAdapterDesc*>* afxEffectAdapterDesc::adapters = 0;

afxEffectAdapterDesc::afxEffectAdapterDesc() 
{ 
  if (!adapters)
    adapters = new Vector<afxEffectAdapterDesc*>;

  adapters->push_back(this);
}

bool afxEffectAdapterDesc::identifyEffect(afxEffectWrapperData* ew)
{
  if (!ew || !ew->effect_data)
  {
    Con::errorf("afxEffectAdapterDesc::identifyEffect() -- effect datablock was not specified.");
    return false;
  }

  if (!adapters)
  {
    Con::errorf("afxEffectAdapterDesc::identifyEffect() -- adapter registration list has not been allocated.");
    return false;
  }

  if (adapters->size() == 0)
  {
    Con::errorf("afxEffectAdapterDesc::identifyEffect() -- no effect adapters have been registered.");
    return false;
  }

  for (S32 i = 0; i < adapters->size(); i++)
  {
    if ((*adapters)[i]->testEffectType(ew->effect_data))
    {
      ew->effect_desc = (*adapters)[i];
      (*adapters)[i]->prepEffect(ew);
      return true;
    }
  }

  Con::errorf("afxEffectAdapterDesc::identifyEffect() -- effect %s has an undefined type. -- %d", 
    ew->effect_data->getName(), adapters->size());
  return false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
