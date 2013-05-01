
#ifndef _AFX_PROJECTILE_H_
#define _AFX_PROJECTILE_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/projectile.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxProjectile

class afxProjectile : public Projectile
{
  typedef Projectile Parent;

public:
  /*C*/               afxProjectile();
  /*D*/               ~afxProjectile();

  void                init(Point3F& pos, Point3F& vel, ShapeBase* src_obj);

  virtual bool        onNewDataBlock(GameBaseData* dptr);

  DECLARE_CONOBJECT(afxProjectile);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_PROJECTILE_H_
