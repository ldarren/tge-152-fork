
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/consoleTypes.h"
#include "core/bitStream.h"

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
  pos_offset = 0.0;
  trans = 0.12f;
  flags = 0;

  ignore_disabled = false;
  ignore_enabled = false;
  is_death_anim = false;
  lock_anim = false;
  ignore_first_person = false;
  ignore_third_person = false;
}

#define myOffset(field) Offset(field, afxAnimClipData)

void afxAnimClipData::initPersistFields()
{
  Parent::initPersistFields();

  addField("clipName",          TypeString,     myOffset(clip_name));
  addField("rate",              TypeF32,        myOffset(rate));
  addField("posOffset",         TypeF32,        myOffset(pos_offset));
  addField("transitionTime",    TypeF32,        myOffset(trans));
  //
  addField("ignoreDisabled",    TypeBool,       myOffset(ignore_disabled));
  addField("ignoreEnabled",     TypeBool,       myOffset(ignore_enabled));
  addField("treatAsDeathAnim",  TypeBool,       myOffset(is_death_anim));
  addField("lockAnimation",     TypeBool,       myOffset(lock_anim));
  addField("ignoreFirstPerson", TypeBool,       myOffset(ignore_first_person));
  addField("ignoreThirdPerson", TypeBool,       myOffset(ignore_third_person));

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
              ((ignore_first_person) ? IGNORE_FIRST_PERSON : 0) |
              ((ignore_third_person) ? IGNORE_THIRD_PERSON : 0) |
              ((is_death_anim) ? IS_DEATH_ANIM : 0));

  stream->writeString(clip_name);
  stream->write(rate);
  stream->write(pos_offset);
  stream->write(trans);
  stream->write(xflags | flags);
}

void afxAnimClipData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  clip_name = stream->readSTString();
  stream->read(&rate);
  stream->read(&pos_offset);
  stream->read(&trans);
  stream->read(&flags);

  ignore_disabled = ((flags & IGNORE_DISABLED) != 0);
  ignore_enabled = ((flags & IGNORE_ENABLED) != 0);
  lock_anim = ((flags & BLOCK_USER_CONTROL) != 0);
  is_death_anim = ((flags & IS_DEATH_ANIM) != 0);
  ignore_first_person = ((flags & IGNORE_FIRST_PERSON) != 0);
  ignore_third_person = ((flags & IGNORE_THIRD_PERSON) != 0);
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
