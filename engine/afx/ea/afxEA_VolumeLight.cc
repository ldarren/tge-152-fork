
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"
#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/ea/afxEA_ParticleEmitter.h"
#include "afx/afxChoreographer.h"
#include "afx/ce/afxVolumeLight.h"

#if defined(TGEA_ENGINE)
#define volumeLight VolumeLight
#endif

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_VolumeLight 

class afxEA_VolumeLight : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxVolumeLightData* light_data;
  volumeLight*      volume_light;

public:
  /*C*/             afxEA_VolumeLight();
  /*D*/             ~afxEA_VolumeLight();

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
  virtual void      ea_set_scope_status(bool flag);
  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//

class sgVolumeLightGhoster : public volumeLight
{
public:
  void force_ghost() 
  {
    mNetFlags.clear(Ghostable | ScopeAlways);
    mNetFlags.set(IsGhost);
  }
};

afxEA_VolumeLight::afxEA_VolumeLight()
{
  light_data = 0;
  volume_light = 0;
}

afxEA_VolumeLight::~afxEA_VolumeLight()
{
  if (volume_light)
    volume_light->deleteObject();
}

void afxEA_VolumeLight::ea_set_datablock(SimDataBlock* db)
{
  light_data = dynamic_cast<afxVolumeLightData*>(db);
}

bool afxEA_VolumeLight::ea_start()
{
  if (!light_data)
  {
    Con::errorf("afxEA_VolumeLight::ea_start() -- missing or incompatible datablock.");
    return false;
  }
  return true;
}

bool afxEA_VolumeLight::ea_update(F32 dt)
{
  if (!volume_light)
  { 
    // create and register effect
    volume_light = new volumeLight();
    ((sgVolumeLightGhoster*)volume_light)->force_ghost();
    volume_light->onNewDataBlock(light_data);

#if defined(TGEA_ENGINE)
      volume_light->mLightTexture = light_data->mLTextureName;
      volume_light->mLPDistance = light_data->mlpDistance;
      volume_light->mShootDistance = light_data->mShootDistance;
      volume_light->mXExtent = light_data->mXextent;
      volume_light->mYExtent = light_data->mYextent;
      volume_light->mSubdivideU = light_data->mSubdivideU;
      volume_light->mSubdivideV = light_data->mSubdivideV;
      volume_light->mFootColor = light_data->mfootColour;
      volume_light->mTailColor = light_data->mtailColour;
#else
      volume_light->setLtexture(light_data->mLTextureName);
      volume_light->setlpDistance(light_data->mlpDistance);
      volume_light->setShootDistance(light_data->mShootDistance);
      volume_light->setXextent(light_data->mXextent);
      volume_light->setYextent(light_data->mYextent);
      volume_light->setSubdivideU(light_data->mSubdivideU);
      volume_light->setSubdivideV(light_data->mSubdivideV);
      volume_light->setfootColour(light_data->mfootColour);
      volume_light->settailColour(light_data->mtailColour);
#endif

    if (!volume_light->registerObject())
    {
      delete volume_light;
      volume_light = 0;
      Con::errorf("afxEA_VolumeLight::ea_update() -- effect failed to register.");
      return false;
    }
    deleteNotify(volume_light);

#if defined(AFX_CAP_LIGHT_EMITTER_LINKAGE)
    if (light_data->afx_emitterEffectName != ST_NULLSTRING)
    {
      afxEffectWrapper* ew = choreographer->findNamedEffect(light_data->afx_emitterEffectName);
      if (ew)
      {
        afxEA_ParticleEmitter* pe = dynamic_cast<afxEA_ParticleEmitter*>(ew);
        if (pe && pe->emitter)
        {
          volume_light->afx_setEffectEmitter(pe->emitter);
        }
      }
    }
#endif

    volume_light->setFadeAmount(updated_scale.x);
  }

  if (volume_light)
  {
    if (do_fades)
      volume_light->setFadeAmount(fade_value*updated_scale.x);
    volume_light->setTransform(updated_xfm);
  }

  return true;
}

void afxEA_VolumeLight::ea_finish(bool was_stopped)
{
  if (volume_light)
  {
    volume_light->deleteObject();
    volume_light = 0;
  }
}

void afxEA_VolumeLight::ea_set_scope_status(bool in_scope)
{
  if (volume_light)
    volume_light->setEnable(in_scope);
}

void afxEA_VolumeLight::onDeleteNotify(SimObject* obj)
{
  if (volume_light == dynamic_cast<volumeLight*>(obj))
    volume_light = 0;

  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_VolumeLightDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_VolumeLightDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return true; }

  virtual afxEffectWrapper* create() const { return new afxEA_VolumeLight; }
};

afxEA_VolumeLightDesc afxEA_VolumeLightDesc::desc;

bool afxEA_VolumeLightDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(afxVolumeLightData) == typeid(*db));
}

bool afxEA_VolumeLightDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->timing.lifetime < 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//