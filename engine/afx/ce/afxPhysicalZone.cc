
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "game/physicalZone.h"

#include "afx/ce/afxPhysicalZone.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxPhysicalZoneData

IMPLEMENT_CO_DATABLOCK_V1(afxPhysicalZoneData);

afxPhysicalZoneData::afxPhysicalZoneData()
{
  mVelocityMod = 1.0f;
  mGravityMod = 1.0f;
  mAppliedForce.zero();
  mPolyhedron = ST_NULLSTRING;
  force_type = PhysicalZone::VECTOR;
  force_mag = 0.0f;
  orient_force = false;
}

static EnumTable::Enums forces_enums[] =
{
   { PhysicalZone::VECTOR,          "vector"        },
   { PhysicalZone::SPHERICAL,       "spherical"     },
   { PhysicalZone::CYLINDRICAL,     "cylindrical"   },
   // aliases
   { PhysicalZone::SPHERICAL,       "sphere"        },
   { PhysicalZone::CYLINDRICAL,     "cylinder"      },
};
static EnumTable forces_tbl(sizeof(forces_enums) / sizeof(forces_enums[0]), forces_enums);

#define myOffset(field) Offset(field, afxPhysicalZoneData)

void afxPhysicalZoneData::initPersistFields()
{
  Parent::initPersistFields();

  addField("velocityMod",  TypeF32,         myOffset(mVelocityMod));
  addField("gravityMod",   TypeF32,         myOffset(mGravityMod));
  addField("appliedForce", TypePoint3F,     myOffset(mAppliedForce));
  addField("polyhedron",   TypeString,      myOffset(mPolyhedron));
  addField("forceType",    TypeEnum,        myOffset(force_type), 1, &forces_tbl);
  addField("orientForce",  TypeBool,        myOffset(orient_force));
}

void afxPhysicalZoneData::packData(BitStream* stream)
{
	Parent::packData(stream);
}

void afxPhysicalZoneData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//