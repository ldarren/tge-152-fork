
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "math/mathIO.h"
#include "math/mathUtils.h"

#include "afx/afxEffectWrapper.h"
#include "afx/xm/afxXfmMod.h"
#include "afx/util/afxPath3D.h"
#include "afx/util/afxPath.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxXM_ShockwaveData : public afxXM_BaseData
{
  typedef afxXM_BaseData Parent;

public:
  F32           rate;
  bool          aim_z_only;

public:
  /*C*/         afxXM_ShockwaveData();

  void          packData(BitStream* stream);
  void          unpackData(BitStream* stream);
  static void   initPersistFields();

  afxXM_Base*   create(afxEffectWrapper* fx);

  DECLARE_CONOBJECT(afxXM_ShockwaveData);
};

class afxConstraint;

class afxXM_Shockwave : public afxXM_Base
{
  typedef afxXM_Base Parent;

  afxXM_ShockwaveData*  db;
  bool                  first;
  Point3F               fixed_pos;

public:
  /*C*/         afxXM_Shockwave(afxXM_ShockwaveData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                       Point3F& scale);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CO_DATABLOCK_V1(afxXM_ShockwaveData);

afxXM_ShockwaveData::afxXM_ShockwaveData()
{
  rate = 1.0f;
  aim_z_only = false;
}

void afxXM_ShockwaveData::initPersistFields()
{
  Parent::initPersistFields();
  addField("rate",      TypeF32,      Offset(rate, afxXM_ShockwaveData));
  addField("aimZOnly",  TypeBool,     Offset(aim_z_only, afxXM_ShockwaveData));
}

void afxXM_ShockwaveData::packData(BitStream* stream)
{
  Parent::packData(stream);
  stream->write(rate);
  stream->write(aim_z_only);
}

void afxXM_ShockwaveData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
  stream->read(&rate);
  stream->read(&aim_z_only);
}

afxXM_Base* afxXM_ShockwaveData::create(afxEffectWrapper* fx)
{
  return new afxXM_Shockwave(this, fx);
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Shockwave::afxXM_Shockwave(afxXM_ShockwaveData* db, afxEffectWrapper* fxw) 
: afxXM_Base(db, fxw)
{ 
  this->db = db; 
  first = true;
  fixed_pos.zero();
}

void afxXM_Shockwave::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, Point3F& scale)
{
  if (first)
  {
    fixed_pos = pos;
    first = false;
  }

  Point3F aim_at_pos = pos2;
  if (db->aim_z_only)
    aim_at_pos.z = fixed_pos.z;
  
  VectorF line_of_sight = aim_at_pos - fixed_pos;
  line_of_sight.normalize();

  pos = fixed_pos + line_of_sight*(elapsed*db->rate);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//