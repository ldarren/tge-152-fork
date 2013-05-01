
#ifndef _AFX_MODEL_H_
#define _AFX_MODEL_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "lightingSystem/sgObjectShadows.h"
#define AFX_MODEL_SHADOWS

class ParticleEmitterData;
class ParticleEmitter;
class ExplosionData;
class TSPartInstance;
class TSShapeInstance;
class TSShape;

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxModel Data

struct afxModelData : public GameBaseData
{
  typedef GameBaseData Parent;

  StringTableEntry      shapeName;
  StringTableEntry      sequence;
  F32                   seq_rate;
  F32                   alpha_mult;
  bool                  use_vertex_alpha;
  U32                   force_on_material_flags;
  U32                   force_off_material_flags;
  F32                   seq_offset;
  bool                  texture_filtering;
  F32                   fog_mult;
#if defined(TGEA_ENGINE)
  struct TextureTagRemapping
  {
     char* old_tag;
     char* new_tag;
  };
  char*                 remap_buffer;
  Vector<TextureTagRemapping> txr_tag_remappings;
#endif
  StringTableEntry      remap_txr_tags;

  Resource<TSShape>     shape;

  bool                  overrideLightingOptions;
  bool                  receiveSunLight;
  bool                  receiveLMLighting;
  bool                  useAdaptiveSelfIllumination;
  bool                  useCustomAmbientLighting;
  bool                  customAmbientForSelfIllumination;
  ColorF                customAmbientLighting;
#if defined(AFX_MODEL_SHADOWS)
  bool                  shadowEnable;
#endif

public:
  /*C*/                 afxModelData();
  /*D*/                 ~afxModelData();

  bool                  preload(bool server, char errorBuffer[256]);
  void                  packData(BitStream* stream);
  void                  unpackData(BitStream* stream);

  static void           initPersistFields();

  DECLARE_CONOBJECT(afxModelData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxModel

class afxModel : public GameBase
{
  typedef GameBase Parent;

private:
  afxModelData*         mDataBlock;
  TSShapeInstance*      shape_inst;
  TSThread*             seq_thread;
  bool                  seq_animates_vis;
  F32                   fade_amt;
  F32                   seq_rate_factor;
  bool                  is_visible;
  S8                    sort_priority;
#if defined(AFX_MODEL_SHADOWS)
  sgObjectShadows       shadows;
#endif

protected:
  Vector<S32>           mCollisionDetails;
  Vector<S32>           mLOSDetails;
  bool                  castRay(const Point3F &start, const Point3F &end, RayInfo* info);

  virtual void          advanceTime(F32 dt);

  virtual bool          prepRenderImage(SceneState*, const U32 stateKey, const U32 startZone, 
                                        const bool modifyBaseZoneState);
#if defined(TGEA_ENGINE)
  void                  renderObject(SceneState* state);
  virtual void          renderObject(SceneState*, RenderInst *ri);
#else
  virtual void          renderObject(SceneState* state, SceneRenderImage* image);
#endif

  virtual bool          onAdd();
  virtual void          onRemove();

public:
  /*C*/                 afxModel();
  /*D*/                 ~afxModel();

  virtual bool          onNewDataBlock(GameBaseData* dptr);

  void                  setFadeAmount(F32 amt) { fade_amt = amt; }
  void                  setSequenceRateFactor(F32 factor);
  void                  setSortPriority(S8 priority) { sort_priority = priority; }

  const char*           getShapeFileName() const { return mDataBlock->shapeName; }
  void                  setVisibility(bool flag) { is_visible = flag; }
  TSShape*              getTSShape() { return mDataBlock->shape; }
  TSShapeInstance*      getTSShapeInstance() { return shape_inst; }

  DECLARE_CONOBJECT(afxModel);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_MODEL_H_
