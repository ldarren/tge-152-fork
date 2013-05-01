//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "rpg/game/rpgTSDynamic.h"
#include "core/bitStream.h"
#include "dgl/dgl.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "math/mathIO.h"
#include "ts/tsShapeInstance.h"
#include "console/consoleTypes.h"
#include "game/shapeBase.h"
#include "game/shadow.h"
#include "sceneGraph/detailManager.h"
#include "sim/netConnection.h"
#include "game/gameConnection.h"
#include "math/mathUtils.h"

#include "stdlib.h" //hahahah stdlib!

IMPLEMENT_CO_NETOBJECT_V1(TSDynamic);


static F32	gDirs[162][3] = {
   {-0.525731, 0.000000, 0.850651}, {-0.442863, 0.238856, 0.864188}, 
   {-0.295242, 0.000000, 0.955423}, {-0.309017, 0.500000, 0.809017}, 
   {-0.162460, 0.262866, 0.951056}, {0.000000, 0.000000, 1.000000}, 
   {0.000000, 0.850651, 0.525731}, {-0.147621, 0.716567, 0.681718}, 
   {0.147621, 0.716567, 0.681718}, {0.000000, 0.525731, 0.850651}, 
   {0.309017, 0.500000, 0.809017}, {0.525731, 0.000000, 0.850651}, 
   {0.295242, 0.000000, 0.955423}, {0.442863, 0.238856, 0.864188}, 
   {0.162460, 0.262866, 0.951056}, {-0.681718, 0.147621, 0.716567}, 
   {-0.809017, 0.309017, 0.500000}, {-0.587785, 0.425325, 0.688191}, 
   {-0.850651, 0.525731, 0.000000}, {-0.864188, 0.442863, 0.238856}, 
   {-0.716567, 0.681718, 0.147621}, {-0.688191, 0.587785, 0.425325}, 
   {-0.500000, 0.809017, 0.309017}, {-0.238856, 0.864188, 0.442863}, 
   {-0.425325, 0.688191, 0.587785}, {-0.716567, 0.681718, -0.147621}, 
   {-0.500000, 0.809017, -0.309017}, {-0.525731, 0.850651, 0.000000}, 
   {0.000000, 0.850651, -0.525731}, {-0.238856, 0.864188, -0.442863}, 
   {0.000000, 0.955423, -0.295242}, {-0.262866, 0.951056, -0.162460}, 
   {0.000000, 1.000000, 0.000000}, {0.000000, 0.955423, 0.295242}, 
   {-0.262866, 0.951056, 0.162460}, {0.238856, 0.864188, 0.442863}, 
   {0.262866, 0.951056, 0.162460}, {0.500000, 0.809017, 0.309017}, 
   {0.238856, 0.864188, -0.442863}, {0.262866, 0.951056, -0.162460}, 
   {0.500000, 0.809017, -0.309017}, {0.850651, 0.525731, 0.000000}, 
   {0.716567, 0.681718, 0.147621}, {0.716567, 0.681718, -0.147621}, 
   {0.525731, 0.850651, 0.000000}, {0.425325, 0.688191, 0.587785}, 
   {0.864188, 0.442863, 0.238856}, {0.688191, 0.587785, 0.425325}, 
   {0.809017, 0.309017, 0.500000}, {0.681718, 0.147621, 0.716567}, 
   {0.587785, 0.425325, 0.688191}, {0.955423, 0.295242, 0.000000}, 
   {1.000000, 0.000000, 0.000000}, {0.951056, 0.162460, 0.262866}, 
   {0.850651, -0.525731, 0.000000}, {0.955423, -0.295242, 0.000000}, 
   {0.864188, -0.442863, 0.238856}, {0.951056, -0.162460, 0.262866}, 
   {0.809017, -0.309017, 0.500000}, {0.681718, -0.147621, 0.716567}, 
   {0.850651, 0.000000, 0.525731}, {0.864188, 0.442863, -0.238856}, 
   {0.809017, 0.309017, -0.500000}, {0.951056, 0.162460, -0.262866}, 
   {0.525731, 0.000000, -0.850651}, {0.681718, 0.147621, -0.716567}, 
   {0.681718, -0.147621, -0.716567}, {0.850651, 0.000000, -0.525731}, 
   {0.809017, -0.309017, -0.500000}, {0.864188, -0.442863, -0.238856}, 
   {0.951056, -0.162460, -0.262866}, {0.147621, 0.716567, -0.681718}, 
   {0.309017, 0.500000, -0.809017}, {0.425325, 0.688191, -0.587785}, 
   {0.442863, 0.238856, -0.864188}, {0.587785, 0.425325, -0.688191}, 
   {0.688191, 0.587785, -0.425325}, {-0.147621, 0.716567, -0.681718}, 
   {-0.309017, 0.500000, -0.809017}, {0.000000, 0.525731, -0.850651}, 
   {-0.525731, 0.000000, -0.850651}, {-0.442863, 0.238856, -0.864188}, 
   {-0.295242, 0.000000, -0.955423}, {-0.162460, 0.262866, -0.951056}, 
   {0.000000, 0.000000, -1.000000}, {0.295242, 0.000000, -0.955423}, 
   {0.162460, 0.262866, -0.951056}, {-0.442863, -0.238856, -0.864188}, 
   {-0.309017, -0.500000, -0.809017}, {-0.162460, -0.262866, -0.951056}, 
   {0.000000, -0.850651, -0.525731}, {-0.147621, -0.716567, -0.681718}, 
   {0.147621, -0.716567, -0.681718}, {0.000000, -0.525731, -0.850651}, 
   {0.309017, -0.500000, -0.809017}, {0.442863, -0.238856, -0.864188}, 
   {0.162460, -0.262866, -0.951056}, {0.238856, -0.864188, -0.442863}, 
   {0.500000, -0.809017, -0.309017}, {0.425325, -0.688191, -0.587785}, 
   {0.716567, -0.681718, -0.147621}, {0.688191, -0.587785, -0.425325}, 
   {0.587785, -0.425325, -0.688191}, {0.000000, -0.955423, -0.295242}, 
   {0.000000, -1.000000, 0.000000}, {0.262866, -0.951056, -0.162460}, 
   {0.000000, -0.850651, 0.525731}, {0.000000, -0.955423, 0.295242}, 
   {0.238856, -0.864188, 0.442863}, {0.262866, -0.951056, 0.162460}, 
   {0.500000, -0.809017, 0.309017}, {0.716567, -0.681718, 0.147621}, 
   {0.525731, -0.850651, 0.000000}, {-0.238856, -0.864188, -0.442863}, 
   {-0.500000, -0.809017, -0.309017}, {-0.262866, -0.951056, -0.162460}, 
   {-0.850651, -0.525731, 0.000000}, {-0.716567, -0.681718, -0.147621}, 
   {-0.716567, -0.681718, 0.147621}, {-0.525731, -0.850651, 0.000000}, 
   {-0.500000, -0.809017, 0.309017}, {-0.238856, -0.864188, 0.442863}, 
   {-0.262866, -0.951056, 0.162460}, {-0.864188, -0.442863, 0.238856}, 
   {-0.809017, -0.309017, 0.500000}, {-0.688191, -0.587785, 0.425325}, 
   {-0.681718, -0.147621, 0.716567}, {-0.442863, -0.238856, 0.864188}, 
   {-0.587785, -0.425325, 0.688191}, {-0.309017, -0.500000, 0.809017}, 
   {-0.147621, -0.716567, 0.681718}, {-0.425325, -0.688191, 0.587785}, 
   {-0.162460, -0.262866, 0.951056}, {0.442863, -0.238856, 0.864188}, 
   {0.162460, -0.262866, 0.951056}, {0.309017, -0.500000, 0.809017}, 
   {0.147621, -0.716567, 0.681718}, {0.000000, -0.525731, 0.850651}, 
   {0.425325, -0.688191, 0.587785}, {0.587785, -0.425325, 0.688191}, 
   {0.688191, -0.587785, 0.425325}, {-0.955423, 0.295242, 0.000000}, 
   {-0.951056, 0.162460, 0.262866}, {-1.000000, 0.000000, 0.000000}, 
   {-0.850651, 0.000000, 0.525731}, {-0.955423, -0.295242, 0.000000}, 
   {-0.951056, -0.162460, 0.262866}, {-0.864188, 0.442863, -0.238856}, 
   {-0.951056, 0.162460, -0.262866}, {-0.809017, 0.309017, -0.500000}, 
   {-0.864188, -0.442863, -0.238856}, {-0.951056, -0.162460, -0.262866}, 
   {-0.809017, -0.309017, -0.500000}, {-0.681718, 0.147621, -0.716567}, 
   {-0.681718, -0.147621, -0.716567}, {-0.850651, 0.000000, -0.525731}, 
   {-0.688191, 0.587785, -0.425325}, {-0.587785, 0.425325, -0.688191}, 
   {-0.425325, 0.688191, -0.587785}, {-0.425325, -0.688191, -0.587785}, 
   {-0.587785, -0.425325, -0.688191}, {-0.688191, -0.587785, -0.425325},
};


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
TSDynamic::TSDynamic()
{
   mNetFlags.set(Ghostable| ScopeAlways);

   mTypeMask |= StaticObjectType;

   mShapeName        = "";
   mAnimation        = "";
   mShapeInstance    = NULL;
   mThread = NULL;
   mLastTime = 0xFFFFFFFF;

   mCount = 1;
   mRadiusX = 0.f;
   mRadiusY = 0.f;
   mRadiusZ = 0.f;

   mSpeed = 1.f;
   mMyScale = 1.f;

   mFadeIn = 0.f;
   mFadeOut = 0.f;



   for (U32 i = 0; i < 162; i++){
      mVelocities[i][0] = (rand() & 255) * 0.01f;
      mVelocities[i][1] = (rand() & 255) * 0.01f;
      mVelocities[i][2] = (rand() & 255) * 0.01f;
   }

}

TSDynamic::~TSDynamic()
{
}

//--------------------------------------------------------------------------
void TSDynamic::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Media");
   addField("shapeName", TypeFilename, Offset(mShapeName, TSDynamic));
   addField("animation", TypeCaseString, Offset(mAnimation, TSDynamic));
   addField("radiusX", TypeF32, Offset(mRadiusX, TSDynamic));
   addField("radiusY", TypeF32, Offset(mRadiusY, TSDynamic));
   addField("radiusZ", TypeF32, Offset(mRadiusZ, TSDynamic));
   addField("speed", TypeF32, Offset(mSpeed, TSDynamic));
   addField("count", TypeS32, Offset(mCount, TSDynamic));
   addField("myscale", TypeF32, Offset(mMyScale, TSDynamic)); //because changing scale was messing with frustrim clipping
   endGroup("Media");

}


//--------------------------------------------------------------------------
bool TSDynamic::onAdd()
{
   if(!Parent::onAdd())
      return false;

   if (!mShapeName || mShapeName[0] == '\0') {
      Con::errorf("TSDynamic::onAdd: no shape name!");
      return false;
   }
   mShapeHash = _StringTable::hashString(mShapeName);
   
   mShape = ResourceManager->load(mShapeName);

   if (bool(mShape) == false) 
   {
      Con::errorf("TSDynamic::onAdd: unable to load shape: %s", mShapeName);
      return false;
   }

   if(isClientObject() && !mShape->preloadMaterialList() && NetConnection::filesWereDownloaded())
      return false;

   mObjBox = mShape->bounds;
   if (mRadiusX> 1.f)
   {
      mObjBox.min.x=-mRadiusX*10;
      mObjBox.max.x=mRadiusX*10;
   }
   if (mRadiusY> 1.f)
   {
      mObjBox.min.y=-mRadiusY*10;
      mObjBox.max.y=mRadiusY*10;
   }
   if (mRadiusZ> 1.f)
   {
      mObjBox.min.z=-mRadiusZ*10;
      mObjBox.max.z=mRadiusZ*10;
   }
      
   
   resetWorldBox();
   setRenderTransform(mObjToWorld);

   
   mShapeInstance = new TSShapeInstance(mShape, isClientObject());

   addToScene();

   
   if (isClientObject())
   if (mAnimation && mAnimation[0] && mShapeInstance)
   {

      S32 seq = mShape->findSequence(mAnimation);
      if(seq!=-1)
      {      
         if (!mThread)
         {
            mThread = mShapeInstance->addThread();
         }
         AssertISV(mThread,"Unable to create tsDynamic thread");
         mShapeInstance->setSequence(mThread,seq,0);
      }
   }


   return true;
}


void TSDynamic::onRemove()
{

   removeFromScene();

   delete mShapeInstance;
   mShapeInstance = NULL;

   Parent::onRemove();
}



//--------------------------------------------------------------------------
bool TSDynamic::prepRenderImage(SceneState* state, const U32 stateKey,
                                       const U32 /*startZone*/, const bool /*modifyBaseState*/)
{

#ifdef DARREN_MMO   
   if (!isDNActive() && mFadeOut>=1.f)
      return false;


   if (isLastState(state, stateKey))
      return false;
   setLastState(state, stateKey);

   // This should be sufficient for most objects that don't manage zones, and
   //  don't need to return a specialized RenderImage...
   if (mShapeInstance && state->isObjectRendered(this)) {
      Point3F cameraOffset;
      getRenderTransform().getColumn(3,&cameraOffset);
      cameraOffset -= state->getCameraPosition();
      F32 dist = cameraOffset.len();
      if (dist < 0.01)
         dist = 0.01;
      F32 fogAmount = state->getHazeAndFog(dist,cameraOffset.z);
      if (fogAmount>0.99f)
         return false;

      const SphereF& sphere = getWorldSphere();
      GameConnection* gconn = GameConnection::getConnectionToServer();
      Point3F loc = gconn->getControlObject()->getPosition();
      loc-=getPosition();
      F32 distSquared = loc.lenSquared();
      F32 rad = sphere.radius;


      if (rad < 10.f)
         rad = 10.f;


      F32 cliprange = 5.f*rad*SceneGraph::smMyVisibleDistanceMod;
      if (distSquared>cliprange*cliprange)
         return false;



      F32 invScale = (1.0f/getMax(getMax(mObjScale.x,mObjScale.y),mObjScale.z));
      //JMR disabling detail levels on DTS
      DetailManager::selectPotentialDetails(mShapeInstance,dist,invScale);
      //DetailManager::selectPotentialDetails(mShapeInstance,1,invScale);
      if (mShapeInstance->getCurrentDetail()<0)
         return false;

      if (1)//mShapeInstance->hasSolid())
      {
         SceneRenderImage* image = new SceneRenderImage;
         image->obj = this;
         image->isTranslucent = false;
         image->textureSortKey = mShapeHash;
         state->insertRenderImage(image);
      }

      /*
      if (mShapeInstance->hasTranslucency())
      {
         SceneRenderImage* image = new SceneRenderImage;
         image->obj = this;
         image->isTranslucent = true;
         image->sortType = SceneRenderImage::Point;
         image->textureSortKey = mShapeHash;
         state->setImageRefPoint(this, image);

         state->insertRenderImage(image);
      }
      */
   }
#endif // DARREN_MMO
   return false;
}


void TSDynamic::setTransform(const MatrixF & mat)
{
   Parent::setTransform(mat);

   // Since the interior is a static object, it's render transform changes 1 to 1
   //  with it's collision transform
   setRenderTransform(mat);
}

void TSDynamic::move(U32 i, F32 time, const Point3F& org, MatrixF& transform)
{
   if (i>161)
      return;
   time*=mSpeed;
   F32 angle = time * mVelocities[i][0];
   F32 sy = sin(angle);
   F32 cy = cos(angle);
   angle = time * mVelocities[i][1];
   F32 sp = sin(angle);
   F32 cp = cos(angle);
   Point3F vec;

   vec[0] = cp*cy;
   vec[1] = cp*sy;
   vec[2] = -sp;

   F32 d = sin(time + i);
   Point3F loc;

   loc[0] = org[0] + gDirs[i][0]*d*mRadiusX + vec[0]*mRadiusX;
   loc[1] = org[1] + gDirs[i][1]*d*mRadiusY + vec[1]*mRadiusY;
   loc[2] = org[2] + gDirs[i][2]*d*mRadiusZ + vec[2]*mRadiusZ;

   Point3F n = loc-mLastPosition[i];
   n[2]=0.f;
   n.normalize();

   transform = MathUtils::createOrientFromDir(n);
   transform.setPosition(loc);

  
}

void TSDynamic::renderObject(SceneState* state, SceneRenderImage* image)
{
#ifdef DARREN_MMO
   AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");


   if (!DetailManager::selectCurrentDetail(mShapeInstance))
      // we were detailed out
      return;

   if (mLastTime==0xFFFFFFFF)
      mLastTime = Platform::getVirtualMilliseconds();

   U32 dtime = Platform::getVirtualMilliseconds()-mLastTime;
   mLastTime = Platform::getVirtualMilliseconds();
   F32 dt = F32(dtime)/1000.f;
   F32 time = F32(mLastTime)*.001f;
   GameConnection* gconn = GameConnection::getConnectionToServer();
   Point3F cloc = gconn->getControlObject()->getPosition();
   F32 rad = mShape->radius*mMyScale;
   

   if (rad < 10.f)
      rad = 10.f;

   F32 radius = rad*2.f;
   F32 cliprange = 12.f*rad*SceneGraph::smMyVisibleDistanceMod;



   if (dt > .1f)
      dt = .1f;

   Point3F location = getPosition();
   MatrixF transform;

   bool dna = isDNActive();
   if (!dna)
   {
      mFadeIn = 0.f;
      mFadeOut+=dt*.2f;
   }
   else
   {
      if (mFadeIn<1.f)
         mFadeIn+=dt*.2f;
      mFadeOut = 0.f;
   }

   RectI viewport;
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   dglGetViewport(&viewport);

   gClientSceneGraph->getLightManager()->sgSetupLights(this);


   // Uncomment this if this is a "simple" (non-zone managing) object
   state->setupObjectProjection(this);


      
   if (mShapeInstance && mThread)
   for (U32 i=0;i<mCount;i++)
   {
      
      move(i, time, location, transform);

      
      F32 fade=1.f;
      
      mLastPosition[i]=transform.getPosition();
      Point3F loc=cloc-mLastPosition[i];
      F32 d = loc.len();

      

      d = cliprange-d;
      if (d<=radius)
      {
         d/=radius;
         fade = d;
      }

      if (fade < .01f)
      {
         
         continue;
      }

      if (!dna)
      {
         if (fade > 1.0f-mFadeOut)
            fade = 1.0f-mFadeOut;
      }
      else
      {
         if (fade > mFadeIn)
            fade = mFadeIn;
      }

      if (fade > 1)
         fade = 1.f;

      
      mShapeInstance->setTime(mThread,F32(i)*(mShapeInstance->getDuration(mThread)/10.f)+time);

      




      // This is something of a hack, but since the 3space objects don't have a
      //  clear conception of texels/meter like the interiors do, we're sorta
      //  stuck.  I can't even claim this is anything more scientific than eyeball
      //  work.  DMM
      
      
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      dglMultMatrix(&transform);
      glScalef(mMyScale, mMyScale,mMyScale);

      // RENDER CODE HERE
      mShapeInstance->setAlphaAlways(1.0);

      Point3F cameraOffset;
      mObjToWorld.getColumn(3,&cameraOffset);
      cameraOffset -= state->getCameraPosition();
      F32 dist = cameraOffset.len();
      F32 fogAmount = state->getHazeAndFog(dist,cameraOffset.z);

      /*
      if (image->isTranslucent == true)
      {
         TSShapeInstance::smNoRenderNonTranslucent = true;
         TSShapeInstance::smNoRenderTranslucent    = false;
      }
      else
      {
         TSShapeInstance::smNoRenderNonTranslucent = false;
         TSShapeInstance::smNoRenderTranslucent    = true;
      }
      */

      TSShapeInstance::smNoRenderNonTranslucent = false;
      TSShapeInstance::smNoRenderTranslucent    = false;

      TSMesh::setOverrideFade( fade);

      mShapeInstance->setupFog(fogAmount,state->getFogColor());
      mShapeInstance->animate();
      mShapeInstance->render();

      //renderShadow(dist,fogAmount);

      TSShapeInstance::smNoRenderNonTranslucent = false;
      TSShapeInstance::smNoRenderTranslucent    = false;
      TextureManager::setSmallTexturesActive(false);

      TSMesh::setOverrideFade(1.0 );
      

      
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();

  //   dglSetCanonicalState();

      if (GameBase::gShowBoundingBox) {
         glDisable(GL_DEPTH_TEST);
         Point3F box;
         glPushMatrix();
         dglMultMatrix(&getTransform());
         box = (mObjBox.min + mObjBox.max) * 0.5;
         glTranslatef(box.x,box.y,box.z);
         box = (mObjBox.max - mObjBox.min) * 0.5;
         glScalef(box.x,box.y,box.z);
         glColor3f(1, 0, 1);
         ShapeBase::wireCube(Point3F(1,1,1),Point3F(0,0,0));
         glPopMatrix();

         glPushMatrix();
         box = (mWorldBox.min + mWorldBox.max) * 0.5;
         glTranslatef(box.x,box.y,box.z);
         box = (mWorldBox.max - mWorldBox.min) * 0.5;
         glScalef(box.x,box.y,box.z);
         glColor3f(0, 1, 1);
         ShapeBase::wireCube(Point3F(1,1,1),Point3F(0,0,0));
         glPopMatrix();
         glEnable(GL_DEPTH_TEST);
      }


      

   }

   //setPosition(location);

   gClientSceneGraph->getLightManager()->sgResetLights();


   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   dglSetViewport(viewport);

   GLfloat matProp[] = {0.0f,0.0f,0.0,0.0};
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,matProp);


   AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
#endif // DARREN_MMO
}

U32 TSDynamic::packUpdate(NetConnection *con, U32 mask, BitStream *stream)
{
   U32 retMask = Parent::packUpdate(con, mask, stream);
#ifdef DARREN_MMO

   mathWrite(*stream, getTransform());
   mathWrite(*stream, getScale());

   stream->write(mDNStartTime);
   stream->write(mDNEndTime);
   stream->write(mCount);
   stream->write(mRadiusX);
   stream->write(mRadiusY);
   stream->write(mRadiusZ);
   stream->write(mSpeed);
   stream->write(mMyScale);

   stream->writeString(mShapeName);
   stream->writeString(mAnimation);

   if ((bool(mShape) == true) && mAnimation && mAnimation[0] && mShapeInstance)
   {

      S32 seq = mShape->findSequence(mAnimation);
      if(seq!=-1)
      {      
         if (!mThread)
         {
            mThread = mShapeInstance->addThread();
         }
         AssertISV(mThread,"Unable to create tsDynamic thread");
         mShapeInstance->setSequence(mThread,seq,0);
         
      }
   }
   
#endif // DARREN_MMO
   return retMask;
}


void TSDynamic::unpackUpdate(NetConnection *con, BitStream *stream)
{
   Parent::unpackUpdate(con, stream);
#ifdef DARREN_MMO
   MatrixF mat;
   Point3F scale;
   mathRead(*stream, &mat);
   mathRead(*stream, &scale);
   setScale(scale);
   setTransform(mat);

   stream->read(&mDNStartTime);
   stream->read(&mDNEndTime);
   stream->read(&mCount);
   stream->read(&mRadiusX);
   stream->read(&mRadiusY);
   stream->read(&mRadiusZ);
   stream->read(&mSpeed);
   stream->read(&mMyScale);

   mShapeName = stream->readSTString();
   mAnimation = stream->readSTString();

   if (bool(mShape))
   {
      mObjBox = mShape->bounds;
      if (mRadiusX> 1.f)
      {
         mObjBox.min.x=-mRadiusX*1.25f;
         mObjBox.max.x=mRadiusX*1.25f;
      }
      if (mRadiusY> 1.f)
      {
         mObjBox.min.y=-mRadiusY*1.25f;
         mObjBox.max.y=mRadiusY*1.25f;
      }
      if (mRadiusZ> 1.f)
      {
         mObjBox.min.z=-mRadiusZ*1.25f;
         mObjBox.max.z=mRadiusZ*1.25f;
      }

   }

#endif // DARREN_MMO      

}

void TSDynamic::inspectPostApply()
{
   if(isServerObject()) {
      setMaskBits(0xffffffff);
   }
}
