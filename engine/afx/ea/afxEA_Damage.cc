
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"
#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/afxChoreographer.h"
#include "afx/ce/afxDamage.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Damage 

class afxEA_Damage : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxDamageData*    damage_data;
  bool              started;
  U8                repeat_cnt;
  U32               dot_delta_ms;
  U32               next_dot_time;
  Point3F           impact_pos;
  SimObjectId       impacted_obj_id;

public:
  /*C*/             afxEA_Damage();

  virtual bool      isDone();

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_Damage::afxEA_Damage()
{
  damage_data = 0;
  started = false;
  repeat_cnt = 0;
  dot_delta_ms = 0; 
  next_dot_time = 0;
  impact_pos.zero();
  impacted_obj_id = 0;
}

bool afxEA_Damage::isDone() 
{ 
  return (damage_data) ? (repeat_cnt >= damage_data->repeats) : true;
}

void afxEA_Damage::ea_set_datablock(SimDataBlock* db)
{
  damage_data = dynamic_cast<afxDamageData*>(db);
}

bool afxEA_Damage::ea_start()
{
  if (!damage_data)
  {
    Con::errorf("afxEA_Damage::ea_start() -- missing or incompatible datablock.");
    return false;
  }

  if (damage_data->repeats > 1)
  {
    dot_delta_ms = full_lifetime/(damage_data->repeats - 1);
    next_dot_time = dot_delta_ms;
  }

  return true;
}

bool afxEA_Damage::ea_update(F32 dt)
{
  if (!started)
  {
    started = true;

    afxConstraint* pos_cons = getPosConstraint();
    if (pos_cons)
      pos_cons->getPosition(impact_pos);

    afxConstraint* aim_cons = getAimConstraint();
    if (aim_cons && aim_cons->getObject())
      impacted_obj_id = aim_cons->getObject()->getId();

    if (choreographer)
      choreographer->inflictDamage(damage_data->label, damage_data->flavor, impacted_obj_id, damage_data->amount, 
                                   repeat_cnt, damage_data->ad_amount, damage_data->radius, impact_pos, 
                                   damage_data->impulse);
    repeat_cnt++;
  }
  else if (repeat_cnt < damage_data->repeats)
  {
    if (next_dot_time <= life_elapsed)
    {
      if (choreographer)
        choreographer->inflictDamage(damage_data->label, damage_data->flavor, impacted_obj_id, damage_data->amount, 
                                     repeat_cnt, 0, 0, impact_pos, 0);
      next_dot_time += dot_delta_ms;
      repeat_cnt++;
    }
  }

  return true;
}

void afxEA_Damage::ea_finish(bool was_stopped)
{
  if (started && (repeat_cnt < damage_data->repeats))
  {
    if (next_dot_time <= life_elapsed)
    {
      if (choreographer)
        choreographer->inflictDamage(damage_data->label, damage_data->flavor, impacted_obj_id, damage_data->amount, 
                                     repeat_cnt, 0, 0, impact_pos, 0);
    }
  }

  started = false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_DamageDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_DamageDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return true; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return false; }

  virtual afxEffectWrapper* create() const { return new afxEA_Damage; }
};

afxEA_DamageDesc afxEA_DamageDesc::desc;

bool afxEA_DamageDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(afxDamageData) == typeid(*db));
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//