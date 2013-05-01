//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "./roadVehicle.h"

#include <platform/platform.h>
#include <dgl/dgl.h>
#include <core/dnet.h>
#include <core/bitStream.h>
#include <game/game.h>
#include <math/mMath.h>
#include <console/simBase.h>
#include <console/console.h>
#include <console/consoleTypes.h>
#include <game/moveManager.h>
#include <ts/tsShapeInstance.h>
#include <core/resManager.h>
#include <math/mathIO.h>
#include <game/shadow.h>
#include <sim/netConnection.h>
#include <lightingSystem/sgLighting.h>

extern bool gEditingMission;

//----------------------------------------------------------------------------
// Vehicle Tire Data Block
//----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(RoadVehicleTire);

RoadVehicleTire::RoadVehicleTire()
{
   shape = 0;
   shapeName = "";
   radius = 0.6;
}

bool RoadVehicleTire::preload(bool server, char errorBuffer[256])
{
   // Load up the tire shape.  ShapeBase has an option to force a
   // CRC check, this is left out here, but could be easily added.
   if (shapeName && shapeName[0]) {

      // Load up the shape resource
      shape = ResourceManager->load(shapeName);
      if (!bool(shape)) {
         dSprintf(errorBuffer, 256, "RoadVehicleTire: Couldn't load shape \"%s\"",shapeName);
         return false;
      }

      // Determin wheel radius from the shape's bounding box.
      // The tire should be built with it's hub axis along the
      // object's Y axis.
      radius = shape->bounds.len_z() / 2;
   }

   return true;
}

void RoadVehicleTire::initPersistFields()
{
   Parent::initPersistFields();

   addField("shapeFile",TypeFilename,Offset(shapeName,RoadVehicleTire));
   addField("radius", TypeF32, Offset(radius, RoadVehicleTire));
}

void RoadVehicleTire::packData(BitStream* stream)
{
   Parent::packData(stream);

   stream->writeString(shapeName);
   stream->write(radius);
}

void RoadVehicleTire::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   shapeName = stream->readSTString();
   stream->read(&radius);
}



//----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(RoadVehicleData);

RoadVehicleData::RoadVehicleData()
{
   shadowEnable = true;
   shadowCanMove = false;
   shadowCanAnimate = true;

   genericShadowLevel = StaticShape_GenericShadowLevel;
   noShadowLevel = StaticShape_NoShadowLevel;

   wheelCount     = 4;
}

void RoadVehicleData::initPersistFields()
{
   Parent::initPersistFields();

   addField("wheelCount",   TypeS8, Offset(wheelCount,   RoadVehicleData));
}

void RoadVehicleData::packData(BitStream* stream)
{
   Parent::packData(stream);
   stream->write(wheelCount);
}

void RoadVehicleData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);
   stream->read(&wheelCount);
}


//----------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(RoadVehicle);

RoadVehicle::RoadVehicle()
{
   overrideOptions = false;

   mTypeMask |= VehicleObjectType;
   mDataBlock = 0;
}

RoadVehicle::~RoadVehicle()
{
}


void RoadVehicle::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Lighting");
   addField("receiveSunLight", TypeBool, Offset(receiveSunLight, RoadVehicle));
   addField("receiveLMLighting", TypeBool, Offset(receiveLMLighting, RoadVehicle));
   addField("useAdaptiveSelfIllumination", TypeBool, Offset(useAdaptiveSelfIllumination, RoadVehicle));
   addField("useCustomAmbientLighting", TypeBool, Offset(useCustomAmbientLighting, RoadVehicle));
   addField("customAmbientSelfIllumination", TypeBool, Offset(customAmbientForSelfIllumination, RoadVehicle));
   addField("customAmbientLighting", TypeColorF, Offset(customAmbientLighting, RoadVehicle));
   addField("lightGroupName", TypeString, Offset(lightGroupName, RoadVehicle));
   addField("useLightingOcclusion", TypeBool, Offset(useLightingOcclusion, RoadVehicle));
   endGroup("Lighting");
}

void RoadVehicle::inspectPostApply()
{
   if(isServerObject()) 
   {
      setMaskBits(0xffffffff);
   }
}


//----------------------------------------------------------------------------

bool RoadVehicle::onAdd()
{
   if(!Parent::onAdd() || !mDataBlock)
      return false;

   // We need to modify our type mask based on what our datablock says...
   mTypeMask |= VehicleObjectType;

   addToScene();

   if (isServerObject())
      scriptOnAdd();
   return true;
}

bool RoadVehicle::onNewDataBlock(GameBaseData* dptr)
{
   mDataBlock = dynamic_cast<RoadVehicleData*>(dptr);
   if (!mDataBlock || !Parent::onNewDataBlock(dptr))
      return false;

   scriptOnNewDataBlock();
   return true;
}

void RoadVehicle::onRemove()
{
   scriptOnRemove();
   removeFromScene();
   Parent::onRemove();
}

void RoadVehicle::renderShadow(F32 dist, F32 fogAmount)
{
	if(!mDataBlock->shadowEnable)
		return;

	bool allowanimate = mDataBlock->shadowCanAnimate &&
		(!(!mDataBlock->shape.isNull() && mDataBlock->shape->sequences.empty()));
   bool allowmove = mDataBlock->shadowCanMove && gEditingMission;

	shadows.sgRender(this, mShapeInstance, dist, fogAmount,
		mDataBlock->genericShadowLevel, mDataBlock->noShadowLevel,
		mDataBlock->shadowNode, allowmove, allowanimate);
}

//----------------------------------------------------------------------------

void RoadVehicle::processTick(const Move* move)
{
   Parent::processTick(move);

   // Image Triggers
   if (move && mDamageState == Enabled) 
   {
      setImageTriggerState(0,move->trigger[0]);
      setImageTriggerState(1,move->trigger[1]);
   }

   if (isMounted()) 
   {
      MatrixF mat;
      mMount.object->getMountTransform(mMount.node,&mat);
      Parent::setTransform(mat);
      Parent::setRenderTransform(mat);
   }
}

void RoadVehicle::interpolateTick(F32)
{
   if (isMounted()) 
   {
      MatrixF mat;
      mMount.object->getRenderMountTransform(mMount.node,&mat);
      Parent::setRenderTransform(mat);
   }
}

void RoadVehicle::setTransform(const MatrixF& mat)
{
   Parent::setTransform(mat);
   setMaskBits(PositionMask);
}

void RoadVehicle::onUnmount(ShapeBase*,S32)
{
   // Make sure the client get's the final server pos.
   setMaskBits(PositionMask);
}


//----------------------------------------------------------------------------

U32 RoadVehicle::packUpdate(NetConnection *connection, U32 mask, BitStream *bstream)
{
   U32 retMask = Parent::packUpdate(connection,mask,bstream);
   if (bstream->writeFlag(mask & PositionMask)) 
   {
      bstream->writeAffineTransform(mObjToWorld);
      mathWrite(*bstream, mObjScale);
   }

   if(bstream->writeFlag(mask & advancedStaticOptionsMask))
   {
	   bstream->writeFlag(receiveSunLight);
	   bstream->writeFlag(useAdaptiveSelfIllumination);
	   bstream->writeFlag(useCustomAmbientLighting);
	   bstream->writeFlag(customAmbientForSelfIllumination);
	   bstream->write(customAmbientLighting);
	   bstream->writeFlag(receiveLMLighting);
      bstream->writeFlag(useLightingOcclusion);

	   if(isServerObject())
	   {
		   lightIds.clear();
		   findLightGroup(connection);

		   U32 maxcount = getMin(lightIds.size(), SG_TSSTATIC_MAX_LIGHTS);
		   bstream->writeInt(maxcount, SG_TSSTATIC_MAX_LIGHT_SHIFT);
		   for(U32 i=0; i<maxcount; i++)
		   {
			   bstream->writeInt(lightIds[i], NetConnection::GhostIdBitSize);
		   }
	   }
	   else
	   {
		   // recording demo...
		   U32 maxcount = getMin(lightIds.size(), SG_TSSTATIC_MAX_LIGHTS);
		   bstream->writeInt(maxcount, SG_TSSTATIC_MAX_LIGHT_SHIFT);
		   for(U32 i=0; i<maxcount; i++)
		   {
			   bstream->writeInt(lightIds[i], NetConnection::GhostIdBitSize);
		   }
	   }
   }
   
   return retMask;
}

void RoadVehicle::unpackUpdate(NetConnection *connection, BitStream *bstream)
{
   Parent::unpackUpdate(connection,bstream);
   if (bstream->readFlag()) 
   {
      MatrixF mat;
      bstream->readAffineTransform(&mat);
      Parent::setTransform(mat);
      Parent::setRenderTransform(mat);

      VectorF scale;
      mathRead(*bstream, &scale);
      setScale(scale);
   }

   if(bstream->readFlag())
   {
	   receiveSunLight = bstream->readFlag();
	   useAdaptiveSelfIllumination = bstream->readFlag();
	   useCustomAmbientLighting = bstream->readFlag();
	   customAmbientForSelfIllumination = bstream->readFlag();
	   bstream->read(&customAmbientLighting);
	   receiveLMLighting = bstream->readFlag();
      useLightingOcclusion = bstream->readFlag();

	   U32 count = bstream->readInt(SG_TSSTATIC_MAX_LIGHT_SHIFT);
	   lightIds.clear();
	   for(U32 i=0; i<count; i++)
	   {
		   S32 id = bstream->readInt(NetConnection::GhostIdBitSize);
		   lightIds.push_back(id);
	   }
   }
}

