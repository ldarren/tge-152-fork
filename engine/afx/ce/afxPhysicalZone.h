
#ifndef _AFX_PHYSICAL_ZONE_H_
#define _AFX_PHYSICAL_ZONE_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/gameBase.h"
#include "game/trigger.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxPhysicalZoneData

class afxPhysicalZoneData : public GameBaseData
{
  typedef GameBaseData Parent;

public:
   F32              mVelocityMod;
   F32              mGravityMod;
   Point3F          mAppliedForce;
   StringTableEntry mPolyhedron;
   S32              force_type;
   F32              force_mag;
   bool             orient_force;

public:
  /*C*/             afxPhysicalZoneData();

  virtual void      packData(BitStream*);
  virtual void      unpackData(BitStream*);

  static void       initPersistFields();

  DECLARE_CONOBJECT(afxPhysicalZoneData);
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_PHYSICAL_ZONE_H_
