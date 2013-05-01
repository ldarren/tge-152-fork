
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "afx/afxEffectWrapper.h"
#include "afx/xm/afxXfmMod.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxXM_FreezeData : public afxXM_BaseData
{
  typedef afxXM_BaseData Parent;

public:
  U32           mask;

public:
  /*C*/         afxXM_FreezeData() : mask(POSITION | ORIENTATION | POSITION2) { }

  void          packData(BitStream* stream);
  void          unpackData(BitStream* stream);
  static void   initPersistFields();

  afxXM_Base*   create(afxEffectWrapper* fx);

  DECLARE_CONOBJECT(afxXM_FreezeData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxXM_Freeze : public afxXM_Base
{
  typedef afxXM_Base Parent;

  U32           mask;
  bool          first;
  Point3F       frozen_pos;
  MatrixF       frozen_ori;
  Point3F       frozen_aim;

public:
  /*C*/         afxXM_Freeze(afxXM_FreezeData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxXM_Freeze_all : public afxXM_Base
{
  typedef afxXM_Base Parent;

  bool          first;
  Point3F       frozen_pos;
  MatrixF       frozen_ori;
  Point3F       frozen_aim;

public:
  /*C*/         afxXM_Freeze_all(afxXM_FreezeData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxXM_Freeze_pos : public afxXM_Base
{
  typedef afxXM_Base Parent;

  bool          first;
  Point3F       frozen_pos;

public:
  /*C*/         afxXM_Freeze_pos(afxXM_FreezeData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxXM_Freeze_pos2 : public afxXM_Base
{
  typedef afxXM_Base Parent;

  bool          first;
  Point3F       frozen_pos2;

public:
  /*C*/         afxXM_Freeze_pos2(afxXM_FreezeData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

class afxXM_Freeze_ori : public afxXM_Base
{
  typedef afxXM_Base Parent;

  bool          first;
  MatrixF       frozen_ori;

public:
  /*C*/         afxXM_Freeze_ori(afxXM_FreezeData*, afxEffectWrapper*);

  virtual void  update(F32 dt, F32 elapsed, Point3F& p, MatrixF& o, Point3F& p2, Point3F& s);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CO_DATABLOCK_V1(afxXM_FreezeData);

void afxXM_FreezeData::initPersistFields()
{
  Parent::initPersistFields();
  addField("mask",  TypeS32,    Offset(mask, afxXM_FreezeData));
}

void afxXM_FreezeData::packData(BitStream* stream)
{
  Parent::packData(stream);
  stream->write(mask);
}

void afxXM_FreezeData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
  stream->read(&mask);
}

afxXM_Base* afxXM_FreezeData::create(afxEffectWrapper* fx)
{
  if (mask == ALL)
    return new afxXM_Freeze_all(this, fx);
  if (mask == POSITION)
    return new afxXM_Freeze_pos(this, fx);
  if (mask == ORIENTATION)
    return new afxXM_Freeze_ori(this, fx);
  if (mask == POSITION2)
    return new afxXM_Freeze_pos2(this, fx);
  return new afxXM_Freeze(this, fx);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Freeze::afxXM_Freeze(afxXM_FreezeData* db, afxEffectWrapper* fxw) 
: afxXM_Base(db, fxw)
{ 
  mask = db->mask; 
  first = true;
}

void afxXM_Freeze::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                          Point3F& scale)
{
  if (first)
  {
    if (mask & POSITION)
      frozen_pos = pos;
    if (mask & ORIENTATION)
      frozen_ori = ori;
    if (mask & POSITION2)
      frozen_aim = pos2;
    first = false;
  }
  else
  {
    if (mask & POSITION)
      pos = frozen_pos;
    if (mask & ORIENTATION)
      ori = frozen_ori;
    if (mask & POSITION2)
      pos2 = frozen_aim;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Freeze_all::afxXM_Freeze_all(afxXM_FreezeData* db, afxEffectWrapper* fxw) 
: afxXM_Base(db, fxw)
{ 
  first = true;
}

void afxXM_Freeze_all::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                              Point3F& scale)
{
  if (first)
  {
    frozen_pos = pos;
    frozen_ori = ori;
    frozen_aim = pos2;
    first = false;
  }
  else
  {
    pos = frozen_pos;
    ori = frozen_ori;
    pos2 = frozen_aim;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Freeze_pos::afxXM_Freeze_pos(afxXM_FreezeData* db, afxEffectWrapper* fxw) 
: afxXM_Base(db, fxw)
{ 
  first = true;
}

void afxXM_Freeze_pos::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                              Point3F& scale)
{
  if (first)
  {
    frozen_pos = pos;
    first = false;
  }
  else
    pos = frozen_pos;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Freeze_pos2::afxXM_Freeze_pos2(afxXM_FreezeData* db, afxEffectWrapper* fxw) 
: afxXM_Base(db, fxw)
{ 
  first = true;
}

void afxXM_Freeze_pos2::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                                Point3F& scale)
{
  if (first)
  {
    frozen_pos2 = pos2;
    first = false;
  }
  else
    pos2 = frozen_pos2;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Freeze_ori::afxXM_Freeze_ori(afxXM_FreezeData* db, afxEffectWrapper* fxw) 
: afxXM_Base(db, fxw)
{ 
  first = true;
}

void afxXM_Freeze_ori::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                              Point3F& scale)
{
  if (first)
  {
    frozen_ori = ori;
    first = false;
  }
  else
    ori = frozen_ori;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//