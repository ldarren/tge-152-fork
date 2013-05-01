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

class afxXM_OscillateData : public afxXM_WeightedBaseData
{
  typedef afxXM_WeightedBaseData Parent;

public:
  U32           mask;

  Point3F       min;
  Point3F       max;

  F32           speed;

  Point3F       axis;

  bool          additive_scale;
  bool          local_offset;

  // offset?

public:
  /*C*/         afxXM_OscillateData();

  void          packData(BitStream* stream);
  void          unpackData(BitStream* stream);
  static void   initPersistFields();

  afxXM_Base*   create(afxEffectWrapper* fx, bool on_server);

  DECLARE_CONOBJECT(afxXM_OscillateData);
};

class afxXM_Oscillate_rot : public afxXM_WeightedBase
{
  typedef afxXM_WeightedBase Parent;

  afxXM_OscillateData* db;

public:
  /*C*/         afxXM_Oscillate_rot(afxXM_OscillateData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                       Point3F& scale);
};

class afxXM_Oscillate_scale : public afxXM_WeightedBase
{
  typedef afxXM_WeightedBase Parent;

  afxXM_OscillateData* db;

public:
  /*C*/         afxXM_Oscillate_scale(afxXM_OscillateData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                       Point3F& scale);
};

class afxXM_Oscillate_position : public afxXM_WeightedBase
{
  typedef afxXM_WeightedBase Parent;

  afxXM_OscillateData* db;

public:
  /*C*/         afxXM_Oscillate_position(afxXM_OscillateData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                       Point3F& scale);
};

class afxXM_Oscillate_position2 : public afxXM_WeightedBase
{
  typedef afxXM_WeightedBase Parent;

  afxXM_OscillateData* db;

public:
  /*C*/         afxXM_Oscillate_position2(afxXM_OscillateData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                       Point3F& scale);
};

class afxXM_Oscillate : public afxXM_WeightedBase
{
  typedef afxXM_WeightedBase Parent;

  afxXM_OscillateData* db;

public:
  /*C*/         afxXM_Oscillate(afxXM_OscillateData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                       Point3F& scale);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CO_DATABLOCK_V1(afxXM_OscillateData);

afxXM_OscillateData::afxXM_OscillateData()
{
  mask = POSITION;
  min.set(0,0,0);
  max.set(1,1,1);
  speed = 1.0f;
  axis.set(0,0,1);
  additive_scale = false;
  local_offset = true;
}

void afxXM_OscillateData::initPersistFields()
{
  Parent::initPersistFields();
  addField("mask",                TypeS32,      Offset(mask,  afxXM_OscillateData));
  addField("min",                 TypePoint3F,  Offset(min,   afxXM_OscillateData));
  addField("max",                 TypePoint3F,  Offset(max,   afxXM_OscillateData));
  addField("speed",               TypeF32,      Offset(speed, afxXM_OscillateData));
  addField("axis",                TypePoint3F,  Offset(axis,  afxXM_OscillateData));
  addField("additiveScale",       TypeBool,     Offset(additive_scale, afxXM_OscillateData));
  addField("localOffset",         TypeBool,     Offset(local_offset,   afxXM_OscillateData));
}

void afxXM_OscillateData::packData(BitStream* stream)
{
  Parent::packData(stream);
  stream->write(mask);
  mathWrite(*stream, min);
  mathWrite(*stream, max);
  stream->write(speed);
  mathWrite(*stream, axis);
  stream->writeFlag(additive_scale);
  stream->writeFlag(local_offset);
}

void afxXM_OscillateData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
  stream->read(&mask);
  mathRead(*stream, &min);
  mathRead(*stream, &max);
  stream->read(&speed);
  mathRead(*stream, &axis);
  additive_scale = stream->readFlag();
  local_offset = stream->readFlag();
}

afxXM_Base* afxXM_OscillateData::create(afxEffectWrapper* fx, bool on_server)
{
  if (mask == ORIENTATION)
    return new afxXM_Oscillate_rot(this, fx);
  if (mask == SCALE)
	  return new afxXM_Oscillate_scale(this, fx);
  if (mask == POSITION)
    return new afxXM_Oscillate_position(this, fx);
  if (mask == POSITION2)
    return new afxXM_Oscillate_position2(this, fx);
  return new afxXM_Oscillate(this, fx);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

inline F32 lerp(F32 t, F32 a, F32 b)
{
  return a + t * (b - a);
}

inline Point3F lerpV(F32 t, const Point3F& a, const Point3F& b)
{
  return Point3F( a.x + t * (b.x - a.x),
                  a.y + t * (b.y - a.y),
                  a.z + t * (b.z - a.z) );
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Oscillate_rot::afxXM_Oscillate_rot(afxXM_OscillateData* db, afxEffectWrapper* fxw) 
: afxXM_WeightedBase(db, fxw) 
{ 
  this->db = db; 
}

void afxXM_Oscillate_rot::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, 
                                 Point3F& pos2, Point3F& scale)
{
  F32 wt_factor = calc_weight_factor(elapsed);

  F32 t = mSin(db->speed*elapsed);  // [-1,1]
  F32 theta = lerp((t+1)/2, db->min.x*wt_factor, db->max.x*wt_factor);
  theta = mDegToRad(theta);

  AngAxisF rot_aa(db->axis, theta);
  MatrixF rot_xfm; rot_aa.setMatrix(&rot_xfm);
  
  ori.mul(rot_xfm);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Oscillate_scale::afxXM_Oscillate_scale(afxXM_OscillateData* db, afxEffectWrapper* fxw) 
: afxXM_WeightedBase(db, fxw) 
{ 
  this->db = db; 
}

void afxXM_Oscillate_scale::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, 
                                 Point3F& pos2, Point3F& scale)
{
  F32 wt_factor = calc_weight_factor(elapsed);

  F32 t = mSin(db->speed*elapsed);  // [-1,1]
  F32 s = lerp((t+1)/2, db->min.x*wt_factor, db->max.x*wt_factor);
  Point3F xm_scale = db->axis*s;
  
  if (db->additive_scale)
    scale += xm_scale;
  else
    scale *= xm_scale;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Oscillate_position::afxXM_Oscillate_position(afxXM_OscillateData* db, afxEffectWrapper* fxw) 
: afxXM_WeightedBase(db, fxw) 
{ 
  this->db = db; 
}

void afxXM_Oscillate_position::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, 
                                 Point3F& pos2, Point3F& scale)
{
  F32 wt_factor = calc_weight_factor(elapsed);

  F32 t = mSin(db->speed*elapsed);  // [-1,1]
  Point3F offset = lerpV(t, db->min*wt_factor, db->max*wt_factor);
  
  if (db->local_offset)
  {
    ori.mulV(offset);
    pos += offset;
  }
  else
    pos += offset;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Oscillate_position2::afxXM_Oscillate_position2(afxXM_OscillateData* db, afxEffectWrapper* fxw) 
: afxXM_WeightedBase(db, fxw) 
{ 
  this->db = db; 
}

void afxXM_Oscillate_position2::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, 
                                 Point3F& pos2, Point3F& scale)
{
  F32 wt_factor = calc_weight_factor(elapsed);

  F32 t = mSin(db->speed*elapsed);  // [-1,1]
  Point3F offset = lerpV(t, db->min*wt_factor, db->max*wt_factor);

  pos2 += offset;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Oscillate::afxXM_Oscillate(afxXM_OscillateData* db, afxEffectWrapper* fxw) 
: afxXM_WeightedBase(db, fxw) 
{ 
  this->db = db; 
}

void afxXM_Oscillate::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, 
                                 Point3F& pos2, Point3F& scale)
{
  F32 wt_factor = calc_weight_factor(elapsed);

  F32 t = mSin(db->speed*elapsed);  // [-1,1]

  if (db->mask & POSITION)
  {
    Point3F offset = lerpV(t, db->min*wt_factor, db->max*wt_factor);
    if (db->local_offset)
    {
      ori.mulV(offset);
      pos += offset;
    }
    else
      pos += offset;
  }

  if (db->mask & POSITION2)
  {
    Point3F offset = lerpV(t, db->min*wt_factor, db->max*wt_factor);
    pos2 += offset;
  }

  if (db->mask & SCALE)
  {
    F32 s = lerp((t+1)/2, db->min.x*wt_factor, db->max.x*wt_factor);
    Point3F xm_scale = db->axis*s;
    if (db->additive_scale)
      scale += xm_scale;
    else
      scale *= xm_scale;
  }
  
  if (db->mask & ORIENTATION)
  {
    F32 theta = lerp((t+1)/2, db->min.x*wt_factor, db->max.x*wt_factor);
    theta = mDegToRad(theta);
    AngAxisF rot_aa(db->axis, theta);
    MatrixF rot_xfm; rot_aa.setMatrix(&rot_xfm);
    ori.mul(rot_xfm);
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

