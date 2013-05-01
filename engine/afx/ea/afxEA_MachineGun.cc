
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"
#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/afxChoreographer.h"
#include "afx/ce/afxProjectile.h"
#include "afx/ce/afxMachineGun.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_MachineGun 

class afxEA_MachineGun : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxMachineGunData* gun_data;
  bool          shooting;
  F32           start_time;
  F32           shot_gap;
  S32           shot_count;

  void          launch_projectile();

public:
  /*C*/         afxEA_MachineGun();

  virtual void  ea_set_datablock(SimDataBlock*);
  virtual bool  ea_start();
  virtual bool  ea_update(F32 dt);
  virtual void  ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_MachineGun::afxEA_MachineGun()
{
  gun_data = 0;
  shooting = false;
  start_time = 0.0f;
  shot_count = 0;
  shot_gap = 0.2f;
}

void afxEA_MachineGun::ea_set_datablock(SimDataBlock* db)
{
  gun_data = dynamic_cast<afxMachineGunData*>(db);
}

bool afxEA_MachineGun::ea_start()
{
  if (!gun_data)
  {
    Con::errorf("afxEA_MachineGun::ea_start() -- missing or incompatible datablock.");
    return false;
  }

  if (gun_data->rounds_per_minute > 0)
    shot_gap = 60.0f/gun_data->rounds_per_minute;

  return true;
}

bool afxEA_MachineGun::ea_update(F32 dt)
{
  if (!shooting)
  {
    start_time = elapsed;
    shooting = true;
  }
  else
  {
    F32 next_shot = start_time + (shot_count+1)*shot_gap;
    while (next_shot < elapsed)
    {
      if (in_scope)
        launch_projectile();
      next_shot += shot_gap;
      shot_count++;
    }
  }

  return true;
}

void afxEA_MachineGun::ea_finish(bool was_stopped)
{
}

void afxEA_MachineGun::launch_projectile()
{
  afxProjectile* projectile = new afxProjectile();
  projectile->onNewDataBlock(gun_data->projectile_data);

  F32 muzzle_vel = gun_data->projectile_data->muzzleVelocity;

  afxConstraint* pos_cons = getPosConstraint();
  ShapeBase* src_obj = (pos_cons) ? (dynamic_cast<ShapeBase*>(pos_cons->getObject())) : 0;

  Point3F dir_vec = updated_aim - updated_pos;
  dir_vec.normalizeSafe();
  dir_vec *= muzzle_vel;
  projectile->init(updated_pos, dir_vec, src_obj);
  if (!projectile->registerObject())
  {
    delete projectile;
    projectile = 0;
    Con::errorf("afxEA_MachineGun::launch_projectile() -- projectile failed to register.");
  }
  if (projectile)
    projectile->setDataField(StringTable->insert("afxOwner"), 0, choreographer->scriptThis());
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_MachineGunDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_MachineGunDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return true; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return false; }

  virtual afxEffectWrapper* create() const { return new afxEA_MachineGun; }
};

afxEA_MachineGunDesc afxEA_MachineGunDesc::desc;

bool afxEA_MachineGunDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(afxMachineGunData) == typeid(*db));
}

bool afxEA_MachineGunDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->timing.lifetime < 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//