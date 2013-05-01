//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "afx/afxEffectWrapper.h"
#include "afx/xm/afxXfmMod.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxXM_GroundConformData : public afxXM_WeightedBaseData
{
  typedef afxXM_WeightedBaseData Parent;

public:
  F32           height;
  bool          do_terrain;
  bool          do_interiors;
  bool          do_orientation;

public:
  /*C*/         afxXM_GroundConformData();

  void          packData(BitStream* stream);
  void          unpackData(BitStream* stream);
  static void   initPersistFields();

  afxXM_Base*   create(afxEffectWrapper* fx, bool on_server);

  DECLARE_CONOBJECT(afxXM_GroundConformData);
};

class afxXM_GroundConform : public afxXM_WeightedBase
{
  typedef afxXM_WeightedBase Parent;

  afxXM_GroundConformData*  db;
  Container*                container;

public:
  /*C*/         afxXM_GroundConform(afxXM_GroundConformData*, afxEffectWrapper*, bool on_server);

  virtual void  update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, Point3F& pos2, 
                       Point3F& scale);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CO_DATABLOCK_V1(afxXM_GroundConformData);

afxXM_GroundConformData::afxXM_GroundConformData()
{
  height = 0.0f;
  do_terrain = true;
  do_interiors = true;
  do_orientation = false;
}

void afxXM_GroundConformData::initPersistFields()
{
  Parent::initPersistFields();
  addField("height",              TypeF32,      Offset(height, afxXM_GroundConformData));
  addField("conformToTerrain",    TypeBool,     Offset(do_terrain, afxXM_GroundConformData));
  addField("conformToInteriors",  TypeBool,     Offset(do_interiors, afxXM_GroundConformData));
  addField("conformOrientation",  TypeBool,     Offset(do_orientation, afxXM_GroundConformData));
}

void afxXM_GroundConformData::packData(BitStream* stream)
{
  Parent::packData(stream);
  stream->write(height);
  stream->write(do_terrain);
  stream->write(do_interiors);
  stream->write(do_orientation);
}

void afxXM_GroundConformData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
  stream->read(&height);
  stream->read(&do_terrain);
  stream->read(&do_interiors);
  stream->read(&do_orientation);
}

afxXM_Base* afxXM_GroundConformData::create(afxEffectWrapper* fx, bool on_server)
{
  return new afxXM_GroundConform(this, fx, on_server);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_GroundConform::afxXM_GroundConform(afxXM_GroundConformData* db, afxEffectWrapper* fxw, bool on_server) 
: afxXM_WeightedBase(db, fxw) 
{ 
  this->db = db;
  this->container = (on_server) ? &gServerContainer : &gClientContainer;
}

void afxXM_GroundConform::update(F32 dt, F32 elapsed, Point3F& pos, MatrixF& ori, 
                                 Point3F& pos2, Point3F& scale)
{
  RayInfo rInfo;
  bool hit = false;
  
  if (db->do_interiors)
  {
    U32 mask = InteriorObjectType;
    if (db->do_terrain)
    {
      mask |= TerrainObjectType;
#if defined(TGEA_ENGINE)
      mask |= AtlasObjectType;
#endif
    }
    
    Point3F above_pos(pos); above_pos.z += 0.1f;
    Point3F below_pos(pos); below_pos.z -= 10000;
    hit = container->castRay(above_pos, below_pos, mask, &rInfo);
    if (!hit)
    {
      above_pos.z = pos.z + 10000;
      below_pos.z = pos.z - 0.1f;
      hit = gClientContainer.castRay(below_pos, above_pos, mask, &rInfo);
    }
  }
  else if (db->do_terrain)
  {
    U32 mask = TerrainObjectType;
#if defined(TGEA_ENGINE)
    mask |= AtlasObjectType;
#endif
    Point3F above_pos(pos); above_pos.z += 10000;
    Point3F below_pos(pos); below_pos.z -= 10000;
    hit = container->castRay(above_pos, below_pos, mask, &rInfo);
  }
  
  if (hit)
  {
    F32 terrain_z = rInfo.point.z;
    F32 wt_factor = calc_weight_factor(elapsed);
    F32 old_z = pos.z;
    F32 new_z = terrain_z + db->height;
    pos.z = ((1-wt_factor)*old_z) + ((wt_factor)*new_z);
    
    if (db->do_orientation)
    {
      Point3F x,y,z;
      z = rInfo.normal;
      z.normalize();
      ori.getColumn(1,&y);
      mCross(y,z,&x);
      x.normalize();
      mCross(z,x,&y);
      ori.setColumn(0,x);
      ori.setColumn(1,y);
      ori.setColumn(2,z);
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

