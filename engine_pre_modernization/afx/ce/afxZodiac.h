
#ifndef _AFX_ZODIAC_H_
#define _AFX_ZODIAC_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/gameBase.h"
#include "interior/interior.h"
#include "afx/ce/afxZodiacDefs.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxZodiacData

class afxZodiacData : public GameBaseData, public afxZodiacDefs
{
  typedef GameBaseData  Parent;

public:
  StringTableEntry  txr_name;
  TextureHandle     txr;
  F32               radius;
  F32               start_ang;
  F32               ang_per_sec;
  F32               grow_in_time;
  F32               shrink_out_time;
  F32               growth_rate;
  ColorF            color;
  U32               blend_flags;
  bool              terrain_ok;
  bool              interiors_ok;
  bool              respect_ori_cons;
  bool              interior_h_only;
  bool              interior_v_ignore;
  bool              interior_back_ignore;
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

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxZodiac

class afxZodiac : public GameBase, public afxZodiacDefs
{
  typedef GameBase Parent;
  
private:
  afxZodiacData*  mDataBlock;
  
protected:
  bool            onAdd();
  void            onRemove();
  bool            onNewDataBlock(GameBaseData*);
     
public:
  /*C*/           afxZodiac();
  /*D*/           ~afxZodiac();
  
public:
  void            advanceTime(F32 dt);
  
  static void     initPersistFields();
  
  U32             packUpdate(NetConnection*, U32 mask, BitStream*);
  void            unpackUpdate(NetConnection*, BitStream*);

  DECLARE_CONOBJECT(afxZodiac);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

struct GridSquare;

class afxZodiacMgr : public afxZodiacDefs
{
private:
  struct ZodiacSpec
  {
     Point3F        pos;          //12// world position
     F32            radius;       // 4// radius of zodiac
     ColorI         color;        // 4// color of zodiac
     F32            angle;        // 4// angle in radians
     U32            zflags;       // 4//    0=normal,1=additive,2=subtractive
     TextureHandle* txr;          // 4// zodiac texture

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
     TextureHandle*   txr;
     ZodiacTriangle*  next;
  };

  //static ZodiacSpec       zodiacs[MAX_ZODIACS];
  static Vector<ZodiacSpec> terr_zodes;
  static Vector<ZodiacSpec> inter_zodes;

  static ZodiacTriangle*  zode_tris_head;
  static ZodiacTriangle*  zode_tris_tail;
  static ZodiacTriangle*  zode_tris;
  static U32              zode_tris_idx;
  static U32              n_zode_tris;

  static void render_inter_zode(U32 zode_idx, U32* zode_surfs, U32 n_zode_surfs, 
                                const Point3F& pos, F32 ang, 
                                const Interior* pInterior,
                                const Vector<Interior::Surface>& inter_surfaces,
                                const Vector<ItrPaddedPoint>& inter_points,
                                const Vector<U32>& inter_windings);
public:
  static void   addZodiac(Point3F& pos, F32 radius, ColorF& color, F32 angle, 
                              TextureHandle* txr, U32 flags);
  static void   reset();

                // terrain zodiac calls
  static S32    numTerrainZodiacs() { return terr_zodes.size(); }
  static void   transformTerrainZodiacs(const MatrixF& world_xfm);
  static void   testTerrainOverlap(GridSquare*, S32 level, Point2I sq_pos, 
                                   afxZodiacBitmask&);
  static void   preallocTerrainTriangles(U32 n_zode_tris);
  static void   buildTerrainTriangle(const Point3F& p1, const Point3F& p2, const Point3F& p3, 
                                     U32 zode_idx);
  static void   renderTerrainTriangles();

                // interior zodiac calls
  static S32    numInteriorZodiacs() { return inter_zodes.size(); }
  static void   renderInteriorZodiacs(const Interior* pInterior,
                                      const MatrixF& mRenderWorldToObj,
                                      const Point3F& mObjScale,
                                      const Box3F& mObjBox,
                                      const Vector<Interior::Surface>& inter_surfaces,
                                      const Vector<ItrPaddedPoint>& inter_points,
                                      const Vector<U32>& inter_windings);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_ZODIAC_H_
