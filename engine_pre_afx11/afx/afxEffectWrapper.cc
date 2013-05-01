 
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/consoleTypes.h"
#include "console/simBase.h"
#include "game/gameBase.h"
#include "game/staticShape.h"
#include "game/fx/particleEngine.h"
#include "game/fx/explosion.h"
#include "game/debris.h"
#include "audio/audio.h"
#include "math/mathIO.h"
#include "audio/audioBuffer.h"
#include "audio/audioDataBlock.h"

#include "afx/ce/afxZodiac.h"
#include "afx/ce/afxModel.h"
#include "afx/ce/afxStaticShape.h"
#include "afx/ce/afxLight.h"
#include "afx/ce/afxAnimClip.h"
#include "afx/ce/afxAnimLock.h"
#include "afx/ce/afxDamage.h"
#include "afx/ce/afxMachineGun.h"
#include "afx/ce/afxMooring.h"
#include "afx/ce/afxScriptEvent.h"
#include "afx/ce/afxCameraShake.h"
#include "afx/ce/afxProjectile.h"
#include "afx/afxResidueMgr.h"
#include "afx/afxChoreographer.h"
#include "afx/afxConstraint.h"
#include "afx/xm/afxXfmMod.h"
#include "afx/afxEffectWrapper.h"
#include "afx/afxEffectAdapters.h"

#ifdef AFX_TLK_SUPPORT_ENABLED
#ifdef AFX_TLK_1_4_SUPPORT_ENABLED
#include "synapseGaming/contentPacks/lightingPack/sgUniversalStaticLight.h"
#include "game/fx/volLight.h"
#else
#include "lightingSystem/sgLightObject.h"
#include "lightingSystem/volLight.h"
#endif
#include "afx/ce/afxVolumeLight.h"
#endif

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
  effect_type = UNKNOWN;
  effect = 0;
  data_ID = 0;
  effect_data.simobject = 0;
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
}

#define myOffset(field) Offset(field, afxEffectWrapperData)

void afxEffectWrapperData::initPersistFields()
{
  Parent::initPersistFields();

#ifdef AFX_TGE_1_3
  Con::registerType("AfxEffectWrapperDataPtr", TypeafxEffectWrapperDataPtr, sizeof(afxEffectWrapperData*),
                    REF_GETDATATYPE(afxEffectWrapperData), REF_SETDATATYPE(afxEffectWrapperData));

  Con::registerType("SimDataBlockPtr", TypeSimDataBlockPtr, sizeof(SimDataBlock*),
                    REF_GETDATATYPE(SimDataBlock), REF_SETDATATYPE(SimDataBlock));

  Con::registerType("ByteRange", TypeByteRange, sizeof(ByteRange),
                    getDataTypeByteRange, setDataTypeByteRange);
#endif

  // the wrapped effect
  addField("effect",            TypeSimDataBlockPtr,  myOffset(effect));
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

  // Conditional Execution Flags
  Con::setIntVariable("$afx::DISABLED", DISABLED);
  Con::setIntVariable("$afx::ENABLED", ENABLED);
  Con::setIntVariable("$afx::FAILING", FAILING);
  Con::setIntVariable("$afx::DEAD", DEAD);
  Con::setIntVariable("$afx::ALIVE", ALIVE);
  Con::setIntVariable("$afx::DYING", DYING);
}

extern ALuint alxGetWaveLen(ALuint buffer);

// use dynamic casts to identify what type of effect data we have
void afxEffectWrapperData::identify_effect()
{
  if (!effect) 
    return;

  if (dynamic_cast<ParticleEmitterData*>(effect))
  {
    effect_data.emitter = (ParticleEmitterData*) effect;
    effect_type = EMITTER;
  }
  else if (dynamic_cast<ExplosionData*>(effect))
  {
    effect_data.explosion = (ExplosionData*) effect;
    effect_type = EXPLOSION;
  }
  else if (dynamic_cast<DebrisData*>(effect))
  {
    effect_data.debris = (DebrisData*) effect;
    effect_type = DEBRIS;
  }
  else if (dynamic_cast<ProjectileData*>(effect))
  {
    effect_data.projectile = (ProjectileData*) effect;
    effect_type = PROJECTILE;
  }
  else if (dynamic_cast<afxMachineGunData*>(effect))
  {
    effect_data.machine_gun = (afxMachineGunData*) effect;
    effect_type = MACHINE_GUN;
  }
  else if (dynamic_cast<afxMooringData*>(effect))
  {
    effect_data.mooring = (afxMooringData*) effect;
    effect_type = MOORING;
  }
  else if (dynamic_cast<afxZodiacData*>(effect))
  {
    effect_data.zodiac = (afxZodiacData*) effect;
    effect_type = ZODIAC;
  }
  else if (dynamic_cast<afxModelData*>(effect))
  {
    effect_data.model = (afxModelData*) effect;
    effect_type = MODEL;
  }
  else if (dynamic_cast<StaticShapeData*>(effect))
  {
    effect_data.static_shape = (StaticShapeData*) effect;
    effect_type = STATIC_SHAPE;
  }
  else if (dynamic_cast<afxLightData*>(effect))
  {
    effect_data.light = (afxLightData*) effect;
    effect_type = LIGHT;
  }
  else if (dynamic_cast<afxAnimClipData*>(effect))
  {
    effect_data.anim_clip = (afxAnimClipData*) effect;
    effect_type = ANIM_CLIP;
  }
  else if (dynamic_cast<afxAnimLockData*>(effect))
  {
    effect_data.anim_lock = (afxAnimLockData*) effect;
    effect_type = ANIM_LOCK;
  }
  else if (dynamic_cast<afxDamageData*>(effect))
  {
    effect_data.damage = (afxDamageData*) effect;
    effect_type = DAMAGE;
  }
  else if (dynamic_cast<afxScriptEventData*>(effect))
  {
    effect_data.script_event = (afxScriptEventData*) effect;
    effect_type = SCRIPT_EVENT;
  }
  else if (dynamic_cast<afxCameraShakeData*>(effect))
  {
    effect_data.camshake = (afxCameraShakeData*) effect;
    effect_type = CAMERA_SHAKE;
  }
  else if (dynamic_cast<AudioProfile*>(effect))
  {
    effect_data.sound = (AudioProfile*) effect;
    effect_type = SOUND;

    if (timing.lifetime < 0)
    {
      AudioProfile* snd = effect_data.sound;
      if (snd->mDescriptionObject && !snd->mDescriptionObject->mDescription.mIsLooping)
      {
        static bool test_for_audio = true;
        static bool can_get_audio_len = false;

        if (test_for_audio)
        {
          can_get_audio_len = (ResourceManager->getCreateFunction(".ogg") != NULL);
          test_for_audio = false;
        }

        if (can_get_audio_len)
        {
          Resource<AudioBuffer> mBuffer = AudioBuffer::find(snd->mFilename);
          if (bool(mBuffer))
          {
            timing.lifetime = 0.001f*alxGetWaveLen(mBuffer->getALBuffer());
            //Con::printf("SFX (%s) duration=%g", snd->mFilename, timing.lifetime);
          }
        }
        else
        {
          timing.lifetime = 0;
          Con::printf("afxEffectWrapperData -- cannot get audio length from file, (%s).", snd->mFilename);
        }
      }
    }
  }
#ifdef AFX_TLK_SUPPORT_ENABLED
  else if (dynamic_cast<afxVolumeLightData*>(effect))
  {
    effect_data.volume_light = (afxVolumeLightData*) effect;
    effect_type = VOLUME_LIGHT;
  }
  else if (dynamic_cast<sgLightObjectData*>(effect))
  {
    effect_data.tlk_light = (sgLightObjectData*) effect;
    effect_type = TLK_LIGHT;
  }
#endif
  // IDENTIFY NEW COMPONENT EFFECT TYPES HERE //
  else
    Con::printf("afxEffectWrapperData::onAdd() -- unknown effect type.");
}

bool afxEffectWrapperData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;

  identify_effect();

  if (!effect_data.simobject)
  {
    if (!Sim::findObject((SimObjectId)data_ID, effect_data.simobject))
    {
      Con::errorf(ConsoleLogEntry::General,
        "afxEffectWrapperData::onAdd() -- bad datablockId: 0x%x", data_ID);
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

  stream->write(effect_type);
  writeDatablockID(stream, effect_data.simobject, packed);

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
}

void afxEffectWrapperData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  stream->read(&effect_type);
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
  if(!Parent::preload(server, errorBuffer))
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
            Con::errorf(ConsoleLogEntry::General,
              "afxEffectWrapperData::preload() -- bad datablockId: 0x%x (xfm_modifiers[%d])",
              db_id, i);
          }
        }
        do_id_convert = false;
      }
    }
  }
  
  return true;
}

void afxEffectWrapperData::getConstraintNames(StringTableEntry names[8], U32* count)
{
  *count = 0;

  if (pos_cons_def.isArbitraryObject())
    names[(*count)++] = pos_cons_def.cons_src_name;
  if (orient_cons_def.isArbitraryObject())
    names[(*count)++] = orient_cons_def.cons_src_name;
  if (aim_cons_def.isArbitraryObject())
    names[(*count)++] = aim_cons_def.cons_src_name;
  if (life_cons_def.isArbitraryObject())
    names[(*count)++] = life_cons_def.cons_src_name;
}

bool afxEffectWrapperData::requiresStop() 
{ 
  switch (effect_type)
  {
  case EXPLOSION:
  case DEBRIS:
  case PROJECTILE:
  case SCRIPT_EVENT:
  case DAMAGE:
    return false;
  case SOUND:
    {
      AudioDescription* ad = effect_data.sound->mDescriptionObject;
      return (ad && ad->mDescription.mIsLooping) ? (timing.lifetime < 0) : false;
    }
  case ZODIAC:
  case EMITTER:
  case MODEL:
  case STATIC_SHAPE:
  case LIGHT:
  case ANIM_CLIP:
  case ANIM_LOCK:
  case CAMERA_SHAKE:
  case MACHINE_GUN:
  case TLK_LIGHT:
  case MOORING:
  case VOLUME_LIGHT:
  default:
    return (timing.lifetime < 0);
  }
}

bool afxEffectWrapperData::runsOnServer() 
{ 
  switch (effect_type)
  {
  case MOORING:
    return ((effect_data.mooring->networking & CLIENT_ONLY) == 0);
  case ANIM_CLIP:
  case ANIM_LOCK:
  case DAMAGE:
  case SCRIPT_EVENT:
  case STATIC_SHAPE:
  case PROJECTILE:
  case MACHINE_GUN:
    return true;
  case EXPLOSION:
  case EMITTER:
  case ZODIAC:
  case LIGHT:
  case TLK_LIGHT:
  case VOLUME_LIGHT:
  case SOUND:
  case CAMERA_SHAKE:
  case MODEL:
  case DEBRIS:
  default:
    return false;
  }
}

bool afxEffectWrapperData::runsOnClient() 
{ 
  switch (effect_type)
  {
  case MOORING:
    return ((effect_data.mooring->networking & CLIENT_ONLY) != 0);
  case EXPLOSION:
  case EMITTER:
  case ZODIAC:
  case LIGHT:
  case TLK_LIGHT:
  case VOLUME_LIGHT:
  case SOUND:
  case ANIM_CLIP:
  case ANIM_LOCK:
  case CAMERA_SHAKE:
  case MODEL:
  case DEBRIS:
    return true;
  case STATIC_SHAPE:
  case PROJECTILE:
  case MACHINE_GUN:
  case SCRIPT_EVENT:
  case DAMAGE:
  default:
    return false;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEffectWrapper

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
  do_fades = false;
  full_lifetime = 0;

  time_factor = 1.0f;
  prop_time_factor = 1.0f;

  //tested_exec_conds = 3;

  dMemset(xfm_modifiers, 0, sizeof(xfm_modifiers));
}

afxEffectWrapper::~afxEffectWrapper()
{
  for (S32 i = 0; i < MAX_XFM_MODIFIERS; i++)
    if (xfm_modifiers[i])
      delete xfm_modifiers[i];

  if (datablock->effect_name != ST_NULLSTRING)
  {
    choreographer->removeNamedEffect(this);
    if (datablock->use_as_cons_obj && !effect_cons_id.undefined())
      cons_mgr->setReferenceEffect(effect_cons_id, 0);
  }
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

  ew_timing = datablock->timing;

  pos_cons_id = cons_mgr->getConstraintId(datablock->pos_cons_def);
  orient_cons_id = cons_mgr->getConstraintId(datablock->orient_cons_def);
  aim_cons_id = cons_mgr->getConstraintId(datablock->aim_cons_def);
  life_cons_id = cons_mgr->getConstraintId(datablock->life_cons_def);

  this->time_factor = (datablock->ignore_time_factor) ? 1.0f : time_factor;

  if (datablock->propagate_time_factor)
    prop_time_factor = time_factor;

  for (int i = 0; i < MAX_XFM_MODIFIERS && datablock->xfm_modifiers[i] != 0; i++)
    xfm_modifiers[i] = datablock->xfm_modifiers[i]->create(this);

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

void afxEffectWrapper::start(F32 timestamp) 
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

  ea_start();
  update(0.0f);
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
    CONS_POS.zero();

  // CONS_XFM is the initial constrained orientation
  afxConstraint* orient_constraint = getOrientConstraint();
  MatrixF CONS_XFM;  
  if (orient_constraint)     
    orient_constraint->getTransform(CONS_XFM, datablock->pos_cons_def.history_time);
  else
    CONS_XFM.identity();

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

  return ea_update(dt);
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
  afxEffectWrapper* adapter = 0;

  switch (datablock->effect_type)
  {
  case MODEL:
    adapter = new afxEA_Model;
    break;
  case STATIC_SHAPE:
    adapter = new afxEA_StaticShape;
    break;
  case EMITTER:
    adapter = new afxEA_ParticleEmitter;
    break;
  case EXPLOSION:
    adapter = new afxEA_Explosion;
    break;
  case ZODIAC: 
    adapter = new afxEA_Zodiac;
    break;    
  case LIGHT:
    adapter = new afxEA_Light;
    break;
  case ANIM_CLIP:
    adapter = new afxEA_AnimClip;
    break;
  case ANIM_LOCK:
    adapter = new afxEA_AnimLock;
    break;
  case DAMAGE:
    adapter = new afxEA_Damage;
    break;
  case SOUND:
    adapter = new afxEA_Sound;
    break;
  case SCRIPT_EVENT:
    adapter = new afxEA_ScriptEvent;
    break;
  case CAMERA_SHAKE:
    adapter = new afxEA_CameraShake;
    break;
  case DEBRIS:
    adapter = new afxEA_Debris;
    break;
  case PROJECTILE:
    adapter = new afxEA_Projectile;
    break;
  case MACHINE_GUN:
    adapter = new afxEA_MachineGun;
    break;
  case MOORING:
    adapter = new afxEA_Mooring;
    break;
  case TLK_LIGHT:
#ifdef AFX_TLK_SUPPORT_ENABLED
    adapter = new afxEA_TLKLight;
#endif
    break;
  case VOLUME_LIGHT:
#ifdef AFX_TLK_SUPPORT_ENABLED
    adapter = new afxEA_VolumeLight;
#endif
    break;
  // CREATE NEW COMPONENT EFFECT ADAPTERS HERE //
  }

  if (adapter)
    adapter->ew_init(choreographer, datablock, cons_mgr, time_factor); 

  return adapter;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//


