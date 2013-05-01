
#ifndef _AFX_MACHINE_GUN_H_
#define _AFX_MACHINE_GUN_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/afxEffectDefs.h"

class ProjectileData;

class afxMachineGunData : public GameBaseData, public afxEffectDefs
{
  typedef GameBaseData  Parent;

public:
  ProjectileData* projectile_data;
  S32             rounds_per_minute;

public:
  /*C*/         afxMachineGunData();

  virtual bool  onAdd();
  virtual void  packData(BitStream*);
  virtual void  unpackData(BitStream*);

  static void   initPersistFields();

  DECLARE_CONOBJECT(afxMachineGunData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_MACHINE_GUN_H_
