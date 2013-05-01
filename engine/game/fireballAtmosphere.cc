//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "console/consoleTypes.h"
#include "dgl/dgl.h"
#include "audio/audioDataBlock.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sceneState.h"
#include "game/gameConnection.h"
#include "math/mathIO.h"
#include "game/fireballAtmosphere.h"
#include "game/debris.h"
#include "math/mathUtils.h"

#define DML_DIR "textures/"
#define COLOR_OFFSET 0.25


IMPLEMENT_CO_NETOBJECT_V1(FireballAtmosphere);
IMPLEMENT_CO_DATABLOCK_V1(FireballAtmosphereData);


//**************************************************************************
// Fireball Atmosphere Data
//**************************************************************************
FireballAtmosphereData::FireballAtmosphereData()
{

   fireball       = NULL;
   fireballID     = 0;
}

IMPLEMENT_CONSOLETYPE(FireballAtmosphereData)
IMPLEMENT_GETDATATYPE(FireballAtmosphereData)
IMPLEMENT_SETDATATYPE(FireballAtmosphereData)

void FireballAtmosphereData::initPersistFields()
{
   Parent::initPersistFields();
   addField("fireball", TypeDebrisDataPtr, Offset(fireball, FireballAtmosphereData));
}

bool FireballAtmosphereData::onAdd()
{
   if(Parent::onAdd() == false)
      return false;

   if(!fireball && fireballID != 0)
   {
      if(!Sim::findObject(SimObjectId(fireballID), fireball))
         Con::errorf(ConsoleLogEntry::General, "FireballAtmosphereData::preload: Invalid packet, bad datablockId(fireball): 0x%x", fireballID);
   }

   return true;
}

void FireballAtmosphereData::packData(BitStream* stream)
{
   Parent::packData(stream);

   if(stream->writeFlag(fireball))
   {
      stream->writeRangedU32(packed? SimObjectId(fireball):
         fireball->getId(),DataBlockObjectIdFirst,DataBlockObjectIdLast);
   }
  
}

void FireballAtmosphereData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   if(stream->readFlag())
      fireballID = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
}

//**************************************************************************
// Fireball Atmosphere
//**************************************************************************
FireballAtmosphere::FireballAtmosphere()
{
   mTimeSinceLastDrop = 0.0;

   mDropRadius     = 600.0;
   mDropsPerMinute = 3.0;
   mMinDropAngle   = 0.0;
   mMaxDropAngle   = 30.0;
   mStartVelocity  = 20.0;
   mDropHeight     = 500.0;
   mDropDir.set(0.5, 0.5, -0.5);
}

void FireballAtmosphere::initPersistFields()
{
   Parent::initPersistFields();

   addField("dropRadius",     TypeF32,             Offset(mDropRadius,        FireballAtmosphere));
   addField("dropsPerMinute", TypeF32,             Offset(mDropsPerMinute,    FireballAtmosphere));
   addField("minDropAngle",   TypeF32,             Offset(mMinDropAngle,      FireballAtmosphere));
   addField("maxDropAngle",   TypeF32,             Offset(mMaxDropAngle,      FireballAtmosphere));
   addField("startVelocity",  TypeF32,             Offset(mStartVelocity,     FireballAtmosphere));
   addField("dropHeight",     TypeF32,             Offset(mDropHeight,        FireballAtmosphere));
   addField("dropDir",        TypePoint3F,         Offset(mDropDir,           FireballAtmosphere));
}

bool FireballAtmosphere::onAdd()
{
   if(!Parent::onAdd())
      return false;
   
   if(mDataBlock && mDataBlock->fireball)
      mDataBlock->fireball->terminalVelocity = mStartVelocity;

   mDropDir.normalize();

   mObjBox.min.set(-1e6, -1e6, -1e6);
   mObjBox.max.set( 1e6,  1e6,  1e6);

   resetWorldBox();
   addToScene();
  
   return true;
}

void FireballAtmosphere::onRemove()
{
   removeFromScene();
   Parent::onRemove();
}

bool FireballAtmosphere::onNewDataBlock(GameBaseData* dptr)
{
   mDataBlock = dynamic_cast<FireballAtmosphereData*>(dptr);
   if(!mDataBlock || !Parent::onNewDataBlock(dptr))
      return false;
   
   scriptOnNewDataBlock();
   return true;
}


bool FireballAtmosphere::prepRenderImage(SceneState* state, const U32 stateKey, const U32, const bool)
{
   if(isLastState(state, stateKey))
      return false;
   setLastState(state, stateKey);

   return false;
}

void FireballAtmosphere::renderObject(SceneState* , SceneRenderImage*)
{
   AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");
   AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
}

void FireballAtmosphere::advanceTime(F32 dt)
{
   mTimeSinceLastDrop += dt;
   F32 dropFrequency = 60.0 / mDropsPerMinute;

   if(mTimeSinceLastDrop > dropFrequency)
   {
      mTimeSinceLastDrop -= dropFrequency;
      dropNewFireball();
   }
}

void FireballAtmosphere::dropNewFireball()
{
   MatrixF camTrans;
   GameConnection* connection = GameConnection::getConnectionToServer();
   connection->getControlCameraTransform(0.0, &camTrans);
   
   Debris *fireball = new Debris;
   fireball->onNewDataBlock(mDataBlock->fireball);

   // set velocity
   VectorF launchVel = MathUtils::randomDir(mDropDir, mMinDropAngle, mMaxDropAngle);
   launchVel *= mStartVelocity;

   // set start point
   VectorF down(0.0, 0.0, -1.0);
   Point3F launchPoint = MathUtils::randomDir(down, 90.0, 90.0);
   launchPoint *= mDropRadius * gRandGen.randF(0.1, 1.0);
   launchPoint += camTrans.getPosition();
   launchPoint.z = 0.0;

   F32 timeToHit = mDropHeight / launchVel.z;
   launchPoint += launchVel * timeToHit;
   
   
   if(!fireball->registerObject())
      delete fireball;
   else
      fireball->init(launchPoint, launchVel);
}

U32 FireballAtmosphere::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
   U32 retMask = Parent::packUpdate(con, mask, stream);

   if(stream->writeFlag(mask & GameBase::InitialUpdateMask))
   {
      stream->write(mDropRadius);
      stream->write(mDropsPerMinute);
      stream->write(mMaxDropAngle);
      stream->write(mMinDropAngle);
      stream->write(mStartVelocity);
      stream->write(mDropHeight);
      stream->write(mDropDir.x);
      stream->write(mDropDir.y);
      stream->write(mDropDir.z);
   }

   return retMask;
}

void FireballAtmosphere::unpackUpdate(NetConnection* con, BitStream* stream)
{
   Parent::unpackUpdate(con, stream);

   if(stream->readFlag())
   {
      stream->read(&mDropRadius);
      stream->read(&mDropsPerMinute);
      stream->read(&mMaxDropAngle);
      stream->read(&mMinDropAngle);
      stream->read(&mStartVelocity);
      stream->read(&mDropHeight);
      stream->read(&mDropDir.x);
      stream->read(&mDropDir.y);
      stream->read(&mDropDir.z);
  }
}
