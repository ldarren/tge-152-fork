
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"

#include "math/mathUtils.h"

#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/afxResidueMgr.h"
#include "afx/util/afxEase.h"
#include "afx/ce/afxZodiacMgr.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Zodiac 

class afxEA_Zodiac : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxZodiacData*    zode_data;
  Point3F           zode_pos;
  F32               zode_radius;
  Point2F           zode_vrange;
  ColorF            zode_color;
  F32               zode_angle;
  F32               zode_angle_offset;

  F32               live_color_factor;
  ColorF            live_color;

  F32               calc_facing_angle();

public:
  /*C*/             afxEA_Zodiac();

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);

  static void       initPersistFields();

  DECLARE_CONOBJECT(afxEA_Zodiac);
};

IMPLEMENT_CONOBJECT(afxEA_Zodiac);

//~~~~~~~~~~~~~~~~~~~~//

F32 afxEA_Zodiac::calc_facing_angle() 
{
  // get direction player is facing
  VectorF shape_vec;
  MatrixF shape_xfm;

  afxConstraint* orient_constraint = getOrientConstraint();
  if (orient_constraint)
    orient_constraint->getTransform(shape_xfm);
  else
    shape_xfm.identity();

  shape_xfm.getColumn(1, &shape_vec);
  shape_vec.z = 0.0f;
  shape_vec.normalize();

  F32 pitch, yaw;
  MathUtils::getAnglesFromVector(shape_vec, yaw, pitch);

  return mRadToDeg(yaw); 
}

afxEA_Zodiac::afxEA_Zodiac()
{
  zode_data = 0;
  zode_pos.zero();
  zode_radius = 1;
  zode_vrange.set(1,1);
  zode_color.set(1,1,1,1);
  zode_angle = 0;
  zode_angle_offset = 0;
  live_color.set(1,1,1,1);
  live_color_factor = 0.0f;
}

void afxEA_Zodiac::ea_set_datablock(SimDataBlock* db)
{
  zode_data = dynamic_cast<afxZodiacData*>(db);
}

bool afxEA_Zodiac::ea_start()
{
  if (!zode_data)
  {
    Con::errorf("afxEA_Zodiac::ea_start() -- missing or incompatible datablock.");
    return false;
  }

  zode_angle_offset = calc_facing_angle();

  return true;
}

bool afxEA_Zodiac::ea_update(F32 dt)
{
  if (!in_scope)
    return true;

  //~~~~~~~~~~~~~~~~~~~~//
  // Zodiac Color

  zode_color = zode_data->color;

  if (live_color_factor > 0.0)
  {
     zode_color.interpolate(zode_color, live_color, live_color_factor);
     //Con::printf("LIVE-COLOR %g %g %g %g  FACTOR is %g", 
     //   live_color.red, live_color.green, live_color.blue, live_color.alpha, 
     //   live_color_factor);
  }
  else
  {
     //Con::printf("LIVE-COLOR-FACTOR is ZERO");
  }

  if (do_fades)
  {
    if (fade_value < 0.01f)
      return true; // too transparent

    if (zode_data->blend_flags == afxZodiacDefs::BLEND_SUBTRACTIVE)
      zode_color *= fade_value*live_fade_factor;
    else
      zode_color.alpha *= fade_value*live_fade_factor;
  }

  if (zode_color.alpha < 0.01f)
    return true;

  //~~~~~~~~~~~~~~~~~~~~//
  // Zodiac

  // scale and grow zode
  zode_radius = zode_data->radius_xy*updated_scale.x + life_elapsed*zode_data->growth_rate;

  // zode is growing
  if (life_elapsed < zode_data->grow_in_time)
  {
    F32 t = life_elapsed/zode_data->grow_in_time;
    zode_radius = afxEase::eq(t, 0.001f, zode_radius, 0.2f, 0.8f);
  }
  // zode is shrinking
  else if (full_lifetime - life_elapsed < zode_data->shrink_out_time)
  {
    F32 t = (full_lifetime - life_elapsed)/zode_data->shrink_out_time;
    zode_radius = afxEase::eq(t, 0.001f, zode_radius, 0.0f, 0.9f);
  }

  zode_radius *= live_scale_factor;

  if (zode_radius < 0.001f)
    return true; // too small

  zode_vrange = zode_data->vert_range;
  if (zode_data->scale_vert_range)
  {
    F32 scale_factor = zode_radius/zode_data->radius_xy;
    zode_vrange *= scale_factor;
  }

  //~~~~~~~~~~~~~~~~~~~~//
  // Zodiac Position

  zode_pos = updated_pos;

  //~~~~~~~~~~~~~~~~~~~~//
  // Zodiac Rotation 

  if (zode_data->respect_ori_cons)
  {
    afxConstraint* orient_constraint = getOrientConstraint();
    if (orient_constraint)
    {
      VectorF shape_vec;
      updated_xfm.getColumn(1, &shape_vec);
      shape_vec.z = 0.0f;
      shape_vec.normalize();
      F32 pitch, yaw;
      MathUtils::getAnglesFromVector(shape_vec, yaw, pitch);
      zode_angle_offset = mRadToDeg(yaw); 
    }
  }

  zode_angle = zode_data->calcRotationAngle(life_elapsed, datablock->rate_factor/prop_time_factor);
  zode_angle = mFmod(zode_angle + zode_angle_offset, 360.0f);     

  //~~~~~~~~~~~~~~~~~~~~//
  // post zodiac
  afxZodiacMgr::addZodiac(zode_pos, zode_radius, zode_vrange, zode_color, zode_angle, &zode_data->txr, zode_data->zflags);

  return true;
}

void afxEA_Zodiac::ea_finish(bool was_stopped)
{
  if (in_scope && ew_timing.residue_lifetime > 0)
  {
    if (do_fades)
    {
      if (fade_value < 0.01f)
        return;
      zode_color.alpha *= fade_value;
    }
    afxResidueMgr::add(ew_timing.residue_lifetime, ew_timing.residue_fadetime, zode_data,
                       zode_pos, zode_radius, zode_vrange, zode_color, zode_angle);
  }
}

#undef myOffset
#define myOffset(field) Offset(field, afxEA_Zodiac)

void afxEA_Zodiac::initPersistFields()
{
  Parent::initPersistFields();

  addField("liveColor",         TypeColorF,     myOffset(live_color));
  addField("liveColorFactor",   TypeF32,        myOffset(live_color_factor));
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_ZodiacDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_ZodiacDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return true; }

  virtual afxEffectWrapper* create() const { return new afxEA_Zodiac; }
};

afxEA_ZodiacDesc afxEA_ZodiacDesc::desc;

bool afxEA_ZodiacDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(afxZodiacData) == typeid(*db));
}

bool afxEA_ZodiacDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->timing.lifetime < 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//