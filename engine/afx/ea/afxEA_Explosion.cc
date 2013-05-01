
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"

#include "game/fx/explosion.h"

#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Explosion 

class afxEA_Explosion : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  ExplosionData*    explosion_data;
  Explosion*        explosion;
  bool              exploded;

public:
  /*C*/             afxEA_Explosion();

  virtual bool      isDone() { return exploded; }

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_Explosion::afxEA_Explosion()
{
  explosion_data = 0;
  explosion = 0;
  exploded = false;
}

void afxEA_Explosion::ea_set_datablock(SimDataBlock* db)
{
  explosion_data = dynamic_cast<ExplosionData*>(db);
}

bool afxEA_Explosion::ea_start()
{
  if (!explosion_data)
  {
    Con::errorf("afxEA_Explosion::ea_start() -- missing or incompatible datablock.");
    return false;
  }

  explosion = new Explosion();
  explosion->onNewDataBlock(explosion_data);

  return true;
}

bool afxEA_Explosion::ea_update(F32 dt)
{
  if (!exploded && explosion)
  {
    if (in_scope)
    {
      explosion->setInitialState(updated_pos, Point3F(0,0,1));
      if (!explosion->registerObject())
      {
        delete explosion;
        explosion = 0;
        Con::errorf("afxEA_Explosion::ea_update() -- effect failed to register.");
        return false;
      }
    }
    exploded = true;
  }

  return true;
}

void afxEA_Explosion::ea_finish(bool was_stopped)
{
  explosion = 0;
  exploded = false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_ExplosionDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_ExplosionDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return true; }

  virtual afxEffectWrapper* create() const { return new afxEA_Explosion; }
};

afxEA_ExplosionDesc afxEA_ExplosionDesc::desc;

bool afxEA_ExplosionDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(ExplosionData) == typeid(*db));
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//