//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------



#include "platformX86UNIX/platformGL.h"
#include "platformX86UNIX/platformX86UNIX.h"
#include "console/console.h"

//-----------------------------------------------
// TGE Modernization Kit
//-----------------------------------------------

#ifndef GL_SHADING_LANGUAGE_VERSION_ARB
#define GL_SHADING_LANGUAGE_VERSION_ARB     0x8B8C
#endif

//-----------------------------------------------
// TGE Modernization Kit
//-----------------------------------------------

#include <dlfcn.h>
#include <SDL/SDL.h>

// declare stub functions
#define GL_FUNCTION(fn_return, fn_name, fn_args, fn_value) fn_return stub_##fn_name fn_args{ fn_value }
#include "platform/GLCoreFunc.h"
#include "platform/GLExtFunc.h"
#undef GL_FUNCTION

// point gl function pointers at stub functions
#define GL_FUNCTION(fn_return,fn_name,fn_args, fn_value) fn_return (*fn_name)fn_args = stub_##fn_name;
#include "platform/GLCoreFunc.h"
#include "platform/GLExtFunc.h"
#undef GL_FUNCTION

static void* dlHandle = NULL;

//------------------------------------------------------------------
//bind functions for each function prototype
//------------------------------------------------------------------

//GL_EXT/ARB
enum {
   ARB_multitexture              = BIT(0),
   ARB_texture_compression       = BIT(1),
   EXT_compiled_vertex_array     = BIT(2),
   EXT_fog_coord                 = BIT(3),
   EXT_paletted_texture          = BIT(4),
   NV_vertex_array_range         = BIT(5),
   EXT_blend_color               = BIT(6),
   EXT_blend_minmax              = BIT(7),
   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 
   ARB_shader_objects            = BIT(8),
   ARB_vertex_shader             = BIT(9),
   ARB_vertex_buffer_object      = BIT(10),
   EXT_framebuffer_object        = BIT(11)
   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 
};

//WGL_ARB
enum {
   WGL_ARB_extensions_string  = BIT(0),
   WGL_EXT_swap_control       = BIT(1),
   WGL_3DFX_gamma_control     = BIT(2)
};


static bool isFnOk( const char *name)
{
   bool ok = false;

   // JMQ: these are specific to torque's d3d->gl wrapper.  They are not used under linux.
   if (dStrcmp(name, "glAvailableVertexBufferEXT")==0)
      ok = true;
   else if (dStrcmp(name, "glAllocateVertexBufferEXT")==0)
      ok = true;
   else if (dStrcmp(name, "glLockVertexBufferEXT")==0)
      ok = true;
   else if (dStrcmp(name, "glUnlockVertexBufferEXT")==0)
      ok = true;
   else if (dStrcmp(name, "glSetVertexBufferEXT")==0)
      ok = true;
   else if (dStrcmp(name, "glOffsetVertexBufferEXT")==0)
      ok = true;
   else if (dStrcmp(name, "glFillVertexBufferEXT")==0)
      ok = true;
   else if (dStrcmp(name, "glFreeVertexBufferEXT")==0)
      ok = true;

   return ok;
}

//------------------------------------------------------------------
//bind functions for each function prototype
//------------------------------------------------------------------
static bool bindGLFunction( void *&fnAddress, const char *name )
{
   void* addr = (void*)SDL_GL_GetProcAddress(name);
   bool ok = (bool)addr;
   if( !ok )
   {
      if (!isFnOk(name))
         Con::errorf(ConsoleLogEntry::General, " Missing OpenGL function '%s'", name);
      else
         ok = true;
   }
   else
      fnAddress = addr;
   return ok;
}

static bool bindEXTFunction( void *&fnAddress, const char *name )
{
   void* addr = (void*)SDL_GL_GetProcAddress(name);
   if( !addr )
      Con::errorf(ConsoleLogEntry::General, " Missing OpenGL extension '%s'", name);
   else
      fnAddress = addr;
   return (addr != NULL);
}

//------------------------------------------------------------------
//binds for each function group
//------------------------------------------------------------------
static bool bindGLFunctions()
{
   bool result = true;
   #define GL_FUNCTION(fn_return, fn_name, fn_args, fn_value) \
   result &= bindGLFunction( *(void**)&fn_name, #fn_name);
   #include "platform/GLCoreFunc.h"
   #undef GL_FUNCTION
   return result;
}

static bool bindEXTFunctions(U32 extMask)
{
   bool result = true;

   #define GL_GROUP_BEGIN( flag ) \
      if( extMask & flag ) {
   #define GL_GROUP_END() }

   #define GL_FUNCTION(fn_return, fn_name, fn_args, fn_value) \
   result &= bindEXTFunction( *(void**)&fn_name, #fn_name);
   #include "platform/GLExtFunc.h"
   #undef GL_FUNCTION

   #undef GL_GROUP_BEGIN
   #undef GL_GROUP_END

   return result;
}

static void unbindGLFunctions()
{
   // point gl function pointers at stub functions
#define GL_FUNCTION(fn_return, fn_name, fn_args, fn_value) fn_name = stub_##fn_name;
#include "platform/GLCoreFunc.h"
#include "platform/GLExtFunc.h"
#undef GL_FUNCTION
}

namespace GLLoader
{

   bool OpenGLInit()
   {
      return OpenGLDLLInit();
   }

   void OpenGLShutdown()
   {
      OpenGLDLLShutdown();
   }

   bool OpenGLDLLInit()
   {
      OpenGLDLLShutdown();

      // load libGL.so
      if (SDL_GL_LoadLibrary("libGL.so") == -1 &&
         SDL_GL_LoadLibrary("libGL.so.1") == -1)
      {
         Con::errorf("Error loading GL library: %s", SDL_GetError());
         return false;
      }

      // bind functions
      if (!bindGLFunctions())
      {
         Con::errorf("Error binding GL functions");
         OpenGLDLLShutdown();
         return false;
      }

      return true;
   }

   void OpenGLDLLShutdown()
   {
      // there is no way to tell SDL to unload the library
      if (dlHandle != NULL)
      {
         dlclose(dlHandle);
         dlHandle = NULL;
      }

      unbindGLFunctions();
   }

}

GLState gGLState;

bool  gOpenGLDisablePT                   = false;
bool  gOpenGLDisableCVA                  = false;
bool  gOpenGLDisableTEC                  = false;
bool  gOpenGLDisableARBMT                = false;
bool  gOpenGLDisableFC                   = false;
bool  gOpenGLDisableTCompress            = false;
bool  gOpenGLNoEnvColor                  = false;
float gOpenGLGammaCorrection             = 0.5;
bool  gOpenGLNoDrawArraysAlpha           = false;

// JMQTODO: really need a platform-shared version of this nastiness
bool GL_EXT_Init( )
{
   gGLState.isDirect3D = false;
	
   // Load extensions...
   //
   const char* pExtString = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
   gGLState.primMode = 0;
   U32 extBitMask = 0;

   // GL_EXT_paletted_texture
   if (pExtString && dStrstr(pExtString, (const char*)"GL_EXT_paletted_texture") != NULL)
   {
      extBitMask |= EXT_paletted_texture;
      gGLState.suppPalettedTexture = true;
   }
   else
      gGLState.suppPalettedTexture = false;

   // EXT_compiled_vertex_array
   if (pExtString && dStrstr(pExtString, (const char*)"GL_EXT_compiled_vertex_array") != NULL)
   {
      extBitMask |= EXT_compiled_vertex_array;
      gGLState.suppLockedArrays = true;
   }
   else
   {
      gGLState.suppLockedArrays = false;
   }

   // ARB_multitexture
   if (pExtString && dStrstr(pExtString, (const char*)"GL_ARB_multitexture") != NULL)
   {
      extBitMask |= ARB_multitexture;
      gGLState.suppARBMultitexture = true;
   } else {
      gGLState.suppARBMultitexture = false;
   }
   
   // EXT_blend_color
   if(pExtString && dStrstr(pExtString, (const char*)"GL_EXT_blend_color") != NULL)
   {
      extBitMask |= EXT_blend_color;
      gGLState.suppEXTblendcolor = true;
   } else {
      gGLState.suppEXTblendcolor = false;
   }

   // EXT_blend_minmax
   if(pExtString && dStrstr(pExtString, (const char*)"GL_EXT_blend_minmax") != NULL)
   {
      //-----------------------------------------------
      // TGE Modernization Kit
      //----------------------------------------------- 
      extBitMask |= EXT_blend_minmax;
      //extBitMask |= EXT_blend_color; // orig
      //-----------------------------------------------
      // TGE Modernization Kit
      //----------------------------------------------- 
      gGLState.suppEXTblendminmax = true;
   } else {
      gGLState.suppEXTblendminmax = false;
   }

   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 
   
   if(pExtString && dStrstr(pExtString, (const char*)"GL_EXT_blend_subtract") != NULL)
   {
      //I'm making the assumption that if subtract is supported, so is minmax.
      gGLState.suppEXTblendsubtract = true;
   } else {
      gGLState.suppEXTblendsubtract = false;
   }
   
   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 


   // EXT_fog_coord
   if (pExtString && dStrstr(pExtString, (const char*)"GL_EXT_fog_coord") != NULL)
   {
      extBitMask |= EXT_fog_coord;
      gGLState.suppFogCoord = true;
   } else {
      gGLState.suppFogCoord = false;
   }

   // EXT_texture_compression_s3tc
   if (pExtString && dStrstr(pExtString, (const char*)"GL_EXT_texture_compression_s3tc") != NULL)
      gGLState.suppS3TC = true;
   else
      gGLState.suppS3TC = false;

   // ARB_texture_compression
   if (pExtString && dStrstr(pExtString, (const char*)"GL_ARB_texture_compression") != NULL)
   {
      extBitMask |= ARB_texture_compression;
      gGLState.suppTextureCompression = true;
   } else {
      gGLState.suppTextureCompression = false;
   }

   // NV_vertex_array_range (not on *nix)
   gGLState.suppVertexArrayRange = false;

   // 3DFX_texture_compression_FXT1
   if (pExtString && dStrstr(pExtString, (const char*)"3DFX_texture_compression_FXT1") != NULL)
      gGLState.suppFXT1 = true;
   else
      gGLState.suppFXT1 = false;

   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 
   if (pExtString && dStrstr(pExtString, (const char*)"GL_ARB_shader_objects") != NULL)
   {
      extBitMask |= ARB_shader_objects;
      gGLState.suppShaderObjects = true;
   }
   else
      gGLState.suppShaderObjects = false;
      
   if (pExtString && dStrstr(pExtString, (const char*)"GL_ARB_shading_language_100") != NULL)
      gGLState.suppShadingLanguage = true;
   else
      gGLState.suppShadingLanguage = false;

   if (pExtString && dStrstr(pExtString, (const char*)"GL_ARB_vertex_shader") != NULL)
   {
      extBitMask |= ARB_vertex_shader;
      gGLState.suppVertexShader = true;
   }
   else
      gGLState.suppVertexShader = false;
      
   if (pExtString && dStrstr(pExtString, (const char*)"GL_ARB_fragment_shader") != NULL)
      gGLState.suppFragmentShader = true;
   else
      gGLState.suppFragmentShader = false;
      
   if (pExtString && dStrstr(pExtString, (const char*)"GL_ARB_vertex_buffer_object") != NULL)
   {
      gGLState.suppARBVertexBuffer = true;
      extBitMask |= ARB_vertex_buffer_object;
   }
   else
      gGLState.suppARBVertexBuffer = false;
      
   if (pExtString && dStrstr(pExtString, (const char*)"GL_EXT_framebuffer_object") != NULL)
   {
      gGLState.suppFramebufferObject = true;
      extBitMask |= EXT_framebuffer_object;
   }
   else
      gGLState.suppFramebufferObject = false;

   if (pExtString && dStrstr(pExtString, (const char*)"GL_ARB_vertex_program") != NULL)
      gGLState.suppARBVertexProgram = true;
   else
      gGLState.suppARBVertexProgram = false;
   
   if (pExtString && dStrstr(pExtString, (const char*)"GL_ATI_text_fragment_shader") != NULL)
      gGLState.suppATITextFragmentShader = true;
   else
      gGLState.suppATITextFragmentShader = false;

   gGLState.suppATIFragmentShader = false;
      
   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 


   if (!bindEXTFunctions(extBitMask))
      Con::warnf("You are missing some OpenGL Extensions.  You may experience rendering problems.");

   // Binary states, i.e., no supporting functions
   // EXT_packed_pixels
   // EXT_texture_env_combine
   //
   // dhc note: a number of these can have multiple matching 'versions', private, ext, and arb.
   gGLState.suppPackedPixels      = pExtString? (dStrstr(pExtString, (const char*)"GL_EXT_packed_pixels") != NULL) : false;
   gGLState.suppTextureEnvCombine = pExtString? (dStrstr(pExtString, (const char*)"GL_EXT_texture_env_combine") != NULL) : false;
   gGLState.suppEdgeClamp         = pExtString? (dStrstr(pExtString, (const char*)"GL_EXT_texture_edge_clamp") != NULL) : false;
   gGLState.suppEdgeClamp        |= pExtString? (dStrstr(pExtString, (const char*)"GL_SGIS_texture_edge_clamp") != NULL) : false;
   gGLState.suppTexEnvAdd         = pExtString? (dStrstr(pExtString, (const char*)"GL_ARB_texture_env_add") != NULL) : false;
   gGLState.suppTexEnvAdd        |= pExtString? (dStrstr(pExtString, (const char*)"GL_EXT_texture_env_add") != NULL) : false;

   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 
   
   gGLState.suppRectangularTexture =  pExtString? (dStrstr(pExtString, (const char*)"GL_ARB_texture_rectangle") != NULL) : false;
   gGLState.suppRectangularTexture |=  pExtString? (dStrstr(pExtString, (const char*)"GL_EXT_texture_rectangle") != NULL) : false;
   gGLState.suppRectangularTexture |=  pExtString? (dStrstr(pExtString, (const char*)"GL_NV_texture_rectangle") != NULL) : false;

   gGLState.suppARBFragmentProgram |= pExtString? (dStrstr(pExtString, (const char*)"GL_ARB_fragment_program") != NULL) : false;
   gGLState.suppEXTCgShader = pExtString? (dStrstr(pExtString, (const char*)"GL_EXT_Cg_shader") != NULL) : false;
   
   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 


   // Anisotropic filtering
   gGLState.suppTexAnisotropic    = pExtString? (dStrstr(pExtString, (const char*)"GL_EXT_texture_filter_anisotropic") != NULL) : false;
   if (gGLState.suppTexAnisotropic)
      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gGLState.maxAnisotropy);
   if (gGLState.suppARBMultitexture)
      glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &gGLState.maxTextureUnits);
   else
      gGLState.maxTextureUnits = 1;

   // JMQ: vsync/swap interval skipped
   gGLState.suppSwapInterval = false;

   Con::printf("OpenGL Init: Enabled Extensions");
   if (gGLState.suppARBMultitexture)    Con::printf("  ARB_multitexture (Max Texture Units: %d)", gGLState.maxTextureUnits);
   if (gGLState.suppEXTblendcolor)        Con::printf("  EXT_blend_color");
   if (gGLState.suppEXTblendminmax)       Con::printf("  EXT_blend_minmax");
   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 
   
   if (gGLState.suppEXTblendsubtract)     Con::printf("  EXT_blend_subtract");
   
   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 
   if (gGLState.suppPalettedTexture)    Con::printf("  EXT_paletted_texture");
   if (gGLState.suppLockedArrays)       Con::printf("  EXT_compiled_vertex_array");
   if (gGLState.suppVertexArrayRange)   Con::printf("  NV_vertex_array_range");
   if (gGLState.suppTextureEnvCombine)  Con::printf("  EXT_texture_env_combine");
   if (gGLState.suppPackedPixels)       Con::printf("  EXT_packed_pixels");
   if (gGLState.suppFogCoord)           Con::printf("  EXT_fog_coord");
   if (gGLState.suppTextureCompression) Con::printf("  ARB_texture_compression");
   if (gGLState.suppS3TC)               Con::printf("  EXT_texture_compression_s3tc");
   if (gGLState.suppFXT1)               Con::printf("  3DFX_texture_compression_FXT1");
   if (gGLState.suppTexEnvAdd)          Con::printf("  (ARB|EXT)_texture_env_add");
   if (gGLState.suppTexAnisotropic)     Con::printf("  EXT_texture_filter_anisotropic (Max anisotropy: %f)", gGLState.maxAnisotropy);
   if (gGLState.suppSwapInterval)       Con::printf("  WGL_EXT_swap_control");

   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 
   
   if (gGLState.suppEXTCgShader)        Con::printf("  EXT_Cg_shader");
   if (gGLState.suppRectangularTexture) Con::printf("  ARB_texture_rectangle");
   if (gGLState.suppARBVertexProgram)   Con::printf("  ARB_vertex_program");
   if (gGLState.suppARBFragmentProgram) Con::printf("  ARB_fragment_program");
   if (gGLState.suppATITextFragmentShader) Con::printf("  ATI_text_fragment_shader");
   if (gGLState.suppShaderObjects)      Con::printf("  ARB_shader_objects");
   if (gGLState.suppShadingLanguage)    Con::printf("  ARB_shading_language (version: %s)", (const char*) glGetString( GL_SHADING_LANGUAGE_VERSION_ARB ));
   if (gGLState.suppVertexShader)       Con::printf("  ARB_vertex_shader");
   if (gGLState.suppFragmentShader)     Con::printf("  ARB_fragment_shader");
   if (gGLState.suppARBVertexBuffer)    Con::printf("  ARB_vertex_buffer_object");
   if (gGLState.suppFramebufferObject)  Con::printf("  EXT_framebuffer_object");
   
   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 

   Con::warnf("OpenGL Init: Disabled Extensions");
   if (!gGLState.suppARBMultitexture)    Con::warnf("  ARB_multitexture");
   if (!gGLState.suppEXTblendcolor)      Con::warnf("  EXT_blend_color");
   if (!gGLState.suppEXTblendminmax)     Con::warnf("  EXT_blend_minmax");
   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 
   
   if (!gGLState.suppEXTblendsubtract)   Con::warnf("  EXT_blend_subtract");
   
   //-----------------------------------------------
   // TGE Modernization Kit
   //-----------------------------------------------  
   
   if (!gGLState.suppPalettedTexture)    Con::warnf("  EXT_paletted_texture");
   if (!gGLState.suppLockedArrays)       Con::warnf("  EXT_compiled_vertex_array");
   if (!gGLState.suppVertexArrayRange)   Con::warnf("  NV_vertex_array_range");
   if (!gGLState.suppTextureEnvCombine)  Con::warnf("  EXT_texture_env_combine");
   if (!gGLState.suppPackedPixels)       Con::warnf("  EXT_packed_pixels");
   if (!gGLState.suppFogCoord)           Con::warnf("  EXT_fog_coord");
   if (!gGLState.suppTextureCompression) Con::warnf("  ARB_texture_compression");
   if (!gGLState.suppS3TC)               Con::warnf("  EXT_texture_compression_s3tc");
   if (!gGLState.suppFXT1)               Con::warnf("  3DFX_texture_compression_FXT1");
   if (!gGLState.suppTexEnvAdd)          Con::warnf("  (ARB|EXT)_texture_env_add");
   if (!gGLState.suppTexAnisotropic)     Con::warnf("  EXT_texture_filter_anisotropic");
   if (!gGLState.suppSwapInterval)       Con::warnf("  WGL_EXT_swap_control");
   //-----------------------------------------------
   // TGE Modernization Kit
   //-----------------------------------------------
   
   if (!gGLState.suppEXTCgShader)        Con::warnf("  EXT_Cg_shader");
   if (!gGLState.suppRectangularTexture) Con::warnf("  ARB_texture_rectangle");
   if (!gGLState.suppARBVertexProgram)   Con::warnf("  ARB_vertex_program");
   if (!gGLState.suppARBFragmentProgram) Con::warnf("  ARB_fragment_program");
   if (!gGLState.suppATITextFragmentShader)  Con::warnf("  ATI_text_fragment_shader");
   if (!gGLState.suppShaderObjects)      Con::warnf("  ARB_shader_objects");
   if (!gGLState.suppShadingLanguage)    Con::warnf("  ARB_shading_language");
   if (!gGLState.suppVertexShader)       Con::warnf("  ARB_vertex_shader");
   if (!gGLState.suppFragmentShader)     Con::warnf("  ARB_fragment_shader");
   if (!gGLState.suppARBVertexBuffer)       Con::warnf("  ARB_vertex_buffer_object");
   if (!gGLState.suppFramebufferObject)  Con::warnf("  EXT_framebuffer_object");
   
   //-----------------------------------------------
   // TGE Modernization Kit
   //-----------------------------------------------
   Con::printf(" ");

   // Set some console variables:
   Con::setBoolVariable( "$FogCoordSupported", gGLState.suppFogCoord );
   Con::setBoolVariable( "$TextureCompressionSupported", gGLState.suppTextureCompression );
   Con::setBoolVariable( "$AnisotropySupported", gGLState.suppTexAnisotropic );
   Con::setBoolVariable( "$PalettedTextureSupported", gGLState.suppPalettedTexture );
   Con::setBoolVariable( "$SwapIntervalSupported", gGLState.suppSwapInterval );

   if (!gGLState.suppPalettedTexture && Con::getBoolVariable("$pref::OpenGL::forcePalettedTexture",false))
   {
      Con::setBoolVariable("$pref::OpenGL::forcePalettedTexture", false);
      Con::setBoolVariable("$pref::OpenGL::force16BitTexture", true);
   }

   return true;
}


