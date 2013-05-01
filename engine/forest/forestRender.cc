//-----------------------------------------------------------------------------
// Forest Renderer Pack
// Copyright (C) PushButton Labs
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "dgl/dgl.h"
#include "core/dnet.h"
#include "game/game.h"
#include "math/mMath.h"
#include "console/simBase.h"
#include "console/console.h"
#include "console/ConsoleTypes.h"
#include "console/consoleInternal.h"
#include "terrain/terrData.h"
#include "terrain/terrRender.h"
#include "sim/sceneObject.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sgUtil.h"
#include "game/gameBase.h"
#include "ts/TSShapeInstance.h"
#include "ts/TSShapeConstruct.h"
#include "ts/tsLastDetail.h"
#include "math/mathIO.h"
#include "platform/profiler.h"
#include "core/frameAllocator.h"

#include "forest/forestItem.h"
#include "forest/forest.h"
#include "forest/globDock.h"
#include "forest/forestClearing.h"

U32 gFogMethod;
bool terrCheck(TerrainBlock* pBlock,
               const Box3F rBox,
               const Point3F localCamPos,
               F32 height,
               bool aboveTerrain);

//--------------------------------------------------------------------------
bool Forest::prepRenderImage(SceneState* state, const U32 , const U32 , const bool )
{
   SceneRenderImage* image = new SceneRenderImage;

   // Make ourselves live at the head of the list.
   image->sortType         = SceneRenderImage::BeginSort;
   image->obj              = this;
   image->isTranslucent    = true;
   image->tieBreaker       = 0.0f;
   state->insertRenderImage(image);
 
   return true;
}

S32 FN_CDECL cmpPointImageFunc(const void* p1, const void* p2)
{
   const SceneRenderImage* psri1 = *((const SceneRenderImage**)p1);
   const SceneRenderImage* psri2 = *((const SceneRenderImage**)p2);

   if (psri1->pointDistSq != psri2->pointDistSq) 
      return ((psri1->pointDistSq > psri2->pointDistSq) ? -1 : 1);
   else
      return (psri1->tieBreaker ? -1 : 1);
}

void Forest::setupTexturing()
{
   // Set up some texture stage info.
   TSShapeInstance::smRenderData.baseTE           = 0;
   TSShapeInstance::smRenderData.fogTE            = 1;
   TSShapeInstance::smRenderData.environmentMapTE = 2;
   
   gFogMethod = TSShapeInstance::FOG_MULTI_1;

   TSShapeInstance::smRenderData.fogMethod = -1; //TSShapeInstance::NO_FOG;
   TSShapeInstance::smRenderData.intraDetailLevel = 1.0f;
}

void Forest::initMaterials()
{
   S32 & fogMethod  = TSShapeInstance::smRenderData.fogMethod;
   S32 & baseTE     = TSShapeInstance::smRenderData.baseTE;
   S32 & fogTE      = TSShapeInstance::smRenderData.fogTE;
   TSShapeInstance::smRenderData.fadeSet = false;
   TSShapeInstance::smRenderData.vertexAlpha.init();

   // -------------------------------------------------
   //if (fogMethod == TSShapeInstance::FOG_MULTI_1)
   {
      // ---------------------------------
      // set up fog texure unit
      glActiveTextureARB(GL_TEXTURE0_ARB + fogTE);
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,   GL_COMBINE);
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB,    GL_INTERPOLATE);
      glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB,    GL_TEXTURE);
      glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB,   GL_SRC_COLOR);
      glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB,    GL_PREVIOUS);
      glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB,   GL_SRC_COLOR);
      glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB,   GL_SRC_ALPHA);
      glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA,  GL_REPLACE);
      glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA,  GL_PREVIOUS);
      glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);

      // the ATI Rage 128 needs a forthcoming driver to do do constant alpha blend
      if (TSShapeInstance::smRenderData.fogTexture)
         glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB,GL_TEXTURE);         
      else
         glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE2_RGB,GL_CONSTANT);

      // bind constant fog bitmap
      if(TSShapeInstance::smRenderData.fogHandle)
         glBindTexture(GL_TEXTURE_2D, TSShapeInstance::smRenderData.fogHandle->getGLName());

      glActiveTextureARB(GL_TEXTURE0_ARB + baseTE);
   }

   //----------------------------------
   // set up texture environment for base texture
   TSShapeInstance::smRenderData.materialFlags = 0;
   TSShapeInstance::smRenderData.materialIndex = TSDrawPrimitive::NoMaterial;

   // draw one-sided...
   glEnable(GL_CULL_FACE);
   glFrontFace(GL_CW);

   // enable vertex arrays...
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   // when blending we modulate and draw using src_alpha, 1-src_alpha...
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   // we modulate in order to apply lighting...
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   LightManager::sgSetupExposureRendering(); // <- potentially missing line
   // but we don't blend by default...
   glDisable(GL_BLEND);
   glDepthMask(GL_TRUE);

   // lighting?
   TSShapeInstance::smRenderData.lightingOn = glIsEnabled(GL_LIGHTING);

   // set vertex color
   TSShapeInstance::smRenderData.vertexAlpha.set();
   Point4F vertexColor(1,1,1,TSShapeInstance::smRenderData.vertexAlpha.current);
   glColor4fv(vertexColor);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,vertexColor);

   LightManager::sgResetExposureRendering();
   // this should be off by default, but we'll end up turning it on asap...
   glDisable(GL_TEXTURE_2D);
}

void Forest::renderNonTranslucentItems()
{
   PROFILE_START(forest_renderNonTranslucentItems);
   bool supportBuffers = dglDoesSupportVertexBuffer();

   for (S32 i=0; i<mNonTranslucentLists.size(); i++)
   {
      #ifdef FOREST_DEBUG
         if (Con::getBoolVariable("$Forest::hack1",false))
         {
            nonTranslucentLists[i].clear();
            continue;
         }
      #endif

      TSShapeInstance * si = smForestInstances[i];
      TSShapeInstance::ObjectInstance::smTransforms = si->mNodeTransforms.address();
      TSShapeInstance::smRenderData.intraDetailLevel = 1.0f;
      S32 bod = si->getShape()->details[0].objectDetailNum;
      S32 vb;

      if (supportBuffers && (si->getShape()->mVertexBuffer != -1 || si->fillVB()))
         vb = si->getShape()->mVertexBuffer;
      else
         vb = -1;

      for (S32 j=0; j<si->getShape()->subShapeFirstTranslucentObject[0]; j++)
      {
         for (S32 k=0; k<mNonTranslucentLists[i].size(); k++)
         {

            ForestItem * fi = mNonTranslucentLists[i][k];
            TSShapeInstance::smRenderData.detailLevel = fi->getTSDetail1();
            S32 dl = fi->getTSDetail1();
            
            if (dl < 0)
               continue;
            
            S32 od = si->getShape()->details[dl].objectDetailNum;
            TSShapeInstance::MeshObjectInstance * object = &si->mMeshObjects[j];
            TSMesh * mesh = object->getMesh(od);

            if(!mesh)
               continue;

            const Point3F & scale = fi->getObjectScale();
            S32 vboffset = (object->getMesh(bod) ? object->getMesh(bod)->vbOffset : -1);
            
            glPushMatrix();

            dglMultMatrix(&fi->getTransform());
            glScalef(scale.x,scale.y,scale.z);

            si->setAlphaAlways(1.0f);

            // fog
            U32 fm = TSShapeInstance::NO_FOG;
            if (fi->getFogged())
               fm = gFogMethod;

            //if (gFogMethod==TSShapeInstance::FOG_MULTI_1)
            {
               glActiveTextureARB(GL_TEXTURE0_ARB + TSShapeInstance::smRenderData.fogTE);

               if ((U32)TSShapeInstance::smRenderData.fogMethod != fm)
               {
                  if (fm == TSShapeInstance::NO_FOG)
                     glDisable(GL_TEXTURE_2D);
                  else
                     glEnable(GL_TEXTURE_2D);
               }

               if (fm == TSShapeInstance::FOG_MULTI_1)
               {
                  Point4F fog(TSShapeInstance::smRenderData.fogColor);
                  F32 fogAmount = fi->getFogAmount();

                  fog.w = fogAmount;
                  glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,fog);
  
                  // the ATI Rage 128 (amoung others) can't do constant alpha blend
                  if (TSShapeInstance::smRenderData.fogTexture)
                     if (TSShapeInstance::smRenderData.fogColor.w != fogAmount)
                     {
                        U8 *bits = TSShapeInstance::smRenderData.fogBitmap->getWritableBits();
                        U8 fog = 255 * fogAmount;
      
                        for (U8 i = 0; i < 64; ++i)
                        {
                           bits[3] = fog;
                           bits += 4;
                        }    
                        TSShapeInstance::smRenderData.fogHandle->refresh();
                     }
               }
               glActiveTextureARB(GL_TEXTURE0_ARB + TSShapeInstance::smRenderData.baseTE);
            }

            TSShapeInstance::smRenderData.fogColor.w = fi->getFogAmount();
            TSShapeInstance::smRenderData.fogMethod = fm;

            // render
            if (vb == -1)
            {  
               mesh->render(object->frame,object->matFrame,si->getMaterialList());
               if (fm == TSShapeInstance::FOG_TWO_PASS)
               { 
                  glDisable(GL_TEXTURE_2D);
                  glEnable(GL_BLEND);
                  
                  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                  glDisableClientState(GL_NORMAL_ARRAY);
                  
                  glColor4fv(TSShapeInstance::smRenderData.fogColor);
                  
                  mesh->renderFog(object->frame,si->getMaterialList());

                  Point4F vertexColor(1,1,1,TSShapeInstance::smRenderData.vertexAlpha.current);

                  glColor4fv(vertexColor);

                  glEnableClientState(GL_NORMAL_ARRAY);
                  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

                  glDisable(GL_BLEND);
                  glEnable(GL_TEXTURE_2D);
               }
            }
            else
            {
               if (mesh && mesh->vertsPerFrame)
               {                  
                  GLuint foffset = (object->frame*mesh->numMatFrames + object->matFrame)*mesh->vertsPerFrame;
                  
                  glSetVertexBufferEXT(vb);
                  glOffsetVertexBufferEXT(vb,vboffset + foffset);
                  mesh->renderVB(object->frame,object->matFrame,si->getMaterialList());
                  if (fm == TSShapeInstance::FOG_TWO_PASS)
                  { 
                     glDisable(GL_TEXTURE_2D);
                     glEnable(GL_BLEND);
                  
                     glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                     glDisableClientState(GL_NORMAL_ARRAY);
                  
                     glColor4fv(TSShapeInstance::smRenderData.fogColor);
                  
                     glOffsetVertexBufferEXT(vb,vboffset + foffset);
                     mesh->renderFog(object->frame,si->getMaterialList());

                     Point4F vertexColor(1,1,1,TSShapeInstance::smRenderData.vertexAlpha.current);

                     glColor4fv(vertexColor);

                     glEnableClientState(GL_NORMAL_ARRAY);
                     glEnableClientState(GL_TEXTURE_COORD_ARRAY);

                     glDisable(GL_BLEND);
                     glEnable(GL_TEXTURE_2D);
                  }
               }
            }

            smRenderCount++;

            glPopMatrix();            
         }
      }

      if (!(TSShapeInstance::smRenderData.materialIndex & TSDrawPrimitive::NoMaterial))
         TSShapeInstance::smRenderData.materialIndex = 99; // won't match any material...

      // clear out for next time
      #ifdef FOREST_DEBUG
         if (!Con::getBoolVariable("$Forest::hack3",false))
            mNonTranslucentLists[i].clear();
      #else
         mNonTranslucentLists[i].clear();
      #endif
   }

PROFILE_END();
}

inline void Forest::renderImage(SceneRenderImage* image)
{
   AssertFatal(image, "Forest::renderImage - got a NULL image!");
   AssertFatal(image->obj, "Forest::renderImage - got a NULL image object!");

   dglSetCanonicalState();
   image->obj->renderObject(smState, image);
}

inline void Forest::renderSpan(SimpleForestRenderList *list, F32 nearSq, F32 farSq)
{
   if(!list)
      return;

   PROFILE_START(forest_RenderSpan);

   S32 mode = 0; // 0 nothing, 1 mesh, 2 billboard

   // Used for tracking state changes.
   TSShapeInstance * prevShape = NULL;

   for (S32 j=0; j<list->size(); j++)
   {
      ForestItem * fi = (*list)[j];

      // Skip to next item, we're too far out.
      if(fi->getSquaredDistance() >= farSq)
         continue;

      // We have done all we can.
      if(fi->getSquaredDistance() < nearSq)
         break; 

      PROFILE_START(forest_renderSpan_RenderItem);

      TSShapeInstance * si = fi->getShapeInstance();

      if(si != prevShape)
      {
         if(mode==1)
            cleanMeshRender();
         if(mode==2)
            cleanBBRender();

         mode = 0;
         prevShape = si;
      }

      // Ascertain detail level.
      S32 dl1    = fi->getTSDetail1();
      F32 alpha1 = fi->getAlpha1();
      S32 dl2    = fi->getTSDetail2();
      F32 alpha2 = fi->getAlpha2();

      bool render1 = false, render2 = false;

      render1 = (dl1 >= 0 && alpha1 > 0.f);
      render2 = (dl2 >= 0 && alpha2 > 0.f);

      if(render1 && render2)
      {
         S32 ss1 = si->getShape()->details[dl1].subShapeNum;
         S32 ss2 = si->getShape()->details[dl2].subShapeNum;

         // Render first detail
         if(ss1 < 0)
         {
            if(mode == 1)   cleanMeshRender();
            if(mode != 2)   prepBBRender();
            mode = 2;

            doBBRender(fi, si, dl1, alpha1);
         }
         else
         {
            if(mode == 2)     cleanBBRender();
            if(mode != 1)     prepMeshRender();
            mode = 1;

            prevShape = si;
            doMeshRender(fi, si, dl1, alpha1);
         }

         // Render second detail
         if(ss2 < 0)
         {
            if(mode == 1)   cleanMeshRender();
            if(mode != 2)   prepBBRender();
            mode = 2;

            doBBRender(fi, si, dl2, alpha2);
         }
         else
         {
            if(mode == 2)     cleanBBRender();
            if(mode != 1)     prepMeshRender();
            mode = 1;

            doMeshRender(fi, si, dl2, alpha2);
         }
      }
      else
      {

         S32 dl; F32 alpha;
         if(render1)
         {
            dl    = dl1;
            alpha = alpha1;
         } 
         else if(render2)
         {
            dl    = dl2;
            alpha = alpha2;
         }
         else
         {
            PROFILE_END();
            continue;
         }

         const TSDetail * detail = &si->getShape()->details[dl];
         S32 ss  = detail->subShapeNum;

         if(ss < 0)
         {
            if(mode == 1)   cleanMeshRender();
            if(mode != 2)   prepBBRender();
            mode = 2;

            doBBRender(fi, si, dl, alpha);
         }
         else
         {
            if(mode == 2)     cleanBBRender();
            if(mode != 1)     prepMeshRender();
            mode = 1;

            doMeshRender(fi, si, dl, alpha);
         }

      }
      smRenderCount++;

      PROFILE_END();
   }

   if(mode == 1)
      cleanMeshRender();

   if(mode == 2)
      cleanBBRender();

   PROFILE_END();
}

inline void Forest::renderSpan(SimpleForestRenderList *list)
{
   if(!list)
      return;

   renderSpan(list, 0, (*list)[0]->getSquaredDistance()+1.f);

   return; 
}

void Forest::processNodeSpans(SceneState::RenderBSPNode& rNode)
{
//   Con::printf("  node %x", &rNode);
   if (rNode.frontIndex != 0xFFFF)
   {
      F32 planeDist = rNode.plane.distToPlane(smState->mCamPosition);

      if (planeDist >= 0)
      {
         processNodeSpans(smState->mTranslucentBSP[rNode.backIndex]);

         if (rNode.rimage != NULL)
            renderImage(rNode.rimage);

         renderSpan((SimpleForestRenderList*)rNode.userData);

         processNodeSpans(smState->mTranslucentBSP[rNode.frontIndex]);
      }
      else
      {
         processNodeSpans(smState->mTranslucentBSP[rNode.frontIndex]);

         if (rNode.rimage != NULL)
            renderImage(rNode.rimage);

         renderSpan((SimpleForestRenderList*)rNode.userData);

         processNodeSpans(smState->mTranslucentBSP[rNode.backIndex]);
      }

      // Mark the list as empty, just in case.
      if(rNode.userData)
        ((SimpleForestRenderList*)rNode.userData)->clear();
   }
   else
   {
      static Vector<SceneRenderImage*> imageList(128);
      imageList.clear();

      SceneRenderImage* pImage = rNode.riList;
      SimpleForestRenderList *list = (SimpleForestRenderList *)rNode.userData;

      F32 imageDistSqr = 0.f;
      while (pImage != NULL)
      {
         // Calculate distance!
         AssertFatal(pImage->sortType == SceneRenderImage::Point, "Bad sort type!");
         pImage->pointDistSq = (smState->mCamPosition - pImage->poly[0]).lenSquared();
         imageList.push_back(pImage);

         imageDistSqr = getMax(pImage->pointDistSq, imageDistSqr);

         pImage = pImage->pNext;
      }

      // Sort distance.
      dQsort(imageList.address(), imageList.size(), sizeof(SceneRenderImage*), cmpPointImageFunc);

      // Track the maximum distance of the point thingummy.      
      F32 lastDistSq = (list ? imageDistSqr : F32_MAX);
      if(list && list->size())
      {
         // Get the biggest dist, first item is furthest out...
         lastDistSq = getMax(lastDistSq, (smState->mCamPosition - (*list)[0]->getPosition()).lenSquared());
      }

      // Now render from back to front.
      for (U32 i = 0; i < imageList.size(); i++)
      {
         // Render span that happens further out than this image...
         if(list)
            renderSpan(list, imageList[i]->pointDistSq, lastDistSq);

         // Render the image...
         renderImage(imageList[i]);

         // Update last dist.
         AssertFatal(lastDistSq >= imageList[i]->pointDistSq, "Invalid ordering!");
         lastDistSq = imageList[i]->pointDistSq;
      }

      // Make sure we render any remaining stuff.
      if(list)
      {
         renderSpan(list, 0, lastDistSq);

         // Mark the list as empty, just in case.
         list->clear();
      }

      imageList.clear();
   }
}

void Forest::buildClippingPlanes(MatrixF camToObj, bool flipClipPlanes)
{
   F64 frustumParam[6];
   dglGetFrustum(&frustumParam[0], &frustumParam[1],
                 &frustumParam[2], &frustumParam[3],
                 &frustumParam[4], &frustumParam[5]);

   Point3F osCamPoint(0, 0, 0);
   camToObj.mulP(osCamPoint);
   sgComputeOSFrustumPlanes(frustumParam,
                            camToObj,
                            osCamPoint,
                            mClipPlane[4],
                            mClipPlane[0],
                            mClipPlane[1],
                            mClipPlane[2],
                            mClipPlane[3]);
   mNumClipPlanes = 4;

   if (flipClipPlanes) 
   {
      mClipPlane[0].neg();
      mClipPlane[1].neg();
      mClipPlane[2].neg();
      mClipPlane[3].neg();
      mClipPlane[4].neg();
      mClipPlane[5].neg();
   }
}

S32 Forest::testSquareVisibility(Point3F &min, Point3F &max, S32 mask, F32 expand)
{
   S32 retMask = 0;
   Point3F minPoint, maxPoint;
   for(S32 i = 0; i < mNumClipPlanes; i++)
   {
      if(mask & (1 << i))
      {
         if(mClipPlane[i].x > 0)
         {
            maxPoint.x = max.x;
            minPoint.x = min.x;
         }
         else
         {
            maxPoint.x = min.x;
            minPoint.x = max.x;
         }
         if(mClipPlane[i].y > 0)
         {
            maxPoint.y = max.y;
            minPoint.y = min.y;
         }
         else
         {
            maxPoint.y = min.y;
            minPoint.y = max.y;
         }
         if(mClipPlane[i].z > 0)
         {
            maxPoint.z = max.z;
            minPoint.z = min.z;
         }
         else
         {
            maxPoint.z = min.z;
            minPoint.z = max.z;
         }
         F32 maxDot = mDot(maxPoint, mClipPlane[i]);
         F32 minDot = mDot(minPoint, mClipPlane[i]);
         F32 planeD = mClipPlane[i].d;
         if(maxDot <= -(planeD + expand))
            return -1;
         if(minDot <= -planeD)
            retMask |= (1 << i);
      }
   }
   return retMask;
}

void Forest::renderObject(SceneState* state, SceneRenderImage* image)
{
   TerrainBlock * terrain  = GlobDock::getTerrain(isServerObject());
   if (!terrain || mForestItems.empty()) // if no forest items, forest empty or hasn't been created yet...
      return;

   PROFILE_START(forest_renderObject);

   U32 watermark = FrameAllocator::getWaterMark();

   smState = state;

   // get a couple variables from the terrain      
   F32 squareSize = terrain->getSquareSize();
   F32 blockSize  = squareSize * TerrainBlock::BlockSquareWidth;

   // set field of view info if it has changed...would be nice to have all camera info
   // in one place, but game just hasn't been set up that way...so we get left,right,far, near from gl,
   // camera matrix from scene state (could get from gl too) and fov from gGame.  Oh well...
   if (!smViewData.init || 
      smViewData.fov != GameGetCameraFov() || 
      terrain        != smViewData.terrain || 
      this           != smViewData.forest || 
      mFabs(state->getVisibleDistance()-smViewData.visDist)>0.5f
      )
      setViewData(GameGetCameraFov(),state->getFogDistance(),state->getVisibleDistance());

   smRenderCount = 0;

   Point3F offset = terrain->getTransform().getPosition();

   // Set up viewport...
   RectI viewport;
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   dglGetViewport(&viewport);

   if (state->isTerrainOverridden() == false)
      state->setupObjectProjection(this);
   else
      state->setupBaseProjection();

   glMatrixMode(GL_MODELVIEW);

   // Set up our clipping information...
   MatrixF xfrm;
   dglGetModelview(&xfrm);
   xfrm.inverse();
   buildClippingPlanes(xfrm, state->mFlipCull);

   Point3F cameraPos;
   xfrm.getColumn(3, &cameraPos);

   // Figure out the potential view area...
   S32 xStart = (S32)mFloor( (cameraPos.x - smViewData.visFarDist + offset.x) / blockSize );
   S32 xEnd   = (S32)mCeil ( (cameraPos.x + smViewData.visFarDist + offset.x) / blockSize );
   S32 yStart = (S32)mFloor( (cameraPos.y - smViewData.visFarDist + offset.y) / blockSize );
   S32 yEnd   = (S32)mCeil ( (cameraPos.y + smViewData.visFarDist + offset.y) / blockSize );

   if(mSingleBlock)
   {
      // Let's just hack this up!
      xStart = 0;
      yStart = 0;

      xEnd = 1;
      yEnd = 1;
   }

   const S32 xExt = (S32)(xEnd - xStart);
   const S32 yExt = (S32)(yEnd - yStart);

   // Check to see if we should be doing occlusion checking...
   Point3F localCamPos = cameraPos;
   terrain->getWorldTransform().mulP(localCamPos);

   F32 height;
   terrain->getHeight(Point2F(localCamPos.x, localCamPos.y), &height);
   
   bool aboveTerrain = (height <= localCamPos.z);

   PROFILE_START(forest_quadtree);

   // Con::printf("-----------------------");
   #ifdef FOREST_DEBUG
      if (!treesSaved)
   #endif
   for(S32 xCurrentOffset = xStart; xCurrentOffset <= xEnd; xCurrentOffset++)
   {
      for(S32 yCurrentOffset = yStart; yCurrentOffset <= yEnd; yCurrentOffset++)
      {
         F32 xOffset = xCurrentOffset * blockSize - offset.x;
         F32 yOffset = yCurrentOffset * blockSize - offset.y;

         // Recursively process the block...
         static SquareStackNode stack[TerrainBlock::BlockShift*4];
         Point3F minPoint, maxPoint;

         stack[0].level = TerrainBlock::BlockShift;
         stack[0].clipFlags = ((1 << mNumClipPlanes) - 1) | FarSphereMask;  // test all the planes
         stack[0].pos.set(0,0);

         S32 curStackSize = 1;
         F32 squareDistance;

         while(curStackSize)
         {
            SquareStackNode *n = stack + curStackSize - 1;

            // see if it's visible
            GridSquare *sq = terrain->findSquare(n->level, n->pos);

            minPoint.set(squareSize * n->pos.x + xOffset,
                         squareSize * n->pos.y + yOffset,
                         fixedToFloat(sq->minHeight));
            maxPoint.set(minPoint.x + squareSize * (1 << n->level),
                         minPoint.y + squareSize * (1 << n->level),
                         fixedToFloat(sq->maxHeight) + ForestItemData::smMaxItemHeight);

            // draw a line to this cube from the camera
            F32 zDiff;
            squareDistance = getSquareDistance(cameraPos, minPoint, maxPoint, &zDiff);

            S32 nextClipFlags = 0;

            if(n->clipFlags)
            {
               if(n->clipFlags & FarSphereMask)
               {
                  if(squareDistance >= smViewData.visFarDist)
                  {
                     curStackSize--;
                     continue;
                  }

                  S32 squareSz = squareSize * (1 << n->level);
                  if(squareDistance + maxPoint.z - minPoint.z + squareSz + squareSz > smViewData.visFarDist)
                     nextClipFlags |= FarSphereMask;
               }

               nextClipFlags |= testSquareVisibility(minPoint, maxPoint, n->clipFlags, squareSize);
               if(nextClipFlags == -1)
               {
                  curStackSize--;
                  continue;
               }
            }

            // See if we need to do an occlusion check.
            if(n->level >= mMinOcclusionLevel && n->level <= mMaxOcclusionLevel)
            {
               PROFILE_START(forest_occlusion);
               if(terrCheck(terrain, Box3F(minPoint, maxPoint), localCamPos, height, aboveTerrain))
               {
                  PROFILE_END();
                  curStackSize--;
                  continue;
               }
               PROFILE_END();
            }

            // Bottom out at level 1 and just do pure distance culling from there in... not as accurate
            // but we save several thousand recursions!
            if(n->level == 1)
            {
               PROFILE_START(forest_processSquare);

               // Get the top left corner we're looking at.
               ForestGrid *fg = &mForestGrid[( n->pos.x & TerrainBlock::BlockMask) + ((n->pos.y & TerrainBlock::BlockMask) << TerrainBlock::BlockShift)];

               static U32 offsets[]  = {0, 1, TerrainBlock::BlockSize, TerrainBlock::BlockSize + 1};
               static U32 offsetsX[] = {0, 1, 0, 1};
               static U32 offsetsY[] = {0, 0, 1, 1};

               // Now check the things at each offset.
               for(S32 i=0; i<4; i++)
               {
                  // Hurray for pointer arithmetic.
                  ForestGrid *realFg = fg + offsets[i];

                  if(realFg->count)
                  {
                     F32 xPos = xOffset + (n->pos.x + offsetsX[i]) * squareSize;
                     F32 yPos = yOffset + (n->pos.y + offsetsY[i]) * squareSize;
                     Point3F wPos(xPos, yPos, minPoint.z);

                     F32 midDist = (wPos - cameraPos).len();

                     processSquare(midDist, *realFg, cameraPos, xOffset + blockSize/2, yOffset + blockSize/2);
                  }
               }

               PROFILE_END();

               curStackSize--;
               continue;
            }
   
            // subdivide this square and throw it on the stack
            S32 squareOneSize  = 1 << n->level;
            S32 squareHalfSize = squareOneSize >> 1;
            S32 nextLevel      = n->level - 1;
            Point2I pos        = n->pos;

            n->level = nextLevel;
            n->clipFlags = nextClipFlags;

            for(S32 i = 1; i < 4; i++)
            {
               n[i].level     = nextLevel;
               n[i].clipFlags = nextClipFlags;
            }

            // push in reverse order of processing.
            n[3].pos = pos;
            n[2].pos.set(pos.x + squareHalfSize,   pos.y);
            n[1].pos.set(pos.x,                    pos.y + squareHalfSize);
            n[0].pos.set(pos.x + squareHalfSize,   pos.y + squareHalfSize);

            curStackSize += 3;
         }
      }
   }

   PROFILE_END();

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   dglSetViewport(viewport);

   #ifdef FOREST_DEBUG
   renderDebug();
   #endif

   // render non-translucent forest items first...
   glMatrixMode(GL_MODELVIEW);
   glEnable(GL_BLEND);
   glEnable(GL_LIGHTING);

   installLights();

   setupTexturing();
   initMaterials();

   renderNonTranslucentItems();

   uninstallLights();
   dglSetCanonicalState();

   // Do a proper BSP insertion sort of all the transparent trees.

   // First, clear out our pointers...
   for(S32 i=0; i < state->mTranslucentBSP.size(); i++)
      state->mTranslucentBSP[i].userData = NULL;

   // First, process the bins (from back to front)
   for(S32 i= smViewData.numBins - 1; i >= 0; i--)
   {
      PROFILE_START(forest_bin);
      ForestRenderList &crl = mTranslucentLists[i];

      // Presort the list.
      PROFILE_START(forest_binSort);
      dQsort(crl.mItems.address(), crl.mItems.size(),sizeof(ForestItem*),Forest::compareDistanceFunc);
      PROFILE_END();


      // Prep the box for the crl.
      const Box3F& rObjBox = crl.mBounds;

      Point3F center;
      rObjBox.getCenter(&center);

      const Point3F xRad((rObjBox.max.x - rObjBox.min.x) * 0.5, 0, 0);
      const Point3F yRad(0, (rObjBox.max.y - rObjBox.min.y) * 0.5 , 0);
      const Point3F zRad(0, 0, (rObjBox.max.z - rObjBox.min.z) * 0.5);

      // Classify the bin....
      SceneState::RenderBSPNode* pNode = &state->mTranslucentBSP[0];
      while (pNode)
      {
         if (pNode->frontIndex != 0xFFFF)
         {
            // Classify against this plane.
            PlaneF::Side s = pNode->plane.whichSideBox(center, xRad, yRad, zRad, Point3F(0,0,0));

            switch(s)
            {
            case PlaneF::Back:
               pNode = &state->mTranslucentBSP[pNode->backIndex];
               break;
            case PlaneF::Front:
               pNode = &state->mTranslucentBSP[pNode->frontIndex];
               break;
            case PlaneF::On:
               // On is an ugly but unfortunately likely case.
               // We sit down and sort each point and insert where appropriate.

               // Sort the list first.
               PROFILE_START(forest_addItems);
               for(S32 i=0; i<crl.mItems.size(); i++)
                  recursiveAddTree(pNode, crl.mItems[i]);
               PROFILE_END();

               // All done.
               pNode = NULL;
               break;
            }
         }
         else
         {
            PROFILE_START(forest_addList);
            // We actually got all the way on one side of the plane or the other,
            // good deal. So let's stuff the whole list in, quick-like.
            if(!pNode->userData)
            {
               SimpleForestRenderList *list = (SimpleForestRenderList*)FrameAllocator::alloc(sizeof(SimpleForestRenderList));
               constructInPlace(list);
               pNode->userData = (void*)list;

               // Copy the presorted list
               list->merge(crl.mItems);
            }
            else
            {
               // Already have a list, add our stuff to it...
               SimpleForestRenderList *list = (SimpleForestRenderList *)pNode->userData;

               // Append our list.
               list->merge(crl.mItems);
            }

            // All done.
            pNode = NULL;
            PROFILE_END();
         }
      }
      PROFILE_END();
   }

   // Ok, now let's render the scenegraph!
   PROFILE_START(forest_processNodeSpans);
   processNodeSpans(state->mTranslucentBSP[0]);
   PROFILE_END();

   for(S32 i=0; i < state->mTranslucentBSP.size(); i++)
   {
      if(state->mTranslucentBSP[i].userData)
      {
         ((SimpleForestRenderList*)state->mTranslucentBSP[i].userData)->clear();
         destructInPlace((SimpleForestRenderList*)state->mTranslucentBSP[i].userData);
      }
      state->mTranslucentBSP[i].userData = NULL;
   }

   // clean up the bins.
   PROFILE_START(forest_binEmpty);
   for(S32 i= smViewData.numBins - 1; i >= 0; i--)
   {
      ForestRenderList &crl = mTranslucentLists[i];
      crl.clear();
   }
   PROFILE_END();

   // OK, now wipe what was there...
   state->mTranslucentBSP[0].frontIndex = 0xFFFF;
   state->mTranslucentBSP[0].backIndex = 0xFFFF;
   state->mTranslucentBSP[0].riList = NULL;
   state->mTranslucentBSP[0].rimage = NULL;

   Con::setIntVariable("$Forest::treeCount", smRenderCount);

   #ifdef FOREST_DEBUG
      treesSaved = Con::getBoolVariable("$Forest::hack3",false);
   #endif

   dglSetCanonicalState();
   glActiveTextureARB(GL_TEXTURE0_ARB);
   glDisable(GL_TEXTURE_2D);
   glActiveTextureARB(GL_TEXTURE1_ARB);
   glDisable(GL_TEXTURE_2D);
   glActiveTextureARB(GL_TEXTURE2_ARB);
   glDisable(GL_TEXTURE_2D);
   glActiveTextureARB(GL_TEXTURE0_ARB);

   FrameAllocator::setWaterMark(watermark);

   PROFILE_END();
}

void Forest::renderDebug()
{
#ifdef FOREST_DEBUG
   if (!Con::getBoolVariable("$Forest::showRadius",false))
      return;

   for (S32 i=0; i<nonTranslucentLists.size(); i++)
   {
      for (S32 j=0; j<nonTranslucentLists[i].size(); j++)
      {
         ForestItem * fi = nonTranslucentLists[i][j];
         renderDebugRadius(fi);
      }
   }

   // go through the translucent list...but worry about re-draw
   for (S32 i=smViewData.numBins-1; i>=0; i--)
   {
      ForestRenderList & list = translucentLists[i];

      for (S32 j=0; j<list.size(); j++)
      {
         ForestItem * fi = list[j];

         // determine whether translucent and non-translucent parts were seperated on this shape
         // if they were, then we already did the debug render above, and we can continue...
         TSShapeInstance * shapeInstance = fi->getShapeInstance();
         S32 tsDL1 = fi->getTSDetail1();
         S32 tsDL2 = fi->getTSDetail1();
         F32 alpha1 = fi->getAlpha1();
         F32 alpha2 = fi->getAlpha2();
         bool isBillboard = (tsDL1>=0 && alpha1>0.0f && shapeInstance->getShape()->details[tsDL1].subShapeNum<0) ||
                            (tsDL2>=0 && alpha2>0.0f && shapeInstance->getShape()->details[tsDL2].subShapeNum<0);
         bool fading = (tsDL1>=0 && alpha1>0.0f && alpha1<1.0f) || (tsDL2>=0 && alpha2<1.0f && alpha2>0.0f);
         S32 firstTObject = shapeInstance->getShape()->subShapeFirstTranslucentObject[0];
         bool separateTranslucency = !isBillboard && firstTObject>0 && !fading;
         if (separateTranslucency)
            continue;

         // not in non-translucent lists, render away...
         renderDebugRadius(fi);
      }
   }
#endif
}

void Forest::renderDebugRadius(ForestItem * fi)
{
   fi;
#ifdef FOREST_DEBUG
   if (fi->collidable())
   {
      F32 radius = fi->getRadius();
      Point3F pos = fi->getCurrentPos();
      F32 zPos   = pos.z + 1.0f; 

      glBegin(GL_LINE_LOOP);
      glLineWidth(10.0f);
      glColor3f(1.0f, 0.0f, 0.0f);
      glVertex3f(pos.x + radius, pos.y, zPos);
      glVertex3f(pos.x + radius*mCos(mDegToRad(45.0f)), pos.y + radius*mSin(mDegToRad(45.0f)), zPos);
      glVertex3f(pos.x, pos.y + radius, zPos);
      glVertex3f(pos.x - radius*mCos(mDegToRad(45.0f)), pos.y + radius*mSin(mDegToRad(45.0f)), zPos);
      glVertex3f(pos.x - radius, pos.y, zPos);
      glVertex3f(pos.x - radius*mCos(mDegToRad(45.0f)), pos.y - radius*mSin(mDegToRad(45.0f)), zPos);
      glVertex3f(pos.x, pos.y - radius, zPos);
      glVertex3f(pos.x + radius*mCos(mDegToRad(45.0f)), pos.y - radius*mSin(mDegToRad(45.0f)), zPos);
      glVertex3f(pos.x + radius, pos.y, zPos);
      zPos = pos.z + fi->getHeight();

      glBegin(GL_LINE_LOOP);
      glLineWidth(10.0f);
      glColor3f(1.0f, 0.0f, 0.0f);
      glVertex3f(pos.x + radius, pos.y, zPos);
      glVertex3f(pos.x + radius*mCos(mDegToRad(45.0f)), pos.y + radius*mSin(mDegToRad(45.0f)), zPos);
      glVertex3f(pos.x, pos.y + radius, zPos);
      glVertex3f(pos.x - radius*mCos(mDegToRad(45.0f)), pos.y + radius*mSin(mDegToRad(45.0f)), zPos);
      glVertex3f(pos.x - radius, pos.y, zPos);
      glVertex3f(pos.x - radius*mCos(mDegToRad(45.0f)), pos.y - radius*mSin(mDegToRad(45.0f)), zPos);
      glVertex3f(pos.x, pos.y - radius, zPos);
      glVertex3f(pos.x + radius*mCos(mDegToRad(45.0f)), pos.y - radius*mSin(mDegToRad(45.0f)), zPos);
      glVertex3f(pos.x + radius, pos.y, zPos);

      glEnd();
   }
#endif
}
