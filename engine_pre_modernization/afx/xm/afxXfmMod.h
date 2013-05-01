
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _AFX_XFM_MOD_BASE_H_
#define _AFX_XFM_MOD_BASE_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/gameBase.h"

#include "math/mPoint.h"
#include "math/mMatrix.h"
#include "math/mMathFn.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// BASE CLASSES
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class BitStream;
class afxEffectWrapper;

class afxXM_Defs
{
protected:
  enum
  {
    POSITION =    BIT(0),
    ORIENTATION = BIT(1),
    POSITION2 =   BIT(2),
    ALL =         (POSITION | ORIENTATION | POSITION2)
  };
};

class afxXM_Base;

class afxXM_BaseData : public  GameBaseData, public afxXM_Defs
{
  typedef GameBaseData Parent;

public:
  bool          ignore_time_factor;

public:
  /*C*/         afxXM_BaseData();

  void          packData(BitStream* stream);
  void          unpackData(BitStream* stream);
  static void   initPersistFields();

  virtual afxXM_Base* create(afxEffectWrapper* fx) { return 0; }

  DECLARE_CONOBJECT(afxXM_BaseData);
};
DECLARE_CONSOLETYPE(afxXM_BaseData);

class afxXM_Base : public afxXM_Defs
{
protected:
  afxEffectWrapper* fx_wrapper;
  afxXM_BaseData*   datablock;
  F32               time_factor;

public:
  /*C*/             afxXM_Base(afxXM_BaseData*, afxEffectWrapper*);
  virtual           ~afxXM_Base() { }

  virtual void      start(F32 timestamp) { }
  virtual void      update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                           Point3F& scale) { };
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxXM_WeightedBaseData : public  afxXM_BaseData
{
  typedef afxXM_BaseData Parent;

public:
  F32           lifetime;
  F32           delay;
  F32           fade_in_time;
  F32           fade_out_time;

public:
  /*C*/         afxXM_WeightedBaseData();

  bool          hasFixedWeight() const;
  F32           getWeightFactor() const;

  void          packData(BitStream* stream);
  void          unpackData(BitStream* stream);
  static void   initPersistFields();

  DECLARE_CONOBJECT(afxXM_WeightedBaseData);
};

class afxXM_WeightedBase : public afxXM_Base
{
protected:
  F32           wt_fadein;
  F32           wt_fadeout;
  F32           wt_start_time;
  F32           wt_full_time;
  F32           wt_fade_time;
  F32           wt_done_time;

  F32           calc_weight_factor(F32 elapsed);

public:
  /*C*/         afxXM_WeightedBase(afxXM_WeightedBaseData*, afxEffectWrapper*);
  virtual       ~afxXM_WeightedBase() { }
};

inline F32 afxXM_WeightedBase::calc_weight_factor(F32 elapsed)
{
  if (elapsed < wt_start_time)     // pre
    return 0;
  else if (elapsed < wt_full_time) // fade-in
  {
    F32 t = elapsed - wt_start_time;
    return t/wt_fadein;
  }
  else if (elapsed < wt_fade_time) // full
    return 1;
  else if (elapsed < wt_done_time) // fade-out
  {
    F32 t = wt_done_time - elapsed;
    return t/wt_fadeout;
  }
  else                             // post
    return 0;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_XFM_MOD_BASE_H_
