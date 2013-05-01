/*  mkMaterial.cpp
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

#include "ModernizationKit/mkMaterial.h"
#include "console/consoleTypes.h"
#include "dgl/gTexManager.h"

_MaterialManager *MaterialManager = NULL;

// Dead texture, dead beef, get it? ...Don't hurt me
#define TEXTURE_DEAD 0xdeadbeef 
#define TEXTURE_LIGHTMAP 0xdecafbad
#define TEXTURE_FOG 0xabacadab 
// ra

IMPLEMENT_CONOBJECT(SubShader);

SubShader::SubShader()
{
   for(U32 i = 0; i < MAX_SHADERS; i++)
   {
      mVertPaths[i] = NULL;
      mFragPaths[i] = NULL;
      mShaderQualities[i] = 0.0f;
   }
   mShaderType = GLSLShaderType;
}

SubShader::~SubShader()
{

}

bool SubShader::onAdd()
{
   if(!Parent::onAdd())
      return false;
   
   // Add us to the rootgroup
   SimGroup* rootGroup = Sim::getRootGroup();
   AssertFatal(rootGroup, "Why don't we have a rootgroup?");
   rootGroup->addObject(this);
   return true;
}

void SubShader::initPersistFields()
{
   addField("VertexFile",    TypeFilename, Offset(mVertPaths,       SubShader), MAX_SHADERS);
   addField("PixelFile",     TypeFilename, Offset(mFragPaths,       SubShader), MAX_SHADERS);
   addField("ShaderQuality", TypeS32,      Offset(mShaderQualities, SubShader), MAX_SHADERS);
   addField("ShaderType",    TypeS32,      Offset(mShaderType,      SubShader));
}

Shader* SubShader::load()
{
   U32 maxQuality = 0;
   U32 bestShader = -1;
   U32 shaderQuality = MaterialManager->getShaderQuality();
   for(U32 i = 0; i < MAX_SHADERS; i++)
   {
      if(mVertPaths[i] || mFragPaths[i])
      {
         if(mShaderQualities[i] <= shaderQuality)
         {
            if(mShaderQualities[i] >= maxQuality)
            {
               maxQuality = mShaderQualities[i];
               bestShader = i;
            }
         }
      }
   }
   
   if(bestShader == -1)
      return NULL;  // No suitable shaders.
   
   AssertFatal((mVertPaths[bestShader] && mFragPaths[bestShader]), "No shader files specified!");
   return ShaderManager->createShader(mVertPaths[bestShader], mFragPaths[bestShader], mShaderType);
}

ConsoleType( SubShader, TypeSubShaderPtr, sizeof(SubShader*))

ConsoleSetType(TypeSubShaderPtr)
{
   if(argc == 1)
   {
      SimGroup *rootGroup = Sim::getRootGroup();
      AssertFatal(rootGroup, "Why don't we have a root group?");
      SubShader **shd = (SubShader **)dptr;
      *shd = (SubShader*)rootGroup->findObject(argv[0]);
      AssertFatal(*shd != NULL, avar("SubShader: requested sub shader (%s) does not exist.", argv[0]));
   }
   else
   {
      Con::printf("(TypeSubShaderPtr) Cannot set multiple args to a single SubShaderPtr");
   }
}

ConsoleGetType(TypeSubShaderPtr)
{
   SubShader **obj = (SubShader **)dptr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%s", *obj ? (*obj)->getName() ? (*obj)->getName() : (*obj)->getIdString() : "");
   return returnBuffer;
}
      
   
   
   

IMPLEMENT_CONOBJECT(Material);

Material::Material()
{
   m11SubShader = NULL;
   m14SubShader = NULL;
   m20SubShader = NULL;
   m20aSubShader = NULL;
   m30SubShader = NULL;
   
   mActiveShader = NULL;
   mName = NULL;
   
   mMapToTexture = NULL;
   
   for(U32 i = 0; i < MAX_MATERIAL_SAMPLERS; i++)
   {
      mSamplerBindings[i] = NULL;
      mTextureNames[i] = NULL;
      mTextures[i] = NULL;
   }
}

Material::~Material()
{

}

void Material::initPersistFields()
{
   addField("Shader11",   TypeSubShaderPtr,    Offset(m11SubShader,        Material));
   addField("Shader14",   TypeSubShaderPtr,    Offset(m14SubShader,        Material));
   addField("Shader20",   TypeSubShaderPtr,    Offset(m20SubShader,        Material));
   addField("Shader20a",  TypeSubShaderPtr,    Offset(m20aSubShader,       Material));
   addField("Shader30",   TypeSubShaderPtr,    Offset(m30SubShader,        Material));
   
   addField("samplers",   TypeString,          Offset(mSamplerBindings,    Material), MAX_MATERIAL_SAMPLERS);
   addField("textures",   TypeFilename,        Offset(mTextureNames,       Material), MAX_MATERIAL_SAMPLERS);
   addField("name",       TypeString,          Offset(mName,               Material));
   addField("mapTo",      TypeString,          Offset(mMapToTexture,       Material));
}

bool Material::onAdd()
{
   if(!Parent::onAdd())
      return false;

   MaterialManager->setShaderQuality(Con::getIntVariable("$pref::Material::qualityLevel"));
   
   // Load our textures
   for(U32 i = 0; i < MAX_MATERIAL_SAMPLERS; i++)
   {
      if(!mTextureNames[i])
      {
         mTextures[i] = (TextureHandle*)TEXTURE_DEAD;
         continue;
      }
      if(mTextureNames[i] && !mTextures[i])
      {
         if(dStrcmp(mTextureNames[i], "Lightmap") == 0)
         {
            mTextures[i] = (TextureHandle*)TEXTURE_LIGHTMAP;
            continue;
         }
         if(dStrcmp(mTextureNames[i], "Fog") == 0)
         {
            mTextures[i] = (TextureHandle*)TEXTURE_FOG;
            continue;
         }
         mTextures[i] = new TextureHandle(mTextureNames[i], MeshTexture);
         if(!mTextures[i]->getName())
         {
            // No texture, sad
            delete mTextures[i];
            mTextures[i] = (TextureHandle*)TEXTURE_DEAD;
         }
      }
   }

   F32 version = ShaderManager->getPixelShaderVersion();
   if(version > 2.9 && (m30SubShader))
   {
      mActiveShader = m30SubShader->load();
      if(mActiveShader)
      {
         MaterialManager->registerMaterial(this);
         return true;
      }
   }
   if(version > 1.9 && (m20SubShader))
   {
      mActiveShader = m20SubShader->load();
      if(mActiveShader)
      {
         MaterialManager->registerMaterial(this);
         return true;
      }
   }
   if(version > 1.9 && (m20aSubShader))
   {
      mActiveShader = m20aSubShader->load();
      if(mActiveShader)
      {
         MaterialManager->registerMaterial(this);
         return true;
      }
   }
   if(version > 1.3 && (m14SubShader))
   {
      mActiveShader = m14SubShader->load();
      if(mActiveShader)
      {
         MaterialManager->registerMaterial(this);
         return true;
      }
   }
   if(version > 1.0 && (m11SubShader))
   {
      mActiveShader = m11SubShader->load();
      if(mActiveShader)
      {
         MaterialManager->registerMaterial(this);
         return true;
      }
   }
   
   // It's possible that the quality level is just too low, and that may change.
   MaterialManager->registerMaterial(this);
   return true;
}

void Material::reload()
{
   F32 version = ShaderManager->getPixelShaderVersion();
   if(version > 2.9 && (m30SubShader))
   {
      mActiveShader = m30SubShader->load();
      if(mActiveShader)
      {
         return;
      }
   }
   if(version > 1.9 && (m20SubShader))
   {
      mActiveShader = m20SubShader->load();
      if(mActiveShader)
      {
         return;
      }
   }
   if(version > 1.9 && (m20aSubShader))
   {
      mActiveShader = m20aSubShader->load();
      if(mActiveShader)
      {
         return;
      }
   }
   if(version > 1.3 && (m14SubShader))
   {
      mActiveShader = m14SubShader->load();
      if(mActiveShader)
      {
         return;
      }
   }
   if(version > 1.0 && (m11SubShader))
   {
      mActiveShader = m11SubShader->load();
      if(mActiveShader)
      {
         return;
      }
   }
}

U32 Material::getLightmapTextureUnit()
{
   for(U32 i = 0; i < MAX_MATERIAL_SAMPLERS; i++)
      if(mTextures[i] == (void*)TEXTURE_LIGHTMAP)
         return i;
   
   // Guess we have no lightmap, return -1
   return -1;
}

U32 Material::getFogTextureUnit()
{
   for(U32 i = 0; i < MAX_MATERIAL_SAMPLERS; i++)
      if(mTextures[i] == (void*)TEXTURE_FOG)
         return i;
   
   // Guess we have no fog, return -1
   return -1;
}

bool Material::bind()
{
   if(!mActiveShader)
      return false;
   
   ShaderManager->bindShader(mActiveShader);
   for(U32 i = 0; i < MAX_MATERIAL_SAMPLERS; i++)
   {
      if(mTextures[i] && mTextures[i] != (void*)TEXTURE_DEAD && mTextures[i] != (void*)TEXTURE_LIGHTMAP && mTextures[i] != (void*)TEXTURE_FOG)
      {
         glActiveTextureARB(GL_TEXTURE0_ARB + i);
         glBindTexture(GL_TEXTURE_2D, mTextures[i]->getGLName());
      }
      if(mSamplerBindings[i])
      {
         ShaderManager->setTextureSampler(mSamplerBindings[i], i);
      }
   }
   glActiveTextureARB(GL_TEXTURE0_ARB);
   return true;
}

_MaterialManager::_MaterialManager()
{

}

_MaterialManager::~_MaterialManager()
{
   mMaterials.clear();
}

void _MaterialManager::create()
{
   AssertFatal(MaterialManager == NULL, "MaterialManager::create: MaterialManager already exists.");
   MaterialManager = new _MaterialManager;
}

void _MaterialManager::destroy()
{
   AssertFatal(MaterialManager != NULL, "MaterialManager::destroy: MaterialManager does not exist.");
   Con::setIntVariable("$pref::Material::qualityLevel", MaterialManager->getShaderQuality());
   delete MaterialManager;
   MaterialManager = NULL;
}

void _MaterialManager::registerMaterial(Material* mat)
{
   mMaterials.push_back(mat);
   
   // Add it to the rootgroup
   SimGroup* rootGroup = Sim::getRootGroup();
   AssertFatal(rootGroup, "Why don't we have a rootgroup?");
   rootGroup->addObject(mat);
}

void _MaterialManager::reloadMaterials()
{
   for(U32 i = 0; i < mMaterials.size(); i++)
   {
      mMaterials[i]->reload();
   }
}

Shader* _MaterialManager::findShaderByMaterial(StringTableEntry name)
{
   StringTableEntry nName = StringTable->insert(name);
   for(U32 i = 0; i < mMaterials.size(); i++)
   {
      if(mMaterials[i]->mName == nName)
      {
         return mMaterials[i]->getActiveShader();
      }
   }
   return NULL;
}

Material* _MaterialManager::findMaterialByName(StringTableEntry name)
{
   StringTableEntry nName = StringTable->insert(name);
   for(U32 i = 0; i < mMaterials.size(); i++)
   {
      if(mMaterials[i]->mName == nName)
      {
         return mMaterials[i];
      }
   }
   return NULL;
}

Material* _MaterialManager::findMaterialByMapToTexture(StringTableEntry tex)
{
   StringTableEntry nName = StringTable->insert(tex);
   for(U32 i = 0; i < mMaterials.size(); i++)
   {
      if(mMaterials[i]->mMapToTexture == nName)
      {
         return mMaterials[i];
      }
   }
   return NULL;
}


ConsoleStaticMethod(MaterialManager, setShaderQuality, void, 2, 2, "(U32 quality)")
{
   MaterialManager->setShaderQuality(dAtoi(argv[1]));
   MaterialManager->reloadMaterials();
   Con::setIntVariable("$pref::Material::qualityLevel", MaterialManager->getShaderQuality());
}

ConsoleFunction(testMaterial, void, 1, 1, "")
{
   Material* mat = MaterialManager->findMaterialByMapToTexture("unittest");
   if(!mat)
   {
      Con::errorf("failed to find material!");
      return;
   }
   mat->bind();
   //ShaderManager->bindShader(NULL);
   ShaderManager->unbindShader();
   U32 light = mat->getLightmapTextureUnit();
   if(light != 0)
   {
      Con::errorf("got wrong lightmap tu");
   }
   U32 fog = mat->getFogTextureUnit();
   if(fog != 1)
   {
      Con::errorf("got wrong fog tu");
   }
}


