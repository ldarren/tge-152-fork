
#ifndef _AFX_ANIM_CLIP_H_
#define _AFX_ANIM_CLIP_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/gameBase.h"

class afxAnimClipData : public GameBaseData
{
  typedef GameBaseData  Parent;

  enum 
  {
    IGNORE_DISABLED     = BIT(0),
    IGNORE_ENABLED      = BIT(1),
    IS_DEATH_ANIM       = BIT(2),
    BLOCK_USER_CONTROL  = BIT(3)
  };


public:
  StringTableEntry      clip_name;
  F32                   rate;
  F32                   trans;
  U32                   flags;

  bool                  ignore_disabled;
  bool                  ignore_enabled;
  bool                  is_death_anim;
  bool                  lock_anim;

public:
  /*C*/                 afxAnimClipData();

  virtual bool          onAdd();
  virtual void          packData(BitStream*);
  virtual void          unpackData(BitStream*);

  static void           initPersistFields();

  DECLARE_CONOBJECT(afxAnimClipData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_ANIM_CLIP_H_
