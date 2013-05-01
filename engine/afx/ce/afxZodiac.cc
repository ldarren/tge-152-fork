
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "core/frameAllocator.h"
#include "terrain/terrRender.h"
#include "interior/interior.h"

#if defined(TGEA_ENGINE)
#include "interior/interiorInstance.h"
#endif

#if defined(GFX_GRAPHICS_LAYER)
#include "gfx/primBuilder.h"
#endif

#include "afx/ce/afxZodiac.h"
#if defined(TGEA_ENGINE)
#include "afx/afxZodiacRenderer.h"
#endif

#if defined(GFX_GRAPHICS_LAYER)
GFX_ImplementTextureProfile(AFX_GFXZodiacTextureProfile, 
                            GFXTextureProfile::DiffuseMap, 
                            GFXTextureProfile::Static | GFXTextureProfile::NoMipmap | GFXTextureProfile::PreserveSize,  
                            GFXTextureProfile::None);
#endif

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxZodiacData

IMPLEMENT_CONSOLETYPE(afxZodiacData)
IMPLEMENT_GETDATATYPE(afxZodiacData)
IMPLEMENT_SETDATATYPE(afxZodiacData)
IMPLEMENT_CO_DATABLOCK_V1(afxZodiacData);

afxZodiacData::afxZodiacData()
{
  txr_name = ST_NULLSTRING;
  radius_xy = 1;
  vert_range.set(0.0f, 0.0f);
  start_ang = 0;
  ang_per_sec = 0;
  color.set(1,1,1,1);
  grow_in_time = 0.0f; 
  shrink_out_time = 0.0f;
  growth_rate = 0.0f;
  blend_flags = BLEND_NORMAL;
  terrain_ok = true;
  interiors_ok = true;
  reflected_ok = false;
  non_reflected_ok = true;
  respect_ori_cons = false;
  scale_vert_range = true;
  interior_h_only = false;
  interior_v_ignore = false;
  interior_back_ignore = false;
  interior_opaque_ignore = false;
  interior_transp_ignore = true;
}

static EnumTable::Enums BlendEnums[] =
{
   { 0,   "normal"        },
   { 1,   "additive"      },
   { 2,   "subtractive"   },
};
static EnumTable BlendTable(3, BlendEnums);

void afxZodiacData::initPersistFields()
{
  Parent::initPersistFields();

  addField("texture",               TypeFilename,   Offset(txr_name,          afxZodiacData));
  addField("radius",                TypeF32,        Offset(radius_xy,         afxZodiacData));
  addField("verticalRange",         TypePoint2F,    Offset(vert_range,        afxZodiacData));
  addField("scaleVerticalRange",    TypeBool,       Offset(scale_vert_range,  afxZodiacData));
  addField("startAngle",            TypeF32,        Offset(start_ang,         afxZodiacData));
  addField("rotationRate",          TypeF32,        Offset(ang_per_sec,       afxZodiacData));
  addField("growInTime",            TypeF32,        Offset(grow_in_time,      afxZodiacData));
  addField("shrinkOutTime",         TypeF32,        Offset(shrink_out_time,   afxZodiacData));
  addField("growthRate",            TypeF32,        Offset(growth_rate,       afxZodiacData));
  addField("color",                 TypeColorF,     Offset(color,             afxZodiacData));
  addField("blend",                 TypeEnum,       Offset(blend_flags,       afxZodiacData), 1, &BlendTable);
  addField("showOnTerrain",         TypeBool,       Offset(terrain_ok,        afxZodiacData));
  addField("showOnInteriors",       TypeBool,       Offset(interiors_ok,      afxZodiacData));
  addField("showInReflections",     TypeBool,       Offset(reflected_ok,      afxZodiacData));
  addField("showInNonReflections",  TypeBool,       Offset(non_reflected_ok,  afxZodiacData));
  addField("trackOrientConstraint", TypeBool,       Offset(respect_ori_cons,  afxZodiacData));

  addField("interiorHorizontalOnly",    TypeBool,  Offset(interior_h_only,        afxZodiacData));
  addField("interiorIgnoreVertical",    TypeBool,  Offset(interior_v_ignore,      afxZodiacData));
  addField("interiorIgnoreBackfaces",   TypeBool,  Offset(interior_back_ignore,   afxZodiacData));
  addField("interiorIgnoreOpaque",      TypeBool,  Offset(interior_opaque_ignore, afxZodiacData));
  addField("interiorIgnoreTransparent", TypeBool,  Offset(interior_transp_ignore, afxZodiacData));
}

bool afxZodiacData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;

  zflags = (blend_flags & BLEND_MASK);
  if (terrain_ok)
    zflags |= SHOW_ON_TERRAIN;
  if (interiors_ok)
    zflags |= SHOW_ON_INTERIORS;
  if (reflected_ok)
    zflags |= SHOW_IN_REFLECTIONS;
  if (non_reflected_ok)
    zflags |= SHOW_IN_NON_REFLECTIONS;
  if (respect_ori_cons)
    zflags |= RESPECT_ORIENTATION;
  if (scale_vert_range)
    zflags |= SCALE_VERT_RANGE;
  if (interior_h_only)
    zflags |= INTERIOR_HORIZ_ONLY;
  if (interior_v_ignore)
    zflags |= INTERIOR_VERT_IGNORE;
  if (interior_back_ignore)
    zflags |= INTERIOR_BACK_IGNORE;
  if (interior_opaque_ignore)
    zflags |= INTERIOR_OPAQUE_IGNORE;
  if (interior_transp_ignore)
    zflags |= INTERIOR_TRANSP_IGNORE;

  return true;
}

void afxZodiacData::packData(BitStream* stream)
{
	Parent::packData(stream);

  /*
  zflags = (blend_flags & BLEND_MASK);
  if (terrain_ok)
    zflags |= SHOW_ON_TERRAIN;
  if (interiors_ok)
    zflags |= SHOW_ON_INTERIORS;
  if (reflected_ok)
    zflags |= SHOW_IN_REFLECTIONS;
  if (non_reflected_ok)
    zflags |= SHOW_IN_NON_REFLECTIONS;
  if (respect_ori_cons)
    zflags |= RESPECT_ORIENTATION;
  if (scale_vert_range)
    zflags |= SCALE_VERT_RANGE;
  if (interior_h_only)
    zflags |= INTERIOR_HORIZ_ONLY;
  if (interior_v_ignore)
    zflags |= INTERIOR_VERT_IGNORE;
  if (interior_back_ignore)
    zflags |= INTERIOR_BACK_IGNORE;
  if (interior_opaque_ignore)
    zflags |= INTERIOR_OPAQUE_IGNORE;
  if (interior_transp_ignore)
    zflags |= INTERIOR_TRANSP_IGNORE;
  */

  stream->writeString(txr_name);
  stream->write(radius_xy);
  stream->write(vert_range.x);
  stream->write(vert_range.y);
  stream->write(start_ang);
  stream->write(ang_per_sec);
  stream->write(grow_in_time);
  stream->write(shrink_out_time);
  stream->write(growth_rate);
  stream->write(color);
  stream->write(zflags);
}

void afxZodiacData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  txr_name = stream->readSTString();
#if defined(GFX_GRAPHICS_LAYER)
  txr = GFXTexHandle();
#else
  txr = TextureHandle();
#endif
  stream->read(&radius_xy);
  stream->read(&vert_range.x);
  stream->read(&vert_range.y);
  stream->read(&start_ang);
  stream->read(&ang_per_sec);
  stream->read(&grow_in_time);
  stream->read(&shrink_out_time);
  stream->read(&growth_rate);
  stream->read(&color);
  stream->read(&zflags);

  blend_flags = (zflags & BLEND_MASK);
  terrain_ok = ((zflags & SHOW_ON_TERRAIN) != 0);
  interiors_ok = ((zflags & SHOW_ON_INTERIORS) != 0);
  reflected_ok = ((zflags & SHOW_IN_REFLECTIONS) != 0);
  non_reflected_ok = ((zflags & SHOW_IN_NON_REFLECTIONS) != 0);
  respect_ori_cons = ((zflags & RESPECT_ORIENTATION) != 0);
  scale_vert_range = ((zflags & SCALE_VERT_RANGE) != 0);
  interior_h_only = ((zflags & INTERIOR_HORIZ_ONLY) != 0);
  interior_v_ignore = ((zflags & INTERIOR_VERT_IGNORE) != 0);
  interior_back_ignore = ((zflags & INTERIOR_BACK_IGNORE) != 0);
  interior_opaque_ignore = ((zflags & INTERIOR_OPAQUE_IGNORE) != 0);
  interior_transp_ignore = ((zflags & INTERIOR_TRANSP_IGNORE) != 0);
}

bool afxZodiacData::preload(bool server, char errorBuffer[256])
{
  if (!Parent::preload(server, errorBuffer))
    return false;

  if (!server)
  {
    if (txr_name && txr_name[0] != '\0')
    {
#if defined(GFX_GRAPHICS_LAYER)
      txr.set(txr_name, &AFX_GFXZodiacTextureProfile);
#else
      txr = TextureHandle(txr_name, BitmapTexture, true);
#endif
    }
  }

  if (vert_range.x == 0.0f && vert_range.y == 0.0f)
    vert_range.x = vert_range.y = radius_xy;

  return true;
}

F32 afxZodiacData::calcRotationAngle(F32 elapsed, F32 rate_factor)
{
  F32 angle = start_ang + elapsed*ang_per_sec*rate_factor;
  angle = mFmod(angle, 360.0f);

  return angle;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
