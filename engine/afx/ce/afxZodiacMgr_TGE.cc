
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "core/frameAllocator.h"

#include "afx/ce/afxZodiac.h"
#include "afx/ce/afxZodiacMgr.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

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
  F32 rad = zode->radius_xy;
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
      F32 radius = inter_zodes[i].radius_xy/mObjScale.x;
      Box3F box; box.min = box.max = pos;
      box.min -= Point3F(radius, radius, inter_zodes[i].vert_range.x);
      box.max += Point3F(radius, radius, inter_zodes[i].vert_range.y);
      
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
      render_inter_zode(i, zode_surfs, n_zode_surfs, pos, inter_zodes[i].angle + yaw, mObjScale.x,
                        pInterior, mSurfaces, mPoints, mWindings);
      
      // free zode-surfaces-array
      FrameAllocator::setWaterMark(zode2_watermark);
    }
  }
}

void afxZodiacMgr::render_inter_zode(U32 zode_idx, U32* zode_surfs, U32 n_zode_surfs, 
                                        const Point3F& pos, F32 ang, F32 scale_factor,
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

  glDepthMask(GL_FALSE);

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

#if defined(AFX_CAP_TRANSP_INTERIOR_SURFS)
      F32 alpha_bias = 1.0f;
      if (surf.surfaceFlags & Interior::SurfaceTrans)
      {
        // surface is transparent
        if (zode->zflags & afxZodiacData::INTERIOR_TRANSP_IGNORE)
          continue;
        alpha_bias = 0.3f;
      }
      else 
      {
        // surface is opaque
        if (zode->zflags & afxZodiacData::INTERIOR_OPAQUE_IGNORE)
          continue;
      }
#endif
    
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
#if defined(AFX_CAP_TRANSP_INTERIOR_SURFS)
      glColor4ub(zode->color.red, zode->color.green, zode->color.blue, zode->color.alpha*alpha_bias);
#else
      glColor4ub(zode->color.red, zode->color.green, zode->color.blue, zode->color.alpha);
#endif
    
      glBegin(GL_TRIANGLE_STRIP);
      for (U32 j = surf.windingStart; j < surf.windingStart + surf.windingCount; j++) 
      {
        const Point3F& vtx = mPoints[mWindings[j]].point;

        // compute UV
        F32 inv_radius = scale_factor/zode->radius_xy;
        F32 u1 = (vtx.x - pos.x)*inv_radius;
        F32 v1 = (vtx.y - pos.y)*inv_radius;
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

#if defined(AFX_CAP_TRANSP_INTERIOR_SURFS)
      F32 alpha_bias = (surf.surfaceFlags & Interior::SurfaceTrans) ? 0.3f : 1.0f;
      glColor4ub(zode->color.red, zode->color.green, zode->color.blue, zode->color.alpha*alpha_bias);
#else
      glColor4ub(zode->color.red, zode->color.green, zode->color.blue, zode->color.alpha);
#endif
    
      glBegin(GL_TRIANGLE_STRIP);
      for (U32 j = surf.windingStart; j < surf.windingStart + surf.windingCount; j++) 
      {
        const Point3F& vtx = mPoints[mWindings[j]].point;

        // compute UV
        F32 inv_radius = scale_factor/zode->radius_xy;
        F32 u1 = (vtx.x - pos.x)*inv_radius;
        F32 v1 = (vtx.y - pos.y)*inv_radius;
        F32 ru1 = u1*cos_ang - v1*sin_ang;
        F32 rv1 = u1*sin_ang + v1*cos_ang;
    
        glTexCoord2f((ru1 + 1.0f)/2.0f, 1.0f - (rv1 + 1.0f)/2.0f);
        glVertex3fv(mPoints[mWindings[j]].point);
      }
      glEnd();
    }
  }

  glDepthMask(GL_TRUE);

  glDisable(GL_POLYGON_OFFSET_FILL);
  
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ZERO);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
