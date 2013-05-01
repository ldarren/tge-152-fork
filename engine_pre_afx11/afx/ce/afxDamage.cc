
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "game/gameBase.h"

#include "afx/ce/afxDamage.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxDamageData

IMPLEMENT_CONSOLETYPE(afxDamageData)
IMPLEMENT_GETDATATYPE(afxDamageData)
IMPLEMENT_SETDATATYPE(afxDamageData)
IMPLEMENT_CO_DATABLOCK_V1(afxDamageData);

afxDamageData::afxDamageData()
{
  label = ST_NULLSTRING;
  flavor = ST_NULLSTRING;
  amount = 0;
  repeats = 1;
  ad_amount = 0;
  radius = 0;
  impulse = 0;
}

#define myOffset(field) Offset(field, afxDamageData)

void afxDamageData::initPersistFields()
{
  Parent::initPersistFields();

#ifdef AFX_TGE_1_3
  Con::registerType("AfxDamageDataPtr", TypeAfxDamageDataPtr, sizeof(afxDamageData*),
                    REF_GETDATATYPE(afxDamageData), REF_SETDATATYPE(afxDamageData));
#endif

  addField("label",               TypeString,     myOffset(label));
  addField("flavor",              TypeString,     myOffset(flavor));
  addField("directDamage",        TypeF32,        myOffset(amount));
  addField("directDamageRepeats", TypeS8,         myOffset(repeats));
  addField("areaDamage",          TypeF32,        myOffset(ad_amount));
  addField("areaDamageRadius",    TypeF32,        myOffset(radius));
  addField("areaDamageImpulse",   TypeF32,        myOffset(impulse));
}

bool afxDamageData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;

  return true;
}

void afxDamageData::packData(BitStream* stream)
{
	Parent::packData(stream);

  stream->writeString(label);
  stream->writeString(flavor);
  stream->write(amount);
  stream->write(repeats);
  stream->write(ad_amount);
  stream->write(radius);
  stream->write(impulse);
}

void afxDamageData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  label = stream->readSTString();
  flavor = stream->readSTString();
  stream->read(&amount);
  stream->read(&repeats);
  stream->read(&ad_amount);
  stream->read(&radius);
  stream->read(&impulse);
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
