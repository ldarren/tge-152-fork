
#ifndef _AFX_MODEL_H_
#define _AFX_MODEL_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/gameBase.h"
#ifdef AFX_TLK_SUPPORT_ENABLED
#include "lightingSystem/sgObjectShadows.h"
#endif AFX_TLK_SUPPORT_ENABLED

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

  Resource<TSShape>     shape;

#ifdef AFX_TLK_SUPPORT_ENABLED
  bool                  receiveSunLight;
  bool                  receiveLMLighting;
  bool                  useAdaptiveSelfIllumination;
  bool                  useCustomAmbientLighting;
  bool                  customAmbientForSelfIllumination;
  ColorF                customAmbientLighting;
  bool                  shadowEnable;
#endif

public:
  /*C*/                 afxModelData();

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
#ifdef AFX_TLK_SUPPORT_ENABLED
  sgObjectShadows       shadows;
#endif

protected:
  virtual void          advanceTime(F32 dt);

  virtual bool          prepRenderImage(SceneState*, const U32 stateKey, const U32 startZone, 
                                        const bool modifyBaseZoneState);
  virtual void          renderObject(SceneState* state, SceneRenderImage* image);

  virtual bool          onAdd();
  virtual void          onRemove();

public:
  /*C*/                 afxModel();
  /*D*/                 ~afxModel();

  virtual bool          onNewDataBlock(GameBaseData* dptr);

  void                  setFadeAmount(F32 amt) { fade_amt = amt; }
  void                  setSequenceRateFactor(F32 factor);

  const char*           getShapeFileName() const { return mDataBlock->shapeName; }
  void                  setVisibility(bool flag) { is_visible = flag; }
  TSShape*              getTSShape() { return mDataBlock->shape; }
  TSShapeInstance*      getTSShapeInstance() { return shape_inst; }

  DECLARE_CONOBJECT(afxModel);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_MODEL_H_
