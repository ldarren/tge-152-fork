
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "game/gameBase.h"

#include "afx/ce/afxAnimLock.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxAnimLockData

IMPLEMENT_CONSOLETYPE(afxAnimLockData)
IMPLEMENT_GETDATATYPE(afxAnimLockData)
IMPLEMENT_SETDATATYPE(afxAnimLockData)
IMPLEMENT_CO_DATABLOCK_V1(afxAnimLockData);

afxAnimLockData::afxAnimLockData()
{
}

#define myOffset(field) Offset(field, afxAnimLockData)

void afxAnimLockData::initPersistFields()
{
  Parent::initPersistFields();

#ifdef AFX_TGE_1_3
  Con::registerType("AfxAnimLockDataPtr", TypeAfxAnimLockDataPtr, sizeof(afxAnimLockData*),
                    REF_GETDATATYPE(afxAnimLockData), REF_SETDATATYPE(afxAnimLockData));
#endif
}

bool afxAnimLockData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;

  return true;
}

void afxAnimLockData::packData(BitStream* stream)
{
	Parent::packData(stream);
}

void afxAnimLockData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
