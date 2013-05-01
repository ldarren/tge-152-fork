
#ifndef _AFX_MULTI_LIGHT_H_
#define _AFX_MULTI_LIGHT_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/afxEffectDefs.h"

class sgLightObjectData;
struct afxLightData;

class afxMultiLightData : public GameBaseData
{
  typedef GameBaseData  Parent;

  bool          do_id_convert;

public:
  GameBaseData* lights[4];

public:
  /*C*/         afxMultiLightData();

  virtual bool  onAdd();
  virtual void  packData(BitStream*);
  virtual void  unpackData(BitStream*);
  virtual bool  preload(bool server, char errorBuffer[256]);

  static void   initPersistFields();

  DECLARE_CONOBJECT(afxMultiLightData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_MULTI_LIGHT_H_
