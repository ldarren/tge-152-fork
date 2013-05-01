
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"

#include "lightingSystem/sgLightObject.h"

#include "afx/ce/afxVolumeLight.h"
#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#if defined(AFX_CAP_LIGHT_EMITTER_LINKAGE)
#include "afx/ea/afxEA_ParticleEmitter.h"
#include "afx/afxChoreographer.h"
#endif

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_TLKLight 

class afxEA_TLKLight : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  sgLightObjectData* light_data;
  sgLightObject*    light;

public:
  /*C*/             afxEA_TLKLight();
  /*D*/             ~afxEA_TLKLight();

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
  virtual void      ea_set_scope_status(bool flag);
  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//

class sgLightObjectGhoster : public sgLightObject
{
public:
  void force_ghost() 
  {
    mNetFlags.clear(Ghostable | ScopeAlways);
    mNetFlags.set(IsGhost);
  }
};

afxEA_TLKLight::afxEA_TLKLight()
{
  light_data = 0;
  light = 0;
}

afxEA_TLKLight::~afxEA_TLKLight()
{
  if (light)
    light->deleteObject();
}

void afxEA_TLKLight::ea_set_datablock(SimDataBlock* db)
{
  light_data = dynamic_cast<sgLightObjectData*>(db);
}

bool afxEA_TLKLight::ea_start()
{
  if (!light_data)
  {
    Con::errorf("afxEA_TLKLight::ea_start() -- missing or incompatible datablock.");
    return false;
  }

  // create and register effect
  light = new sgLightObject();
  ((sgLightObjectGhoster*)light)->force_ghost();
  light->onNewDataBlock(light_data);
  if (!light->registerObject())
  {
    delete light;
    light = 0;
    Con::errorf("afxEA_TLKLight::ea_update() -- effect failed to register.");
    return false;
  }
  deleteNotify(light);

#if defined(AFX_CAP_LIGHT_EMITTER_LINKAGE)
  if (light_data->afx_emitterEffectName != ST_NULLSTRING)
  {
    afxEffectWrapper* ew = choreographer->findNamedEffect(light_data->afx_emitterEffectName);
    if (ew)
    {
      afxEA_ParticleEmitter* pe = dynamic_cast<afxEA_ParticleEmitter*>(ew);
      if (pe && pe->emitter)
      {
        light->afx_setEffectEmitter(pe->emitter);
      }
    }
  }
#endif

  return true;
}

bool afxEA_TLKLight::ea_update(F32 dt)
{
  if (light)
  {
    if (do_fades)
      light->setFadeAmount(fade_value*updated_scale.x);
    light->setTransform(updated_xfm);
  }

  return true;
}

void afxEA_TLKLight::ea_finish(bool was_stopped)
{
  if (light)
  {
    light->deleteObject();
    light = 0;
  }
}

void afxEA_TLKLight::ea_set_scope_status(bool in_scope)
{
  if (light)
    light->setEnable(in_scope);
}

void afxEA_TLKLight::onDeleteNotify(SimObject* obj)
{
  if (light == dynamic_cast<sgLightObject*>(obj))
    light = 0;

  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_TLKLightDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_TLKLightDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return true; }

  virtual afxEffectWrapper* create() const { return new afxEA_TLKLight; }
};

afxEA_TLKLightDesc afxEA_TLKLightDesc::desc;

//class sgUniversalStaticLightData;

bool afxEA_TLKLightDesc::testEffectType(const SimDataBlock* db) const
{
  if (typeid(afxVolumeLightData) == typeid(*db))
    return false;
  return (dynamic_cast<const sgLightObjectData*>(db) != 0);
}

bool afxEA_TLKLightDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->timing.lifetime < 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//