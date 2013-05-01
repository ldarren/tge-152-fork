
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "math/mathIO.h"
#include "math/mathUtils.h"

#include "afx/afxEffectWrapper.h"
#include "afx/xm/afxXfmMod.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxXM_RandomRotData : public afxXM_BaseData
{
  typedef afxXM_BaseData Parent;

public:
  Point3F       axis;
  F32           theta_min;
  F32           theta_max;
  F32           phi_min;
  F32           phi_max;

public:
  /*C*/         afxXM_RandomRotData(); 

  void          packData(BitStream* stream);
  void          unpackData(BitStream* stream);
  bool          onAdd();
  static void   initPersistFields();

  afxXM_Base*   create(afxEffectWrapper* fx);

  DECLARE_CONOBJECT(afxXM_RandomRotData);
};

class afxXM_RandomRot : public afxXM_Base
{
  typedef afxXM_Base Parent;
  MatrixF       rand_ori;

public:
  /*C*/         afxXM_RandomRot(afxXM_RandomRotData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CO_DATABLOCK_V1(afxXM_RandomRotData);

afxXM_RandomRotData::afxXM_RandomRotData()
{
  axis.set(0,0,1);
  theta_min = 0.0f; 
  theta_max = 360.0f;
  phi_min = 0.0f; 
  phi_max = 360.0f;
}

void afxXM_RandomRotData::initPersistFields()
{
  Parent::initPersistFields();
  addField("axis",      TypePoint3F,  Offset(axis, afxXM_RandomRotData));
  addField("thetaMin",  TypeF32,      Offset(theta_min, afxXM_RandomRotData));
  addField("thetaMax",  TypeF32,      Offset(theta_max, afxXM_RandomRotData));
  addField("phiMin",    TypeF32,      Offset(phi_min, afxXM_RandomRotData));
  addField("phiMax",    TypeF32,      Offset(phi_max, afxXM_RandomRotData));
}

void afxXM_RandomRotData::packData(BitStream* stream)
{
  Parent::packData(stream);
  mathWrite(*stream, axis);
  stream->write(theta_min);
  stream->write(theta_max);
  stream->write(phi_min);
  stream->write(phi_max);
}

void afxXM_RandomRotData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
  mathRead(*stream, &axis);
  stream->read(&theta_min);
  stream->read(&theta_max);
  stream->read(&phi_min);
  stream->read(&phi_max);
}

bool afxXM_RandomRotData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;

  axis.normalizeSafe();

  return true;
}

afxXM_Base* afxXM_RandomRotData::create(afxEffectWrapper* fx)
{
  return new afxXM_RandomRot(this, fx);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_RandomRot::afxXM_RandomRot(afxXM_RandomRotData* db, afxEffectWrapper* fxw)
: afxXM_Base(db, fxw) 
{ 
  Point3F rand_dir = MathUtils::randomDir(db->axis, db->theta_min, db->theta_max, db->phi_min, db->phi_max);
  rand_ori = MathUtils::createOrientFromDir(rand_dir);
}

void afxXM_RandomRot::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, Point3F& scale)
{
  ori = rand_ori;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//