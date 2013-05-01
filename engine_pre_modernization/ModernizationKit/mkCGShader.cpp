/*  mkCGShader.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */
 
#include "ModernizationKit/mkShaderManager.h"
#ifdef SHADER_MANAGER_USE_CG
#include "console/console.h"
#include "platform/platform.h"
#include "core/resManager.h"
#include "core/stringTable.h"
#include "core/frameAllocator.h" 
 
#define SHADER_TYPE_PIX_SIZE_CG 4
#define SHADER_TYPE_VERT_SIZE_CG 4 
 
static const char* cgExtArrayPix[SHADER_TYPE_PIX_SIZE_CG] = {"FP40.cgsh", "FP30.cgsh", "ARBF.cgsh", "FP20.cgsh"};
static const char* cgExtArrayVert[SHADER_TYPE_VERT_SIZE_CG] = {"VP40.cgsh", "VP30.cgsh", "ARBV.cgsh", "VP20.cgsh"};
static CGprofile   cgProfileArrayPix[SHADER_TYPE_PIX_SIZE_CG] = {CG_PROFILE_FP40, CG_PROFILE_FP30, CG_PROFILE_ARBFP1, CG_PROFILE_FP20};
static CGprofile   cgProfileArrayVert[SHADER_TYPE_VERT_SIZE_CG] = {CG_PROFILE_VP40, CG_PROFILE_VP30, CG_PROFILE_ARBVP1, CG_PROFILE_VP20};

// I HATE CG ERROR CHECKING!!!
// Right, so we need these here to tell us if there is an error, but they don't actually give us the error text.
void CGPixelShaderErrorHandler(CGcontext context, CGerror err, void *data)
{
   CGShader* shader = static_cast<CGShader *>(data);
   
   if(err == CG_COMPILER_ERROR)
   {
      shader->mFragCompileStatus = false;
      shader->mLinkStatus = false;
   }
}

void CGVertexShaderErrorHandler(CGcontext context, CGerror err, void *data)
{
   CGShader* shader = static_cast<CGShader *>(data);
   
   if(err == CG_COMPILER_ERROR)
   {
      shader->mVertCompileStatus = false;
      shader->mLinkStatus = false;
   }
}

CGShader::CGShader()
{
   mFragmentShader = NULL;
   mVertexShader = NULL;
   
   mVertErrorLog = "No log";
   mFragErrorLog = "No log";
   mFragCompilationLog = "No log";
   mVertCompilationLog = "No log";
   
   mVertCompileStatus = true;
   mFragCompileStatus = true;
   mLinkStatus = true;
   
   mVertexProfile = CG_PROFILE_UNKNOWN;
   mFragmentProfile = CG_PROFILE_UNKNOWN;

   mPixelSourceString = NULL;
   mVertexSourceString = NULL;
   mPixelSourceStringLength = 0;
   mVertexSourceStringLength = 0;
   
   mVertexFilepath = NULL;
   mFragFilepath = NULL;
}

bool CGShader::loadVertexShaderFromFile(const char* vertPath)
{
   char fileNameBufferVert[512];
   Con::expandScriptFilename(fileNameBufferVert, sizeof(fileNameBufferVert), vertPath);
   U32 lenVert = dStrlen(fileNameBufferVert);
   Stream *v;
   v = ResourceManager->openStream(fileNameBufferVert);
   if(!v)
   {
      AssertWarn(false, avar("Failed to load Cg vertex shader file: %s", fileNameBufferVert));
      return false;
   }
   
   mVertexProfile = ShaderManager->mCGVertexProfile;
   const char* vs;
   mVertexSourceStringLength = ResourceManager->getSize(fileNameBufferVert);
   mVertexSourceString = (char *)dMalloc((mVertexSourceStringLength + 1) * sizeof(U8));
   v->read(mVertexSourceStringLength, mVertexSourceString);
   ResourceManager->closeStream(v);
   const char* vsSourceString = reinterpret_cast<const char*>(mVertexSourceString);
   mVertErrorLog = "No log";
   mVertCompileStatus = true;
   #if defined(TORQUE_DEBUG) || defined(SHADER_MANAGER_DEBUG)
   cgSetErrorHandler(&CGVertexShaderErrorHandler, this);
   #endif

   mVertexShader = cgCreateProgramFromFile(ShaderManager->mCGContext, CG_SOURCE, 
                   fileNameBufferVert, mVertexProfile, NULL, NULL);
                   
   if(mVertCompileStatus == false)
      return false;
   
   mVertexFilepath = StringTable->insert(fileNameBufferVert);
                   
   cgGLLoadProgram(mVertexShader);
   return true;
}

bool CGShader::loadFragmentShaderFromFile(const char* fragPath)
{
   char fileNameBufferPix[512];
   Con::expandScriptFilename(fileNameBufferPix, sizeof(fileNameBufferPix), fragPath);
   U32 lenPix = dStrlen(fileNameBufferPix);
   
   Stream *p;
   p = ResourceManager->openStream(fileNameBufferPix);
   if(!p)
   {
      AssertWarn(false, avar("Failed to find Cg fragment shader file: %s", fileNameBufferPix));
      return false;
   }
   
   mFragmentProfile = ShaderManager->mCGFragmentProfile;

   const char* ps;
   mPixelSourceStringLength = ResourceManager->getSize(fileNameBufferPix);
   mPixelSourceString = (char *)dMalloc((mPixelSourceStringLength + 1) * sizeof(U8));
   p->read(mPixelSourceStringLength, mPixelSourceString);
   ResourceManager->closeStream(p);
  
   const char* psSourceString = reinterpret_cast<const char*>(mPixelSourceString);
   mFragErrorLog = "No log";
   
   mFragCompileStatus = true;
   
   mLinkStatus = true;
   
   #if defined(TORQUE_DEBUG) || defined(SHADER_MANAGER_DEBUG)
   cgSetErrorHandler(&CGPixelShaderErrorHandler, this);   
   #endif
   
                      
   mFragmentShader = cgCreateProgramFromFile(ShaderManager->mCGContext, CG_SOURCE, 
                     fileNameBufferPix, mFragmentProfile, NULL, NULL);
                     
   if(mFragCompileStatus == false)
      return false;
   
   mFragFilepath = StringTable->insert(fileNameBufferPix);
   
   cgGLLoadProgram(mFragmentShader);
   return true;
}

bool CGShader::loadShaderFromFiles(const char* vertPath, const char* fragPath)
{
   if(!ShaderManager->mCGInit)
      ShaderManager->initCG();
      
   if(vertPath)
   {
      if(!loadVertexShaderFromFile(vertPath))
      {
         if(fragPath)
         {
            loadShaderFromFiles("Shaders/Debug/errorVert.cg", "Shaders/Debug/errorPixl.cg");
         }
         else
         {
            loadShaderFromFiles("Shaders/Debug/errorVert.cg", NULL);
         }
         return true;
      }
   }
   if(fragPath)
   {
      if(!loadFragmentShaderFromFile(fragPath))
      {
         if(vertPath)
         {
            loadShaderFromFiles("Shaders/Debug/errorVert.cg", "Shaders/Debug/errorPixl.cg");
         }
         else
         {
            loadShaderFromFiles(NULL, "Shaders/Debug/errorPixl.cg");
         }
         return true;
      }
   }
   
   mFilepath = NULL;
   
   return true;
}

bool CGShader::compileShaderFromSource(const char* vertSource, const char* fragSource, bool includeLibs, bool link)
{
   if(!ShaderManager->mCGInit)
      ShaderManager->initCG();

   mFragmentProfile = ShaderManager->mCGFragmentProfile;
   mVertexProfile = ShaderManager->mCGVertexProfile;
   
   mPixelSourceStringLength = dStrlen(fragSource);
   mVertexSourceStringLength = dStrlen(vertSource);
   
   if(mPixelSourceString)
   {
      dFree(mPixelSourceString);
   }
   if(mVertexSourceString)
   {
      dFree(mVertexSourceString);
   }
   
   mPixelSourceString = (char *)dMalloc((mPixelSourceStringLength + 1) * sizeof(U8));
   mVertexSourceString = (char *)dMalloc((mVertexSourceStringLength + 1) * sizeof(U8));
   
   dMemcpy(mPixelSourceString, fragSource, mPixelSourceStringLength + 1);
   dMemcpy(mVertexSourceString, vertSource, mVertexSourceStringLength + 1);

   if(mVertexShader)
      cgDestroyProgram(mVertexShader);
   if(mFragmentShader)
      cgDestroyProgram(mFragmentShader);

   mVertErrorLog = "No log";
   mFragErrorLog = "No log";
   
   mFragCompileStatus = true;
   mVertCompileStatus = true;
   mLinkStatus = true;
   
   #if defined(TORQUE_DEBUG) || defined(SHADER_MANAGER_DEBUG)
   cgSetErrorHandler(&CGVertexShaderErrorHandler, this);
   #endif

   mVertexShader = cgCreateProgram(ShaderManager->mCGContext, CG_SOURCE, vertSource, mVertexProfile, NULL, NULL);
   
   const char *vertLog = cgGetLastListing(ShaderManager->mCGContext);
   if(vertLog)
      mVertErrorLog = StringTable->insert(cgGetLastListing(ShaderManager->mCGContext));
   
   #if defined(TORQUE_DEBUG) || defined(SHADER_MANAGER_DEBUG)
   cgSetErrorHandler(&CGPixelShaderErrorHandler, this);   
   #endif
   
   mFragmentShader = cgCreateProgram(ShaderManager->mCGContext, CG_SOURCE, fragSource, mFragmentProfile, NULL, NULL);
   
   const char* fragLog = cgGetLastListing(ShaderManager->mCGContext);
   if(fragLog)
      mFragErrorLog = StringTable->insert(cgGetLastListing(ShaderManager->mCGContext));
   
   cgGLLoadProgram(mVertexShader);
   cgGLLoadProgram(mFragmentShader);
   
   reloadParameters();
   return true;
}

CGShaderParameter* CGShader::getNamedParameter(StringTableEntry name)
{
   StringTableEntry name2 = StringTable->insert(name);

   for(U32 i = 0; i < mParameters.size(); i++)
   {
      if(mParameters[i]->mName == name2)
      {
         return mParameters[i];
      }
   }

   // No parameter, let's see if we have one in the shader
   CGparameter id = cgGetNamedParameter(mFragmentShader, name);
   CGShaderParameter* param = NULL;
   param = new CGShaderParameter;
   param->mName = name2;
   param->mFragmentId = NULL;
   param->mVertexId = NULL;
   mParameters.push_back(param);
   if(id)
   {
      param->mFragmentId = id;
   }
   id = cgGetNamedParameter(mVertexShader, name);
   if(id)
   {
      param->mVertexId = id;
   }
   
   return param;
}

bool CGShader::setParameter1f(StringTableEntry name, F32 value0)
{
   CGShaderParameter* param = getNamedParameter(name);
   if(!param)
      return false;
   
   if(param->mVertexId)
      cgGLSetParameter1f(param->mVertexId, value0);
   if(param->mFragmentId)
      cgGLSetParameter1f(param->mFragmentId, value0);
   return true;  
}

bool CGShader::setParameter2f(StringTableEntry name, F32 value0, F32 value1)
{
   CGShaderParameter* param = getNamedParameter(name);
   if(!param)
      return false;
   
   if(param->mVertexId)
      cgGLSetParameter2f(param->mVertexId, value0, value1);
   if(param->mFragmentId)
      cgGLSetParameter2f(param->mFragmentId, value0, value1);
   return true;
}

bool CGShader::setParameter3f(StringTableEntry name, F32 value0, F32 value1, F32 value2)
{
   CGShaderParameter* param = getNamedParameter(name);
   if(!param)
      return false;
   
   if(param->mVertexId)
      cgGLSetParameter3f(param->mVertexId, value0, value1, value2);
   if(param->mFragmentId)
      cgGLSetParameter3f(param->mFragmentId, value0, value1, value2);
   return true;
}

bool CGShader::setParameter4f(StringTableEntry name, F32 value0, F32 value1, F32 value2, F32 value3)
{
   CGShaderParameter* param = getNamedParameter(name);
   if(!param)
      return false;
   
   if(param->mVertexId)
      cgGLSetParameter4f(param->mVertexId, value0, value1, value2, value3);
   if(param->mFragmentId)
      cgGLSetParameter4f(param->mFragmentId, value0, value1, value2, value3);
   return true;
}

bool CGShader::setParameter1i(StringTableEntry name, U32 value0)
{
   return setParameter1f(name, (F32)value0);
}

bool CGShader::setParameter2i(StringTableEntry name, U32 value0, U32 value1)
{
   return setParameter2f(name, (F32)value0, (F32)value1);
}

bool CGShader::setParameter3i(StringTableEntry name, U32 value0, U32 value1, U32 value2)
{
   return setParameter3f(name, (F32)value0, (F32)value1, (F32)value2);
}

bool CGShader::setParameter4i(StringTableEntry name, U32 value0, U32 value1, U32 value2, U32 value3)
{
   return setParameter4f(name, (F32)value0, (F32)value1, (F32)value2, (F32)value3);
}

bool CGShader::setParameter4x4f(StringTableEntry name, bool transpose, MatrixF *m)
{
   CGShaderParameter* param = getNamedParameter(name);
   if(!param->mFragmentId || !param->mVertexId)
      return false;
      
   if(transpose)
   {
      m = &m->transpose();
   }
   
   if(param->mVertexId)
      cgGLSetMatrixParameterfc(param->mVertexId, (F32*)m);
   if(param->mFragmentId)
      cgGLSetMatrixParameterfc(param->mFragmentId, (F32*)m);
   return true;
}

bool CGShader::setTextureSampler(StringTableEntry name, U32 value0)
{
   return true;
}

void CGShader::updateStateMatrices()
{
   CGShaderParameter* param = getNamedParameter("ModelViewProj");
      
   if(param->mVertexId)
   {
      cgGLSetStateMatrixParameter(param->mVertexId, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
   }
   if(param->mFragmentId)
      cgGLSetStateMatrixParameter(param->mFragmentId, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
      
   param = getNamedParameter("ModelView");
   
   if(param->mVertexId)
      cgGLSetStateMatrixParameter(param->mVertexId, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
   if(param->mFragmentId)
      cgGLSetStateMatrixParameter(param->mFragmentId, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
      
   param = getNamedParameter("ModelViewIT");
   
   if(param->mVertexId)
      cgGLSetStateMatrixParameter(param->mVertexId, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
   if(param->mFragmentId)
      cgGLSetStateMatrixParameter(param->mFragmentId, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
}

void CGShader::reloadParameters()
{
   for(U32 i = 0; i < mParameters.size(); i++)
   {
      delete mParameters[i];
   }
   mParameters.clear();
}

void CGShader::updateStatus()
{
   // Don't think we really have to do anything here
}

const char* CGShader::getVertexLog()
{
   return mVertErrorLog;
}

const char* CGShader::getFragmentLog()
{
   return mFragErrorLog;
}

const char* CGShader::getProgramLog()
{
   // Cg has no concept of a program.  This is really for GLSL's benefit.
   return "No log";
}

void CGShader::bind()
{
   cgGLEnableProfile(mVertexProfile);
   cgGLEnableProfile(mFragmentProfile);
   
   cgGLBindProgram(mVertexShader);
   cgGLBindProgram(mFragmentShader);
   
   updateStateMatrices();
}

void CGShader::unbind()
{
   cgGLDisableProfile(mVertexProfile);
   cgGLDisableProfile(mFragmentProfile);
}

void CGShader::reset()
{
   for(U32 i = 0; i < mParameters.size(); i++)
   {
      delete mParameters[i];
   }
   mParameters.clear();
}

void CGShader::kill()
{
   for(U32 i = 0; i < mParameters.size(); i++)
   {
      delete mParameters[i];
   }
   mParameters.clear();
   dFree(mPixelSourceString);
   dFree(mVertexSourceString);
}

void CGShader::resurrect()
{
   const char* vsSourceString = reinterpret_cast<const char*>(mVertexSourceString);
   const char* psSourceString = reinterpret_cast<const char*>(mPixelSourceString);

   mVertexShader = cgCreateProgram(ShaderManager->mCGContext, CG_SOURCE, vsSourceString, mVertexProfile, NULL, NULL);
   mFragmentShader = cgCreateProgram(ShaderManager->mCGContext, CG_SOURCE, psSourceString, mFragmentProfile, NULL, NULL);
   cgGLLoadProgram(mVertexShader);
   cgGLLoadProgram(mFragmentShader);
}
#endif

