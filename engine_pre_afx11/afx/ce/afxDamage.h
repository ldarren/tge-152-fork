
#ifndef _AFX_DAMAGE_H_
#define _AFX_DAMAGE_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/gameBase.h"
#include "afx/afxEffectDefs.h"

class afxDamageData : public GameBaseData, public afxEffectDefs
{
  typedef GameBaseData  Parent;

public:
  StringTableEntry  label;
  StringTableEntry  flavor;

  F32           amount;
  U8            repeats;
  F32           ad_amount;
  F32           radius;
  F32           impulse;

public:
  /*C*/         afxDamageData();

  virtual bool  onAdd();
  virtual void  packData(BitStream*);
  virtual void  unpackData(BitStream*);

  static void   initPersistFields();

  DECLARE_CONOBJECT(afxDamageData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_DAMAGE_H_
