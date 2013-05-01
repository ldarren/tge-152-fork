
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/consoleTypes.h"
#include "core/bitStream.h"

#include "afx/ce/afxScriptEvent.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxScriptEventData

IMPLEMENT_CONSOLETYPE(afxScriptEventData)
IMPLEMENT_SETDATATYPE(afxScriptEventData)
IMPLEMENT_GETDATATYPE(afxScriptEventData)
IMPLEMENT_CO_DATABLOCK_V1(afxScriptEventData);

afxScriptEventData::afxScriptEventData()
{
  method_name = ST_NULLSTRING;
  script_data = ST_NULLSTRING;
}

#define myOffset(field) Offset(field, afxScriptEventData)

void afxScriptEventData::initPersistFields()
{
  Parent::initPersistFields();

  addField("methodName",  TypeString,   myOffset(method_name));
  addField("scriptData",  TypeString,   myOffset(script_data));
}

void afxScriptEventData::packData(BitStream* stream)
{
  Parent::packData(stream);

  stream->writeString(method_name);
  stream->writeString(script_data);
}

void afxScriptEventData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  method_name = stream->readSTString();
  script_data = stream->readSTString();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
