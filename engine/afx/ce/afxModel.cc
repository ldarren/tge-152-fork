
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "game/objectTypes.h"
#include "sim/netConnection.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/detailManager.h"
#include "ts/tsShapeInstance.h"

#include "afx/ce/afxModel.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxModelData

IMPLEMENT_CO_DATABLOCK_V1(afxModelData);
 
afxModelData::afxModelData()
{
  shapeName = ST_NULLSTRING;
  sequence = ST_NULLSTRING;
  seq_rate = 1.0f;
  alpha_mult = 1.0f;
  use_vertex_alpha = false;
  force_on_material_flags = 0;
  force_off_material_flags = 0;
  seq_offset = 0.0f;
  texture_filtering = true;
  fog_mult = 1.0f;
  remap_txr_tags = ST_NULLSTRING;
#if defined(TGEA_ENGINE)
  remap_buffer = 0;
#endif

  overrideLightingOptions = false;
  receiveSunLight = true;
  receiveLMLighting = true;
  useAdaptiveSelfIllumination = false;
  useCustomAmbientLighting = false;
  customAmbientForSelfIllumination = false;
  customAmbientLighting = ColorF(0.0f, 0.0f, 0.0f);
#if defined(AFX_MODEL_SHADOWS)
  shadowEnable = false;
#endif
}

afxModelData::~afxModelData()
{
#if defined(TGEA_ENGINE)
   if (remap_buffer)
      dFree(remap_buffer);
#endif
}

static void debug_print_flags(const char* msg, U32 flags)
{
  Con::printf(msg);
  if ((flags & TSMaterialList::S_Wrap) == TSMaterialList::S_Wrap)
    Con::printf("    S_Wrap");
  if ((flags & TSMaterialList::T_Wrap) == TSMaterialList::T_Wrap)
    Con::printf("    T_Wrap");
  if ((flags & TSMaterialList::Translucent) == TSMaterialList::Translucent)
    Con::printf("    Translucent");
  if ((flags & TSMaterialList::Additive) == TSMaterialList::Additive)
    Con::printf("    Additive");
  if ((flags & TSMaterialList::Subtractive) == TSMaterialList::Subtractive)
    Con::printf("    Subtractive");
  if ((flags & TSMaterialList::SelfIlluminating) == TSMaterialList::SelfIlluminating)
    Con::printf("    SelfIlluminating");
  if ((flags & TSMaterialList::NeverEnvMap) == TSMaterialList::NeverEnvMap)
    Con::printf("    NeverEnvMap");
  if ((flags & TSMaterialList::NoMipMap) == TSMaterialList::NoMipMap)
    Con::printf("    NoMipMap");
  if ((flags & TSMaterialList::MipMap_ZeroBorder) == TSMaterialList::MipMap_ZeroBorder)
    Con::printf("    MipMap_ZeroBorder");
  if ((flags & TSMaterialList::IflMaterial) == TSMaterialList::IflMaterial)
    Con::printf("    IflMaterial");
  if ((flags & TSMaterialList::IflFrame) == TSMaterialList::IflFrame)
    Con::printf("    IflFrame");
  if ((flags & TSMaterialList::DetailMapOnly) == TSMaterialList::DetailMapOnly)
    Con::printf("    DetailMapOnly");
  if ((flags & TSMaterialList::BumpMapOnly) == TSMaterialList::BumpMapOnly)
    Con::printf("    BumpMapOnly");
  if ((flags & TSMaterialList::ReflectanceMapOnly) == TSMaterialList::ReflectanceMapOnly)
    Con::printf("    ReflectanceMapOnly");
  if ((flags & TSMaterialList::AuxiliaryMap) == TSMaterialList::AuxiliaryMap)
    Con::printf("    AuxiliaryMap");
}

bool afxModelData::preload(bool server, char errorBuffer[256])
{
  if (Parent::preload(server, errorBuffer) == false)
    return false;
  
  // don't need to do this stuff on the server
  if (server) 
    return true;
  
  if (shapeName != ST_NULLSTRING && shape.isNull())
  {
    shape = ResourceManager->load(shapeName);
    if (shape.isNull())
    {
      dSprintf(errorBuffer, sizeof(errorBuffer), "afxModelData::load: Couldn't load shape \"%s\"", shapeName);
      return false;
    }

#if defined(TGEA_ENGINE)
    // just parse up the string and collect the remappings in txr_tag_remappings.
    if (remap_txr_tags != ST_NULLSTRING)
    {
       txr_tag_remappings.clear();
       if (remap_buffer)
          dFree(remap_buffer);

       remap_buffer = dStrdup(remap_txr_tags);

       char* remap_token = dStrtok(remap_buffer, " \t");
       while (remap_token != NULL)
       {
          char* colon = dStrchr(remap_token, ':');
          if (colon)
          {
             *colon = '\0';
             txr_tag_remappings.increment();
             txr_tag_remappings.last().old_tag = remap_token;
             txr_tag_remappings.last().new_tag = colon+1;
          }
          remap_token = dStrtok(NULL, " \t");
       }
    }

    // this little hack messes things up when remapping texture tags
    if (txr_tag_remappings.size() == 0)
    {
      // this little hack forces the textures to preload
      TSShapeInstance* pDummy = new TSShapeInstance(shape);
      delete pDummy;
    }
#else
    // this little hack forces the textures to preload
    TSShapeInstance* pDummy = new TSShapeInstance(shape);
    delete pDummy;
#endif
  }

  return true;
}

IMPLEMENT_CONSOLETYPE(afxModelData)
IMPLEMENT_SETDATATYPE(afxModelData)
IMPLEMENT_GETDATATYPE(afxModelData)

#define myOffset(field) Offset(field, afxModelData)

void afxModelData::initPersistFields()
{
  Parent::initPersistFields();

  addField("shapeFile",             TypeFilename, myOffset(shapeName));
  addField("sequence",              TypeFilename, myOffset(sequence));
  addField("sequenceRate",          TypeF32,      myOffset(seq_rate));
  addField("alphaMult",             TypeF32,      myOffset(alpha_mult));
  addField("useVertexAlpha",        TypeBool,     myOffset(use_vertex_alpha));  
  addField("forceOnMaterialFlags",  TypeS32,      myOffset(force_on_material_flags));
  addField("forceOffMaterialFlags", TypeS32,      myOffset(force_off_material_flags));
  addField("sequenceOffset",        TypeF32,      myOffset(seq_offset));
  addField("textureFiltering",      TypeBool,     myOffset(texture_filtering));
  addField("fogMult",               TypeF32,      myOffset(fog_mult));
  addField("remapTextureTags",      TypeString,   myOffset(remap_txr_tags));

  addField("overrideLightingOptions",       TypeBool,   myOffset(overrideLightingOptions));
  addField("receiveSunLight",               TypeBool,   myOffset(receiveSunLight));
  addField("receiveLMLighting",             TypeBool,   myOffset(receiveLMLighting));
  addField("useAdaptiveSelfIllumination",   TypeBool,   myOffset(useAdaptiveSelfIllumination));
  addField("useCustomAmbientLighting",      TypeBool,   myOffset(useCustomAmbientLighting));
  addField("customAmbientSelfIllumination", TypeBool,   myOffset(customAmbientForSelfIllumination));
  addField("customAmbientLighting",         TypeColorF, myOffset(customAmbientLighting));
#if defined(AFX_MODEL_SHADOWS)
  addField("shadowEnable",                  TypeBool,   myOffset(shadowEnable));
#endif

  // Material Flags
  Con::setIntVariable("$MaterialFlags::S_Wrap",              TSMaterialList::S_Wrap);
  Con::setIntVariable("$MaterialFlags::T_Wrap",              TSMaterialList::T_Wrap);
  Con::setIntVariable("$MaterialFlags::Translucent",         TSMaterialList::Translucent);
  Con::setIntVariable("$MaterialFlags::Additive",            TSMaterialList::Additive);
  Con::setIntVariable("$MaterialFlags::Subtractive",         TSMaterialList::Subtractive);
  Con::setIntVariable("$MaterialFlags::SelfIlluminating",    TSMaterialList::SelfIlluminating);
  Con::setIntVariable("$MaterialFlags::NeverEnvMap",         TSMaterialList::NeverEnvMap);
  Con::setIntVariable("$MaterialFlags::NoMipMap",            TSMaterialList::NoMipMap);
  Con::setIntVariable("$MaterialFlags::MipMap_ZeroBorder",   TSMaterialList::MipMap_ZeroBorder);
  Con::setIntVariable("$MaterialFlags::IflMaterial",         TSMaterialList::IflMaterial);
  Con::setIntVariable("$MaterialFlags::IflFrame",            TSMaterialList::IflFrame);
  Con::setIntVariable("$MaterialFlags::DetailMapOnly",       TSMaterialList::DetailMapOnly);
  Con::setIntVariable("$MaterialFlags::BumpMapOnly",         TSMaterialList::BumpMapOnly);
  Con::setIntVariable("$MaterialFlags::ReflectanceMapOnly",  TSMaterialList::ReflectanceMapOnly);
  Con::setIntVariable("$MaterialFlags::AuxiliaryMap",        TSMaterialList::AuxiliaryMap);

#if defined(AFX_CAP_AFXMODEL_TYPE)
  Con::setIntVariable("$TypeMasks::afxModelObjectType",      afxModelObjectType);
#endif
}

void afxModelData::packData(BitStream* stream)
{
  Parent::packData(stream);

  stream->writeString(shapeName);
  stream->writeString(sequence);
  stream->write(seq_rate);  
  stream->write(alpha_mult); 
  stream->write(use_vertex_alpha); 
  stream->write(force_on_material_flags);
  stream->write(force_off_material_flags);
  stream->write(seq_offset);
  stream->writeFlag(texture_filtering);
  stream->write(fog_mult);
#if defined(TGEA_ENGINE)
  stream->writeString(remap_txr_tags);
#endif

  stream->writeFlag(overrideLightingOptions);
  stream->writeFlag(receiveSunLight);
  stream->writeFlag(useAdaptiveSelfIllumination);
  stream->writeFlag(useCustomAmbientLighting);
  stream->writeFlag(customAmbientForSelfIllumination);
  stream->write(customAmbientLighting);
  stream->writeFlag(receiveLMLighting);
#if defined(AFX_MODEL_SHADOWS)
  stream->writeFlag(shadowEnable);
#endif
}

void afxModelData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  shapeName = stream->readSTString();
  sequence = stream->readSTString();
  stream->read(&seq_rate);
  stream->read(&alpha_mult);
  stream->read(&use_vertex_alpha);
  stream->read(&force_on_material_flags);
  stream->read(&force_off_material_flags);
  stream->read(&seq_offset);
  texture_filtering = stream->readFlag();
  stream->read(&fog_mult);
#if defined(TGEA_ENGINE)
  remap_txr_tags = stream->readSTString();
#endif

  overrideLightingOptions = stream->readFlag();
  receiveSunLight = stream->readFlag();
  useAdaptiveSelfIllumination = stream->readFlag();
  useCustomAmbientLighting = stream->readFlag();
  customAmbientForSelfIllumination = stream->readFlag();
  stream->read(&customAmbientLighting);
  receiveLMLighting = stream->readFlag();
#if defined(AFX_MODEL_SHADOWS)
  shadowEnable = stream->readFlag();
#endif
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxModel

IMPLEMENT_CO_NETOBJECT_V1(afxModel);

afxModel::afxModel()
{
#if defined(AFX_CAP_AFXMODEL_TYPE)
  mTypeMask |= afxModelObjectType;
#endif

  overrideOptions = false;

  shape_inst = 0;
  seq_thread = 0;
  seq_animates_vis = false;
  fade_amt = 1.0f;
  seq_rate_factor = 1.0f;
  is_visible = true;
  sort_priority = 0;

  // this allows initial scene lighting ambient calculation
  mLightingInfo.mDirty = true; // new - may have versioning issues
}

afxModel::~afxModel()
{
  delete shape_inst;
}

void afxModel::setSequenceRateFactor(F32 factor)
{
  seq_rate_factor = factor;
  if (shape_inst != NULL && seq_thread != NULL)
    shape_inst->setTimeScale(seq_thread, seq_rate_factor*mDataBlock->seq_rate);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

bool afxModel::onNewDataBlock(GameBaseData* dptr)
{
  mDataBlock = dynamic_cast<afxModelData*>(dptr);
  if (!mDataBlock || !Parent::onNewDataBlock(dptr))
    return false;

  overrideOptions = mDataBlock->overrideLightingOptions;
  receiveSunLight = mDataBlock->receiveSunLight;
  receiveLMLighting = mDataBlock->receiveLMLighting;
  useAdaptiveSelfIllumination = mDataBlock->useAdaptiveSelfIllumination;
  useCustomAmbientLighting = mDataBlock->useCustomAmbientLighting;
  customAmbientForSelfIllumination = mDataBlock->customAmbientForSelfIllumination;
  customAmbientLighting = mDataBlock->customAmbientLighting;

  return true;
}

bool afxModel::onAdd()
{
  // first check if we have a server connection, if we don't then this is on the server
  // and we should exit, then check if the parent fails to add the object
  NetConnection* conn = NetConnection::getConnectionToServer();
  if (!conn || !Parent::onAdd())
    return false;

  // setup our bounding box
  if (!mDataBlock->shape.isNull())
    mObjBox = mDataBlock->shape->bounds;
  else
    mObjBox = Box3F(Point3F(-1, -1, -1), Point3F(1, 1, 1));

  // setup the shape instance and sequence
  if (!mDataBlock->shape.isNull())
  {
#if defined(TGEA_ENGINE)
     if (/*isClientObject() && */mDataBlock->txr_tag_remappings.size() > 0)
     {
        // temporarily substitute material tags with alternates
        TSMaterialList* mat_list = mDataBlock->shape->materialList;
        if (mat_list)
        {
           for (S32 i = 0; i < mDataBlock->txr_tag_remappings.size(); i++)
           {
              afxModelData::TextureTagRemapping* remap = &mDataBlock->txr_tag_remappings[i];
              for (S32 j = 0; j < mat_list->mMaterialNames.size(); j++) 
              {
                 if (dStricmp(mat_list->mMaterialNames[j], remap->old_tag) == 0)
                 {
                    //Con::printf("REMAP TEXTURE TAG [%s] TO [%s]", remap->old_tag, remap->new_tag);
                    delete [] mat_list->mMaterialNames[j];
                    mat_list->mMaterialNames[j] = new char[dStrlen(remap->new_tag)+1];
                    dStrcpy(mat_list->mMaterialNames[j], remap->new_tag);
                    break;
                 }
              }
           }
        }
     }
#endif

    shape_inst = new TSShapeInstance(mDataBlock->shape);

#if defined(TGEA_ENGINE)
    if (true) // isClientObject())
    {
       shape_inst->cloneMaterialList();

       // restore the material tags to original form
       if (mDataBlock->txr_tag_remappings.size() > 0)
       {
          TSMaterialList* mat_list = mDataBlock->shape->materialList;
          if (mat_list)
          {
             for (S32 i = 0; i < mDataBlock->txr_tag_remappings.size(); i++)
             {
                afxModelData::TextureTagRemapping* remap = &mDataBlock->txr_tag_remappings[i];
                for (S32 j = 0; j < mat_list->mMaterialNames.size(); j++) 
                {
                   if (dStrcmp(mat_list->mMaterialNames[j], remap->new_tag) == 0)
                   {
                      //Con::printf("UNREMAP TEXTURE TAG [%s] TO [%s]", remap->new_tag, remap->old_tag);
                      delete [] mat_list->mMaterialNames[j];
                      mat_list->mMaterialNames[j] = new char[dStrlen(remap->old_tag)+1];
                      dStrcpy(mat_list->mMaterialNames[j], remap->old_tag);
                      break;
                   }
                }
             }
          }
       }
    }
#endif

    if (mDataBlock->sequence != ST_NULLSTRING)
    {
      TSShape* shape = shape_inst->getShape();
      S32 seq = shape->findSequence(mDataBlock->sequence);
      if (seq != -1)
      {      
        seq_thread = shape_inst->addThread();
      
        F32 seq_pos = 0.0f;
        if (mDataBlock->seq_offset > 0.0f && mDataBlock->seq_offset < shape_inst->getDuration(seq_thread))
          seq_pos = mDataBlock->seq_offset / shape_inst->getDuration(seq_thread);

        shape_inst->setTimeScale(seq_thread, seq_rate_factor*mDataBlock->seq_rate);
        shape_inst->setSequence(seq_thread, seq, seq_pos);
        seq_animates_vis = shape->sequences[seq].visMatters.testAll();
      }
    }

    // deal with material changes
    if (shape_inst && (mDataBlock->force_on_material_flags | mDataBlock->force_off_material_flags))
    {
      shape_inst->cloneMaterialList();
      TSMaterialList* mats = shape_inst->getMaterialList();
      if (mDataBlock->force_on_material_flags != 0)
      {
        for (U32 i = 0; i < mats->getMaterialCount(); i++)
          mats->setFlags(i, mats->getFlags(i) | mDataBlock->force_on_material_flags);
      }

      if (mDataBlock->force_off_material_flags != 0)
      {
        for (U32 i = 0; i < mats->getMaterialCount(); i++)
          mats->setFlags(i, mats->getFlags(i) & ~mDataBlock->force_off_material_flags);
      }
    }
  }

  resetWorldBox();

  // Scan out the collision hulls...
  U32 i;
  for (i = 0; i < mDataBlock->shape->details.size(); i++)
  {
    char* name = (char*)mDataBlock->shape->names[mDataBlock->shape->details[i].nameIndex];

    if (dStrstr((const char*)dStrlwr(name), "collision-"))
    {
      mCollisionDetails.push_back(i);

      // The way LOS works is that it will check to see if there is a LOS detail that matches
      // the the collision detail + 1 + MaxCollisionShapes (this variable name should change in
      // the future). If it can't find a matching LOS it will simply use the collision instead.
      // We check for any "unmatched" LOS's further down
      mLOSDetails.increment();

      char buff[128];
      dSprintf(buff, sizeof(buff), "LOS-%d", i + 1 + 8/*MaxCollisionShapes*/);
      U32 los = mDataBlock->shape->findDetail(buff);
      if (los == -1)
        mLOSDetails.last() = i;
      else
        mLOSDetails.last() = los;
    }
  }

  // Snag any "unmatched" LOS details
  for (i = 0; i < mDataBlock->shape->details.size(); i++)
  {
    char* name = (char*)mDataBlock->shape->names[mDataBlock->shape->details[i].nameIndex];

    if (dStrstr((const char*)dStrlwr(name), "los-"))
    {
      // See if we already have this LOS
      bool found = false;
      for (U32 j = 0; j < mLOSDetails.size(); j++)
      {
        if (mLOSDetails[j] == i)
        {
          found = true;
          break;
        }
      }

      if (!found)
        mLOSDetails.push_back(i);
    }
  }

  // Compute the hull accelerators (actually, just force the shape to compute them)
  for (i = 0; i < mCollisionDetails.size(); i++)
    shape_inst->getShape()->getAccelerator(mCollisionDetails[i]);

  // tell engine the model exists
  gClientContainer.addObject(this);
  gClientSceneGraph->addObjectToScene(this);
  removeFromProcessList();
  gClientProcessList.addObject(this);
  conn->addObject(this);

  return true;
}

void afxModel::onRemove()
{
  mSceneManager->removeObjectFromScene(this);
  getContainer()->removeObject(this);
  Parent::onRemove();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

void afxModel::advanceTime(F32 dt)
{
  if (seq_thread)
    shape_inst->advanceTime(dt, seq_thread);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

bool afxModel::prepRenderImage(SceneState* state, const U32 stateKey, const U32/*startZone*/, 
                               const bool/*modifyBaseState*/)
{
  if (!is_visible)
    return false;

  if (isLastState(state, stateKey))
    return false; 

  setLastState(state, stateKey);
  
  // This should be sufficient for most objects that don't manage zones, and
  //  don't need to return a specialized RenderImage...
  if (shape_inst && state->isObjectRendered(this))
  {
    // calculate distance to camera
    Point3F cameraOffset;
    mObjToWorld.getColumn(3, &cameraOffset);
    cameraOffset -= state->getCameraPosition();   
    F32 dist = cameraOffset.len();
    if (dist < 0.01f)
      dist = 0.01f;
    
    // don't bother rendering if fog is thick enough
    F32 fogAmount = state->getHazeAndFog(dist, cameraOffset.z);
    fogAmount *= mDataBlock->fog_mult;
    if (fogAmount > 0.99f)
      return false;
    
#if defined(TGEA_ENGINE)
    DetailManager::selectPotentialDetails(shape_inst,getRenderTransform(),state);
    if (shape_inst->getCurrentDetail() < 0)
      return false;

    renderObject(state);
#else
    // don't bother rendering if eliminated by detail manager 
    F32 invScale = (1.0f/getMax(getMax(mObjScale.x, mObjScale.y), mObjScale.z));
    DetailManager::selectPotentialDetails(shape_inst, dist, invScale);
    if (shape_inst->getCurrentDetail() < 0)
      return false;

    F32 opacity = fade_amt*mDataBlock->alpha_mult;
     
    // if model's dynamic opacity is < 1, render all meshes in one pass
    if (opacity < 1.0f)
    {
      SceneRenderImage* image = new SceneRenderImage;
      image->obj = this;
      image->isTranslucent = true;
      image->sortType = SceneRenderImage::Point;
      image->tieBreaker = sort_priority;
      image->textureSortKey = (U32)(dsize_t)mDataBlock;
      state->setImageRefPoint(this, image); 
      state->insertRenderImage(image);
    }
    // if model's dynamic opacity is 1, render opaque then transparent meshes
    else
    {
      if (shape_inst->hasSolid())
      {
        SceneRenderImage* image = new SceneRenderImage;
        image->obj = this;
        image->isTranslucent = false;
        image->textureSortKey = (U32)(dsize_t)mDataBlock;
        state->insertRenderImage(image);
      }
      if (shape_inst->hasTranslucency())
      {
        SceneRenderImage* image = new SceneRenderImage;
        image->obj = this;
        image->isTranslucent = true;
        image->sortType = SceneRenderImage::Point;
        image->tieBreaker = sort_priority;
        image->textureSortKey = (U32)(dsize_t)mDataBlock;
        state->setImageRefPoint(this, image);  
        state->insertRenderImage(image);
      }
    }
#endif
  }
  
  return false;
}

bool afxModel::castRay(const Point3F &start, const Point3F &end, RayInfo* info)
{
  if (shape_inst)
  {
    RayInfo shortest;
    shortest.t = 1e8;

    info->object = NULL;
    if (mLOSDetails.size() > 0)
    {
      for (U32 i = 0; i < mLOSDetails.size(); i++)
      {
        shape_inst->animate(mLOSDetails[i]);
        if (shape_inst->castRay(start, end, info, mLOSDetails[i]))
        {
          info->object = this;
          if (info->t < shortest.t)
            shortest = *info;
        }
      }
    }
    else
    {
      if (mCollisionDetails.size() > 0)
      {
        for (U32 i = 0; i < mCollisionDetails.size(); i++)
        {
          shape_inst->animate(mCollisionDetails[i]);
          if (shape_inst->castRay(start, end, info, mCollisionDetails[i]))
          {
            info->object = this;
            if (info->t < shortest.t)
              shortest = *info;
          }
        }
      }
    }

    if (info->object == this) 
    {
      // Copy out the shortest time...
      *info = shortest;
      return true;
    }
  }

  return false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
