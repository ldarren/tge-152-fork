
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
#include "math/mathIO.h"

#include "afx/ce/afxLight.h"

#ifdef AFX_TLK_SUPPORT_ENABLED

#ifdef AFX_TLK_1_4_SUPPORT_ENABLED
#define TLK_LIGHT_INFO light_info.sgLightInfoData
#else
#define TLK_LIGHT_INFO light_info
#endif

#define NUM_TLK_LIGHTING_MODELS 6
// "SG - Original Advanced (Lighting Pack)"
// "SG - Original Stock (Lighting Pack)"
// "SG - Inverse Square (Lighting Pack)"
// "SG - Inverse Square Fast Falloff (Lighting Pack)"
// "SG - Near Linear (Lighting Pack)"
// "SG - Near Linear Fast Falloff (Lighting Pack)"
static StringTableEntry lm_old_names[NUM_TLK_LIGHTING_MODELS];

// "Original Advanced"
// "Original Stock"
// "Inverse Square"
// "Inverse Square Fast Falloff"
// "Near Linear"
// "Near Linear Fast Falloff"
static StringTableEntry lm_new_names[NUM_TLK_LIGHTING_MODELS];

#endif




//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxLightData

IMPLEMENT_CO_DATABLOCK_V1(afxLightData);

afxLightData::afxLightData()
{
  light_info.mType = LightInfo::Point;
  light_info.mPos.set(0,0,0);
  light_info.mDirection.set(0,0,-1);
  light_info.mColor.set(0,0,0);
  light_info.mAmbient.set(0,0,0);
  light_info.mRadius = 0;
  made_for_LL = false;

#ifdef AFX_TLK_SUPPORT_ENABLED
  TLK_LIGHT_INFO.sgSpotAngle = 90.0f;
  TLK_LIGHT_INFO.sgCastsShadows = false;
  TLK_LIGHT_INFO.sgLocalAmbientAmount = 0;
  TLK_LIGHT_INFO.sgSmoothSpotLight = false;
  TLK_LIGHT_INFO.sgDoubleSidedAmbient = false;
  TLK_LIGHT_INFO.sgUseNormals = true;
  TLK_LIGHT_INFO.sgDiffuseRestrictZone = false;
  TLK_LIGHT_INFO.sgAmbientRestrictZone = false;
  TLK_LIGHT_INFO.sgLightingModelName = StringTable->insert("SG - Near Linear (Lighting Pack)");
#endif
}

IMPLEMENT_CONSOLETYPE(afxLightData)
IMPLEMENT_SETDATATYPE(afxLightData)
IMPLEMENT_GETDATATYPE(afxLightData)

#define myOffset(field) Offset(field, afxLightData)

static EnumTable::Enums light_type_enums[] =
{
   { LightInfo::Point,          "Point"},
   { LightInfo::Spot,           "Spot"},
   { LightInfo::Vector,         "Vector"},
   { LightInfo::Ambient,        "Ambient"},
#ifdef AFX_TLK_SUPPORT_ENABLED
   { LightInfo::SGStaticPoint,  "SGStaticPoint"},
   { LightInfo::SGStaticSpot,   "SGStaticSpot"},
#endif
};
static EnumTable light_type_tbl(sizeof(light_type_enums) / sizeof(light_type_enums[0]), light_type_enums);

void afxLightData::initPersistFields()
{
  Parent::initPersistFields();

#ifdef AFX_TGE_1_3
  Con::registerType("AfxLightDataPtr", TypeAfxLightDataPtr, sizeof(afxLightData*), 
                    REF_GETDATATYPE(afxLightData), REF_SETDATATYPE(afxLightData));
#endif

  addField("type",            TypeEnum,     myOffset(light_info.mType), 1, &light_type_tbl);
  addField("direction",       TypePoint3F,  myOffset(light_info.mDirection));
  addField("color",           TypeColorF,   myOffset(light_info.mColor));
  addField("ambientColor",    TypeColorF,   myOffset(light_info.mAmbient));
  addField("radius",          TypeF32,      myOffset(light_info.mRadius));
  addField("madeForLegacyLighting",  TypeBool,     myOffset(made_for_LL));
#ifdef AFX_TLK_SUPPORT_ENABLED
  addField("sgSpotAngle",          TypeF32,    myOffset(TLK_LIGHT_INFO.sgSpotAngle));
  addField("sgCastsShadows",       TypeBool,   myOffset(TLK_LIGHT_INFO.sgCastsShadows));
  addField("sgLocalAmbientAmount", TypeF32,    myOffset(TLK_LIGHT_INFO.sgLocalAmbientAmount));
  addField("sgSmoothSpotLight",    TypeBool,   myOffset(TLK_LIGHT_INFO.sgSmoothSpotLight));
  addField("sgDoubleSidedAmbient", TypeBool,   myOffset(TLK_LIGHT_INFO.sgDoubleSidedAmbient));
  addField("sgUseNormals",         TypeBool,   myOffset(TLK_LIGHT_INFO.sgUseNormals));
  addField("sgDiffuseRestrictZone",TypeBool,   myOffset(TLK_LIGHT_INFO.sgDiffuseRestrictZone));
  addField("sgAmbientRestrictZone",TypeBool,   myOffset(TLK_LIGHT_INFO.sgAmbientRestrictZone));
  addField("sgLightingModelName",  TypeString, myOffset(TLK_LIGHT_INFO.sgLightingModelName));
#endif

#ifdef AFX_TLK_SUPPORT_ENABLED
  if (lm_old_names[0] == 0)
  {
    lm_old_names[0] = StringTable->insert("SG - Original Advanced (Lighting Pack)");
    lm_old_names[1] = StringTable->insert("SG - Original Stock (Lighting Pack)");
    lm_old_names[2] = StringTable->insert("SG - Inverse Square (Lighting Pack)");
    lm_old_names[3] = StringTable->insert("SG - Inverse Square Fast Falloff (Lighting Pack)");
    lm_old_names[4] = StringTable->insert("SG - Near Linear (Lighting Pack)");
    lm_old_names[5] = StringTable->insert("SG - Near Linear Fast Falloff (Lighting Pack)");
    //
    lm_new_names[0] = StringTable->insert("Original Advanced");
    lm_new_names[1] = StringTable->insert("Original Stock");
    lm_new_names[2] = StringTable->insert("Inverse Square");
    lm_new_names[3] = StringTable->insert("Inverse Square Fast Falloff");
    lm_new_names[4] = StringTable->insert("Near Linear");
    lm_new_names[5] = StringTable->insert("Near Linear Fast Falloff");
  }
#endif
}

bool afxLightData::preload(bool server, char errorBuffer[256])
{
  if (Parent::preload(server, errorBuffer) == false)
    return false;
  
  if (server) 
  {
#ifdef AFX_TLK_SUPPORT_ENABLED
    if (made_for_LL)
    {
      light_info.mColor.red *= 0.5f;
      light_info.mColor.green *= 0.5f;
      light_info.mColor.blue *= 0.5f;
    }

    for (U32 i = 0; i < NUM_TLK_LIGHTING_MODELS; i++)
    {
      if (TLK_LIGHT_INFO.sgLightingModelName == lm_new_names[i])
      {
        TLK_LIGHT_INFO.sgLightingModelName = lm_old_names[i];
        //Con::printf("TLK: lighting model [%s] replaced with [%s]", lm_new_names[i], lm_old_names[i]); 
        break;
      }
    }

#else
    if (!made_for_LL)
    {
      light_info.mColor.red *= 2.0;
      light_info.mColor.green *= 2.0f;
      light_info.mColor.blue *= 2.0f;
    }
#endif
  }

  return true;
}

void afxLightData::packData(BitStream* stream)
{
  Parent::packData(stream);

  stream->writeRangedU32(light_info.mType, 0, 5);

  // this can be refined by light type and fields used
  mathWrite(*stream, light_info.mDirection);
  stream->write(light_info.mColor.red);
  stream->write(light_info.mColor.green);
  stream->write(light_info.mColor.blue);
  stream->write(light_info.mAmbient.red);
  stream->write(light_info.mAmbient.green);
  stream->write(light_info.mAmbient.blue);
  stream->write(light_info.mRadius);

#ifdef AFX_TLK_SUPPORT_ENABLED
  stream->write(TLK_LIGHT_INFO.sgSpotAngle);
  stream->write(TLK_LIGHT_INFO.sgCastsShadows);
  stream->write(TLK_LIGHT_INFO.sgLocalAmbientAmount);
  stream->write(TLK_LIGHT_INFO.sgSmoothSpotLight);
  stream->write(TLK_LIGHT_INFO.sgDoubleSidedAmbient);
  stream->writeString(TLK_LIGHT_INFO.sgLightingModelName);
  stream->write(TLK_LIGHT_INFO.sgUseNormals);
  stream->write(TLK_LIGHT_INFO.sgDiffuseRestrictZone);
  stream->write(TLK_LIGHT_INFO.sgAmbientRestrictZone);
#endif
}

void afxLightData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  light_info.mType = (LightInfo::Type) stream->readRangedU32(0, 5);

  // this can be refined by light type and fields used
  mathRead(*stream, &light_info.mDirection);
  stream->read(&light_info.mColor.red);
  stream->read(&light_info.mColor.green);
  stream->read(&light_info.mColor.blue);
  stream->read(&light_info.mAmbient.red);
  stream->read(&light_info.mAmbient.green);
  stream->read(&light_info.mAmbient.blue);
  stream->read(&light_info.mRadius);

#ifdef AFX_TLK_SUPPORT_ENABLED
  stream->read(&TLK_LIGHT_INFO.sgSpotAngle);
  stream->read(&TLK_LIGHT_INFO.sgCastsShadows);
  stream->read(&TLK_LIGHT_INFO.sgLocalAmbientAmount);
  stream->read(&TLK_LIGHT_INFO.sgSmoothSpotLight);
  stream->read(&TLK_LIGHT_INFO.sgDoubleSidedAmbient);
  TLK_LIGHT_INFO.sgLightingModelName = stream->readSTString();
  stream->read(&TLK_LIGHT_INFO.sgUseNormals);
  stream->read(&TLK_LIGHT_INFO.sgDiffuseRestrictZone);
  stream->read(&TLK_LIGHT_INFO.sgAmbientRestrictZone);
#endif
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxLight

IMPLEMENT_CO_NETOBJECT_V1(afxLight);

afxLight::afxLight()
{
  fade_amt = 1.0f;
  is_visible = true;
}

afxLight::~afxLight()
{
}

void afxLight::initPersistFields()
{
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

bool afxLight::onNewDataBlock(GameBaseData* dptr)
{
  mDataBlock = dynamic_cast<afxLightData*>(dptr);
  if (!mDataBlock || !Parent::onNewDataBlock(dptr))
    return false;

  direction0 = mDataBlock->light_info.mDirection;

  return true;
}

bool afxLight::prepRenderImage(SceneState* state, const U32 stateKey, const U32 startZone, const bool modifyBaseState)
{
   return Parent::prepRenderImage(state, stateKey, startZone, modifyBaseState);
}

void afxLight::registerLights(LightManager* lightManager, bool lightingScene)
{
  if (lightingScene || !is_visible)
    return;

  light_info = mDataBlock->light_info;

  getRenderTransform().getColumn(3, &light_info.mPos);
  if (fade_amt < 1.0f)
  {
    light_info.mColor *= fade_amt;
    light_info.mAmbient *= fade_amt;
  }
#ifdef AFX_TLK_SUPPORT_ENABLED
  if (light_info.mType == LightInfo::Spot   ||
      light_info.mType == LightInfo::Vector ||
      light_info.mType == LightInfo::SGStaticSpot) 
#else
  if (light_info.mType == LightInfo::Spot   ||
      light_info.mType == LightInfo::Vector) 
#endif
  {
    //Con::printf( "pre-rot:  %f %f %f", light_info.mDirection.x, light_info.mDirection.y, light_info.mDirection.z );
    light_info.mDirection = direction0;
    MatrixF m = getRenderTransform();
    m.setPosition( Point3F(0,0,0) );
    m.mulP(light_info.mDirection);
    //Con::printf( "post-rot: %f %f %f", light_info.mDirection.x, light_info.mDirection.y, light_info.mDirection.z );
  }

#ifdef AFX_TGE_1_5
  lightManager->sgRegisterGlobalLight(&light_info);
#else
  lightManager->addLight(&light_info);
#endif
}

bool afxLight::onAdd()
{
  // first check if we have a server connection, if we dont then this is on the server
  //  and we should exit, then check if the parent fails to add the object
  NetConnection* conn = NetConnection::getConnectionToServer();
  if(!conn || !Parent::onAdd())
    return false;

  // tell engine the model exists
  gClientContainer.addObject(this);
  gClientSceneGraph->addObjectToScene(this);

  removeFromProcessList();
  gClientProcessList.addObject(this);

  conn->addObject(this);

  Sim::getLightSet()->addObject(this);

  return true;
}

void afxLight::onRemove()
{
  mSceneManager->removeObjectFromScene(this);
  getContainer()->removeObject(this);

  Parent::onRemove();
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//