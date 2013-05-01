//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "game/fx/precipitation.h"

#include "dgl/dgl.h"
#include "math/mathIO.h"
#include "console/consoleTypes.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sceneState.h"
#include "terrain/sky.h"
#include "game/gameConnection.h"
#include "game/player.h"
#include "core/bitStream.h"
#include "platform/profiler.h"

static const U32 dropHitMask = TerrainObjectType |
                               InteriorObjectType |
                               WaterObjectType |
                               StaticShapeObjectType |
                               StaticTSObjectType;

const U32   Precipitation::csmDropsPerSide = 4;                  ///< How many drops are on a side of the material texture
const F32   Precipitation::csmDropsPerSideF = csmDropsPerSide;   ///< How many drops are on a side of the material texture
const U32   Precipitation::csmFramesPerSide = 2;                 ///< How many frames are on a side of a splash animation
const F32   Precipitation::csmFramesPerSideF = csmFramesPerSide; ///< How many frames are on a side of a splash animation

IMPLEMENT_CO_NETOBJECT_V1(Precipitation);
IMPLEMENT_CO_DATABLOCK_V1(PrecipitationData);

//----------------------------------------------------------
// PrecipitationData
//----------------------------------------------------------
PrecipitationData::PrecipitationData()
{
   soundProfile      = NULL;
   soundProfileId    = 0;

   mDropName = StringTable->insert("");
   mSplashName = StringTable->insert("");

   mDropSize          = 0.5f;
   mSplashSize        = 0.5f;
   mUseTrueBillboards = true;
   mSplashMS          = 250;
}
IMPLEMENT_CONSOLETYPE(PrecipitationData)
IMPLEMENT_GETDATATYPE(PrecipitationData)
IMPLEMENT_SETDATATYPE(PrecipitationData)

void PrecipitationData::initPersistFields()
{
   Parent::initPersistFields();

   addField("soundProfile",      TypeAudioProfilePtr, Offset(soundProfile,       PrecipitationData));
   addField("dropTexture",       TypeFilename,        Offset(mDropName,          PrecipitationData));
   addField("splashTexture",     TypeFilename,        Offset(mSplashName,        PrecipitationData));
   addField("dropSize",          TypeF32,             Offset(mDropSize,          PrecipitationData));
   addField("splashSize",        TypeF32,             Offset(mSplashSize,        PrecipitationData));
   addField("splashMS",          TypeS32,             Offset(mSplashMS,          PrecipitationData));
   addField("useTrueBillboards", TypeBool,            Offset(mUseTrueBillboards, PrecipitationData));
}

bool PrecipitationData::onAdd()
{
   if (Parent::onAdd() == false)
      return false;

   if (!soundProfile && soundProfileId != 0)
      if (Sim::findObject(soundProfileId, soundProfile) == false)
         Con::errorf(ConsoleLogEntry::General, "Error, unable to load sound profile for precipitation datablock");

   return true;
}

void PrecipitationData::packData(BitStream* stream)
{
   Parent::packData(stream);

   if (stream->writeFlag(soundProfile != NULL))
      stream->writeRangedU32(soundProfile->getId(), DataBlockObjectIdFirst,
                             DataBlockObjectIdLast);

   stream->writeString(mDropName);
   stream->writeString(mSplashName);
   stream->write(mDropSize);
   stream->write(mSplashSize);
   stream->write(mSplashMS);
   stream->writeFlag(mUseTrueBillboards);
}

void PrecipitationData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   if (stream->readFlag())
      soundProfileId = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   else
      soundProfileId = 0;

   mDropName = stream->readSTString();
   mSplashName = stream->readSTString();

   stream->read(&mDropSize);
   stream->read(&mSplashSize);
   stream->read(&mSplashMS);
   mUseTrueBillboards = stream->readFlag();
}

//----------------------------------------------------------
// Precipitation!
//----------------------------------------------------------
Precipitation::Precipitation()
{
   mTypeMask |= ProjectileObjectType;
   mNetFlags.set(Ghostable|ScopeAlways);

   mDropHead = NULL;
   mSplashHead = NULL;
   mNumDrops = 5000;
   mPercentage = 1.0f;

   mMinSpeed = 1.5f;
   mMaxSpeed = 2.0f;

   mBoxWidth = 200.0f;
   mBoxHeight = 100.0f;

   mMinMass = 0.75f;
   mMaxMass = 0.85f;

   mMaxTurbulence = 0.1f;
   mTurbulenceSpeed = 0.2f;
   mUseTurbulence = false;

   mRotateWithCamVel = true;

   mDoCollision = true;

   mStormData.valid = false;
   mStormData.startPct = 0.0f;
   mStormData.endPct = 0.0f;
   mStormData.startTime = 0;
   mStormData.totalTime = 0;

   mAudioHandle = 0;
   mDropHandle = TextureHandle();
   mSplashHandle = TextureHandle();

   // Allocate our buffers
   texCoords = new Point2F[4 * csmDropsPerSide*csmDropsPerSide];      ///< texture coords for rain texture
   splashCoords = new Point2F[4 * csmFramesPerSide*csmFramesPerSide]; ///< texture coordinates for splash texture

   U32 count = 0;
   for (U32 v = 0; v < csmDropsPerSide; v++)
   {
      const F32 y1 = (F32) v / csmDropsPerSideF;
      const F32 y2 = (F32)(v+1) / csmDropsPerSideF;

      for (U32 u = 0; u < csmDropsPerSide; u++)
      {
         const F32 x1 = (F32) u / csmDropsPerSideF;
         const F32 x2 = (F32)(u+1) / csmDropsPerSideF;

         texCoords[4*count+0].x = x1;
         texCoords[4*count+0].y = y1;

         texCoords[4*count+1].x = x2;
         texCoords[4*count+1].y = y1;

         texCoords[4*count+2].x = x2;
         texCoords[4*count+2].y = y2;

         texCoords[4*count+3].x = x1;
         texCoords[4*count+3].y = y2;
         count++;
      }
   }

   count = 0;
   for (U32 v = 0; v < csmFramesPerSide; v++)
   {
      const F32 y1 = (F32) v / csmFramesPerSideF;
      const F32 y2 = (F32)(v+1) / csmFramesPerSideF;

      for (U32 u = 0; u < csmFramesPerSide; u++)
      {
         const F32 x1 = (F32) u / csmFramesPerSideF;
         const F32 x2 = (F32)(u+1) / csmFramesPerSideF;

         splashCoords[4*count+0].x = x1;
         splashCoords[4*count+0].y = y1;

         splashCoords[4*count+1].x = x2;
         splashCoords[4*count+1].y = y1;

         splashCoords[4*count+2].x = x2;
         splashCoords[4*count+2].y = y2;

         splashCoords[4*count+3].x = x1;
         splashCoords[4*count+3].y = y2;
         count++;
      }
   }
}

Precipitation::~Precipitation()
{
   // Cleanup our buffers
   delete [] texCoords;
   delete [] splashCoords;
}

void Precipitation::inspectPostApply()
{
   if (isClientObject())
      fillDropList();

   setMaskBits(DataMask);
}

//--------------------------------------------------------------------------
// Console stuff...
//--------------------------------------------------------------------------
void Precipitation::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Movement");
   addField("minSpeed", TypeF32, Offset(mMinSpeed, Precipitation));
   addField("maxSpeed", TypeF32, Offset(mMaxSpeed, Precipitation));

   addField("minMass", TypeF32, Offset(mMinMass, Precipitation));
   addField("maxMass", TypeF32, Offset(mMaxMass, Precipitation));
   endGroup("Movement");

   addGroup("Turbulence");
   addField("maxTurbulence", TypeF32, Offset(mMaxTurbulence, Precipitation));
   addField("turbulenceSpeed", TypeF32, Offset(mTurbulenceSpeed, Precipitation));

   addField("rotateWithCamVel", TypeBool, Offset(mRotateWithCamVel, Precipitation));
   addField("useTurbulence", TypeBool, Offset(mUseTurbulence, Precipitation));
   endGroup("Turbulence");

   addField("numDrops", TypeS32, Offset(mNumDrops, Precipitation));
   addField("boxWidth", TypeF32, Offset(mBoxWidth, Precipitation));
   addField("boxHeight", TypeF32, Offset(mBoxHeight, Precipitation));

   addField("doCollision", TypeBool, Offset(mDoCollision, Precipitation));
}

//-----------------------------------
// Console methods...
ConsoleMethod(Precipitation, setPercentage, void, 3, 3, "precipitation.setPercentage(percentage <0.0 to 1.0>)")
{
   object->setPercentage(dAtof(argv[2]));
}

ConsoleMethod(Precipitation, modifyStorm, void, 4, 4, "precipitation.modifyStorm(Percentage <0 to 1>, Time<sec>)")
{
   object->modifyStorm(dAtof(argv[2]), dAtof(argv[3]) * 1000);
}

//--------------------------------------------------------------------------
// Backend
//--------------------------------------------------------------------------
bool Precipitation::onAdd()
{
   if(!Parent::onAdd())
      return false;

   if (isClientObject())
   {
      if (mDataBlock->soundProfile)
         mAudioHandle = alxPlay(mDataBlock->soundProfile, &getTransform() );

      fillDropList();
   }

   mObjBox.min.set(-1e6f, -1e6f, -1e6f);
   mObjBox.max.set( 1e6f,  1e6f,  1e6f);

   if (isClientObject())
   {
      mDropHandle = TextureHandle(((PrecipitationData*)mDataBlock)->mDropName, MeshTexture);
      mSplashHandle = TextureHandle(((PrecipitationData*)mDataBlock)->mSplashName, MeshTexture);
   }

   resetWorldBox();
   addToScene();

   return true;
}

void Precipitation::onRemove()
{
   removeFromScene();
   Parent::onRemove();

   if (mAudioHandle)
      alxStop(mAudioHandle);
   mAudioHandle = 0;

   if (isClientObject())
      killDropList();
}

bool Precipitation::onNewDataBlock(GameBaseData* dptr)
{
   mDataBlock = dynamic_cast<PrecipitationData*>(dptr);
   if (!mDataBlock || !Parent::onNewDataBlock(dptr))
      return false;

   if (isClientObject())
   {
      mDropHandle = TextureHandle(((PrecipitationData*)mDataBlock)->mDropName, MeshTexture);
      mSplashHandle = TextureHandle(((PrecipitationData*)mDataBlock)->mSplashName, MeshTexture);
   }

   scriptOnNewDataBlock();
   return true;
}

U32 Precipitation::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
   Parent::packUpdate(con, mask, stream);

   if (stream->writeFlag(mask & DataMask))
   {
      stream->write(mNumDrops);
      stream->write(mMinSpeed);
      stream->write(mMaxSpeed);
      stream->write(mBoxWidth);
      stream->write(mBoxHeight);
      stream->write(mMinMass);
      stream->write(mMaxMass);
      stream->write(mMaxTurbulence);
      stream->write(mTurbulenceSpeed);
      stream->writeFlag(mUseTurbulence);
      stream->writeFlag(mRotateWithCamVel);
      stream->writeFlag(mDoCollision);
   }

   if (stream->writeFlag(mask & PercentageMask))
   {
      stream->write(mPercentage);
   }

   if (stream->writeFlag(!(mask & ~(DataMask | PercentageMask | StormMask)) && (mask & StormMask)))
   {
      stream->write(mStormData.endPct);
      stream->write(mStormData.totalTime);
   }

   return 0;
}

void Precipitation::unpackUpdate(NetConnection* con, BitStream* stream)
{
   Parent::unpackUpdate(con, stream);

   U32 oldDrops = mNumDrops * mPercentage;
   if (stream->readFlag())
   {
      stream->read(&mNumDrops);
      stream->read(&mMinSpeed);
      stream->read(&mMaxSpeed);
      stream->read(&mBoxWidth);
      stream->read(&mBoxHeight);
      stream->read(&mMinMass);
      stream->read(&mMaxMass);
      stream->read(&mMaxTurbulence);
      stream->read(&mTurbulenceSpeed);
      mUseTurbulence = stream->readFlag();
      mRotateWithCamVel = stream->readFlag();
      mDoCollision = stream->readFlag();
   }

   if (stream->readFlag())
   {
      stream->read(&mPercentage);
   }

   if (stream->readFlag())
   {
      F32 pct;
      U32 time;
      stream->read(&pct);
      stream->read(&time);
      modifyStorm(pct, time);
   }

   U32 newDrops = mNumDrops * mPercentage;
   if (isClientObject() && oldDrops != newDrops)
      fillDropList();
}

//--------------------------------------------------------------------------
// Support functions
//--------------------------------------------------------------------------
VectorF Precipitation::getWindVelocity()
{
   Sky* sky = gClientSceneGraph->getCurrentSky();
   return (sky && sky->mEffectPrecip) ? -sky->getWindVelocity() : VectorF( 0.0f, 0.0f, 0.0f );
}

void Precipitation::fillDropList()
{
   AssertFatal(isClientObject(), "Precipitation is doing stuff on the server - BAD!");

   F32 density = Con::getFloatVariable("$pref::precipitationDensity", 1.0f);
   U32 newDropCount = (U32)(mNumDrops * mPercentage * density);
   U32 dropCount = 0;
   if (mDropHead)
   {
      Raindrop* curr = mDropHead;
      while (curr)
      {
         dropCount++;
         curr = curr->next;
         if (dropCount == newDropCount && curr)
         {
            //delete the remaining drops
            Raindrop* next = curr->next;
            curr->next = NULL;
            while (next)
            {
               Raindrop* last = next;
               next = next->next;
               last->next = NULL;
               destroySplash(last);
               delete last;
            }
            break;
         }
      }
   }

   if (dropCount < newDropCount)
   {
      //move to the end
      Raindrop* curr = mDropHead;
      if (curr)
      {
         while (curr->next)
            curr = curr->next;
      }
      else
      {
         mDropHead = curr = new Raindrop;
         spawnNewDrop(curr);
         dropCount++;
      }

      //and add onto it
      while (dropCount < newDropCount)
      {
         curr->next = new Raindrop;
         curr = curr->next;
         spawnNewDrop(curr);
         dropCount++;
      }
   }
}

void Precipitation::killDropList()
{
   AssertFatal(isClientObject(), "Precipitation is doing stuff on the server - BAD!");

   Raindrop* curr = mDropHead;
   while (curr)
   {
      Raindrop* next = curr->next;
      delete curr;
      curr = next;
   }
   mDropHead = NULL;
}

void Precipitation::spawnDrop(Raindrop *drop)
{
   PROFILE_START(PrecipSpawnDrop);
   AssertFatal(isClientObject(), "Precipitation is doing stuff on the server - BAD!");

   drop->velocity = Platform::getRandom() * (mMaxSpeed - mMinSpeed) + mMinSpeed;

   drop->position.x = Platform::getRandom() * mBoxWidth - (mBoxWidth / 2.0f);
   drop->position.y = Platform::getRandom() * mBoxWidth - (mBoxWidth / 2.0f);

   drop->texCoordIndex = (U32)(Platform::getRandom() * (csmDropsPerSideF*csmDropsPerSideF - 0.5f));
   drop->valid = true;
   drop->time = Platform::getRandom() * M_2PI_F;
   drop->mass = Platform::getRandom() * (mMaxMass - mMinMass) + mMinMass;
   PROFILE_END();
}

void Precipitation::spawnNewDrop(Raindrop *drop)
{
   AssertFatal(isClientObject(), "Precipitation is doing stuff on the server - BAD!");

   spawnDrop(drop);
   drop->position.z = Platform::getRandom() * mBoxHeight - (mBoxHeight / 2.0f);
}

void Precipitation::findDropCutoff(Raindrop *drop)
{
   PROFILE_START(PrecipFindDropCutoff);
   AssertFatal(isClientObject(), "Precipitation is doing stuff on the server - BAD!");

   if (mDoCollision)
   {
      VectorF velocity = getWindVelocity() / drop->mass - VectorF(0.0f, 0.0f, drop->velocity);
      velocity.normalize();

      Point3F end   = drop->position + 100.0f * velocity;
      Point3F start = drop->position - 500.0f * velocity;

      RayInfo rInfo;
      if (getContainer()->castRay(start, end, dropHitMask, &rInfo))
      {
         drop->hitPos = rInfo.point;
         drop->hitType = rInfo.object->getTypeMask();
      }
      else
         drop->hitPos.set(0.0f,0.0f,-1000.0f);

      drop->valid = drop->position.z > drop->hitPos.z;
   }
   else
   {
      drop->hitPos.set(0.0f,0.0f,-1000.0f);
      drop->valid = true;
   }
   PROFILE_END();
}

inline void Precipitation::wrapDrop(Raindrop *drop, Box3F &box)
{
   bool recalcCutoff = false;

   const Point3F	&boxMin = box.min;
   const Point3F	&boxMax = box.max;
   Point3F			&position = drop->position;
   
   if (position.x < boxMin.x)
   {		
      position.x = boxMin.x + (mBoxWidth - mFmod( (boxMin.x - position.x), mBoxWidth ) );
	  
      recalcCutoff = true;
   }
   else if (position.x > boxMax.x)
   {
      position.x = boxMax.x - (mBoxWidth - mFmod( (position.x - boxMin.x), mBoxWidth ) );
	  
      recalcCutoff = true;
   }

   if (position.y < boxMin.y)
   {
      position.y = boxMin.y + (mBoxWidth - mFmod( (boxMin.y - position.y), mBoxWidth ) );
	  
      recalcCutoff = true;
   }
   else if (position.y > boxMax.y)
   {
      position.y = boxMax.y - (mBoxWidth - mFmod( (position.y - boxMin.y), mBoxWidth ) );
	  
      recalcCutoff = true;
   }

   if (position.z < boxMin.z)
   {
      spawnDrop(drop);
	  
      position.x += boxMin.x;
      position.y += boxMin.y;
	  
      position.z = boxMin.z + (mBoxHeight - mFmod( (boxMin.z - position.z), mBoxHeight ) );

      recalcCutoff = true;
   }
   else if (position.z > boxMax.z)
   {
      position.z = boxMax.z - (mBoxHeight - mFmod( (position.z - boxMin.z), mBoxHeight ) );
	  
      recalcCutoff = true;
   }
   
   if (recalcCutoff)
      findDropCutoff(drop);
}

void Precipitation::createSplash(Raindrop *drop)
{
   PROFILE_START(PrecipCreateSplash);
   if (drop != mSplashHead && !(drop->nextSplashDrop || drop->prevSplashDrop))
   {
      if (!mSplashHead)
      {
         mSplashHead = drop;
         drop->prevSplashDrop = NULL;
         drop->nextSplashDrop = NULL;
      }
      else
      {
         mSplashHead->prevSplashDrop = drop;
         drop->nextSplashDrop = mSplashHead;
         drop->prevSplashDrop = NULL;
         mSplashHead = drop;
      }
   }

   drop->animStartTime = Platform::getVirtualMilliseconds();
   PROFILE_END();
}

void Precipitation::destroySplash(Raindrop *drop)
{
   PROFILE_START(PrecipDestroySplash);

   if (drop->nextSplashDrop)
      drop->nextSplashDrop->prevSplashDrop = drop->prevSplashDrop;
   if (drop->prevSplashDrop)
      drop->prevSplashDrop->nextSplashDrop = drop->nextSplashDrop;

   drop->nextSplashDrop = NULL;
   drop->prevSplashDrop = NULL;

   if (drop == mSplashHead)
      mSplashHead = NULL;

   PROFILE_END();
}

//--------------------------------------------------------------------------
// Processing
//--------------------------------------------------------------------------
void Precipitation::setPercentage(F32 pct)
{
   mPercentage = pct;
   if (isServerObject())
      setMaskBits(PercentageMask);
}

void Precipitation::modifyStorm(F32 pct, U32 ms)
{
   pct = mClampF(pct, 0.0f, 1.0f);
   mStormData.endPct = pct;
   mStormData.totalTime = ms;

   if (isServerObject())
   {
      setMaskBits(StormMask);
      return;
   }

   mStormData.startTime = Platform::getVirtualMilliseconds();
   mStormData.startPct = mPercentage;
   mStormData.valid = true;
}

void Precipitation::interpolateTick(F32 delta)
{
   PROFILE_START(PrecipInterpolate);
   AssertFatal(isClientObject(), "Precipitation is doing stuff on the server - BAD!");

   const F32		dt = 1.0f-delta;
   const VectorF	startTurbulence = dt * getWindVelocity();
   const F32		dtTurbulenceSpeed = dt * mTurbulenceSpeed;
   
   register Raindrop* curr = mDropHead;

   while (curr)
   {
      if (!curr->toRender)
      {
         curr = curr->next;
         continue;
      }

      if (mUseTurbulence)
      {
         F32 renderTime = curr->time + dtTurbulenceSpeed;
         
		 VectorF	turbulence = startTurbulence + VectorF(mSin(renderTime), mCos(renderTime), 0.0f) * mMaxTurbulence;
         
		 curr->renderPosition = curr->position + turbulence / curr->mass;
      }
	  else
      {
		curr->renderPosition = curr->position + startTurbulence / curr->mass;
	  }

      curr->renderPosition.z -= dt * curr->velocity;

      curr = curr->next;
   }
   PROFILE_END();
}

void Precipitation::processTick(const Move *)
{
   //nothing to do on the server
   if (isServerObject())
      return;

   //we need to update positions and do some collision here
   GameConnection* conn = GameConnection::getConnectionToServer();
   if (!conn)
      return; //need connection to server

   ShapeBase* camObj = conn->getCameraObject();
   if (!camObj)
      return;

   PROFILE_START(PrecipProcess);

   //update the storm if necessary
   if (mStormData.valid)
   {
      F32 t = (Platform::getVirtualMilliseconds() - mStormData.startTime) / (F32)mStormData.totalTime;
      if (t >= 1.0f)
      {
         mPercentage = mStormData.endPct;
         mStormData.valid = false;
      }
      else
         mPercentage = mStormData.startPct * (1.0f-t) + mStormData.endPct * t;
      fillDropList();
   }

   MatrixF camMat;
   camObj->getEyeTransform(&camMat);
   Point3F camPos, camDir;
   camMat.getColumn(3, &camPos);
   camMat.getColumn(1, &camDir);
   camDir.normalize();

   //make a box
   Box3F box(camPos.x - mBoxWidth * 0.5f, camPos.y - mBoxWidth * 0.5f, camPos.z - mBoxHeight * 0.5f,
             camPos.x + mBoxWidth * 0.5f, camPos.y + mBoxWidth * 0.5f, camPos.z + mBoxHeight * 0.5f);

   //offset the renderbox in the direction of the camera direction
   //in order to have more of the drops actually rendered
   box.min.x += camDir.x * mBoxWidth * 0.25f;
   box.max.x += camDir.x * mBoxWidth * 0.25f;
   box.min.y += camDir.y * mBoxWidth * 0.25f;
   box.max.y += camDir.y * mBoxWidth * 0.25f;
   box.min.z += camDir.z * mBoxHeight * 0.25f;
   box.max.z += camDir.z * mBoxHeight * 0.25f;

   const VectorF windVel = getWindVelocity();
   const F32 fovDot = camObj->getCameraFov() * (1.0f / 180.0f);
   const bool	useFunkyEffectThingy = (mSplashHandle.getGLName() != 0);
   Raindrop* curr = mDropHead;

   while (curr)
   {
      //update position
      if (mUseTurbulence)
         curr->time += mTurbulenceSpeed;
      curr->position += windVel / curr->mass;
      curr->position.z -= curr->velocity;

      //wrap position
      wrapDrop(curr, box);

      if (curr->valid && (curr->position.z < curr->hitPos.z))
      {
		curr->valid = false;

		// Bump back so we don't spawn splashes where they ought not
		// be. It might be better to revisit this and use the hitPos.
		curr->position -= windVel / curr->mass;
		curr->position.z += curr->velocity;

		//do some funky effect thingy for hitting something
		if (useFunkyEffectThingy)
		   createSplash(curr);
      }

      //render test
	  curr->toRender = false;
	  if ( curr->valid )
      {
		const VectorF lookVec = curr->position - camPos;
		curr->toRender = (mDot(lookVec, camDir) > fovDot);
	  }

      curr = curr->next;
   }

   //update splashes
   const U32 currTime = Platform::getVirtualMilliseconds();
   const F32 invSplashMS = (1.0f / mDataBlock->mSplashMS);

   curr = mSplashHead;
   while (curr)
   {
      F32 pct = (F32)(currTime - curr->animStartTime) * invSplashMS;
      if (pct >= 1.0f)
      {
         Raindrop *next = curr->nextSplashDrop;
         destroySplash(curr);
         curr = next;
         continue;
      }

      curr->splashIndex = (U32)((csmFramesPerSideF*csmFramesPerSideF) * pct);
      curr = curr->nextSplashDrop;
   }
   PROFILE_END();
}

//--------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------
bool Precipitation::prepRenderImage(SceneState* state, const U32 stateKey,
                                    const U32 /*startZone*/, const bool /*modifyBaseState*/)
{
   if (isLastState(state, stateKey))
      return false;
   setLastState(state, stateKey);

   // This should be sufficient for most objects that don't manage zones, and
   //  don't need to return a specialized RenderImage...
   if (state->isObjectRendered(this)) {
      SceneRenderImage* image = new SceneRenderImage;
      image->obj = this;
      image->isTranslucent = true;
      image->sortType = SceneRenderImage::EndSort;
      state->insertRenderImage(image);
   }

   if (!mAudioHandle && mDataBlock->soundProfile)
      mAudioHandle = alxPlay(mDataBlock->soundProfile, &getTransform());

   return false;
}

void Precipitation::renderObject(SceneState* state, SceneRenderImage*)
{
   PROFILE_START(PrecipRender);
   AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");

   RectI viewport;
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   dglGetViewport(&viewport);

   // Uncomment this if this is a "simple" (non-zone managing) object
   state->setupObjectProjection(this);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();

   renderPrecip(state);
   renderSplashes(state);

   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   dglSetViewport(viewport);

   AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
   PROFILE_END();
}


//--------------------------------------------------------------------------
struct PrecipVert
{
   Point3F	vert;
   Point2F	texCoord;
};

static Vector<PrecipVert>	renderPrecipVerts(__FILE__, __LINE__);
static Vector<U16>			pvIndices(__FILE__, __LINE__);

void Precipitation::renderPrecip(SceneState *state)
{
   GameConnection* conn = GameConnection::getConnectionToServer();
   if (!conn)
      return; //need connection to server

   ShapeBase* camObj = conn->getCameraObject();
   if (!camObj)
      return;

   PROFILE_START(PrecipRenderPrecip);

   renderPrecipVerts.clear();
   pvIndices.clear();
   U32 vertCount = 0;

   const Point3F camPos = state->getCameraPosition();
   const VectorF camVel = camObj->getVelocity();
   const VectorF windVel = getWindVelocity();

   Raindrop *curr = mDropHead;

   while ( curr )
   {
      if (!curr->toRender)
      {
         curr = curr->next;
         continue;
      }

      const Point3F pos = curr->renderPosition;
      
      VectorF right;
      VectorF up;

      // two forms of billboards - true billboards (1st codeblock)
      // or axis-aligned with velocity (2nd codeblock)
      // the axis-aligned billboards are aligned with the velocity
      // of the raindrop, and tilted slightly towards the camera
      if (mDataBlock->mUseTrueBillboards)
      {
         state->mModelview.getRow(0,&right);
         state->mModelview.getRow(2,&up);
         right.normalize();
         up.normalize();
      }
      else
      {
	     VectorF orthoDir = (camPos - pos);

         VectorF velocity = windVel / curr->mass;
         if (mRotateWithCamVel && camVel != VectorF( 0.0f, 0.0f, 0.0f ))
         {
            const F32 distance = orthoDir.len();
            velocity -= camVel / (distance > 2.0f ? distance : 2.0f) * 0.3f;
         }

         velocity.z -= curr->velocity;
         velocity.normalize();
         orthoDir.normalize();

         right = mCross(-velocity, orthoDir);
         right.normalize();
         up = mCross(orthoDir, right) * 0.5f - velocity * 0.5f;
         up.normalize();
      }
      right *= mDataBlock->mDropSize;
      up *= mDataBlock->mDropSize;
	  
      renderPrecipVerts.increment(4);
      pvIndices.increment(6);
      PrecipVert *verts = &(renderPrecipVerts[vertCount * 4]);
      U16 *ind = &(pvIndices[vertCount * 6]);
  
      const Point2F	*texCoordPoint = &(texCoords[4*curr->texCoordIndex]);
      
      verts[0].vert = pos + right - up;
      verts[0].texCoord = texCoordPoint[3];
      ind[0] = vertCount * 4;
      ind[5] = vertCount * 4;

      verts[1].vert = pos - right - up;
      verts[1].texCoord = texCoordPoint[2];
      ind[1] = vertCount * 4 + 1;

      verts[2].vert = pos - right + up;
      verts[2].texCoord = texCoordPoint[1];
      ind[2] = vertCount * 4 + 2;
      ind[3] = vertCount * 4 + 2;

      verts[3].vert = pos + right + up;
      verts[3].texCoord = texCoordPoint[0];
      ind[4] = vertCount * 4 + 3;

      vertCount++;
	 
      //debug collision render
      //if (curr->cutoffHeight != -1000)
      //{
      //   VectorF windVel = getWindVelocity();
      //   VectorF velocity = windVel / curr->mass - VectorF(0, 0, curr->velocity);
      //   velocity.normalize();

      //   Point3F start = curr->position;// - 10000 * velocity;
      //   F32 height = start.z - curr->cutoffHeight;
      //   F32 t = height / velocity.z;
      //   Point3F end = start - t * velocity;

      //   glDisable(GL_TEXTURE_2D);
      //   glDisable(GL_BLEND);

      //   glBegin(GL_LINES);
      //   glColor3f(1.0, 0.0, 0.0);
      //   glVertex3fv(&(start.x));
      //   glColor3f(0.0, 1.0, 0.0);
      //   glVertex3fv(&(end.x));
      //   glEnd();

      //   glBegin(GL_TRIANGLE_FAN);
      //   glColor3f(0.0,0.0,1.0);
      //   glVertex3fv(&((end - right + up).x));
      //   glVertex3fv(&((end + right + up).x));
      //   glVertex3fv(&((end + right - up).x));
      //   glVertex3fv(&((end - right - up).x));
      //   glEnd();

      //   glEnable(GL_TEXTURE_2D);
      //   glEnable(GL_BLEND);
      //}
      //end debug collision render

      curr = curr->next;
   }
   
   if ( vertCount == 0 )
   {
      PROFILE_END();
	  return;
   }

   glEnable(GL_BLEND);
   glDepthMask(GL_FALSE);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mDropHandle.getGLName());

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glColor3f(1.0f, 1.0f, 1.0f);
   
   glVertexPointer( 3, GL_FLOAT, sizeof( PrecipVert ), &(renderPrecipVerts[0].vert) );
   glTexCoordPointer( 2, GL_FLOAT, sizeof( PrecipVert ), &(renderPrecipVerts[0].texCoord) );
	  
   glDrawElements( GL_TRIANGLES, pvIndices.size(), GL_UNSIGNED_SHORT, pvIndices.address() );

   glDisable(GL_TEXTURE_2D);

   glDepthMask(GL_TRUE);
   glDisable(GL_BLEND);

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   PROFILE_END();
}

void Precipitation::renderSplashes(SceneState *state)
{
   PROFILE_START(PrecipRenderSplash);

   //setup the billboard
   VectorF right, up;
   state->mModelview.getRow(0, &right);
   state->mModelview.getRow(2, &up);
   right.normalize();
   up.normalize();
   right *= mDataBlock->mSplashSize;
   up *= mDataBlock->mSplashSize;

   renderPrecipVerts.clear();
   pvIndices.clear();
   U32 vertCount = 0;

   Raindrop *curr = mSplashHead;
   while (curr)
   {
      renderPrecipVerts.increment(4);
      pvIndices.increment(6);
      PrecipVert *verts = &(renderPrecipVerts[vertCount * 4]);
      U16 *ind = &(pvIndices[vertCount * 6]);
  
      const Point2F	*texCoordPoint = &(splashCoords[4*curr->splashIndex]);
      
      verts[0].vert = curr->hitPos + right - up;
      verts[0].texCoord = texCoordPoint[3];
      ind[0] = vertCount * 4;
      ind[5] = vertCount * 4;

      verts[1].vert = curr->hitPos - right - up;
      verts[1].texCoord = texCoordPoint[2];
      ind[1] = vertCount * 4 + 1;

      verts[2].vert = curr->hitPos - right + up;
      verts[2].texCoord = texCoordPoint[1];
      ind[2] = vertCount * 4 + 2;
      ind[3] = vertCount * 4 + 2;

      verts[3].vert = curr->hitPos + right + up;
      verts[3].texCoord = texCoordPoint[0];
      ind[4] = vertCount * 4 + 3;

      vertCount++;

      curr = curr->nextSplashDrop;
   }
   
   if ( vertCount == 0 )
   {
      PROFILE_END();
	  return;
   }
   
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mSplashHandle.getGLName());
   glEnable(GL_BLEND);
   glDepthMask(GL_FALSE);

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glColor3f(1.0f, 1.0f, 1.0f);
  
   glVertexPointer( 3, GL_FLOAT, sizeof( PrecipVert ), &(renderPrecipVerts[0].vert) );
   glTexCoordPointer( 2, GL_FLOAT, sizeof( PrecipVert ), &(renderPrecipVerts[0].texCoord) );
  
   glDrawElements( GL_TRIANGLES, pvIndices.size(), GL_UNSIGNED_SHORT, pvIndices.address() );

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisable(GL_BLEND);
   glDepthMask(GL_TRUE);
   glDisable(GL_TEXTURE_2D);

   PROFILE_END();
}
