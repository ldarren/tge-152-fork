
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "game/gameBase.h"

#include "afx/ce/afxAnimClip.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxAnimClipData

IMPLEMENT_CONSOLETYPE(afxAnimClipData)
IMPLEMENT_GETDATATYPE(afxAnimClipData)
IMPLEMENT_SETDATATYPE(afxAnimClipData)
IMPLEMENT_CO_DATABLOCK_V1(afxAnimClipData);

afxAnimClipData::afxAnimClipData()
{
  clip_name = ST_NULLSTRING;
  rate = 1.0f;
  trans = 0.12f;
  flags = 0;

  ignore_disabled = false;
  ignore_enabled = false;
  is_death_anim = false;
  lock_anim = false;
}

#define myOffset(field) Offset(field, afxAnimClipData)

void afxAnimClipData::initPersistFields()
{
  Parent::initPersistFields();

#ifdef AFX_TGE_1_3
  Con::registerType("AfxAnimClipDataPtr", TypeAfxAnimClipDataPtr, sizeof(afxAnimClipData*),
                    REF_GETDATATYPE(afxAnimClipData), REF_SETDATATYPE(afxAnimClipData));
#endif

  addField("clipName",          TypeString,     myOffset(clip_name));
  addField("rate",              TypeF32,        myOffset(rate));
  addField("transitionTime",    TypeF32,        myOffset(trans));
  //
  addField("ignoreDisabled",    TypeBool,       myOffset(ignore_disabled));
  addField("ignoreEnabled",     TypeBool,       myOffset(ignore_enabled));
  addField("treatAsDeathAnim",  TypeBool,       myOffset(is_death_anim));
  addField("lockAnimation",     TypeBool,       myOffset(lock_anim));

  // synonyms
  addField("ignoreCorpse",      TypeBool,       myOffset(ignore_disabled));
  addField("ignoreLiving",      TypeBool,       myOffset(ignore_enabled));
}

bool afxAnimClipData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;

  return true;
}

void afxAnimClipData::packData(BitStream* stream)
{
	Parent::packData(stream);

  U8 xflags = (((ignore_disabled) ? IGNORE_DISABLED : 0) | 
              ((ignore_enabled) ? IGNORE_ENABLED : 0) | 
              ((lock_anim) ? BLOCK_USER_CONTROL : 0) | 
              ((is_death_anim) ? IS_DEATH_ANIM : 0));

  stream->writeString(clip_name);
  stream->write(rate);
  stream->write(trans);
  stream->write(xflags | flags);
}

void afxAnimClipData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  clip_name = stream->readSTString();
  stream->read(&rate);
  stream->read(&trans);
  stream->read(&flags);

  ignore_disabled = ((flags & IGNORE_DISABLED) != 0);
  ignore_enabled = ((flags & IGNORE_ENABLED) != 0);
  lock_anim = ((flags & BLOCK_USER_CONTROL) != 0);
  is_death_anim = ((flags & IS_DEATH_ANIM) != 0);
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
