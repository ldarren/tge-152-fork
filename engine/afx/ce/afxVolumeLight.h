
#ifndef _AFX_VOLUME_LIGHT_H_
#define _AFX_VOLUME_LIGHT_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "lightingSystem/volLight.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxVolumeLightData

class afxVolumeLightData : public sgLightObjectData
{
  typedef sgLightObjectData Parent;

public:
	StringTableEntry  mLTextureName;
	F32               mlpDistance;
	F32               mShootDistance;
	F32               mXextent;
	F32               mYextent;
	U32               mSubdivideU;
	U32               mSubdivideV;
	ColorF            mfootColour;
	ColorF            mtailColour;

public:
  /*C*/             afxVolumeLightData();

  virtual void      packData(BitStream*);
  virtual void      unpackData(BitStream*);

  static void       initPersistFields();

  DECLARE_CONOBJECT(afxVolumeLightData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_VOLUME_LIGHT_H_
