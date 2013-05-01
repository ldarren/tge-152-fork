/*  mkRenderTexture.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/mkRenderTexture.h"
#include "platform/platformGL.h"
#include "math/mRect.h"
#include "dgl/dgl.h"
#include "dgl/gBitmap.h"
#include "core/frameAllocator.h"
#include "console/console.h"

_RenderTextureManager *RenderTextureManager = NULL;

RenderTextureFormat::RenderTextureFormat(RTColorFormat _cf, RTDepthFormat _df, 
                                         RTAntiAliasing _aa, RTStencilFormat _sf, 
                                         U32 _aas, bool _pb, bool _mip)
{
   mColorFormat = _cf;
   mDepthFormat = _df;
   mAntiAliasingFormat = _aa;
   mStencilFormat = _sf;
   
   mNumAASamples = _aas;
   mAllowPbuffer = _pb;
   
   mMip = _mip;
   
   mIsPbuffer = false;
}

RenderTextureFormat::RenderTextureFormat()
{
   mColorFormat = RGBA0;
   mDepthFormat = Depth0;
   mAntiAliasingFormat = None;
   mStencilFormat = Stencil0;
   
   mNumAASamples = 0;
   mAllowPbuffer = false;
   
   mIsPbuffer = false;
}

RenderTexture::RenderTexture()
{
   mInit = false;
   mTextureId = 0;
   mMip = false;
   mWidth = 0;
   mHeight = 0;
   mDepth = false;
}

RenderTexture::~RenderTexture()
{

}

void RenderTexture::bindToUse(U32 samplerNum)
{
   AssertFatal(mInit, "RenderTexture not initialized");
   glActiveTextureARB(GL_TEXTURE0_ARB + samplerNum);
   glBindTexture(GL_TEXTURE_2D, mTextureId);
}

GBitmap* RenderTexture::download()
{
   AssertFatal(mInit, "RenderTexture not initialized");
      
   U8 *pixels = new U8[mWidth * mHeight * 4];
   
   glBindTexture(GL_TEXTURE_2D, mTextureId);
   glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
   glBindTexture(GL_TEXTURE_2D, 0);
   
   GBitmap *bitmap = new GBitmap;
   bitmap->allocateBitmap(mWidth, mHeight, false, GBitmap::RGBA);
   
   for(U32 y = 0; y < mHeight; y++)
      dMemcpy(bitmap->getAddress(0, y), pixels + y * mWidth * 4, U32(mWidth * 4));
      
   delete[] pixels;
            
   return bitmap;
}

void RenderTexture::copyBackBuffer()
{
   Point2I extent = Platform::getWindowSize();
   AssertFatal(mWidth >= extent.x, "RenderTexture not wide enough to copy backbuffer!");
   AssertFatal(mHeight >= extent.y, "RenderTexture not tall enough to copy backbuffer!");
   
   glBindTexture(GL_TEXTURE_2D, mTextureId);
   glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, extent.x, extent.y);
   glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderTexture::kill()
{
   reset();
}

void RenderTexture::resurrect()
{
   init(mWidth, mHeight, mFormat);
}

FBORenderTexture::FBORenderTexture()
{
   mFBO = 0;
   mRB = 0;
}

FBORenderTexture::~FBORenderTexture()
{
   kill();
}

void FBORenderTexture::init(U32 width, U32 height, RenderTextureFormat format)
{
   GLenum texFormat = 0, depthFormat = 0;
   
   glGenFramebuffersEXT(1, &mFBO);
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFBO);
   
   switch (format.mColorFormat)
   {
      case RGB5_A1:
      texFormat = GL_RGB5_A1;
      break;
      
      case RGBA8:
      texFormat = GL_RGBA8;
      break;
   }
   
   switch (format.mDepthFormat)
   {
      case Depth16:
      depthFormat = GL_DEPTH_COMPONENT16_ARB;
      break;
      
      case Depth24:
      depthFormat = GL_DEPTH_COMPONENT24_ARB;
      break;
      
      case Depth32:
      depthFormat = GL_DEPTH_COMPONENT32_ARB;
      break;
   }
   
   if(texFormat != 0)
   {
      glGenTextures(1, &mTextureId);
      Con::printf("Render texture id is %i", mTextureId);
      glBindTexture(GL_TEXTURE_2D, mTextureId);
      
      if(format.mMip)
      {
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glGenerateMipmapEXT(GL_TEXTURE_2D);
         mMip = true;
      }
      else
      {
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         mMip = false;
      }
   
      glTexImage2D(GL_TEXTURE_2D, 0, texFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
   
      glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mTextureId, 0);
   }
   
   if(depthFormat != 0)
   {
      glGenRenderbuffersEXT(1, &mRB);
      glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mRB);
      glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, depthFormat, width, height);
      glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mRB);
   }
   else
   {
      glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
   }
   
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
   
   mInit = true;
   mDepth = depthFormat != 0;
   mWidth = width;
   mHeight = height;
   mFormat = format;
}

void FBORenderTexture::makeActiveTarget()
{
   AssertFatal(mInit, "RenderTexture not initialized");
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFBO);
   if(mDepth)
      glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mRB);
   
   RectI viewport;
   viewport.point.x = 0;
   viewport.point.y = 0;
   viewport.extent.x = mWidth;
   viewport.extent.y = mHeight;
   
   dglSetViewport(viewport);
   glViewport(0, 0, mWidth, mHeight);
}

void FBORenderTexture::finish()
{
   if(mMip)
   {
      glBindTexture(GL_TEXTURE_2D, mTextureId);
      glGenerateMipmapEXT(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, 0);
   }
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
}

void FBORenderTexture::reset()
{
   glDeleteFramebuffersEXT(1, &mFBO);
   if(mDepth)
      glDeleteRenderbuffersEXT(1, &mRB);
   
   glDeleteTextures(1, &mTextureId);
   
   mInit = false;
}

void _RenderTextureManager::create()
{
   AssertFatal(!RenderTextureManager, "RenderTextureManager already created");
   RenderTextureManager = new _RenderTextureManager;
   
   // DO NOT REMOVE!!!
   // DO NOT REMOVE!!!
   // DO NOT REMOVE!!!
   // Is it perefectly clear that you DO NOT REMOVE!!! this code?
   //
   // Okay, I'll explain this one.  For reasons I will NEVER understand,
   // RenderTexture::download fails the first time it is called.  All subsequent
   // times it works.  The first time, nada.  glGetTexImage doesn't touch the pixel
   // data, and doesn't generate an error.  It just flat out fails completely.
   RenderTextureFormat testFormat(RGBA8, Depth0, None, Stencil0, 0, true, false);
   RenderTextureFormat bestFit;
   
   RenderTextureManager->getClosestMatch(testFormat, bestFit);
   RenderTexture *foo = RenderTextureManager->createRenderTexture(128, 128, bestFit);
   GBitmap *baz = foo->download();
   delete baz;
   RenderTextureManager->deleteRenderTexture(foo);
}

void _RenderTextureManager::destroy()
{
   AssertFatal(RenderTextureManager, "No RenderTextureManager created");
   delete RenderTextureManager;
}

_RenderTextureManager::_RenderTextureManager()
{
   mCurRenderTexture = NULL;
}

_RenderTextureManager::~_RenderTextureManager()
{
   kill();
}

void _RenderTextureManager::getClosestMatch(RenderTextureFormat &input, RenderTextureFormat &output)
{
   // Color goes through
   output.mColorFormat = input.mColorFormat;
   
   // Depth goes through
   output.mDepthFormat = input.mDepthFormat;
   
   // Total hax
   if(!input.mAllowPbuffer || (input.mStencilFormat == Stencil0 && input.mAntiAliasingFormat == None))
   {
      // Antialiasing is always nulled
      output.mAntiAliasingFormat = None;
   
      // Stencil format is always nulled
      output.mStencilFormat = Stencil0;
   
      // Samples are always nulled
      output.mNumAASamples = 0;
   }
   else
   {
      output.mIsPbuffer = true;
      
      output.mAntiAliasingFormat = input.mAntiAliasingFormat;
      
      output.mStencilFormat = input.mStencilFormat;
      
      output.mNumAASamples = input.mNumAASamples;
   }
   
   // Pass through mip for now
   output.mMip = input.mMip;
}

RenderTexture* _RenderTextureManager::createRenderTexture(U32 width, U32 height, RenderTextureFormat format)
{
   AssertFatal(isPow2(width) && isPow2(height), "Dimensions must be powers of two!");
   
   width = getNextPow2(width);
   height = getNextPow2(height);
   
   RenderTexture* tex = NULL;
   
   if(!format.mIsPbuffer)
   {
      tex = new FBORenderTexture;
      tex->init(width, height, format);
   
      mRenderTextures.push_back(tex);
   }
   else
   {
      AssertFatal(false, "We don't support pbuffers yet!");
      //tex = new PBRenderTexture;
      //tex->init(width, height, format);
      
      //mRenderTextures.push_back(tex);
   }
   
   return tex;
}

void _RenderTextureManager::renderToTexture(RenderTexture *tex)
{
   if(mCurRenderTexture)
      mCurRenderTexture->finish();
   mCurRenderTexture = tex;
   if(tex)
   {
      tex->makeActiveTarget();
   }
}

void _RenderTextureManager::reset()
{
   for(U32 i = 0; i < mRenderTextures.size(); i++)
   {
      mRenderTextures[i]->reset();
   }
}

void _RenderTextureManager::resurrect()
{
   for(U32 i = 0; i < mRenderTextures.size(); i++)
   {
      mRenderTextures[i]->resurrect();
   }
}

void _RenderTextureManager::kill()
{
   for(U32 i = 0; i < mRenderTextures.size(); i++)
   {
      delete mRenderTextures[i];
   }
   mRenderTextures.clear();
}

void _RenderTextureManager::deleteRenderTexture(RenderTexture *tex)
{
   for(U32 i = 0; i < mRenderTextures.size(); i++)
   {
      if(mRenderTextures[i] == tex)
      {
         delete mRenderTextures[i];
         mRenderTextures.erase(i);
         return;
      }
   }
   
   AssertFatal(false, "Render texture does not exist, what?");
}



