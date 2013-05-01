//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// afxMagicMissile is a heavily modified variation of the stock TGE Projectile class. In 
// addition to numerous AFX customizations, it also incorporates functionality based on
// the following TGE resources:
//
// Guided or Seeker Projectiles by Derk Adams
//   http://www.garagegames.com/index.php?sec=mg&mod=resource&page=view&qid=6778
//
// Projectile Ballistic Coefficients (drag factors) by Mark Owen
//   http://www.garagegames.com/index.php?sec=mg&mod=resource&page=view&qid=5128
//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _AFX_MAGIC_MISSILE_H_
#define _AFX_MAGIC_MISSILE_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/gameBase.h"
#include "ts/tsShape.h"
#if (TORQUE_GAME_ENGINE < 1510)
#include "sceneGraph/lightManager.h"
#endif
#include "platform/platformAudio.h"
#include "game/fx/particleEmitter.h"

class SplashData;
class ShapeBase;
class TSShapeInstance;

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxMagicMissileData

class afxMagicMissileData : public GameBaseData
{
  typedef GameBaseData Parent;
  
public:
  enum { MaxLifetimeTicks = 4095 };
  //enum { MaxLifetimeTicks = 65535 };
  
public:
  StringTableEntry      missile_shape_name;
  Resource<TSShape>     missile_shape;
  Point3F               missile_shape_scale;

  U32                   collision_mask;

                        // built-in light
  bool                  hasLight;
  F32                   lightRadius;
  ColorF                lightColor;
  bool                  hasWaterLight;
  ColorF                waterLightColor;

  F32                   muzzleVelocity;
  Point3F               starting_vel_vec;
  bool                  isBallistic;

                        // guidance behavior
  bool                  isGuided;
  F32                   precision;
  S32                   trackDelay;

                        // simple physics
  F32                   ballisticCoefficient;
  F32                   gravityMod;

                        // terrain following
  bool                  followTerrain;
  F32                   followTerrainHeight;
  F32                   followTerrainAdjustRate;
  S32                   followTerrainAdjustDelay;

                        // water/air transitions
  SplashData*           splash;   
  S32                   splashId; 

  U32                   lifetime;     // ticks
  S32                   fadeDelay;    // ticks

  AudioProfile*         sound;  
  S32                   soundId;  
  ParticleEmitterData*  particleEmitter;
  S32                   particleEmitterId;
  ParticleEmitterData*  particleWaterEmitter;
  S32                   particleWaterEmitterId;
  F32                   acceleration;
  S32                   accelDelay;
  U32                   accelLifetime;

  StringTableEntry      launch_node;
  Point3F               launch_offset;
  Point3F               launch_offset_server;
  Point3F               launch_offset_client;
  Point3F               launch_node_offset;
  F32                   launch_pitch;
  F32                   launch_pan;
  bool                  echo_launch_offset;

                        // wiggle behavior
  Vector<F32>           wiggle_magnitudes;
  Vector<F32>           wiggle_speeds;
  StringTableEntry      wiggle_axis_string;
  Point3F*              wiggle_axis;
  U32                   wiggle_num_axis;

                        // hover behavior
  F32                   hover_altitude;
  F32                   hover_attack_distance;
  F32                   hover_attack_gradient;
  U32                   hover_time;

  
public:
  /*C*/                 afxMagicMissileData();
  /*D*/                 ~afxMagicMissileData();
  
  bool                  onAdd();
  void                  packData(BitStream*);
  void                  unpackData(BitStream*);
  bool                  preload(bool server, char errorBuffer[256]);
  
  static void           initPersistFields();
  
  DECLARE_CONOBJECT(afxMagicMissileData);
};
DECLARE_CONSOLETYPE(afxMagicMissileData);

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxMagicMissile

//class afxMagicSpell;
class afxChoreographer;

class afxMagicMissile : public GameBase
{
  typedef GameBase Parent;
  friend class afxMagicSpell;

public:
  enum UpdateMasks 
  {
    GuideMask     = Parent::NextFreeMask << 0,
    LaunchMask    = Parent::NextFreeMask << 1,
    ImpactMask    = Parent::NextFreeMask << 2,
    NextFreeMask  = Parent::NextFreeMask << 3
  }; 
  
private:
  afxMagicMissileData*  datablock;
  afxChoreographer* choreographer;

  bool              client_only;
  bool              server_only;
  U32               elapsed_ticks;
  bool              use_accel;
  ParticleEmitter*  air_emitter;
  ParticleEmitter*  water_emitter;
  AUDIOHANDLE       sound_handle;
  Point3F           missile_pos;
  Point3F           missile_vel;
  TSShapeInstance*  missile_shape;
  LightInfo         light_info;
  Point3F           interp_delta_base;
  Point3F           interp_back_delta;
  U32               collision_mask;
  F32               prec_inc;

  bool              did_launch;
  bool              did_impact; 
  
  SceneObject*      missile_target;
  ShapeBase*        collide_exempt;

  bool              hover_attack_go;
  U32               hover_attack_tick;
  
  F32               starting_velocity;
  Point3F           starting_vel_vec;

private:
  void              init(bool on_server, bool on_client);
  bool              point_in_water(const Point3F &point);
  void              create_splash(const Point3F& pos);
  void              get_launch_data(Point3F& pos, Point3F& vel);
  void              emit_particles(const Point3F&, const Point3F&, const Point3F&, const U32);
  void              update_sound();
  void              kill_sound();
  bool              is_active() const { return (did_launch && !did_impact); }

public:
  /*C*/             afxMagicMissile();
  /*C*/             afxMagicMissile(bool on_server, bool on_client);
  /*D*/             ~afxMagicMissile();
  
  virtual void      registerLights(LightManager*, bool lightingScene);
  virtual bool      onAdd();
  virtual void      onRemove();
  virtual bool      onNewDataBlock(GameBaseData *dptr);
  virtual void      processTick(const Move *move);
  virtual void      interpolateTick(F32 delta);
  virtual bool      prepRenderImage(SceneState*, const U32 key, const U32 startZone, 
                                    const bool modifyBaseZoneState=false);
  virtual void      renderObject(SceneState*, SceneRenderImage*);
  virtual U32       packUpdate(NetConnection*, U32 mask, BitStream*);
  virtual void      unpackUpdate(NetConnection*, BitStream*);
  virtual void      onDeleteNotify(SimObject*);

  static void       initPersistFields();  
  
  void              launch();
  void              setChoreographer(afxChoreographer*); 
  void              setStartingVelocityVector(const Point3F& vel_vec);
  void              setStartingVelocity(const F32 vel);
  void              getStartingVelocityValues(F32& vel, Point3F& vel_vec); 
  
  DECLARE_CONOBJECT(afxMagicMissile);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxMagicMissileCallback

class afxMagicMissileCallback
{
public:
  virtual void impactNotify(const Point3F& p, const Point3F& n, SceneObject*)=0;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_MAGIC_MISSILE_H_

