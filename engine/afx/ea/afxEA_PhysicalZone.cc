
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"

#include "game/physicalZone.h"

#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/ce/afxPhysicalZone.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_PhysicalZone 

class afxEA_PhysicalZone : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxPhysicalZoneData* zone_data;
  PhysicalZone*     physical_zone;

public:
  /*C*/             afxEA_PhysicalZone();
  /*D*/             ~afxEA_PhysicalZone();

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
  virtual void      ea_set_scope_status(bool flag);
  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_PhysicalZone::afxEA_PhysicalZone()
{
  zone_data = 0;
  physical_zone = 0;
}

afxEA_PhysicalZone::~afxEA_PhysicalZone()
{
  if (physical_zone)
    physical_zone->deleteObject();
}

void afxEA_PhysicalZone::ea_set_datablock(SimDataBlock* db)
{
  zone_data = dynamic_cast<afxPhysicalZoneData*>(db);
}

bool afxEA_PhysicalZone::ea_start()
{
  if (!zone_data)
  {
    Con::errorf("afxEA_PhysicalZone::ea_start() -- missing or incompatible datablock.");
    return false;
  }
  return true;
}

bool afxEA_PhysicalZone::ea_update(F32 dt)
{
  if (!physical_zone)
  {
    // create and register effect
    physical_zone = new PhysicalZone();
    physical_zone->mVelocityMod = zone_data->mVelocityMod;
    physical_zone->mGravityMod = zone_data->mGravityMod;
    physical_zone->mAppliedForce = zone_data->mAppliedForce;
    physical_zone->force_type = zone_data->force_type;
    physical_zone->orient_force = zone_data->orient_force;
    physical_zone->setField("polyhedron", zone_data->mPolyhedron);

    if (!physical_zone->registerObject())
    {
      delete physical_zone;
      physical_zone = 0;
      Con::errorf("afxEA_PhysicalZone::ea_update() -- effect failed to register.");
      return false;
    }
    deleteNotify(physical_zone);
    physical_zone->activate();
  }

  if (physical_zone)
  {
    if (do_fades)
      physical_zone->setFadeAmount(fade_value);
    physical_zone->setTransform(updated_xfm);
  }

  return true;
}

void afxEA_PhysicalZone::ea_finish(bool was_stopped)
{
  if (physical_zone)
  {
    physical_zone->deleteObject();
    physical_zone = 0;
  }
}

void afxEA_PhysicalZone::ea_set_scope_status(bool in_scope)
{
  if (physical_zone)
  {
    if (in_scope && !physical_zone->isActive())
      physical_zone->activate();
    else if (!in_scope && physical_zone->isActive())
      physical_zone->deactivate();
  }
}

void afxEA_PhysicalZone::onDeleteNotify(SimObject* obj)
{
  if (physical_zone == dynamic_cast<PhysicalZone*>(obj))
    physical_zone = 0;

  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_PhysicalZoneDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_PhysicalZoneDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return true; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return false; }

  virtual afxEffectWrapper* create() const { return new afxEA_PhysicalZone; }
};

afxEA_PhysicalZoneDesc afxEA_PhysicalZoneDesc::desc;

bool afxEA_PhysicalZoneDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(afxPhysicalZoneData) == typeid(*db));
}

bool afxEA_PhysicalZoneDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->timing.lifetime < 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//