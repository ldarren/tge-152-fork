
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

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

#define N_LIGHTING_MODELS 6
//
// "SG - Original Advanced (Lighting Pack)"
// "SG - Original Stock (Lighting Pack)"
// "SG - Inverse Square (Lighting Pack)"
// "SG - Inverse Square Fast Falloff (Lighting Pack)"
// "SG - Near Linear (Lighting Pack)"
// "SG - Near Linear Fast Falloff (Lighting Pack)"
static StringTableEntry lm_old_names[N_LIGHTING_MODELS];
//
// "Original Advanced"
// "Original Stock"
// "Inverse Square"
// "Inverse Square Fast Falloff"
// "Near Linear"
// "Near Linear Fast Falloff"
static StringTableEntry lm_new_names[N_LIGHTING_MODELS];

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxLightData

IMPLEMENT_CO_DATABLOCK_V1(afxLightData);

afxLightData::afxLightData()
{
  light_info.mType = LightInfo::Point;
  light_info.mPos.set(0,0,0);
  light_info.mDirection.set(0,0,1);
  light_info.mColor.set(0,0,0);
  light_info.mAmbient.set(0,0,0);
  light_info.mRadius = 0;
  made_for_LL = false;

  light_info.sgSpotAngle = 90.0f;
  light_info.sgCastsShadows = false;
  light_info.sgLocalAmbientAmount = 0;
  light_info.sgSmoothSpotLight = false;
  light_info.sgDoubleSidedAmbient = false;
  light_info.sgUseNormals = true;
  light_info.sgDiffuseRestrictZone = false;
  light_info.sgAmbientRestrictZone = false;
  light_info.sgLightingModelName = StringTable->insert("SG - Near Linear (Lighting Pack)");
#if defined(TGEA_ENGINE)
  light_info.sgSupportedFeatures = LightInfo::sgNoSpecCube;
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
   { LightInfo::SGStaticPoint,  "SGStaticPoint"},
   { LightInfo::SGStaticSpot,   "SGStaticSpot"},
};
static EnumTable light_type_tbl(sizeof(light_type_enums) / sizeof(light_type_enums[0]), light_type_enums);

#if defined(TGEA_ENGINE)
static EnumTable::Enums gsgLightFeatureTypes[] =
{
	{LightInfo::sgFull,				"Full"},
	{LightInfo::sgNoCube,			"NoCube"},
	{LightInfo::sgNoSpecCube,		"NoSpecCube"}
};
static EnumTable gsgLightFeatureTypeTable(3, &gsgLightFeatureTypes[0]);
#endif

void afxLightData::initPersistFields()
{
  Parent::initPersistFields();

  addField("type",            TypeEnum,     myOffset(light_info.mType), 1, &light_type_tbl);
  addField("direction",       TypePoint3F,  myOffset(light_info.mDirection));
  addField("color",           TypeColorF,   myOffset(light_info.mColor));
  addField("ambientColor",    TypeColorF,   myOffset(light_info.mAmbient));
  addField("radius",          TypeF32,      myOffset(light_info.mRadius));
#if defined(AFX_CAP_LIGHT_ILLUMINATION_MASKING)
  addField("lightIlluminationMask",       TypeS8,       myOffset(light_info.illumObjectTypeMask));
#endif
  addField("madeForLegacyLighting",  TypeBool,     myOffset(made_for_LL));

  addField("sgSpotAngle",          TypeF32,    myOffset(light_info.sgSpotAngle));
  addField("sgCastsShadows",       TypeBool,   myOffset(light_info.sgCastsShadows));
  addField("sgLocalAmbientAmount", TypeF32,    myOffset(light_info.sgLocalAmbientAmount));
  addField("sgSmoothSpotLight",    TypeBool,   myOffset(light_info.sgSmoothSpotLight));
  addField("sgDoubleSidedAmbient", TypeBool,   myOffset(light_info.sgDoubleSidedAmbient));
  addField("sgUseNormals",         TypeBool,   myOffset(light_info.sgUseNormals));
  addField("sgDiffuseRestrictZone",TypeBool,   myOffset(light_info.sgDiffuseRestrictZone));
  addField("sgAmbientRestrictZone",TypeBool,   myOffset(light_info.sgAmbientRestrictZone));
  addField("sgLightingModelName",  TypeString, myOffset(light_info.sgLightingModelName));

#if defined(TGEA_ENGINE)
  addField("sgSupportedFeatures",   TypeEnum,   myOffset(light_info.sgSupportedFeatures), 1, &gsgLightFeatureTypeTable);
#endif

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
}

bool afxLightData::preload(bool server, char errorBuffer[256])
{
  if (Parent::preload(server, errorBuffer) == false)
    return false;
  
  if (server) 
  {
    if (made_for_LL)
    {
      light_info.mColor.red *= 0.5f;
      light_info.mColor.green *= 0.5f;
      light_info.mColor.blue *= 0.5f;
    }

    for (U32 i = 0; i < N_LIGHTING_MODELS; i++)
    {
      if (light_info.sgLightingModelName == lm_new_names[i])
      {
        light_info.sgLightingModelName = lm_old_names[i];
        //Con::printf("TLK: lighting model [%s] replaced with [%s]", lm_new_names[i], lm_old_names[i]); 
        break;
      }
    }
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
#if defined(AFX_CAP_LIGHT_ILLUMINATION_MASKING)
  stream->write(light_info.illumObjectTypeMask);
#endif

  stream->write(light_info.sgSpotAngle);
  stream->write(light_info.sgCastsShadows);
  stream->write(light_info.sgLocalAmbientAmount);
  stream->write(light_info.sgSmoothSpotLight);
  stream->write(light_info.sgDoubleSidedAmbient);
  stream->writeString(light_info.sgLightingModelName);
  stream->write(light_info.sgUseNormals);
  stream->write(light_info.sgDiffuseRestrictZone);
  stream->write(light_info.sgAmbientRestrictZone);
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
#if defined(AFX_CAP_LIGHT_ILLUMINATION_MASKING)
  stream->read(&light_info.illumObjectTypeMask);
#endif

  stream->read(&light_info.sgSpotAngle);
  stream->read(&light_info.sgCastsShadows);
  stream->read(&light_info.sgLocalAmbientAmount);
  stream->read(&light_info.sgSmoothSpotLight);
  stream->read(&light_info.sgDoubleSidedAmbient);
  light_info.sgLightingModelName = stream->readSTString();
  stream->read(&light_info.sgUseNormals);
  stream->read(&light_info.sgDiffuseRestrictZone);
  stream->read(&light_info.sgAmbientRestrictZone);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxLight

IMPLEMENT_CO_NETOBJECT_V1(afxLight);

afxLight::afxLight()
{
  fade_amt = 1.0f;
  is_visible = true;
  direction0.set(0.0f, 0.0f, 1.0f);
#if defined(TGEA_ENGINE)
  dir_matrix.identity();
#endif
}

afxLight::~afxLight()
{
} 

#if defined(TGEA_ENGINE)
void afxLight::calc_dir_matrix()
{
   // assumes that direction0 is already normalized

   // when close to (0,0,1) dir_matrix is identity
   if (direction0.z > 0.9999999f)
   {
      dir_matrix.identity();
   }
   // when close to (0,0,-1) dir_matrix does 180deg rotation
   else if (direction0.z < -0.9999999f)
   {
      dir_matrix.identity();
      dir_matrix[0] = -1.0f;
      dir_matrix[10] = -1.0f;
   }
   // calculate dir_matrix in non-degenerative cases
   else
   {
      VectorF up_v(0.0f, 0.0f, 1.0f);
      F32 theta = mAcos(mDot(up_v, direction0));
      Point3F axis; 
      mCross(direction0, up_v, &axis);
      axis.normalizeSafe();
      AngAxisF thetaRot(axis, theta);
      thetaRot.setMatrix(&dir_matrix);
   }
}
#endif

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

bool afxLight::onNewDataBlock(GameBaseData* dptr)
{
  mDataBlock = dynamic_cast<afxLightData*>(dptr);
  if (!mDataBlock || !Parent::onNewDataBlock(dptr))
    return false;

  direction0 = mDataBlock->light_info.mDirection;
  direction0.normalizeSafe();
#if defined(TGEA_ENGINE)
  calc_dir_matrix();
#endif

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
#if defined(TGEA_ENGINE)
  if (light_info.mType == LightInfo::Vector) 
#else
  if (light_info.mType == LightInfo::Spot   ||
      light_info.mType == LightInfo::Vector) 
#endif
  {
    light_info.mDirection = direction0;
    MatrixF m = getRenderTransform();
    m.setPosition( Point3F(0,0,0) );
    m.mulP(light_info.mDirection);
#if !defined(TGEA_ENGINE)
    light_info.mDirection *= -1.0;
#endif
  }

#if defined(TGEA_ENGINE)
  if (light_info.mType == LightInfo::Spot)
  {
     light_info.sgLightingTransform = getTransform()*dir_matrix;
     light_info.sgLightingTransform.setPosition(Point3F(0.0f, 0.0f, 0.0f));

     VectorF up_v(0.0f, 0.0f, 1.0f);
     light_info.sgLightingTransform.mulV(up_v);
     light_info.mDirection = -up_v;
     light_info.mDirection.normalizeSafe();
     light_info.sgSpotPlane = PlaneF(light_info.mPos, -light_info.mDirection);
  }
#endif

#if defined(TGEA_ENGINE)
  lightManager->sgRegisterGlobalLight(&light_info, this, false);
#else
  lightManager->sgRegisterGlobalLight(&light_info);
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