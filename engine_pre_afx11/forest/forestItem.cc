//-----------------------------------------------------------------------------
// Forest Renderer Pack
// Copyright (C) PushButton Labs
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "dgl/dgl.h"
#include "core/dnet.h"
#include "core/bitstream.h"
#include "game/game.h"
#include "math/mMath.h"
#include "math/MathIO.h"
#include "console/simBase.h"
#include "console/console.h"
#include "console/ConsoleTypes.h"
#include "game/moveManager.h"
#include "game/gameConnection.h"
#include "ts/tsShapeInstance.h"
#include "core/resManager.h"
#include "terrain/terrData.h"
#include "terrain/terrRender.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sceneState.h"

#include "forest/forestItem.h"
#include "forest/forest.h"
#include "forest/forestTextureSheet.h"

extern void wireCube(F32 size,Point3F pos);

//#define DEFAULT_FULL_VOLUME_DISTANCE 10.0f
//#define DEFAULT_MAX_AUDIBLE_DISTANCE 50.0f

U32                ForestItem::smTotalItems = 0;
bool               ForestItem::smLowDetail  = false;
bool               ForestItem::smDrawRadius = true;
F32                ForestItemData::smMaxItemHeight = 0.f;
//Audio::Description ForestItem::smAudioDescription;

//const S32 SOUND_INTERVAL = 4000;
//----------------------------------------------------------------------------

IMPLEMENT_CO_DATABLOCK_V1(ForestItemData);


ForestItemData::ForestItemData()
{
   mAligned      = false;
   mCentered     = false;
   mClearCut     = false;
   mCollidable   = false;
   mMaxItems     = 1;
   mRadius       = 2.0f;
   mMinScaleFactor  = 1.0f;
   mMaxScaleFactor  = 1.0f;
   mSound        = 0;
   mShapeFile    = 0;
   mInitedShape  = false;
   mDisableBillboardLighting = false;
}

void ForestItemData::initPersistFields()
{
   Parent::initPersistFields();

   addField("disableBillboardLighting", TypeBool,
                                       Offset(mDisableBillboardLighting,
                                                                   ForestItemData));
   addField("centered",      TypeBool,       Offset(mCentered,     ForestItemData));
   addField("clearcut",      TypeBool,       Offset(mClearCut,     ForestItemData));
   addField("collidable",    TypeBool,       Offset(mCollidable,   ForestItemData));
   addField("groundAligned", TypeBool,       Offset(mAligned,      ForestItemData));
   addField("maxItems",      TypeS32,        Offset(mMaxItems,     ForestItemData));
   addField("radius",        TypeF32,        Offset(mRadius,       ForestItemData));
   addField("minScaleFactor",   TypeF32,     Offset(mMinScaleFactor,ForestItemData));
   addField("maxScaleFactor",   TypeF32,     Offset(mMaxScaleFactor,ForestItemData));
   addField("sound",         TypeCaseString, Offset(mSound,        ForestItemData));
   addField("shapeFile",     TypeCaseString, Offset(mShapeFile,    ForestItemData));
}

void ForestItemData::consoleInit()
{
    Con::addVariable("pref::FI::lowDetail", TypeBool, &ForestItem::smLowDetail);
    Con::addVariable("drawForestRadius", TypeBool, &ForestItem::smDrawRadius);
}

//----------------------------------------------------------------------------

void ForestItemData::packData(BitStream* stream)
{ 
   AssertISV(false, "ForestItemData::packData - Don't declare ForestItemDatas as datablocks unless you know what you are doing!");
   Parent::packData(stream);

   stream->writeFlag(mCentered);
   stream->writeFlag(mClearCut);
   stream->writeFlag(mCollidable);
   stream->writeFlag(mAligned);
   stream->writeFlag(mDisableBillboardLighting);

   stream->write(mMaxItems);
   stream->write(mRadius);
   stream->write(mMinScaleFactor);
   stream->write(mMaxScaleFactor);
   stream->writeString(getName());
   stream->writeString(category);
   stream->writeString(mSound);
   stream->writeString(mShapeFile);
}

//----------------------------------------------------------------------------

void ForestItemData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   mCentered   = stream->readFlag();
   mClearCut   = stream->readFlag();
   mCollidable = stream->readFlag();
   mAligned    = stream->readFlag();
   mDisableBillboardLighting = stream->readFlag();

   stream->read(&mMaxItems);
   stream->read(&mRadius);
   stream->read(&mMinScaleFactor);
   stream->read(&mMaxScaleFactor);
   char readBuffer[1024];
   stream->readString(readBuffer);
   assignName(readBuffer);
   stream->readString(readBuffer);
   category   = StringTable->insert(readBuffer);
   stream->readString(readBuffer);
   mSound     = StringTable->insert(readBuffer);
   stream->readString(readBuffer);
   mShapeFile = StringTable->insert(readBuffer);

}

//----------------------------------------------------------------------------

void ForestItemData::initShape()
{
   if(mInitedShape)
      return;

   // Scan out the collision hulls...
   U32 i;
   for (i = 0; i < MaxCollisionShapes; i++) 
   {
      char buff[128];
      dSprintf(buff, sizeof(buff), "Collision-%d", i + 1);
      mCollisionDetails[i] = mShapeInstance->getShape()->findDetail(buff);
      mLOSDetails[i]       = mCollisionDetails[i];
   }

   // Compute the hull accelerators (actually, just force the shape to compute them)
   for (i = 0; i < MaxCollisionShapes; i++) 
      if (mCollisionDetails[i] != -1) 
         mShapeInstance->getShape()->getAccelerator(mCollisionDetails[i]);

   // Update max height, used for avoiding popping during rendering.
   F32 potentialMax = mShapeInstance->getShape()->bounds.len_z() * mMaxScaleFactor;

   if(potentialMax > smMaxItemHeight)
      smMaxItemHeight = potentialMax;

   mInitedShape = true;
}


//----------------------------------------------------------------------------

ForestItem::ForestItem()
{
   mObjScale.set(1,1,1);
   mObjToWorld.identity();
   mPosition.set(0,0,0);
   smTotalItems++;
   mCollideTime = 0;
   mOffsetCache.set(-10000, -10000); // Hopefully a safe sentinel value.
}

ForestItem::~ForestItem()
{

}

void ForestItem::consoleInit()
{

}


void ForestItem::setPosition(Point3F const& in_pos,Point3F const& in_rot, F32 in_scaleFactor)
{
   MatrixF mat;
   mObjScale.set(in_scaleFactor, in_scaleFactor, in_scaleFactor);
   mat.set(EulerF(0, 0, in_rot.z));
   mat.setPosition(in_pos);
   setTransform(mat);
   mPosition = in_pos;
   mInvScale = (1.0f/getMax(getMax(mObjScale.x, mObjScale.y), mObjScale.z));
}

//----------------------------------------------------------------------------

void ForestItem::setPosition(Point3F const& in_pos)
{
   MatrixF & mat = mObjToWorld;
   mat.setPosition(in_pos);
   setTransform(mat);
}

//----------------------------------------------------------------------------

void ForestItem::setTransform(MatrixF const& mat)
{
   mObjToWorld = mat;
}

//----------------------------------------------------------------------------
void ForestItem::setShapeInstance( TSShapeInstance * in_shapeInst, S32 num )
{ 
   // Invalidate the init if we have to...
   if(mDataBlock->mShapeInstance != in_shapeInst)
      mDataBlock->mInitedShape = false;

   mDataBlock->mShapeInstance = in_shapeInst; // these two items get set over
   mDataBlock->mShapeIndex = num;             // and over...no worry

   // And make sure it has latest collision info.
   mDataBlock->initShape();

   F32 radius = in_shapeInst->getShape()->radius;

   mPoints[0].set( -radius, 0, radius );
   mPoints[1].set( radius, 0, radius );
   mPoints[2].set( radius, 0, -radius );
   mPoints[3].set( -radius, 0, -radius );

   mCenter = in_shapeInst->getShape()->center;
}

//----------------------------------------------------------------------------

const Point3F & ForestItem::getCurrentPos()
{
   static Point3F curPos;
   mObjToWorld.getColumn(3,&curPos);
   return curPos;
}

