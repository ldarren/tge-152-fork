//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------



#ifndef _PLATFORMGL_H_
#define _PLATFORMGL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define GLAPI extern
#define GLAPIENTRY

#include "platformX86UNIX/gl_types.h"

#define GL_FUNCTION(fn_return,fn_name,fn_args,fn_value) extern fn_return (*fn_name)fn_args; 
#include "platform/GLCoreFunc.h"
#include "platform/GLExtFunc.h"
#undef GL_FUNCTION

// GLU functions are linked at compile time, except in the dedicated server build
#ifndef DEDICATED
#define GL_FUNCTION(fn_return,fn_name,fn_args,fn_value) fn_return fn_name fn_args; 
#else
#define GL_FUNCTION(fn_return,fn_name,fn_args,fn_value) extern fn_return (*fn_name)fn_args; 
#endif
#include "platform/GLUFunc.h"
#undef GL_FUNCTION

namespace GLLoader
{
    bool OpenGLInit();
    void OpenGLShutdown();

    bool OpenGLDLLInit();
    void OpenGLDLLShutdown();
}

/*
 * GL state information.
 */
struct GLState
{
   bool suppARBMultitexture;
   bool suppEXTblendcolor;
   bool suppEXTblendminmax;

   //------------------------------------------------------------------
   // TGE Modernization Kit
   //------------------------------------------------------------------

   bool suppEXTblendsubtract;

   //------------------------------------------------------------------
   // TGE Modernization Kit
   //------------------------------------------------------------------


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

   //------------------------------------------------------------------
   // TGE Modernization Kit
   //------------------------------------------------------------------

   bool suppRectangularTexture;
   bool suppShaderObjects;
   bool suppShadingLanguage;
   bool suppVertexShader;
   bool suppFragmentShader;
   bool suppARBVertexBuffer;
   bool suppFramebufferObject;
   bool suppARBVertexProgram;
   bool suppARBFragmentProgram;
   bool suppATIFragmentShader;
   bool suppATITextFragmentShader;
   bool suppEXTCgShader;

   //------------------------------------------------------------------
   // TGE Modernization Kit
   //------------------------------------------------------------------



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
        return gGLState.suppVertexBuffer;
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

//------------------------------------------------------------------
// TGE Modernization Kit
//------------------------------------------------------------------

inline bool dglDoesSupportRectangularTexture()
{
   return gGLState.suppRectangularTexture;
}

inline bool dglDoesSupportEXTblendsubtract()
{
   return gGLState.suppEXTblendsubtract;
}

inline bool dglDoesSupportVertexShader()
{
   return gGLState.suppVertexShader;
}

inline bool dglDoesSupportFragmentShader()
{
   return gGLState.suppFragmentShader;
}

inline bool dglDoesSupportARBVertexBuffer()
{
   return gGLState.suppARBVertexBuffer;
}

inline bool dglDoesSupportEXTFramebufferObject()
{
   return gGLState.suppFramebufferObject;
}

inline bool dglDoesSupportARBVertexProgram()
{
   return gGLState.suppARBVertexProgram;
}

inline bool dglDoesSupportARBFragmentProgram()
{
   return gGLState.suppARBFragmentProgram;
}

inline bool dglDoesSupportATIFragmentShader()
{
   return gGLState.suppATIFragmentShader;
}

inline bool dglDoesSupportATITextFragmentShader()
{
   return gGLState.suppATITextFragmentShader;
}

inline bool dglDoesSupportCgGLSL()
{
   return gGLState.suppEXTCgShader;
}

//------------------------------------------------------------------
// TGE Modernization Kit
//------------------------------------------------------------------


#ifdef __cplusplus
}
#endif

#endif

