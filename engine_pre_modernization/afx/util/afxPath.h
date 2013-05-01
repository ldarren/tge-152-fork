
#ifndef _AFX_PATH_H_
#define _AFX_PATH_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/gameBase.h"
#include "core/tVector.h"

class afxPathData : public GameBaseData
{
  typedef GameBaseData  Parent;

  bool                  resolved;

public:  
  StringTableEntry      points_string;
  StringTableEntry      roll_string;
  StringTableEntry      loop_string;
  F32                   delay;              
  F32                   lifetime;           

  U32                   num_points;
  Point3F*              points;
  F32*                  rolls;
  U32                   loop_type;
  F32                   mult;
  F32                   time_offset;
  bool                  reverse;
  Point3F               offset;

public:
  /*C*/                 afxPathData();
  /*D*/                 ~afxPathData();

  virtual bool          onAdd();
  virtual void          onRemove();
  virtual void          packData(BitStream*);
  virtual void          unpackData(BitStream*);
  virtual bool          preload(bool server, char errorBuffer[256]);

  //F32                   calcRotationAngle(F32 elapsed, F32 rate_factor=1.0f);

  static void           initPersistFields();

  DECLARE_CONOBJECT(afxPathData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_PATH_H_
