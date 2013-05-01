
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "game/gameBase.h"

#include "afx/ce/afxCameraShake.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxCameraShakeData

IMPLEMENT_CONSOLETYPE(afxCameraShakeData)
IMPLEMENT_GETDATATYPE(afxCameraShakeData)
IMPLEMENT_SETDATATYPE(afxCameraShakeData)
IMPLEMENT_CO_DATABLOCK_V1(afxCameraShakeData);

afxCameraShakeData::afxCameraShakeData()
{
  camShakeFreq.set( 10.0, 10.0, 10.0 );
  camShakeAmp.set( 1.0, 1.0, 1.0 );
  camShakeRadius = 10.0;
  camShakeFalloff = 10.0;
}

#define myOffset(field) Offset(field, afxCameraShakeData)

void afxCameraShakeData::initPersistFields()
{
  Parent::initPersistFields();

#ifdef AFX_TGE_1_3
  Con::registerType("AfxCameraShakeDataPtr", TypeAfxCameraShakeDataPtr, sizeof(afxCameraShakeData*),
                    REF_GETDATATYPE(afxCameraShakeData), REF_SETDATATYPE(afxCameraShakeData));
#endif

  addField("frequency", TypePoint3F,   Offset(camShakeFreq,       afxCameraShakeData));
  addField("amplitude", TypePoint3F,   Offset(camShakeAmp,        afxCameraShakeData));
  addField("radius",    TypeF32,       Offset(camShakeRadius,     afxCameraShakeData));
  addField("falloff",   TypeF32,       Offset(camShakeFalloff,    afxCameraShakeData));
}

bool afxCameraShakeData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;

  return true;
}

void afxCameraShakeData::packData(BitStream* stream)
{
	Parent::packData(stream);

  stream->write(camShakeFreq.x);
  stream->write(camShakeFreq.y);
  stream->write(camShakeFreq.z);
  stream->write(camShakeAmp.x);
  stream->write(camShakeAmp.y);
  stream->write(camShakeAmp.z);
  stream->write(camShakeRadius);
  stream->write(camShakeFalloff);
}

void afxCameraShakeData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  stream->read(&camShakeFreq.x);
  stream->read(&camShakeFreq.y);
  stream->read(&camShakeFreq.z);
  stream->read(&camShakeAmp.x);
  stream->read(&camShakeAmp.y);
  stream->read(&camShakeAmp.z);
  stream->read(&camShakeRadius);
  stream->read(&camShakeFalloff);
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
