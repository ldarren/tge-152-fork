
#ifndef _AFX_EMITTER_PARTICLE_H_
#define _AFX_EMITTER_PARTICLE_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/fx/particleEmitter.h"

class afxPathData;
class afxPath3D;

class afxParticleEmitterData : public ParticleEmitterData
{
  typedef ParticleEmitterData Parent;

public:
  // The afx enhanced particle emitter allows fading
  // of particle color, size, velocity, and/or offset.
  // Fading is controlled by a common value which is
  // set externally using setFadeAmount().
  //
  bool        fade_velocity;
  bool        fade_offset;
  Point3F     pe_vector;
  // new -- consider vector in world space?
  bool        pe_vector_is_world;

  // new -- transform paths?
  StringTableEntry      tpaths_string;       // 
  Vector<afxPathData*>  tPathDataBlocks;     // datablocks for paths
  Vector<U32>           tPathDataBlockIds;   // datablock IDs which correspond to the pathDataBlocks

public:
  /*C*/       afxParticleEmitterData();

  void        packData(BitStream* stream);
  void        unpackData(BitStream* stream);
  bool        onAdd();
  bool        preload(bool server, char errorBuffer[256]);

  static void initPersistFields();

  DECLARE_CONOBJECT(afxParticleEmitterData);
};

DECLARE_CONSOLETYPE(afxParticleEmitterData)

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// VECTOR

class afxParticleEmitterVectorData : public afxParticleEmitterData
{
  typedef afxParticleEmitterData Parent;

public:
  /*C*/       afxParticleEmitterVectorData();

  void        packData(BitStream* stream);
  void        unpackData(BitStream* stream);
  bool        onAdd();
  bool        preload(bool server, char errorBuffer[256]);

  static void initPersistFields();

  DECLARE_CONOBJECT(afxParticleEmitterVectorData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// CONE

class afxParticleEmitterConeData : public afxParticleEmitterData
{
  typedef afxParticleEmitterData Parent;

public:
  F32         spread_min;
  F32         spread_max;

public:
  /*C*/       afxParticleEmitterConeData();

  void        packData(BitStream* stream);
  void        unpackData(BitStream* stream);
  bool        onAdd();
  bool        preload(bool server, char errorBuffer[256]);

  static void initPersistFields();

  DECLARE_CONOBJECT(afxParticleEmitterConeData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// PATH

class afxParticleEmitterPathData : public afxParticleEmitterData
{
  typedef afxParticleEmitterData Parent;

public:
  enum PathOriginTypes 
  {
    PATHEMIT_ORIGIN,
    PATHEMIT_POINT,
    PATHEMIT_VECTOR,
    PATHEMIT_TANGENT
  };   
  StringTableEntry      epaths_string;       // 
  Vector<afxPathData*>  epathDataBlocks;     // datablocks for paths
  Vector<U32>           epathDataBlockIds;   // datablock IDs which correspond to the pathDataBlocks
  U32                   path_origin_type;

public:
  /*C*/                 afxParticleEmitterPathData();

  void                  packData(BitStream* stream);
  void                  unpackData(BitStream* stream);
  bool                  onAdd();
  bool                  preload(bool server, char errorBuffer[256]);

  static void           initPersistFields();

  DECLARE_CONOBJECT(afxParticleEmitterPathData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// DISC

class afxParticleEmitterDiscData : public afxParticleEmitterData
{
  typedef afxParticleEmitterData Parent;

public:
  F32                   pe_radius_min;
  F32                   pe_radius_max;

public:
  /*C*/                 afxParticleEmitterDiscData();

  void                  packData(BitStream* stream);
  void                  unpackData(BitStream* stream);
  bool                  onAdd();
  bool                  preload(bool server, char errorBuffer[256]);

  static void           initPersistFields();

  DECLARE_CONOBJECT(afxParticleEmitterDiscData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxParticleEmitter : public ParticleEmitter
{
  typedef ParticleEmitter Parent;

private:
  afxParticleEmitterData* mDataBlock;

protected:
  Point3F       pe_vector, pe_vector_norm;

  // these go with the "pathsTransform" field
  Vector<afxPath3D*> tpaths;
  Vector<F32>        tpath_mults;
  U32                n_tpath_points;
  Point3F**          tpath_points;

  void          init_paths();
  void          cleanup_paths();

  Particle*     alloc_particle();
  ParticleData* pick_particle_type();
  void          afx_emitParticles(const Point3F& point, const bool useLastPosition, const Point3F& velocity, const U32 numMilliseconds);
  void          afx_emitParticles(const Point3F& start, const Point3F& end, const Point3F& velocity, const U32 numMilliseconds);
  void          preCompute(const MatrixF& mat);

  virtual void  sub_particleUpdate(Particle*);
  virtual void  sub_preCompute(const MatrixF& mat)=0;
  virtual void  sub_addParticle(const Point3F& pos, const Point3F& vel, const U32 age_offset, S32 part_idx)=0;

public:
  /*C*/         afxParticleEmitter();
  /*D*/         ~afxParticleEmitter();

  virtual void  emitParticlesExt(const MatrixF& xfm, const Point3F& point, const Point3F& velocity, const U32 numMilliseconds);

  afxParticleEmitterData* getDataBlock(){ return mDataBlock; }
  bool          onNewDataBlock(GameBaseData* dptr);
  bool          onAdd();
  void          onRemove();
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// VECTOR

class afxParticleEmitterVector : public afxParticleEmitter
{
  typedef afxParticleEmitter Parent;

private:
  afxParticleEmitterVectorData* mDataBlock;

public:
  /*C*/       afxParticleEmitterVector();
  /*D*/       ~afxParticleEmitterVector();

  bool        onNewDataBlock(GameBaseData* dptr);

protected:
  void        sub_preCompute(const MatrixF& mat);
  void        sub_addParticle(const Point3F& pos, const Point3F& vel, const U32 age_offse, S32 part_idxt);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// CONE

class afxParticleEmitterCone : public afxParticleEmitter
{
  typedef afxParticleEmitter Parent;

private:
  afxParticleEmitterData* mDataBlock;
  Point3F     cone_v, cone_s0, cone_s1;

public:
  /*C*/       afxParticleEmitterCone();
  /*D*/       ~afxParticleEmitterCone();

  bool        onNewDataBlock(GameBaseData* dptr);

protected:
  void        sub_preCompute(const MatrixF& mat);
  void        sub_addParticle(const Point3F& pos, const Point3F& vel, const U32 age_offset, S32 part_idx);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// PATH

class afxParticleEmitterPath : public afxParticleEmitter
{
  typedef afxParticleEmitter Parent;

private:
  afxParticleEmitterPathData* mDataBlock;

  Vector<afxPath3D*> epaths;
  Vector<F32>        epath_mults;
  U32                n_epath_points;
  Point3F**          epath_points;

  void               init_paths();
  void               cleanup_paths();

public:
  /*C*/       afxParticleEmitterPath();
  /*D*/       ~afxParticleEmitterPath();

  bool        onNewDataBlock(GameBaseData* dptr);

protected:
  bool        onAdd();
  void        onRemove();
  void        sub_preCompute(const MatrixF& mat);
  void        sub_addParticle(const Point3F& pos, const Point3F& vel, const U32 age_offset, S32 part_idx);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// DISC

class afxParticleEmitterDisc : public afxParticleEmitter
{
  typedef afxParticleEmitter Parent;

private:
  afxParticleEmitterDiscData* mDataBlock;
  Point3F     disc_v, disc_r;

public:
  /*C*/       afxParticleEmitterDisc();
  /*D*/       ~afxParticleEmitterDisc();

  bool        onNewDataBlock(GameBaseData* dptr);

protected:
  void        sub_preCompute(const MatrixF& mat);
  void        sub_addParticle(const Point3F& pos, const Point3F& vel, const U32 age_offset, S32 part_idx);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_EMITTER_PARTICLE_H_
