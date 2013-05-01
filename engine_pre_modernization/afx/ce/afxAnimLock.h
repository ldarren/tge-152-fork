
#ifndef _AFX_ANIM_LOCK_H_
#define _AFX_ANIM_LOCK_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/gameBase.h"

class afxAnimLockData : public GameBaseData
{
  typedef GameBaseData  Parent;

public:
  /*C*/                 afxAnimLockData();

  virtual bool          onAdd();
  virtual void          packData(BitStream*);
  virtual void          unpackData(BitStream*);

  static void           initPersistFields();

  DECLARE_CONOBJECT(afxAnimLockData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_ANIM_LOCK_H_
