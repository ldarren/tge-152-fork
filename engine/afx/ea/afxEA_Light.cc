
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"

#include "afx/ce/afxLight.h"

#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Light 

class afxEA_Light : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxLightData*     light_data;
  afxLight*         light;

public:
  /*C*/             afxEA_Light();
  /*D*/             ~afxEA_Light();

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
  virtual void      ea_set_scope_status(bool flag);
  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_Light::afxEA_Light()
{
  light_data = 0;
  light = 0;
}

afxEA_Light::~afxEA_Light()
{
  if (light)
    light->deleteObject();
}

void afxEA_Light::ea_set_datablock(SimDataBlock* db)
{
  light_data = dynamic_cast<afxLightData*>(db);
}

bool afxEA_Light::ea_start()
{
  if (!light_data)
  {
    Con::errorf("afxEA_Light::ea_start() -- missing or incompatible datablock.");
    return false;
  }

  // create and register effect
  light = new afxLight();
  light->onNewDataBlock(light_data);
  if (!light->registerObject())
  {
    delete light;
    light = 0;
    Con::errorf("afxEA_Light::ea_update() -- effect failed to register.");
    return false;
  }
  deleteNotify(light);

  return true;
}

bool afxEA_Light::ea_update(F32 dt)
{
  if (light)
  {
    if (do_fades)
      light->setFadeAmount(fade_value*updated_scale.x);
    light->setTransform(updated_xfm);
  }

  return true;
}

void afxEA_Light::ea_finish(bool was_stopped)
{
  if (light)
  {
    light->deleteObject();
    light = 0;
  }
}

void afxEA_Light::ea_set_scope_status(bool in_scope)
{
  if (light)
    light->setVisibility(in_scope);
}

void afxEA_Light::onDeleteNotify(SimObject* obj)
{
  if (light == dynamic_cast<afxLight*>(obj))
    light = 0;

  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_LightDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_LightDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return true; }

  virtual afxEffectWrapper* create() const { return new afxEA_Light; }
};

afxEA_LightDesc afxEA_LightDesc::desc;

bool afxEA_LightDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(afxLightData) == typeid(*db));
}

bool afxEA_LightDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->timing.lifetime < 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//