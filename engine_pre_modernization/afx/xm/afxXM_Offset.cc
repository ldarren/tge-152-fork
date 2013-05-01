
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
// LOCAL OFFSET

class afxXM_LocalOffsetData : public afxXM_WeightedBaseData
{
  typedef afxXM_WeightedBaseData Parent;
  
public:
  Point3F       local_offset;
  
public:
  /*C*/         afxXM_LocalOffsetData() : local_offset(0,0,0) { }
  
  void          packData(BitStream* stream);
  void          unpackData(BitStream* stream);
  static void   initPersistFields();
  
  afxXM_Base*   create(afxEffectWrapper* fx);
  
  DECLARE_CONOBJECT(afxXM_LocalOffsetData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxXM_LocalOffset_weighted : public afxXM_WeightedBase
{
  typedef afxXM_WeightedBase Parent;
  Point3F       local_offset;
  
public:
  /*C*/         afxXM_LocalOffset_weighted(afxXM_LocalOffsetData*, afxEffectWrapper*);
  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

// this fixed variation is used when
// the weight factors are constant.

class afxXM_LocalOffset_fixed : public afxXM_Base
{
  typedef afxXM_Base Parent;
  Point3F       local_offset;
  
public:
  /*C*/         afxXM_LocalOffset_fixed(afxXM_LocalOffsetData*, afxEffectWrapper*);
  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// WORLD OFFSET

class afxXM_WorldOffsetData : public afxXM_WeightedBaseData
{
  typedef afxXM_WeightedBaseData Parent;
  
public:
  Point3F       world_offset;
  bool          offset_pos2;
  
public:
  /*C*/         afxXM_WorldOffsetData();
  
  void          packData(BitStream* stream);
  void          unpackData(BitStream* stream);
  static void   initPersistFields();
  
  afxXM_Base*   create(afxEffectWrapper* fx);
  
  DECLARE_CONOBJECT(afxXM_WorldOffsetData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxXM_WorldOffset_weighted : public afxXM_WeightedBase
{
  typedef afxXM_WeightedBase Parent;
  Point3F       world_offset;
  
public:
  /*C*/         afxXM_WorldOffset_weighted(afxXM_WorldOffsetData*, afxEffectWrapper*);
  
  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

// this fixed variation is used when
// the weight factors are constant.

class afxXM_WorldOffset_fixed : public afxXM_Base
{
  typedef afxXM_WeightedBase Parent;
  Point3F       world_offset;
  
public:
  /*C*/         afxXM_WorldOffset_fixed(afxXM_WorldOffsetData*, afxEffectWrapper*);
  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxXM_WorldOffset2_weighted : public afxXM_WeightedBase
{
  typedef afxXM_WeightedBase Parent;
  Point3F       world_offset;
  
public:
  /*C*/         afxXM_WorldOffset2_weighted(afxXM_WorldOffsetData*, afxEffectWrapper*);
  
  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

// this fixed variation is used when
// the weight factors are constant.

class afxXM_WorldOffset2_fixed : public afxXM_Base
{
  typedef afxXM_WeightedBase Parent;
  Point3F       world_offset;
  
public:
  /*C*/         afxXM_WorldOffset2_fixed(afxXM_WorldOffsetData*, afxEffectWrapper*);
  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// LOCAL OFFSET

IMPLEMENT_CO_DATABLOCK_V1(afxXM_LocalOffsetData);

void afxXM_LocalOffsetData::initPersistFields()
{
  Parent::initPersistFields();
  addField("localOffset",  TypePoint3F,   Offset(local_offset, afxXM_LocalOffsetData));
}

void afxXM_LocalOffsetData::packData(BitStream* stream)
{
  Parent::packData(stream);
  mathWrite(*stream, local_offset);
}

void afxXM_LocalOffsetData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
  mathRead(*stream, &local_offset);
}

afxXM_Base* afxXM_LocalOffsetData::create(afxEffectWrapper* fx)
{
  if (hasFixedWeight())
    return new afxXM_LocalOffset_fixed(this, fx);
  else
    return new afxXM_LocalOffset_weighted(this, fx);
}

//~~~~~~~~~~~~~~~~~~~~//

afxXM_LocalOffset_weighted::afxXM_LocalOffset_weighted(afxXM_LocalOffsetData* db, afxEffectWrapper* fxw) 
: afxXM_WeightedBase(db, fxw) 
{ 
  local_offset = db->local_offset*db->getWeightFactor(); 
}

void afxXM_LocalOffset_weighted::update(F32 dt, F32 elapsed, Point3F& pos,
                                        MatrixF& ori, Point3F& pos2, Point3F& scale)
{
  F32 wt_factor = calc_weight_factor(elapsed);
  Point3F offset(local_offset*wt_factor);
  ori.mulV(offset);
  pos += offset;
}

//~~~~~~~~~~~~~~~~~~~~//

afxXM_LocalOffset_fixed::afxXM_LocalOffset_fixed(afxXM_LocalOffsetData* db, afxEffectWrapper* fxw) 
: afxXM_Base(db, fxw) 
{ 
  local_offset = db->local_offset*db->getWeightFactor(); 
}

void afxXM_LocalOffset_fixed::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, 
                                     Point3F& pos2, Point3F& scale)
{
  Point3F offset(local_offset);
  ori.mulV(offset);
  pos += offset;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// WORLD OFFSET

IMPLEMENT_CO_DATABLOCK_V1(afxXM_WorldOffsetData);

afxXM_WorldOffsetData::afxXM_WorldOffsetData()
{
  world_offset.zero();
  offset_pos2 = false;
}

void afxXM_WorldOffsetData::initPersistFields()
{
  Parent::initPersistFields();
  addField("worldOffset",   TypePoint3F,  Offset(world_offset, afxXM_WorldOffsetData));
  addField("offsetPos2",    TypeBool,     Offset(offset_pos2, afxXM_WorldOffsetData));
}

void afxXM_WorldOffsetData::packData(BitStream* stream)
{
  Parent::packData(stream);
  mathWrite(*stream, world_offset);
  stream->write(offset_pos2);
}

void afxXM_WorldOffsetData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
  mathRead(*stream, &world_offset);
  stream->read(&offset_pos2);
}

afxXM_Base* afxXM_WorldOffsetData::create(afxEffectWrapper* fx)
{
  if (offset_pos2)
  {
    if (hasFixedWeight())
      return new afxXM_WorldOffset2_fixed(this, fx);
    else
      return new afxXM_WorldOffset2_weighted(this, fx);
  }
  else
  {
    if (hasFixedWeight())
      return new afxXM_WorldOffset_fixed(this, fx);
    else
      return new afxXM_WorldOffset_weighted(this, fx);
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_WorldOffset_weighted::afxXM_WorldOffset_weighted(afxXM_WorldOffsetData* db, afxEffectWrapper* fxw) 
: afxXM_WeightedBase(db, fxw) 
{ 
  world_offset = db->world_offset*db->getWeightFactor();
}

void afxXM_WorldOffset_weighted::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, Point3F& scale)
{
  F32 wt_factor = calc_weight_factor(elapsed);
  pos += world_offset*wt_factor;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_WorldOffset_fixed::afxXM_WorldOffset_fixed(afxXM_WorldOffsetData* db, afxEffectWrapper* fxw) 
: afxXM_Base(db, fxw) 
{ 
  world_offset = db->world_offset*db->getWeightFactor();
}

void afxXM_WorldOffset_fixed::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, Point3F& scale)
{
  pos += world_offset;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_WorldOffset2_weighted::afxXM_WorldOffset2_weighted(afxXM_WorldOffsetData* db, afxEffectWrapper* fxw) 
: afxXM_WeightedBase(db, fxw) 
{ 
  world_offset = db->world_offset*db->getWeightFactor();
}

void afxXM_WorldOffset2_weighted::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, Point3F& scale)
{
  F32 wt_factor = calc_weight_factor(elapsed);
  pos2 += world_offset*wt_factor;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_WorldOffset2_fixed::afxXM_WorldOffset2_fixed(afxXM_WorldOffsetData* db, afxEffectWrapper* fxw) 
: afxXM_Base(db, fxw) 
{ 
  world_offset = db->world_offset*db->getWeightFactor();
}

void afxXM_WorldOffset2_fixed::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, Point3F& scale)
{
  pos2 += world_offset;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//