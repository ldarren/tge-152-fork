
#ifndef _AFX_ZODIAC_MGR_H_
#define _AFX_ZODIAC_MGR_H_

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
#ifndef _AFX_ZODIAC_H_
#include "afx/ce/afxZodiac.h"
#endif

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

struct GridSquare;

#if defined(TGEA_ENGINE)
class ShaderData;
#endif

class afxZodiacMgr : public afxZodiacDefs
{
#if defined(TGEA_ENGINE)
  friend class afxZodiacRenderer;
  friend class AtlasClipMapBatcher;
  friend struct TerrainRender;
#endif

private:
  struct ZodiacSpec
  {
     Point3F        pos;          //12// world position
     F32            radius_xy;    // 4// radius of zodiac
     Point2F        vert_range;   // 8// vertical range
     ColorI         color;        // 4// color of zodiac
     F32            angle;        // 4// angle in radians
     U32            zflags;       // 4// 0=normal,1=additive,2=subtractive
#if defined(GFX_GRAPHICS_LAYER)
     GFXTexHandle* txr;           // 4// zodiac texture
#else
     TextureHandle* txr;          // 4// zodiac texture
#endif

     Point3F        loc_pos;      //12// transformed to local position
     F32            loc_cos_ang;  // 4// cosine of local rotation angle
     F32            loc_sin_ang;  // 4// sine of local rotation angle
  };

  struct ZodiacTriangle 
  {
     ColorI           color;
     Point2F          texco1;
     Point3F          point1;
     Point2F          texco2;
     Point3F          point2;
     Point2F          texco3;
     Point3F          point3;
     U32              zflags; // 0=normal,1=additive,2=subtractive
#if defined(GFX_GRAPHICS_LAYER)
     GFXTexHandle*   txr;
#else
     TextureHandle*   txr;
#endif
     ZodiacTriangle*  next;
  };

  static Vector<ZodiacSpec> terr_zodes;
  static Vector<ZodiacSpec> inter_zodes;

  static ZodiacTriangle*  zode_tris_head;
  static ZodiacTriangle*  zode_tris_tail;
  static ZodiacTriangle*  zode_tris;
  static U32              zode_tris_idx;
  static U32              n_zode_tris;

#if defined(TGEA_ENGINE)
public:
  static ZodiacSpec*      live_zodiac;
private:
#endif

#if defined(TGEA_ENGINE)
  static ShaderData*      terrain_zode_shader;
  static ShaderData*      atlas_zode_shader;
  static ShaderData*      interior_zode_shader;
#else
  static void render_inter_zode(U32 zode_idx, U32* zode_surfs, U32 n_zode_surfs, 
                                const Point3F& pos, F32 ang, F32 scale_factor,
                                const Interior* pInterior,
                                const Vector<Interior::Surface>& inter_surfaces,
                                const Vector<ItrPaddedPoint>& inter_points,
                                const Vector<U32>& inter_windings);
#endif
public:
#if defined(GFX_GRAPHICS_LAYER)
  static void   addZodiac(Point3F& pos, F32 radius, Point2F& vert_range, ColorF& color, F32 angle, 
                              GFXTexHandle* txr, U32 flags);
#else
  static void   addZodiac(Point3F& pos, F32 radius, Point2F& vert_range, ColorF& color, F32 angle, 
                          TextureHandle* txr, U32 flags);
#endif
  static void   frameReset();
  static void   missionCleanup();

  static S32    numTerrainZodiacs() { return terr_zodes.size(); }
  static S32    numInteriorZodiacs() { return inter_zodes.size(); }

  static void   transformTerrainZodiacs(const MatrixF& world_xfm);
  static void   testTerrainOverlap(GridSquare*, S32 level, Point2I sq_pos, afxZodiacBitmask&);

#if defined(TGEA_ENGINE)
  static void   setupZodiacTerrainRendering(U32 zode_idx, SceneGraphData&);
  static void   clearZodiacTerrainRendering();
  static void   renderInteriorZodiacs(const Interior*, const InteriorInstance*);
  static ShaderData* getTerrainZodiacShader();
  static ShaderData* getAtlasZodiacShader();
  static ShaderData* getInteriorZodiacShader();
#else
  static void   preallocTerrainTriangles(U32 n_zode_tris);
  static void   buildTerrainTriangle(const Point3F& p1, const Point3F& p2, const Point3F& p3, U32 zode_idx);
  static void   renderTerrainTriangles();
  static void   renderInteriorZodiacs(const Interior*, const MatrixF& mRenderWorldToObj, const Point3F& mObjScale,
                                      const Box3F& mObjBox, const Vector<Interior::Surface>& inter_surfaces,
                                      const Vector<ItrPaddedPoint>& inter_points, const Vector<U32>& inter_windings);
#endif
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_ZODIAC_MGR_H_
