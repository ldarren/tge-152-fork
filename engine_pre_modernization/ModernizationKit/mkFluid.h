/*  mkFluid.h
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#ifndef _MKFLUID_H_
#define _MKFLUID_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

class fluid;
class CameraQuery;
class PlaneF;
class RenderTexture;

/// This is fluid/waterblock's main point of contact with the MK.
/// Okay, that's a lie.  I couldn't extract a lot of stuff easily from the fluid
/// or waterblock classes, esp. in fluid::render.  This stores what I could
/// extract, and serves more to get things out of SceneGraph.
class mkFluid
{ 
   friend class fluid;
   
   /// @name Statics
   ///
   /// @{
   
   static U32 smGlobalReflectionSize;
   
   static bool smGlobalRefraction;
   
   /// @}
   
   U32 mOcclusionQuery;
   
   U32 mNumPixelsRendered;
   
   RenderTexture* mReflectionRT;
   
   /// Reflection size
   U32 mReflectionSize;
   
   /// Do we allow refraction
   bool mDoRefraction;

   /// This renders white to the alpha channel in the area the water occupies.
   ///
   /// @param vertices           A pointer to the vertices we will use for rendering
   /// @param indices            A pointer to the indices we will use for rendering
   /// @param stride             The number of bytes between each vertex position in vertices
   /// @param numIndices         The number of indices we are using
   void renderVisibilityMask(void* vertices, void* indices, U32 stride, U32 numIndices);
   
   /// Loads up the proper material based on if we need a refraction mask if the cam is submerged, if we have
   /// a reflection, and if we refract.
   ///
   /// @param eyeSubmerged             Is the camera submerged?
   /// @param refractionMask           Render the refraction mask?
   bool loadShader(bool eyeSubmerged, bool refractionMask);
   
   public:
   /// Statics
   ///
   /// @{
   
   static void setGlobalReflectionSize(U32 size);
   
   static U32 getGlobalReflectionSize() { return smGlobalReflectionSize; }
   
   static void enableGlobalRefraction(bool refract);
   
   static bool isGlobalRefractive() { return smGlobalRefraction; }
   
   /// @}
   
   mkFluid();
   ~mkFluid();
   
   void prepReflection(CameraQuery *camq, PlaneF plane);
   
   void bindReflectionTexture(U32 sampler);
   
   U32 getPixelsRenderedLastFrame();
   
   /// Set the reflection size
   void setReflectionSize(U32 size);
   
   /// What is our reflection size?
   U32 getReflectionSize() { return mReflectionSize; };
   
   /// To refract or not to refract, that is the question
   void enableRefraction(bool refract);
   
   /// And this is the answer
   bool isRefractive() { return mDoRefraction; };
};

#endif
