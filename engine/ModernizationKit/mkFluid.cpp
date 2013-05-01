/*  mkFluid.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/mkFluid.h"
#include "platform/platform.h"
#include "platform/platformGL.h"
#include "ModernizationKit/ModernizationKit.h"
#include "gui/core/guiTSControl.h"

// Statics
U32 mkFluid::smGlobalReflectionSize = 0;
bool mkFluid::smGlobalRefraction = false;

void mkFluid::setGlobalReflectionSize(U32 size)
{
   smGlobalReflectionSize = getNextPow2(size);
}

void mkFluid::enableGlobalRefraction(bool refract)
{
   smGlobalRefraction = refract;
}

void mkFluid::setReflectionSize(U32 size)
{
   mReflectionSize = getNextPow2(mClamp(size, 1, getGlobalReflectionSize()));
   if(mReflectionRT && mReflectionRT->getWidth() != mReflectionSize)
   {
      RenderTextureManager->deleteRenderTexture(mReflectionRT);
      mReflectionRT = NULL;
   }
   if(!mReflectionRT && mReflectionSize > 1)
   {
      RenderTextureFormat testFormat(RGBA8, Depth24, None, Stencil0, 0, false, false);
      RenderTextureFormat bestFit;
      RenderTextureManager->getClosestMatch(testFormat, bestFit);
      
      mReflectionRT = RenderTextureManager->createRenderTexture(mReflectionSize, mReflectionSize, bestFit);
   }
}

void mkFluid::enableRefraction(bool refract)
{
   mDoRefraction = mClamp((U32)refract, 0, (U32)isGlobalRefractive());
}

// Not statics

mkFluid::mkFluid()
{
   mReflectionSize = getGlobalReflectionSize();
   mDoRefraction = isGlobalRefractive();
   mNumPixelsRendered = 1000; // Why not?
   mOcclusionQuery = 0;
   mReflectionRT = NULL;
}

mkFluid::~mkFluid()
{

}

void mkFluid::prepReflection(CameraQuery *camq, PlaneF plane)
{
   // Create/update the RT if needed
   setReflectionSize(mReflectionSize);
   MKGFX->prepRenderPlanarReflection(camq, plane, mReflectionRT);
}

void mkFluid::bindReflectionTexture(U32 sampler)
{
   if(!mReflectionRT)
      return;
   
   mReflectionRT->bindToUse(sampler);
}
 
U32 mkFluid::getPixelsRenderedLastFrame()
{
   return mNumPixelsRendered;
}

void mkFluid::renderVisibilityMask(void* vertices, void* indices, U32 stride, U32 numIndices)
{   
   if(!loadShader(false, true))
      return;
   
   glEnableClientState ( GL_VERTEX_ARRAY );
   glVertexPointer     ( 3, GL_FLOAT, stride, vertices );
   glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
   glDrawElements      ( GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indices );
   glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
   dglSetCanonicalState();
}

bool mkFluid::loadShader(bool eyeSubmerged, bool refractionMask)
{
   if(refractionMask)
   {
      Material *mat = MaterialManager->findMaterialByName("waterRefractionMask");
      if(!mat)
         return false;
      return mat->bind();
   }
   else if(eyeSubmerged && mDoRefraction)
   {
      Material *mat = MaterialManager->findMaterialByName("waterSubmerged");
      if(!mat)
         return false;
      if(!mat->bind())
         return false;
         
      Point2I extent = Platform::getWindowSize();
      F32 texWidth = getNextPow2((F32)extent.x);
      F32 texHeight = getNextPow2((F32)extent.y);
      
      ShaderManager->setParameter("RefractTexSize", texWidth, texHeight);
      ShaderManager->setParameter("TexRatio", (F32)extent.x/texWidth, (F32)extent.y/texHeight);
      return true;
   }
   else if(mDoRefraction)
   {
      if(mReflectionSize <= 1)
         return false;
      Material *mat = MaterialManager->findMaterialByName("waterReflectRefract");
      if(!mat)
         return false;
      if(!mat->bind())
         return false;
      
      Point2I extent = Platform::getWindowSize();
      F32 texWidth = getNextPow2((F32)extent.x);
      F32 texHeight = getNextPow2((F32)extent.y);
      
      ShaderManager->setParameter("ReflectTexSize", (F32)mReflectionSize);
      ShaderManager->setParameter("RefractTexSize", texWidth, texHeight);
      ShaderManager->setParameter("TexRatio", (F32)extent.x/texWidth, (F32)extent.y/texHeight);
      return true;
   }
   else
   {
      if(mReflectionSize <= 1)
         return false;
      Material *mat = MaterialManager->findMaterialByName("waterReflectOnly");
      if(!mat)
         return false;
      if(!mat->bind())
         return false;
         
      ShaderManager->setParameter("ReflectTexSize", (F32)mReflectionSize);
      return true;
   }
}

ConsoleStaticMethod(MKFluid, setReflectionSize, void, 2, 2, "(U32 size)")
{
   U32 size = dAtoi(argv[1]);
   if(size == 0)
      size++;
   size = getNextPow2(size);
   mkFluid::setGlobalReflectionSize(size);
   Con::setIntVariable("$pref::Water::reflectionSize", size);
}

ConsoleStaticMethod(MKFluid, enableRefraction, void, 2, 2, "(bool enable)")
{
   mkFluid::enableGlobalRefraction(dAtob(argv[1]));
   Con::setBoolVariable("$pref::Water::Refract", dAtob(argv[1]));
}


