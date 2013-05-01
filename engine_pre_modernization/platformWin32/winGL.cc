//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.

//-----------------------------------------------------------------------------

#include "platformWin32/platformWin32.h"
#include "platformWin32/platformGL.h"
#include "dgl/dgl.h"
#include "platform/platformVideo.h"
#include "core/unicode.h"

#include "console/console.h"
#include "console/consoleTypes.h"

//-----------------------------------------------
// TGE Modernization Kit
//-----------------------------------------------

#ifndef GL_SHADING_LANGUAGE_VERSION_ARB
#define GL_SHADING_LANGUAGE_VERSION_ARB		0x8B8C
#endif

//-----------------------------------------------
// TGE Modernization Kit
//-----------------------------------------------

GLState gGLState;

bool gOpenGLDisablePT         = false;
bool gOpenGLDisableCVA        = false;
bool gOpenGLDisableTEC        = false;
bool gOpenGLDisableARBMT      = false;
bool gOpenGLDisableFC         = false;
bool gOpenGLDisableTCompress  = false;
bool gOpenGLNoEnvColor        = false;
float gOpenGLGammaCorrection  = 0.5;
bool gOpenGLNoDrawArraysAlpha = false;

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


//------------------------------------------------------------------
//create dummy functions and set real functions to dummies for:
// -core GL
// -core WGL
// -WGL extensions
//------------------------------------------------------------------

//defines...
//-------------
// we want to declare/define all GL functions here and set them all to a "stub"
// function so that if they're called before they're initialized, they'll spew
// some console spam to make it easier to debug.  We also need to declare/define
// a "dll" version, which will ALWAYS point to the function defined in the dll.
// This for special functionality like wireframe, logging, and performance metrics
// that will override the normal function and do some additional work.  We'll make
// the dll versions extern so that we can seperate out all the performance/logging
// functionality to a seperate file to keep this one a little bit cleaner.  So,
// our macros look like this for the most part (although a lot uglier in practice):

// type name##_t(params) { console_warning; return ret; }
// type name(params) = name##_t;
// extern type dll##name(params = name##_t

#ifndef TORQUE_LIB

#define GL_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY fn_name##_t)fn_args \
   { Con::printf("Could not load this GL function: %s", #fn_name); fn_body } \
   fn_type (APIENTRY * fn_name)fn_args = fn_name##_t; \
   extern fn_type (APIENTRY * dll##fn_name)fn_args = fn_name##_t;
#define WGL_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY d##fn_name##_t)fn_args \
   { Con::printf("Could not load this WGL function: %s", #fn_name); fn_body } \
   fn_type (APIENTRY * d##fn_name)fn_args = d##fn_name##_t; \
   extern fn_type (APIENTRY * dlld##fn_name)fn_args = d##fn_name##_t;
#define WGLD3D_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY dwgl##fn_name##_t)fn_args \
   { Con::printf("Could not load this WGLD3D function: wgl%s", #fn_name); fn_body } \
   fn_type (APIENTRY * dwgl##fn_name)fn_args = dwgl##fn_name##_t; \
   extern fn_type (APIENTRY * dlldwgl##fn_name)fn_args = dwgl##fn_name##_t;
#define WGLEXT_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY d##fn_name##_t)fn_args \
   { Con::printf("Could not load this WGLEXT function: %s", #fn_name); fn_body } \
   fn_type (APIENTRY * d##fn_name)fn_args = d##fn_name##_t;

#else

#define GL_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY fn_name##_t)fn_args \
   { fn_body } \
   fn_type (APIENTRY * fn_name)fn_args = fn_name##_t; \
   extern fn_type (APIENTRY * dll##fn_name)fn_args = fn_name##_t;
#define WGL_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY d##fn_name##_t)fn_args \
   { fn_body } \
   fn_type (APIENTRY * d##fn_name)fn_args = d##fn_name##_t; \
   extern fn_type (APIENTRY * dlld##fn_name)fn_args = d##fn_name##_t;
#define WGLD3D_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY dwgl##fn_name##_t)fn_args \
   { fn_body } \
   fn_type (APIENTRY * dwgl##fn_name)fn_args = dwgl##fn_name##_t; \
   extern fn_type (APIENTRY * dlldwgl##fn_name)fn_args = dwgl##fn_name##_t;
#define WGLEXT_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_type (APIENTRY d##fn_name##_t)fn_args \
   { fn_body } \
   fn_type (APIENTRY * d##fn_name)fn_args = d##fn_name##_t;

#endif

//includes...
#include "platform/GLCoreFunc.h"
#include "platform/GLExtFunc.h"
#include "platform/GLUFunc.h"
#include "platformWin32/GLWinFunc.h"
#include "platformWin32/GLWinExtFunc.h"

//undefs...
#undef GL_FUNCTION
#undef WGL_FUNCTION
#undef WGLD3D_FUNCTION
#undef WGLEXT_FUNCTION

// These functions won't be in the normal OGL dll, so don't give
// errors about them because we know we'll be ok without them
static bool isFnOk(const char* name)
{
   if (dStrcmp(name, "glAvailableVertexBufferEXT") == 0 ||
      dStrcmp(name, "glAllocateVertexBufferEXT") == 0  ||
      dStrcmp(name, "glLockVertexBufferEXT") == 0 ||
      dStrcmp(name, "glUnlockVertexBufferEXT") == 0 ||
      dStrcmp(name, "glSetVertexBufferEXT") == 0 ||
      dStrcmp(name, "glOffsetVertexBufferEXT") == 0 ||
      dStrcmp(name, "glFillVertexBufferEXT") == 0 ||
      dStrcmp(name, "glFreeVertexBufferEXT") == 0)
      return true;
   return false;
}

//------------------------------------------------------------------
//bind functions for each function prototype
//------------------------------------------------------------------
static bool bindGLFunction( void *&fnAddress, const char *name )
{
   void* addr = (void*)(GetProcAddress( winState.hinstOpenGL, name ));
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

static bool bindGLUFunction( void *&fnAddress, const char *name )
{
   void* addr =  (void*)(GetProcAddress( winState.hinstGLU, name ));
   if( !addr )
      Con::errorf(ConsoleLogEntry::General, " Missing GLU function '%s'", name);
   else
      fnAddress = addr;
   return (addr != NULL);
}

static bool bindEXTFunction( void *&fnAddress, const char *name )
{
   void* addr = (void*)(dwglGetProcAddress( name ));
   if( !addr )
      Con::errorf(ConsoleLogEntry::General, " Missing OpenGL extension '%s'", name);
   else
      fnAddress = addr;
   return (addr != NULL);
}

static bool bindWGLFunction( void *&fnAddress, const char *name )
{
   void* addr = (void*)(GetProcAddress( winState.hinstOpenGL, name ));
   if( !addr )
      Con::errorf(ConsoleLogEntry::General, " Missing WGL function '%s'", name);
   else
      fnAddress = addr;
   return (addr != NULL);
}

static bool bindWGLEXTFunction( void *&fnAddress, const char *name )
{
   void* addr = (void*)(dwglGetProcAddress( name ));
   if( !addr )
      Con::errorf(ConsoleLogEntry::General, " Missing WGLEXT function '%s'", name);
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
   result &= bindGLFunction( *(void**)&dll##fn_name, #fn_name); \
   fn_name = dll##fn_name;
   #include "platform/GLCoreFunc.h"
   #undef GL_FUNCTION
   return result;
}

static bool bindGLUFunctions()
{
   bool result = true;
   #define GL_FUNCTION(fn_return, fn_name, fn_args, fn_value) \
   result &= bindGLUFunction( *(void**)&dll##fn_name, #fn_name); \
   fn_name = dll##fn_name;
   #include "platform/GLUFunc.h"
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
   result &= bindEXTFunction( *(void**)&dll##fn_name, #fn_name); \
   fn_name = dll##fn_name;
   #include "platform/GLExtFunc.h"
   #undef GL_FUNCTION

   #undef GL_GROUP_BEGIN
   #undef GL_GROUP_END

   return result;
}

static bool bindWGLFunctions(const char* prefix)
{
   //ugh... the stupid D3D wrapper prefixes some functions
   //with either wd3d or wgl, so we have to account for that
   static char buff[200];
   bool result = true;
   #define WGLD3D_FUNCTION(fn_return, fn_name, fn_args, fn_value) \
   dSprintf(buff, 200, "%s%s", prefix, #fn_name); \
   result &= bindWGLFunction( *(void**)&dlldwgl##fn_name, buff); \
   dwgl##fn_name = dlldwgl##fn_name;
   #define WGL_FUNCTION(fn_return, fn_name, fn_args, fn_valud) \
   result &= bindWGLFunction( *(void**)&dlld##fn_name, #fn_name); \
   d##fn_name = dlld##fn_name;
   #include "platformWin32/GLWinFunc.h"
   #undef WGLD3D_FUNCTION
   #undef WGL_FUNCTION

   return result;
}

static bool bindWGLEXTFunctions(U32 extMask)
{
   bool result = true;

   #define WGL_GROUP_BEGIN( flag ) \
      if( extMask & flag ) {
   #define WGL_GROUP_END() }

   #define WGLEXT_FUNCTION(fn_return, fn_name, fn_args, fn_value) result &= bindWGLEXTFunction( *(void**)&d##fn_name, #fn_name);
   #include "platformWin32/GLWinExtFunc.h"
   #undef WGLEXT_FUNCTION

   #undef WGL_GROUP_BEGIN
   #undef WGL_GROUP_END

   return result;
}

//------------------------------------------------------------------
//unbind functions
//------------------------------------------------------------------

//GL core, GL_EXT/ARB, and GLU can all be done in one shot
static void unbindGLFunctions()
{
#define GL_FUNCTION(fn_type, fn_name, fn_args, fn_body) fn_name = dll##fn_name = fn_name##_t;
#include "platform/GLCoreFunc.h"
#include "platform/GLExtFunc.h"
#include "platform/GLUFunc.h"
#undef GL_FUNCTION
}

static void unbindWGLFunctions()
{
#define WGL_FUNCTION(fn_type, fn_name, fn_args, fn_body) d##fn_name = dlld##fn_name = d##fn_name##_t;
#define WGLD3D_FUNCTION(fn_type, fn_name, fn_args, fn_body) dwgl##fn_name = dlldwgl##fn_name = dwgl##fn_name##_t;
#include "platformWin32/GLWinFunc.h"
#undef WGLD3D_FUNCTION
#undef WGL_FUNCTION
}

static void unbindWGLEXTFunctions()
{
#define WGLEXT_FUNCTION(fn_type, fn_name, fn_args, fn_body) d##fn_name = d##fn_name##_t;
#include "platformWin32/GLWinExtFunc.h"
#undef WGLEXT_FUNCTION
}

//------------------------------------------------------------------
// GL_Shutdown - unbind all functions and unload libraries
//------------------------------------------------------------------
void GL_Shutdown( void )
{
   if ( winState.hinstOpenGL )
      FreeLibrary( winState.hinstOpenGL );
   winState.hinstOpenGL = NULL;

   if ( winState.hinstGLU )
      FreeLibrary( winState.hinstGLU );
   winState.hinstGLU = NULL;

   unbindGLFunctions(); //we can do GL, GLU, and EXTs in the same function
   unbindWGLFunctions();//these have to be seperate
   unbindWGLEXTFunctions();

   gGLState.suppSwapInterval = false;
}

//---------------------------------------------------------
// GL_Init - load OpenGL library and bind core GL/GLU/WGL functions
//---------------------------------------------------------
bool GL_Init( const char *dllname_gl, const char *dllname_glu )
{
   if ( winState.hinstOpenGL && winState.hinstGLU)
      return true;

#ifdef UNICODE
   UTF16 dn_gl[1024], dn_glu[1024];
   convertUTF8toUTF16((UTF8 *)dllname_gl, dn_gl, sizeof(dn_gl));
   convertUTF8toUTF16((UTF8 *)dllname_glu, dn_glu, sizeof(dn_glu));
#endif
   // Load OpenGL DLL
   if (!winState.hinstOpenGL)
   {
#ifdef UNICODE
       if ( ( winState.hinstOpenGL = LoadLibrary( dn_gl ) ) == 0 )
#else
      if ( ( winState.hinstOpenGL = LoadLibrary( dllname_gl ) ) == 0 )
#endif
         return false;
   }

   // Load OpenGL GLU DLL
   if ( !winState.hinstGLU )
   {
#ifdef UNICODE
       if ( ( winState.hinstGLU = LoadLibrary( dn_glu ) ) == 0 )
#else
      if ( ( winState.hinstGLU = LoadLibrary( dllname_glu ) ) == 0 )
#endif
         return false;
   }

   if (!bindGLFunctions())
      Con::errorf("You are missing some OpenGL functions.  That's bad.");
   if (!bindGLUFunctions())
      Con::errorf("You are missing some GLU functions.  That's bad.");
   // these will have already been bound thru the OGL version
   if (dStrstr(dllname_gl, "d3d") == NULL)
   {
      if (!bindWGLFunctions("wgl"))
         Con::errorf("You are missing some WGL Functions.  That's REALLY bad.");
   }
   else
      if (!bindWGLFunctions("wd3d"))
         Con::errorf("You are missing some WGL Functions.  That's REALLY bad.");


   return true;
}


//---------------------------------------------------------
// GL_EXT_Init - Initialize all GL/WGL extensions
//---------------------------------------------------------
bool GL_EXT_Init( )
{
   const char* d3dinfo = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
   gGLState.isDirect3D = d3dinfo? (dStrstr(d3dinfo, (const char*)"Direct3D") != NULL) : false;
   
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

   // NV_vertex_array_range
   if (pExtString && dStrstr(pExtString, (const char*)"NV_vertex_array_range") != NULL)
   {
      extBitMask |= NV_vertex_array_range;
      gGLState.suppVertexArrayRange = true;
   }
   else
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
      Con::warnf("You are missing some OpenGL Extensions.  This is bad.");

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

   //----------------------

   //WGL extensions....
   U32 wglExtMask = 0;

   // Swap interval
   if (pExtString && dStrstr(pExtString, (const char*)"WGL_EXT_swap_control") != NULL)
   {
      wglExtMask |= WGL_EXT_swap_control;
      gGLState.suppSwapInterval = true;
   }
   else
   {
      gGLState.suppSwapInterval = false;
   }

   if (!bindWGLEXTFunctions(wglExtMask))
   {
      Con::warnf("You are missing some WGLEXT Functions.  That's possibly VERY bad.");
   }

   Con::printf("OpenGL Init: Enabled Extensions");
   if (gGLState.suppARBMultitexture)      Con::printf("  ARB_multitexture (Max Texture Units: %d)", gGLState.maxTextureUnits);
   if (gGLState.suppEXTblendcolor)        Con::printf("  EXT_blend_color");
   if (gGLState.suppEXTblendminmax)       Con::printf("  EXT_blend_minmax");
   
   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 
   
   if (gGLState.suppEXTblendsubtract)     Con::printf("  EXT_blend_subtract");
   
   //-----------------------------------------------
   // TGE Modernization Kit
   //----------------------------------------------- 
   
   if (gGLState.suppPalettedTexture)      Con::printf("  EXT_paletted_texture");
   if (gGLState.suppLockedArrays)         Con::printf("  EXT_compiled_vertex_array");
   if (gGLState.suppVertexArrayRange)     Con::printf("  NV_vertex_array_range");
   if (gGLState.suppTextureEnvCombine)    Con::printf("  EXT_texture_env_combine");
   if (gGLState.suppPackedPixels)         Con::printf("  EXT_packed_pixels");
   if (gGLState.suppFogCoord)             Con::printf("  EXT_fog_coord");
   if (gGLState.suppTextureCompression)   Con::printf("  ARB_texture_compression");
   if (gGLState.suppS3TC)                 Con::printf("  EXT_texture_compression_s3tc");
   if (gGLState.suppFXT1)                 Con::printf("  3DFX_texture_compression_FXT1");
   if (gGLState.suppTexEnvAdd)            Con::printf("  (ARB|EXT)_texture_env_add");
   if (gGLState.suppTexAnisotropic)       Con::printf("  EXT_texture_filter_anisotropic (Max anisotropy: %g)", gGLState.maxAnisotropy);
   if (gGLState.suppSwapInterval)         Con::printf("  WGL_EXT_swap_control");
   if (gGLState.maxFSAASamples)           Con::printf("  ATI_FSAA");
   // Mac only? ^^^^^^^^^^^^^^

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
   
   GLint tUnits;
   
   if(gGLState.suppARBVertexProgram)
   {
      Con::printf("ARB_vertex_program information");
      
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_INSTRUCTIONS_ARB, &tUnits);
      Con::printf("  Max vertex program instructions: %i", tUnits);
      
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_INSTRUCTIONS_ARB, &tUnits);
      Con::printf("  Max vertex program native instructions: %i", tUnits);
      
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_TEMPORARIES_ARB, &tUnits);
      Con::printf("  Max vertex program temporaries: %i", tUnits);
      
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEMPORARIES_ARB, &tUnits);
      Con::printf("  Max vertex program native temporaries: %i", tUnits);
      
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_PARAMETERS_ARB, &tUnits);
      Con::printf("  Max vertex program parameters: %i", tUnits);
      
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_PARAMETERS_ARB, &tUnits);
      Con::printf("  Max vertex program native parameters: %i", tUnits);
      
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ATTRIBS_ARB, &tUnits);
      Con::printf("  Max vertex program attributes: %i", tUnits);
      
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ATTRIBS_ARB, &tUnits);
      Con::printf("  Max vertex program native attributes: %i", tUnits);
      
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ADDRESS_REGISTERS_ARB, &tUnits);
      Con::printf("  Max vertex program address registers: %i", tUnits);
      
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB, &tUnits);
      Con::printf("  Max vertex program native address registers: %i", tUnits);
      
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB, &tUnits);
      Con::printf("  Max vertex program local parameters: %i", tUnits);
      
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB, &tUnits);
      Con::printf("  Max vertex program env parameters: %i", tUnits);
      
      glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB, &tUnits);
      Con::printf("  Max vertex attributes: %i", tUnits);
      
      glGetIntegerv(GL_MAX_PROGRAM_MATRICES_ARB, &tUnits);
      Con::printf("  Max vertex program matrices: %i", tUnits);
      
      glGetIntegerv(GL_MAX_PROGRAM_MATRIX_STACK_DEPTH_ARB, &tUnits);
      Con::printf("  Max vertex program matrix stack depth: %i", tUnits);
   }
   
   if (gGLState.suppVertexShader)
   {
      Con::printf("ARB_vertex_shader information");
      
      glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB, &tUnits);
      Con::printf("  Max vertex shader texture units: %i", tUnits);
      glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB, &tUnits);
      Con::printf("  Max combined tex. image units: %i", tUnits);
      glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS_ARB, &tUnits);
      Con::printf("  Max tex. image units: %i", tUnits);
      glGetIntegerv(GL_MAX_TEXTURE_COORDS_ARB, &tUnits);
      Con::printf("  Max texture coords: %i", tUnits);
      glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB, &tUnits);
      Con::printf("  Max uniform vertex components: %i", tUnits);
      glGetIntegerv(GL_MAX_VARYING_FLOATS_ARB, &tUnits);
      Con::printf("  Max varying floats: %i", tUnits);
      glGetIntegerv(GL_MAX_VERTEX_ATTRIBS_ARB, &tUnits);
      Con::printf("  Max vertex attribs: %i", tUnits);
   }
   if(gGLState.suppARBFragmentProgram)
   {
      Con::printf("ARB_fragment_program information");
      
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_PARAMETERS_ARB, &tUnits);
      Con::printf("  Max fragment program parameters: %i", tUnits);
      
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_LOCAL_PARAMETERS_ARB, &tUnits);
      Con::printf("  Max fragment program local parameters: %i", tUnits);
      
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_ENV_PARAMETERS_ARB, &tUnits);
      Con::printf("  Max fragment program env parameters: %i", tUnits);
      
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_ALU_INSTRUCTIONS_ARB, &tUnits);
      Con::printf("  Max fragment program ALU instructions: %i", tUnits);
      
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB, &tUnits);
      Con::printf("  Max fragment program native ALU instructions: %i", tUnits);
      
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_TEX_INSTRUCTIONS_ARB, &tUnits);
      Con::printf("  Max fragment program texture instructions: %i", tUnits);
      
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB, &tUnits);
      Con::printf("  Max fragment program native texture instructions: %i", tUnits);
      
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_TEX_INDIRECTIONS_ARB, &tUnits);
      Con::printf("  Max fragment program texture indirections: %i", tUnits);
      
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_MAX_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB, &tUnits);
      Con::printf("  Max fragment program native texture indirections: %i", tUnits);
   }
      
   if (gGLState.suppFragmentShader)
   {
      Con::printf("ARB_fragment_shader information");
      glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS_ARB, &tUnits);
      Con::printf("  Max fragment uniform components: %i", tUnits);
   }
   
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
   if (!gGLState.suppSwapInterval)       Con::warnf("  Vertical Sync");
   if (!gGLState.maxFSAASamples)         Con::warnf("  ATI_FSAA");
   // Mac only?  ^^^^^^^^^^^^^^
   
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
   
   Con::printf("");

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

