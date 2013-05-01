
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "game/gameBase.h"
#ifdef AFX_TGE_1_3
#include "sim/frameAllocator.h"
#else
#include "core/frameAllocator.h"
#endif
#include "terrain/terrRender.h"
#include "interior/interior.h"

#include "afx/ce/afxZodiac.h"

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
  radius = 1;
  start_ang = 0;
  ang_per_sec = 0;
  color.set(1,1,1,1);
  grow_in_time = 0.0f; 
  shrink_out_time = 0.0f;
  growth_rate = 0.0f;
  blend_flags = BLEND_NORMAL;
  terrain_ok = true;
  interiors_ok = true;
  respect_ori_cons = false;
  interior_h_only = false;
  interior_v_ignore = false;
  interior_back_ignore = false;
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

#ifdef AFX_TGE_1_3
  Con::registerType("AfxZodiacDataPtr", TypeAfxZodiacDataPtr, sizeof(afxZodiacData*),
                    REF_GETDATATYPE(afxZodiacData), REF_SETDATATYPE(afxZodiacData));
#endif

  addField("texture",               TypeFilename,   Offset(txr_name,        afxZodiacData));
  addField("radius",                TypeF32,        Offset(radius,          afxZodiacData));
  addField("startAngle",            TypeF32,        Offset(start_ang,       afxZodiacData));
  addField("rotationRate",          TypeF32,        Offset(ang_per_sec,     afxZodiacData));
  addField("growInTime",            TypeF32,        Offset(grow_in_time,    afxZodiacData));
  addField("shrinkOutTime",         TypeF32,        Offset(shrink_out_time, afxZodiacData));
  addField("growthRate",            TypeF32,        Offset(growth_rate,     afxZodiacData));
  addField("color",                 TypeColorF,     Offset(color,           afxZodiacData));
  addField("blend",                 TypeEnum,       Offset(blend_flags,     afxZodiacData), 1, &BlendTable);
  addField("showOnTerrain",         TypeBool,       Offset(terrain_ok,      afxZodiacData));
  addField("showOnInteriors",       TypeBool,       Offset(interiors_ok,    afxZodiacData));
  addField("trackOrientConstraint", TypeBool,       Offset(respect_ori_cons,      afxZodiacData));

  addField("interiorHorizontalOnly",    TypeBool,  Offset(interior_h_only,       afxZodiacData));
  addField("interiorIgnoreVertical",    TypeBool,  Offset(interior_v_ignore,     afxZodiacData));
  addField("interiorIgnoreBackfaces",   TypeBool,  Offset(interior_back_ignore,  afxZodiacData));
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
  if (respect_ori_cons)
    zflags |= RESPECT_ORIENTATION;
  if (interior_h_only)
    zflags |= INTERIOR_HORIZ_ONLY;
  if (interior_v_ignore)
    zflags |= INTERIOR_VERT_IGNORE;
  if (interior_back_ignore)
    zflags |= INTERIOR_BACK_IGNORE;

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
  if (respect_ori_cons)
    zflags |= RESPECT_ORIENTATION;
  if (interior_h_only)
    zflags |= INTERIOR_HORIZ_ONLY;
  if (interior_v_ignore)
    zflags |= INTERIOR_VERT_IGNORE;
  if (interior_back_ignore)
    zflags |= INTERIOR_BACK_IGNORE;
  */

  stream->writeString(txr_name);
  stream->write(radius);
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
  txr = TextureHandle();
  stream->read(&radius);
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
  respect_ori_cons = ((zflags & RESPECT_ORIENTATION) != 0);
  interior_h_only = ((zflags & INTERIOR_HORIZ_ONLY) != 0);
  interior_v_ignore = ((zflags & INTERIOR_VERT_IGNORE) != 0);
  interior_back_ignore = ((zflags & INTERIOR_BACK_IGNORE) != 0);
}

bool afxZodiacData::preload(bool server, char errorBuffer[256])
{
  if (!Parent::preload(server, errorBuffer))
    return false;

  if (!server)
  {
    if (txr_name && txr_name[0] != '\0')
    {
      txr = TextureHandle(txr_name, BitmapTexture, true);
    }
  }

  return true;
}

F32 afxZodiacData::calcRotationAngle(F32 elapsed, F32 rate_factor)
{
  F32 angle = start_ang + elapsed*ang_per_sec*rate_factor;
  angle = mFmod(angle, 360.0f);

  return angle;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// afxZodiac

IMPLEMENT_CO_NETOBJECT_V1(afxZodiac);

afxZodiac::afxZodiac()
{
  mNetFlags.set(Ghostable);
  mTypeMask |= EnvironmentObjectType;
}

afxZodiac::~afxZodiac()
{
}

void afxZodiac::initPersistFields()
{
  Parent::initPersistFields();
}

bool afxZodiac::onAdd()
{
  if(!Parent::onAdd())
    return false;
  
  return true;
}

void afxZodiac::onRemove()
{
  Parent::onRemove();
}

bool afxZodiac::onNewDataBlock(GameBaseData* dptr)
{
  mDataBlock = dynamic_cast<afxZodiacData*>(dptr);
  if (!mDataBlock || !Parent::onNewDataBlock(dptr))
    return false;
  
  scriptOnNewDataBlock();
  return true;
}

void afxZodiac::advanceTime(F32 dt)
{
  Parent::advanceTime(dt);
}

U32 afxZodiac::packUpdate(NetConnection* con, U32 mask, BitStream* stream)
{
  U32 retMask = Parent::packUpdate(con, mask, stream);
  
  return retMask;
}

void afxZodiac::unpackUpdate(NetConnection* con, BitStream* stream)
{
  Parent::unpackUpdate(con, stream);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
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

void afxZodiacMgr::addZodiac(Point3F& pos, F32 radius, ColorF& color, F32 angle, 
                             TextureHandle* txr, U32 flags)
{
  ZodiacSpec z;
  z.pos = pos;
  z.radius = radius;
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
  //Con::printf("afxZodiacMgr::transformTerrainZodiacs()");

  VectorF facing_vec;
  world_xfm.getColumn(1, &facing_vec);
  F32 yaw = mAtan(facing_vec.x, facing_vec.y);
  while (yaw < 0.0) yaw += M_2PI;

  for (S32 i = 0; i < terr_zodes.size(); i++)
  {
    world_xfm.mulP(terr_zodes[i].pos, &terr_zodes[i].loc_pos);
    F32 ang = terr_zodes[i].angle + yaw;
    terr_zodes[i].loc_cos_ang = mCos(ang);
    terr_zodes[i].loc_sin_ang = mSin(ang);
  }

  zode_tris_head = zode_tris_tail = NULL;
}

void afxZodiacMgr::reset()
{
  terr_zodes.clear();
  inter_zodes.clear();
}

void afxZodiacMgr::preallocTerrainTriangles(U32 n_zode_tris)
{
  afxZodiacMgr::n_zode_tris = n_zode_tris;
  zode_tris_idx = 0;

  if (n_zode_tris == 0)
  {
    zode_tris = NULL;
    return;
  }

  zode_tris = (ZodiacTriangle*) FrameAllocator::alloc(sizeof(ZodiacTriangle)*n_zode_tris);
  for (U32 i = 0; i < (n_zode_tris-1); i++)
    zode_tris[i].next = &zode_tris[i+1];

  if (!zode_tris_head)
  {
    zode_tris_head = zode_tris;
    zode_tris_tail = &zode_tris[n_zode_tris-1];
    zode_tris_tail->next = NULL;
  }
  else
  {
    zode_tris_tail->next = zode_tris;
    zode_tris_tail = &zode_tris[n_zode_tris-1];
    zode_tris_tail->next = NULL;
  }
}

void afxZodiacMgr::buildTerrainTriangle(const Point3F& p1, const Point3F& p2, const Point3F& p3, 
                                        U32 zode_idx)
{
  if (zode_tris_idx >= n_zode_tris || zode_tris == NULL)
    return;

  ZodiacSpec* zode = &terr_zodes[zode_idx];
  F32 cos_ang = terr_zodes[zode_idx].loc_cos_ang;
  F32 sin_ang = terr_zodes[zode_idx].loc_sin_ang;

  ZodiacTriangle* tri = &zode_tris[zode_tris_idx++];

  tri->point1 = p1;
  tri->point2 = p2;
  tri->point3 = p3;

  // assign color
  tri->color = zode->color;

  // compute UVs
  Point3F ctr_pt = zode->loc_pos;
  F32 rad = zode->radius;
  //
  F32 u1 = (tri->point1.x - ctr_pt.x)/rad;
  F32 v1 = (tri->point1.y - ctr_pt.y)/rad;
  F32 u2 = (tri->point2.x - ctr_pt.x)/rad;
  F32 v2 = (tri->point2.y - ctr_pt.y)/rad;
  F32 u3 = (tri->point3.x - ctr_pt.x)/rad;
  F32 v3 = (tri->point3.y - ctr_pt.y)/rad;

  // rotate the UVs
  F32 ru1 = u1*cos_ang - v1*sin_ang;
  F32 rv1 = u1*sin_ang + v1*cos_ang;
  F32 ru2 = u2*cos_ang - v2*sin_ang;
  F32 rv2 = u2*sin_ang + v2*cos_ang;
  F32 ru3 = u3*cos_ang - v3*sin_ang;
  F32 rv3 = u3*sin_ang + v3*cos_ang;

  // center the rotated UVs and assign to triangle 
  tri->texco1.set((ru1 + 1.0f)/2.0f, 1.0f - (rv1 + 1.0f)/2.0f);
  tri->texco2.set((ru2 + 1.0f)/2.0f, 1.0f - (rv2 + 1.0f)/2.0f);
  tri->texco3.set((ru3 + 1.0f)/2.0f, 1.0f - (rv3 + 1.0f)/2.0f);

  tri->zflags = zode->zflags;
  tri->txr = zode->txr;
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
      if (dist > afxZodiacMgr::terr_zodes[i].radius)
        zode_bits.unset(i);

      // Z_FACTOR HERE
    }
  }
}

void afxZodiacMgr::renderTerrainTriangles()
{
  if (!zode_tris_head) 
    return;
  
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glEnable(GL_BLEND);
  
  U32 blend = 0;
  TextureHandle* txr = NULL;
  
  for (ZodiacTriangle* walk = zode_tris_head; walk; walk = walk->next)
  {
    // need new glBegin/glEnd sequence when texture or blend changes
    if (txr != walk->txr || blend != (walk->zflags & BLEND_MASK))
    {
      if (walk != zode_tris_head)
        glEnd();

      // set current blend op
      blend = (walk->zflags & BLEND_MASK);
      switch (blend = (walk->zflags & BLEND_MASK))
      {
      case BLEND_ADDITIVE:
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        break;
      case BLEND_SUBTRACTIVE:
        glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        break;
      case BLEND_NORMAL:
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
      }
      
      // set current texture
      txr = walk->txr;
      glBindTexture(GL_TEXTURE_2D, txr->getGLName());
      glBegin(GL_TRIANGLES);
    }

    glColor4ub(walk->color.red, walk->color.green, walk->color.blue, walk->color.alpha);
    glTexCoord2fv(walk->texco1);
    glVertex3fv(walk->point1);
    glTexCoord2fv(walk->texco2);
    glVertex3fv(walk->point2);
    glTexCoord2fv(walk->texco3);
    glVertex3fv(walk->point3);
  }
  glEnd();
  
  glBlendFunc(GL_ONE, GL_ZERO);

  glDisable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glDisable(GL_BLEND);
}

void afxZodiacMgr::renderInteriorZodiacs(const Interior* pInterior,
                                         const MatrixF& mRenderWorldToObj,
                                         const Point3F& mObjScale,
                                         const Box3F& mObjBox,
                                         const Vector<Interior::Surface>& mSurfaces,
                                         const Vector<ItrPaddedPoint>& mPoints,
                                         const Vector<U32>& mWindings)
{
  S32 n_zodes = inter_zodes.size();
  if (n_zodes > 0)
  {
    // compute yaw offset for this object's transform
    VectorF facing_vec; mRenderWorldToObj.getColumn(1, &facing_vec);
    F32 yaw = mAtan(facing_vec.x, facing_vec.y);
    while (yaw < 0.0) yaw += M_2PI;
    
    // set scale pointer if scale != 1
    const Point3F* scale = NULL;
    if (mObjScale.x != 1.0f || mObjScale.y != 1.0f || mObjScale.z != 1.0f)
      scale = &mObjScale;
    
    // loop through the zodiacs
    for (U32 i = 0; i < n_zodes; i++) 
    {
      // skip zodiacs without SHOW_ON_INTERIORS set
      //if ((inter_zodes[i].zflags & afxZodiacData::SHOW_ON_INTERIORS) == 0)
      //  continue;
      
      // transform the zodiac into local space
      Point3F pos;
      mRenderWorldToObj.mulP(inter_zodes[i].pos, &pos);
      if (scale)
        pos.convolveInverse(*scale);
      
      // construct bounding box (Z_FACTOR HERE)
      F32 z_factor = 1.0f;
      F32 radius = inter_zodes[i].radius;
      Box3F box;
      box.min = box.max = pos;
      box.min -= Point3F(radius, radius, z_factor*radius);
      box.max += Point3F(radius, radius, z_factor*radius);
      
      // skip zodiacs not overlapping this object
      if (mObjBox.isOverlapped(box) == false)
        continue;
      
      // Allocate zode-surfaces-list
      // The number of zode surfaces cannot exceed the total number of non-null surfaces in
      // interior...
      U32 zode2_watermark = FrameAllocator::getWaterMark();
      U32* zode_surfs = (U32*) FrameAllocator::alloc(mSurfaces.size()*sizeof(U32));
      U32 n_zode_surfs = 0;
      
      // create zode-surfaces-array
      if (((Interior*)pInterior)->buildLightPolyList(zode_surfs, &n_zode_surfs, box, mRenderWorldToObj, 
        mObjScale) == false) 
      {
        FrameAllocator::setWaterMark(zode2_watermark);
        continue;
      }
      
      // render zodiacs
      render_inter_zode(i, zode_surfs, n_zode_surfs, pos, inter_zodes[i].angle + yaw,
                        pInterior, mSurfaces, mPoints, mWindings);
      
      // free zode-surfaces-array
      FrameAllocator::setWaterMark(zode2_watermark);
    }
  }
}

void afxZodiacMgr::render_inter_zode(U32 zode_idx, U32* zode_surfs, U32 n_zode_surfs, 
                                        const Point3F& pos, F32 ang,
                                        const Interior* inter,
                                        const Vector<Interior::Surface>& mSurfaces,
                                        const Vector<ItrPaddedPoint>& mPoints,
                                        const Vector<U32>& mWindings)
{
  ZodiacSpec* zode = &inter_zodes[zode_idx];
  F32 cos_ang = mCos(ang);
  F32 sin_ang = mSin(ang);

  if (dglDoesSupportARBMultitexture()) 
  {
    glActiveTextureARB(GL_TEXTURE1_ARB);
    glDisable(GL_TEXTURE_2D);
    glActiveTextureARB(GL_TEXTURE0_ARB);
  }
  
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glEnable(GL_BLEND);

  // set the blending style
  U32 blend = (zode->zflags & afxZodiacData::BLEND_MASK);
  if (blend == afxZodiacData::BLEND_ADDITIVE)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);                  // additive
  else if (blend == afxZodiacData::BLEND_SUBTRACTIVE) 
    glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);       // subtractive
  else /*if (blend == afxZodiacData::BLEND_NORMAL)*/  
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // normal

  // set the texture
  glBindTexture(GL_TEXTURE_2D, zode->txr->getGLName());
  
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(-1,-1);

  if (zode->zflags & afxZodiacData::INTERIOR_FILTERS)
  {
    for (U32 i = 0; i < n_zode_surfs; i++) 
    {
      const Interior::Surface& surf = mSurfaces[zode_surfs[i]];
    
      const PlaneF& plane = inter->getPlane(surf.planeIndex);
      if (zode->zflags & afxZodiacData::INTERIOR_HORIZ_ONLY)
      {
        if (!plane.isHorizontal())
          continue;
      }
      else if (zode->zflags & afxZodiacData::INTERIOR_VERT_IGNORE)
      {
        if (plane.isVertical())
          continue;
      }

      if (zode->zflags & afxZodiacData::INTERIOR_BACK_IGNORE)
      {
        if (plane.whichSide(pos) == PlaneF::Back)
          continue;
      }

      // set the color
      glColor4ub(zode->color.red, zode->color.green, zode->color.blue, zode->color.alpha);
    
      glBegin(GL_TRIANGLE_STRIP);
      for (U32 j = surf.windingStart; j < surf.windingStart + surf.windingCount; j++) 
      {
        const Point3F& vtx = mPoints[mWindings[j]].point;

        // compute UV
        F32 u1 = (vtx.x - pos.x)/zode->radius;
        F32 v1 = (vtx.y - pos.y)/zode->radius;
        F32 ru1 = u1*cos_ang - v1*sin_ang;
        F32 rv1 = u1*sin_ang + v1*cos_ang;
    
        glTexCoord2f((ru1 + 1.0f)/2.0f, 1.0f - (rv1 + 1.0f)/2.0f);
        glVertex3fv(mPoints[mWindings[j]].point);
      }
      glEnd();
    }
  }
  else
  {
    for (U32 i = 0; i < n_zode_surfs; i++) 
    {
      const Interior::Surface& surf = mSurfaces[zode_surfs[i]];
    
      // set the color
      glColor4ub(zode->color.red, zode->color.green, zode->color.blue, zode->color.alpha);
    
      glBegin(GL_TRIANGLE_STRIP);
      for (U32 j = surf.windingStart; j < surf.windingStart + surf.windingCount; j++) 
      {
        const Point3F& vtx = mPoints[mWindings[j]].point;

        // compute UV
        F32 u1 = (vtx.x - pos.x)/zode->radius;
        F32 v1 = (vtx.y - pos.y)/zode->radius;
        F32 ru1 = u1*cos_ang - v1*sin_ang;
        F32 rv1 = u1*sin_ang + v1*cos_ang;
    
        glTexCoord2f((ru1 + 1.0f)/2.0f, 1.0f - (rv1 + 1.0f)/2.0f);
        glVertex3fv(mPoints[mWindings[j]].point);
      }
      glEnd();
    }
  }
  
  glDisable(GL_POLYGON_OFFSET_FILL);
  
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ZERO);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
