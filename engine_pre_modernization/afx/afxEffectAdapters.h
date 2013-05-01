
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _AFX_EFFECT_ADAPTER_H_
#define _AFX_EFFECT_ADAPTER_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afxEffectWrapper.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_ParticleEmitter 

class ParticleEmitter;

class afxEA_ParticleEmitter : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  bool              do_bbox_update;

public:
  ParticleEmitter*  emitter;

  /*C*/             afxEA_ParticleEmitter();
  /*D*/             ~afxEA_ParticleEmitter();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Explosion 

class Explosion;

class afxEA_Explosion : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  Explosion*        explosion;
  bool              exploded;

public:
  /*C*/             afxEA_Explosion();
  /*D*/             ~afxEA_Explosion();

  virtual bool      isDone() { return exploded; }

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Debris 

class Debris;

class afxEA_Debris : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  Debris*           debris;
  bool              exploded;

public:
  /*C*/             afxEA_Debris();
  /*D*/             ~afxEA_Debris();

  virtual bool      isDone() { return exploded; }

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Projectile 

class afxProjectile;

class afxEA_Projectile : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxProjectile*    projectile;
  bool              launched;
  bool              impacted;

public:
  /*C*/             afxEA_Projectile();
  /*D*/             ~afxEA_Projectile();

  virtual bool      isDone() { return impacted; }

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_MachineGun 

class afxMachineGun;

class afxEA_MachineGun : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  bool              shooting;
  F32               start_time;
  F32               shot_gap;
  S32               shot_count;

  void              launch_projectile();

public:
  /*C*/             afxEA_MachineGun();
  /*D*/             ~afxEA_MachineGun();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Mooring 

class afxMooring;

class afxEA_Mooring : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxMooring*       obj;

public:
  /*C*/             afxEA_Mooring();
  /*D*/             ~afxEA_Mooring();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Zodiac 

class afxEA_Zodiac : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  Point3F           zode_pos;
  F32               zode_radius;
  ColorF            zode_color;
  F32               zode_angle;
  F32               zode_angle_offset;

  F32               calc_facing_angle();

public:
  /*C*/             afxEA_Zodiac();
  /*D*/             ~afxEA_Zodiac();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Model 

class afxModel;
class afxEffectWrapper;

class afxEA_Model : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxModel*         model;

public:
  /*C*/             afxEA_Model();
  /*D*/             ~afxEA_Model();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
  virtual void      ea_set_scope_status(bool flag);
  virtual void      onDeleteNotify(SimObject*);

  virtual void      getUpdatedBoxCenter(Point3F& pos);
  virtual TSShape*  getTSShape();
  virtual TSShapeInstance*  getTSShapeInstance();
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_StaticShape 

class afxStaticShape;

class afxEA_StaticShape : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxStaticShape*   static_shape;
  bool              fade_out_started;

public:
  /*C*/             afxEA_StaticShape();
  /*D*/             ~afxEA_StaticShape();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
  virtual void      ea_set_scope_status(bool flag);
  virtual void      onDeleteNotify(SimObject*);

  virtual void              getUpdatedBoxCenter(Point3F& pos);
  virtual TSShape*          getTSShape();
  virtual TSShapeInstance*  getTSShapeInstance();
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Light 

class afxLight;

class afxEA_Light : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxLight*         light;

public:
  /*C*/             afxEA_Light();
  /*D*/             ~afxEA_Light();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
  virtual void      ea_set_scope_status(bool flag);
  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_AnimClip 

class afxEA_AnimClip : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  bool              started;
  F32               anim_lifetime;
  U32               anim_tag;
  U32               lock_tag;

public:
  /*C*/             afxEA_AnimClip();
  /*D*/             ~afxEA_AnimClip();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_AnimLock 

class afxEA_AnimLock : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  bool              started;
  U32               lock_tag;

public:
  /*C*/             afxEA_AnimLock();
  /*D*/             ~afxEA_AnimLock();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Damage 

class afxEA_Damage : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  bool              started;
  U8                repeat_cnt;
  U32               dot_delta_ms;
  U32               next_dot_time;
  Point3F           impact_pos;
  SimObjectId       impacted_obj_id;

public:
  /*C*/             afxEA_Damage();
  /*D*/             ~afxEA_Damage();

  virtual bool      isDone();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Sound 

class afxEA_Sound : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  AUDIOHANDLE       sound_handle;

public:
  /*C*/             afxEA_Sound();
  /*D*/             ~afxEA_Sound();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_ScriptEvent 

class afxEA_ScriptEvent : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  bool              ran_script;

public:
  /*C*/             afxEA_ScriptEvent();
  /*D*/             ~afxEA_ScriptEvent();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_CameraShake 

class CameraShake;

class afxEA_CameraShake : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  CameraShake*      camera_shake;

public:
  /*C*/             afxEA_CameraShake();
  /*D*/             ~afxEA_CameraShake();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

#ifdef AFX_TLK_SUPPORT_ENABLED

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_TLKLight 

class sgLightObject;

class afxEA_TLKLight : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  sgLightObject*    tlk_light;

public:
  /*C*/             afxEA_TLKLight();
  /*D*/             ~afxEA_TLKLight();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
  virtual void      ea_set_scope_status(bool flag);
  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_VolumeLight 

class volumeLight;

class afxEA_VolumeLight : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  volumeLight* volume_light;

public:
  /*C*/             afxEA_VolumeLight();
  /*D*/             ~afxEA_VolumeLight();

  virtual void      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
  virtual void      ea_set_scope_status(bool flag);
  virtual void      onDeleteNotify(SimObject*);
};

#endif

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//


#endif // _AFX_EFFECT_ADAPTER_H_
