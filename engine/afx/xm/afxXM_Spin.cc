
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "math/mathIO.h"
#include "math/mathUtils.h"
#include "math/mRandom.h"

#include "afx/afxEffectWrapper.h"
#include "afx/xm/afxXfmMod.h"
#include "afx/util/afxPath3D.h"
#include "afx/util/afxPath.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxXM_SpinData : public afxXM_WeightedBaseData
{
  typedef afxXM_WeightedBaseData Parent;

public:
  Point3F       spin_axis;
  F32           spin_angle;
  F32           spin_angle_var;
  F32           spin_rate;
  F32           spin_rate_var;

public:
  /*C*/         afxXM_SpinData() : spin_axis(0,0,1), spin_angle(0),  spin_angle_var(0), spin_rate(0), spin_rate_var(0) { }

  void          packData(BitStream* stream);
  void          unpackData(BitStream* stream);
  bool          onAdd();
  static void   initPersistFields();

  afxXM_Base*   create(afxEffectWrapper* fx, bool on_server);

  DECLARE_CONOBJECT(afxXM_SpinData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxXM_Spin_weighted : public afxXM_WeightedBase
{
  typedef afxXM_WeightedBase Parent;

  Point3F       spin_axis;
  F32           spin_rate;
  F32           theta;

public:
  /*C*/         afxXM_Spin_weighted(afxXM_SpinData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxXM_Spin_fixed : public afxXM_Base
{
  typedef afxXM_WeightedBase Parent;

  Point3F       spin_axis;
  F32           spin_rate;
  F32           theta;

public:
  /*C*/         afxXM_Spin_fixed(afxXM_SpinData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CO_DATABLOCK_V1(afxXM_SpinData);

void afxXM_SpinData::initPersistFields()
{
  Parent::initPersistFields();
  addField("spinAxis",            TypePoint3F,  Offset(spin_axis, afxXM_SpinData));
  addField("spinAngle",           TypeF32,      Offset(spin_angle, afxXM_SpinData));
  addField("spinAngleVariance",   TypeF32,      Offset(spin_angle_var, afxXM_SpinData));
  addField("spinRate",            TypeF32,      Offset(spin_rate, afxXM_SpinData));
  addField("spinRateVariance",    TypeF32,      Offset(spin_rate_var, afxXM_SpinData));
}

void afxXM_SpinData::packData(BitStream* stream)
{
  Parent::packData(stream);
  mathWrite(*stream, spin_axis);
  stream->write(spin_angle);
  stream->write(spin_angle_var);
  stream->write(spin_rate);
  stream->write(spin_rate_var);
}

void afxXM_SpinData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
  mathRead(*stream, &spin_axis);
  stream->read(&spin_angle);
  stream->read(&spin_angle_var);
  stream->read(&spin_rate);
  stream->read(&spin_rate_var);
}

bool afxXM_SpinData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;

  spin_axis.normalizeSafe();

  return true;
}

afxXM_Base* afxXM_SpinData::create(afxEffectWrapper* fx, bool on_server)
{
  if (hasFixedWeight())
    return new afxXM_Spin_fixed(this, fx);
  else
    return new afxXM_Spin_weighted(this, fx);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Spin_weighted::afxXM_Spin_weighted(afxXM_SpinData* db, afxEffectWrapper* fxw)
: afxXM_WeightedBase(db, fxw) 
{ 
  spin_axis = db->spin_axis;

  spin_rate = db->spin_rate;
  if (db->spin_rate_var != 0.0f)
    spin_rate += gRandGen.randF()*2.0f*db->spin_rate_var - db->spin_rate_var;
  spin_rate *= db->getWeightFactor()/time_factor;

  F32 spin_angle = db->spin_angle;
  if (db->spin_angle_var != 0.0f)
    spin_angle += gRandGen.randF()*2.0f*db->spin_angle_var - db->spin_angle_var;
  theta = mDegToRad(spin_angle);
}

void afxXM_Spin_weighted::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, Point3F& scale)
{
  F32 wt_factor = calc_weight_factor(elapsed);
  F32 rate = spin_rate*wt_factor;
  theta += mDegToRad(dt*rate);

  AngAxisF spin_aa(spin_axis, theta);
  MatrixF spin_xfm; spin_aa.setMatrix(&spin_xfm);
  
  ori.mul(spin_xfm);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Spin_fixed::afxXM_Spin_fixed(afxXM_SpinData* db, afxEffectWrapper* fxw)
: afxXM_Base(db, fxw) 
{ 
  spin_axis = db->spin_axis;

  spin_rate = db->spin_rate;
  if (db->spin_rate_var != 0.0f)
    spin_rate += gRandGen.randF()*2.0f*db->spin_rate_var - db->spin_rate_var;
  spin_rate *= db->getWeightFactor()/time_factor;

  F32 spin_angle = db->spin_angle;
  if (db->spin_angle_var != 0.0f)
    spin_angle += gRandGen.randF()*2.0f*db->spin_angle_var - db->spin_angle_var;
  theta = mDegToRad(spin_angle);
}

void afxXM_Spin_fixed::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, Point3F& scale)
{
  theta += mDegToRad(dt*spin_rate);

  AngAxisF spin_aa(spin_axis, theta);
  MatrixF spin_xfm; spin_aa.setMatrix(&spin_xfm);
  
  ori.mul(spin_xfm);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//