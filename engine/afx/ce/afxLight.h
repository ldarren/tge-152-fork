
#ifndef _AFX_LIGHT_H_
#define _AFX_LIGHT_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#if (TORQUE_GAME_ENGINE < 1510)
#include "sceneGraph/lightManager.h"
#endif

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxLight Data

struct afxLightData : public GameBaseData
{
  typedef GameBaseData Parent;

  LightInfo             light_info;
  bool                  made_for_LL;

public:
  /*C*/                 afxLightData();

  bool                  preload(bool server, char errorBuffer[256]);

  virtual void          packData(BitStream* stream);
  virtual void          unpackData(BitStream* stream);

  static void           initPersistFields();

  DECLARE_CONOBJECT(afxLightData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxLight

class afxLight : public GameBase
{
  typedef GameBase Parent;

private:
  afxLightData*         mDataBlock;
  LightInfo             light_info;
  F32                   fade_amt;
  bool                  is_visible;
  Point3F               direction0;

#if defined(TGEA_ENGINE)
  MatrixF               dir_matrix;
  void                  calc_dir_matrix();
#endif

protected:
  virtual bool          prepRenderImage(SceneState*, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState);
  virtual void          registerLights(LightManager*, bool lightingScene);
  virtual bool          onAdd();
  virtual void          onRemove();

public:
  /*C*/                 afxLight();
  /*D*/                 ~afxLight();

  virtual bool          onNewDataBlock(GameBaseData* dptr);

  void                  setFadeAmount(F32 amt) { fade_amt = amt; }
  void                  setVisibility(bool flag) { is_visible = flag; }

  DECLARE_CONOBJECT(afxLight);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_LIGHT_H_
