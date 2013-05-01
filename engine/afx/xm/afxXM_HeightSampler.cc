
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

class afxXM_HeightSamplerData : public afxXM_WeightedBaseData
{
  typedef afxXM_WeightedBaseData Parent;

public:
  bool          aim_z_only;

public:
  /*C*/         afxXM_HeightSamplerData();

  void          packData(BitStream* stream);
  void          unpackData(BitStream* stream);
  static void   initPersistFields();

  afxXM_Base*   create(afxEffectWrapper* fx, bool on_server);

  DECLARE_CONOBJECT(afxXM_HeightSamplerData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxConstraint;

class afxXM_HeightSampler : public afxXM_WeightedBase
{
  typedef afxXM_WeightedBase Parent;

public:
  /*C*/           afxXM_HeightSampler(afxXM_HeightSamplerData*, afxEffectWrapper*);

  virtual void    update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CO_DATABLOCK_V1(afxXM_HeightSamplerData);

afxXM_HeightSamplerData::afxXM_HeightSamplerData()
{
  aim_z_only = false;
}

void afxXM_HeightSamplerData::initPersistFields()
{
  Parent::initPersistFields();
  addField("aimZOnly",  TypeBool,    Offset(aim_z_only, afxXM_HeightSamplerData));
}

void afxXM_HeightSamplerData::packData(BitStream* stream)
{
  Parent::packData(stream);
  stream->write(aim_z_only);
}

void afxXM_HeightSamplerData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
  stream->read(&aim_z_only);
}

afxXM_Base* afxXM_HeightSamplerData::create(afxEffectWrapper* fx, bool on_server)
{
  return new afxXM_HeightSampler(this, fx);
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_HeightSampler::afxXM_HeightSampler(afxXM_HeightSamplerData* db, afxEffectWrapper* fxw) 
: afxXM_WeightedBase(db, fxw)
{ 
}

void afxXM_HeightSampler::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, Point3F& scale)
{
  afxConstraint* pos_cons = fx_wrapper->getPosConstraint();
  if (!pos_cons)
    return;
      
  Point3F base_pos;
  pos_cons->getPosition(base_pos);
  
  F32 range = 0.5f;
  F32 height = (base_pos.z > pos.z) ? (base_pos.z - pos.z) : 0.0f;
  F32 factor = mClampF(1.0f - (height/range), 0.0f, 1.0f);

  //Con::printf("SET height=%g liveScaleFactor=%g", height, factor);
  fx_wrapper->setField("liveScaleFactor", avar("%g", factor));
  fx_wrapper->setField("liveFadeFactor", avar("%g", factor));
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//