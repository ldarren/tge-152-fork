 
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "game/staticShape.h"
#include "game/gameBase.h"
#include "game/fx/particleEngine.h"
#include "game/fx/explosion.h"
#include "game/debris.h"
#include "game/fx/cameraFXMgr.h"
#include "math/mathUtils.h"
#include "terrain/terrRender.h"
#include "terrain/sun.h"

#include "afx/util/afxEase.h"
#include "afx/ce/afxAnimClip.h"
#include "afx/ce/afxDamage.h"
#include "afx/ce/afxLight.h"
#include "afx/ce/afxModel.h"
#include "afx/ce/afxProjectile.h"
#include "afx/ce/afxMachineGun.h"
#include "afx/ce/afxStaticShape.h"
#include "afx/ce/afxScriptEvent.h"
#include "afx/ce/afxMooring.h"
#include "afx/ce/afxCameraShake.h"
#include "afx/ce/afxZodiac.h"
#include "afx/afxChoreographer.h"
#include "afx/afxResidueMgr.h"
#include "afx/afxConstraint.h"
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
// afxEA_ParticleEmitter

afxEA_ParticleEmitter::afxEA_ParticleEmitter()
{
  //effect_flags = REQURES_STOP | RUNS_ON_SERVER | RUNS_ON_CLIENT;
  emitter = 0;
  do_bbox_update = false;
}

afxEA_ParticleEmitter::~afxEA_ParticleEmitter()
{
  if (emitter)
    emitter->deleteWhenEmpty();
}

void afxEA_ParticleEmitter::ea_start()
{
  if (datablock->effect_data.emitter)
  {
    emitter = new ParticleEmitter();
    emitter->onNewDataBlock(datablock->effect_data.emitter);
    if (!emitter->registerObject())
    {
      delete emitter;
      emitter = NULL;
      Con::errorf(ConsoleLogEntry::General, "ParticleEmitter effect failed to register. (%s)", datablock->getName());
      return;
    }

#ifndef AFX_TGE_1_3
    if (datablock->forced_bbox.isValidBox())
    {
      do_bbox_update = true;
    }
#endif
  }
}

bool afxEA_ParticleEmitter::ea_update(F32 dt)
{
  if (emitter && in_scope)
  {
#ifndef AFX_TGE_1_3
    if (do_bbox_update)
    {
      Box3F bbox = emitter->getObjBox();

      bbox.min = updated_pos + datablock->forced_bbox.min; 
      bbox.max = updated_pos + datablock->forced_bbox.max; 
      emitter->setForcedObjBox(bbox);
      emitter->setTransform(emitter->getTransform());

      if (!datablock->update_forced_bbox)
        do_bbox_update = false;
    }
#endif

    if (do_fades)
      emitter->setFadeAmount(fade_value);
    
    emitter->emitParticlesExt(updated_xfm, updated_pos, Point3F(0.0,0.0,0.0), (U32)(dt*1000));
  }

  return true;
}

void afxEA_ParticleEmitter::ea_finish(bool was_stopped)
{
  if (emitter)
  {
    // make sure particles are fully faded.
    //   note - fully faded particles are not always
    //     invisible, so they are still kept alive and 
    //     deleted via deleteWhenEmpty().
    if (do_fades)
      emitter->setFadeAmount(0.0f);
    emitter->deleteWhenEmpty();
    emitter = 0;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Explosion

afxEA_Explosion::afxEA_Explosion()
{
  explosion = 0;
  exploded = false;
}

afxEA_Explosion::~afxEA_Explosion()
{
}

void afxEA_Explosion::ea_start()
{
  if (datablock->effect_data.explosion)
  {
    explosion = new Explosion();
    explosion->onNewDataBlock(datablock->effect_data.explosion);
  }
}

bool afxEA_Explosion::ea_update(F32 dt)
{
  if (!exploded && explosion)
  {
    if (in_scope)
    {
      explosion->setInitialState(updated_pos, Point3F(0,0,1));
      if (!explosion->registerObject())
      {
        delete explosion;
        explosion = 0;
      }
    }
    exploded = true;
  }

  return true;
}

void afxEA_Explosion::ea_finish(bool was_stopped)
{
  explosion = 0;
  exploded = false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Debris

afxEA_Debris::afxEA_Debris()
{
  debris = 0;
  exploded = false;
}

afxEA_Debris::~afxEA_Debris()
{
}

void afxEA_Debris::ea_start()
{
  if (datablock->effect_data.debris)
  {
    debris = new Debris();
    debris->onNewDataBlock(datablock->effect_data.debris);
  }
}

bool afxEA_Debris::ea_update(F32 dt)
{
  if (!exploded && debris)
  {
    if (in_scope)
    {
      //Point3F dir_vec = updated_aim - updated_pos;
      //dir_vec.normalizeSafe();
      
      Point3F dir_vec(0,1,0);
      updated_xfm.mulV(dir_vec);

      debris->init(updated_pos, dir_vec);
      if (!debris->registerObject())
      {
        delete debris;
        debris = 0;
      }
    }
    exploded = true;
  }

  return true;
}

void afxEA_Debris::ea_finish(bool was_stopped)
{
  debris = 0;
  exploded = false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Projectile

/*
   // Create the projectile object
   %p = new (%this.projectileType)() {
      dataBlock        = %projectile;
      initialVelocity  = %muzzleVelocity;
      initialPosition  = %obj.getMuzzlePoint(%slot);
      sourceObject     = %obj;
      sourceSlot       = %slot;
      client           = %obj.client;
   };
*/

afxEA_Projectile::afxEA_Projectile()
{
  projectile = 0;
  launched = false;
  impacted = false;
}

afxEA_Projectile::~afxEA_Projectile()
{
}

void afxEA_Projectile::ea_start()
{
  if (datablock->effect_data.projectile)
  {
    projectile = new afxProjectile();
    projectile->onNewDataBlock(datablock->effect_data.projectile);
  }
}

bool afxEA_Projectile::ea_update(F32 dt)
{
  if (!launched && projectile)
  {
    if (in_scope)
    {
      afxConstraint* pos_cons = getPosConstraint();
      ShapeBase* src_obj = (pos_cons) ? (dynamic_cast<ShapeBase*>(pos_cons->getObject())) : 0;

      F32 muzzle_vel = datablock->effect_data.projectile->muzzleVelocity;

      Point3F dir_vec = updated_aim - updated_pos;
      dir_vec.normalizeSafe();
      dir_vec *= muzzle_vel;
      projectile->init(updated_pos, dir_vec, src_obj);
      if (!projectile->registerObject())
      {
        delete projectile;
        projectile = 0;
      }
      if (projectile)
        projectile->setDataField(StringTable->insert("afxOwner"), 0, choreographer->scriptThis());
    }
    launched = true;
  }

  return true;
}

void afxEA_Projectile::ea_finish(bool was_stopped)
{
  projectile = 0;
  launched = false;
  impacted = false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_MachineGun

afxEA_MachineGun::afxEA_MachineGun()
{
  shooting = false;
  start_time = 0.0f;
  shot_count = 0;
  shot_gap = 0.2f;
}

afxEA_MachineGun::~afxEA_MachineGun()
{
}

void afxEA_MachineGun::ea_start()
{
  if (datablock->effect_data.machine_gun->rounds_per_minute > 0)
    shot_gap = 60.0f/datablock->effect_data.machine_gun->rounds_per_minute;
}

bool afxEA_MachineGun::ea_update(F32 dt)
{
  if (!shooting)
  {
    start_time = elapsed;
    shooting = true;
  }
  else
  {
    F32 next_shot = start_time + (shot_count+1)*shot_gap;
    while (next_shot < elapsed)
    {
      if (in_scope)
        launch_projectile();
      next_shot += shot_gap;
      shot_count++;
    }
  }

  return true;
}

void afxEA_MachineGun::ea_finish(bool was_stopped)
{
}

void afxEA_MachineGun::launch_projectile()
{
  afxProjectile* projectile = new afxProjectile();
  projectile->onNewDataBlock(datablock->effect_data.machine_gun->projectile_data);

  F32 muzzle_vel = datablock->effect_data.machine_gun->projectile_data->muzzleVelocity;

  afxConstraint* pos_cons = getPosConstraint();
  ShapeBase* src_obj = (pos_cons) ? (dynamic_cast<ShapeBase*>(pos_cons->getObject())) : 0;

  Point3F dir_vec = updated_aim - updated_pos;
  dir_vec.normalizeSafe();
  dir_vec *= muzzle_vel;
  projectile->init(updated_pos, dir_vec, src_obj);
  if (!projectile->registerObject())
  {
    delete projectile;
    projectile = 0;
  }
  if (projectile)
    projectile->setDataField(StringTable->insert("afxOwner"), 0, choreographer->scriptThis());
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Mooring

afxEA_Mooring::afxEA_Mooring()
{
  obj = 0;
}

afxEA_Mooring::~afxEA_Mooring()
{
  if (obj)
    obj->deleteObject();
}

void afxEA_Mooring::ea_start()
{
}

bool afxEA_Mooring::ea_update(F32 dt)
{
  if (!obj && datablock->effect_data.mooring)
  {
    if (datablock->use_ghost_as_cons_obj && datablock->effect_name != ST_NULLSTRING)
    {
      obj = new afxMooring(datablock->effect_data.mooring->networking, 
                           choreographer->getChoreographerId(), 
                           datablock->effect_name);
    }
    else
    {
      obj = new afxMooring(datablock->effect_data.mooring->networking, 0, ST_NULLSTRING);
    }

    obj->onNewDataBlock(datablock->effect_data.mooring);
    if (!obj->registerObject())
    {
      delete obj;
      obj = 0;
      return false;
    }
    deleteNotify(obj);
  }

  if (obj)
  {
    obj->setTransform(updated_xfm);
    //obj->setScale(updated_scale);
  }

  return true;
}

void afxEA_Mooring::ea_finish(bool was_stopped)
{
}

void afxEA_Mooring::onDeleteNotify(SimObject* obj)
{
  if (this->obj == obj)
    obj = 0;

  Parent::onDeleteNotify(obj);
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Zodiac

F32 afxEA_Zodiac::calc_facing_angle() 
{
  // get direction player is facing
  VectorF shape_vec;
  MatrixF shape_xfm;

  afxConstraint* orient_constraint = getOrientConstraint();
  if (orient_constraint)
    orient_constraint->getTransform(shape_xfm);
  else
    shape_xfm.identity();

  shape_xfm.getColumn(1, &shape_vec);
  shape_vec.z = 0.0f;
  shape_vec.normalize();

  F32 pitch, yaw;
  MathUtils::getAnglesFromVector(shape_vec, yaw, pitch);

  return mRadToDeg(yaw); 
}

afxEA_Zodiac::afxEA_Zodiac()
{
  zode_pos.zero();
  zode_radius = 1;
  zode_color.set(1,1,1,1);
  zode_angle = 0;
  zode_angle_offset = 0;
}

afxEA_Zodiac::~afxEA_Zodiac()
{
}

void afxEA_Zodiac::ea_start()
{
  zode_angle_offset = calc_facing_angle();
}

bool afxEA_Zodiac::ea_update(F32 dt)
{
  afxZodiacData* zode = datablock->effect_data.zodiac;

  if (!in_scope)
    return false;

  // UPDATE POSITION
  zode_pos = updated_pos;

  // UPDATE ROTATION ANGLE
  afxConstraint* orient_constraint = getOrientConstraint();
  if (orient_constraint && zode->respect_ori_cons)
  {
    VectorF shape_vec;
    updated_xfm.getColumn(1, &shape_vec);
    shape_vec.z = 0.0f;
    shape_vec.normalize();
    F32 pitch, yaw;
    MathUtils::getAnglesFromVector(shape_vec, yaw, pitch);
    zode_angle_offset = mRadToDeg(yaw); 
  }

  zode_angle = zode->calcRotationAngle(life_elapsed, datablock->rate_factor/prop_time_factor);
  zode_angle = mFmod(zode_angle + zode_angle_offset, 360.0f);     

  // UPDATE SIZE
  zode_radius = zode->radius*updated_scale.x + life_elapsed*zode->growth_rate;

  // zode is growing
  if (life_elapsed < zode->grow_in_time)
    zode_radius = afxEase::eq(life_elapsed/zode->grow_in_time, 0.001f, zode_radius, 0.2f, 0.8f);

  // zode is shrinking
  else if (full_lifetime - life_elapsed < zode->shrink_out_time)
    zode_radius = afxEase::eq((full_lifetime - life_elapsed)/zode->shrink_out_time, 0.001f, zode_radius, 0.0f, 0.9f);

  // ADD ZODIAC TO TERRAIN
  zode_color = zode->color;
  if (do_fades)
  {
    if (fade_value < 0.01f)
      return false;

    if (zode->blend_flags == afxZodiacDefs::BLEND_SUBTRACTIVE)
       zode_color *= fade_value;
    else
       zode_color.alpha *= fade_value;
  }

  afxZodiacMgr::addZodiac(zode_pos, zode_radius, zode_color, zode_angle, &zode->txr, zode->zflags);

  return true;
}

void afxEA_Zodiac::ea_finish(bool was_stopped)
{
  if (in_scope && ew_timing.residue_lifetime > 0)
  {
    if (do_fades)
    {
      if (fade_value < 0.01f)
        return;
      zode_color.alpha *= fade_value;
    }
    afxZodiacData* zode = datablock->effect_data.zodiac;
    afxResidueMgr::add(ew_timing.residue_lifetime, ew_timing.residue_fadetime, zode,
                       zode_pos, zode_radius, zode_color, zode_angle);
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Model -- This is the adapter for afxModel, a lightweight animated model effect.
//

afxEA_Model::afxEA_Model()
{
  model = 0;
}

afxEA_Model::~afxEA_Model()
{
  if (model)
    model->deleteObject();
}

void afxEA_Model::ea_start()
{
}

bool afxEA_Model::ea_update(F32 dt)
{
  if (!model && datablock->effect_data.model)
  {
    // create and register effect
    model = new afxModel();
    model->onNewDataBlock(datablock->effect_data.model);
    if (!model->registerObject())
    {
      delete model;
      model = 0;
      return false;
    }
    deleteNotify(model);

    model->setSequenceRateFactor(datablock->rate_factor/prop_time_factor);
  }

  if (model)
  {
    if (do_fades)
    {
      model->setFadeAmount(fade_value);
    }
    model->setTransform(updated_xfm);
    model->setScale(updated_scale);
  }

  return true;
}

void afxEA_Model::ea_finish(bool was_stopped)
{
  if (!model)
    return;
  
  if (in_scope && ew_timing.residue_lifetime > 0)
  {
    clearNotify(model);
    afxResidueMgr::add(ew_timing.residue_lifetime, ew_timing.residue_fadetime, model);
    model = 0;
  }
  else
  {
    model->deleteObject();
    model = 0;
  }
}

void afxEA_Model::ea_set_scope_status(bool in_scope)
{
  if (model)
    model->setVisibility(in_scope);
}

void afxEA_Model::onDeleteNotify(SimObject* obj)
{
  if (model == dynamic_cast<afxModel*>(obj))
    model = 0;

  Parent::onDeleteNotify(obj);
}

void afxEA_Model::getUpdatedBoxCenter(Point3F& pos)
{
  if (model)
    pos = model->getBoxCenter();
}

TSShape* afxEA_Model::getTSShape()
{
  return (model) ? model->getTSShape() : 0;
}

TSShapeInstance* afxEA_Model::getTSShapeInstance()
{
  return (model) ? model->getTSShapeInstance() : 0;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_StaticShape

afxEA_StaticShape::afxEA_StaticShape()
{
  static_shape = 0;
  fade_out_started = false;
}

afxEA_StaticShape::~afxEA_StaticShape()
{
  if (static_shape)
    static_shape->deleteObject();
}

void afxEA_StaticShape::ea_start()
{
  // fades are handled using startFade() calls.
  do_fades = false;
}

bool afxEA_StaticShape::ea_update(F32 dt)
{
  if (!static_shape && datablock->effect_data.static_shape)
  {
    // create and register effect
    static_shape = new afxStaticShape();
    if (datablock->use_ghost_as_cons_obj && datablock->effect_name != ST_NULLSTRING)
      static_shape->init(choreographer->getChoreographerId(), datablock->effect_name);

    static_shape->onNewDataBlock(datablock->effect_data.static_shape);
    if (!static_shape->registerObject())
    {
      delete static_shape;
      static_shape = 0;
      return false;
    }
    deleteNotify(static_shape);

    if (ew_timing.fade_in_time > 0.0f)
      static_shape->startFade(ew_timing.fade_in_time, 0, false);
  }

  if (static_shape)
  {
    if (!fade_out_started && elapsed > fade_out_start)
    {
      if (ew_timing.fade_out_time > 0.0f)
        static_shape->startFade(ew_timing.fade_out_time, 0, true);
      fade_out_started = true;
    }
    static_shape->setTransform(updated_xfm);
    static_shape->setScale(updated_scale);
  }

  return true;
}

void afxEA_StaticShape::ea_finish(bool was_stopped)
{
  if (!static_shape)
    return;
  
  static_shape->deleteObject();
  static_shape = 0;
}

void afxEA_StaticShape::ea_set_scope_status(bool in_scope)
{
  if (static_shape)
    static_shape->setVisibility(in_scope);
}

void afxEA_StaticShape::onDeleteNotify(SimObject* obj)
{
  if (static_shape == dynamic_cast<afxStaticShape*>(obj))
    static_shape = 0;

  Parent::onDeleteNotify(obj);
}

void afxEA_StaticShape::getUpdatedBoxCenter(Point3F& pos)
{
  if (static_shape)
    pos = static_shape->getBoxCenter();
}


TSShape* afxEA_StaticShape::getTSShape()
{
  return (static_shape) ? ((TSShape*)static_shape->getShape()) : 0;
}

TSShapeInstance* afxEA_StaticShape::getTSShapeInstance()
{
  return (static_shape) ? static_shape->getShapeInstance() : 0;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Light

afxEA_Light::afxEA_Light()
{
  light = 0;
}

afxEA_Light::~afxEA_Light()
{
  if (light)
    light->deleteObject();
}

void afxEA_Light::ea_start()
{
}

bool afxEA_Light::ea_update(F32 dt)
{
  if (!light)
  {
    if (datablock->effect_data.light)
    {
      // create and register effect
      light = new afxLight();
      light->onNewDataBlock(datablock->effect_data.light);
      if (!light->registerObject())
      {
        delete light;
        light = 0;
      }
      deleteNotify(light);

      light->setFadeAmount(updated_scale.x);
    }
  }

  if (light)
  {
    if (do_fades)
      light->setFadeAmount(fade_value*updated_scale.x);
    light->setTransform(updated_xfm);
  }

  return true;
}

void afxEA_Light::ea_finish(bool was_stopped)
{
  if (light)
  {
    light->deleteObject();
    light = 0;
  }
}

void afxEA_Light::ea_set_scope_status(bool in_scope)
{
  if (light)
    light->setVisibility(in_scope);
}

void afxEA_Light::onDeleteNotify(SimObject* obj)
{
  if (light == dynamic_cast<afxLight*>(obj))
    light = 0;

  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_AnimClip

afxEA_AnimClip::afxEA_AnimClip()
{
  started = false;
  anim_lifetime = 0;
  anim_tag = 0;
  lock_tag = 0;
}

afxEA_AnimClip::~afxEA_AnimClip()
{
}

void afxEA_AnimClip::ea_start()
{
  afxAnimClipData* clip = datablock->effect_data.anim_clip;
  afxConstraint* pos_constraint = getPosConstraint();
  if (full_lifetime == INFINITE_LIFETIME && pos_constraint != 0)
    anim_lifetime = pos_constraint->getAnimClipDuration(clip->clip_name);
  else 
    anim_lifetime = full_lifetime;
  anim_tag = 0;
  lock_tag = 0;
}

bool afxEA_AnimClip::ea_update(F32 dt)
{
  afxConstraint* pos_constraint = getPosConstraint();
  if (!started && pos_constraint != 0)
  {
    bool go_for_it = true;

    if (pos_constraint->getDamageState() == ShapeBase::Enabled)
      go_for_it = !datablock->effect_data.anim_clip->ignore_enabled;
    else if (pos_constraint->getDamageState() == ShapeBase::Disabled)
      go_for_it = !datablock->effect_data.anim_clip->ignore_disabled;

    if (go_for_it)
    {
      afxAnimClipData* clip = datablock->effect_data.anim_clip;
      F32 rate = clip->rate/prop_time_factor;
      F32 pos = mFmod(life_elapsed, anim_lifetime)/anim_lifetime;
      if (clip->rate < 0) 
        pos = 1.0f - pos;
      anim_tag = pos_constraint->setAnimClip(clip->clip_name, pos, rate, clip->trans, 
                                             clip->is_death_anim);
      if (clip->lock_anim)
        lock_tag = pos_constraint->lockAnimation();
    }
    started = true;
  }

  return true;
}

void afxEA_AnimClip::ea_finish(bool was_stopped)
{
  afxConstraint* pos_constraint = getPosConstraint();
  if (pos_constraint && anim_tag != 0)
    pos_constraint->resetAnimation(anim_tag, datablock->effect_data.anim_clip->is_death_anim);
  if (pos_constraint && lock_tag != 0)
    pos_constraint->unlockAnimation(lock_tag);

  started = false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_AnimLock

afxEA_AnimLock::afxEA_AnimLock()
{
  started = false;
  lock_tag = 0;
}

afxEA_AnimLock::~afxEA_AnimLock()
{
}

void afxEA_AnimLock::ea_start()
{
}

bool afxEA_AnimLock::ea_update(F32 dt)
{
  afxConstraint* pos_constraint = getPosConstraint();
  if (!started && pos_constraint != 0)
  {
    lock_tag = pos_constraint->lockAnimation();
    started = true;
  }

  return true;
}

void afxEA_AnimLock::ea_finish(bool was_stopped)
{
  afxConstraint* pos_constraint = getPosConstraint();
  if (pos_constraint && lock_tag != 0)
  {
    pos_constraint->unlockAnimation(lock_tag);
  }

  started = false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Damage

afxEA_Damage::afxEA_Damage()
{
  started = false;
  repeat_cnt = 0;
  dot_delta_ms = 0; 
  next_dot_time = 0;
  impact_pos.zero();
  impacted_obj_id = 0;
}

afxEA_Damage::~afxEA_Damage()
{
}

bool afxEA_Damage::isDone() 
{ 
  return (repeat_cnt >= datablock->effect_data.damage->repeats);
}

void afxEA_Damage::ea_start()
{
  afxDamageData* dmg = datablock->effect_data.damage;

  if (dmg->repeats > 1)
  {
    dot_delta_ms = full_lifetime/(dmg->repeats - 1);
    next_dot_time = dot_delta_ms;
  }
}

bool afxEA_Damage::ea_update(F32 dt)
{
  afxDamageData* dmg = datablock->effect_data.damage;

  if (!started)
  {
    started = true;

    afxConstraint* pos_cons = getPosConstraint();
    if (pos_cons)
      pos_cons->getPosition(impact_pos);

    afxConstraint* aim_cons = getAimConstraint();
    if (aim_cons && aim_cons->getObject())
      impacted_obj_id = aim_cons->getObject()->getId();

    if (choreographer)
      choreographer->inflictDamage(dmg->label, dmg->flavor, impacted_obj_id, dmg->amount, 
                                   repeat_cnt, dmg->ad_amount, dmg->radius, impact_pos, 
                                   dmg->impulse);
    repeat_cnt++;
  }
  else if (repeat_cnt < dmg->repeats)
  {
    if (next_dot_time <= life_elapsed)
    {
      if (choreographer)
        choreographer->inflictDamage(dmg->label, dmg->flavor, impacted_obj_id, dmg->amount, 
                                     repeat_cnt, 0, 0, impact_pos, 0);
      next_dot_time += dot_delta_ms;
      repeat_cnt++;
    }
  }

  return true;
}

void afxEA_Damage::ea_finish(bool was_stopped)
{
  afxDamageData* dmg = datablock->effect_data.damage;

  if (started && (repeat_cnt < dmg->repeats))
  {
    if (next_dot_time <= life_elapsed)
    {
      if (choreographer)
        choreographer->inflictDamage(dmg->label, dmg->flavor, impacted_obj_id, dmg->amount, 
                                     repeat_cnt, 0, 0, impact_pos, 0);
    }
  }

  started = false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Sound

afxEA_Sound::afxEA_Sound()
{
  sound_handle = NULL_AUDIOHANDLE;
}

afxEA_Sound::~afxEA_Sound()
{
  sound_handle = NULL_AUDIOHANDLE;
}

void afxEA_Sound::ea_start()
{
}

bool afxEA_Sound::ea_update(F32 dt)
{
  if (sound_handle == NULL_AUDIOHANDLE)
  {
    AudioProfile* snd = datablock->effect_data.sound;
    // ADJUST STARTING POINT BASED ON ELAPSED?
    sound_handle = alxPlay(snd, &updated_xfm, 0);
  }

  if (sound_handle != NULL_AUDIOHANDLE)
  {
    alxSourceMatrixF(sound_handle, &updated_xfm);
    alxSourcef(sound_handle, AL_GAIN_LINEAR, updated_scale.x*fade_value);
  }

  return true;
}

void afxEA_Sound::ea_finish(bool was_stopped)
{
  if (sound_handle != NULL_AUDIOHANDLE)
  {
    alxStop(sound_handle);
    sound_handle = NULL_AUDIOHANDLE;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_ScriptEvent

afxEA_ScriptEvent::afxEA_ScriptEvent()
{
  ran_script = false;
}

afxEA_ScriptEvent::~afxEA_ScriptEvent()
{
}

void afxEA_ScriptEvent::ea_start()
{
  ran_script = (datablock->effect_data.script_event->method_name == ST_NULLSTRING);
}

bool afxEA_ScriptEvent::ea_update(F32 dt)
{
  if (!ran_script && choreographer != NULL)
  {
    afxScriptEventData* db = datablock->effect_data.script_event;
    afxConstraint* pos_constraint = getPosConstraint();
    choreographer->executeScriptEvent(db->method_name, pos_constraint, updated_pos, 
                                      db->script_data);
    ran_script = true;
  }

  return true;
}

void afxEA_ScriptEvent::ea_finish(bool was_stopped)
{
  ran_script = false;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_CameraShake

afxEA_CameraShake::afxEA_CameraShake()
{
  camera_shake = 0;
}

afxEA_CameraShake::~afxEA_CameraShake()
{
  delete camera_shake;
}

void afxEA_CameraShake::ea_start()
{
  afxConstraint* pos_constraint = getPosConstraint();
  afxConstraint* aim_constraint = getAimConstraint();

  if (aim_constraint && pos_constraint)
  {
    if (full_lifetime <= 0 || full_lifetime == INFINITE_LIFETIME)
    {
      Con::warnf(ConsoleLogEntry::General, "afxCameraShake effect requires a finite lifetime. (%s)", datablock->getName());
      return;
    }

    ShapeBase* shaken = dynamic_cast<ShapeBase*>(aim_constraint->getObject());
    if (shaken)
    {
      afxCameraShakeData* db = datablock->effect_data.camshake;

      Point3F pos; pos_constraint->getPosition(pos);
      VectorF diff = shaken->getPosition() - pos;
      F32 dist = diff.len();
      if (dist < db->camShakeRadius)
      {
        camera_shake = new CameraShake;
        camera_shake->setDuration(full_lifetime);
        camera_shake->setFrequency(db->camShakeFreq);

        F32 falloff =  dist/db->camShakeRadius;
        falloff = 1 + falloff*10.0;
        falloff = 1.0 / (falloff*falloff);

        VectorF shakeAmp = db->camShakeAmp*falloff;
        camera_shake->setAmplitude(shakeAmp);
        camera_shake->setFalloff(db->camShakeFalloff);
        camera_shake->init();
      }
    }
  }
}

bool afxEA_CameraShake::ea_update(F32 dt)
{
  afxConstraint* aim_constraint = getAimConstraint();
  if (camera_shake && aim_constraint)
  {
    camera_shake->update(dt);
    
    ShapeBase* shaken = dynamic_cast<ShapeBase*>(aim_constraint->getObject());
    if (shaken)
    {
      MatrixF fxTrans = camera_shake->getTrans();
      MatrixF curTrans = shaken->getRenderTransform();
      curTrans.mul(fxTrans);
      
      Point3F	cameraPosWorld;
      curTrans.getColumn(3,&cameraPosWorld);
      shaken->setPosition(cameraPosWorld);
    }
  }
  
  return true;
}

void afxEA_CameraShake::ea_finish(bool was_stopped)
{
  delete camera_shake;
  camera_shake = 0;
}

#ifdef AFX_TLK_SUPPORT_ENABLED


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_TLKLight

class sgLightObjectGhoster : public sgLightObject
{
public:
  void force_ghost() 
  {
    mNetFlags.clear(Ghostable | ScopeAlways);
    mNetFlags.set(IsGhost);
  }
};

afxEA_TLKLight::afxEA_TLKLight()
{
  tlk_light = 0;
}

afxEA_TLKLight::~afxEA_TLKLight()
{
  if (tlk_light)
    tlk_light->deleteObject();
}

void afxEA_TLKLight::ea_start()
{
}

bool afxEA_TLKLight::ea_update(F32 dt)
{
  if (!tlk_light)
  {
    if (datablock->effect_data.tlk_light)
    {
      sgLightObjectData* db = datablock->effect_data.tlk_light;
      
      // create and register effect
      tlk_light = new sgLightObject();
      ((sgLightObjectGhoster*)tlk_light)->force_ghost();
      tlk_light->onNewDataBlock(db);
      if (!tlk_light->registerObject())
      {
        delete tlk_light;
        tlk_light = 0;
      }
      deleteNotify(tlk_light);

      if (db->afx_emitterEffectName != ST_NULLSTRING)
      {
        afxEffectWrapper* ew = choreographer->findNamedEffect(db->afx_emitterEffectName);
        if (ew)
        {
          afxEA_ParticleEmitter* pe = dynamic_cast<afxEA_ParticleEmitter*>(ew);
          if (pe && pe->emitter)
          {
            tlk_light->afx_setEffectEmitter(pe->emitter);
          }
        }
      }

      tlk_light->setFadeAmount(updated_scale.x);
    }
  }

  if (tlk_light)
  {
    if (do_fades)
      tlk_light->setFadeAmount(fade_value*updated_scale.x);
    tlk_light->setTransform(updated_xfm);
  }

  return true;
}

void afxEA_TLKLight::ea_finish(bool was_stopped)
{
  if (tlk_light)
  {
    tlk_light->deleteObject();
    tlk_light = 0;
  }
}

void afxEA_TLKLight::ea_set_scope_status(bool in_scope)
{
  if (tlk_light)
    tlk_light->setEnable(in_scope);
}

void afxEA_TLKLight::onDeleteNotify(SimObject* obj)
{
  if (tlk_light == dynamic_cast<sgLightObject*>(obj))
    tlk_light = 0;

  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_VolumeLight

class sgVolumeLightGhoster : public volumeLight
{
public:
  void force_ghost() 
  {
    mNetFlags.clear(Ghostable | ScopeAlways);
    mNetFlags.set(IsGhost);
  }
};

afxEA_VolumeLight::afxEA_VolumeLight()
{
  volume_light = 0;
}

afxEA_VolumeLight::~afxEA_VolumeLight()
{
  if (volume_light)
    volume_light->deleteObject();
}

void afxEA_VolumeLight::ea_start()
{
}

bool afxEA_VolumeLight::ea_update(F32 dt)
{
  if (!volume_light)
  {
    if (datablock->effect_data.volume_light)
    {
      afxVolumeLightData* db = datablock->effect_data.volume_light;
      
      // create and register effect
      volume_light = new volumeLight();
      ((sgVolumeLightGhoster*)volume_light)->force_ghost();
      volume_light->onNewDataBlock(db);

      volume_light->setLtexture(db->mLTextureName);
      volume_light->setlpDistance(db->mlpDistance);
      volume_light->setShootDistance(db->mShootDistance);
      volume_light->setXextent(db->mXextent);
      volume_light->setYextent(db->mYextent);
      volume_light->setSubdivideU(db->mSubdivideU);
      volume_light->setSubdivideV(db->mSubdivideV);
      volume_light->setfootColour(db->mfootColour);
      volume_light->settailColour(db->mtailColour);
      
      if (!volume_light->registerObject())
      {
        delete volume_light;
        volume_light = 0;
      }
      deleteNotify(volume_light);

      if (db->afx_emitterEffectName != ST_NULLSTRING)
      {
        afxEffectWrapper* ew = choreographer->findNamedEffect(db->afx_emitterEffectName);
        if (ew)
        {
          afxEA_ParticleEmitter* pe = dynamic_cast<afxEA_ParticleEmitter*>(ew);
          if (pe && pe->emitter)
          {
            volume_light->afx_setEffectEmitter(pe->emitter);
          }
        }
      }

      volume_light->setFadeAmount(updated_scale.x);
    }
  }

  if (volume_light)
  {
    if (do_fades)
      volume_light->setFadeAmount(fade_value*updated_scale.x);
    volume_light->setTransform(updated_xfm);
  }

  return true;
}

void afxEA_VolumeLight::ea_finish(bool was_stopped)
{
  if (volume_light)
  {
    volume_light->deleteObject();
    volume_light = 0;
  }
}

void afxEA_VolumeLight::ea_set_scope_status(bool in_scope)
{
  if (volume_light)
    volume_light->setEnable(in_scope);
}

void afxEA_VolumeLight::onDeleteNotify(SimObject* obj)
{
  if (volume_light == dynamic_cast<volumeLight*>(obj))
    volume_light = 0;

  Parent::onDeleteNotify(obj);
}

#endif

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//


