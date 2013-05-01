
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"

#include "lightingSystem/sgLightObject.h"
#include "afx/ce/afxLight.h"
#include "afx/ce/afxMultiLight.h"

#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_MultiLight 

class afxEA_MultiLight : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxMultiLightData*  multi_light_data;

  enum { UNKNOWN, AFX_LIGHT, SG_LIGHT };
  U8                light_types[4];

  union
  {
    afxLight*       afx_light;
    sgLightObject*  sg_light;
    SimObject*      object;
  } lights[4];


public:
  /*C*/             afxEA_MultiLight();
  /*D*/             ~afxEA_MultiLight();

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
  virtual void      ea_set_scope_status(bool flag);
  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_MultiLight::afxEA_MultiLight()
{
  multi_light_data = 0;
  dMemset(light_types, 0, sizeof(light_types));
  dMemset(lights, 0, sizeof(lights));
}

afxEA_MultiLight::~afxEA_MultiLight()
{
  for (S32 i = 0; i < 4; i++)
  {
    if (lights[i].object)
      lights[i].object->deleteObject();
  }
}

void afxEA_MultiLight::ea_set_datablock(SimDataBlock* db)
{
  multi_light_data = dynamic_cast<afxMultiLightData*>(db);
}

bool afxEA_MultiLight::ea_start()
{
  if (!multi_light_data)
  {
    Con::errorf("afxEA_MultiLight::ea_start() -- missing or incompatible datablock.");
    return false;
  }

  // create lights
  for (S32 i = 0; i < 4; i++)
  {
    if (dynamic_cast<afxLightData*>(multi_light_data->lights[i]))
    {
      afxLightData* afx_light_data = (afxLightData*)multi_light_data->lights[i]; 
      light_types[i] = AFX_LIGHT;
      lights[i].afx_light = new afxLight();
      lights[i].afx_light->onNewDataBlock(afx_light_data);
    }
    else if (dynamic_cast<sgLightObjectData*>(multi_light_data->lights[i]))
    {
      sgLightObjectData* sg_light_data = (sgLightObjectData*)multi_light_data->lights[i]; 
      light_types[i] = SG_LIGHT;
      lights[i].sg_light = new sgLightObject();
      lights[i].sg_light->onNewDataBlock(sg_light_data);
    }
  }

  // register lights
  for (S32 i = 0; i < 4; i++)
  {
    if (lights[i].object)
    {
      if (!lights[i].object->registerObject())
      {
        delete lights[i].object;
        lights[i].object = 0;
        light_types[i] = UNKNOWN;
        Con::errorf("afxEA_MultiLight::ea_start() -- light[%d] failed to register.", i);
      }
      else
      {
        deleteNotify(lights[i].object);
      }
    }
  }

  return true;
}

bool afxEA_MultiLight::ea_update(F32 dt)
{
  for (S32 i = 0; i < 4; i++)
  {
    switch (light_types[i])
    {
    case AFX_LIGHT:
      if (do_fades)
        lights[i].afx_light->setFadeAmount(fade_value*updated_scale.x);
      lights[i].afx_light->setTransform(updated_xfm);
      break;
    case SG_LIGHT:
      if (do_fades)
        lights[i].sg_light->setFadeAmount(fade_value*updated_scale.x);
      lights[i].sg_light->setTransform(updated_xfm);
      break;
    case UNKNOWN:
      break;
    }
  }

  return true;
}

void afxEA_MultiLight::ea_finish(bool was_stopped)
{
  for (S32 i = 0; i < 4; i++)
  {
    if (lights[i].object)
    {
      lights[i].object->deleteObject();
      lights[i].object = 0;
    }
  }
}

void afxEA_MultiLight::ea_set_scope_status(bool in_scope)
{
  for (S32 i = 0; i < 4; i++)
  {
    switch (light_types[i])
    {
    case AFX_LIGHT:
      lights[i].afx_light->setVisibility(in_scope);
      break;
    case SG_LIGHT:
      lights[i].sg_light->setEnable(in_scope);
      break;
    case UNKNOWN:
      break;
    }
  }
}

void afxEA_MultiLight::onDeleteNotify(SimObject* obj)
{
  for (S32 i = 0; i < 4; i++)
  {
    if (lights[i].object == obj)
    {
      lights[i].object = 0;
      light_types[i] = UNKNOWN;
    }
  }

  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_MultiLightDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_MultiLightDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return true; }

  virtual afxEffectWrapper* create() const { return new afxEA_MultiLight; }
};

afxEA_MultiLightDesc afxEA_MultiLightDesc::desc;

bool afxEA_MultiLightDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(afxMultiLightData) == typeid(*db));
}

bool afxEA_MultiLightDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->timing.lifetime < 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//