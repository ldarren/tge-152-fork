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

#include "forest/globDock.h"
#include "forest/forestClearing.h"
#include "forest/forestItem.h"
#include "forest/forest.h"

void Forest::prepMeshRender()
{
   initMaterials();
   setupTexturing();

   TSShapeInstance::smRenderData.materialIndex = -1;
   TSShapeInstance::smRenderData.fogMethod = -1;

   installLights();

   // Somehow we get into a non-modelview state. WTF!
   glMatrixMode(GL_MODELVIEW);
}

void Forest::doMeshRender(ForestItem *fi, TSShapeInstance *si, S32 dl, F32 alpha)
{
   const TSDetail * detail = &si->getShape()->details[dl];
   const S32 ss   = detail->subShapeNum;
   const S32 bod  = si->getShape()->details[0].objectDetailNum;
   const S32 od   = detail->objectDetailNum;
   const F32 aa   = si->getAlphaAlwaysValue();
   const bool sepTrans  = fi->getSeparateTranslucency();
   bool allowVBs  = false;
   S32  vb;

   alpha = mClampF(alpha, 0.f, 1.f);

   // Check that we can do advanced rendering, and fill the buffer if we need to.
   if (dglDoesSupportARBMultitexture() && dglDoesSupportVertexBuffer() &&
      (si->getShape()->mVertexBuffer != -1 || si->fillVB()))
   {
      vb = si->getShape()->mVertexBuffer;
      allowVBs = true;
   }
   else
      vb = -1;

   si->setEnvironmentMap(smState->getEnvironmentMap());
   si->setEnvironmentMapOn(true,1);
   
   // fog
   U32 fm = TSShapeInstance::NO_FOG;
   if (fi->getFogged())
      fm = TSShapeInstance::FOG_MULTI_1;

   {
      glActiveTextureARB(GL_TEXTURE0_ARB + TSShapeInstance::smRenderData.fogTE);

      if ((U32)TSShapeInstance::smRenderData.fogMethod != fm)
      {
         if (fm == TSShapeInstance::NO_FOG)
            glDisable(GL_TEXTURE_2D);
         else
            glEnable(GL_TEXTURE_2D);
      }

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

      glActiveTextureARB(GL_TEXTURE0_ARB + TSShapeInstance::smRenderData.baseTE);
   }

   TSShapeInstance::smRenderData.fogColor.w = fi->getFogAmount();
   TSShapeInstance::smRenderData.fogMethod = fm;

   // set up render data   
   TSShapeInstance::smRenderData.detailLevel = dl;
   TSShapeInstance::ObjectInstance::smTransforms = si->mNodeTransforms.address();
   const TSShape * shape = si->getShape();

   // run through the meshes
   TSShapeInstance::smRenderData.currentTransform = NULL;
   S32 start = sepTrans ? shape->subShapeFirstTranslucentObject[ss] : shape->subShapeFirstObject[ss];
   S32 end   = shape->subShapeFirstObject[ss] + shape->subShapeNumObjects[ss];

   // but we don't blend by default...
   glDepthMask(GL_FALSE);

   const Point3F &scale = fi->getObjectScale();

   TSShapeInstance::smRenderData.currentTransform = NULL;

   glPushMatrix();
   dglMultMatrix(&fi->getTransform());
   glScalef(scale.x,scale.y,scale.z);

   PROFILE_START(forest_renderMesh);
   if (vb == -1)
   {
      for (S32 i=start; i<end; i++)
      {
         si->mMeshObjects[i].visible = alpha;
         si->mMeshObjects[i].render(od,si->getMaterialList());
         si->mMeshObjects[i].visible = 1.f;
      }
   }
   else
   {
      for (S32 i=start; i<end; i++)
      {
         si->mMeshObjects[i].visible = alpha;
         si->mMeshObjects[i].renderVB(vb,od,si->getMaterialList());
         si->mMeshObjects[i].visible = 1.f;
      }
   }
   PROFILE_END();

   // if we have a matrix pushed, pop it now
   if (TSShapeInstance::smRenderData.currentTransform)
   {
      glPopMatrix();
      TSShapeInstance::smRenderData.currentTransform = NULL;
   }

   glPopMatrix();
}

void Forest::cleanMeshRender()
{
   // Make sure we're doing depth tests.
   glDepthMask(true);
   uninstallLights();
}