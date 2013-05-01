
#ifndef _AFX_ZODIAC_H_
#define _AFX_ZODIAC_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _ARCANE_FX_H_
#include "afx/arcaneFX.h"
#endif
#ifndef _INTERIOR_H_
#include "interior/interior.h"
#endif
#ifndef _AFX_ZODIAC_DEFS_H_
#include "afx/ce/afxZodiacDefs.h"
#endif

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxZodiacData

class afxZodiacData : public GameBaseData, public afxZodiacDefs
{
  typedef GameBaseData  Parent;

public:
  StringTableEntry  txr_name;
#if defined(GFX_GRAPHICS_LAYER)
  GFXTexHandle      txr;
#else
  TextureHandle     txr;
#endif
  F32               radius_xy;
  Point2F           vert_range;
  F32               start_ang;
  F32               ang_per_sec;
  F32               grow_in_time;
  F32               shrink_out_time;
  F32               growth_rate;
  ColorF            color;
  U32               blend_flags;
  bool              terrain_ok;
  bool              interiors_ok;
  bool              reflected_ok;
  bool              non_reflected_ok;
  bool              respect_ori_cons;
  bool              scale_vert_range;
  bool              interior_h_only;
  bool              interior_v_ignore;
  bool              interior_back_ignore;
  bool              interior_opaque_ignore;
  bool              interior_transp_ignore;
  U32               zflags;

public:
  /*C*/             afxZodiacData();

  virtual bool      onAdd();
  virtual void      packData(BitStream*);
  virtual void      unpackData(BitStream*);
  virtual bool      preload(bool server, char errorBuffer[256]);

  F32               calcRotationAngle(F32 elapsed, F32 rate_factor=1.0f);

  static void       initPersistFields();

  DECLARE_CONOBJECT(afxZodiacData);
};

#if defined(GFX_GRAPHICS_LAYER)
GFX_DeclareTextureProfile(AFX_GFXZodiacTextureProfile);
#endif
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_ZODIAC_H_
