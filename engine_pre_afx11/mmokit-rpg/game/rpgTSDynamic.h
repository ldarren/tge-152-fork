//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _RPGTSDynamic_H_
#define _RPGTSDynamic_H_

#ifndef _SCENEOBJECT_H_
#include "sim/sceneObject.h"
#endif
#ifndef _RESMANAGER_H_
#include "core/resManager.h"
#endif


class TSThread;
class TSShape;
class TSShapeInstance;
class TSDynamic;



//--------------------------------------------------------------------------
class TSDynamic : public SceneObject
{
   typedef SceneObject Parent;
   static U32 smUniqueIdentifier;


  protected:
   bool onAdd();
   void onRemove();

  protected:

   StringTableEntry  mAnimation;
   StringTableEntry  mShapeName;
   U32               mShapeHash;
   F32               mRadiusX;
   F32               mRadiusY;
   F32               mRadiusZ;
   F32               mSpeed;
   S32               mCount;
   F32               mMyScale;
   Resource<TSShape> mShape;
   TSShapeInstance*  mShapeInstance;
   U32               mLastTime;
   TSThread*         mThread;
   Point3F           mVelocities[162];
   Point3F           mLastPosition[162];

   F32               mFadeIn;
   F32               mFadeOut;


  public:
   

   // Rendering
  protected:
   bool prepRenderImage  ( SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState=false);
   void renderObject     ( SceneState *state, SceneRenderImage *image);   
   void setTransform     ( const MatrixF &mat);
   void move(U32 i, F32 time, const Point3F& org, MatrixF& transform);
  public:
   TSDynamic();
   ~TSDynamic();

   DECLARE_CONOBJECT(TSDynamic);
   static void initPersistFields();

   U32  packUpdate  (NetConnection *conn, U32 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn,           BitStream *stream);


   //virtual void installLights();
   //virtual void uninstallLights();

   void inspectPostApply();
};

#endif // _RPGTSDynamic_H_

