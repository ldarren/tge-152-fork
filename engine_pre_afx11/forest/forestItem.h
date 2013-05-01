//-----------------------------------------------------------------------------
// Forest Renderer Pack
// Copyright (C) PushButton Labs
//-----------------------------------------------------------------------------

#ifndef _FORESTITEM_H_
#define _FORESTITEM_H_

#include "ts/tsshapeInstance.h"
#include "audio/audio.h"
#include "forest/forestCollision.h"
#include "game/gameBase.h"

class Forest;

//----------------------------------------------------------------------------

struct ForestItemData : public GameBaseData 
{
   typedef GameBaseData Parent;
   
   F32  mMinScaleFactor;
   F32  mMaxScaleFactor;

   F32  mRadius;
   bool mCollidable;
   bool mAligned;
   S32  mMaxItems;
   bool mCentered;
   bool mClearCut;
   bool mDisableBillboardLighting;
   StringTableEntry mSound;
   StringTableEntry mShapeFile;

   static F32 smMaxItemHeight;

   enum Constants 
   {
      MaxCollisionShapes = 8
   };

   // these are set during forest creation...
   TSShapeInstance * mShapeInstance;
   S32 mShapeIndex;
   S32 mCollisionDetails[MaxCollisionShapes];
   S32 mLOSDetails[MaxCollisionShapes];
   bool mInitedShape;
   
   void initShape();

   DECLARE_CONOBJECT(ForestItemData);
   ForestItemData();
   
   static void  consoleInit();
   static void  initPersistFields();
   
   virtual void packData(BitStream* stream);
   virtual void unpackData(BitStream* stream);
   
   const F32 getMinScaleFactor() const { return mMinScaleFactor;}
   const F32 getMaxScaleFactor() const { return mMaxScaleFactor;}
   F32       getRadius()            { return mRadius; }
};


//----------------------------------------------------------------------------

class ForestItem
{
   static U32                smTotalItems;

   U32                mCollideTime;
   ForestItemData   * mDataBlock;
   F32                mInvScale;
   MatrixF            mObjToWorld;   // object transform
   Point3F            mObjScale;     // object scale
   Point3F            mPosition;

   // used for rendering -- pack as best we can...
   S8 tsDL1;
   S8 tsDL2;

   //BJGTODO - put in single U8...
   U8 separateTranslucency; // bool...
   U8 fogged;               // bool...
   F32 alpha1;
   F32 alpha2;
   F32 mViewDistance;
   F32 fogAmount;

public:
   // Billboard caching code.
   Point2F mOffsetCache;  ///< If offsets change, we reset the cache info.
   SphereF mValidView; ///< If cam goes outside this sphere we recalc.
   F32     mRotY;      ///< Amount to roll by if any.
   S32     mBitmap;    ///< bitmap we're currently rendering as a billboard.
   Point3F mVerts[4];  ///< Cached geometry in worldspace.
   Point3F mPoints[4]; ///< Cached geometry in objectspace.
   Point3F mCenter;    ///< Center of the shape in objectspace;
   Point3F mNormal;    ///< Our normal.

   static bool smLowDetail;
   static bool smDrawRadius;
   
   static void consoleInit();
   static void initPersistFields();
   static U32  getTotalItems()     { return smTotalItems; }
   static void resetTotalItems()   { smTotalItems = 0;    }
   
   ForestItem();
   ~ForestItem();

   // a bunch of methods that are used by forest render function...
   S32 getTSDetail1() { return tsDL1; }
   S32 getTSDetail2() { return tsDL2; }
   F32 getAlpha1()    { return alpha1; }
   F32 getAlpha2()    { return alpha2; }
   S32 getSeparateTranslucency() { return separateTranslucency; }
   void setForestDetails(S32 dl1, F32 a1, S32 dl2, F32 a2, bool sep) { tsDL1=dl1; tsDL2=dl2; alpha1=a1; alpha2=a2; separateTranslucency = sep; }
   void setSquaredDistance(F32 d) { mViewDistance = d; }
   void setFog(F32 f) { fogAmount = f; fogged = fogAmount > 1.0f/64.0f; }
   bool getFogged() { return fogged; }
   F32 getFogAmount() { return fogAmount; }
   Point3F & getObjectScale() { return mObjScale; }
   F32 getInvScale() { return mInvScale; }
   S32 getShapeNum() { return mDataBlock->mShapeIndex; }
   TSShapeInstance * getShapeInstance() { return mDataBlock->mShapeInstance; }

   bool            collidable()                 { return mDataBlock->mCollidable; }
   bool            clearCutItem()               { return mDataBlock->mClearCut;   }
   StringTableEntry getCategory()               { return mDataBlock->category;    }
   const Point3F & getCurrentPos();
   F32             getHeight()                  { return mDataBlock->mShapeInstance->getShape()->bounds.max.z * mObjScale.z; }
   const Point3F & getPosition()                { return mPosition;               }
   F32             getRadius()                  { return mDataBlock->mRadius * mObjScale.x;}
   F32             getSquaredDistance() const   { return mViewDistance;           }
   MatrixF const & getTransform() const         { return mObjToWorld;             }
   void            onCollision(F32 volume);
   void            setCurrentPos(F32 in_x, F32 in_y, F32 in_z);
   void            setOffsets(F32 xOffset, F32 yOffset);
   void            setPosition(Point3F const& in_pos, Point3F const& in_viewRot, F32 in_scaleFactor);
   void            setPosition(Point3F const& in_pos);
   void            setShapeInstance(TSShapeInstance * in_shapeInst, S32 num);
   void            setTransform(MatrixF const&);
   void            setDataBlock(ForestItemData * in_forestData)  {mDataBlock = in_forestData;};
   Box3F           getObjBox() { return getShapeInstance()->getShape()->bounds; };
   ForestItemData *getData() { return mDataBlock; };

   // Collision magic...
   bool castRay(const Point3F &start, const Point3F &end, RayInfo* info);
   bool buildPolyList(AbstractPolyList* polyList, const Box3F &box, const SphereF& sphere);
};

inline void ForestItem::setOffsets(F32 xOffset, F32 yOffset)
{
   Point3F curPos(mPosition.x+xOffset,mPosition.y+yOffset,mPosition.z);
   mObjToWorld.setColumn(3, curPos);

   if((mOffsetCache - Point2F(xOffset, yOffset)).lenSquared() > 1)
   {
      mValidView.radius = 0;
   }
}

//----------------------------------------------------------------------------


#endif
