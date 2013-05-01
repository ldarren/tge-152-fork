/*  mkDRL.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/mkDRL.h"
#include "ModernizationKit/mkShaderManager.h"
#include "lightingSystem/sgLighting.h"
#include "ModernizationKit/ModernizationKit.h"

_DRL *DRL = NULL;

void _DRL::create()
{
   AssertFatal(!DRL, "DRL already initialized");
   DRL = new _DRL;
}

void _DRL::init()
{
   mInit = true;
   Point2I extent = Platform::getWindowSize();
   U32 width = getNextPow2(extent.x);
   U32 height = getNextPow2(extent.y);
   
   RenderTextureFormat testFormat(RGBA8, Depth0, None, Stencil0, 0, false, false);
   RenderTextureFormat bestFit;
   RenderTextureManager->getClosestMatch(testFormat, bestFit);
   
   mFinalRT = RenderTextureManager->createRenderTexture(width, height, bestFit);
   mBloom0RT = RenderTextureManager->createRenderTexture(width/2, height/2, bestFit);
   mBloom1RT = RenderTextureManager->createRenderTexture(width/2, height/2, bestFit);
   
   mInfo0RT = RenderTextureManager->createRenderTexture(1, 1, bestFit);
   mInfo1RT = RenderTextureManager->createRenderTexture(1, 1, bestFit);
   
   RenderTextureFormat avgInt(RGBA8, Depth0, None, Stencil0, 0, false, false);
   RenderTextureManager->getClosestMatch(avgInt, bestFit);
   
   U32 i = 0;
   while( width > 1 && height > 1)
   {
      width /= 4;
      height /= 4;
      if(width < 1)
         width = 1;
      if(height < 1)
         height = 1;
      mAvgIntPingPongRTs[i] = RenderTextureManager->createRenderTexture(width, height, bestFit);
      i++;
   }
   mAvgIntPingPongIdx = i - 1;
   
   mIsActive = Con::getBoolVariable("$pref::DRL::enable");
   mBloomQuality = Con::getIntVariable("$pref::DRL::bloomQuality");
   ShaderManager->setGlobalParameter("tge_LightingOverbright", mIsActive ? 1.0f : SG_LIGHTING_OVERBRIGHT_AMOUNT);
}

void _DRL::savePrefs()
{
   Con::setBoolVariable("$pref::DRL::enable", mIsActive);
   Con::setIntVariable("$pref::DRL::bloomQuality", mBloomQuality);
}

void _DRL::destroy()
{
   AssertFatal(DRL, "DRL not initialized");
   DRL->savePrefs();
   delete DRL;
}

_DRL::_DRL()
{
   mIsActive = true;
   mInit = false;
   
   mScale = 2.0f;
   mConstant = -0.1f;
   
   mGoalIntensity = 0.5f;
   mAverageIntensity = 0.0f;
   
   mScaleChangeRate = 1.8f;
   mBiasChangeRate = 0.36f;
   mChangeTolerance = 0.03f;
   
   mMaxBias = 0.0f;
   mMinBias = -0.2f;
   
   mMaxScale = 4.0f;
   mMinScale = 1.0f;
   
   mBloomColorOffset = 0.0f;
   mBloomMultiplier = 1.0f;
   mBloomQuality = 3;
   mBloomRadius = 1.0f;
   
   mUseSwapInfo = false;
}

_DRL::~_DRL()
{

}

void _DRL::setActive(bool active)
{
   mIsActive = active;
   ShaderManager->setGlobalParameter("tge_LightingOverbright", mIsActive ? 1.0f : SG_LIGHTING_OVERBRIGHT_AMOUNT);
   Con::setBoolVariable("$pref::DRL::enable", mIsActive);
}

struct DRLVert
{
   Point2F vert;
   Point2F texCoord;
};

void _DRL::render()
{
   if(!mInit)
      init();
   F32 shaderVer = ShaderManager->getPixelShaderVersion();
   bool suppFBO = dglDoesSupportEXTFramebufferObject();
   if(shaderVer < 1.9f || !suppFBO || !mIsActive || MKGFX->isReflectPass())
      return;
   
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glEnable(GL_TEXTURE_2D);
   glDepthMask(GL_FALSE);
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   
   render20();
   
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   
   glBlendFunc(GL_ONE, GL_ZERO);
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix(); 
   glDisable(GL_TEXTURE_2D);
   glDepthMask(GL_TRUE);
}

void _DRL::bindInfoTexture(U32 samplerNum)
{
   if(mUseSwapInfo)
   {
      mInfo1RT->bindToUse(samplerNum);
   }
   else
   {
      mInfo0RT->bindToUse(samplerNum);
   }
}

void _DRL::loadBloomGenShader()
{
   Material *mat = MaterialManager->findMaterialByName("DRLBloomGen");
   if(!mat)
      return;
   mat->bind();
   ShaderManager->setParameter("SubtractiveTerm", mGoalIntensity + mBloomColorOffset);
}

void _DRL::loadBlurShader(bool horiz, U32 extentX, U32 extentY, U32 texWidth, U32 texHeight)
{
   Material *mat = MaterialManager->findMaterialByName("DRLBlur");
   if(!mat || !mat->bind())
      return;
   
   F32 texelWidth = 1.0f/(F32)texWidth;
   F32 texelHeight = 1.0f/(F32)texHeight;
   
   if(horiz)
   {
      ShaderManager->setParameter("yOffsets", 0.0f, 0.0f, 0.0f, 0.0f);
      ShaderManager->setParameter("xOffsets", texelWidth * 0.5f * mBloomRadius, texelWidth * 1.5f * mBloomRadius,
                                              texelWidth * 2.5f * mBloomRadius, texelWidth * 3.5f * mBloomRadius);
   }
   else
   {
      ShaderManager->setParameter("xOffsets", 0.0f, 0.0f, 0.0f, 0.0f);
      ShaderManager->setParameter("yOffsets", texelHeight * 0.5f * mBloomRadius, texelHeight * 1.5f * mBloomRadius,
                                              texelHeight * 2.5f * mBloomRadius, texelHeight * 3.5f * mBloomRadius);
   }
   
   ShaderManager->setParameter("MaxExtents", (F32)extentX/(F32)texWidth - texelWidth, 
                                             (F32)extentY/(F32)texHeight - texelHeight); 
}

void _DRL::loadCompositionShader()
{
   Material *mat;
   if(mBloomQuality >= 2)
   {
      mat = MaterialManager->findMaterialByName("DRLComposition");
   }
   else
   {
      mat = MaterialManager->findMaterialByName("DRLFakeBloom");
   }
   if(!mat)
      return;
   
   mat->bind();
   ShaderManager->setParameter("BloomMultiplier", mBloomMultiplier);
   ShaderManager->setParameter("SubtractiveTerm", mGoalIntensity + mBloomColorOffset);
}

void _DRL::pingPongAvgInt()
{
   DRLVert verts[4];
   U16 indices[6];
   indices[0] = 0;
   indices[1] = 1;
   indices[2] = 2;
   indices[3] = 3;
   indices[4] = 2;
   indices[5] = 3;
   glVertexPointer(2, GL_FLOAT, sizeof(DRLVert), &verts[0].vert);
   glTexCoordPointer(2, GL_FLOAT, sizeof(DRLVert), &verts[0].texCoord);
   verts[0].vert.set(-1.0f, -1.0f);
   verts[0].texCoord.set(0.0f, 0.0f);
   
   verts[1].vert.set(-1.0f, 1.0f);
   verts[1].texCoord.set(0.0f, 1.0f);
   
   verts[2].vert.set(1.0f, 1.0f);
   verts[2].texCoord.set(1.0f, 1.0f);
   
   verts[3].vert.set(1.0f, -1.0f);
   verts[3].texCoord.set(1.0f, 0.0f);
   // mAvgIntPingPongRTs[0] contains data
   for(U32 i = 0; i < mAvgIntPingPongIdx; i++)
   {
      glViewport(0, 0, mAvgIntPingPongRTs[i+1]->getWidth(), mAvgIntPingPongRTs[i+1]->getHeight());
      RenderTextureManager->renderToTexture(mAvgIntPingPongRTs[i+1]);
      mAvgIntPingPongRTs[i]->bindToUse(0);
      glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, &indices[0]);
   }
}

void _DRL::calcInfoTexture()
{
   pingPongAvgInt();
   Material *mat = MaterialManager->findMaterialByName("DRLInfoCalc");
   if(!mat)
   {
      RenderTextureManager->renderToTexture(NULL);
      return;
   }
   if(!mat->bind())
      return;
      
   if(mUseSwapInfo)
   {
      RenderTextureManager->renderToTexture(mInfo0RT);
      mInfo1RT->bindToUse(0);
   }
   else
   {
      RenderTextureManager->renderToTexture(mInfo1RT);
      mInfo0RT->bindToUse(0);
   }
   
   mAvgIntPingPongRTs[mAvgIntPingPongIdx]->bindToUse(1);
   
   ShaderManager->setParameter("GoalIntensity", mGoalIntensity);
   ShaderManager->setParameter("ChangeRates", mScaleChangeRate, mBiasChangeRate);
   ShaderManager->setParameter("ScaleLimits", mMinScale, mMaxScale);
   ShaderManager->setParameter("BiasLimits", mMinBias, mMaxBias);
   ShaderManager->setParameter("ChangeTolerance", mChangeTolerance);
   
   glViewport(0, 0, 1, 1);
   DRLVert verts[4];
   U16 indices[6];
   indices[0] = 0;
   indices[1] = 1;
   indices[2] = 2;
   indices[3] = 3;
   indices[4] = 2;
   indices[5] = 3;
   glVertexPointer(2, GL_FLOAT, sizeof(DRLVert), &verts[0].vert);
   glTexCoordPointer(2, GL_FLOAT, sizeof(DRLVert), &verts[0].texCoord);
   verts[0].vert.set(-1.0f, -1.0f);
   verts[0].texCoord.set(0.0f, 0.0f);
   
   verts[1].vert.set(-1.0f, 1.0f);
   verts[1].texCoord.set(0.0f, 1.0f);
   
   verts[2].vert.set(1.0f, 1.0f);
   verts[2].texCoord.set(1.0f, 1.0f);
   
   verts[3].vert.set(1.0f, -1.0f);
   verts[3].texCoord.set(1.0f, 0.0f);
   
   glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, &indices[0]);
   
   RenderTextureManager->renderToTexture(NULL);
   ShaderManager->unbindShader();
   mUseSwapInfo = !mUseSwapInfo;
}

void _DRL::render20()
{
   Point2I extent = Platform::getWindowSize();
   U32 width = getNextPow2(extent.x);
   U32 height = getNextPow2(extent.y);
   
   if((mFinalRT->getWidth() != width) || (mFinalRT->getHeight() != height))
   {
      RenderTextureFormat testFormat(RGBA8, Depth0, None, Stencil0, 0, false, false);
      RenderTextureFormat bestFit;
      RenderTextureManager->getClosestMatch(testFormat, bestFit);
      
      RenderTextureManager->deleteRenderTexture(mFinalRT);
      RenderTextureManager->deleteRenderTexture(mBloom0RT);
      RenderTextureManager->deleteRenderTexture(mBloom1RT);
      for(U32 i = 0; i < mAvgIntPingPongIdx + 1; i++)
      {
         RenderTextureManager->deleteRenderTexture(mAvgIntPingPongRTs[i]);
      }
      
      mFinalRT = RenderTextureManager->createRenderTexture(width, height, bestFit);
      mBloom0RT = RenderTextureManager->createRenderTexture(width/2, height/2, bestFit);
      mBloom1RT = RenderTextureManager->createRenderTexture(width/2, height/2, bestFit);
      
      RenderTextureFormat avgInt(RGBA8, Depth0, None, Stencil0, 0, false, true);
      RenderTextureManager->getClosestMatch(avgInt, bestFit);
      U32 i = 0;
      while( width > 1 && height > 1)
      {
         width /= 4;
         height /= 4;
         if(width < 1)
            width = 1;
         if(height < 1)
            height = 1;
         mAvgIntPingPongRTs[i] = RenderTextureManager->createRenderTexture(width, height, bestFit);
         i++;
      }
      mAvgIntPingPongIdx = i - 1;
      width = getNextPow2(extent.x);
      height = getNextPow2(extent.y);
   }
   
   F32 texWidth = (F32)width;
   F32 texHeight = (F32)height;
   F32 extentX = (F32)extent.x;
   F32 extentY = (F32)extent.y;
   
   F32 texCoordX = extentX/texWidth;
   F32 texCoordY = extentY/texHeight;
   
   DRLVert verts[4];
   U16 indices[6];
   indices[0] = 0;
   indices[1] = 1;
   indices[2] = 2;
   indices[3] = 3;
   indices[4] = 2;
   indices[5] = 3;
   glVertexPointer(2, GL_FLOAT, sizeof(DRLVert), &verts[0].vert);
   glTexCoordPointer(2, GL_FLOAT, sizeof(DRLVert), &verts[0].texCoord);
   
   // We cheat a lot here to save about 4MB of VRAM
   MKGFX->copyBackBufferToRefractionTexture();
   MKGFX->bindRefractionTexture(0);
   
   verts[0].vert.set(-1.0f, -1.0f);
   verts[0].texCoord.set(0.0f, 0.0f);
   
   verts[1].vert.set(-1.0f, 1.0f);
   verts[1].texCoord.set(0.0f, texCoordY);
   
   verts[2].vert.set(1.0f, 1.0f);
   verts[2].texCoord.set(texCoordX, texCoordY);
   
   verts[3].vert.set(1.0f, -1.0f);
   verts[3].texCoord.set(texCoordX, 0.0f);
   
   if(mBloomQuality >= 2)
   {
      // Render screen into half size texture, and apply DRL scale/bias
      loadBloomGenShader();
      bindInfoTexture(1);
      RenderTextureManager->renderToTexture(mBloom0RT);
      glViewport(0, 0, extent.x/2, extent.y/2);
   
      glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, &indices[0]);
   }
   
   if(mBloomQuality >= 3)
   {
      // Render the halfsize image into another half size texture, applying an eight tap horizontal blur.
      RenderTextureManager->renderToTexture(mBloom1RT);
      mBloom0RT->bindToUse(0);
      loadBlurShader(true, extent.x/2, extent.y/2, width/2, height/2);
      glViewport(0, 0, extent.x/2, extent.y/2);
   
      glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, &indices[0]);
   
      // Render that back into the original RT with an eight tap vertical blur.  We have bloom now, yay!
      RenderTextureManager->renderToTexture(mBloom0RT);
      mBloom1RT->bindToUse(0);
      loadBlurShader(false, extent.x/2, extent.y/2, width/2, height/2);
      glViewport(0, 0, extent.x/2, extent.y/2);
   
      glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, &indices[0]);
   }
   
   // Composite the bloom and screen copy into a render texture.
   // We apply DRL to the screen texture here as well.
   RenderTextureManager->renderToTexture(mFinalRT);
   MKGFX->bindRefractionTexture(0);
   if(mBloomQuality >= 2)
   {
      mBloom0RT->bindToUse(1);
   }
   bindInfoTexture(2);
   loadCompositionShader();
   glViewport(0, 0, extent.x, extent.y);
   
   glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, &indices[0]);
   
   // Bit of cleanup
   glActiveTextureARB(GL_TEXTURE1_ARB);
   glBindTexture(GL_TEXTURE_2D, 0);
   
   glActiveTextureARB(GL_TEXTURE2_ARB);
   glBindTexture(GL_TEXTURE_2D, 0);
   
   ShaderManager->unbindShader();
   
   // Render this into a 1/16 size texture, which we mip down to 1x1 for average intensity computation.
   RenderTextureManager->renderToTexture(mAvgIntPingPongRTs[0]);
   mFinalRT->bindToUse(0);
   glViewport(0, 0, width/4, height/4);
   
   glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, &indices[0]);
   
   // calc the info texture
   calcInfoTexture();
   
   glVertexPointer(2, GL_FLOAT, sizeof(DRLVert), &verts[0].vert);
   glTexCoordPointer(2, GL_FLOAT, sizeof(DRLVert), &verts[0].texCoord);
   
   // Now, render the composited image into the back buffer and we're done!
   RenderTextureManager->renderToTexture(NULL);
   mFinalRT->bindToUse(0);
   glViewport(0, 0, extent.x, extent.y);
   
   glDrawElements(GL_QUADS, 4, GL_UNSIGNED_SHORT, &indices[0]);
   
   return;
}

void _DRL::setGoalIntensity(F32 intensity)
{
   mGoalIntensity = mClampF(intensity, 0.0f, 1.0f);
}

void _DRL::setChangeRates(F32 scaleRate, F32 biasRate)
{
   mScaleChangeRate = scaleRate;
   mBiasChangeRate = biasRate;
}

void _DRL::setChangeTolerance(F32 tolerance)
{
   mChangeTolerance = tolerance;
}

void _DRL::setScaleLimits(F32 min, F32 max)
{
   mMinScale = min;
   mMaxScale = max;
}

void _DRL::setBiasLimits(F32 min, F32 max)
{
   mMinBias = min;
   mMaxBias = max;
}

void _DRL::setBloomColorOffset(F32 offset)
{
   mBloomColorOffset = offset;
}

void _DRL::setBloomMultiplier(F32 multiplier)
{
   mBloomMultiplier = multiplier;
}

void _DRL::setBloomQuality(U32 quality)
{
   mBloomQuality = getMin(quality, U32(3));
}

void _DRL::setBloomRadius(F32 radius)
{
   mBloomRadius = radius;
}

ConsoleStaticMethod(DRL, enable, void, 2, 2, "(bool enable) enable or disable DRL")
{
   DRL->setActive(dAtob(argv[1]));
}

ConsoleStaticMethod(DRL, setGoalIntensity, void, 2, 2, "(float intensity) Sets the goal intensity the DRL effect tries to reach")
{
   DRL->setGoalIntensity(dAtof(argv[1]));
}

ConsoleStaticMethod(DRL, setChangeRates, void, 3, 3, "(float scaleRate, float biasRate) Sets the maximum amount the scale and bias can change in 1 second")
{
   DRL->setChangeRates(dAtof(argv[1]), dAtof(argv[2]));
}

ConsoleStaticMethod(DRL, setChangeTolerance, void, 2, 2, "(float tolerance) Sets a tolerance around the goal intensity where the DRL effect stops adjusting")
{
   DRL->setChangeTolerance(dAtof(argv[1]));
}

ConsoleStaticMethod(DRL, setScaleLimits, void, 3, 3, "(float min, float max) Sets the minimum and maximum values of the scalar")
{
   DRL->setScaleLimits(dAtof(argv[1]), dAtof(argv[2]));
}

ConsoleStaticMethod(DRL, setBiasLimits, void, 3, 3, "(float min, float max) Sets the minimum and maximum values of the bias")
{
   DRL->setBiasLimits(dAtof(argv[1]), dAtof(argv[2]));
}

ConsoleStaticMethod(DRL, setBloomColorOffset, void, 2, 2, "(float offset) Sets the offset from the goal intensity at which blooming kicks in")
{
   DRL->setBloomColorOffset(dAtof(argv[1]));
}

ConsoleStaticMethod(DRL, setBloomMultiplier, void, 2, 2, "(float multiplier) Final color = DRLColor + multiplier * bloom")
{
   DRL->setBloomMultiplier(dAtof(argv[1]));
}

ConsoleStaticMethod(DRL, setBloomQuality, void, 2, 2, "(U32 quality) 0 = None, 1 = Fake, 2 = Minimum, 3 = Blur shader")
{
   U32 quality = dAtoi(argv[1]);
   if(quality > 3)
      quality = 3;
   DRL->setBloomQuality(quality);
   Con::setIntVariable("$pref::DRL::bloomQuality", quality);
}

ConsoleStaticMethod(DRL, setBloomRadius, void, 2, 2, "(F32 radius) sets the bloom radius")
{
   DRL->setBloomRadius(dAtof(argv[1]));
}
