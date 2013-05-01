
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

class afxXM_AimData : public afxXM_WeightedBaseData
{
  typedef afxXM_WeightedBaseData Parent;

public:
  bool          aim_z_only;

public:
  /*C*/         afxXM_AimData();

  void          packData(BitStream* stream);
  void          unpackData(BitStream* stream);
  static void   initPersistFields();

  afxXM_Base*   create(afxEffectWrapper* fx, bool on_server);

  DECLARE_CONOBJECT(afxXM_AimData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxConstraint;

class afxXM_Aim_weighted : public afxXM_WeightedBase
{
  typedef afxXM_WeightedBase Parent;
  afxConstraint*  aim_cons;

public:
  /*C*/           afxXM_Aim_weighted(afxXM_AimData*, afxEffectWrapper*);

  virtual void    update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxXM_Aim_weighted_z : public afxXM_WeightedBase
{
  typedef afxXM_WeightedBase Parent;
  afxConstraint*  aim_cons;

public:
  /*C*/           afxXM_Aim_weighted_z(afxXM_AimData*, afxEffectWrapper*);

  virtual void    update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxXM_Aim_fixed : public afxXM_Base
{
  typedef afxXM_WeightedBase Parent;
  afxConstraint*  aim_cons;

public:
  /*C*/           afxXM_Aim_fixed(afxXM_AimData*, afxEffectWrapper*);

  virtual void    update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxXM_Aim_fixed_z : public afxXM_Base
{
  typedef afxXM_WeightedBase Parent;
  afxConstraint*  aim_cons;

public:
  /*C*/           afxXM_Aim_fixed_z(afxXM_AimData*, afxEffectWrapper*);

  virtual void    update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CO_DATABLOCK_V1(afxXM_AimData);

afxXM_AimData::afxXM_AimData()
{
  aim_z_only = false;
}

void afxXM_AimData::initPersistFields()
{
  Parent::initPersistFields();
  addField("aimZOnly",  TypeBool,    Offset(aim_z_only, afxXM_AimData));
}

void afxXM_AimData::packData(BitStream* stream)
{
  Parent::packData(stream);
  stream->write(aim_z_only);
}

void afxXM_AimData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
  stream->read(&aim_z_only);
}

afxXM_Base* afxXM_AimData::create(afxEffectWrapper* fx, bool on_server)
{
  if (aim_z_only)
  {
    if (hasFixedWeight())
      return new afxXM_Aim_fixed_z(this, fx);
    else
      return new afxXM_Aim_weighted_z(this, fx);
  }
  else
  {
    if (hasFixedWeight())
      return new afxXM_Aim_fixed(this, fx);
    else
      return new afxXM_Aim_weighted(this, fx);
  }
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Aim_weighted::afxXM_Aim_weighted(afxXM_AimData* db, afxEffectWrapper* fxw) 
: afxXM_WeightedBase(db, fxw)
{ 
  aim_cons = 0;
}

void afxXM_Aim_weighted::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                       Point3F& scale)
{
  if (!aim_cons)
  {
    aim_cons = fx_wrapper->getAimConstraint();
    if (!aim_cons)
      return;
  }
      
  Point3F aim_at_pos;
  aim_cons->getPosition(aim_at_pos);
  
  VectorF line_of_sight = aim_at_pos - pos;
  line_of_sight.normalize();

  F32 wt_factor = calc_weight_factor(elapsed); 

  QuatF qt_ori_old( ori );
  
  MatrixF ori_new = MathUtils::createOrientFromDir(line_of_sight); 
  QuatF qt_ori_new( ori_new );

  QuatF qt_ori = qt_ori_old.slerp(qt_ori_new, wt_factor);

  qt_ori.setMatrix( &ori );
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Aim_weighted_z::afxXM_Aim_weighted_z(afxXM_AimData* db, afxEffectWrapper* fxw) 
: afxXM_WeightedBase(db, fxw)
{ 
  aim_cons = 0;
}

void afxXM_Aim_weighted_z::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                                  Point3F& scale)
{
  if (!aim_cons)
  {
    aim_cons = fx_wrapper->getAimConstraint();
    if (!aim_cons)
      return;
  }
      
  Point3F aim_at_pos;
  aim_cons->getPosition(aim_at_pos);
  
  aim_at_pos.z = pos.z;

  VectorF line_of_sight = aim_at_pos - pos;
  line_of_sight.normalize();

  F32 wt_factor = calc_weight_factor(elapsed); 

  QuatF qt_ori_old( ori );
  
  MatrixF ori_new = MathUtils::createOrientFromDir(line_of_sight); 
  QuatF qt_ori_new( ori_new );

  QuatF qt_ori = qt_ori_old.slerp(qt_ori_new, wt_factor);

  qt_ori.setMatrix( &ori );
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Aim_fixed::afxXM_Aim_fixed(afxXM_AimData* db, afxEffectWrapper* fxw) 
: afxXM_Base(db, fxw)
{ 
  aim_cons = 0;
}

void afxXM_Aim_fixed::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                             Point3F& scale)
{
  if (!aim_cons)
  {
    aim_cons = fx_wrapper->getAimConstraint();
    if (!aim_cons)
      return;
  }
      
  Point3F aim_at_pos;
  aim_cons->getPosition(aim_at_pos);
  
  VectorF line_of_sight = aim_at_pos - pos;
  line_of_sight.normalize();
  
  ori = MathUtils::createOrientFromDir(line_of_sight); 
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Aim_fixed_z::afxXM_Aim_fixed_z(afxXM_AimData* db, afxEffectWrapper* fxw) 
: afxXM_Base(db, fxw)
{ 
  aim_cons = 0;
}

void afxXM_Aim_fixed_z::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                         Point3F& scale)
{
  if (!aim_cons)
  {
    aim_cons = fx_wrapper->getAimConstraint();
    if (!aim_cons)
      return;
  }
      
  Point3F aim_at_pos;
  aim_cons->getPosition(aim_at_pos);
  aim_at_pos.z = pos.z;

  VectorF line_of_sight = aim_at_pos - pos;
  line_of_sight.normalize();

  ori = MathUtils::createOrientFromDir(line_of_sight);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//