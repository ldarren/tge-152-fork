/*  mkShaderManager.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/mkShaderManager.h"
#include "console/console.h"
#include "platform/platform.h"
#include "core/resManager.h"
#include "core/stringTable.h"
#include "core/frameAllocator.h"
#include "interior/interior.h"
#include "ts/tsShapeInstance.h"
#include "ModernizationKit/mkGLSLValidate.h"

#define SHADER_TYPE_SIZE 4
// allowable extensions for checking if a shader pair exists
static const char* shaderExtArray[SHADER_TYPE_SIZE] = {"Pixl.glsl", "Pixl.cgsh", "Pixl.arbp", "Pixl.atit"};
static const char* shaderExtTypeArray[SHADER_TYPE_SIZE] = {".glsl", ".cgsh", ".arbp", ".atit"};
_ShaderManager *ShaderManager = NULL;
//_ATIShaderParser *ATIShaderParser = NULL;

//#pragma mark ---- Shader Manager Initialization ----

_ShaderManager::_ShaderManager()
{
   mCurrBound = NULL;
   mDefaultParamValue = 0.0f;
   
   mValidateVertex = 0;
   mValidateFragment = 0;

   mSupportsGLSLPixl = false;
   mSupportsASMPixl = false;
   mSupportsCgPixl = false;
   mSupportsATITPixl = false;
   mSupportsGLSLVert = false;
   mSupportsASMVert = false;
   mSupportsCgVert = false;
   mSupportsATITVert = false;
   
   mPixelShaderVersion = 0.0f;
   mVertexShaderVersion = 0.0f;
#ifdef SHADER_MANAGER_USE_CG
   mCGContext = NULL;
   mCGFragmentProfile = CG_PROFILE_UNKNOWN;
   mCGVertexProfile = CG_PROFILE_UNKNOWN;
   mCGInit = false;
#endif
}

_ShaderManager::~_ShaderManager()
{
   kill();
}

void _ShaderManager::create()
{
   AssertFatal(ShaderManager == NULL, "ShaderManager::create: ShaderManager already exists.");
   ShaderManager = new _ShaderManager;
   // Tell script what the shader types are
   Con::setIntVariable("$ShaderTypes::GLSLShaderType",      GLSLShaderType);
   Con::setIntVariable("$ShaderTypes::CGShaderType",        CGShaderType);
   Con::setIntVariable("$ShaderTypes::ASMShaderType",       ASMShaderType);
   Con::setIntVariable("$ShaderTypes::ATITextShaderType",   ATITextShaderType);
   ShaderManager->init();
   //ATIShaderParser = new _ATIShaderParser;
}

void _ShaderManager::destroy()
{
   AssertFatal(ShaderManager != NULL, "ShaderManager::destroy: ShaderManager does not exist.");
   delete ShaderManager;
   ShaderManager = NULL;
   //delete ATIShaderParser;
   //ATIShaderParser = NULL;
}

void _ShaderManager::init()
{
   const char* extString = (const char*) glGetString( GL_EXTENSIONS );
   AssertFatal(extString, "We need the extension string to init the shader manager!");
      
   if (dStrstr(extString, (const char*)"GL_ARB_vertex_shader") != NULL)
      mSupportsGLSLVert = true;
   
   if (dStrstr(extString, (const char*)"GL_ARB_fragment_shader") != NULL)
      mSupportsGLSLPixl = true;
      
   if (dStrstr(extString, (const char*)"GL_ARB_vertex_program") != NULL)
   {
      mSupportsASMVert = true;
      mSupportsATITVert = true;
   }
   
   if(dStrstr(extString, (const char*)"GL_ATI_text_fragment_shader") != NULL)
   {
      mSupportsATITPixl = true;
   }
      
   if (dStrstr(extString, (const char*)"GL_ARB_fragment_program") != NULL)
      mSupportsASMPixl = true;
   
   if(mSupportsGLSLVert && !mSupportsASMVert)
   {
      Con::errorf("We support GLSL but not ASM vertex shaders?  Will play it safe and disable GLSL shaders");
      mSupportsGLSLVert = false;
   }
   
   if(mSupportsGLSLPixl && !mSupportsASMPixl)
   {
      Con::errorf("We support GLSL but not ASM pixel shaders?  Will play it safe and disable GLSL shaders");
      mSupportsGLSLPixl = false;
   }
   
   bool suppShaderLOD = false;
   bool suppNVVert3 = false;
   
   if (dStrstr(extString, (const char*)"GL_ATI_shader_texture_lod") != NULL)
      suppShaderLOD = true;
      
   if (dStrstr(extString, (const char*)"GL_NV_vertex_program3") != NULL)
      suppNVVert3 = true;
      
   if(suppShaderLOD || suppNVVert3)
   {
      mPixelShaderVersion = 3.0f;
      mVertexShaderVersion = 3.0f;
   }
   else if(mSupportsGLSLPixl || mSupportsASMPixl)
   {
      mPixelShaderVersion = 2.0f;
      mVertexShaderVersion = 2.0f;
   }
   else if(mSupportsASMVert)
   {
      mVertexShaderVersion = 1.1f;
   }
   
   if(mSupportsATITPixl && mPixelShaderVersion < 1.9f)
   {
      mPixelShaderVersion = 1.4f;
   }

#ifdef SHADER_MANAGER_USE_CG      
   initCG();
#endif
   Con::setFloatVariable("$PixelShaderVersion", mPixelShaderVersion);
   Con::setFloatVariable("$VertexShaderVersion", mVertexShaderVersion);
}

#ifdef SHADER_MANAGER_USE_CG
void _ShaderManager::initCG()
{
   mCGInit = true;
   mCGContext = cgCreateContext();
   
   mCGVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
   mCGFragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
   
   if(mCGVertexProfile != CG_PROFILE_UNKNOWN)
      mSupportsCgVert = true;
   
   if(mCGFragmentProfile != CG_PROFILE_UNKNOWN)
      mSupportsCgPixl = true;
   
   if(mCGFragmentProfile == CG_PROFILE_FP20)
      mPixelShaderVersion = 1.1f;
}
#endif

//#pragma mark ---- Shader Creation ----

bool _ShaderManager::confirmSupport(bool vertShader, bool fragShader, ShaderType type)
{
   switch (type)
   {
      case GLSLShaderType:
      if(vertShader && !mSupportsGLSLVert)
         return false;
      if(fragShader && !mSupportsGLSLPixl)
         return false;
      break;
#ifdef SHADER_MANAGER_USE_CG
      case CGShaderType:
      if(vertShader && !mSupportsCgVert)
         return false;
      if(fragShader && !mSupportsCgPixl)
         return false;
      break;
#endif
      
      case ASMShaderType:
      if(vertShader && !mSupportsASMVert)
         return false;
      if(fragShader && !mSupportsASMPixl)
         return false;
      break;
      
      case ATITextShaderType:
      if(vertShader && !mSupportsATITVert)
         return false;
      if(fragShader && !mSupportsATITPixl)
         return false;
      break;
      
      default:
      return false;
   }
   return true;
}

Shader* _ShaderManager::createShader(const char* vertPath, const char* fragPath, ShaderType type)
{
   if(!vertPath && !fragPath)
   {
      AssertFatal(false, "Come on, you have to give the shader manager at least one file to load");
      // Qu'est-ce que c'est?!
      return NULL;
   }
   
   // Make sure we *can* load the shader
   if(!confirmSupport(vertPath != NULL, fragPath != NULL, type))
   {
      return NULL;
   }
   
   StringTableEntry vert = NULL, frag = NULL;
   if(vertPath)
      vert = StringTable->insert(vertPath);
   
   if(fragPath)
      frag = StringTable->insert(fragPath);
   
   for(U32 i = 0; i < mShaders.size(); i++)
   {
      bool match = false;
      if(mShaders[i]->mVertexFilepath == vert)
      {
         match = true;
      }
      else
      {
         continue;
      }
      
      if(mShaders[i]->mFragFilepath == frag)
      {
         match = true;
      }
      else
      {
         continue;
      }
      
      if(match)
         return mShaders[i];
   }

   Shader* shd = NULL;
   switch (type)
   {
      case GLSLShaderType:
         shd = new GLSLShader;
         break;
#ifdef SHADER_MANAGER_USE_CG
      case CGShaderType:
         shd = new CGShader;
         break;
#endif
      
      case ASMShaderType:
         shd = new ASMShader;
         break;
      
      case ATITextShaderType:
         shd = new ATITShader;
         break;
   }
   shd->mType = type;
   shd->loadShaderFromFiles(vertPath, fragPath);
   if(shd)
      mShaders.push_back(shd);
   return shd;
}
   

Shader* _ShaderManager::compileShaderFromSource(const char* vertSource, const char* fragSource, 
                                                    bool includeLibs, bool link, ShaderType type)
{
   Shader* shd = NULL;
   switch(type)
   {
      case GLSLShaderType :
         shd = new GLSLShader;
         shd->mType = GLSLShaderType;
         break;
#ifdef SHADER_MANAGER_USE_CG
      case CGShaderType :
         shd = new CGShader;
         shd->mType = CGShaderType;
         break;
#endif
      default :
         AssertFatal(false, "We only support Cg and GLSL at the moment");
   }
   
   shd->compileShaderFromSource(vertSource, fragSource, includeLibs, link);
   if(shd)
      mShaders.push_back(shd);
   
   return shd;
}

void _ShaderManager::deleteShader(Shader* shd)
{
   if(!shd)
      return;

   for(U32 i = 0; i < mShaders.size(); i++)
      if(mShaders[i] == shd)
         mShaders.erase(i);
         
   shd->kill();
   delete shd;
   return;
}

//#pragma mark ---- Context Switch Code ----

void _ShaderManager::reset()
{
#ifdef SHADER_MANAGER_USE_CG
   cgDestroyContext(mCGContext);
#endif

   for(U32 i = 0; i < mShaders.size(); i++)
   {
      mShaders[i]->reset();
   }
}

void _ShaderManager::resurrect()
{
#ifdef SHADER_MANAGER_USE_CG
   initCG();
#endif
   for(U32 i = 0; i < mShaders.size(); i++)
   {
      mShaders[i]->resurrect();
   }
}

void _ShaderManager::kill()
{
   Con::printf("Killing shader manager");
   if(Con::getBoolVariable("$pref::shaderManager::exportGlobalParameters"))
   {
      exportGlobalParameters();
   }

#ifdef SHADER_MANAGER_USE_CG
   cgDestroyContext(mCGContext);
#endif
   GLSLValidate::kill();
   if(mValidateVertex)
      glDeleteObjectARB(mValidateVertex);
   if(mValidateFragment)
      glDeleteObjectARB(mValidateFragment);
   mGlobal4x4fParameters.clear();
   mGlobal1fParameters.clear();
   mGlobal2fParameters.clear();
   mGlobal3fParameters.clear();
   mGlobal4fParameters.clear();

   for(U32 i = 0; i < mShaders.size(); i++)
   {
      mShaders[i]->kill();
      delete mShaders[i];
   }
   mShaders.clear();
}

//#pragma mark ---- Global Parameter Exporting ----

void _ShaderManager::exportGlobalParameters()
{
   const char* path = Con::getVariable("$pref::shaderManager::exportFile");
   if(dStrlen(path) == 0)
   {
      Con::setVariable("$pref::shaderManager::exportFile", "Shaders/globalParameters.cs");
      path = Con::getVariable("$pref::shaderManager::exportFile");
   }
   
   FileStream fStream;
   if(!fStream.open(path, FileStream::Write))
   {
      Con::errorf("Could not open global parameters file!");
      return;
   }
   
   Con::printf("Exporting global parameters");
   
   for(U32 i = 0; i < mGlobal1fParameters.size(); i++)
   {
      mGlobal1fParameters[i].save(fStream);
   }
   
   for(U32 i = 0; i < mGlobal2fParameters.size(); i++)
   {
      mGlobal2fParameters[i].save(fStream);
   }
   
   for(U32 i = 0; i < mGlobal3fParameters.size(); i++)
   {
      mGlobal3fParameters[i].save(fStream);
   }
   
   for(U32 i = 0; i < mGlobal4fParameters.size(); i++)
   {
      mGlobal4fParameters[i].save(fStream);
   }
   
   fStream.close();
}

void Global1f::save(FileStream &fStream)
{
   if(!mShouldSave)
      return;

   char buff[256];
   dSprintf(buff, sizeof(buff), "ShaderManager::setGlobalParameter(\"%s\", %f);\r\n", 
                                mName, value0);
   fStream.write(dStrlen(buff), buff);
   
   dSprintf(buff, sizeof(buff), "ShaderManager::setGlobalParameterLimits(\"%s\", %f, %f);\r\n",
                                mName, minX, maxX);
   fStream.write(dStrlen(buff), buff);
}

void Global2f::save(FileStream &fStream)
{
   if(!mShouldSave)
      return;

   char buff[256];
   dSprintf(buff, sizeof(buff), "ShaderManager::setGlobalParameter(\"%s\", %f, %f);\r\n", 
                                mName, value0, value1);
   fStream.write(dStrlen(buff), buff);
   
   dSprintf(buff, sizeof(buff), "ShaderManager::setGlobalParameterLimits(\"%s\", %f, %f, %f, %f);\r\n",
                                mName, minX, maxX, minY, maxY);
   fStream.write(dStrlen(buff), buff);
}

void Global3f::save(FileStream &fStream)
{
   if(!mShouldSave)
      return;

   char buff[256];
   dSprintf(buff, sizeof(buff), "ShaderManager::setGlobalParameter(\"%s\", %f, %f, %f);\r\n", 
                                mName, value0, value1, value2);
   fStream.write(dStrlen(buff), buff);
   
   dSprintf(buff, sizeof(buff), "ShaderManager::setGlobalParameterLimits(\"%s\", %f, %f, %f, %f, %f, %f);\r\n",
                                mName, minX, maxX, minY, maxY, minZ, maxZ);
   fStream.write(dStrlen(buff), buff);
}

void Global4f::save(FileStream &fStream)
{
   if(!mShouldSave)
      return;

   char buff[256];
   dSprintf(buff, sizeof(buff), "ShaderManager::setGlobalParameter(\"%s\", %f, %f, %f, %f);\r\n", 
                                mName, value0, value1, value2, value3);
   fStream.write(dStrlen(buff), buff);
   
   dSprintf(buff, sizeof(buff), "ShaderManager::setGlobalParameterLimits(\"%s\", %f, %f, %f, %f, %f, %f, %f, %f);\r\n",
                                mName, minX, maxX, minY, maxY, minZ, maxZ, minW, maxW);
   fStream.write(dStrlen(buff), buff);
}

//#pragma mark ---- GLSL Validation ----

void _ShaderManager::validateGLSL()
{
   if(!dglDoesSupportFragmentShader())
   {
      Con::errorf("GLSL not supported!");
      return;
   }

   U32 passed = 0;
   U32 failed = 0;
   U32 len = 0;
   Con::printf("Validating GLSL implementation...");
   Con::printf("");
   char intro[512];
   dStrcpy(intro, "Shader");
   
   len = dStrlen(intro);
   for(U32 j = len; j < 40; j++)
      intro[j] = ' ';
   dStrcpy(intro + 40, "Expected");
   
   len = dStrlen(intro);
   for(U32 j = len; j < 53; j++)
      intro[j] = ' ';
   dStrcpy(intro + 53, "Actual");

   Con::printf(intro);
   
   for(U32 i = 0; i < GLSLValidate::getNumShaders(); i++)
   {
      char print[512];
      ValidationShader shader = GLSLValidate::shaders[i];
      bool success = validateGLSLShader(shader.file, shader.vertex);
      dStrcpy(print, shader.file);
      len = dStrlen(print);
      
      for(U32 j = len; j < 40; j++)
         print[j] = ' ';
      dStrcpy(print + 40, shader.pass ? "success" : "error");
      
      len = dStrlen(print);
      for(U32 j = len; j < 53; j++)
         print[j] = ' ';
      dStrcpy(print + 53, success ? "success" : "error");
      
      len = dStrlen(print);
      for(U32 j = len; j < 65; j++)
         print[j] = ' ';
      dStrcpy(print + 65, (shader.pass == success) ? "pass" : "fail");
      if(shader.pass == success)
      {
         passed++;
         Con::printf(print);
      }
      else
      {
         failed++;
         Con::errorf(print);
      }
   }
   Con::printf("Completed validation.  %i passed, %i failed, score is %g percent", 
                passed, failed, (F32)passed / (F32)(passed + failed) * 100.0f);
   Con::printf("");
}

bool _ShaderManager::validateGLSLShader(StringTableEntry file, bool vertex)
{
   if(!mValidateVertex)
      mValidateVertex = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);

   if(!mValidateFragment)
      mValidateFragment = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
      
   U32 storedWaterMark = FrameAllocator::getWaterMark();
   U8 *vs;
   char fileNameBuffer[512];
   dStrcpy(fileNameBuffer, "Shaders/GLSLValidate/");
   U32 len = dStrlen(fileNameBuffer);
   dStrcpy(fileNameBuffer + len, file);
   Stream *s = ResourceManager->openStream(fileNameBuffer);
   U32 bufferSize = ResourceManager->getSize(fileNameBuffer);
   vs = (U8 *) FrameAllocator::alloc((bufferSize + 1) * sizeof(U8));
   vs[bufferSize] = 0;
   s->read(bufferSize, vs);
   ResourceManager->closeStream(s);
   
   const char* vv = reinterpret_cast<const char*>(vs);
   
   GLint success;
   
   if(vertex)
   {
      glShaderSourceARB(mValidateVertex, 1, &vv, NULL);
      glCompileShaderARB(mValidateVertex);
      glGetObjectParameterivARB(mValidateVertex, GL_OBJECT_COMPILE_STATUS_ARB, &success);
   }
   else
   {
      glShaderSourceARB(mValidateFragment, 1, &vv, NULL);
      glCompileShaderARB(mValidateFragment);
      glGetObjectParameterivARB(mValidateFragment, GL_OBJECT_COMPILE_STATUS_ARB, &success);
   }
   
   FrameAllocator::setWaterMark(storedWaterMark);
   
   return success == 1;
}

//#pragma mark ---- Global Parameter Management ----

void _ShaderManager::setGlobalParameter(StringTableEntry name, bool transpose, MatrixF m)
{
   name = StringTable->insert(name);

   for(U32 i = 0; i < mGlobal4x4fParameters.size(); i++)
   {
      if(name == mGlobal4x4fParameters[i].mName)
      {
         mGlobal4x4fParameters[i].m = m;
         mGlobal4x4fParameters[i].mTranspose = transpose;
         return;
      }
   }
   
   for(U32 i = 0; i < mGlobal1fParameters.size(); i++)
   {
      if(name == mGlobal1fParameters[i].mName)
      {
         mGlobal1fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal2fParameters.size(); i++)
   {
      if(name == mGlobal2fParameters[i].mName)
      {
         mGlobal2fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal3fParameters.size(); i++)
   {
      if(name == mGlobal3fParameters[i].mName)
      {
         mGlobal3fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal4fParameters.size(); i++)
   {
      if(name == mGlobal4fParameters[i].mName)
      {
         mGlobal4fParameters.erase(i);
         break;
      }
   }
   
   Global4x4f global;
   global.m = m;
   global.mTranspose = transpose;
   global.mName = name;
   mGlobal4x4fParameters.push_back(global);
}

void _ShaderManager::setGlobalParameter(StringTableEntry name, F32 value0, bool save /*=true*/)
{
   name = StringTable->insert(name);

   for(U32 i = 0; i < mGlobal1fParameters.size(); i++)
   {
      if(name == mGlobal1fParameters[i].mName)
      {
         mGlobal1fParameters[i].value0 = value0;
         return;
      }
   }
   
   for(U32 i = 0; i < mGlobal2fParameters.size(); i++)
   {
      if(name == mGlobal2fParameters[i].mName)
      {
         mGlobal2fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal3fParameters.size(); i++)
   {
      if(name == mGlobal3fParameters[i].mName)
      {
         mGlobal3fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal4fParameters.size(); i++)
   {
      if(name == mGlobal4fParameters[i].mName)
      {
         mGlobal4fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal4x4fParameters.size(); i++)
   {
      if(name == mGlobal4x4fParameters[i].mName)
      {
         mGlobal4x4fParameters.erase(i);
         break;
      }
   }
   
   Global1f global;
   global.value0 = value0;
   global.mName = name;
   global.minX = 0.0f;
   global.maxX = 1.0f;
   global.mShouldSave = save;
   mGlobal1fParameters.push_back(global);
}

void _ShaderManager::setGlobalParameter(StringTableEntry name, F32 value0, F32 value1, bool save /*=true*/)
{
   name = StringTable->insert(name);

   for(U32 i = 0; i < mGlobal2fParameters.size(); i++)
   {
      if(name == mGlobal2fParameters[i].mName)
      {
         mGlobal2fParameters[i].value0 = value0;
         mGlobal2fParameters[i].value1 = value1;
         return;
      }
   }
      
   for(U32 i = 0; i < mGlobal1fParameters.size(); i++)
   {
      if(name == mGlobal1fParameters[i].mName)
      {
         mGlobal1fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal3fParameters.size(); i++)
   {
      if(name == mGlobal3fParameters[i].mName)
      {
         mGlobal3fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal4fParameters.size(); i++)
   {
      if(name == mGlobal4fParameters[i].mName)
      {
         mGlobal4fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal4x4fParameters.size(); i++)
   {
      if(name == mGlobal4x4fParameters[i].mName)
      {
         mGlobal4x4fParameters.erase(i);
         break;
      }
   }
   
   Global2f global;
   global.value0 = value0;
   global.value1 = value1;
   global.mName = name;
   global.minX = 0.0f;
   global.maxX = 1.0f;
   global.minY = 0.0f;
   global.maxY = 1.0f;
   global.mShouldSave = save;
   mGlobal2fParameters.push_back(global);
}

void _ShaderManager::setGlobalParameter(StringTableEntry name, F32 value0, F32 value1, F32 value2, bool save /*=true*/)
{
   name = StringTable->insert(name);

   for(U32 i = 0; i < mGlobal3fParameters.size(); i++)
   {
      if(name == mGlobal3fParameters[i].mName)
      {
         mGlobal3fParameters[i].value0 = value0;
         mGlobal3fParameters[i].value1 = value1;
         mGlobal3fParameters[i].value2 = value2;
         return;
      }
   }
   
   for(U32 i = 0; i < mGlobal1fParameters.size(); i++)
   {
      if(name == mGlobal1fParameters[i].mName)
      {
         mGlobal1fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal2fParameters.size(); i++)
   {
      if(name == mGlobal2fParameters[i].mName)
      {
         mGlobal2fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal4fParameters.size(); i++)
   {
      if(name == mGlobal4fParameters[i].mName)
      {
         mGlobal4fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal4x4fParameters.size(); i++)
   {
      if(name == mGlobal4x4fParameters[i].mName)
      {
         mGlobal4x4fParameters.erase(i);
         break;
      }
   }
   
   Global3f global;
   global.value0 = value0;
   global.value1 = value1;
   global.value2 = value2;
   global.mName = name;
   global.minX = 0.0f;
   global.maxX = 1.0f;
   global.minY = 0.0f;
   global.maxY = 1.0f;
   global.minZ = 0.0f;
   global.maxZ = 1.0f;
   global.mShouldSave = save;
   mGlobal3fParameters.push_back(global);
}

void _ShaderManager::setGlobalParameter(StringTableEntry name, F32 value0, F32 value1, F32 value2, F32 value3, bool save /*=true*/)
{
   name = StringTable->insert(name);

   for(U32 i = 0; i < mGlobal4fParameters.size(); i++)
   {
      if(name == mGlobal4fParameters[i].mName)
      {
         mGlobal4fParameters[i].value0 = value0;
         mGlobal4fParameters[i].value1 = value1;
         mGlobal4fParameters[i].value2 = value2;
         mGlobal4fParameters[i].value3 = value3;
         return;
      }
   }
      
   for(U32 i = 0; i < mGlobal1fParameters.size(); i++)
   {
      if(name == mGlobal1fParameters[i].mName)
      {
         mGlobal1fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal2fParameters.size(); i++)
   {
      if(name == mGlobal2fParameters[i].mName)
      {
         mGlobal2fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal3fParameters.size(); i++)
   {
      if(name == mGlobal3fParameters[i].mName)
      {
         mGlobal3fParameters.erase(i);
         break;
      }
   }
   
   for(U32 i = 0; i < mGlobal4x4fParameters.size(); i++)
   {
      if(name == mGlobal4x4fParameters[i].mName)
      {
         mGlobal4x4fParameters.erase(i);
         break;
      }
   }
   
   Global4f global;
   global.value0 = value0;
   global.value1 = value1;
   global.value2 = value2;
   global.value3 = value3;
   global.mName = name;
   global.minX = 0.0f;
   global.maxX = 1.0f;
   global.minY = 0.0f;
   global.maxY = 1.0f;
   global.minZ = 0.0f;
   global.maxZ = 1.0f;
   global.minW = 0.0f;
   global.maxW = 1.0f;
   global.mShouldSave = save;
   mGlobal4fParameters.push_back(global);
}

void _ShaderManager::setGlobalParameterLimits(StringTableEntry name, F32 minX, F32 maxX)
{
   name = StringTable->insert(name);

   for(U32 i = 0; i < mGlobal1fParameters.size(); i++)
   {
      if(name == mGlobal1fParameters[i].mName)
      {
         mGlobal1fParameters[i].minX = minX;
         mGlobal1fParameters[i].maxX = maxX;
         return;
      }
   }
}

void _ShaderManager::setGlobalParameterLimits(StringTableEntry name, F32 minX, F32 maxX, F32 minY, F32 maxY)
{
   name = StringTable->insert(name);

   for(U32 i = 0; i < mGlobal2fParameters.size(); i++)
   {
      if(name == mGlobal2fParameters[i].mName)
      {
         mGlobal2fParameters[i].minX = minX;
         mGlobal2fParameters[i].maxX = maxX;
         mGlobal2fParameters[i].minY = minY;
         mGlobal2fParameters[i].maxY = maxY;
         return;
      }
   }
}

void _ShaderManager::setGlobalParameterLimits(StringTableEntry name, F32 minX, F32 maxX, F32 minY, 
                                                                     F32 maxY, F32 minZ, F32 maxZ)
{
   name = StringTable->insert(name);

   for(U32 i = 0; i < mGlobal3fParameters.size(); i++)
   {
      if(name == mGlobal3fParameters[i].mName)
      {
         mGlobal3fParameters[i].minX = minX;
         mGlobal3fParameters[i].maxX = maxX;
         mGlobal3fParameters[i].minY = minY;
         mGlobal3fParameters[i].maxY = maxY;
         mGlobal3fParameters[i].minZ = minZ;
         mGlobal3fParameters[i].maxZ = maxZ;
         return;
      }
   }
}

void _ShaderManager::setGlobalParameterLimits(StringTableEntry name, F32 minX, F32 maxX, F32 minY, F32 maxY, 
                                                                     F32 minZ, F32 maxZ, F32 minW, F32 maxW)
{
   name = StringTable->insert(name);

   for(U32 i = 0; i < mGlobal4fParameters.size(); i++)
   {
      if(name == mGlobal4fParameters[i].mName)
      {
         mGlobal4fParameters[i].minX = minX;
         mGlobal4fParameters[i].maxX = maxX;
         mGlobal4fParameters[i].minY = minY;
         mGlobal4fParameters[i].maxY = maxY;
         mGlobal4fParameters[i].minZ = minZ;
         mGlobal4fParameters[i].maxZ = maxZ;
         mGlobal4fParameters[i].minW = minW;
         mGlobal4fParameters[i].maxW = maxW;
         return;
      }
   }
}

void _ShaderManager::deleteGlobalParameter(StringTableEntry name)
{
   name = StringTable->insert(name);

   for(U32 i = 0; i < mGlobal1fParameters.size(); i++)
   {
      if(name == mGlobal1fParameters[i].mName)
      {
         mGlobal1fParameters.erase(i);
         for(U32 i = 0; i < mShaders.size(); i++)
         {
            mShaders[i]->bind();
            mShaders[i]->setParameter1f(name, 0.0f);
            mShaders[i]->unbind();
         }
         return;
      }
   }
   
   for(U32 i = 0; i < mGlobal2fParameters.size(); i++)
   {
      if(name == mGlobal2fParameters[i].mName)
      {
         mGlobal2fParameters.erase(i);
         for(U32 i = 0; i < mShaders.size(); i++)
         {
            mShaders[i]->bind();
            mShaders[i]->setParameter2f(name, 0.0f, 0.0f);
            mShaders[i]->unbind();
         }
         return;
      }
   }
   
   for(U32 i = 0; i < mGlobal3fParameters.size(); i++)
   {
      if(name == mGlobal3fParameters[i].mName)
      {
         mGlobal3fParameters.erase(i);
         for(U32 i = 0; i < mShaders.size(); i++)
         {
            mShaders[i]->bind();
            mShaders[i]->setParameter3f(name, 0.0f, 0.0f, 0.0f);
            mShaders[i]->unbind();
         }
         return;
      }
   }
   
   for(U32 i = 0; i < mGlobal4fParameters.size(); i++)
   {
      if(name == mGlobal4fParameters[i].mName)
      {
         mGlobal4fParameters.erase(i);
         for(U32 i = 0; i < mShaders.size(); i++)
         {
            mShaders[i]->bind();
            mShaders[i]->setParameter4f(name, 0.0f, 0.0f, 0.0f, 0.0f);
            mShaders[i]->unbind();
         }
         return;
      }
   }
   
   for(U32 i = 0; i < mGlobal4x4fParameters.size(); i++)
   {
      if(name == mGlobal4x4fParameters[i].mName)
      {
         mGlobal4x4fParameters.erase(i);
         return;
      }
   }
}

//#pragma mark ---- Shader Management ----

void _ShaderManager::unbindShader()
{
   if(mCurrBound)
      mCurrBound->unbind();
      
   mCurrBound = NULL;
}

void _ShaderManager::bindShader(Shader* shd)
{
   if(mCurrBound)
      mCurrBound->unbind();

   shd->bind();
   mCurrBound = shd;
   for(U32 i = 0; i < mGlobal4x4fParameters.size(); i++)
   {
      mCurrBound->setParameter4x4f(mGlobal4x4fParameters[i].mName, mGlobal4x4fParameters[i].mTranspose,
                                   &mGlobal4x4fParameters[i].m);
   }
   for(U32 i = 0; i < mGlobal1fParameters.size(); i++)
   {
      mCurrBound->setParameter1f(mGlobal1fParameters[i].mName, mGlobal1fParameters[i].value0);
   }
   for(U32 i = 0; i < mGlobal2fParameters.size(); i++)
   {
      mCurrBound->setParameter2f(mGlobal2fParameters[i].mName, mGlobal2fParameters[i].value0,
                                 mGlobal2fParameters[i].value1);
   }
   for(U32 i = 0; i < mGlobal3fParameters.size(); i++)
   {
      mCurrBound->setParameter3f(mGlobal3fParameters[i].mName, mGlobal3fParameters[i].value0,
                                 mGlobal3fParameters[i].value1, mGlobal3fParameters[i].value2);
   }
   for(U32 i = 0; i < mGlobal4fParameters.size(); i++)
   {
      mCurrBound->setParameter4f(mGlobal4fParameters[i].mName, mGlobal4fParameters[i].value0,
                                 mGlobal4fParameters[i].value1, mGlobal4fParameters[i].value2,
                                 mGlobal4fParameters[i].value3);
   }
}

void _ShaderManager::reloadShaders()
{
   U32 size = mShaders.size();
   StringTableEntry filepath, vertPath, fragPath;
   Shader* shd;
   
   for(U32 i = 0; i < size; i++)
   {
      filepath = mShaders[i]->mFilepath;
      if(!filepath || !filepath[0])
      {
         vertPath = mShaders[i]->mVertexFilepath;
         fragPath = mShaders[i]->mFragFilepath;
         mShaders[i]->kill();
         mShaders[i]->loadShaderFromFiles(vertPath, fragPath);
         continue;
      }
   }
}

bool _ShaderManager::doesSupportShaders()
{
   bool support;
   support = dglDoesSupportFragmentShader();
   #ifdef SHADER_MANAGER_USE_CG
   if(!mCGInit)
      initCG();
   support |= (mCGFragmentProfile != CG_PROFILE_UNKNOWN);
   #endif
   support |= dglDoesSupportATITextFragmentShader();
   support |= dglDoesSupportATIFragmentShader();
   support |= dglDoesSupportARBFragmentProgram();
   
   return support;
}

//#pragma mark ---- Parameter Setting ----

bool _ShaderManager::setParameter(StringTableEntry name, F32 value0)
{
   if(mCurrBound)
      return mCurrBound->setParameter1f(name, value0);
      
   AssertFatal(false, "No currently bound shader!");
   return false;
}

bool _ShaderManager::setParameter(StringTableEntry name, F32 value0, F32 value1)
{
   if(mCurrBound)
      return mCurrBound->setParameter2f(name, value0, value1);
      
   AssertFatal(false, "No currently bound shader!");
   return false;
}

bool _ShaderManager::setParameter(StringTableEntry name, F32 value0, F32 value1, F32 value2)
{
   if(mCurrBound)
      return mCurrBound->setParameter3f(name, value0, value1, value2);
      
   AssertFatal(false, "No currently bound shader!");
   return false;
}

bool _ShaderManager::setParameter(StringTableEntry name, F32 value0, F32 value1, F32 value2, F32 value3)
{
   if(mCurrBound)
      return mCurrBound->setParameter4f(name, value0, value1, value2, value3);
      
   AssertFatal(false, "No currently bound shader!");
   return false;
}

bool _ShaderManager::setParameter(StringTableEntry name, U32 value0)
{
   if(mCurrBound)
      return mCurrBound->setParameter1i(name, value0);
      
   AssertFatal(false, "No currently bound shader!");
   return false;
}

bool _ShaderManager::setParameter(StringTableEntry name, U32 value0, U32 value1)
{
   if(mCurrBound)
      return mCurrBound->setParameter2i(name, value0, value1);
      
   AssertFatal(false, "No currently bound shader!");
   return false;
}

bool _ShaderManager::setParameter(StringTableEntry name, U32 value0, U32 value1, U32 value2)
{
   if(mCurrBound)
      return mCurrBound->setParameter3i(name, value0, value1, value2);
      
   AssertFatal(false, "No currently bound shader!");
   return false;
}

bool _ShaderManager::setParameter(StringTableEntry name, U32 value0, U32 value1, U32 value2, U32 value3)
{
   if(mCurrBound)
      return mCurrBound->setParameter4i(name, value0, value1, value2, value3);
      
   AssertFatal(false, "No currently bound shader!");
   return false;
}

bool _ShaderManager::setTextureSampler(StringTableEntry name, U32 value0)
{
   if(mCurrBound)
      return mCurrBound->setTextureSampler(name, value0);
      
   AssertFatal(false, "No currently bound shader!");
   return false;
}

bool _ShaderManager::setParameter(StringTableEntry name, bool transpose, MatrixF *m)
{
   if(mCurrBound)
      return mCurrBound->setParameter4x4f(name, transpose, m);

   AssertFatal(false, "No currently bound shader!");
   return false;
}

//#pragma mark ---- Shader Virtual Class ----
//#pragma mark - You can ignore this

bool Shader::setParameter1f(StringTableEntry name, F32 value0)
{
   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
   return false;
}

bool Shader::setParameter2f(StringTableEntry name, F32 value0, F32 value1)
{
   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
   return false;
}

bool Shader::setParameter3f(StringTableEntry name, F32 value0, F32 value1, F32 value2)
{
   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
   return false;
}

bool Shader::setParameter4f(StringTableEntry name, F32 value0, F32 value1, F32 value2, F32 value3)
{
   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
   return false;
}

bool Shader::setParameter1i(StringTableEntry name, U32 value0)
{
   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
   return false;
}

bool Shader::setParameter2i(StringTableEntry name, U32 value0, U32 value1)
{
   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
   return false;
}

bool Shader::setParameter3i(StringTableEntry name, U32 value0, U32 value1, U32 value2)
{
   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
   return false;
}

bool Shader::setParameter4i(StringTableEntry name, U32 value0, U32 value1, U32 value2, U32 value3)
{
   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
   return false;
}

bool Shader::setTextureSampler(StringTableEntry name, U32 value0)
{
   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
   return false;
}

bool Shader::setParameter4x4f(StringTableEntry name, bool transpose, MatrixF *m)
{
//   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
   return false;
}

void Shader::updateStateMatrices()
{
   //stupid Cg only function
}

void Shader::reloadParameters()
{

}

void Shader::bind()
{
   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
}

void Shader::unbind()
{
   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
}

bool Shader::loadShaderFromFiles(const char* vertPath, const char* fragPath)
{
   return false;
}

bool Shader::compileShaderFromSource(const char* vertSource, const char* fragSource, bool includeLibs, bool link)
{
   return false;
}

const char* Shader::getProgramLog()
{
   const char* foo = "";
   return foo;
}

const char* Shader::getVertexLog()
{
   const char* foo = "";
   return foo;
}

const char* Shader::getFragmentLog()
{
   const char* foo = "";
   return foo;
}

void Shader::updateStatus()
{

}

void Shader::reset()
{
   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
}

void Shader::resurrect()
{
   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
}

void Shader::kill()
{
   AssertISV(false, "Pure virtual (essentially) function called.  Go tell Alex to finish the shader manager");
}

//#pragma mark ---- Global Parameter Functions ----

ConsoleStaticMethod(ShaderManager, setGlobalParameter, void, 3, 6, "(paramName, F32 value0, ...)")
{
   U32 numParams = argc - 2;
   F32 value0, value1, value2, value3;
   
   switch(argc)
   {
      case 6:
         ShaderManager->setGlobalParameter(argv[1], dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]), dAtof(argv[5]));
         break;
         
      case 5:
         ShaderManager->setGlobalParameter(argv[1], dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]));
         break;
    
      case 4:
         ShaderManager->setGlobalParameter(argv[1], dAtof(argv[2]), dAtof(argv[3]));
         break;
         
      case 3: 
         ShaderManager->setGlobalParameter(argv[1], dAtof(argv[2]));
         break;
   }
}

ConsoleStaticMethod(ShaderManager, setGlobalParameterLimits, void, 4, 10, "(paramName, F32 minX, F32 maxX, F32 minY ...)")
{
   if(argc == 5 || argc == 7 || argc == 9)
      return;

   U32 paramType = argc - 2;
   paramType /= 2;
   
   switch(paramType)
   {
      case 4:
         ShaderManager->setGlobalParameterLimits(argv[1], dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]), dAtof(argv[5]),
                                                          dAtof(argv[6]), dAtof(argv[7]), dAtof(argv[8]), dAtof(argv[9]));
         break;
         
      case 3:
         ShaderManager->setGlobalParameterLimits(argv[1], dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]), dAtof(argv[5]),
                                                          dAtof(argv[6]), dAtof(argv[7]));
         break;
         
      case 2:
         ShaderManager->setGlobalParameterLimits(argv[1], dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]), dAtof(argv[5]));
         break;
      
      case 1:
         ShaderManager->setGlobalParameterLimits(argv[1], dAtof(argv[2]), dAtof(argv[3]));
         break;
   }
}

ConsoleStaticMethod(ShaderManager, getGlobalParameterMaxLimits, const char*, 2, 2, "(paramName)")
{
  char * ret = Con::getReturnBuffer(512);
   for(U32 i = 0; i < ShaderManager->mGlobal1fParameters.size(); i++)
   {
      if(dStricmp(argv[1], ShaderManager->mGlobal1fParameters[i].mName) == 0)
      {
         dSprintf(ret, sizeof(ret), "%g %g %g %g", ShaderManager->mGlobal1fParameters[i].maxX,
                                                   0.0, 0.0, 0.0);
         return ret;
      }
   }
   
   for(U32 i = 0; i < ShaderManager->mGlobal2fParameters.size(); i++)
   {
      if(dStricmp(argv[1], ShaderManager->mGlobal2fParameters[i].mName) == 0)
      {
         dSprintf(ret, sizeof(ret), "%g %g %g %g", ShaderManager->mGlobal2fParameters[i].maxX,
                                                   ShaderManager->mGlobal2fParameters[i].maxY,
                                                   0.0, 0.0);
         return ret;
      }
   }
   
   for(U32 i = 0; i < ShaderManager->mGlobal3fParameters.size(); i++)
   {
      if(dStricmp(argv[1], ShaderManager->mGlobal3fParameters[i].mName) == 0)
      {
         dSprintf(ret, sizeof(ret), "%g %g %g %g", ShaderManager->mGlobal3fParameters[i].maxX,
                                                   ShaderManager->mGlobal3fParameters[i].maxY,
                                                   ShaderManager->mGlobal3fParameters[i].maxZ,
                                                   0.0);
         return ret;
      }
   }
   
   for(U32 i = 0; i < ShaderManager->mGlobal4fParameters.size(); i++)
   {
      if(dStricmp(argv[1], ShaderManager->mGlobal4fParameters[i].mName) == 0)
      {
         dSprintf(ret, sizeof(ret), "%g %g %g %g", ShaderManager->mGlobal4fParameters[i].maxX,
                                                   ShaderManager->mGlobal4fParameters[i].maxY,
                                                   ShaderManager->mGlobal4fParameters[i].maxZ,
                                                   ShaderManager->mGlobal4fParameters[i].maxW);
         return ret;
      }
   }
   
   dSprintf(ret, sizeof(ret), "%g %g %g %g", 1.0, 1.0, 1.0, 1.0);
   return ret;
}

ConsoleStaticMethod(ShaderManager, getGlobalParameterMinLimits, const char*, 2, 2, "(paramName)")
{
  char * ret = Con::getReturnBuffer(512);
   for(U32 i = 0; i < ShaderManager->mGlobal1fParameters.size(); i++)
   {
      if(dStricmp(argv[1], ShaderManager->mGlobal1fParameters[i].mName) == 0)
      {
         dSprintf(ret, sizeof(ret), "%g %g %g %g", ShaderManager->mGlobal1fParameters[i].minX, 
                                                               0.0, 0.0, 0.0);
         return ret;
      }
   }
   
   for(U32 i = 0; i < ShaderManager->mGlobal2fParameters.size(); i++)
   {
      if(dStricmp(argv[1], ShaderManager->mGlobal2fParameters[i].mName) == 0)
      {
         dSprintf(ret, sizeof(ret), "%g %g %g %g", ShaderManager->mGlobal2fParameters[i].minX, 
                                                               ShaderManager->mGlobal2fParameters[i].minY,
                                                               0.0, 0.0);
         return ret;
      }
   }
   
   for(U32 i = 0; i < ShaderManager->mGlobal3fParameters.size(); i++)
   {
      if(dStricmp(argv[1], ShaderManager->mGlobal3fParameters[i].mName) == 0)
      {
         dSprintf(ret, sizeof(ret), "%g %g %g %g", ShaderManager->mGlobal3fParameters[i].minX, 
                                                               ShaderManager->mGlobal3fParameters[i].minY,
                                                               ShaderManager->mGlobal3fParameters[i].minZ,
                                                               0.0);
         return ret;
      }
   }
   
   for(U32 i = 0; i < ShaderManager->mGlobal4fParameters.size(); i++)
   {
      if(dStricmp(argv[1], ShaderManager->mGlobal4fParameters[i].mName) == 0)
      {
         dSprintf(ret, sizeof(ret), "%g %g %g %g", ShaderManager->mGlobal4fParameters[i].minX, 
                                                               ShaderManager->mGlobal4fParameters[i].minY,
                                                               ShaderManager->mGlobal4fParameters[i].minZ,
                                                               ShaderManager->mGlobal4fParameters[i].minW);
         return ret;
      }
   }
   
   dSprintf(ret, sizeof(ret), "%g %g %g %g", 0.0, 0.0, 0.0, 0.0);
   return ret;
}

ConsoleStaticMethod(ShaderManager, setGlobalParameter1f, void, 3, 3, "(paramName, F32 value0)")
{
   ShaderManager->setGlobalParameter(argv[1], dAtof(argv[2]));
}

ConsoleStaticMethod(ShaderManager, setGlobalParameter2f, void, 4, 4, "(paramName, F32 value0, F32 value1)")
{
   ShaderManager->setGlobalParameter(argv[1], dAtof(argv[2]), dAtof(argv[3]));
}

ConsoleStaticMethod(ShaderManager, setGlobalParameter3f, void, 5, 5, "(paramName, F32 value0, F32 value1, F32 value2)")
{
   ShaderManager->setGlobalParameter(argv[1], dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]));
}

ConsoleStaticMethod(ShaderManager, setGlobalParameter4f, void, 6, 6, 
                                    "(paramName, F32 value0, F32 value1, F32 value2, F32 value3)")
{
   ShaderManager->setGlobalParameter(argv[1], dAtof(argv[2]), dAtof(argv[3]), dAtof(argv[4]), dAtof(argv[5]));
}

ConsoleStaticMethod(ShaderManager, deleteGlobalParameter, void, 2, 2, "(paramName)")
{
   ShaderManager->deleteGlobalParameter(argv[1]);
}

ConsoleStaticMethod(ShaderManager, getNumGlobal1fParameters, S32, 1, 1, "()")
{
   return ShaderManager->mGlobal1fParameters.size();
}

ConsoleStaticMethod(ShaderManager, getNumGlobal2fParameters, S32, 1, 1, "()")
{
   return ShaderManager->mGlobal2fParameters.size();
}

ConsoleStaticMethod(ShaderManager, getNumGlobal3fParameters, S32, 1, 1, "()")
{
   return ShaderManager->mGlobal3fParameters.size();
}

ConsoleStaticMethod(ShaderManager, getNumGlobal4fParameters, S32, 1, 1, "()")
{
   return ShaderManager->mGlobal4fParameters.size();
}

ConsoleStaticMethod(ShaderManager, getGlobal1fParameters, const char*, 1, 1, "()")
{
   char * ret = Con::getReturnBuffer(4096);
   U32 pos = 0;
   for(U32 i = 0; i < ShaderManager->mGlobal1fParameters.size(); i++)
   {
      dStrcpy(ret + pos, ShaderManager->mGlobal1fParameters[i].mName);
      pos += dStrlen(ShaderManager->mGlobal1fParameters[i].mName);
      ret[pos] = ' ';
      pos++;
   }
   ret[pos] = NULL;
   return(ret);
}

ConsoleStaticMethod(ShaderManager, getGlobal2fParameters, const char*, 1, 1, "()")
{
   char * ret = Con::getReturnBuffer(4096);
   U32 pos = 0;
   for(U32 i = 0; i < ShaderManager->mGlobal2fParameters.size(); i++)
   {
      dStrcpy(ret + pos, ShaderManager->mGlobal2fParameters[i].mName);
      pos += dStrlen(ShaderManager->mGlobal2fParameters[i].mName);
      ret[pos] = ' ';
      pos++;
   }
   ret[pos] = NULL;
   return(ret);
}

ConsoleStaticMethod(ShaderManager, getGlobal3fParameters, const char*, 1, 1, "()")
{
   char * ret = Con::getReturnBuffer(4096);
   U32 pos = 0;
   for(U32 i = 0; i < ShaderManager->mGlobal3fParameters.size(); i++)
   {
      dStrcpy(ret + pos, ShaderManager->mGlobal3fParameters[i].mName);
      pos += dStrlen(ShaderManager->mGlobal3fParameters[i].mName);
      ret[pos] = ' ';
      pos++;
   }
   ret[pos] = NULL;
   return(ret);
}

ConsoleStaticMethod(ShaderManager, getGlobal4fParameters, const char*, 1, 1, "()")
{
   char * ret = Con::getReturnBuffer(4096);
   U32 pos = 0;
   for(U32 i = 0; i < ShaderManager->mGlobal4fParameters.size(); i++)
   {
      dStrcpy(ret + pos, ShaderManager->mGlobal4fParameters[i].mName);
      pos += dStrlen(ShaderManager->mGlobal4fParameters[i].mName);
      ret[pos] = ' ';
      pos++;
   }
   ret[pos] = NULL;
   return(ret);
}

ConsoleStaticMethod(ShaderManager, getGlobalParameterValue, const char*, 2, 2, "(param)")
{
   char * ret = Con::getReturnBuffer(512);
   for(U32 i = 0; i < ShaderManager->mGlobal1fParameters.size(); i++)
   {
      if(dStricmp(argv[1], ShaderManager->mGlobal1fParameters[i].mName) == 0)
      {
         dSprintf(ret, sizeof(ret), "%g %g %g %g", ShaderManager->mGlobal1fParameters[i].value0, 0.0, 0.0, 0.0);
         return ret;
      }
   }
   
   for(U32 i = 0; i < ShaderManager->mGlobal2fParameters.size(); i++)
   {
      if(dStricmp(argv[1], ShaderManager->mGlobal2fParameters[i].mName) == 0)
      {
         dSprintf(ret, sizeof(ret), "%g %g %g %g", ShaderManager->mGlobal2fParameters[i].value0, 
                                                   ShaderManager->mGlobal2fParameters[i].value1, 0.0, 0.0);
         return ret;
      }
   }
   
   for(U32 i = 0; i < ShaderManager->mGlobal3fParameters.size(); i++)
   {
      if(dStricmp(argv[1], ShaderManager->mGlobal3fParameters[i].mName) == 0)
      {
         dSprintf(ret, sizeof(ret), "%g %g %g %g", ShaderManager->mGlobal3fParameters[i].value0, 
                                                   ShaderManager->mGlobal3fParameters[i].value1, 
                                                   ShaderManager->mGlobal3fParameters[i].value2, 0.0);
         return ret;
      }
   }
   
   for(U32 i = 0; i < ShaderManager->mGlobal4fParameters.size(); i++)
   {
      if(dStricmp(argv[1], ShaderManager->mGlobal4fParameters[i].mName) == 0)
      {
         dSprintf(ret, sizeof(ret), "%g %g %g %g", ShaderManager->mGlobal4fParameters[i].value0, 
                                                   ShaderManager->mGlobal4fParameters[i].value1, 
                                                   ShaderManager->mGlobal4fParameters[i].value2, 
                                                   ShaderManager->mGlobal4fParameters[i].value3);
         return ret;
      }
   }
   
   dSprintf(ret, sizeof(ret), "%g %g %g %g", 0.0, 0.0, 0.0, 0.0);
   return ret;
}

ConsoleFunction(validateGLSL, void, 1, 1, "")
{
   ShaderManager->validateGLSL();
}

ConsoleFunction(reloadShaders, void, 1, 1, "")
{
   ShaderManager->reloadShaders();
}

