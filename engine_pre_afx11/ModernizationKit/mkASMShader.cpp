/*  mkASMShader.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */
 
#include "ModernizationKit/mkShaderManager.h"
#include "console/console.h"
#include "platform/platform.h"
#include "core/resManager.h"
#include "core/stringTable.h"
#include "core/frameAllocator.h" 

#define ASM_SHADER_SHOW_STATS

ASMShader::ASMShader()
{
   mVertexProgram = 0;
   mFragmentProgram = 0;
   
   mVertexFilepath = NULL;
   mFragFilepath = NULL;
   mFilepath = NULL;
   
   mPixelSourceString = NULL;
   mVertexSourceString = NULL;
   mPixelSourceStringLength = 0;
   mVertexSourceStringLength = 0;
}

bool ASMShader::loadVertexShaderFromFile(const char* vertPath)
{
   char fileNameBufferVert[512];
   Con::expandScriptFilename(fileNameBufferVert, sizeof(fileNameBufferVert), vertPath);
   
   const char* vs;
   
   Stream *v = ResourceManager->openStream(fileNameBufferVert);
   if(!v)
   {
      AssertWarn(false, avar("Failed to find ASM vertex shader file: %s", fileNameBufferVert));
      return false;
   }
   
   mVertexSourceStringLength = ResourceManager->getSize(fileNameBufferVert);
   mVertexSourceString = (char *)dMalloc((mVertexSourceStringLength + 1) * sizeof(U8));
   v->read(mVertexSourceStringLength, mVertexSourceString);
   ResourceManager->closeStream(v);
   
   glEnable(GL_VERTEX_PROGRAM_ARB);
   glGenProgramsARB(1, &mVertexProgram);
   glBindProgramARB(GL_VERTEX_PROGRAM_ARB, mVertexProgram);
   glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, mVertexSourceStringLength, mVertexSourceString);
   
   #if defined(TORQUE_DEBUG) || defined(SHADER_MANAGER_DEBUG)
   const char* errorString = NULL;
   errorString = (const char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB);
   Con::printf("Error string for %s is %s", fileNameBufferVert, errorString);
   if(errorString && errorString[0] != '\0')
      return false;
   #endif
   #ifdef ASM_SHADER_SHOW_STATS
   bool stats = Con::getBoolVariable("$pref::shaderManager::showASMStats");
   if(stats)
   {
      GLint param;
      Con::printf("Stats for %s", fileNameBufferVert);
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_INSTRUCTIONS_ARB, &param);
      Con::printf("%i instructions", param);
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB, &param);
      Con::printf("%i native instructions", param);
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_TEMPORARIES_ARB, &param);
      Con::printf("%i temporaries", param);
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_TEMPORARIES_ARB, &param);
      Con::printf("%i native temporaries", param);
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_PARAMETERS_ARB, &param);
      Con::printf("%i parameters", param);
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_PARAMETERS_ARB, &param);
      Con::printf("%i native parameters", param);
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_ATTRIBS_ARB, &param);
      Con::printf("%i attribs", param);
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_ATTRIBS_ARB, &param);
      Con::printf("%i native attribs", param);
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_ADDRESS_REGISTERS_ARB, &param);
      Con::printf("%i address registers", param);
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB, &param);
      Con::printf("%i native address registers", param);
      glGetProgramivARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB, &param);
      if(param)
         Con::printf("Program is under native limits!");
      else
         Con::printf("Program exceeds native limits!");
   }
      #endif
   glDisable(GL_VERTEX_PROGRAM_ARB);
   mVertexFilepath = StringTable->insert(fileNameBufferVert);
   return true;
}

bool ASMShader::loadFragmentShaderFromFile(const char* fragPath)
{
   char fileNameBufferPix[512];
   Con::expandScriptFilename(fileNameBufferPix, sizeof(fileNameBufferPix), fragPath);
   
   const char* ps;
   
   Stream *p = ResourceManager->openStream(fileNameBufferPix);
   if(!p)
   {
      AssertWarn(false, avar("Failed to find ASM fragment shader file: %s", fileNameBufferPix));
      return false;
   }
   mPixelSourceStringLength = ResourceManager->getSize(fileNameBufferPix);
   mPixelSourceString = (char *)dMalloc((mPixelSourceStringLength + 1) * sizeof(U8));
   p->read(mPixelSourceStringLength, mPixelSourceString);
   ResourceManager->closeStream(p);
   
   glEnable(GL_FRAGMENT_PROGRAM_ARB);
   glGenProgramsARB(1, &mFragmentProgram);
   glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, mFragmentProgram);
   glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, mPixelSourceStringLength, mPixelSourceString);
   
   #if defined(TORQUE_DEBUG) || defined(SHADER_MANAGER_DEBUG)
   const char* errorString = NULL;
   errorString = (const char*)glGetString(GL_PROGRAM_ERROR_STRING_ARB);
   Con::printf("Error string for %s is %s", fileNameBufferPix, errorString);
   if(errorString && errorString[0] != '\0')
      return false;
   #endif
   
   #ifdef ASM_SHADER_SHOW_STATS
   bool stats = Con::getBoolVariable("$pref::shaderManager::showASMStats");
   if(stats)
   {
      GLint param;
      Con::printf("");
      Con::printf("Stats for %s", fileNameBufferPix);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_INSTRUCTIONS_ARB, &param);
      Con::printf("%i instructions", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_INSTRUCTIONS_ARB, &param);
      Con::printf("%i native instructions", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_TEMPORARIES_ARB, &param);
      Con::printf("%i temporaries", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_TEMPORARIES_ARB, &param);
      Con::printf("%i native temporaries", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_PARAMETERS_ARB, &param);
      Con::printf("%i parameters", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_PARAMETERS_ARB, &param);
      Con::printf("%i native parameters", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_ATTRIBS_ARB, &param);
      Con::printf("%i attribs", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_ATTRIBS_ARB, &param);
      Con::printf("%i native attribs", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_ADDRESS_REGISTERS_ARB, &param);
      Con::printf("%i address registers", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_ADDRESS_REGISTERS_ARB, &param);
      Con::printf("%i native address registers", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_ALU_INSTRUCTIONS_ARB, &param);
      Con::printf("%i ALU instructions", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_ALU_INSTRUCTIONS_ARB, &param);
      Con::printf("%i native ALU instructions", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_TEX_INSTRUCTIONS_ARB, &param);
      Con::printf("%i TEX instructions", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_TEX_INSTRUCTIONS_ARB, &param);
      Con::printf("%i native TEX instructions", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_TEX_INDIRECTIONS_ARB, &param);
      Con::printf("%i TEX indirections", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_NATIVE_TEX_INDIRECTIONS_ARB, &param);
      Con::printf("%i native TEX indirections", param);
   
      glGetProgramivARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_UNDER_NATIVE_LIMITS_ARB, &param);
      if(param)
         Con::printf("Program is under native limits!");
      else
         Con::printf("Program exceeds native limits!");
      Con::printf("");
   }
   #endif
   glDisable(GL_FRAGMENT_PROGRAM_ARB);
   
   mFragFilepath = StringTable->insert(fileNameBufferPix);
   return true;
}

bool ASMShader::loadShaderFromFiles(const char* vertPath, const char* fragPath)
{
   if(vertPath)
   {
      if(!loadVertexShaderFromFile(vertPath))
      {
         if(fragPath)
         {
            loadShaderFromFiles("Shaders/Debug/errorVert.arb", "Shaders/Debug/errorPixl.arb");
         }
         else
         {
            loadShaderFromFiles("Shaders/Debug/errorVert.arb", NULL);
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
            glDeleteProgramsARB(1, &mVertexProgram);
            mVertexProgram = 0;
            loadShaderFromFiles("Shaders/Debug/errorVert.arb", "Shaders/Debug/errorPixl.arb");
         }
         else
         {
            loadShaderFromFiles(NULL, "Shaders/Debug/errorPixl.arb");
         }
         return true;
      }
   }
   
   mFilepath = NULL;
   
   return true;
}

ASMShaderParameter* ASMShader::getNamedParameter(StringTableEntry name)
{
   for(U32 i = 0; i < mParameters.size(); i++)
   {
      if(dStricmp(mParameters[i]->mName, name) == 0)
      {
         return mParameters[i];
      }
   }
   
   //No parameter...
   const char* paramString;
   ASMShaderParameter* param = NULL;
   paramString = dStrstr(mVertexSourceString, name);
   if(paramString)
   {
      param = new ASMShaderParameter;
      const char* openBracket = dStrstr(paramString, "[");
      const char* closeBracket = dStrstr(paramString, "]");
      char* num = (char *)dMalloc((closeBracket - openBracket + 1) * sizeof(U8));
      num = dStrncpy(num, openBracket + 1, (closeBracket - (openBracket + 1)));
      num[(closeBracket - (openBracket + 1))] = NULL;
      param->mName = StringTable->insert(name);
      param->mVertexId = dAtoi(num);
      const char* env = dStrstr(paramString, "program.env");
      param->mVertexIsEnv = (env != NULL && env < openBracket);
      param->mFragmentId = -1;
      param->mFragmentIsEnv = false;
      mParameters.push_back(param);
      dFree(num);
   }
   paramString = dStrstr(mPixelSourceString, name);
   if(paramString)
   {
      if(!param)
      {
         param = new ASMShaderParameter;
         mParameters.push_back(param);
         param->mVertexId = -1;
         param->mVertexIsEnv = false;
         param->mName = StringTable->insert(name);
      }
      const char* openBracket = dStrstr(paramString, "[");
      const char* closeBracket = dStrstr(paramString, "]");
      char* num = (char *)dMalloc((closeBracket - openBracket + 1) * sizeof(U8));
      num = dStrncpy(num, openBracket + 1, (closeBracket - (openBracket + 1)));
      num[(closeBracket - (openBracket + 1))] = NULL;
      param->mFragmentId = dAtoi(num);
      const char* env = dStrstr(paramString, "program.env");
      param->mFragmentIsEnv = (env != NULL && env < openBracket);
      dFree(num);
   }
   return param;
}

bool ASMShader::setParameter1f(StringTableEntry name, F32 value0)
{
   ASMShaderParameter* param = getNamedParameter(name);
   if(!param)
      return false;

   if(param->mVertexId != -1)
   {
      if(param->mVertexIsEnv)
         glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, param->mVertexId, value0, value0, value0, value0);
      else
         glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, param->mVertexId, value0, value0, value0, value0);
   }
   if(param->mFragmentId != -1)
   {
      if(param->mFragmentIsEnv)
         glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, param->mFragmentId, value0, value0, value0, value0);
      else
         glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, param->mFragmentId, value0, value0, value0, value0);
   }
   return true;
}

bool ASMShader::setParameter2f(StringTableEntry name, F32 value0, F32 value1)
{
   ASMShaderParameter* param = getNamedParameter(name);
   if(!param)
      return false;

   if(param->mVertexId != -1)
   {
      if(param->mVertexIsEnv)
         glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, param->mVertexId, value0, value1, value0, value1);
      else
         glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, param->mVertexId, value0, value1, value0, value1);
   }
   if(param->mFragmentId != -1)
   {
      if(param->mFragmentIsEnv)
         glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, param->mFragmentId, value0, value1, value0, value1);
      else
         glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, param->mFragmentId, value0, value1, value0, value1);
   }
   return true;
}

bool ASMShader::setParameter3f(StringTableEntry name, F32 value0, F32 value1, F32 value2)
{
   ASMShaderParameter* param = getNamedParameter(name);
   if(!param)
      return false;

   if(param->mVertexId != -1)
   {
      if(param->mVertexIsEnv)
         glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, param->mVertexId, value0, value1, value2, value0);
      else
         glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, param->mVertexId, value0, value1, value2, value0);
   }
   if(param->mFragmentId != -1)
   {
      if(param->mFragmentIsEnv)
         glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, param->mFragmentId, value0, value1, value2, value0);
      else
         glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, param->mFragmentId, value0, value1, value2, value0);
   }
   return true;
}

bool ASMShader::setParameter4f(StringTableEntry name, F32 value0, F32 value1, F32 value2, F32 value3)
{
   ASMShaderParameter* param = getNamedParameter(name);
   if(!param)
      return false;

   if(param->mVertexId != -1)
   {
      if(param->mVertexIsEnv)
         glProgramEnvParameter4fARB(GL_VERTEX_PROGRAM_ARB, param->mVertexId, value0, value1, value2, value3);
      else
         glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, param->mVertexId, value0, value1, value2, value3);
   }
   if(param->mFragmentId != -1)
   {
      if(param->mFragmentIsEnv)
         glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, param->mFragmentId, value0, value1, value2, value3);
      else
         glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, param->mFragmentId, value0, value1, value2, value3);
   }
   return true;
}

bool ASMShader::setParameter1i(StringTableEntry name, U32 value0)
{
   return setParameter1f(name, (F32)value0);
}

bool ASMShader::setParameter2i(StringTableEntry name, U32 value0, U32 value1)
{
   return setParameter2f(name, (F32)value0, (F32)value1);
}

bool ASMShader::setParameter3i(StringTableEntry name, U32 value0, U32 value1, U32 value2)
{
   return setParameter3f(name, (F32)value0, (F32)value1, (F32)value2);
}

bool ASMShader::setParameter4i(StringTableEntry name, U32 value0, U32 value1, U32 value2, U32 value3)
{
   return setParameter4f(name, (F32)value0, (F32)value1, (F32)value2, (F32)value3);
}

bool ASMShader::setParameter4x4f(StringTableEntry name, bool transpose, MatrixF *m)
{
   return false;
}

bool ASMShader::setTextureSampler(StringTableEntry name, U32 value0)
{
   return true;
}

void ASMShader::bind()
{
   glEnable(GL_VERTEX_PROGRAM_ARB);
   glEnable(GL_FRAGMENT_PROGRAM_ARB);
   
   glBindProgramARB(GL_VERTEX_PROGRAM_ARB, mVertexProgram);
   glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, mFragmentProgram);
}

void ASMShader::unbind()
{
   glDisable(GL_VERTEX_PROGRAM_ARB);
   glDisable(GL_FRAGMENT_PROGRAM_ARB);
}

void ASMShader::reset()
{
   glDeleteProgramsARB(1, &mVertexProgram);
   glDeleteProgramsARB(1, &mFragmentProgram);
}

void ASMShader::resurrect()
{
   glEnable(GL_VERTEX_PROGRAM_ARB);
   glEnable(GL_FRAGMENT_PROGRAM_ARB);
   
   glGenProgramsARB(1, &mVertexProgram);
   glGenProgramsARB(1, &mFragmentProgram);
   
   glBindProgramARB(GL_VERTEX_PROGRAM_ARB, mVertexProgram);
   glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, mFragmentProgram);
   
   glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, mVertexSourceStringLength, mVertexSourceString);
   glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, mPixelSourceStringLength, mPixelSourceString);
   
   glDisable(GL_VERTEX_PROGRAM_ARB);
   glDisable(GL_FRAGMENT_PROGRAM_ARB);
}

void ASMShader::kill()
{
   glDeleteProgramsARB(1, &mVertexProgram);
   glDeleteProgramsARB(1, &mFragmentProgram);
   for(U32 i = 0; i < mParameters.size(); i++)
   {
      delete mParameters[i];
   }
   mParameters.clear();
   dFree(mPixelSourceString);
   dFree(mVertexSourceString);
}

