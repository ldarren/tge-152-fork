
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"

#include "game/projectile.h"

#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/afxChoreographer.h"
#include "afx/ce/afxProjectile.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Projectile 

/*
   // Create the projectile object
   %p = new (%this.projectileType)() {
      dataBlock        = %projectile;
      initialVelocity  = %muzzleVelocity;
      initialPosition  = %obj.getMuzzlePoint(%slot);
      sourceObject     = %obj;
      sourceSlot       = %slot;
      client           = %obj.client;
   };
*/

class afxEA_Projectile : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  ProjectileData*   projectile_data;
  afxProjectile*    projectile;
  bool              launched;
  bool              impacted;

public:
  /*C*/             afxEA_Projectile();

  virtual bool      isDone() { return impacted; }

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

afxEA_Projectile::afxEA_Projectile()
{
  projectile_data = 0;
  projectile = 0;
  launched = false;
  impacted = false;
}

void afxEA_Projectile::ea_set_datablock(SimDataBlock* db)
{
  projectile_data = dynamic_cast<ProjectileData*>(db);
}

bool afxEA_Projectile::ea_start()
{
  if (!projectile_data)
  {
    Con::errorf("afxEA_Projectile::ea_start() -- missing or incompatible datablock.");
    return false;
  }

  projectile = new afxProjectile();
  projectile->onNewDataBlock(projectile_data);

  return true;
}

bool afxEA_Projectile::ea_update(F32 dt)
{
  if (!launched && projectile)
  {
    if (in_scope)
    {
      afxConstraint* pos_cons = getPosConstraint();
      ShapeBase* src_obj = (pos_cons) ? (dynamic_cast<ShapeBase*>(pos_cons->getObject())) : 0;

      F32 muzzle_vel = projectile_data->muzzleVelocity;

      Point3F dir_vec = updated_aim - updated_pos;
      dir_vec.normalizeSafe();
      dir_vec *= muzzle_vel;
      projectile->init(updated_pos, dir_vec, src_obj);
      if (!projectile->registerObject())
      {
        delete projectile;
        projectile = 0;
        Con::errorf("afxEA_Projectile::ea_update() -- effect failed to register.");
        return false;
      }
      if (projectile)
        projectile->setDataField(StringTable->insert("afxOwner"), 0, choreographer->scriptThis());
    }
    launched = true;
  }

  return true;
}

void afxEA_Projectile::ea_finish(bool was_stopped)
{
  projectile = 0;
  launched = false;
  impacted = false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_ProjectileDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_ProjectileDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return true; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return false; }

  virtual afxEffectWrapper* create() const { return new afxEA_Projectile; }
};

afxEA_ProjectileDesc afxEA_ProjectileDesc::desc;

bool afxEA_ProjectileDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(ProjectileData) == typeid(*db));
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//