
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "game/shapeBase.h"

#include "afx/ce/afxProjectile.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxProjectile

IMPLEMENT_CO_NETOBJECT_V1(afxProjectile);

afxProjectile::afxProjectile()
{
}

afxProjectile::~afxProjectile()
{
}

void afxProjectile::init(Point3F& pos, Point3F& vel, ShapeBase* src_obj)
{
  mCurrPosition = pos;
  mCurrVelocity = vel;
  if (src_obj)
  {
    mSourceObject = src_obj;
    mSourceObjectId = src_obj->getId();
    mSourceObjectSlot = 0;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

bool afxProjectile::onNewDataBlock(GameBaseData* dptr)
{
  return Parent::onNewDataBlock(dptr);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//