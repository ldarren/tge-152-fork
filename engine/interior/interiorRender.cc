//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "interior/interior.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "lightingSystem/sgLightManager.h"

#include "dgl/dgl.h"
#include "dgl/gBitmap.h"
#include "dgl/gTexManager.h"
#include "math/mMatrix.h"
#include "math/mRect.h"
#include "math/mathUtils.h"
#include "dgl/materialList.h"
#include "dgl/materialPropertyMap.h"
#include "interior/interiorSubObject.h"
#include "interior/interiorInstance.h"
#include "core/bitVector.h"
#include "dgl/stripCache.h"
#include "platform/profiler.h"

//!!!!!!!TBD -- there should be a platform fn called memMove!
#include <string.h>


extern bool sgFogActive;
extern U16* sgActivePolyList;
extern U32  sgActivePolyListSize;
extern U16* sgFogPolyList;
extern U32  sgFogPolyListSize;
extern U16* sgEnvironPolyList;
extern U32  sgEnvironPolyListSize;

Point3F sgOSCamPosition;


#include "interior/itf.h" // render data structs and processing fns.

OutputPoint sgRenderBuffer[512];
U32         sgRenderIndices[2048];
U32         csgNumAllowedPoints = 256;

extern "C" {
   F32   texGen0[8];
   F32   texGen1[8];
   Point2F *fogCoordinatePointer;
}

void emitPrimitive(OutputPoint*                  renderBuffer,
                   U32*                          renderIndices,
                   const U32*                    winding,
                   const U32                     numPoints,
                   const U32                     offset,
                   ItrPaddedPoint*               srcPoints)
{
   U32 currIndex = 0;
   U32 last = 2;
   while (last < numPoints)
   {
      // First
      renderIndices[currIndex++] = offset + last - 2;
      renderIndices[currIndex++] = offset + last - 1;
      renderIndices[currIndex++] = offset + last - 0;
      last++;

      if (last == numPoints)
         break;

      // Second
      renderIndices[currIndex++] = offset + last - 1;
      renderIndices[currIndex++] = offset + last - 2;
      renderIndices[currIndex++] = offset + last - 0;
      last++;
   }

   processTriFan(renderBuffer,
                 srcPoints,
                 winding,
                 numPoints);
}

void emitPrimitiveFC_VB(OutputPointFC_VB*	renderBuffer,
                        U32*					renderIndices,
                        const U32*			winding,
                        const U32			numPoints,
                        const U32			offset,
                        ItrPaddedPoint*	srcPoints)
{
   U32 currIndex = 0;
   U32 last = 2;
   while (last < numPoints)
   {
      // First
      renderIndices[currIndex++] = offset + last - 2;
      renderIndices[currIndex++] = offset + last - 1;
      renderIndices[currIndex++] = offset + last - 0;
      last++;

      if (last == numPoints)
         break;

      // Second
      renderIndices[currIndex++] = offset + last - 1;
      renderIndices[currIndex++] = offset + last - 2;
      renderIndices[currIndex++] = offset + last - 0;
      last++;
   }

   processTriFanFC_VB(renderBuffer,
                      srcPoints,
                      winding,
                      numPoints);
}

void emitPrimitiveSP(OutputPoint*                  renderBuffer,
                     U32*                          renderIndices,
                     const U32*                    winding,
                     const U32                     numPoints,
                     const U32                     offset,
                     ItrPaddedPoint*               srcPoints,
                     const ColorI*                 srcColors)
{
   U32 currIndex = 0;
   U32 last = 2;
   while (last < numPoints)
   {
      // First
      renderIndices[currIndex++] = offset + last - 2;
      renderIndices[currIndex++] = offset + last - 1;
      renderIndices[currIndex++] = offset + last - 0;
      last++;

      if (last == numPoints)
         break;

      // Second
      renderIndices[currIndex++] = offset + last - 1;
      renderIndices[currIndex++] = offset + last - 2;
      renderIndices[currIndex++] = offset + last - 0;
      last++;
   }

   processTriFanSP(renderBuffer,
                   srcPoints,
                   winding, numPoints,
                   srcColors);
}


void emitPrimitiveVC_TF(OutputPoint*                  renderBuffer,
                        U32*                          renderIndices,
                        const U32*                    winding,
                        const U32                     numPoints,
                        const U32                     offset,
                        ItrPaddedPoint*               srcPoints,
                        const ColorI*                 srcColors)
{
   U32 currIndex = 0;
   U32 last = 2;
   while (last < numPoints)
   {
      // First
      renderIndices[currIndex++] = offset + last - 2;
      renderIndices[currIndex++] = offset + last - 1;
      renderIndices[currIndex++] = offset + last - 0;
      last++;

      if (last == numPoints)
         break;

      // Second
      renderIndices[currIndex++] = offset + last - 1;
      renderIndices[currIndex++] = offset + last - 2;
      renderIndices[currIndex++] = offset + last - 0;
      last++;
   }

   processTriFanVC_TF(renderBuffer,
                      srcPoints,
                      winding,
                      numPoints,
                      srcColors);
}

void emitPrimitiveSP_FC(OutputPoint*                  renderBuffer,
                        U32*                          renderIndices,
                        const U32*                    winding,
                        const U32                     numPoints,
                        const U32                     offset,
                        ItrPaddedPoint*               srcPoints,
                        const ColorI*                 srcColors)
{
   U32 currIndex = 0;
   U32 last = 2;
   while (last < numPoints)
   {
      // First
      renderIndices[currIndex++] = offset + last - 2;
      renderIndices[currIndex++] = offset + last - 1;
      renderIndices[currIndex++] = offset + last - 0;
      last++;

      if (last == numPoints)
         break;

      // Second
      renderIndices[currIndex++] = offset + last - 1;
      renderIndices[currIndex++] = offset + last - 2;
      renderIndices[currIndex++] = offset + last - 0;
      last++;
   }

   processTriFanSP_FC(renderBuffer,
                      srcPoints,
                      winding, numPoints,
                      srcColors);
}

void emitPrimitiveSP_FC_VB(OutputPointSP_FC_VB*	renderBuffer,
                           U32*			renderIndices,
                           const U32*		winding,
                           const U32		numPoints,
                           const U32		offset,
                           ItrPaddedPoint*	srcPoints,
                           const ColorI*        srcColors)
{
   U32 currIndex = 0;
   U32 last = 2;
   while (last < numPoints)
   {
      // First
      renderIndices[currIndex++] = offset + last - 2;
      renderIndices[currIndex++] = offset + last - 1;
      renderIndices[currIndex++] = offset + last - 0;
      last++;

      if (last == numPoints)
         break;

      // Second
      renderIndices[currIndex++] = offset + last - 1;
      renderIndices[currIndex++] = offset + last - 2;
      renderIndices[currIndex++] = offset + last - 0;
      last++;
   }

   processTriFanSP_FC_VB(renderBuffer,
                         srcPoints,
                         winding,
                         numPoints,
                         srcColors);
}

void flushPrimitives(const U32* indices,
                     const U32  count,
                     const U32  vcount)
{
   if (count == 0)
      return;

   if (Interior::smLockArrays && dglDoesSupportCompiledVertexArray())
   {
      glLockArraysEXT(0, vcount);
      glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, indices);
      glUnlockArraysEXT();
   } else {
      glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, indices);
   }
}

void flushPrimitivesVB(const U32* indices,
                       const U32  count,
                       const U32  vcount,
                       const S32  handle)
{
   if (count == 0)
      return;

   glSetVertexBufferEXT(handle);

   if (Interior::smLockArrays && dglDoesSupportCompiledVertexArray())
   {
      glLockArraysEXT(0, vcount);
      glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, indices);
      glUnlockArraysEXT();
   } else {
      glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, indices);
   }
}

//} // namespace {}

//--------------------------------------------------------------------------
void Interior::setupFog(SceneState* state)
{
   if (sgFogActive)
   {
      if (useFogCoord())
      {
         glEnable(GL_FOG);
         glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);
         GLfloat fogColor[4];
         fogColor[0] = state->getFogColor().red;
         fogColor[1] = state->getFogColor().green;
         fogColor[2] = state->getFogColor().blue;
         fogColor[3] = 1.0f;
         glFogfv(GL_FOG_COLOR, fogColor);
         glFogi(GL_FOG_MODE, GL_LINEAR);
         glFogf(GL_FOG_START, 0.0f);
         glFogf(GL_FOG_END, 1.0f);
      }
   }
}

void Interior::clearFog()
{
   if (sgFogActive)
   {
      if (useFogCoord())
      {
         glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FRAGMENT_DEPTH_EXT);
         glDisable(GL_FOG);
      }
   }
}

void Interior::setOSCamPosition(const Point3F& pos)
{
   sgOSCamPosition = pos;
}


//------------------------------------------------------------------------------
void Interior::render(const bool useAlarmLighting, MaterialList* pMaterials, const LM_HANDLE instanceHandle,
                      const Vector<ColorI>* normalVLights,
                      const Vector<ColorI>* alarmVLights)
{
   if (smRenderMode != 0)
   {
      PROFILE_START(IRO_DebugRender);
      debugRender(pMaterials, instanceHandle);
      PROFILE_END();
      return;
   }

   if (dglDoesSupportARBMultitexture())
   {
      if (smUseVertexLighting == false)
      {
         if (useFogCoord())
         {
            PROFILE_START(IRO_RenderARB_FC);
            renderARB_FC(useAlarmLighting, pMaterials, instanceHandle);
            PROFILE_END();
         }
         else
         {
            PROFILE_START(IRO_RenderARB);
            renderARB(useAlarmLighting, pMaterials, instanceHandle);
            PROFILE_END();
         }
      }
      else
      {
         if (useFogCoord())
         {
            PROFILE_START(IRO_Render_VC_FC);
            render_vc_fc(useAlarmLighting, pMaterials, instanceHandle, normalVLights, alarmVLights);
            PROFILE_END();
         }
         else
         {
            PROFILE_START(IRO_Render_VC_TF);
            render_vc_tf(useAlarmLighting, pMaterials, instanceHandle, normalVLights, alarmVLights);
            PROFILE_END();
         }
      }
   }
   else
   {
      if (useFogCoord())
      {
         PROFILE_START(IRO_Render_VC_FC);
         render_vc_fc(useAlarmLighting, pMaterials, instanceHandle, normalVLights, alarmVLights);
         PROFILE_END();
      }
      else
      {
         PROFILE_START(Render_VC_TF);
         render_vc_tf(useAlarmLighting, pMaterials, instanceHandle, normalVLights, alarmVLights);
         PROFILE_END();
      }
   }
}

void Interior::renderStaticMeshesSimple(bool texture, bool lightmap, LM_HANDLE instancelmhandle)
{
   // Lightmaps
   if (lightmap)
   {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glEnable(GL_TEXTURE_2D);

      if(texture)
         LightManager::sgSetupExposureRendering();
      else
         glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   }
   else
   {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glDisable(GL_TEXTURE_2D);
   }

   // Base textures
   glActiveTextureARB(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   // Misc setup
   glBlendFunc(GL_ONE, GL_ZERO);

   //glActiveTextureARB(GL_TEXTURE1_ARB);
   //glBindTexture(GL_TEXTURE_2D, mWhite->getGLName());
   //glActiveTextureARB(GL_TEXTURE0_ARB);

   // Render static meshes
   for (U32 i = 0; i < mStaticMeshes.size(); i++)
   {
      ConstructorSimpleMesh* mesh = mStaticMeshes[i];

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      dglMultMatrix(&mesh->transform);
      glScalef(mesh->scale.x, mesh->scale.y, mesh->scale.z);

      mesh->render(false, true, lightmap, false, getLMHandle(), instancelmhandle);

      glPopMatrix();
   }

   glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_CULL_FACE);

   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(-1, -1);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDepthMask(GL_FALSE);

   // And the transparent bits
   for (U32 i = 0; i < mStaticMeshes.size(); i++)
   {
      ConstructorSimpleMesh* mesh = mStaticMeshes[i];

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      dglMultMatrix(&mesh->transform);
      glScalef(mesh->scale.x, mesh->scale.y, mesh->scale.z);

		mesh->render(true, true, lightmap, false, getLMHandle(), instancelmhandle);

      glPopMatrix();
   }

   glActiveTextureARB(GL_TEXTURE1_ARB);
   LightManager::sgResetExposureRendering();
   glDisable(GL_TEXTURE_2D);

   glDepthMask(GL_TRUE);
   glDisable(GL_POLYGON_OFFSET_FILL);

   glDisable(GL_CULL_FACE);
	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);
}

//------------------------------------------------------------------------------
void Interior::render_vc_tf(const bool useAlarmLighting, MaterialList* pMaterials, const LM_HANDLE,
                            const Vector<ColorI>* normalVLights,
                            const Vector<ColorI>* alarmVLights)
{
   const Vector<ColorI>* pVertexColors = useAlarmLighting ? alarmVLights : normalVLights;

   OutputPoint* pFirstOutputPoint = (OutputPoint*)sgRenderBuffer;
   U32 currRenderBufferPoint = 0;
   U32 currIndexPoint        = 0;

   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glBlendFunc(GL_ONE, GL_ZERO);

   LightManager::sgSetupExposureRendering();

   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->point);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->texCoord);
   glEnableClientState(GL_COLOR_ARRAY);
   glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(OutputPoint), &pFirstOutputPoint->fogColor[0]);

   // Ok, our verts are set up, draw our polys.
   U32 currentlyBound = U32(-1);
   U32 currentTexGen  = U32(-1);

   // Draw the polys!
   U32 i;
   for (i = 0; i < sgActivePolyListSize; i++) {
      const Surface& rSurface = mSurfaces[sgActivePolyList[i]];

      // Setup the base texture...
      U32 baseName = pMaterials->getMaterial(rSurface.textureIndex).getGLName();
      if (baseName != currentlyBound) {
         flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
         currIndexPoint        = 0;
         currRenderBufferPoint = 0;

         glBindTexture(GL_TEXTURE_2D, baseName);
         currentlyBound = baseName;
      }

      if (currRenderBufferPoint + rSurface.windingCount >= 512 ||
          currIndexPoint        + (rSurface.windingCount - 2) * 3 >= 2048) {
         flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
         currIndexPoint        = 0;
         currRenderBufferPoint = 0;
      }

      if (rSurface.texGenIndex != currentTexGen)
      {
         currentTexGen = rSurface.texGenIndex;
         memcpy(texGen0,  &mTexGenEQs[rSurface.texGenIndex], sizeof(F32)*8);
      }

      emitPrimitiveSP(&sgRenderBuffer[currRenderBufferPoint],
                      &sgRenderIndices[currIndexPoint],
                      &mWindings[rSurface.windingStart],
                      rSurface.windingCount,
                      currRenderBufferPoint,
                      &mPoints[0],
                      &((*pVertexColors)[rSurface.windingStart]));
      currRenderBufferPoint += rSurface.windingCount;
      currIndexPoint        += (rSurface.windingCount - 2) * 3;
      AssertFatal(currRenderBufferPoint < 512 && currIndexPoint < 2048, "Aw, crap.1");
   }
   flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
   currIndexPoint        = 0;
   currRenderBufferPoint = 0;

   if(dglDoesSupportARBMultitexture())
   {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      LightManager::sgResetExposureRendering();
      glActiveTextureARB(GL_TEXTURE0_ARB);
   }
   
   glDisableClientState(GL_COLOR_ARRAY);
   if (sgFogActive) {
      extern Point2F* sgFogTexCoords;
      glVertexPointer(3, GL_FLOAT, sizeof(ItrPaddedPoint), &mPoints[0].point);
      glTexCoordPointer(2, GL_FLOAT, sizeof(Point2F), sgFogTexCoords);

      glBindTexture(GL_TEXTURE_2D, gClientSceneGraph->getFogTexture().getGLName());
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      extern ColorF gInteriorFogColor;
      glColor4f(gInteriorFogColor.red,
                gInteriorFogColor.green,
                gInteriorFogColor.blue,
                1);

      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(-1,-1);

      if (dglDoesSupportCompiledVertexArray())
         glLockArraysEXT(0, mPoints.size());
      for (U32 i = 0; i < sgActivePolyListSize; i++) {
         const Surface& rSurface = mSurfaces[sgActivePolyList[i]];
         glDrawElements(GL_TRIANGLE_STRIP, rSurface.windingCount, GL_UNSIGNED_INT, &mWindings[rSurface.windingStart]);
      }
      if (dglDoesSupportCompiledVertexArray())
         glUnlockArraysEXT();

      glDisable(GL_POLYGON_OFFSET_FILL);
   }

   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   glDisable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ZERO);
}


//------------------------------------------------------------------------------
void Interior::render_vc_fc(const bool useAlarmLighting, MaterialList* pMaterials, const LM_HANDLE,
                            const Vector<ColorI>* normalVLights,
                            const Vector<ColorI>* alarmVLights)

{
   const Vector<ColorI>* pVertexColors = useAlarmLighting ? alarmVLights : normalVLights;

   OutputPoint* pFirstOutputPoint = (OutputPoint*)sgRenderBuffer;
   U32 currRenderBufferPoint = 0;
   U32 currIndexPoint        = 0;
   bool supportBuffers;
   GLint bufferHandle;
   OutputPointSP_FC_VB *vertexBuffer = NULL;

   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glBlendFunc(GL_ONE, GL_ZERO);

   LightManager::sgSetupExposureRendering();

   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->point);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->texCoord);
   glEnableClientState(GL_COLOR_ARRAY);
   glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(OutputPoint), &pFirstOutputPoint->fogColor[0]);
   if (sgFogActive) {
      // Need the fog coord pointer enabled here...
      glEnableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
      glFogCoordPointerEXT(GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->lmCoord.x);
   }

   // Ok, our verts are set up, draw our polys.
   U32 currentlyBound = U32(-1);
   U32 currentTexGen  = U32(-1);

   if ((supportBuffers = dglDoesSupportVertexBuffer()) == true)
   {
      bufferHandle = gInteriorLMManager.getVertexBuffer(GL_V12FTVFMT_EXT);
      // guess we ran out of video memory
      if (bufferHandle == -1)
         supportBuffers = false;
   }

   // Draw the polys!
   U32 i;
   for (i = 0; i < sgActivePolyListSize; i++) {
      const Surface& rSurface = mSurfaces[sgActivePolyList[i]];

      // Setup the base texture...
      U32 baseName = pMaterials->getMaterial(rSurface.textureIndex).getGLName();
      if (baseName != currentlyBound) {
         if (supportBuffers)
         {
            if (vertexBuffer)
               glUnlockVertexBufferEXT(bufferHandle);
            flushPrimitivesVB(sgRenderIndices, currIndexPoint, currRenderBufferPoint, bufferHandle);
            vertexBuffer = (OutputPointSP_FC_VB *) glLockVertexBufferEXT(bufferHandle,512);
         }
         else
            flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
         currIndexPoint        = 0;
         currRenderBufferPoint = 0;

         glBindTexture(GL_TEXTURE_2D, baseName);
         currentlyBound = baseName;
      }

      if (currRenderBufferPoint + rSurface.windingCount >= 512 ||
          currIndexPoint        + (rSurface.windingCount - 2) * 3 >= 2048) {
         if (supportBuffers)
         {
            if (vertexBuffer)
               glUnlockVertexBufferEXT(bufferHandle);
            flushPrimitivesVB(sgRenderIndices, currIndexPoint, currRenderBufferPoint, bufferHandle);
            vertexBuffer = (OutputPointSP_FC_VB *) glLockVertexBufferEXT(bufferHandle,512);
         }
         else
            flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
         currIndexPoint        = 0;
         currRenderBufferPoint = 0;
      }

      if (rSurface.texGenIndex != currentTexGen)
      {
         currentTexGen = rSurface.texGenIndex;
         memcpy(texGen0,  &mTexGenEQs[rSurface.texGenIndex], sizeof(F32)*8);
      }

      if (supportBuffers)
         emitPrimitiveSP_FC_VB(&vertexBuffer[currRenderBufferPoint],
                               &sgRenderIndices[currIndexPoint],
                               &mWindings[rSurface.windingStart],
                               rSurface.windingCount,
                               currRenderBufferPoint,
                               &mPoints[0],
                               &((*pVertexColors)[rSurface.windingStart]));
      else
         emitPrimitiveSP_FC(&sgRenderBuffer[currRenderBufferPoint],
                            &sgRenderIndices[currIndexPoint],
                            &mWindings[rSurface.windingStart],
                            rSurface.windingCount,
                            currRenderBufferPoint,
                            &mPoints[0],
                            &((*pVertexColors)[rSurface.windingStart]));
      currRenderBufferPoint += rSurface.windingCount;
      currIndexPoint        += (rSurface.windingCount - 2) * 3;
      AssertFatal(currRenderBufferPoint < 512 && currIndexPoint < 2048, "Aw, crap.2");
   }
   if (supportBuffers)
   {
      if (vertexBuffer)
         glUnlockVertexBufferEXT(bufferHandle);
      flushPrimitivesVB(sgRenderIndices, currIndexPoint, currRenderBufferPoint, bufferHandle);
   }
   else
      flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
   currIndexPoint        = 0;
   currRenderBufferPoint = 0;

   if(dglDoesSupportARBMultitexture())
   {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      LightManager::sgResetExposureRendering();
      glActiveTextureARB(GL_TEXTURE0_ARB);
   }
   
   if (sgFogActive) {
      // Need the fog coord pointer enabled here...
      glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
   }

   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   glDisable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ZERO);
}


void Interior::renderARB_vc_tf(const bool useAlarmLighting, MaterialList* pMaterials, const LM_HANDLE,
                               const Vector<ColorI>* normalVLights,
                               const Vector<ColorI>* alarmVLights)

{
   // DMMNOTE: NON-FUNCTIONAL ON THE GEFORCE!  DO NOT PUT IN RENDER FUNCTION!!!
   // DMMNOTE: NON-FUNCTIONAL ON THE GEFORCE!  DO NOT PUT IN RENDER FUNCTION!!!
   // DMMNOTE: NON-FUNCTIONAL ON THE GEFORCE!  DO NOT PUT IN RENDER FUNCTION!!!
   U32 i;
   const Vector<ColorI>* pVertexColors = useAlarmLighting ? alarmVLights : normalVLights;

   extern Point2F* sgFogTexCoords;
   fogCoordinatePointer = sgFogTexCoords;

   OutputPoint* pFirstOutputPoint = (OutputPoint*)sgRenderBuffer;
   U32 currRenderBufferPoint = 0;
   U32 currIndexPoint        = 0;

   // Base textures
   glActiveTextureARB(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   glActiveTextureARB(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   glBindTexture(GL_TEXTURE_2D, gClientSceneGraph->getFogTexture().getGLName());
   glActiveTextureARB(GL_TEXTURE0_ARB);

   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->point);
   glEnableClientState(GL_COLOR_ARRAY);
   glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(OutputPoint), &pFirstOutputPoint->fogColor[0]);

//    // Fog texture
//    if (sgFogActive)
//    {
   glClientActiveTextureARB(GL_TEXTURE1_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->texCoord);
//      glClientActiveTextureARB(GL_TEXTURE0_ARB);
//    }
//    else
//    {
//       glActiveTextureARB(GL_TEXTURE1_ARB);
//       glDisable(GL_TEXTURE_2D);

//       glClientActiveTextureARB(GL_TEXTURE1_ARB);
//       glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//    }

   glClientActiveTextureARB(GL_TEXTURE0_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->lmCoord);

   // Misc setup
   glBlendFunc(GL_ONE, GL_ZERO);
   glActiveTextureARB(GL_TEXTURE0_ARB);

   // Ok, our verts are set up, draw our polys.
   U32 currentlyBound = U32(-1);
   U32 currentTexGen   = U32(-1);

   // Draw the polys!
   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
   for (i = 0; i < sgActivePolyListSize; i++) {
      const Surface& rSurface = mSurfaces[sgActivePolyList[i]];

      // Setup the base texture...
      U32 baseName = pMaterials->getMaterial(rSurface.textureIndex).getGLName();
      if (baseName != currentlyBound) {
         flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
         currIndexPoint        = 0;
         currRenderBufferPoint = 0;

         glBindTexture(GL_TEXTURE_2D, baseName);
         currentlyBound = baseName;
      }

      if (currRenderBufferPoint + rSurface.windingCount >= 512 ||
          currIndexPoint        + (rSurface.windingCount - 2) * 3 >= 2048) {
         flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
         currIndexPoint        = 0;
         currRenderBufferPoint = 0;
      }
      if (rSurface.texGenIndex != currentTexGen)
      {
         currentTexGen = rSurface.texGenIndex;
         memcpy(texGen0,  &mTexGenEQs[rSurface.texGenIndex], sizeof(F32)*8);
      }

      emitPrimitiveVC_TF(&sgRenderBuffer[currRenderBufferPoint],
                         &sgRenderIndices[currIndexPoint],
                         &mWindings[rSurface.windingStart],
                         rSurface.windingCount,
                         currRenderBufferPoint,
                         &mPoints[0],
                         &((*pVertexColors)[rSurface.windingStart]));
      currRenderBufferPoint += rSurface.windingCount;
      currIndexPoint        += (rSurface.windingCount - 2) * 3;
      AssertFatal(currRenderBufferPoint < 512 && currIndexPoint < 2048, "Aw, crap.3");
   }
   flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
   currIndexPoint        = 0;
   currRenderBufferPoint = 0;

   glClientActiveTextureARB(GL_TEXTURE1_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glClientActiveTextureARB(GL_TEXTURE0_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_COLOR_ARRAY);

   glDisable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ZERO);
}


/*void Interior::renderSimple(MaterialList* pMaterials)
{
   U32 i;

   OutputPoint* pFirstOutputPoint = (OutputPoint*)sgRenderBuffer;
   U32 currRenderBufferPoint = 0;
   U32 currIndexPoint        = 0;

   // Set up texturing
   glActiveTextureARB(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   // Set up our pointers...
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->point);

   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->texCoord);

   // Ok, our verts are set up, draw our polys.
   U32 currentlyBound0 = U32(-1);
   U32 currentlyBound1 = U32(-1);
   U32 currentTexGen   = U32(-1);

   // Draw the polys!
   glColor4f(1, 1, 1, 1);
   for (i = 0; i < sgActivePolyListSize; i++)
   {
      const Surface& rSurface = mSurfaces[sgActivePolyList[i]];

      // Setup the base material...
      U32 baseName = pMaterials->getMaterial(rSurface.textureIndex).getGLName();

      if (baseName != currentlyBound1)
      {
         flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);

         currIndexPoint        = 0;
         currRenderBufferPoint = 0;
         currentlyBound1 = baseName;

         glBindTexture(GL_TEXTURE_2D, baseName);
      }

      if (currRenderBufferPoint + rSurface.windingCount >= 512 ||
         currIndexPoint        + (rSurface.windingCount - 2) * 3 >= 2048)
      {
         flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
         currIndexPoint        = 0;
         currRenderBufferPoint = 0;
      }

      if (rSurface.texGenIndex != currentTexGen)
      {
         currentTexGen = rSurface.texGenIndex;
         dMemcpy(texGen0,  &mTexGenEQs[rSurface.texGenIndex], sizeof(F32)*8);
      }
      dMemcpy(texGen1, &mLMTexGenEQs[sgActivePolyList[i]], sizeof(F32)*8);

      emitPrimitive(&sgRenderBuffer[currRenderBufferPoint],
         &sgRenderIndices[currIndexPoint],
         &mWindings[rSurface.windingStart],
         rSurface.windingCount,
         currRenderBufferPoint,
         &mPoints[0]);

      currRenderBufferPoint += rSurface.windingCount;
      currIndexPoint        += (rSurface.windingCount - 2) * 3;
      AssertFatal(currRenderBufferPoint < 512 && currIndexPoint < 2048, "Aw, crap.4");
   }

   flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
   currIndexPoint        = 0;
   currRenderBufferPoint = 0;

   if (sgFogActive)
   {
      // Set up our pointers.
      extern Point2F* sgFogTexCoords;
      glVertexPointer(3, GL_FLOAT, sizeof(ItrPaddedPoint), &mPoints[0].point);
      glTexCoordPointer(2, GL_FLOAT, sizeof(Point2F), sgFogTexCoords);

      //  Put the fog texture on.
      glBindTexture(GL_TEXTURE_2D, gClientSceneGraph->getFogTexture().getGLName());
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      extern ColorF gInteriorFogColor;
      glColor4f(  gInteriorFogColor.red,
                  gInteriorFogColor.green,
                  gInteriorFogColor.blue,
                  1.0);

      if (dglDoesSupportCompiledVertexArray())
         glLockArraysEXT(0, mPoints.size());

      for (U32 i = 0; i < sgActivePolyListSize; i++)
      {
         const Surface& rSurface = mSurfaces[sgActivePolyList[i]];
         glDrawElements(GL_TRIANGLE_STRIP, rSurface.windingCount, GL_UNSIGNED_INT, &mWindings[rSurface.windingStart]);
      }

      if (dglDoesSupportCompiledVertexArray())
         glUnlockArraysEXT();

      glActiveTextureARB(GL_TEXTURE1_ARB);
      glEnable(GL_TEXTURE_2D);
   }

   glClientActiveTextureARB(GL_TEXTURE1_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glClientActiveTextureARB(GL_TEXTURE0_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ZERO);
} */

void Interior::renderARB(const bool useAlarmLighting, MaterialList* pMaterials, const LM_HANDLE instanceHandle)
{
   U32 i;
   Vector<U32>* pLMapIndices = useAlarmLighting ? &mAlarmLMapIndices : &mNormalLMapIndices;

   OutputPoint* pFirstOutputPoint = (OutputPoint*)sgRenderBuffer;
   U32 currRenderBufferPoint = 0;
   U32 currIndexPoint        = 0;

   // Lightmaps
   glActiveTextureARB(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   // Base textures
   glActiveTextureARB(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   // Misc setup
   glBlendFunc(GL_ONE, GL_ZERO);
   
   LightManager::sgSetupExposureRendering();
   
   glActiveTextureARB(GL_TEXTURE0_ARB);

   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->point);

   glClientActiveTextureARB(GL_TEXTURE1_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->texCoord);

   glClientActiveTextureARB(GL_TEXTURE0_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->lmCoord);

   detailMapping.sgEnableDetailMapping(&pFirstOutputPoint->texCoord, sizeof(OutputPoint));
   
   // Ok, our verts are set up, draw our polys.
   U32 currentlyBound0 = U32(-1);
   U32 currentlyBound1 = U32(-1);
   U32 currentTexGen   = U32(-1);

   // Draw the polys!
   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
   for (i = 0; i < sgActivePolyListSize; i++) {
      const Surface& rSurface = mSurfaces[sgActivePolyList[i]];

      // Setup the base texture...
      U32 baseName = pMaterials->getMaterial(rSurface.textureIndex).getGLName();
      if (baseName != currentlyBound1) {
         flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
         currIndexPoint        = 0;
         currRenderBufferPoint = 0;

         glActiveTextureARB(GL_TEXTURE1_ARB);
         glBindTexture(GL_TEXTURE_2D, baseName);
         currentlyBound1 = baseName;
         
         detailMapping.sgBindDetailMap(rSurface.textureIndex);
         
         glActiveTextureARB(GL_TEXTURE0_ARB);
      }

      // Setup the lightmap
      baseName = (*pLMapIndices)[sgActivePolyList[i]];
      if (baseName != currentlyBound0) {
         flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
         currIndexPoint        = 0;
         currRenderBufferPoint = 0;

         U32 glName = gInteriorLMManager.getHandle(mLMHandle, instanceHandle, baseName)->getGLName();
         AssertFatal(glName, "Interior::renderARB: invalid glName for texture handle");

         glBindTexture(GL_TEXTURE_2D, glName);
         currentlyBound0 = baseName;
      }

      if (currRenderBufferPoint + rSurface.windingCount >= 512 ||
          currIndexPoint        + (rSurface.windingCount - 2) * 3 >= 2048) {
         flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
         currIndexPoint        = 0;
         currRenderBufferPoint = 0;
      }
      if (rSurface.texGenIndex != currentTexGen)
      {
         currentTexGen = rSurface.texGenIndex;
         memcpy(texGen0,  &mTexGenEQs[rSurface.texGenIndex], sizeof(F32)*8);
      }
      memcpy(texGen1, &mLMTexGenEQs[sgActivePolyList[i]], sizeof(F32)*8);

      emitPrimitive(&sgRenderBuffer[currRenderBufferPoint],
                    &sgRenderIndices[currIndexPoint],
                    &mWindings[rSurface.windingStart],
                    rSurface.windingCount,
                    currRenderBufferPoint,
                    &mPoints[0]);
      currRenderBufferPoint += rSurface.windingCount;
      currIndexPoint        += (rSurface.windingCount - 2) * 3;
      AssertFatal(currRenderBufferPoint < 512 && currIndexPoint < 2048, "Aw, crap.4");
   }
   flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
   currIndexPoint        = 0;
   currRenderBufferPoint = 0;

   detailMapping.sgDisableDetailMapping();

   if (sgFogActive) {
      glVertexPointer(3, GL_FLOAT, sizeof(ItrPaddedPoint), &mPoints[0].point);

      extern Point2F* sgFogTexCoords;
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);

      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glTexCoordPointer(2, GL_FLOAT, sizeof(Point2F), sgFogTexCoords);

      glActiveTextureARB(GL_TEXTURE1_ARB);
      glDisable(GL_TEXTURE_2D);

      glActiveTextureARB(GL_TEXTURE0_ARB);
      glBindTexture(GL_TEXTURE_2D, gClientSceneGraph->getFogTexture().getGLName());
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      extern ColorF gInteriorFogColor;
      glColor4f(gInteriorFogColor.red,
                gInteriorFogColor.green,
                gInteriorFogColor.blue,
                1);

      if (dglDoesSupportCompiledVertexArray())
         glLockArraysEXT(0, mPoints.size());
      for (U32 i = 0; i < sgActivePolyListSize; i++) {
         const Surface& rSurface = mSurfaces[sgActivePolyList[i]];
         glDrawElements(GL_TRIANGLE_STRIP, rSurface.windingCount, GL_UNSIGNED_INT, &mWindings[rSurface.windingStart]);
      }
      if (dglDoesSupportCompiledVertexArray())
         glUnlockArraysEXT();

      glActiveTextureARB(GL_TEXTURE1_ARB);
      glEnable(GL_TEXTURE_2D);
   }

   glClientActiveTextureARB(GL_TEXTURE1_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glClientActiveTextureARB(GL_TEXTURE0_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);

   glActiveTextureARB(GL_TEXTURE1_ARB);
   LightManager::sgResetExposureRendering();
   glActiveTextureARB(GL_TEXTURE0_ARB);

   // Render environment maps...
   if (smRenderEnvironmentMaps && sgEnvironPolyListSize) {
      currentlyBound1 = (U32)-1;
      currentlyBound0 = (U32)-1;

      // Base textures
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      // Envmaps textures
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      // Misc setup
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glActiveTextureARB(GL_TEXTURE0_ARB);

      for (i = 0; i < sgEnvironPolyListSize; i++) {
         const Surface& rSurface = mSurfaces[sgEnvironPolyList[i]];

         // Setup the base texture...
         U32 baseName = mMaterialList->getMaterial(rSurface.textureIndex).getGLName();
         if (baseName != currentlyBound0) {
            glBindTexture(GL_TEXTURE_2D, baseName);
            currentlyBound0 = baseName;
         }

         // Setup the environment map...
         baseName = mEnvironMaps[rSurface.textureIndex]->getGLName();
         if (baseName != currentlyBound1) {
            glActiveTextureARB(GL_TEXTURE1_ARB);
            glBindTexture(GL_TEXTURE_2D, baseName);
            currentlyBound1 = baseName;
            glActiveTextureARB(GL_TEXTURE0_ARB);
         }

         const PlaneF& plane = getPlane(rSurface.planeIndex);
         Point3F normal = plane;
         if (planeIsFlipped(rSurface.planeIndex))
            normal.neg();

         // And the colors...
         F32 baseLevel = mEnvironFactors[rSurface.textureIndex];

         glBegin(GL_TRIANGLE_STRIP);
         for (U32 j = rSurface.windingStart; j < rSurface.windingStart + rSurface.windingCount; j++) {
            F32 s = mTexGenEQs[rSurface.texGenIndex].planeX.distToPlane(mPoints[mWindings[j]].point);
            F32 t = mTexGenEQs[rSurface.texGenIndex].planeY.distToPlane(mPoints[mWindings[j]].point);

            Point3F u = mPoints[mWindings[j]].point - sgOSCamPosition;
            Point3F r = u;
            F32 dot = mDot(normal, u) * 2.0f;
            u = normal * dot;
            r -= u;
            F32 m = 2.0f * mSqrt(r.x*r.x + r.y*r.y + (r.z+1.0f)*(r.z+1.0f));

            glColor4f(1.0f, 1.0f, 1.0f, baseLevel);
            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, s, t);
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, r.x/m + 0.5f,
                                 r.y/m + 0.5f);
            glVertex3fv(mPoints[mWindings[j]].point);
         }
         glEnd();
      }
   } // if (environment render on);

   glDisable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ZERO);
}


void Interior::renderARB_FC(const bool useAlarmLighting, MaterialList* pMaterials, const LM_HANDLE instanceHandle)
{
   U32 i;
   Vector<U32>* pLMapIndices = useAlarmLighting ? &mAlarmLMapIndices : &mNormalLMapIndices;

   OutputPoint* pFirstOutputPoint = (OutputPoint*)sgRenderBuffer;
   U32 currRenderBufferPoint = 0;
   U32 currIndexPoint        = 0;
   bool supportBuffers;
   GLint bufferHandle;
   OutputPointFC_VB *vertexBuffer = NULL;

   // Lightmaps
   glActiveTextureARB(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   // Base textures
   glActiveTextureARB(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   // Misc setup
   glBlendFunc(GL_ONE, GL_ZERO);
   
   LightManager::sgSetupExposureRendering();
   
   glActiveTextureARB(GL_TEXTURE0_ARB);

   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->point);
   glClientActiveTextureARB(GL_TEXTURE1_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->texCoord);
   glClientActiveTextureARB(GL_TEXTURE0_ARB);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->lmCoord);
   if (sgFogActive)
   {
      // Need the fog coord pointer enabled here...
      glEnableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
      glFogCoordPointerEXT(GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->fogCoord);
   }

   detailMapping.sgEnableDetailMapping(&pFirstOutputPoint->texCoord, sizeof(OutputPoint));

   // Ok, our verts are set up, draw our polys.
   U32 currentlyBound0 = U32(-1);
   U32 currentlyBound1 = U32(-1);
   U32 currentTexGen = U32(-1);

   if ((supportBuffers = dglDoesSupportVertexBuffer()) == true)
   {
      bufferHandle = gInteriorLMManager.getVertexBuffer(GL_V12FMTVFMT_EXT);
      // guess we ran out of video memory
      if (bufferHandle == -1)
         supportBuffers = false;
   }

   // Draw the polys!
   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
   for (i = 0; i < sgActivePolyListSize; i++) {
      const Surface& rSurface = mSurfaces[sgActivePolyList[i]];

      // Setup the base texture...
      U32 baseName = pMaterials->getMaterial(rSurface.textureIndex).getGLName();
      if (baseName != currentlyBound1) {
         if (supportBuffers)
         {
            if (vertexBuffer)
               glUnlockVertexBufferEXT(bufferHandle);
            flushPrimitivesVB(sgRenderIndices, currIndexPoint, currRenderBufferPoint, bufferHandle);
            vertexBuffer = (OutputPointFC_VB *) glLockVertexBufferEXT(bufferHandle,512);
         }
         else
            flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);

         currIndexPoint        = 0;
         currRenderBufferPoint = 0;

         glActiveTextureARB(GL_TEXTURE1_ARB);
         glBindTexture(GL_TEXTURE_2D, baseName);
         currentlyBound1 = baseName;
         
         detailMapping.sgBindDetailMap(rSurface.textureIndex);
         
         glActiveTextureARB(GL_TEXTURE0_ARB);				
      }

      // Setup the lightmap
      baseName = (*pLMapIndices)[sgActivePolyList[i]];
      if (baseName != currentlyBound0) {
         if (supportBuffers)
         {
            if (vertexBuffer)
               glUnlockVertexBufferEXT(bufferHandle);
            flushPrimitivesVB(sgRenderIndices, currIndexPoint, currRenderBufferPoint, bufferHandle);
            vertexBuffer = (OutputPointFC_VB *) glLockVertexBufferEXT(bufferHandle,512);
         }
         else
            flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);

         currIndexPoint        = 0;
         currRenderBufferPoint = 0;

         U32 glName = gInteriorLMManager.getHandle(mLMHandle, instanceHandle, baseName)->getGLName();
         AssertFatal(glName, "Interior::renderARB_FC: invalid glName for texture handle");

         if (glName)
         glBindTexture(GL_TEXTURE_2D, glName);
         else if (mWhite->getGLName())
            glBindTexture(GL_TEXTURE_2D, mWhite->getGLName());
         currentlyBound0 = baseName;	
      }

      if (currRenderBufferPoint + rSurface.windingCount >= 512 ||
          currIndexPoint        + (rSurface.windingCount - 2) * 3 >= 2048) {
         if (supportBuffers)
         {
            if (vertexBuffer)
               glUnlockVertexBufferEXT(bufferHandle);
            flushPrimitivesVB(sgRenderIndices, currIndexPoint, currRenderBufferPoint, bufferHandle);
            vertexBuffer = (OutputPointFC_VB *) glLockVertexBufferEXT(bufferHandle,512);
         }
         else
            flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
         currIndexPoint        = 0;
         currRenderBufferPoint = 0;
      }
      if (rSurface.texGenIndex != currentTexGen)
      {
         currentTexGen = rSurface.texGenIndex;
         memcpy(texGen0,  &mTexGenEQs[rSurface.texGenIndex], sizeof(F32)*8);
      }
      memcpy(texGen1,  &mLMTexGenEQs[sgActivePolyList[i]], sizeof(F32)*8);

      if (supportBuffers)
         emitPrimitiveFC_VB(&vertexBuffer[currRenderBufferPoint],
                            &sgRenderIndices[currIndexPoint],
                            &mWindings[rSurface.windingStart],
                            rSurface.windingCount,
                            currRenderBufferPoint,
                            &mPoints[0]);
      else
         emitPrimitive(&sgRenderBuffer[currRenderBufferPoint],
                       &sgRenderIndices[currIndexPoint],
                       &mWindings[rSurface.windingStart],
                       rSurface.windingCount,
                       currRenderBufferPoint,
                       &mPoints[0]);
      currRenderBufferPoint += rSurface.windingCount;
      currIndexPoint        += (rSurface.windingCount - 2) * 3;
      AssertFatal(currRenderBufferPoint < 512 && currIndexPoint < 2048, "Aw, crap.5");
   }

   if (supportBuffers)
   {
      if (vertexBuffer)
         glUnlockVertexBufferEXT(bufferHandle);
      flushPrimitivesVB(sgRenderIndices, currIndexPoint, currRenderBufferPoint, bufferHandle);
   }
   else
      flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);

   currIndexPoint        = 0;
   currRenderBufferPoint = 0;

   detailMapping.sgDisableDetailMapping();

   if (sgFogActive)
      glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
   glClientActiveTextureARB(GL_TEXTURE1_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glClientActiveTextureARB(GL_TEXTURE0_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);

   glActiveTextureARB(GL_TEXTURE1_ARB);
   LightManager::sgResetExposureRendering();
   glActiveTextureARB(GL_TEXTURE0_ARB);
   
   // Render environment maps...
   if (smRenderEnvironmentMaps && sgEnvironPolyListSize) {
      glDisable(GL_FOG);
      currentlyBound1 = (U32)-1;
      currentlyBound0 = (U32)-1;

      // Base textures
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      // Envmaps textures
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      // Misc setup
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glEnable(GL_POLYGON_OFFSET_FILL);
      glPolygonOffset(-1,-1);

      for (i = 0; i < sgEnvironPolyListSize; i++) {
         const Surface& rSurface = mSurfaces[sgEnvironPolyList[i]];

         // Setup the base texture...
         U32 baseName = mMaterialList->getMaterial(rSurface.textureIndex).getGLName();
         if (baseName != currentlyBound0) {
            glBindTexture(GL_TEXTURE_2D, baseName);
            currentlyBound0 = baseName;
         }

         // Setup the environment map...
         baseName = mEnvironMaps[rSurface.textureIndex]->getGLName();
         if (baseName != currentlyBound1) {
            glActiveTextureARB(GL_TEXTURE1_ARB);
            glBindTexture(GL_TEXTURE_2D, baseName);
            currentlyBound1 = baseName;
            glActiveTextureARB(GL_TEXTURE0_ARB);
         }

         const PlaneF& plane = getPlane(rSurface.planeIndex);
         Point3F normal = plane;
         if (planeIsFlipped(rSurface.planeIndex))
            normal.neg();

         // And the colors...
         F32 baseLevel = mEnvironFactors[rSurface.textureIndex];

         glBegin(GL_TRIANGLE_STRIP);
         for (U32 j = rSurface.windingStart; j < rSurface.windingStart + rSurface.windingCount; j++) {
            F32 s = mTexGenEQs[rSurface.texGenIndex].planeX.distToPlane(mPoints[mWindings[j]].point);
            F32 t = mTexGenEQs[rSurface.texGenIndex].planeY.distToPlane(mPoints[mWindings[j]].point);

            Point3F u = mPoints[mWindings[j]].point - sgOSCamPosition;
            Point3F r = u;
            F32 dot = mDot(normal, u) * 2.0f;
            u = normal * dot;
            r -= u;
            F32 m = 2.0f * mSqrt(r.x*r.x + r.y*r.y + (r.z+1.0f)*(r.z+1.0f));

            glColor4f(1.0f, 1.0f, 1.0f, baseLevel * (1.0f - mPoints[mWindings[j]].fogCoord));
            glMultiTexCoord2fARB(GL_TEXTURE0_ARB, s, t);
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, r.x/m + 0.5f,
                                 r.y/m + 0.5f);
            glVertex3fv(mPoints[mWindings[j]].point);
         }
         glEnd();
      }
      glDisable(GL_POLYGON_OFFSET_FILL);
   } // if (environment render on);

   glDisable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ZERO);
}


void Interior::renderLights(LightInfo*     pInfo,
                            const MatrixF& transform,
                            const Point3F& scale,
                            Vector<U32> surfaces)
{
   Point3F lightPoint = pInfo->mPos;
   transform.mulP(lightPoint);
   lightPoint.convolveInverse(scale);

   if (dglDoesSupportARBMultitexture()) {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glDisable(GL_TEXTURE_2D);
      glActiveTextureARB(GL_TEXTURE0_ARB);
   }

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, mLightFalloff->getGLName());

   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

   glActiveTextureARB(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   LightManager::sgSetupExposureRendering();
   
   glBindTexture(GL_TEXTURE_2D, mLightFalloff->getGLName());

   glActiveTextureARB(GL_TEXTURE0_ARB);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE);

   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(-0.075f, -0.075f);
   glDepthMask(GL_FALSE);

   glActiveTextureARB(GL_TEXTURE1_ARB);

   const F32	mRadiusSquared = (pInfo->mRadius*pInfo->mRadius);

   int currentlyBound1 = -1;
   for(U32 i=0; i<surfaces.size(); i++)
   {
      const Surface& rSurface = mSurfaces[surfaces[i]];

      const PlaneF& plane = getPlane(rSurface.planeIndex);

      F32 d = plane.distToPlane(lightPoint);
      F32 d2 = d;
      if(Interior::planeIsFlipped(rSurface.planeIndex))
	     d2 = -d2;
      if((d2 < 0.0f) && (!pInfo->sgDoubleSidedAmbient))
		 continue;
      d2 = d;	// used to avoid creating Point3F if we don't have to - moved centerPoint down
      d = mFabs(d);
      if(d >= pInfo->mRadius)
         continue;
		 
      F32 mr = 1.0f / mSqrt(mRadiusSquared - d*d);

      const Point3F centerPoint = lightPoint - plane * d2;

      Point3F normalS;
      if(mFabs(plane.z) < 0.9f)
         mCross(plane, Point3F(0.0f, 0.0f, 1.0f), &normalS);
      else
         mCross(plane, Point3F(0.0f, 1.0f, 0.0f), &normalS);

      Point3F normalT;
      mCross(plane, normalS, &normalT);

      normalS.normalize();
      normalT.normalize();

      PlaneF splane(centerPoint, normalS);
      PlaneF tplane(centerPoint, normalT);

      U32 baseName = mMaterialList->getMaterial(rSurface.textureIndex).getGLName();
      if(baseName != currentlyBound1)
      {
         //glActiveTextureARB(GL_TEXTURE1_ARB);
         glBindTexture(GL_TEXTURE_2D, baseName);
         currentlyBound1 = baseName;
         //glActiveTextureARB(GL_TEXTURE0_ARB);
      }

      F32 factor = (pInfo->mRadius - d) / pInfo->mRadius;
      glColor4f(pInfo->mColor.red, pInfo->mColor.green, pInfo->mColor.blue, factor);

      F32 tt[8];
      dMemcpy(tt, &mTexGenEQs[rSurface.texGenIndex], sizeof(F32)*8);

      const U32 count = rSurface.windingStart + rSurface.windingCount;
      glBegin(GL_TRIANGLE_STRIP);
      for (U32 j = rSurface.windingStart; j < count; j++)
      {
         const Point3F& rPoint = mPoints[mWindings[j]].point;

         glMultiTexCoord2fARB(GL_TEXTURE0_ARB, ((splane.distToPlane(rPoint) * mr) + 1.0f) * 0.5f,
            ((tplane.distToPlane(rPoint) * mr) + 1.0f) * 0.5f);
         glMultiTexCoord2fARB(GL_TEXTURE1_ARB,
            ((tt[0]*rPoint.x) + (tt[1]*rPoint.y) + (tt[2]*rPoint.z) + (tt[3])),
            ((tt[4]*rPoint.x) + (tt[5]*rPoint.y) + (tt[6]*rPoint.z) + (tt[7])));

		 // fog coord (not always supported and terrain still doesn't fog)...
		 //mPoints[mWindings[j]].fogCoord

         glVertex3fv(mPoints[mWindings[j]].point);
      }
      glEnd();
   }

   glDisable(GL_POLYGON_OFFSET_FILL);
   glDepthMask(GL_TRUE);

   glActiveTextureARB(GL_TEXTURE1_ARB);
   LightManager::sgResetExposureRendering();
   glActiveTextureARB(GL_TEXTURE0_ARB);

   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   glDisable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ZERO);
}

void Interior::renderStaticMeshes(InteriorInstance *instance)
{
   LightManager *lm = gClientSceneGraph->getLightManager();
   S32 maxlights = lm->sgGetMaxBestLights();

   const MatrixF &transform = instance->getTransform();
   const MatrixF &rendertransform = instance->getRenderTransform();
   const Point3F &scale = instance->getScale();

   // base texture
   glActiveTextureARB(GL_TEXTURE1_ARB);
   glEnable(GL_TEXTURE_2D);
   LightManager::sgSetupExposureRendering();

   // light map
   glActiveTextureARB(GL_TEXTURE0_ARB);
   glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);

   glEnable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ZERO);

   // need to build interior space *after* sgSetupLights...
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

   // Render static meshes
   for (U32 i = 0; i < mStaticMeshes.size(); i++)
   {
      ConstructorSimpleMesh* mesh = mStaticMeshes[i];

      // ok this is a start - but we should really have a
      // world box based viz test in SceneState to cull
      // unseen objects...
      if(!mesh->containsPrimitiveType(false))
         continue;

      Box3F interiorspacebox, worldspacebox;
      MathUtils::transformBoundingBox(mesh->bounds, mesh->transform, mesh->scale, interiorspacebox);
      MathUtils::transformBoundingBox(interiorspacebox, transform, scale, worldspacebox);

      lm->sgSetupLights(instance, worldspacebox, maxlights);
      // have to manually install lights in this mode...
      lm->sgFilterLights(false, true);
      lm->sgInstallLights();
      // and this...
      Point4F color = Point4F(1, 1, 1, 1);
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      dglMultMatrix(&rendertransform);
      glScalef(scale.x, scale.y, scale.z);
      dglMultMatrix(&mesh->transform);
      glScalef(mesh->scale.x, mesh->scale.y, mesh->scale.z);

      mesh->render(false, true, true, true, getLMHandle(), instance->getLMHandle());

      glPopMatrix();

      lm->sgResetLights();
   }

   glEnable(GL_POLYGON_OFFSET_FILL);
   glPolygonOffset(-0.075f, -0.075f);

	glDisable(GL_CULL_FACE);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   // And the transparent bits
   for (U32 i = 0; i < mStaticMeshes.size(); i++)
   {
      ConstructorSimpleMesh* mesh = mStaticMeshes[i];
      if(!mesh->containsPrimitiveType(true))
         continue;

      Box3F interiorspacebox, worldspacebox;
      MathUtils::transformBoundingBox(mesh->bounds, mesh->transform, mesh->scale, interiorspacebox);
      MathUtils::transformBoundingBox(interiorspacebox, transform, scale, worldspacebox);

      lm->sgSetupLights(instance, worldspacebox, maxlights);
      // have to manually install lights in this mode...
      lm->sgInstallLights();
      // and this...
      Point4F color = Point4F(1, 1, 1, 1);
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      dglMultMatrix(&rendertransform);
      glScalef(scale.x, scale.y, scale.z);
      dglMultMatrix(&mesh->transform);
      glScalef(mesh->scale.x, mesh->scale.y, mesh->scale.z);

      mesh->render(true, true, true, true, getLMHandle(), instance->getLMHandle());

      glPopMatrix();

      lm->sgResetLights();
   }

   // so instance has something to pop...
   glPushMatrix();

   glActiveTextureARB(GL_TEXTURE1_ARB);
   LightManager::sgResetExposureRendering();
   glDisable(GL_TEXTURE_2D);

   glActiveTextureARB(GL_TEXTURE0_ARB);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   glDisable(GL_CULL_FACE);
	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);

   glPolygonOffset(0.0f, 0.0f);
   glDisable(GL_POLYGON_OFFSET_FILL);
}

void Interior::renderAsShape()
{
   OutputPoint* pFirstOutputPoint = (OutputPoint*)sgRenderBuffer;
   U32 currRenderBufferPoint = 0;
   U32 currIndexPoint        = 0;

   glEnable(GL_LIGHTING);
   ColorF foo(1.0f,1.0f,1.0f,0.0f);
   glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, foo);

   // Base texture:
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glBlendFunc(GL_ONE, GL_ZERO);

   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->point);

   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->texCoord);

   if (sgFogActive) {
      // Need the fog coord pointer enabled here...
      glEnableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
      glFogCoordPointerEXT(GL_FLOAT, sizeof(OutputPoint), &pFirstOutputPoint->fogCoord);
   }

   // Ok, our verts are set up, draw our polys.
   U32 currentlyBound0 = U32(-1);
   U32 currentlyBound1 = U32(-1);
   U32 currentTexGen   = U32(-1);

   // Draw the polys!
   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
   for (S32 i = 0; i < mSurfaces.size(); i++)
   {
      const Surface& rSurface = mSurfaces[i];
      glNormal3fv(getPlane(rSurface.planeIndex));

      // Setup the base texture...
      U32 baseName = mMaterialList->getMaterial(rSurface.textureIndex).getGLName();

      //  Always flush.
      currIndexPoint        = 0;
      currRenderBufferPoint = 0;
      flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);

      if (baseName != currentlyBound1)
      {
         glBindTexture(GL_TEXTURE_2D, baseName);
         currentlyBound1 = baseName;
      }

      if (rSurface.texGenIndex != currentTexGen)
      {
         currentTexGen = rSurface.texGenIndex;
         memcpy(texGen0,  &mTexGenEQs[rSurface.texGenIndex], sizeof(F32)*8);
      }

      emitPrimitive(&sgRenderBuffer[currRenderBufferPoint],
                    &sgRenderIndices[currIndexPoint],
                    &mWindings[rSurface.windingStart],
                    rSurface.windingCount,
                    currRenderBufferPoint,
                    &mPoints[0]);

      currRenderBufferPoint += rSurface.windingCount;
      currIndexPoint        += (rSurface.windingCount - 2) * 3;
      flushPrimitives(sgRenderIndices, currIndexPoint, currRenderBufferPoint);
      currIndexPoint        = 0;
      currRenderBufferPoint = 0;

      AssertFatal(currRenderBufferPoint < 512 && currIndexPoint < 2048, "Aw, crap.");
   }
   currIndexPoint        = 0;
   currRenderBufferPoint = 0;

   glDisable(GL_LIGHTING);

   if (sgFogActive) {
      glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
   }

   if (sgFogActive && false)
   {
      glVertexPointer(3, GL_FLOAT, sizeof(ItrPaddedPoint), &mPoints[0].point);

      extern Point2F* sgFogTexCoords;

      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glTexCoordPointer(2, GL_FLOAT, sizeof(Point2F), sgFogTexCoords);
      glBindTexture(GL_TEXTURE_2D, gClientSceneGraph->getFogTexture().getGLName());
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      extern ColorF gInteriorFogColor;
      glColor4f(gInteriorFogColor.red,
         gInteriorFogColor.green,
         gInteriorFogColor.blue,
         1.0f);

      if (dglDoesSupportCompiledVertexArray())
         glLockArraysEXT(0, mPoints.size());
      for (U32 i = 0; i < sgActivePolyListSize; i++)
      {
         const Surface& rSurface = mSurfaces[sgActivePolyList[i]];
         glDrawElements(GL_TRIANGLE_STRIP, rSurface.windingCount, GL_UNSIGNED_INT, &mWindings[rSurface.windingStart]);
      }
      if (dglDoesSupportCompiledVertexArray())
         glUnlockArraysEXT();

      glActiveTextureARB(GL_TEXTURE1_ARB);
      glEnable(GL_TEXTURE_2D);
   }

   glClientActiveTextureARB(GL_TEXTURE1_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glClientActiveTextureARB(GL_TEXTURE0_ARB);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);

   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);

   glDisable(GL_LIGHTING);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);
   glBlendFunc(GL_ONE, GL_ZERO);

   renderStaticMeshesSimple(true, false, 0);
}
