
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

class afxXM_ScaleData : public afxXM_WeightedBaseData
{
  typedef afxXM_WeightedBaseData Parent;

public:
  Point3F       scale;

public:
  /*C*/         afxXM_ScaleData();

  void          packData(BitStream* stream);
  void          unpackData(BitStream* stream);
  static void   initPersistFields();

  afxXM_Base*   create(afxEffectWrapper* fx);

  DECLARE_CONOBJECT(afxXM_ScaleData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxXM_Scale_weighted : public afxXM_WeightedBase
{
  typedef afxXM_WeightedBase Parent;

  Point3F       xm_scale;

public:
  /*C*/         afxXM_Scale_weighted(afxXM_ScaleData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CO_DATABLOCK_V1(afxXM_ScaleData);

afxXM_ScaleData::afxXM_ScaleData()
{
  scale.set(0,0,0);
}

void afxXM_ScaleData::initPersistFields()
{
  Parent::initPersistFields();
  addField("scale",  TypePoint3F,   Offset(scale, afxXM_ScaleData));
}

void afxXM_ScaleData::packData(BitStream* stream)
{
  Parent::packData(stream);
  mathWrite(*stream, scale);
}

void afxXM_ScaleData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
  mathRead(*stream, &scale);
}

afxXM_Base* afxXM_ScaleData::create(afxEffectWrapper* fx)
{
  if (hasFixedWeight())
    return new afxXM_Scale_weighted(this, fx);
  else
    return new afxXM_Scale_weighted(this, fx);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Scale_weighted::afxXM_Scale_weighted(afxXM_ScaleData* db, afxEffectWrapper* fxw) 
: afxXM_WeightedBase(db, fxw) 
{ 
  xm_scale = db->scale; 
}

void afxXM_Scale_weighted::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, Point3F& scale)
{
  F32 wt_factor = calc_weight_factor(elapsed);
  scale += xm_scale*wt_factor;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//