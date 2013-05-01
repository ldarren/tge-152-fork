
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "terrain/terrRender.h"

#include "afx/ce/afxZodiac.h"
#include "afx/ce/afxZodiacMgr.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxZodiacMgr

static bool used = false;

Vector<afxZodiacMgr::ZodiacSpec>  afxZodiacMgr::terr_zodes;
Vector<afxZodiacMgr::ZodiacSpec>  afxZodiacMgr::inter_zodes;

afxZodiacMgr::ZodiacTriangle*   afxZodiacMgr::zode_tris_head = NULL;
afxZodiacMgr::ZodiacTriangle*   afxZodiacMgr::zode_tris_tail = NULL;
afxZodiacMgr::ZodiacTriangle*   afxZodiacMgr::zode_tris = NULL;
U32                             afxZodiacMgr::zode_tris_idx = 0;
U32                             afxZodiacMgr::n_zode_tris = 0;

#if defined(TGEA_ENGINE)
afxZodiacMgr::ZodiacSpec*       afxZodiacMgr::live_zodiac = 0;
ShaderData*                     afxZodiacMgr::terrain_zode_shader = 0;
ShaderData*                     afxZodiacMgr::atlas_zode_shader = 0;
ShaderData*                     afxZodiacMgr::interior_zode_shader = 0;
#endif    
    
#if defined(GFX_GRAPHICS_LAYER)
void afxZodiacMgr::addZodiac(Point3F& pos, F32 radius, Point2F& vert_range, ColorF& color, F32 angle, 
                             GFXTexHandle* txr, U32 flags)
#else
void afxZodiacMgr::addZodiac(Point3F& pos, F32 radius, Point2F& vert_range, ColorF& color, F32 angle, 
                             TextureHandle* txr, U32 flags)
#endif
{
  ZodiacSpec z;
  z.pos = pos;
  z.radius_xy = radius;
  z.vert_range = vert_range;
  z.color = color;
  z.angle = mDegToRad(angle);
  z.zflags = flags;
  z.txr = txr;

  if ((flags & SHOW_ON_TERRAIN) != 0)
  {
    if (terr_zodes.size() < MAX_ZODIACS)
      terr_zodes.push_back(z);
  }

  if ((flags & SHOW_ON_INTERIORS) != 0)
  {
    if (inter_zodes.size() < MAX_ZODIACS)
      inter_zodes.push_back(z);
  }
}

void afxZodiacMgr::transformTerrainZodiacs(const MatrixF& world_xfm)
{
  VectorF facing_vec;
  world_xfm.getColumn(1, &facing_vec);
  F32 yaw = mAtan(facing_vec.x, facing_vec.y);
  while (yaw < 0.0) yaw += M_2PI_F;

  for (S32 i = 0; i < terr_zodes.size(); i++)
  {
    world_xfm.mulP(terr_zodes[i].pos, &terr_zodes[i].loc_pos);
    F32 ang = terr_zodes[i].angle + yaw;
    terr_zodes[i].loc_cos_ang = mCos(ang);
    terr_zodes[i].loc_sin_ang = mSin(ang);
  }

  zode_tris_head = zode_tris_tail = NULL;
}

void afxZodiacMgr::frameReset()
{
  terr_zodes.clear();
  inter_zodes.clear();
}

void afxZodiacMgr::missionCleanup()
{
#if defined(TGEA_ENGINE)
  terrain_zode_shader = 0;
  atlas_zode_shader = 0;
  interior_zode_shader = 0;
#endif
}

void afxZodiacMgr::testTerrainOverlap(GridSquare* sq, S32 level, Point2I sq_pos, 
                                      afxZodiacBitmask& zode_bits)
{
  F32 block_x = sq_pos.x*TerrainRender::mSquareSize + TerrainRender::mBlockPos.x;
  F32 block_y = sq_pos.y*TerrainRender::mSquareSize + TerrainRender::mBlockPos.y;
  //F32 block_z = fixedToFloat(sq->minHeight);

  F32 block_sz = TerrainRender::mSquareSize*(1 << level);
  //F32 block_ht = fixedToFloat(sq->maxHeight - sq->minHeight);

  S32 lastBit = zode_bits.findLastSetBit(terr_zodes.size());
  for (S32 i = 0; i <= lastBit; i++)
  {
    if (zode_bits.test(i))
    {
      Point3F* pos = &afxZodiacMgr::terr_zodes[i].loc_pos;

      Point2F vec;

      if (pos->x < block_x)
        vec.x = block_x - pos->x;
      else if (pos->x > block_x + block_sz)
        vec.x = pos->x - (block_x + block_sz);
      else
        vec.x = 0;

      if (pos->y < block_y)
        vec.y = block_y - pos->y;
      else if (pos->y > block_y + block_sz)
        vec.y = pos->y - (block_y + block_sz);
      else
        vec.y = 0;

      F32 dist = vec.len();
      if (dist > afxZodiacMgr::terr_zodes[i].radius_xy)
        zode_bits.unset(i);
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
