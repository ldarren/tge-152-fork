/*  mkGLSLShader.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/mkShaderManager.h"
#include "console/console.h"
#include "platform/platform.h"
#include "core/resManager.h"
#include "core/stringTable.h"
#include "core/frameAllocator.h"

// Allows dumping active uniforms and vertex attributes to the console on shader compilation
#define GLSL_SHADER_SHOW_STATS

GLSLShader::GLSLShader()
{
   mProgram = NULL;
   mFragmentShader = NULL;
   mVertexShader = NULL;
   
   mPixelSourceString = NULL;
   mVertexSourceString = NULL;
   mPixelSourceStringLength = 0;
   mVertexSourceStringLength = 0;
   
   mVertCompileStatus = false;
   mFragCompileStatus = false;
   mLinkStatus = false;
   
   mVertexFilepath = NULL;
   mFragFilepath = NULL;
}

bool GLSLShader::loadVertexShaderFromFile(const char* vertPath)
{
   char fileNameBufferVert[512];
   
   Con::expandScriptFilename(fileNameBufferVert, sizeof(fileNameBufferVert), vertPath);

   const char* vs;
   
   Stream *v = ResourceManager->openStream(fileNameBufferVert);
   if(!v)
   {
      AssertWarn(false, avar("Failed to find GLSL vertex shader file: %s", fileNameBufferVert));
      return false;
   }
   
   mVertexSourceStringLength = ResourceManager->getSize(fileNameBufferVert);   
   mVertexSourceString = (char *)dMalloc((mVertexSourceStringLength + 1) * sizeof(U8));
   v->read(mVertexSourceStringLength, mVertexSourceString);
   mVertexSourceString[mVertexSourceStringLength] = NULL;
   ResourceManager->closeStream(v);
   
   mVertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
   const char* vsSourceString = reinterpret_cast<const char*>(mVertexSourceString);
      
   glShaderSourceARB(mVertexShader, 1,
                     &vsSourceString, NULL);
                     
   glCompileShaderARB(mVertexShader);
   
   #if defined(TORQUE_DEBUG) || defined(SHADER_MANAGER_DEBUG)
   Con::printf("Checking for errors in %s", vertPath);
   bool error = false;
   GLint success;
   glGetObjectParameterivARB(mVertexShader, GL_OBJECT_COMPILE_STATUS_ARB, (GLint*)&mVertCompileStatus);
   if(!mVertCompileStatus)
   {
      GLint infoLogLength = 0;
      char* infoLog;
      Con::errorf("GLSL vertex shader failed to compile!");
      glGetObjectParameterivARB(mVertexShader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength);
      if(infoLogLength > 1)
      {
         infoLog = (char *)FrameAllocator::alloc(infoLogLength * sizeof(char));
         glGetInfoLogARB(mVertexShader, infoLogLength, NULL, infoLog);
         Con::errorf("GLSL vertex shader info log %s", infoLog);
      }
      error = true;
   }
   
   if(error)
   {
      glDeleteObjectARB(mVertexShader);
      return false;
   }
   #endif
   
   glAttachObjectARB(mProgram, mVertexShader);
   mVertexFilepath = StringTable->insert(fileNameBufferVert);
   return true;
}

bool GLSLShader::loadFragmentShaderFromFile(const char* fragPath)
{
   char fileNameBufferPix[512];
   
   Con::expandScriptFilename(fileNameBufferPix, sizeof(fileNameBufferPix), fragPath);
   
   const char* ps;
   
   Stream *p = ResourceManager->openStream(fileNameBufferPix);
   if(!p)
   {
      AssertWarn(false, avar("Failed to find GLSL fragment shader file: %s", fileNameBufferPix));
      return false;
   }
      
   mPixelSourceStringLength = ResourceManager->getSize(fileNameBufferPix);
   mPixelSourceString = (char *)dMalloc((mPixelSourceStringLength + 1) * sizeof(U8));
   p->read(mPixelSourceStringLength, mPixelSourceString);
   mPixelSourceString[mPixelSourceStringLength] = NULL;
   ResourceManager->closeStream(p);

   mFragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
   const char* psSourceString = reinterpret_cast<const char*>(mPixelSourceString);
   
   glShaderSourceARB(mFragmentShader, 1,
                     &psSourceString, NULL);
   glCompileShaderARB(mFragmentShader);
   
   #if defined(TORQUE_DEBUG) || defined(SHADER_MANAGER_DEBUG)
   Con::printf("Checking for errors in %s", fragPath);
   bool error = false;
   GLint success;
   glGetObjectParameterivARB(mFragmentShader, GL_OBJECT_COMPILE_STATUS_ARB, (GLint*)&mFragCompileStatus);
   if(!mFragCompileStatus)
   {
      GLint infoLogLength = 0;
      char* infoLog;
      Con::errorf("GLSL fragment shader failed to compile!");
      glGetObjectParameterivARB(mFragmentShader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength);
      if(infoLogLength > 1)
      {
         infoLog = (char *)FrameAllocator::alloc(infoLogLength * sizeof(char));
         glGetInfoLogARB(mFragmentShader, infoLogLength, NULL, infoLog);
         Con::errorf("GLSL fragment shader info log %s", infoLog);
      }
      error = true;
   } 
   
   if(error)
   {
      glDeleteObjectARB(mFragmentShader);
      return false;
   }
   #endif
      
   glAttachObjectARB(mProgram, mFragmentShader);
   
   mFragFilepath = StringTable->insert(fileNameBufferPix);
   
   return true;
}

// Load from separate vert/pixel paths
bool GLSLShader::loadShaderFromFiles(const char* vertPath, const char* fragPath)
{      
   mProgram = glCreateProgramObjectARB();
   
   if(vertPath)
   {
      if(!loadVertexShaderFromFile(vertPath))
      {
         glDeleteObjectARB(mProgram);
         if(fragPath)
         {
            loadShaderFromFiles("Shaders/Debug/errorVert.glsl", "Shaders/Debug/errorPixl.glsl");
         }
         else
         {
            loadShaderFromFiles("Shaders/Debug/errorVert.glsl", NULL);
         }
         return true;
      }
   }
   if(fragPath)
   {
      if(!loadFragmentShaderFromFile(fragPath))
      {
         glDeleteObjectARB(mProgram);
         if(vertPath)
         {
            glDeleteObjectARB(mVertexShader);
            loadShaderFromFiles("Shaders/Debug/errorVert.glsl", "Shaders/Debug/errorPixl.glsl");
         }
         else
         {
            loadShaderFromFiles(NULL, "Shaders/Debug/errorPixl.glsl");
         }
         return true;
      }
   }
   glLinkProgramARB(mProgram);
   
   #if defined(TORQUE_DEBUG) || defined(SHADER_MANAGER_DEBUG)
   bool error = false;
   GLint success;
   glGetObjectParameterivARB(mProgram, GL_OBJECT_LINK_STATUS_ARB, (GLint*)&mLinkStatus);
   if(!mLinkStatus)
   {
      GLint infoLogLength = 0;
      char* infoLog;
      Con::errorf("GLSL program failed to link!");
      glGetObjectParameterivARB(mProgram, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength);
      if(infoLogLength > 1)
      {
         infoLog = (char *)FrameAllocator::alloc(infoLogLength * sizeof(char));
         glGetInfoLogARB(mProgram, infoLogLength, NULL, infoLog);
         Con::errorf("GLSL program info log %s", infoLog);
      }
      error = true;
   }
      
   glValidateProgramARB(mProgram);
   glGetObjectParameterivARB(mProgram, GL_OBJECT_VALIDATE_STATUS_ARB, &success);
   if(!success)
   {
      GLint infoLogLength = 0;
      char* infoLog;
      Con::errorf("GLSL program failed validation!");
      glGetObjectParameterivARB(mProgram, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength);
      if(infoLogLength > 1)
      {
         infoLog = (char *)FrameAllocator::alloc(infoLogLength * sizeof(char));
         glGetInfoLogARB(mProgram, infoLogLength, NULL, infoLog);
         Con::printf("GLSL program info log %s", infoLog);
      }
      error = true;
   }
   
   GLint infoLogLength = 0;
   char* infoLog;
   glGetObjectParameterivARB(mProgram, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength);
   if(infoLogLength > 1)
   {
      infoLog = (char *)FrameAllocator::alloc(infoLogLength * sizeof(char));
      glGetInfoLogARB(mProgram, infoLogLength, NULL, infoLog);
      Con::printf("GLSL program info log %s", infoLog);
   }
   
   if(error)
   {
      glDeleteObjectARB(mProgram);
      glDeleteObjectARB(mVertexShader);
      glDeleteObjectARB(mFragmentShader);
      if(vertPath && fragPath)
      {
         loadShaderFromFiles("Shaders/Debug/errorVert.glsl", "Shaders/Debug/errorPixl.glsl");
         return true;
      }
      else if(vertPath)
      {
         loadShaderFromFiles("Shaders/Debug/errorVert.glsl", NULL);
         return true;
      }
      else if(fragPath)
      {
         loadShaderFromFiles(NULL, "Shaders/Debug/errorPixl.glsl");
         return true;
      }
      else
      {
         AssertFatal(false, "We have no files.  No wonder we failed to load a shader");
      }
   }
      
   #endif
   
   #ifdef GLSL_SHADER_SHOW_STATS
   bool stats = Con::getBoolVariable("$pref::shaderManager::showGLSLStats");
   if(stats)
   {
      GLint count;
      char name[512];
   
      glGetObjectParameterivARB(mProgram, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &count);
      Con::printf("  Active uniforms: %i", count);
      for(U32 i = 0; i < count; i++)
      {
         glGetActiveUniformARB(mProgram, i, 512, NULL, NULL, NULL, &name[0]);
         Con::printf("    Uniform %i is %s", i, name);
      }
      glGetObjectParameterivARB(mProgram, GL_OBJECT_ACTIVE_ATTRIBUTES_ARB, &count);
      Con::printf("  Active attributes: %i", count);
      Con::printf("");
   }
   
   #endif
   
   mFilepath = NULL;
   
   return true;
}

// Load from raw source (used by shader editor)
bool GLSLShader::compileShaderFromSource(const char* vertSource, const char* fragSource, bool includeLibs, bool link)
{
   if(!mProgram)
   {
      mVertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
      mFragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
      mProgram = glCreateProgramObjectARB();
      glAttachObjectARB(mProgram, mVertexShader);
      glAttachObjectARB(mProgram, mFragmentShader);
   }
   
   static Vector<StringTableEntry> vsSource(__FILE__, __LINE__);
   static Vector<StringTableEntry> psSource(__FILE__, __LINE__);
   vsSource.clear();
   psSource.clear();

   vsSource.push_back(vertSource);
   psSource.push_back(fragSource);
   
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
   
   glShaderSourceARB(mVertexShader, vsSource.size(),
                     vsSource.address(), NULL);
   glShaderSourceARB(mFragmentShader, psSource.size(),
                     psSource.address(), NULL);
   glCompileShaderARB(mVertexShader);
   glCompileShaderARB(mFragmentShader);
   
   if(link)
   {
      glLinkProgramARB(mProgram);
      reloadParameters();
   }
   return true;
}

// This could be part of compilation, but takes up a lot of time
void GLSLShader::updateStatus()
{
   glGetObjectParameterivARB(mVertexShader, GL_OBJECT_COMPILE_STATUS_ARB, (GLint*)&mVertCompileStatus);

   glGetObjectParameterivARB(mFragmentShader, GL_OBJECT_COMPILE_STATUS_ARB, (GLint*)&mFragCompileStatus);
   
   glGetObjectParameterivARB(mProgram, GL_OBJECT_LINK_STATUS_ARB, (GLint*)&mLinkStatus);
}

const char* GLSLShader::getProgramLog()
{
   char* infoLog;
   U32 infoLogLength;
   U32 length = 0;
   U32 s = FrameAllocator::getWaterMark();
   glGetObjectParameterivARB(mProgram, GL_OBJECT_INFO_LOG_LENGTH_ARB, (GLint*)&infoLogLength);
   if(infoLogLength > 1)
   {
      infoLog = (char *)FrameAllocator::alloc(infoLogLength * sizeof(char));
      glGetInfoLogARB(mProgram, infoLogLength, (GLint*)&length, infoLog);
   }
   if(length == 0)
   {
      infoLog = "No log";
   }
   FrameAllocator::setWaterMark(s);
   return infoLog;
}

const char* GLSLShader::getVertexLog()
{
   char* infoLog;
   U32 infoLogLength;
   U32 length = 0;
   U32 s = FrameAllocator::getWaterMark();
   glGetObjectParameterivARB(mVertexShader, GL_OBJECT_INFO_LOG_LENGTH_ARB, (GLint*)&infoLogLength);
   if(infoLogLength > 1)
   {
      infoLog = (char *)FrameAllocator::alloc(infoLogLength * sizeof(char));
      glGetInfoLogARB(mVertexShader, infoLogLength, (GLint*)&length, infoLog);
   }
   if(length == 0)
   {
      infoLog = "No log";
   }
   FrameAllocator::setWaterMark(s);
   return infoLog;
}

const char* GLSLShader::getFragmentLog()
{
   char* infoLog;
   U32 infoLogLength;
   U32 length = 0;
   U32 s = FrameAllocator::getWaterMark();
   glGetObjectParameterivARB(mFragmentShader, GL_OBJECT_INFO_LOG_LENGTH_ARB, (GLint*)&infoLogLength);
   if(infoLogLength > 1)
   {
      infoLog = (char *)FrameAllocator::alloc(infoLogLength * sizeof(char));
      glGetInfoLogARB(mFragmentShader, infoLogLength, (GLint*)&length, infoLog);
   }
   if(length == 0)
   {
      infoLog = "No log";
   }
   FrameAllocator::setWaterMark(s);
   return infoLog;
}
   

GLSLShaderParameter* GLSLShader::getNamedParameter(StringTableEntry name)
{
   // Using the string table saves a ton of string compares, but is case insensitive.
   StringTableEntry name2 = StringTable->insert(name);

   for(U32 i = 0; i < mParameters.size(); i++)
   {
      if(mParameters[i]->mName == name2)
      {
         return mParameters[i];
      }
   }

   // No parameter, let's see if we have one in the shader
   U32 id = glGetUniformLocationARB(mProgram, name);
   GLSLShaderParameter* param = new GLSLShaderParameter;
   param->mName = name2;
   param->mId = id;
   mParameters.push_back(param);
   return param;
}

bool GLSLShader::setParameter1f(StringTableEntry name, F32 value0)
{
   GLSLShaderParameter* param = getNamedParameter(name);
   
   U32 id = param->mId;
   
   if(id == -1)
      return false;
   
   glUniform1fARB(id, value0);
   return true;
}

bool GLSLShader::setParameter2f(StringTableEntry name, F32 value0, F32 value1)
{
   GLSLShaderParameter* param = getNamedParameter(name);
   
   U32 id = param->mId;
   
   if(id == -1)
      return false;
   
   glUniform2fARB(id, value0, value1);
   return true;
}

bool GLSLShader::setParameter3f(StringTableEntry name, F32 value0, F32 value1, F32 value2)
{
   GLSLShaderParameter* param = getNamedParameter(name);
   
   U32 id = param->mId;
   
   if(id == -1)
      return false;
   
   glUniform3fARB(id, value0, value1, value2);
   return true;
}

bool GLSLShader::setParameter4f(StringTableEntry name, F32 value0, F32 value1, F32 value2, F32 value3)
{
   GLSLShaderParameter* param = getNamedParameter(name);
   
   U32 id = param->mId;
   
   if(id == -1)
      return false;
   
   glUniform4fARB(id, value0, value1, value2, value3);
   return true;
}

bool GLSLShader::setParameter1i(StringTableEntry name, U32 value0)
{
   GLSLShaderParameter* param = getNamedParameter(name);
   
   if(param->mId == -1)
      return false;

   glUniform1iARB(param->mId, value0);
   return true;
}
   
bool GLSLShader::setParameter2i(StringTableEntry name, U32 value0, U32 value1)
{
   GLSLShaderParameter* param = getNamedParameter(name);
   
   if(param->mId == -1)
      return false;
      
   glUniform2iARB(param->mId, value0, value1);
   return true;
}

bool GLSLShader::setParameter3i(StringTableEntry name, U32 value0, U32 value1, U32 value2)
{
   GLSLShaderParameter* param = getNamedParameter(name);
   
   if(param->mId == -1)
      return false;
      
   glUniform3iARB(param->mId, value0, value1, value2);
   return true;
}

bool GLSLShader::setParameter4i(StringTableEntry name, U32 value0, U32 value1, U32 value2, U32 value3)
{
   GLSLShaderParameter* param = getNamedParameter(name);
   
   if(param->mId == -1)
      return false;
      
   glUniform4iARB(param->mId, value0, value1, value2, value3);
   return true;
}

bool GLSLShader::setTextureSampler(StringTableEntry name, U32 value0)
{
   return setParameter1i(name, value0);
}

bool GLSLShader::setParameter4x4f(StringTableEntry name, bool transpose, MatrixF *mat)
{
   GLSLShaderParameter* param = getNamedParameter(name);
   
   if(param->mId == -1)
      return false;

   glUniformMatrix4fvARB(param->mId, 1, transpose, (F32*)mat);
   return true;
}

void GLSLShader::reloadParameters()
{
   // We store parameters which don't actually exist in the shader
   // to save the time required to continuously check if the parameter exists
   // when chances are it doesn't.  This function clears the parameter list
   // forcing a reload of *ALL* parameters, so if we have added one, it will
   // be properly detected.
   for(U32 i = 0; i < mParameters.size(); i++)
   {
      delete mParameters[i];
   }
   mParameters.clear();
}

void GLSLShader::bind()
{
   glUseProgramObjectARB(mProgram);
}

void GLSLShader::unbind()
{
   glUseProgramObjectARB(0);
}

void GLSLShader::reset()
{
   glDetachObjectARB(mProgram, mVertexShader);
   glDetachObjectARB(mProgram, mFragmentShader);
   glDeleteObjectARB(mVertexShader);
   glDeleteObjectARB(mFragmentShader);
   glDeleteObjectARB(mProgram);
   for(U32 i = 0; i < mParameters.size(); i++)
   {
      delete mParameters[i];
   }
   mParameters.clear();
}

void GLSLShader::kill()
{
   glDetachObjectARB(mProgram, mVertexShader);
   glDetachObjectARB(mProgram, mFragmentShader);
   glDeleteObjectARB(mVertexShader);
   glDeleteObjectARB(mFragmentShader);
   glDeleteObjectARB(mProgram);
   for(U32 i = 0; i < mParameters.size(); i++)
   {
      delete mParameters[i];
   }
   mParameters.clear();
   dFree(mPixelSourceString);
   dFree(mVertexSourceString);
}

void GLSLShader::resurrect()
{
   mVertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
   mFragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
   mProgram = glCreateProgramObjectARB();
   
   const char* vsSourceString = reinterpret_cast<const char*>(mVertexSourceString);
   const char* psSourceString = reinterpret_cast<const char*>(mPixelSourceString);
   
   
   if(mVertexSourceString)
   {
      glShaderSourceARB(mVertexShader, 1,
                        &vsSourceString, NULL);
      glCompileShaderARB(mVertexShader);
      glAttachObjectARB(mProgram, mVertexShader);
   }
   if(mPixelSourceString)
   {
      glShaderSourceARB(mFragmentShader, 1,
                        &psSourceString, NULL);
      glCompileShaderARB(mFragmentShader);
      glAttachObjectARB(mProgram, mFragmentShader);
   }
   
   glLinkProgramARB(mProgram);
}

