//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _PLATFORMGL_H_
#define _PLATFORMGL_H_

/// We use the standard Apple OpenGL framework headers.
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>

/// Using aglMacro.h gives us signifigant performance gains in opengl immediate mode ( glVertex etc )
#if !defined(TORQUE_DEBUG)
#define USE_AGL_MACRO
#endif

#if defined(USE_AGL_MACRO)
#if !defined(AGLContext)
typedef struct __AGLContextRec       *AGLContext;
#endif
#include <AGL/aglMacro.h>
extern AGLContext agl_ctx;
#endif

/// Allows outline mode drawing via a function pointer swapping trick.
/// Must be included AFTER all the OpenGL headers.
#include "platformMacCarb/macCarbOutlineGL.h"


//------------------------------------------------------------------------------
/// This is legacy stuff for the d3d wrapper layer.
// The code that requires these stubs should probably be ifdef'd out of any non w32 build
//------------------------------------------------------------------------------
extern GLboolean glAvailableVertexBufferEXT();
extern GLint glAllocateVertexBufferEXT(GLsizei size, GLint format, GLboolean preserve);
extern void* glLockVertexBufferEXT(GLint handle, GLsizei size);
extern void glUnlockVertexBufferEXT(GLint handle);
extern void glSetVertexBufferEXT(GLint handle);
extern void glOffsetVertexBufferEXT(GLint handle, GLuint offset);
extern void glFillVertexBufferEXT(GLint handle, GLint first, GLsizei count);
extern void glFreeVertexBufferEXT(GLint handle);
// these are extensions for glAllocateVertexBufferEXT
#define GL_V12MTVFMT_EXT                      0x8702
#define GL_V12MTNVFMT_EXT                     0x8703
#define GL_V12FTVFMT_EXT                      0x8704
#define GL_V12FMTVFMT_EXT                     0x8705
//------------------------------------------------------------------------------

// make sure this is defined, as we need to use it when around.
// some versions of OSX only define the SGIS version ( same hexcode )
#ifndef GL_CLAMP_TO_EDGE_EXT
#define GL_CLAMP_TO_EDGE_EXT                     0x812F
#endif

/// GL state information.
struct GLState
{
   bool suppARBMultitexture;
   bool suppEXTblendcolor;
   bool suppEXTblendminmax;
   bool suppPackedPixels;
   bool suppTexEnvAdd;
   bool suppLockedArrays;

   bool suppTextureEnvCombine;
   bool suppVertexArrayRange;
   bool suppFogCoord;
   bool suppEdgeClamp;

   bool suppTextureCompression;
   bool suppS3TC;
   bool suppFXT1;
   bool suppTexAnisotropic;

   bool suppPalettedTexture;
   bool suppVertexBuffer;
   bool suppSwapInterval;

   GLint maxFSAASamples;

   unsigned int triCount[4];
   unsigned int primCount[4];
   unsigned int primMode; // 0-3

   GLfloat maxAnisotropy;
   GLint   maxTextureUnits;
   
   bool isDirect3D;
};

extern GLState gGLState;

extern bool gOpenGLDisablePT;
extern bool gOpenGLDisableCVA;
extern bool gOpenGLDisableTEC;
extern bool gOpenGLDisableARBMT;
extern bool gOpenGLDisableFC;
extern bool gOpenGLDisableTCompress;
extern bool gOpenGLNoEnvColor;
extern float gOpenGLGammaCorrection;
extern bool gOpenGLNoDrawArraysAlpha;

//------------------------------------------------------------------------------
/// Inline state getters for dgl
//------------------------------------------------------------------------------
inline void dglSetRenderPrimType(unsigned int type)
{
   gGLState.primMode = type;
}

inline void dglClearPrimMetrics()
{
   for(int i = 0; i < 4; i++)
      gGLState.triCount[i] = gGLState.primCount[i] = 0;
}

inline bool dglDoesSupportPalettedTexture()
{
   return gGLState.suppPalettedTexture && (gOpenGLDisablePT == false);
}

inline bool dglDoesSupportCompiledVertexArray()
{
   return gGLState.suppLockedArrays && (gOpenGLDisableCVA == false);
}

inline bool dglDoesSupportTextureEnvCombine()
{
   return gGLState.suppTextureEnvCombine && (gOpenGLDisableTEC == false);
}

inline bool dglDoesSupportARBMultitexture()
{
   return gGLState.suppARBMultitexture && (gOpenGLDisableARBMT == false);
}

inline bool dglDoesSupportEXTBlendColor()
{
   return gGLState.suppEXTblendcolor;
}

inline bool dglDoesSupportEXTBlendMinMax()
{
   return gGLState.suppEXTblendminmax;
}

inline bool dglDoesSupportVertexArrayRange()
{
   return gGLState.suppVertexArrayRange;
}

inline bool dglDoesSupportFogCoord()
{
   return gGLState.suppFogCoord && (gOpenGLDisableFC == false);
}

inline bool dglDoesSupportEdgeClamp()
{
   return gGLState.suppEdgeClamp;
}

inline bool dglDoesSupportTextureCompression()
{
   return gGLState.suppTextureCompression && (gOpenGLDisableTCompress == false);
}

inline bool dglDoesSupportS3TC()
{
   return gGLState.suppS3TC;
}

inline bool dglDoesSupportFXT1()
{
   return gGLState.suppFXT1;
}

inline bool dglDoesSupportTexEnvAdd()
{
   return gGLState.suppTexEnvAdd;
}

inline bool dglDoesSupportTexAnisotropy()
{
   return gGLState.suppTexAnisotropic;
}

inline bool dglDoesSupportVertexBuffer()
{
   return false;
}

inline GLfloat dglGetMaxAnisotropy()
{
   return gGLState.maxAnisotropy;
}

inline GLint dglGetMaxTextureUnits()
{
   if (dglDoesSupportARBMultitexture())
      return gGLState.maxTextureUnits;
   else
      return 1; 
}

//------------------------------------------------------------------------------
/// For radeon cards we can do fast FSAA mode switching.
/// Sets Full Scene Anti-Aliasing (FSAA) samples ( 1x, 2x, 4x ) via aglSetInteger()
//------------------------------------------------------------------------------
#define ATI_FSAA_LEVEL ((unsigned long)510)  
void dglSetFSAASamples(GLint aasamp);

#endif // _PLATFORMGL_H_
