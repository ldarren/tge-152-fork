
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "dgl/dgl.h"
#include "core/bitStream.h"
#include "console/consoleTypes.h"
#include "sim/netConnection.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/detailManager.h"
#include "ts/tsShapeInstance.h"
#include "ts/tsMesh.h"

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

#ifdef AFX_TLK_SUPPORT_ENABLED
  receiveSunLight = true;
  receiveLMLighting = true;
  useAdaptiveSelfIllumination = false;
  useCustomAmbientLighting = false;
  customAmbientForSelfIllumination = false;
  customAmbientLighting = ColorF(0.0f, 0.0f, 0.0f);
  shadowEnable = false;
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

    // this little hack forces the textures to preload
    TSShapeInstance* pDummy = new TSShapeInstance(shape);
    delete pDummy;
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

#ifdef AFX_TGE_1_3
  Con::registerType("AfxModelDataPtr", TypeAfxModelDataPtr, sizeof(afxModelData*), 
                    REF_GETDATATYPE(afxModelData), REF_SETDATATYPE(afxModelData));
#endif

  addField("shapeFile",             TypeFilename, myOffset(shapeName));
  addField("sequence",              TypeFilename, myOffset(sequence));
  addField("sequenceRate",          TypeF32,      myOffset(seq_rate));
  addField("alphaMult",             TypeF32,      myOffset(alpha_mult));
  addField("useVertexAlpha",        TypeBool,     myOffset(use_vertex_alpha));  
  addField("forceOnMaterialFlags",  TypeS32,      myOffset(force_on_material_flags));
  addField("forceOffMaterialFlags", TypeS32,      myOffset(force_off_material_flags));

#ifdef AFX_TLK_SUPPORT_ENABLED
  addField("receiveSunLight",               TypeBool,   myOffset(receiveSunLight));
  addField("receiveLMLighting",             TypeBool,   myOffset(receiveLMLighting));
  addField("useAdaptiveSelfIllumination",   TypeBool,   myOffset(useAdaptiveSelfIllumination));
  addField("useCustomAmbientLighting",      TypeBool,   myOffset(useCustomAmbientLighting));
  addField("customAmbientSelfIllumination", TypeBool,   myOffset(customAmbientForSelfIllumination));
  addField("customAmbientLighting",         TypeColorF, myOffset(customAmbientLighting));
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

#ifdef AFX_TLK_SUPPORT_ENABLED
  stream->writeFlag(receiveSunLight);
  stream->writeFlag(useAdaptiveSelfIllumination);
  stream->writeFlag(useCustomAmbientLighting);
  stream->writeFlag(customAmbientForSelfIllumination);
  stream->write(customAmbientLighting);
  stream->writeFlag(receiveLMLighting);
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

#ifdef AFX_TLK_SUPPORT_ENABLED
  receiveSunLight = stream->readFlag();
  useAdaptiveSelfIllumination = stream->readFlag();
  useCustomAmbientLighting = stream->readFlag();
  customAmbientForSelfIllumination = stream->readFlag();
  stream->read(&customAmbientLighting);
  receiveLMLighting = stream->readFlag();
  shadowEnable = stream->readFlag();
#endif
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxModel

IMPLEMENT_CO_NETOBJECT_V1(afxModel);

afxModel::afxModel()
{
#ifdef AFX_TGE_1_5
  overrideOptions = false;
#endif

  shape_inst = 0;
  seq_thread = 0;
  seq_animates_vis = false;
  fade_amt = 1.0f;
  seq_rate_factor = 1.0f;
  is_visible = true;
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

#ifdef AFX_TLK_SUPPORT_ENABLED
#ifdef AFX_TLK_1_4_SUPPORT_ENABLED
  sgSceneObjectData.sgReceiveSunLight = mDataBlock->receiveSunLight;
  sgSceneObjectData.sgReceiveLMLighting = mDataBlock->receiveLMLighting;
  sgSceneObjectData.sgUseAdaptiveSelfIllumination = mDataBlock->useAdaptiveSelfIllumination;
  sgSceneObjectData.sgUseCustomAmbientLighting = mDataBlock->useCustomAmbientLighting;
  sgSceneObjectData.sgCustomAmbientForSelfIllumination = mDataBlock->customAmbientForSelfIllumination;
  sgSceneObjectData.sgCustomAmbientLighting = mDataBlock->customAmbientLighting;
#else
  receiveSunLight = mDataBlock->receiveSunLight;
  receiveLMLighting = mDataBlock->receiveLMLighting;
  useAdaptiveSelfIllumination = mDataBlock->useAdaptiveSelfIllumination;
  useCustomAmbientLighting = mDataBlock->useCustomAmbientLighting;
  customAmbientForSelfIllumination = mDataBlock->customAmbientForSelfIllumination;
  customAmbientLighting = mDataBlock->customAmbientLighting;
#endif
#endif

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
    shape_inst = new TSShapeInstance(mDataBlock->shape);
    if (mDataBlock->sequence != ST_NULLSTRING)
    {
      TSShape* shape = shape_inst->getShape();
      S32 seq = shape->findSequence(mDataBlock->sequence);
      if (seq != -1)
      {
        seq_thread = shape_inst->addThread();
        shape_inst->setTimeScale(seq_thread, seq_rate_factor*mDataBlock->seq_rate);
        shape_inst->setSequence(seq_thread, seq, 0);
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
    if (dist < 0.01)
      dist = 0.01;
    
    // don't bother rendering if fog is thick enough
    F32 fogAmount = state->getHazeAndFog(dist, cameraOffset.z);
    if (fogAmount > 0.99f)
      return false;
    
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
        image->textureSortKey = (U32)(dsize_t)mDataBlock;
        state->setImageRefPoint(this, image);  
        state->insertRenderImage(image);
      }
    }
  }
  
  return false;
}

void afxModel::renderObject(SceneState* state, SceneRenderImage* image)
{
  AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");

  if (!shape_inst || !DetailManager::selectCurrentDetail(shape_inst))
    return;

  RectI viewport;
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  dglGetViewport(&viewport);

#if defined(AFX_TGE_1_5)
  gClientSceneGraph->getLightManager()->sgSetupLights(this);
#elif defined(AFX_TLK_1_4_SUPPORT_ENABLED)
  sgSceneObjectData.installLightsAdvanced(this);
#else
  installLights();
#endif

  state->setupObjectProjection(this);

  // This is something of a hack, but since the 3space objects don't have a
  //  clear conception of texels/meter like the interiors do, we're sorta
  //  stuck.  I can't even claim this is anything more scientific than eyeball
  //  work.  DMM
  F32 axis = (getObjBox().len_x() + getObjBox().len_y() + getObjBox().len_z()) / 3.0;
  F32 dist = (getRenderWorldBox().getClosestPoint(state->getCameraPosition()) - state->getCameraPosition()).len();
  if (dist != 0)
  {
    F32 projected = dglProjectRadius(dist, axis) / 350;
    if (projected < (1.0 / 16.0))
    {
      TextureManager::setSmallTexturesActive(true);
    }
  }

  F32 opacity = fade_amt*mDataBlock->alpha_mult;

  shape_inst->animate();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  dglMultMatrix(&mObjToWorld);
  glScalef(mObjScale.x, mObjScale.y, mObjScale.z);
  
  shape_inst->setEnvironmentMap(state->getEnvironmentMap());
  shape_inst->setEnvironmentMapOn(true,1);
  ///shape_inst->setAlphaAlways(1.0);
  
  Point3F cameraOffset;
  mObjToWorld.getColumn(3,&cameraOffset);
  cameraOffset -= state->getCameraPosition();
  F32 fogAmount = state->getHazeAndFog(cameraOffset.len(),cameraOffset.z);
  
  // render all transparent and opaque meshes
  if (opacity < 1.0f)
  {
    TSShapeInstance::smNoRenderNonTranslucent = false;
    TSShapeInstance::smNoRenderTranslucent    = false;
  }
  // render transparent meshes only 
  else if (image->isTranslucent == true)
  {
    TSShapeInstance::smNoRenderNonTranslucent = true;
    TSShapeInstance::smNoRenderTranslucent    = false;
  }
  // render opaque meshes only
  else
  {
    TSShapeInstance::smNoRenderNonTranslucent = false;
    TSShapeInstance::smNoRenderTranslucent    = true;
  }
  
  if (seq_animates_vis || mDataBlock->use_vertex_alpha)
    shape_inst->setAlphaAlways(opacity);
  else
    TSMesh::setOverrideFade(opacity);

  shape_inst->setupFog(fogAmount, state->getFogColor());
  shape_inst->render();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

#ifdef AFX_TLK_SUPPORT_ENABLED
  // shadow rendering code -- this works well for independent models
  // but for mounted models, the shadowing must be a merged operation
  // to appear correct
  if (mDataBlock->shadowEnable)
  {
    shadows.sgRender(this, shape_inst, cameraOffset.len(), fogAmount, 0.0f, 0.0f, -1, true, true);
  }
#endif

  if (seq_animates_vis || mDataBlock->use_vertex_alpha)
    shape_inst->setAlphaAlways(1.0f);
  else
    TSMesh::setOverrideFade(1.0f);

  TSShapeInstance::smNoRenderNonTranslucent = false;
  TSShapeInstance::smNoRenderTranslucent    = false;
  TextureManager::setSmallTexturesActive(false);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  dglSetViewport(viewport);

#if defined(AFX_TGE_1_5)
  gClientSceneGraph->getLightManager()->sgResetLights();
#elif defined(AFX_TLK_1_4_SUPPORT_ENABLED)
  sgSceneObjectData.uninstallLightsAdvanced(this);
#else
  uninstallLights();
#endif

  dglSetCanonicalState();
  AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//