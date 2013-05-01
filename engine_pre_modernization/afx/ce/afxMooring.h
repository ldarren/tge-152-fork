
#ifndef _AFX_MOORING_H_
#define _AFX_MOORING_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/gameBase.h"
#include "afx/afxEffectDefs.h"

class afxMooringData : public GameBaseData, public afxEffectDefs
{
  typedef GameBaseData  Parent;

public:
  U8            networking;
  bool          track_pos_only;
  bool          display_axis_marker;

public:
  /*C*/         afxMooringData();

  virtual bool  onAdd();
  virtual void  packData(BitStream*);
  virtual void  unpackData(BitStream*);

  static void   initPersistFields();

  DECLARE_CONOBJECT(afxMooringData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxMooring

class afxMooring : public GameBase, public afxEffectDefs
{
  typedef GameBase Parent;

private:
  afxMooringData*       mDataBlock;
  U32                   chor_id;
  bool                  hookup_with_chor;
  StringTableEntry      ghost_cons_name;

protected:
   enum MaskBits 
   {
      PositionMask = Parent::NextFreeMask,
	    NextFreeMask = Parent::NextFreeMask << 1
   };

public:
  /*C*/                 afxMooring();
  /*C*/                 afxMooring(U32 networking, U32 chor_id, StringTableEntry cons_name);
  /*D*/                 ~afxMooring();

  virtual bool          onNewDataBlock(GameBaseData* dptr);
  virtual void          advanceTime(F32 dt);
  virtual bool          onAdd();
  virtual void          onRemove();
  virtual U32           packUpdate(NetConnection*, U32, BitStream*);
  virtual void          unpackUpdate(NetConnection*, BitStream*);
  virtual void          setTransform(const MatrixF&);

  virtual bool          prepRenderImage(SceneState*, const U32 stateKey, const U32 startZone, 
                                        const bool modifyBaseZoneState);
  virtual void          renderObject(SceneState* state, SceneRenderImage* image);

  DECLARE_CONOBJECT(afxMooring);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_MOORING_H_
