//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "console/consoleTypes.h"
#include "console/typeValidators.h"
#include "core/bitStream.h"
#include "game/fx/explosion.h"
#include "game/fx/splash.h"
#include "game/shapeBase.h"
#include "ts/tsShapeInstance.h"
#include "game/projectile.h"
#include "audio/audioDataBlock.h"
#include "math/mathUtils.h"
#include "math/mathIO.h"
#include "sim/netConnection.h"
#include "terrain/waterBlock.h"
#include "game/fx/particleEngine.h"
#include "sim/decalManager.h"

IMPLEMENT_CO_DATABLOCK_V1(ProjectileData);
IMPLEMENT_CO_NETOBJECT_V1(Projectile);

const U32 Projectile::csmStaticCollisionMask =  TerrainObjectType    |
                                                InteriorObjectType   |
                                                StaticObjectType;

const U32 Projectile::csmDynamicCollisionMask = PlayerObjectType        |
                                                VehicleObjectType       |
                                                DamagableItemObjectType;

const U32 Projectile::csmDamageableMask = Projectile::csmDynamicCollisionMask;

U32 Projectile::smProjectileWarpTicks = 5;


//--------------------------------------------------------------------------
//
ProjectileData::ProjectileData()
{
   projectileShapeName = NULL;

   sound = NULL;
   soundId = 0;

   explosion = NULL;
   explosionId = 0;

   waterExplosion = NULL;
   waterExplosionId = 0;

   splash = NULL;
   splashId = 0;

   hasLight = false;
   lightRadius = 1;
   lightColor.set(1, 1, 1);

   hasWaterLight = false;
   waterLightColor.set(1, 1, 1);

   faceViewer = false;
   scale.set( 1.0, 1.0, 1.0 );

   isBallistic = false;

	velInheritFactor = 1.0;
	muzzleVelocity = 50;

	armingDelay = 0;
   fadeDelay = 20000 / 32;
   lifetime = 20000 / 32;

   activateSeq = -1;
   maintainSeq = -1;

   gravityMod = 1.0;
   bounceElasticity = 0.999;
   bounceFriction = 0.3;

   particleEmitter = NULL;
   particleEmitterId = 0;

   particleWaterEmitter = NULL;
   particleWaterEmitterId = 0;

   decalCount = 0;
   for (U32 i = 0; i < NumDecals; i++)
   {
      decals[i] = NULL;
      decalId[i] = 0;
   }
}

//--------------------------------------------------------------------------
IMPLEMENT_CONSOLETYPE(ProjectileData)
IMPLEMENT_GETDATATYPE(ProjectileData)
IMPLEMENT_SETDATATYPE(ProjectileData)

void ProjectileData::initPersistFields()
{
   Parent::initPersistFields();

   addNamedField(particleEmitter,  TypeParticleEmitterDataPtr, ProjectileData);
   addNamedField(particleWaterEmitter, TypeParticleEmitterDataPtr, ProjectileData);

   addNamedField(projectileShapeName, TypeFilename, ProjectileData);
   addNamedField(scale, TypePoint3F, ProjectileData);

   addNamedField(sound, TypeAudioProfilePtr, ProjectileData);

   addNamedField(explosion, TypeExplosionDataPtr, ProjectileData);
   addNamedField(waterExplosion, TypeExplosionDataPtr, ProjectileData);

   addNamedField(splash, TypeSplashDataPtr, ProjectileData);
   addField("decals", TypeDecalDataPtr, Offset(decals, ProjectileData), NumDecals);

   addNamedField(hasLight, TypeBool, ProjectileData);
   addNamedFieldV(lightRadius, TypeF32, ProjectileData, new FRangeValidator(1, 20));
   addNamedField(lightColor, TypeColorF, ProjectileData);
	
   addNamedField(hasWaterLight, TypeBool, ProjectileData);
   addNamedField(waterLightColor, TypeColorF, ProjectileData);

   addNamedField(isBallistic, TypeBool, ProjectileData);
	addNamedFieldV(velInheritFactor, TypeF32, ProjectileData, new FRangeValidator(0, 1));
   addNamedFieldV(muzzleVelocity, TypeF32, ProjectileData, new FRangeValidator(0, 10000));

	addNamedFieldV(lifetime, TypeS32, ProjectileData, new IRangeValidatorScaled(TickMs, 0, Projectile::MaxLivingTicks));
   addNamedFieldV(armingDelay, TypeS32, ProjectileData, new IRangeValidatorScaled(TickMs, 0, Projectile::MaxLivingTicks));
   addNamedFieldV(fadeDelay, TypeS32, ProjectileData, new IRangeValidatorScaled(TickMs, 0, Projectile::MaxLivingTicks));

   addNamedFieldV(bounceElasticity, TypeF32, ProjectileData, new FRangeValidator(0, 0.999));
   addNamedFieldV(bounceFriction, TypeF32, ProjectileData, new FRangeValidator(0, 1));
   addNamedFieldV(gravityMod, TypeF32, ProjectileData, new FRangeValidator(0, 1));
}


//--------------------------------------------------------------------------
bool ProjectileData::onAdd()
{
   if(!Parent::onAdd())
      return false;

   if (!particleEmitter && particleEmitterId != 0)
      if (Sim::findObject(particleEmitterId, particleEmitter) == false)
         Con::errorf(ConsoleLogEntry::General, "ProjectileData::onAdd: Invalid packet, bad datablockId(particleEmitter): %d", particleEmitterId);

   if (!particleWaterEmitter && particleWaterEmitterId != 0)
      if (Sim::findObject(particleWaterEmitterId, particleWaterEmitter) == false)
         Con::errorf(ConsoleLogEntry::General, "ProjectileData::onAdd: Invalid packet, bad datablockId(particleWaterEmitter): %d", particleWaterEmitterId);

   if (!explosion && explosionId != 0)
      if (Sim::findObject(explosionId, explosion) == false)
         Con::errorf(ConsoleLogEntry::General, "ProjectileData::onAdd: Invalid packet, bad datablockId(explosion): %d", explosionId);

   if (!waterExplosion && waterExplosionId != 0)
      if (Sim::findObject(waterExplosionId, waterExplosion) == false)
         Con::errorf(ConsoleLogEntry::General, "ProjectileData::onAdd: Invalid packet, bad datablockId(waterExplosion): %d", waterExplosionId);

   if (!splash && splashId != 0)
      if (Sim::findObject(splashId, splash) == false)
         Con::errorf(ConsoleLogEntry::General, "ProjectileData::onAdd: Invalid packet, bad datablockId(splash): %d", splashId);

   if (!sound && soundId != 0)
      if (Sim::findObject(soundId, sound) == false)
         Con::errorf(ConsoleLogEntry::General, "ProjectileData::onAdd: Invalid packet, bad datablockid(sound): %d", soundId);

   lightColor.clamp();
   waterLightColor.clamp();

   return true;
}


bool ProjectileData::preload(bool server, char errorBuffer[256])
{
   if (Parent::preload(server, errorBuffer) == false)
      return false;

   if (projectileShapeName && projectileShapeName[0] != '\0')
   {
      projectileShape = ResourceManager->load(projectileShapeName);
      if (bool(projectileShape) == false)
      {
         dSprintf(errorBuffer, sizeof(errorBuffer), "ProjectileData::load: Couldn't load shape \"%s\"", projectileShapeName);
         return false;
      }
      activateSeq = projectileShape->findSequence("activate");
      maintainSeq = projectileShape->findSequence("maintain");
   }

   if (bool(projectileShape)) // create an instance to preload shape data
   {
      TSShapeInstance* pDummy = new TSShapeInstance(projectileShape, !server);
      delete pDummy;
   }

   // load up all the supplied decal datablocks
   //  move non-null ones to the front of the array
   //  for our random decal picker later
   U32 i;
   DecalData *tmpDecals[NumDecals];
   for (i = 0; i < NumDecals; i++)
   {
      tmpDecals[i] = NULL;
      if(!decals[i] && decalId[i] != 0)
         if(!Sim::findObject(decalId[i], decals[i]))
            Con::errorf( ConsoleLogEntry::General, "ProjectileData::preload Invalid packet, bad datablockId(decals): 0x%x", decalId[i]);

      if (!server && decals[i])
      {
         tmpDecals[decalCount] = decals[i];
         decalCount++;
      }
   }
   if (!server && decalCount > 0)
      for (i = 0; i < NumDecals; i++)
         decals[i] = tmpDecals[i];

   return true;
}

//--------------------------------------------------------------------------
void ProjectileData::packData(BitStream* stream)
{
   Parent::packData(stream);

   stream->writeString(projectileShapeName);
   stream->writeFlag(faceViewer);
   if(stream->writeFlag(scale.x != 1 || scale.y != 1 || scale.z != 1))
   {
      stream->write(scale.x);
      stream->write(scale.y);
      stream->write(scale.z);
   }

   if (stream->writeFlag(particleEmitter != NULL))
      stream->writeRangedU32(particleEmitter->getId(), DataBlockObjectIdFirst,
                                                   DataBlockObjectIdLast);
   if (stream->writeFlag(particleWaterEmitter != NULL))
      stream->writeRangedU32(particleWaterEmitter->getId(), DataBlockObjectIdFirst,
                                                   DataBlockObjectIdLast);
   if (stream->writeFlag(explosion != NULL))
      stream->writeRangedU32(explosion->getId(), DataBlockObjectIdFirst,
                                                 DataBlockObjectIdLast);
   if (stream->writeFlag(waterExplosion != NULL))
      stream->writeRangedU32(waterExplosion->getId(), DataBlockObjectIdFirst,
                                                      DataBlockObjectIdLast);
   if (stream->writeFlag(splash != NULL))
      stream->writeRangedU32(splash->getId(), DataBlockObjectIdFirst,
                                              DataBlockObjectIdLast);
   if (stream->writeFlag(sound != NULL))
      stream->writeRangedU32(sound->getId(), DataBlockObjectIdFirst,
                                             DataBlockObjectIdLast);
   for (U32 i = 0; i < NumDecals; i++)
      if (stream->writeFlag(decals[i] != NULL))
         stream->writeRangedU32(decals[i]->getId(), DataBlockObjectIdFirst,
                                                 DataBlockObjectIdLast);

   if(stream->writeFlag(hasLight))
   {
      stream->writeFloat(lightRadius/20.0, 8);
      stream->writeFloat(lightColor.red,7);
      stream->writeFloat(lightColor.green,7);
      stream->writeFloat(lightColor.blue,7);
   }

   if(stream->writeFlag(hasWaterLight))
   {
      stream->writeFloat(waterLightColor.red, 7);
      stream->writeFloat(waterLightColor.green, 7);
      stream->writeFloat(waterLightColor.blue, 7);
   }

   stream->writeRangedU32(lifetime, 0, Projectile::MaxLivingTicks);
   stream->writeRangedU32(armingDelay, 0, Projectile::MaxLivingTicks);
   stream->writeRangedU32(fadeDelay, 0, Projectile::MaxLivingTicks);

   if(stream->writeFlag(isBallistic))
   {
      stream->write(gravityMod);
      stream->write(bounceElasticity);
      stream->write(bounceFriction);
   }

   // Neither velInheritVelocity nor muzzleVelocity are transmitted to the
   // client. This is because in stock Torque, it is not needed for the
   // client-side simulation - in general, it is good design to not transmit
   // useless information. You could easily add stream->write() calls here,
   // and read calls in unpackData, if you did have need of them.
}

void ProjectileData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   projectileShapeName = stream->readSTString();

   faceViewer = stream->readFlag();
   if(stream->readFlag())
   {
      stream->read(&scale.x);
      stream->read(&scale.y);
      stream->read(&scale.z);
   }
   else
      scale.set(1,1,1);

   if (stream->readFlag())
      particleEmitterId = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   if (stream->readFlag())
      particleWaterEmitterId = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   if (stream->readFlag())
      explosionId = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   if (stream->readFlag())
      waterExplosionId = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   if (stream->readFlag())
      splashId = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   if (stream->readFlag())
      soundId = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   for (U32 i = 0; i < NumDecals; i++)
      if (stream->readFlag())
         decalId[i] = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);

   hasLight = stream->readFlag();
   if(hasLight)
   {
      lightRadius = stream->readFloat(8) * 20;
      lightColor.red = stream->readFloat(7);
      lightColor.green = stream->readFloat(7);
      lightColor.blue = stream->readFloat(7);
   }
   hasWaterLight = stream->readFlag();
   if(hasWaterLight)
   {
      waterLightColor.red = stream->readFloat(7);
      waterLightColor.green = stream->readFloat(7);
      waterLightColor.blue = stream->readFloat(7);
   }
   lifetime = stream->readRangedU32(0, Projectile::MaxLivingTicks);
   armingDelay = stream->readRangedU32(0, Projectile::MaxLivingTicks);
   fadeDelay = stream->readRangedU32(0, Projectile::MaxLivingTicks);

   isBallistic = stream->readFlag();
   if(isBallistic)
   {
      stream->read(&gravityMod);
      stream->read(&bounceElasticity);
      stream->read(&bounceFriction);
   }
}


//--------------------------------------------------------------------------
//--------------------------------------
//
Projectile::Projectile()
{
   // Todo: ScopeAlways?
   mNetFlags.set(Ghostable);
   mTypeMask |= ProjectileObjectType;

   mCurrPosition.set(0, 0, 0);
   mCurrVelocity.set(0, 0, 1);

   mSourceObjectId = -1;
   mSourceObjectSlot = -1;

   mCurrTick         = 0;

   mParticleEmitter   = NULL;
   mParticleWaterEmitter = NULL;

   mSoundHandle = NULL_AUDIOHANDLE;

   mProjectileShape   = NULL;
   mActivateThread    = NULL;
	mMaintainThread    = NULL;

   mCollideHitType   = 0;

   mHidden           = false;
   mFadeValue        = 1.0;

   mLastRenderPos.set(0.0f,0.0f,0.0f);
}

Projectile::~Projectile()
{
   delete mProjectileShape;
   mProjectileShape = NULL;
}

//--------------------------------------------------------------------------
void Projectile::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Physics");
   addField("initialPosition",  TypePoint3F, Offset(mCurrPosition, Projectile));
   addField("initialVelocity", TypePoint3F, Offset(mCurrVelocity, Projectile));
   endGroup("Physics");

   addGroup("Source");
   addField("sourceObject",     TypeS32,     Offset(mSourceObjectId, Projectile));
   addField("sourceSlot",       TypeS32,     Offset(mSourceObjectSlot, Projectile));
   endGroup("Source");
}

bool Projectile::calculateImpact(float,
                                 Point3F& pointOfImpact,
                                 float&   impactTime)
{
   Con::warnf(ConsoleLogEntry::General, "Projectile::calculateImpact: Should never be called");

   impactTime = 0;
   pointOfImpact.set(0, 0, 0);
   return false;
}


//--------------------------------------------------------------------------
F32 Projectile::getUpdatePriority(CameraScopeQuery *camInfo, U32 updateMask, S32 updateSkips)
{
   F32 ret = Parent::getUpdatePriority(camInfo, updateMask, updateSkips);
   // if the camera "owns" this object, it should have a slightly higher priority
   if(mSourceObject == camInfo->camera)
      return ret + 0.2;
   return ret;
}

bool Projectile::onAdd()
{
   if(!Parent::onAdd())
      return false;

   if (isServerObject())
   {
      ShapeBase* ptr;
      if (Sim::findObject(mSourceObjectId, ptr))
         mSourceObject = ptr;
      else
      {
         if (mSourceObjectId != -1)
            Con::errorf(ConsoleLogEntry::General, "Projectile::onAdd: mSourceObjectId is invalid");
         mSourceObject = NULL;
      }

      mCurrTick = 0;
   }
   else
   {
      if (bool(mDataBlock->projectileShape))
      {
         mProjectileShape = new TSShapeInstance(mDataBlock->projectileShape, isClientObject());

         if (mDataBlock->activateSeq != -1)
         {
            mActivateThread = mProjectileShape->addThread();
            mProjectileShape->setTimeScale(mActivateThread, 1);
            mProjectileShape->setSequence(mActivateThread, mDataBlock->activateSeq, 0);
         }
      }
      if (mDataBlock->particleEmitter != NULL)
      {
         ParticleEmitter* pEmitter = new ParticleEmitter;
         pEmitter->onNewDataBlock(mDataBlock->particleEmitter);
         if (pEmitter->registerObject() == false)
         {
            Con::warnf(ConsoleLogEntry::General, "Could not register particle emitter for particle of class: %s", mDataBlock->getName());
            delete pEmitter;
            pEmitter = NULL;
         }
         mParticleEmitter = pEmitter;
      }
      if (mDataBlock->particleWaterEmitter != NULL)
      {
         ParticleEmitter* pEmitter = new ParticleEmitter;
         pEmitter->onNewDataBlock(mDataBlock->particleWaterEmitter);
         if (pEmitter->registerObject() == false)
         {
            Con::warnf(ConsoleLogEntry::General, "Could not register particle emitter for particle of class: %s", mDataBlock->getName());
            delete pEmitter;
            pEmitter = NULL;
         }
         mParticleWaterEmitter = pEmitter;
      }
      if (mDataBlock->hasLight == true)
         Sim::getLightSet()->addObject(this);
   }
   if (bool(mSourceObject))
      processAfter(mSourceObject);

   // Setup our bounding box
   if (bool(mDataBlock->projectileShape) == true)
      mObjBox = mDataBlock->projectileShape->bounds;
   else
      mObjBox = Box3F(Point3F(0, 0, 0), Point3F(0, 0, 0));
   resetWorldBox();
   addToScene();

   return true;
}


void Projectile::onRemove()
{
   if (bool(mParticleEmitter)) {
      mParticleEmitter->deleteWhenEmpty();
      mParticleEmitter = NULL;
   }
   if (bool(mParticleWaterEmitter)) {
      mParticleWaterEmitter->deleteWhenEmpty();
      mParticleWaterEmitter = NULL;
   }
   if (mSoundHandle != NULL_AUDIOHANDLE) {
      alxStop(mSoundHandle);
      mSoundHandle = NULL_AUDIOHANDLE;
   }

   removeFromScene();
   Parent::onRemove();
}


bool Projectile::onNewDataBlock(GameBaseData* dptr)
{
   mDataBlock = dynamic_cast<ProjectileData*>(dptr);
   if (!mDataBlock || !Parent::onNewDataBlock(dptr))
      return false;

   return true;
}


//--------------------------------------------------------------------------

void Projectile::registerLights(LightManager * lightManager, bool lightingScene)
{
   if(lightingScene)
      return;

   if (mDataBlock->hasLight && mHidden == false)
   {
      mLight.mType = LightInfo::Point;
      getRenderTransform().getColumn(3, &mLight.mPos);
      mLight.mRadius = mDataBlock->lightRadius;
      if (mDataBlock->hasWaterLight && pointInWater(mLight.mPos))
         mLight.mColor = mDataBlock->waterLightColor;
      else
      mLight.mColor  = mDataBlock->lightColor;
      lightManager->sgRegisterGlobalLight(&mLight);
   }
}

//----------------------------------------------------------------------------

void Projectile::emitParticles(const Point3F& from, const Point3F& to, const Point3F& vel, const U32 ms)
{
   if( mHidden )
      return;

   Point3F axis = -vel;

   if( axis.isZero() )
      axis.set( 0.0, 0.0, 1.0 );
   else
      axis.normalize();

   bool fromWater = pointInWater(from);
   bool toWater   = pointInWater(to);

   if (!fromWater && !toWater && bool(mParticleEmitter))                                        // not in water
      mParticleEmitter->emitParticles(from, to, axis, vel, ms);
   else if (fromWater && toWater && bool(mParticleWaterEmitter))                                // in water
      mParticleWaterEmitter->emitParticles(from, to, axis, vel, ms);
   else if (!fromWater && toWater && bool(mParticleEmitter) && bool(mParticleWaterEmitter))     // entering water
   {
      // cast the ray to get the surface point of the water
      RayInfo rInfo;
      if (gClientContainer.castRay(from, to, WaterObjectType, &rInfo))
      {
         MatrixF trans = getTransform();
         trans.setPosition(rInfo.point);

         Splash *splash = new Splash();
         splash->onNewDataBlock(mDataBlock->splash);
         splash->setTransform(trans);
         splash->setInitialState(trans.getPosition(), Point3F(0.0, 0.0, 1.0));
         if (!splash->registerObject())
         {
            delete splash;
            splash = NULL;
         }

         // create an emitter for the particles out of water and the particles in water
         mParticleEmitter->emitParticles(from, rInfo.point, axis, vel, ms);
         mParticleWaterEmitter->emitParticles(rInfo.point, to, axis, vel, ms);
      }
   }
   else if (fromWater && !toWater && bool(mParticleEmitter) && bool(mParticleWaterEmitter))     // leaving water
   {
      // cast the ray in the opposite direction since that point is out of the water, otherwise
      //  we hit water immediately and wont get the appropriate surface point
      RayInfo rInfo;
      if (gClientContainer.castRay(to, from, WaterObjectType, &rInfo))
      {
         MatrixF trans = getTransform();
         trans.setPosition(rInfo.point);

         Splash *splash = new Splash();
         splash->onNewDataBlock(mDataBlock->splash);
         splash->setTransform(trans);
         splash->setInitialState(trans.getPosition(), Point3F(0.0, 0.0, 1.0));
         if (!splash->registerObject())
         {
            delete splash;
            splash = NULL;
         }

         // create an emitter for the particles out of water and the particles in water
         mParticleEmitter->emitParticles(rInfo.point, to, axis, vel, ms);
         mParticleWaterEmitter->emitParticles(from, rInfo.point, axis, vel, ms);
      }
   }
}


//----------------------------------------------------------------------------

class ObjectDeleteEvent : public SimEvent
{
public:
   void process(SimObject *object)
   {
      object->deleteObject();
   }
};

void Projectile::explode(const Point3F& p, const Point3F& n, const U32 collideType)
{
   // Make sure we don't explode twice...
   if (mHidden == true)
      return;

   mHidden = true;

   if (isServerObject()) {
      // Do what the server needs to do, damage the surrounding objects, etc.
      mExplosionPosition = p + (n*0.01);
      mExplosionNormal = n;
      mCollideHitType  = collideType;

      char buffer[128];
      dSprintf(buffer, sizeof(buffer),  "%g %g %g", mExplosionPosition.x,
                                                    mExplosionPosition.y,
                                                    mExplosionPosition.z);
      Con::executef(mDataBlock, 4, "onExplode", scriptThis(), buffer, Con::getFloatArg(mFadeValue));

      setMaskBits(ExplosionMask);
		Sim::postEvent(this, new ObjectDeleteEvent, Sim::getCurrentTime() + DeleteWaitTime);
   } else {
      // Client just plays the explosion at the right place...
      //
      Explosion* pExplosion = NULL;

      if (mDataBlock->waterExplosion && pointInWater(p))
      {
         pExplosion = new Explosion;
         pExplosion->onNewDataBlock(mDataBlock->waterExplosion);
      }
      else
      if (mDataBlock->explosion)
      {
         pExplosion = new Explosion;
         pExplosion->onNewDataBlock(mDataBlock->explosion);
      }

      if( pExplosion )
      {
         MatrixF xform(true);
         xform.setPosition(p);
         pExplosion->setTransform(xform);
         pExplosion->setInitialState(p, n);
         pExplosion->setCollideType( collideType );
         if (pExplosion->registerObject() == false)
         {
            Con::errorf(ConsoleLogEntry::General, "Projectile(%s)::explode: couldn't register explosion",
                        mDataBlock->getName() );
            delete pExplosion;
            pExplosion = NULL;
         }

         if(mDataBlock->decalCount > 0)
         {
            if(collideType & (TerrainObjectType | InteriorObjectType))
            {
               // randomly choose a decal between 0 and (decal count - 1)
               U32 idx = (U32)(mCeil(mDataBlock->decalCount * Platform::getRandom()) - 1.0f);

               // this should never choose a NULL idx, but check anyway
               if(mDataBlock->decals[idx] != NULL)
               {
                  DecalManager *decalMngr = gClientSceneGraph->getCurrentDecalManager();
                  if(decalMngr)
                     decalMngr->addDecal(p, n, mDataBlock->decals[idx]);
               }
            }
         }
      }

      // Client object
      updateSound();
   }
   }

void Projectile::updateSound()
{
   if (!mDataBlock->sound)
      return;

   if (mHidden && mSoundHandle != NULL_AUDIOHANDLE)
   {
      alxStop(mSoundHandle);
      mSoundHandle = NULL_AUDIOHANDLE;
   }
   else if(!mHidden)
   {
      if (mSoundHandle == NULL_AUDIOHANDLE)
         mSoundHandle = alxPlay(mDataBlock->sound, &getRenderTransform(), &getPosition());
      alxSourceMatrixF(mSoundHandle, &getRenderTransform());
   }
}

Point3F Projectile::getVelocity() const
{
   return mCurrVelocity;
}

void Projectile::processTick(const Move* move)
{
   Parent::processTick(move);

   mCurrTick++;
   if(mSourceObject && mCurrTick > SourceIdTimeoutTicks)
   {
      mSourceObject = 0;
      mSourceObjectId = 0;
   }

   // See if we can get out of here the easy way ...
   if (isServerObject() && mCurrTick >= mDataBlock->lifetime)
   {
      deleteObject();
      return;
   }
   else if (mHidden == true)
      return;

   // ... otherwise, we have to do some simulation work.
   F32 timeLeft;
   RayInfo rInfo;
   Point3F oldPosition;
   Point3F newPosition;

   oldPosition = mCurrPosition;
   if(mDataBlock->isBallistic)
      mCurrVelocity.z -= 9.81 * mDataBlock->gravityMod * (F32(TickMs) / 1000.0f);

   newPosition = oldPosition + mCurrVelocity * (F32(TickMs) / 1000.0f);

   // disable the source objects collision reponse while we determine
   // if the projectile is capable of moving from the old position
   // to the new position, otherwise we'll hit ourself
   if (bool(mSourceObject))
      mSourceObject->disableCollision();

   // Determine if the projectile is going to hit any object between the previous
   // position and the new position. This code is executed both on the server
   // and on the client (for prediction purposes). It is possible that the server
   // will have registered a collision while the client prediction has not. If this
   // happens the client will be corrected in the next packet update.
   if (getContainer()->castRay(oldPosition, newPosition, csmDynamicCollisionMask | csmStaticCollisionMask, &rInfo) == true)
   {
      // make sure the client knows to bounce
      if(isServerObject() && (rInfo.object->getType() & csmStaticCollisionMask) == 0)
         setMaskBits(BounceMask);

      // Next order of business: do we explode on this hit?
      if(mCurrTick > mDataBlock->armingDelay)
      {
         MatrixF xform(true);
         xform.setColumn(3, rInfo.point);
         setTransform(xform);
         mCurrPosition    = rInfo.point;
         mCurrVelocity    = Point3F(0, 0, 0);

         // Get the object type before the onCollision call, in case
         // the object is destroyed.
         U32 objectType = rInfo.object->getType();

         // re-enable the collision response on the source object since
         // we need to process the onCollision and explode calls
         if(mSourceObject)
            mSourceObject->enableCollision();

         // Ok, here is how this works:
         // onCollision is called to notify the server scripts that a collision has occured, then
         // a call to explode is made to start the explosion process. The call to explode is made
         // twice, once on the server and once on the client.
         // The server process is responsible for two things:
         //    1) setting the ExplosionMask network bit to guarantee that the client calls explode
         //    2) initiate the explosion process on the server scripts
         // The client process is responsible for only one thing:
         //    1) drawing the appropriate explosion

         // It is possible that during the processTick the server may have decided that a hit
         // has occured while the client prediction has decided that a hit has not occured.
         // In this particular scenario the client will have failed to call onCollision and
         // explode during the processTick. However, the explode function will be called
         // during the next packet update, due to the ExplosionMask network bit being set.
         // onCollision will remain uncalled on the client however, therefore no client
         // specific code should be placed inside the function!
         onCollision(rInfo.point, rInfo.normal, rInfo.object);
         explode(rInfo.point, rInfo.normal, objectType );

         // break out of the collision check, since we've exploded
         // we dont want to mess with the position and velocity
      }
      else
      {
         if(mDataBlock->isBallistic)
         {
            // Otherwise, this represents a bounce.  First, reflect our velocity
            //  around the normal...
            Point3F bounceVel = mCurrVelocity - rInfo.normal * (mDot( mCurrVelocity, rInfo.normal ) * 2.0);;
            mCurrVelocity = bounceVel;

            // Add in surface friction...
            Point3F tangent = bounceVel - rInfo.normal * mDot(bounceVel, rInfo.normal);
            mCurrVelocity  -= tangent * mDataBlock->bounceFriction;

            // Now, take elasticity into account for modulating the speed of the grenade
            mCurrVelocity *= mDataBlock->bounceElasticity;

            U32 timeLeft = 1.0 * (1.0 - rInfo.t);
            oldPosition = rInfo.point + rInfo.normal * 0.05;
            newPosition = oldPosition + (mCurrVelocity * ((timeLeft/1000.0) * TickMs));
         }
      }
   }

   // re-enable the collision response on the source object now
   // that we are done processing the ballistic movment
      if (bool(mSourceObject))
         mSourceObject->enableCollision();

   if(isClientObject())
   {
      updateSound();
   }

   mCurrDeltaBase = newPosition;
   mCurrBackDelta = mCurrPosition - newPosition;
   mCurrPosition = newPosition;

   MatrixF xform(true);
   xform.setColumn(3, mCurrPosition);
   setTransform(xform);
}


void Projectile::advanceTime(F32 dt)
{
   Parent::advanceTime(dt);

   if (mHidden == true || dt == 0.0)
      return;

   if (mActivateThread &&
         mProjectileShape->getDuration(mActivateThread) > mProjectileShape->getTime(mActivateThread) + dt)
   {
      mProjectileShape->advanceTime(dt, mActivateThread);
   }
   else
   {
      if (mMaintainThread)
      {
         mProjectileShape->advanceTime(dt, mMaintainThread);
      }
      else if (mActivateThread && mDataBlock->maintainSeq != -1)
      {
         mMaintainThread = mProjectileShape->addThread();
         mProjectileShape->setTimeScale(mMaintainThread, 1);
         mProjectileShape->setSequence(mMaintainThread, mDataBlock->maintainSeq, 0);
         mProjectileShape->advanceTime(dt, mMaintainThread);
      }
   }
   
   Point3F renderPos;
   mRenderObjToWorld.getColumn(3, &renderPos);
   if (mLastRenderPos.isZero())
      mLastRenderPos = renderPos;
   emitParticles(mLastRenderPos, renderPos, mCurrVelocity, U32(dt * 1000));
   mLastRenderPos = renderPos;
}

void Projectile::interpolateTick(F32 delta)
{
   Parent::interpolateTick(delta);

   if(mHidden == true)
      return;

   Point3F interpPos = mCurrDeltaBase + mCurrBackDelta * delta;
   Point3F dir = mCurrVelocity;
   if(dir.isZero())
      dir.set(0,0,1);
   else
      dir.normalize();

   MatrixF xform(true);
	xform = MathUtils::createOrientFromDir(dir);
   xform.setPosition(interpPos);
   setRenderTransform(xform);

   // fade out the projectile image
   S32 time = (S32)(mCurrTick - delta);
   if(time > mDataBlock->fadeDelay)
   {
      F32 fade = F32(time - mDataBlock->fadeDelay);
      mFadeValue = 1.0 - (fade / F32(mDataBlock->lifetime));
   }
   else
      mFadeValue = 1.0;

   updateSound();
}


//--------------------------------------------------------------------------
void Projectile::onCollision(const Point3F& hitPosition, const Point3F& hitNormal, SceneObject* hitObject)
{
   // No client specific code should be placed or branched from this function
   if(isClientObject())
      return;

   if (hitObject != NULL && isServerObject())
   {
      char *posArg = Con::getArgBuffer(64);
      char *normalArg = Con::getArgBuffer(64);

      dSprintf(posArg, 64, "%g %g %g", hitPosition.x, hitPosition.y, hitPosition.z);
      dSprintf(normalArg, 64, "%g %g %g", hitNormal.x, hitNormal.y, hitNormal.z);

      Con::executef(mDataBlock, 6, "onCollision",
         scriptThis(),
         Con::getIntArg(hitObject->getId()),
         Con::getFloatArg(mFadeValue),
         posArg,
         normalArg);
   }
}

//--------------------------------------------------------------------------
U32 Projectile::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
   U32 retMask = Parent::packUpdate(con, mask, stream);

   // Initial update
   if (stream->writeFlag(mask & GameBase::InitialUpdateMask))
   {
      Point3F pos;
      getTransform().getColumn(3, &pos);
      stream->writeCompressedPoint(pos);
      F32 len = mCurrVelocity.len();
      if(stream->writeFlag(len > 0.02))
   {
         Point3F outVel = mCurrVelocity;
         outVel *= 1 / len;
         stream->writeNormalVector(outVel, 10);
         len *= 32.0; // 5 bits for fraction
         if(len > 8191)
            len = 8191;
         stream->writeInt((S32)len, 13);
      }

      stream->writeRangedU32(mCurrTick, 0, MaxLivingTicks);
      if (bool(mSourceObject))
      {
         // Potentially have to write this to the client, let's make sure it has a
         //  ghost on the other side...
         S32 ghostIndex = con->getGhostIndex(mSourceObject);
         if (stream->writeFlag(ghostIndex != -1))
         {
            stream->writeRangedU32(U32(ghostIndex), 0, NetConnection::MaxGhostCount);
            stream->writeRangedU32(U32(mSourceObjectSlot),
                                   0, ShapeBase::MaxMountedImages - 1);
         }
         else // havn't recieved the ghost for the source object yet, try again later
            retMask |= GameBase::InitialUpdateMask;
      }
      else
         stream->writeFlag(false);
   }

   // explosion update
   if (stream->writeFlag((mask & ExplosionMask) && mHidden))
   {
      mathWrite(*stream, mExplosionPosition);
      mathWrite(*stream, mExplosionNormal);
      stream->write(mCollideHitType);
   }

   // bounce update
   if (stream->writeFlag(mask & BounceMask))
   {
      // Bounce against dynamic object
      mathWrite(*stream, mCurrPosition);
      mathWrite(*stream, mCurrVelocity);
   }

   return retMask;
}

void Projectile::unpackUpdate(NetConnection* con, BitStream* stream)
{
   Parent::unpackUpdate(con, stream);

   // initial update
   if (stream->readFlag())
   {
      Point3F pos;
      stream->readCompressedPoint(&pos);
      if(stream->readFlag())
      {
         stream->readNormalVector(&mCurrVelocity, 10);
         mCurrVelocity *= stream->readInt(13) / 32.0f;
   }
      else
         mCurrVelocity.set(0, 0, 0);

      mCurrDeltaBase = pos;
      mCurrBackDelta = mCurrPosition - pos;
      mCurrPosition  = pos;
      setPosition(mCurrPosition);

      mCurrTick = stream->readRangedU32(0, MaxLivingTicks);
      if (stream->readFlag())
      {
         mSourceObjectId   = stream->readRangedU32(0, NetConnection::MaxGhostCount);
         mSourceObjectSlot = stream->readRangedU32(0, ShapeBase::MaxMountedImages - 1);

         NetObject* pObject = con->resolveGhost(mSourceObjectId);
         if (pObject != NULL)
            mSourceObject = dynamic_cast<ShapeBase*>(pObject);
      }
      else
      {
         mSourceObjectId   = -1;
         mSourceObjectSlot = -1;
         mSourceObject     = NULL;
      }
   }

   // explosion update
   if (stream->readFlag())
   {
      Point3F explodePoint;
      Point3F explodeNormal;
      mathRead(*stream, &explodePoint);
      mathRead(*stream, &explodeNormal);
      stream->read(&mCollideHitType);

      // start the explosion visuals
      explode(explodePoint, explodeNormal, mCollideHitType);
   }

   // bounce update
   if (stream->readFlag())
   {
      mathRead(*stream, &mCurrPosition);
      mathRead(*stream, &mCurrVelocity);
   }
}

//--------------------------------------------------------------------------
void Projectile::prepModelView(SceneState* state)
{
   Point3F targetVector;
   if( mDataBlock->faceViewer )
   {
      targetVector = state->getCameraPosition() - getRenderPosition();
      targetVector.normalize();

      MatrixF explOrient = MathUtils::createOrientFromDir( targetVector );
      explOrient.setPosition( getRenderPosition() );
      dglMultMatrix( &explOrient );
   }
   else
   {
      dglMultMatrix( &getRenderTransform() );
   }
}

//--------------------------------------------------------------------------
bool Projectile::prepRenderImage(SceneState* state, const U32 stateKey,
                                       const U32 /*startZone*/, const bool /*modifyBaseState*/)
{
   if (isLastState(state, stateKey))
      return false;
   setLastState(state, stateKey);

   if (mHidden == true || mFadeValue <= (1.0/255.0))
      return false;

   // This should be sufficient for most objects that don't manage zones, and
   //  don't need to return a specialized RenderImage...
   if (state->isObjectRendered(this)) {
      SceneRenderImage* image = new SceneRenderImage;
      image->obj = this;
      image->isTranslucent = true;
      image->sortType = SceneRenderImage::Point;
      state->setImageRefPoint(this, image);

      // For projectiles, the datablock pointer is a good enough sort key, since they aren't
      //  skinned at all...
      image->textureSortKey = (U32)(dsize_t)mDataBlock;

      state->insertRenderImage(image);
   }

   return false;
}


static ColorF cubeColors[8] = {
   ColorF(0, 0, 0), ColorF(1, 0, 0), ColorF(0, 1, 0), ColorF(0, 0, 1),
   ColorF(1, 1, 0), ColorF(1, 0, 1), ColorF(0, 1, 1), ColorF(1, 1, 1)
};

static Point3F cubePoints[8] = {
   Point3F(-1, -1, -1), Point3F(-1, -1,  1), Point3F(-1,  1, -1), Point3F(-1,  1,  1),
   Point3F( 1, -1, -1), Point3F( 1, -1,  1), Point3F( 1,  1, -1), Point3F( 1,  1,  1)
};

static U32 cubeFaces[6][4] = {
   { 0, 2, 6, 4 }, { 0, 2, 3, 1 }, { 0, 1, 5, 4 },
   { 3, 2, 6, 7 }, { 7, 6, 4, 5 }, { 3, 7, 5, 1 }
};

static void wireCube(const Point3F& size, const Point3F& pos)
{
   glDisable(GL_CULL_FACE);

   for(int i = 0; i < 6; i++) {
      glBegin(GL_LINE_LOOP);
      for(int vert = 0; vert < 4; vert++) {
         int idx = cubeFaces[i][vert];
         glVertex3f(cubePoints[idx].x * size.x + pos.x, cubePoints[idx].y * size.y + pos.y, cubePoints[idx].z * size.z + pos.z);
      }
      glEnd();
   }
}

void Projectile::renderObject(SceneState* state, SceneRenderImage *)
{
   AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");

   RectI viewport;
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   dglGetViewport(&viewport);

   // Uncomment this if this is a "simple" (non-zone managing) object
   state->setupObjectProjection(this);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();

   prepModelView( state );

   glScalef( mDataBlock->scale.x, mDataBlock->scale.y, mDataBlock->scale.z );

   if(mProjectileShape)
   {
      AssertFatal(mProjectileShape != NULL,
                  "Projectile::renderObject: Error, projectile shape should always be present in renderObject");
      mProjectileShape->selectCurrentDetail();
      mProjectileShape->animate();

      Point3F cameraOffset;
      mObjToWorld.getColumn(3,&cameraOffset);
      cameraOffset -= state->getCameraPosition();
      F32 fogAmount = state->getHazeAndFog(cameraOffset.len(),cameraOffset.z);

      if (mFadeValue == 1.0)
      {
         mProjectileShape->setupFog(fogAmount, state->getFogColor());
      }
      else
      {
         mProjectileShape->setupFog(0.0, state->getFogColor());
         mProjectileShape->setAlphaAlways(mFadeValue * (1.0 - fogAmount));
      }
      mProjectileShape->render();
   }

   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   dglSetViewport(viewport);
   // Debugging Bounding Box

   if (!mProjectileShape || gShowBoundingBox)
   {
      glDisable(GL_DEPTH_TEST);
      Point3F box;
      glPushMatrix();
      dglMultMatrix(&getRenderTransform());
      box = (mObjBox.min + mObjBox.max) * 0.5;
      glTranslatef(box.x,box.y,box.z);
      box = (mObjBox.max - mObjBox.min) * 0.5;
      glScalef(box.x,box.y,box.z);
      glColor3f(1, 0, 1);
      wireCube(Point3F(1,1,1),Point3F(0,0,0));
      glPopMatrix();

      glPushMatrix();
      box = (mWorldBox.min + mWorldBox.max) * 0.5;
      glTranslatef(box.x,box.y,box.z);
      box = (mWorldBox.max - mWorldBox.min) * 0.5;
      glScalef(box.x,box.y,box.z);
      glColor3f(0, 1, 1);
      wireCube(Point3F(1,1,1),Point3F(0,0,0));
      glPopMatrix();
      glEnable(GL_DEPTH_TEST);
   }

   dglSetCanonicalState();
   AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
}

bool Projectile::pointInWater(const Point3F &point)
{
   SimpleQueryList sql;
   if (isServerObject())
      gServerSceneGraph->getWaterObjectList(sql);
   else
      gClientSceneGraph->getWaterObjectList(sql);

   for (U32 i = 0; i < sql.mList.size(); i++)
   {
      WaterBlock* pBlock = dynamic_cast<WaterBlock*>(sql.mList[i]);
      if (pBlock && pBlock->isPointSubmergedSimple( point ))
         return true;
   }
   return false;
}
