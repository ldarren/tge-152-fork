
#ifndef _AFX_STATIC_SHAPE_H_
#define _AFX_STATIC_SHAPE_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/staticShape.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxStaticShapeData

class afxStaticShapeData : public StaticShapeData
{
  typedef StaticShapeData Parent;

public:
  StringTableEntry  sequence;
	bool              ignore_scene_amb;
  bool              use_custom_scene_amb;
	ColorF            custom_scene_amb;

public:
  /*C*/       afxStaticShapeData();

  void        packData(BitStream* stream);
  void        unpackData(BitStream* stream);

  static void initPersistFields();

  DECLARE_CONOBJECT(afxStaticShapeData);
};

DECLARE_CONSOLETYPE(afxStaticShapeData)

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxStaticShape

class afxStaticShape : public StaticShape
{
  typedef StaticShape Parent;

private:
  StaticShapeData*      mDataBlock;
  afxStaticShapeData*   afx_data;
  bool                  is_visible;
  U32                   chor_id;
  bool                  hookup_with_chor;
  StringTableEntry      ghost_cons_name;

protected:
  virtual bool          prepRenderImage(SceneState*, const U32 stateKey, const U32 startZone, 
                                        const bool modifyBaseZoneState);

public:
  /*C*/                 afxStaticShape();
  /*D*/                 ~afxStaticShape();

  void                  init(U32 chor_id, StringTableEntry cons_name);

  virtual bool          onNewDataBlock(GameBaseData* dptr);
  virtual void          advanceTime(F32 dt);
  virtual U32           packUpdate(NetConnection*, U32, BitStream*);
  virtual void          unpackUpdate(NetConnection*, BitStream*);

  const char*           getShapeFileName() const { return mDataBlock->shapeName; }
  void                  setVisibility(bool flag) { is_visible = flag; }

  virtual bool          getLightingAmbientColor(ColorF*);

  DECLARE_CONOBJECT(afxStaticShape);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_STATIC_SHAPE_H_
