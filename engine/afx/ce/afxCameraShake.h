
#ifndef _AFX_CAMERA_SHAKE_H_
#define _AFX_CAMERA_SHAKE_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxCameraShakeData : public GameBaseData
{
  typedef GameBaseData  Parent;

public:
  VectorF               camShakeFreq;
  VectorF               camShakeAmp;
  F32                   camShakeRadius;
  F32                   camShakeFalloff;

public:
  /*C*/                 afxCameraShakeData();

  virtual bool          onAdd();
  virtual void          packData(BitStream*);
  virtual void          unpackData(BitStream*);

  static void           initPersistFields();

  DECLARE_CONOBJECT(afxCameraShakeData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_CAMERA_SHAKE_H_
