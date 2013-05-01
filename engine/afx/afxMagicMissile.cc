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

#include "afx/arcaneFX.h"

#include "sceneGraph/sceneGraph.h"
#include "game/fx/splash.h"
#include "audio/audioDataBlock.h"
#include "math/mathUtils.h"
#include "math/mathIO.h"
#include "sim/netConnection.h"
#include "terrain/waterBlock.h"
#include "ts/tsShapeInstance.h"
#if !defined(TGEA_ENGINE)
#include "game/fx/particleEngine.h"
#endif

#include "afx/util/afxEase.h"
#include "afx/afxMagicMissile.h"
#include "afx/afxMagicSpell.h"
#include "afx/afxChoreographer.h"

class ObjectDeleteEvent : public SimEvent
{
public:
  void process(SimObject *object)
  {
    object->deleteObject();
  }
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxMagicMissileData

IMPLEMENT_CONSOLETYPE(afxMagicMissileData)
IMPLEMENT_CO_DATABLOCK_V1(afxMagicMissileData);

afxMagicMissileData::afxMagicMissileData()
{
  missile_shape_name = ST_NULLSTRING;

  collision_mask = arcaneFX::sMissileCollisionMask;

  hasLight = false;
  lightRadius = 1;
  lightColor.set(1,1,1);
#if defined(AFX_CAP_LIGHT_ILLUMINATION_MASKING)
  lightIlluminationMask = LightInfo::IllumALL;
#endif
  hasWaterLight = false;
  waterLightColor.set(1,1,1);

  missile_shape_scale.set(1,1,1);

  muzzleVelocity = 50;
  starting_vel_vec.zero();

  isBallistic = false;

  isGuided = false;
  precision = 0; 
  trackDelay = 0;
  ballisticCoefficient = 1.0f; 

  gravityMod = 1.0;

  followTerrain = false;
  followTerrainHeight = 0.1f;
  followTerrainAdjustRate = 20.0f;
  //
  followTerrainAdjustDelay = 0;
  //

  lifetime = MaxLifetimeTicks;

  splash = NULL;
  splashId = 0;

  sound = NULL;
  soundId = 0;

  particleEmitter = NULL;
  particleEmitterId = 0;

  particleWaterEmitter = NULL;
  particleWaterEmitterId = 0;

  acceleration = 0;
  accelDelay = 0;
  accelLifetime = 0;

  //launch_node = StringTable->insert("Bip01 R Hand");
  launch_node = ST_NULLSTRING;
  launch_offset.zero();
  launch_offset_server.zero();
  launch_offset_client.zero();
  launch_node_offset.zero();
  launch_pitch = 0;
  launch_pan = 0;

  echo_launch_offset = false;

  wiggle_axis_string = ST_NULLSTRING;
  wiggle_num_axis = 0;
  wiggle_axis = 0;

  hover_altitude = 0;
  hover_attack_distance = 0;
  hover_attack_gradient = 0;
  hover_time = 0;
}

afxMagicMissileData::~afxMagicMissileData()
{
  if (wiggle_axis)
    delete [] wiggle_axis;
}

IMPLEMENT_GETDATATYPE(afxMagicMissileData)
IMPLEMENT_SETDATATYPE(afxMagicMissileData)

#define myOffset(field) Offset(field, afxMagicMissileData)

void afxMagicMissileData::initPersistFields()
{
  Parent::initPersistFields();

  IRangeValidatorScaled* ticksFromMS;
  ticksFromMS = new IRangeValidatorScaled(TickMs, 0, MaxLifetimeTicks);

    // missile shape
  addField("missileShapeName",    TypeFilename, myOffset(missile_shape_name));
  addField("missileShapeScale",   TypePoint3F,  myOffset(missile_shape_scale));

  addField("collisionMask",         TypeS32,      myOffset(collision_mask));

    // built-in light
  addNamedField(hasLight,         TypeBool,     afxMagicMissileData);
  addNamedFieldV(lightRadius,     TypeF32,      afxMagicMissileData,  new FRangeValidator(1, 20));
  addNamedField(lightColor,       TypeColorF,   afxMagicMissileData);
#if defined(AFX_CAP_LIGHT_ILLUMINATION_MASKING)
  addNamedField(lightIlluminationMask, TypeS8,       afxMagicMissileData);
#endif
  addNamedField(hasWaterLight,    TypeBool,     afxMagicMissileData);
  addNamedField(waterLightColor,  TypeColorF,   afxMagicMissileData);

  addNamedFieldV(muzzleVelocity,    TypeF32,      afxMagicMissileData,  new FRangeValidator(0, 10000));
  addField("startingVelocityVector",TypePoint3F,  myOffset(starting_vel_vec));

  addNamedField(isBallistic,            TypeBool,   afxMagicMissileData); 

  addNamedField(isGuided,               TypeBool,   afxMagicMissileData);
  addNamedFieldV(precision,             TypeF32,    afxMagicMissileData,  new FRangeValidator(0, 100)); 
  addNamedFieldV(trackDelay,            TypeS32,    afxMagicMissileData,  new FRangeValidator(0, 100000)); 
  addNamedFieldV(ballisticCoefficient,  TypeF32,    afxMagicMissileData,  new FRangeValidator(0, 1));
  
  addNamedFieldV(gravityMod,            TypeF32,    afxMagicMissileData,  new FRangeValidator(0, 1));

    // terrain following
  addField("followTerrain",             TypeBool, myOffset(followTerrain));
  addField("followTerrainHeight",       TypeF32,  myOffset(followTerrainHeight));
  addField("followTerrainAdjustRate",   TypeF32,  myOffset(followTerrainAdjustRate));
  addFieldV("followTerrainAdjustDelay", TypeS32,  myOffset(followTerrainAdjustDelay), ticksFromMS); 

  //

  addNamedFieldV(lifetime,    TypeS32,              afxMagicMissileData,  ticksFromMS);
#if !defined(BROKEN_SPLASH_CODE)
  addNamedField(splash,       TypeSplashDataPtr,    afxMagicMissileData);
#endif
  addNamedField(sound,        TypeAudioProfilePtr,  afxMagicMissileData);

  addNamedField(particleEmitter,        TypeParticleEmitterDataPtr,   afxMagicMissileData);
  addNamedField(particleWaterEmitter,   TypeParticleEmitterDataPtr,   afxMagicMissileData);

  addNamedField(acceleration,     TypeF32,  afxMagicMissileData);
  addNamedFieldV(accelDelay,      TypeS32,  afxMagicMissileData,  ticksFromMS);
  addNamedFieldV(accelLifetime,   TypeS32,  afxMagicMissileData,  ticksFromMS);

  addField("launchNode",        TypeString,   myOffset(launch_node));
  addField("launchOffset",      TypePoint3F,  myOffset(launch_offset));
  addField("launchOffsetServer",TypePoint3F,  myOffset(launch_offset_server));
  addField("launchOffsetClient",TypePoint3F,  myOffset(launch_offset_client));
  addField("launchNodeOffset",  TypePoint3F,  myOffset(launch_node_offset));
  addField("launchAimPitch",    TypeF32,      myOffset(launch_pitch));
  addField("launchAimPan",      TypeF32,      myOffset(launch_pan));
  //
  addField("echoLaunchOffset",  TypeBool,     myOffset(echo_launch_offset));

  addField("wiggleMagnitudes", TypeF32Vector, myOffset(wiggle_magnitudes));
  addField("wiggleSpeeds",     TypeF32Vector, myOffset(wiggle_speeds));
  addField("wiggleAxis",       TypeString,    myOffset(wiggle_axis_string));

  addField("hoverAltitude",       TypeF32, myOffset(hover_altitude));
  addField("hoverAttackDistance", TypeF32, myOffset(hover_attack_distance));
  addField("hoverAttackGradient", TypeF32, myOffset(hover_attack_gradient));
  addFieldV("hoverTime", TypeS32, Offset(hover_time, afxMagicMissileData), ticksFromMS);  

  // for compatability with Projectile
  addField("projectileShapeName", TypeFilename, myOffset(missile_shape_name));
  addField("scale",               TypePoint3F,  myOffset(missile_shape_scale));
}

bool afxMagicMissileData::onAdd()
{
  if(!Parent::onAdd())
    return false;

  if (!particleEmitter && particleEmitterId != 0)
    if (Sim::findObject(particleEmitterId, particleEmitter) == false)
      Con::errorf(ConsoleLogEntry::General, "afxMagicMissileData::onAdd: Invalid packet, bad datablockId(particleEmitter): %d", particleEmitterId);

  if (!particleWaterEmitter && particleWaterEmitterId != 0)
    if (Sim::findObject(particleWaterEmitterId, particleWaterEmitter) == false)
      Con::errorf(ConsoleLogEntry::General, "afxMagicMissileData::onAdd: Invalid packet, bad datablockId(particleWaterEmitter): %d", particleWaterEmitterId);

#if !defined(BROKEN_SPLASH_CODE)
  if (!splash && splashId != 0)
    if (Sim::findObject(splashId, splash) == false)
      Con::errorf(ConsoleLogEntry::General, "afxMagicMissileData::onAdd: Invalid packet, bad datablockId(splash): %d", splashId);
#endif

  if (!sound && soundId != 0)
    if (Sim::findObject(soundId, sound) == false)
      Con::errorf(ConsoleLogEntry::General, "afxMagicMissileData::onAdd: Invalid packet, bad datablockid(sound): %d", soundId);

  lightColor.clamp();
  waterLightColor.clamp();

  // Wiggle axes ////////////////////////////////////////////////////////////  
  if (wiggle_axis_string != ST_NULLSTRING && wiggle_num_axis == 0)   
  {
    // Tokenize input string and convert to Point3F array
    //
    Vector<char*> dataBlocks(__FILE__, __LINE__);

    // make a copy of points_string
    char* tokCopy = new char[dStrlen(wiggle_axis_string) + 1];
    dStrcpy(tokCopy, wiggle_axis_string);

    // extract tokens one by one, adding them to dataBlocks
    char* currTok = dStrtok(tokCopy, " \t");
    while (currTok != NULL) 
    {
      dataBlocks.push_back(currTok);
      currTok = dStrtok(NULL, " \t");
    }

    // bail if there were no tokens in the string
    if (dataBlocks.size() == 0) 
    {
      Con::warnf(ConsoleLogEntry::General, "afxMagicMissileData(%s) invalid wiggle axis string. No tokens found", getName());
      delete [] tokCopy;
      return false;
    }

    // Find wiggle_num_axis (round up to multiple of 3) // WARNING here if not multiple of 3?
    for (U32 i = 0; i < dataBlocks.size()%3; i++) 
    {
      dataBlocks.push_back("0.0");
    }

    wiggle_num_axis = dataBlocks.size()/3;
    wiggle_axis = new Point3F[wiggle_num_axis];

    U32 p_i = 0;
    for (U32 i = 0; i < dataBlocks.size(); i+=3, p_i++)
    {
      F32 x,y,z;
      x = dAtof(dataBlocks[i]);  // What about overflow?
      y = dAtof(dataBlocks[i+1]);
      z = dAtof(dataBlocks[i+2]);
      wiggle_axis[p_i].set(x,y,z);

      wiggle_axis[p_i].normalizeSafe(); // sufficient????
    }
      
    delete [] tokCopy; 
  }

  return true;
}

bool afxMagicMissileData::preload(bool server, char errorBuffer[256])
{
  if (Parent::preload(server, errorBuffer) == false)
    return false;

  if (missile_shape_name != ST_NULLSTRING) 
  {
    missile_shape = ResourceManager->load(missile_shape_name);
    if (missile_shape.isNull()) 
    {
      dSprintf(errorBuffer, sizeof(errorBuffer), "afxMagicMissileData::load: Couldn't load shape \"%s\"", missile_shape_name);
      return false;
    }
  }

  if (missile_shape) // create an instance to preload shape data
  {
    TSShapeInstance* pDummy = new TSShapeInstance(missile_shape, !server);
    delete pDummy;
  }

  return true;
}

//--------------------------------------------------------------------------
// Modified from floorPlanRes.cc
// Read a vector of items
template <class T>
bool readVector(Vector<T> & vec, Stream & stream, const char * msg)
{
   U32   num, i;
   bool  Ok = true;
   stream.read( & num );
   vec.setSize( num );
   for( i = 0; i < num && Ok; i++ ){
      Ok = stream.read(& vec[i]);
      AssertISV( Ok, avar("math vec read error (%s) on elem %d", msg, i) );
   }
   return Ok;
}
// Write a vector of items
template <class T>
bool writeVector(const Vector<T> & vec, Stream & stream, const char * msg)
{
   bool  Ok = true;
   stream.write( vec.size() );
   for( U32 i = 0; i < vec.size() && Ok; i++ ) {
      Ok = stream.write(vec[i]);
      AssertISV( Ok, avar("vec write error (%s) on elem %d", msg, i) );
   }
   return Ok;
}
//--------------------------------------------------------------------------

void afxMagicMissileData::packData(BitStream* stream)
{
  Parent::packData(stream);

  stream->writeString(missile_shape_name);

  if (stream->writeFlag(missile_shape_scale.x != 1 || 
                        missile_shape_scale.y != 1 || 
                        missile_shape_scale.z != 1))
  {
    stream->write(missile_shape_scale.x);
    stream->write(missile_shape_scale.y);
    stream->write(missile_shape_scale.z);
  }

  stream->write(collision_mask);

  if (stream->writeFlag(particleEmitter != NULL))
    stream->writeRangedU32(particleEmitter->getId(), DataBlockObjectIdFirst, DataBlockObjectIdLast);
  if (stream->writeFlag(particleWaterEmitter != NULL))
    stream->writeRangedU32(particleWaterEmitter->getId(), DataBlockObjectIdFirst, DataBlockObjectIdLast);
#if !defined(BROKEN_SPLASH_CODE)
  if (stream->writeFlag(splash != NULL))
    stream->writeRangedU32(splash->getId(), DataBlockObjectIdFirst, DataBlockObjectIdLast);
#endif
  if (stream->writeFlag(sound != NULL))
    stream->writeRangedU32(sound->getId(), DataBlockObjectIdFirst, DataBlockObjectIdLast);

  if(stream->writeFlag(hasLight))
  {
    stream->writeFloat(lightRadius/20.0, 8);
    stream->writeFloat(lightColor.red,7);
    stream->writeFloat(lightColor.green,7);
    stream->writeFloat(lightColor.blue,7);
#if defined(AFX_CAP_LIGHT_ILLUMINATION_MASKING)
    stream->write(lightIlluminationMask);
#endif
  }

  if(stream->writeFlag(hasWaterLight))
  {
    stream->writeFloat(waterLightColor.red, 7);
    stream->writeFloat(waterLightColor.green, 7);
    stream->writeFloat(waterLightColor.blue, 7);
  }

  stream->writeRangedU32(lifetime, 0, MaxLifetimeTicks);

  if(stream->writeFlag(isBallistic))
  {
    stream->write(gravityMod);
    stream->write(ballisticCoefficient);
  }

  if(stream->writeFlag(isGuided))
  {
    stream->write(precision);
    stream->write(trackDelay);
  }

  stream->write(muzzleVelocity);
  mathWrite(*stream, starting_vel_vec);
  stream->write(acceleration);
  stream->write(accelDelay);
  stream->write(accelLifetime);

  stream->writeString(launch_node);
  mathWrite(*stream, launch_offset);
  mathWrite(*stream, launch_offset_server);
  mathWrite(*stream, launch_offset_client);
  mathWrite(*stream, launch_node_offset);
  stream->write(launch_pitch);
  stream->write(launch_pan);
  stream->write(echo_launch_offset);

  writeVector(wiggle_magnitudes, *stream, "afxMagicMissile: wiggle_magnitudes");
  writeVector(wiggle_speeds, *stream, "afxMagicMissile: wiggle_speeds");

  stream->write(wiggle_num_axis);
  for (U32 i = 0; i < wiggle_num_axis; i++)
    mathWrite(*stream, wiggle_axis[i]);

  stream->write(hover_altitude);
  stream->write(hover_attack_distance);
  stream->write(hover_attack_gradient);
  stream->writeRangedU32(hover_time, 0, MaxLifetimeTicks);
}

void afxMagicMissileData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  missile_shape_name = stream->readSTString();

  if(stream->readFlag())
  {
    stream->read(&missile_shape_scale.x);
    stream->read(&missile_shape_scale.y);
    stream->read(&missile_shape_scale.z);
  }
  else
    missile_shape_scale.set(1,1,1);

  stream->read(&collision_mask);

  if (stream->readFlag())
    particleEmitterId = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
  if (stream->readFlag())
    particleWaterEmitterId = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
#if !defined(BROKEN_SPLASH_CODE)
  if (stream->readFlag())
    splashId = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
#endif
  if (stream->readFlag())
    soundId = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);

  hasLight = stream->readFlag();
  if(hasLight)
  {
    lightRadius = stream->readFloat(8) * 20;
    lightColor.red = stream->readFloat(7);
    lightColor.green = stream->readFloat(7);
    lightColor.blue = stream->readFloat(7);
#if defined(AFX_CAP_LIGHT_ILLUMINATION_MASKING)
    stream->read(&lightIlluminationMask);
#endif
  }
  hasWaterLight = stream->readFlag();
  if(hasWaterLight)
  {
    waterLightColor.red = stream->readFloat(7);
    waterLightColor.green = stream->readFloat(7);
    waterLightColor.blue = stream->readFloat(7);
  }

  lifetime = stream->readRangedU32(0, MaxLifetimeTicks);

  isBallistic = stream->readFlag();
  if(isBallistic)
  {
    stream->read(&gravityMod);
    stream->read(&ballisticCoefficient);
  }

  isGuided = stream->readFlag();
  if(isGuided)
  {
    stream->read(&precision);
    stream->read(&trackDelay);
  }

  stream->read(&muzzleVelocity);
  mathRead(*stream, &starting_vel_vec);
  stream->read(&acceleration);
  stream->read(&accelDelay);
  stream->read(&accelLifetime);

  launch_node = stream->readSTString();
  mathRead(*stream, &launch_offset);
  mathRead(*stream, &launch_offset_server);
  mathRead(*stream, &launch_offset_client);
  mathRead(*stream, &launch_node_offset);
  stream->read(&launch_pitch);
  stream->read(&launch_pan);
  stream->read(&echo_launch_offset);

  readVector(wiggle_magnitudes, *stream, "afxMagicMissile: wiggle_magnitudes");
  readVector(wiggle_speeds, *stream, "afxMagicMissile: wiggle_speeds");

  if (wiggle_axis)
    delete [] wiggle_axis;
  wiggle_axis = 0;
  wiggle_num_axis = 0;

  stream->read(&wiggle_num_axis);
  if (wiggle_num_axis > 0)
  {
    wiggle_axis = new Point3F[wiggle_num_axis];
    for (U32 i = 0; i < wiggle_num_axis; i++)
      mathRead(*stream, &wiggle_axis[i]);
  }

  stream->read(&hover_altitude);
  stream->read(&hover_attack_distance);
  stream->read(&hover_attack_gradient);
  hover_time = stream->readRangedU32(0, MaxLifetimeTicks);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxMagicMissile

IMPLEMENT_CO_NETOBJECT_V1(afxMagicMissile);

afxMagicMissile::afxMagicMissile()
{
  init(true, true);
}

afxMagicMissile::afxMagicMissile(bool on_server, bool on_client)
{
  init(on_server, on_client);
}

afxMagicMissile::~afxMagicMissile()
{
  delete missile_shape;
  missile_shape = NULL;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

void afxMagicMissile::initPersistFields()
{
  Parent::initPersistFields();

  addField("initialPosition", TypePoint3F, Offset(missile_pos, afxMagicMissile));
  addField("initialVelocity", TypePoint3F, Offset(missile_vel, afxMagicMissile));
}

bool afxMagicMissile::onAdd()
{
  if (!Parent::onAdd())
    return false;

  if (isClientObject())
  {
    if (!datablock->missile_shape.isNull())
      missile_shape = new TSShapeInstance(datablock->missile_shape, true);

    if (datablock->particleEmitter != NULL)
    {
      ParticleEmitter* pEmitter = new ParticleEmitter;
      pEmitter->onNewDataBlock(datablock->particleEmitter);
      if (pEmitter->registerObject() == false)
      {
        Con::warnf(ConsoleLogEntry::General, "Could not register particle emitter for particle of class: %s", datablock->getName());
        delete pEmitter;
        pEmitter = NULL;
      }
      air_emitter = pEmitter;
    }

    if (datablock->particleWaterEmitter != NULL)
    {
      ParticleEmitter* pEmitter = new ParticleEmitter;
      pEmitter->onNewDataBlock(datablock->particleWaterEmitter);
      if (pEmitter->registerObject() == false)
      {
        Con::warnf(ConsoleLogEntry::General, "Could not register particle emitter for particle of class: %s", datablock->getName());
        delete pEmitter;
        pEmitter = NULL;
      }
      water_emitter = pEmitter;
    }

    if (datablock->hasLight == true)
      Sim::getLightSet()->addObject(this);
  }

  // detect for acceleration
  use_accel = (datablock->acceleration != 0 && datablock->accelLifetime > 0);

  // Setup our bounding box
  if (!datablock->missile_shape.isNull())
    mObjBox = datablock->missile_shape->bounds;
  else
    mObjBox = Box3F(Point3F(0, 0, 0), Point3F(0, 0, 0));
  resetWorldBox();

  addToScene();

  return true;
}

void afxMagicMissile::onRemove()
{
  if (air_emitter) 
  {
    air_emitter->deleteWhenEmpty();
    air_emitter = NULL;
  }

  if (water_emitter) 
  {
    water_emitter->deleteWhenEmpty();
    water_emitter = NULL;
  }

  kill_sound();

  removeFromScene();
  Parent::onRemove();
}

bool afxMagicMissile::onNewDataBlock(GameBaseData* dptr)
{
  datablock = dynamic_cast<afxMagicMissileData*>(dptr);
  if (!datablock || !Parent::onNewDataBlock(dptr))
    return false;

  starting_velocity = datablock->muzzleVelocity;
  starting_vel_vec = datablock->starting_vel_vec;
  collision_mask = datablock->collision_mask;

  return true;
}

void afxMagicMissile::registerLights(LightManager* lightManager, bool lightingScene)
{
  if (lightingScene || !is_active())
    return;

  if (datablock->hasLight) 
  {
    light_info.mType = LightInfo::Point;
    getRenderTransform().getColumn(3, &light_info.mPos);
    light_info.mRadius = datablock->lightRadius;
    if (datablock->hasWaterLight && point_in_water(light_info.mPos))
      light_info.mColor = datablock->waterLightColor;
    else
      light_info.mColor  = datablock->lightColor;
#if defined(AFX_CAP_LIGHT_ILLUMINATION_MASKING)
    light_info.illumObjectTypeMask = datablock->lightIlluminationMask;
#endif

#if defined(TGEA_ENGINE)
    lightManager->sgRegisterGlobalLight(&light_info, this, false);
#else
    lightManager->sgRegisterGlobalLight(&light_info);
#endif
  }
}

void afxMagicMissile::processTick(const Move* move)
{
  // called on client and server
  
  Parent::processTick(move);
  
  // only active from launch to impact
  if (!is_active())
    return;
  
  elapsed_ticks++;
  
  // missile fizzles out by exceeding lifetime
  if ((isServerObject() || client_only) && elapsed_ticks >= datablock->lifetime)
  {
    did_impact = true;
    setMaskBits(ImpactMask);
    if (choreographer)
    {
      Point3F n = missile_vel; n.normalizeSafe();
      choreographer->impactNotify(missile_pos, n, 0);
    }
    Sim::postEvent(this, new ObjectDeleteEvent, Sim::getCurrentTime() + 500);
    return;
  }
    
  static F32 dT = F32(TickMs)*0.001f;
  
  Point3F old_pos = missile_pos;
  
  // adjust missile velocity from gravity and drag influences
  if (datablock->isBallistic)
  {
    F32 dV = (1 - datablock->ballisticCoefficient)*dT;
    Point3F d(missile_vel.x*dV, missile_vel.y*dV, 9.81f*datablock->gravityMod*dT);
    missile_vel -= d;
  }
  
  // adjust missile velocity from acceleration
  if (use_accel)
  {
    if (elapsed_ticks > datablock->accelDelay && 
        elapsed_ticks <= datablock->accelDelay + datablock->accelLifetime)
    {
      Point3F d = missile_vel; d.normalizeSafe();
      missile_vel += d*datablock->acceleration*dT;
    }
  }
  
  // adjust missile_vel from guidance system influences
  if (datablock->isGuided && missile_target && elapsed_ticks > datablock->trackDelay) 
  {
    // get the position tracked by the guidance system
    Point3F target_pos = missile_target->getRenderPosition(); 
    target_pos.z += (missile_target->getObjBox().len_z()/2);
    
    Point3F target_vec = target_pos - missile_pos;

    F32 target_dist_sq = target_vec.lenSquared();
    if (target_dist_sq < 4.0f)
      prec_inc += 1.0f;
    
    // hover
    if (datablock->hover_altitude > 0.0f)
    {
      Point3F target_vec_xy(target_vec.x, target_vec.y, 0);
      F32 xy_dist = target_vec_xy.len();
      
      if (xy_dist > datablock->hover_attack_distance)
      {          
        hover_attack_go = false;
        
        if (xy_dist > datablock->hover_attack_distance + datablock->hover_attack_gradient)
        {
          target_pos.z += datablock->hover_altitude;          
        }
        else
        {
          target_pos.z += afxEase::eq( (xy_dist-datablock->hover_attack_distance)/datablock->hover_attack_gradient, 
            0.0f, datablock->hover_altitude, 
            0.25f, 0.75f);
        }          			
        target_vec = target_pos - missile_pos;
      }
      
      else
      {
        if (!hover_attack_go) 
        {
          hover_attack_go = true;
          hover_attack_tick = 0;
        }
        hover_attack_tick++;
        
        if (hover_attack_tick < datablock->hover_time)
        {
          target_pos.z += datablock->hover_altitude;
          target_vec = target_pos - missile_pos;
        }
      }
    }
    
    // apply precision 
    
    // extract speed
    F32 speed = missile_vel.len(); 
    
    // normalize vectors
    target_vec.normalizeSafe();
    missile_vel.normalize();
    
    F32 prec = datablock->precision;

    // fade in precision gradually to avoid sudden turn
    if (elapsed_ticks < datablock->trackDelay + 16)
      prec *= (elapsed_ticks - datablock->trackDelay)/16.0f;

    prec += prec_inc;
    if (prec > 100)
      prec = 100;

    // apply precision weighting
    target_vec *= prec;
    missile_vel *= (100 - prec);
    
    missile_vel += target_vec;
    missile_vel.normalize();
    missile_vel *= speed;
  } 
  
  // wiggle
  for (U32 i = 0; i < datablock->wiggle_num_axis; i++)
  {
    if (i >= datablock->wiggle_magnitudes.size() || i >= datablock->wiggle_speeds.size()) 
      break;

    F32 wiggle_mag   = datablock->wiggle_magnitudes[i];
    F32 wiggle_speed = datablock->wiggle_speeds[i];
    Point3F wiggle_axis = datablock->wiggle_axis[i];
    //wiggle_axis.normalizeSafe(); // sufficient????

    F32 theta = wiggle_mag * mSin(wiggle_speed*(elapsed_ticks*TickSec));
    //Con::printf( "theta: %f", theta );    
    AngAxisF thetaRot(wiggle_axis, theta);
    MatrixF temp(true);
    thetaRot.setMatrix(&temp);
    temp.mulP(missile_vel);
  }

  Point3F new_pos = old_pos + missile_vel*dT;

  // conform to terrain
  if (datablock->followTerrain && elapsed_ticks >= datablock->followTerrainAdjustDelay) 
  {
    U32 mask = TerrainObjectType; //  | InteriorObjectType;
#if defined(TGEA_ENGINE)
    mask |= AtlasObjectType;
#endif

    F32 ht = datablock->followTerrainHeight;
    F32 ht_rate = datablock->followTerrainAdjustRate;
    F32 ht_min = 0.05f;
    if (ht < ht_min)
      ht = ht_min;

    Point3F above_pos = new_pos; above_pos.z += 10000;
    Point3F below_pos = new_pos; below_pos.z -= 10000;
    RayInfo rInfo;
    if (gClientContainer.castRay(above_pos, below_pos, mask, &rInfo)) 
    {
      F32 terrain_z = rInfo.point.z;
      F32 seek_z = terrain_z + ht;
      if (new_pos.z < seek_z)
      {
        new_pos.z += ht_rate*dT;
        if (new_pos.z > seek_z)
          new_pos.z = seek_z;
      }
      else if (new_pos.z > seek_z)
      {
        new_pos.z -= ht_rate*dT;
        if (new_pos.z < seek_z)
          new_pos.z = seek_z;
      }

      if (new_pos.z < terrain_z + ht_min)
        new_pos.z = terrain_z + ht_min;
    }
  }

  // only check for impacts on server
  if (isServerObject())
  {
    // avoid collision with the spellcaster
    if (collide_exempt)
      collide_exempt->disableCollision();
  
    // check for collision along ray from old to new position
    RayInfo rInfo;
    bool did_hit = getContainer()->castRay(old_pos, new_pos, collision_mask, &rInfo);
  
    // restore collisions on spellcaster 
    if (collide_exempt)
      collide_exempt->enableCollision();
  
    // process impact
    if (did_hit)
    {
      MatrixF xform(true);
      xform.setColumn(3, rInfo.point);
      setTransform(xform);
      missile_pos = rInfo.point;
      missile_vel = Point3F(0, 0, 0);
      did_impact = true;
      setMaskBits(ImpactMask);
      if (choreographer)
      {
        choreographer->impactNotify(rInfo.point, rInfo.normal, rInfo.object);
        Sim::postEvent(this, new ObjectDeleteEvent, Sim::getCurrentTime() + 500);
      }
    }
  }
  else // if (isClientObject())
  {
    emit_particles(missile_pos, new_pos, missile_vel, TickMs);
    update_sound();
  }
  
  // interp values used in interpolateTick()
  interp_delta_base = new_pos;
  interp_back_delta = missile_pos - new_pos;

  missile_pos = new_pos;
  
  MatrixF xform(true);
  xform.setColumn(3, missile_pos);
  setTransform(xform);
}

void afxMagicMissile::interpolateTick(F32 delta)
{
  // client-only call
  Parent::interpolateTick(delta);
  
  // only active from launch to impact
  if (!is_active())
    return;
  
  // interpolate position and get normalized velocity vector
  Point3F interp_pos = interp_delta_base + interp_back_delta*delta;

  Point3F dir = missile_vel;
  if (dir.isZero())
    dir.set(0,0,1);
  else
    dir.normalize();
  
  // update missile position with interpolated position
  MatrixF xform = MathUtils::createOrientFromDir(dir);
  xform.setPosition(interp_pos);
  setRenderTransform(xform);
  
  // update the audio
  update_sound();
}

U32 afxMagicMissile::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
{
  U32 retMask = Parent::packUpdate(conn, mask, stream);

  // Initial update
  if (stream->writeFlag(mask & GameBase::InitialUpdateMask))
  {
    Point3F pos;
    getTransform().getColumn(3, &pos);
    stream->writeCompressedPoint(pos);
    F32 len = missile_vel.len();
    if(stream->writeFlag(len > 0.02))
    {
      Point3F outVel = missile_vel;
      outVel *= 1 / len;
      stream->writeNormalVector(outVel, 10);
      len *= 32.0; // 5 bits for fraction
      if(len > 8191)
        len = 8191;
      stream->writeInt((S32)len, 13);
    }

    stream->writeRangedU32(elapsed_ticks, 0, afxMagicMissileData::MaxLifetimeTicks);

    if (choreographer)
    {
      S32 ghostIndex = conn->getGhostIndex(choreographer);
      if (stream->writeFlag(ghostIndex != -1))
        stream->writeRangedU32(U32(ghostIndex), 0, NetConnection::MaxGhostCount);
      else // no ghost yet, try again later
        retMask |= GameBase::InitialUpdateMask;
    }
    else
      stream->writeFlag(false);
  }

  // impact update
  if (stream->writeFlag(mask & ImpactMask))
  {
    mathWrite(*stream, missile_pos);
    mathWrite(*stream, missile_vel);
    stream->writeFlag(did_impact);
  }

  // guided update
  if (stream->writeFlag(mask & GuideMask))
  {
    mathWrite(*stream, missile_pos);
    mathWrite(*stream, missile_vel);
  }   

  return retMask;
}

void afxMagicMissile::unpackUpdate(NetConnection* conn, BitStream* stream)
{
  Parent::unpackUpdate(conn, stream);

  // initial update
  if (stream->readFlag())
  {
    Point3F pos;
    stream->readCompressedPoint(&pos);
    if(stream->readFlag())
    {
      stream->readNormalVector(&missile_vel, 10);
      missile_vel *= stream->readInt(13) / 32.0f;
    }
    else
      missile_vel.set(0, 0, 0);

    // interp values used in interpolateTick()
    interp_delta_base = pos;
    interp_back_delta = missile_pos - pos;

    missile_pos  = pos;
    setPosition(missile_pos);

    elapsed_ticks = stream->readRangedU32(0, afxMagicMissileData::MaxLifetimeTicks);

    if (stream->readFlag())
    {
      U32 id   = stream->readRangedU32(0, NetConnection::MaxGhostCount);
      choreographer = dynamic_cast<afxChoreographer*>(conn->resolveGhost(id));
      if (choreographer)
      {
        deleteNotify(choreographer);
      }
    }
    else
    {
      if (choreographer)
        clearNotify(choreographer);
      choreographer = 0;
    }
  }

  // impact update
  if (stream->readFlag())
  {
    mathRead(*stream, &missile_pos);
    mathRead(*stream, &missile_vel);
    did_impact = stream->readFlag();
    kill_sound();
  }

  // guided update
  if (stream->readFlag())
  {
    mathRead(*stream, &missile_pos);
    mathRead(*stream, &missile_vel);
  }
}

void afxMagicMissile::onDeleteNotify(SimObject* obj)
{
  ShapeBase* shape_test = dynamic_cast<ShapeBase*>(obj);
  if (shape_test == collide_exempt)
  {
    collide_exempt = NULL;
    Parent::onDeleteNotify(obj);
    return;
  }

  SceneObject* target_test = dynamic_cast<SceneObject*>(obj);
  if (target_test == missile_target)
  {
    missile_target = NULL;
    Parent::onDeleteNotify(obj);
    return;
  }

  afxChoreographer* ch = dynamic_cast<afxChoreographer*>(obj);
  if (ch == choreographer)
  {
    choreographer = NULL;
    Parent::onDeleteNotify(obj);
    return;
  }

  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// private:

void afxMagicMissile::init(bool on_server, bool on_client)
{
  mTypeMask |= ProjectileObjectType;

  datablock = NULL;
  choreographer = NULL;

  if (on_server != on_client)
  {
    client_only = on_client;
    server_only = on_server;
    mNetFlags.clear(Ghostable | ScopeAlways);
    if (client_only)
      mNetFlags.set(IsGhost);
  }
  else
  {
    // note -- neither server or client set makes no sense so we
    // treat as if both are set.
    mNetFlags.set(Ghostable | ScopeAlways);
    client_only = server_only = false;
  }

  elapsed_ticks = 0;
  use_accel = false;
  air_emitter   = NULL;
  water_emitter = NULL;
  sound_handle = NULL_AUDIOHANDLE;
  missile_pos.zero();
  missile_vel.set(0, 0, 1);
  missile_shape   = NULL;

  light_info.mType = LightInfo::Point;
  light_info.mPos.zero();
  light_info.mDirection.zero();
  light_info.mColor.set(0,0,0);
  light_info.mAmbient.set(0,0,0);
  light_info.mRadius = 0;

  interp_delta_base.zero();
  interp_back_delta.zero();
  collision_mask = 0;
  prec_inc = 0.0f;

  did_launch = false;
  did_impact = false;

  missile_target = NULL;
  collide_exempt = NULL;

  hover_attack_go = false;
  hover_attack_tick = 0;

  starting_velocity = 0.0;
  starting_vel_vec.zero();
}

bool afxMagicMissile::point_in_water(const Point3F &point)
{
#if !defined(BROKEN_SPLASH_CODE)
  SimpleQueryList sql;
  if (isServerObject())
    gServerSceneGraph->getWaterObjectList(sql);
  else
    gClientSceneGraph->getWaterObjectList(sql);

  for (U32 i = 0; i < sql.mList.size(); i++)
  {
    WaterBlock* pBlock = dynamic_cast<WaterBlock*>(sql.mList[i]);
    if (pBlock && pBlock->isPointSubmergedSimple(point))
      return true;
  }
#endif
  return false;
}

void afxMagicMissile::create_splash(const Point3F& pos)
{
#if !defined(BROKEN_SPLASH_CODE)
  if (!datablock || !datablock->splash)
    return;

  MatrixF xfm = getTransform();
  xfm.setPosition(pos);

  Splash* splash = new Splash();
  splash->onNewDataBlock(datablock->splash);
  splash->setTransform(xfm);
  splash->setInitialState(xfm.getPosition(), Point3F(0.0, 0.0, 1.0));
  if (!splash->registerObject())
  {
    delete splash;
    splash = NULL;
  }
#endif
}

void afxMagicMissile::get_launch_data(Point3F& pos, Point3F& vel)
{
  if (!choreographer)
    return;

  // get caster from choreographer (currently magic-spell only)
  ShapeBase* caster = NULL;
  if (dynamic_cast<afxMagicSpell*>(choreographer))
    caster = ((afxMagicSpell*)choreographer)->caster;
  if (!caster)
    return;

  MatrixF caster_xfm = caster->getRenderTransform();

  // calculate launch position
  Point3F offset_override = (isClientObject()) ?  datablock->launch_offset_client : 
                                                  datablock->launch_offset_server;
  // override
  if (!offset_override.isZero())
  {
    caster_xfm.mulV(offset_override);
    pos = caster->getRenderPosition() + offset_override;
  }
  // no override 
  else
  {
    // get transformed launch offset
    VectorF launch_offset = datablock->launch_offset;
    caster_xfm.mulV(launch_offset);
    
    StringTableEntry launch_node = datablock->launch_node;
    
    // calculate position of missile at launch
    if (launch_node != ST_NULLSTRING)
    {
      TSShapeInstance* shape_inst = caster->getShapeInstance();
      if (!shape_inst || !shape_inst->getShape())
        launch_node = ST_NULLSTRING;
      else
      {
        S32 node_ID = shape_inst->getShape()->findNode(launch_node);
        MatrixF node_xfm = caster->getRenderTransform();
        node_xfm.scale(caster->getScale());
        node_xfm.mul(shape_inst->mNodeTransforms[node_ID]);
        /* before
        MatrixF node_xfm;
        node_xfm.mul(caster->getRenderTransform(), shape_inst->mNodeTransforms[node_ID]);
        */
        
        VectorF node_offset = datablock->launch_node_offset;
        node_xfm.mulV(node_offset);
        
        pos = node_xfm.getPosition() + launch_offset + node_offset;
      }
    }   
    // calculate launch position without launch node
    else
      pos = caster->getRenderPosition() + launch_offset;
  }

  if (datablock->echo_launch_offset)
  {
    VectorF offset = pos - caster->getRenderPosition();
    MatrixF caster_xfm_inv = caster_xfm;
    caster_xfm_inv.affineInverse();
    caster_xfm_inv.mulV(offset);
    if (isServerObject())
      Con::printf("launchOffsetServer = \"%g %g %g\";", offset.x, offset.y, offset.z);
    else
      Con::printf("launchOffsetClient = \"%g %g %g\";", offset.x, offset.y, offset.z);
  }

  // calculate launch velocity vector
  if (starting_vel_vec.isZero())
  {
    // setup aiming matrix to straight forward and level
    MatrixF aim_mtx;
    AngAxisF aim_aa(Point3F(0,1,0),0);
    aim_aa.setMatrix(&aim_mtx);
    
    // setup pitch matrix
    MatrixF pitch_mtx;
    AngAxisF pitch_aa(Point3F(1,0,0),mDegToRad(datablock->launch_pitch));
    pitch_aa.setMatrix(&pitch_mtx);
    
    // setup pan matrix
    MatrixF pan_mtx;
    AngAxisF pan_aa(Point3F(0,0,1),mDegToRad(datablock->launch_pan));
    pan_aa.setMatrix(&pan_mtx);
    
    // calculate adjusted aiming matrix
    aim_mtx.mul(pitch_mtx);
    aim_mtx.mul(pan_mtx);
    
    // calculate final aiming vector
    MatrixF aim2_mtx;
    aim2_mtx.mul(caster_xfm, aim_mtx);
    VectorF aim_vec;
    aim2_mtx.getColumn(1,&aim_vec);
    aim_vec.normalizeSafe();
    
    // give velocity vector a magnitude
    vel = aim_vec*datablock->muzzleVelocity;
  }
  else
  {
    vel = starting_vel_vec*starting_velocity;
  }
}

void afxMagicMissile::emit_particles(const Point3F& from, const Point3F& to, 
                                     const Point3F& vel, const U32 ms)
{
  Point3F axis = -vel;
  if (axis.isZero())
    axis.set(0.0, 0.0, 1.0);
  else
    axis.normalize();

  bool fromWater = point_in_water(from);
  bool toWater   = point_in_water(to);

  // not in water
  if (!fromWater && !toWater && air_emitter)                                        
    air_emitter->emitParticles(from, to, axis, vel, ms);

  // in water
  else if (fromWater && toWater && water_emitter)                                
    water_emitter->emitParticles(from, to, axis, vel, ms);

  // entering water
  else if (!fromWater && toWater && air_emitter && water_emitter)     
  {
    RayInfo rInfo;
    if (gClientContainer.castRay(from, to, WaterObjectType, &rInfo))
    {
      create_splash(rInfo.point);
      air_emitter->emitParticles(from, rInfo.point, axis, vel, ms);
      water_emitter->emitParticles(rInfo.point, to, axis, vel, ms);
    }
  }

  // leaving water
  else if (fromWater && !toWater && air_emitter && water_emitter)     
  {
    RayInfo rInfo;
    if (gClientContainer.castRay(to, from, WaterObjectType, &rInfo))
    {
      create_splash(rInfo.point);
      air_emitter->emitParticles(rInfo.point, to, axis, vel, ms);
      water_emitter->emitParticles(from, rInfo.point, axis, vel, ms);
    }
  }
}

void afxMagicMissile::update_sound()
{
  if (!datablock->sound)
    return;

  if (sound_handle == NULL_AUDIOHANDLE)
    sound_handle = alxPlay(datablock->sound, &getRenderTransform(), &getRenderPosition());
  alxSourceMatrixF(sound_handle, &getRenderTransform());
}

void afxMagicMissile::kill_sound()
{
  if (sound_handle != NULL_AUDIOHANDLE) 
  {
    alxStop(sound_handle);
    sound_handle = NULL_AUDIOHANDLE;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// public:

void afxMagicMissile::launch()
{
  get_launch_data(missile_pos, missile_vel);
  did_launch = true;

  afxMagicSpell* spell = dynamic_cast<afxMagicSpell*>(choreographer);
  if (spell)
  {
    missile_target = spell->target;
    collide_exempt = spell->caster;

    if (spell->caster)
      processAfter(spell->caster);
    if (missile_target)
      deleteNotify(missile_target);
    if (collide_exempt)
      deleteNotify(collide_exempt);
  }
  else
  {
    missile_target = 0;
    collide_exempt = 0;
  }
}

void afxMagicMissile::setChoreographer(afxChoreographer* chor)
{
  if (choreographer)
    clearNotify(choreographer);
  choreographer = chor;
  if (choreographer)
    deleteNotify(choreographer);
}

void afxMagicMissile::setStartingVelocityVector(const Point3F& vel_vec)
{
  starting_vel_vec = vel_vec;
}

void afxMagicMissile::setStartingVelocity(const F32 vel)
{
  starting_velocity = vel;
}

void afxMagicMissile::getStartingVelocityValues(F32& vel, Point3F& vel_vec)
{
  vel = starting_velocity;
  vel_vec = starting_vel_vec;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

ConsoleMethod(afxMagicMissile, setStartingVelocityVector, void, 3, 3, "(Point3F velocity_vec)") 
{
  VectorF vel_vec(0,0,0);
  dSscanf(argv[2],"%g %g %g",&vel_vec.x,&vel_vec.y,&vel_vec.z);
  object->setStartingVelocityVector(vel_vec);
}

ConsoleMethod(afxMagicMissile, setStartingVelocity, void, 3, 3, "(F32 velocity)") 
{
  object->setStartingVelocity(dAtof(argv[2]));
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
