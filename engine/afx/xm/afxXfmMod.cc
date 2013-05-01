
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "afx/afxEffectWrapper.h"
#include "afx/xm/afxXfmMod.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// BASE CLASSES
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CONSOLETYPE(afxXM_BaseData)
IMPLEMENT_SETDATATYPE(afxXM_BaseData)
IMPLEMENT_GETDATATYPE(afxXM_BaseData)
IMPLEMENT_CO_DATABLOCK_V1(afxXM_BaseData);

afxXM_BaseData::afxXM_BaseData()
{
  ignore_time_factor = false;
}

void afxXM_BaseData::initPersistFields()
{
  Parent::initPersistFields();

  addField("ignoreTimeFactor",  TypeBool,   Offset(ignore_time_factor, afxXM_BaseData));

  Con::setIntVariable("$afxXfmMod::POS",            POSITION);
  Con::setIntVariable("$afxXfmMod::ORI",            ORIENTATION);
  Con::setIntVariable("$afxXfmMod::POS2",           POSITION2);
  Con::setIntVariable("$afxXfmMod::SCALE",          SCALE);
  Con::setIntVariable("$afxXfmMod::ALL_BUT_SCALE",  ALL_BUT_SCALE);
  Con::setIntVariable("$afxXfmMod::ALL",            ALL);
}

void afxXM_BaseData::packData(BitStream* stream)
{
  Parent::packData(stream);

  stream->write(ignore_time_factor);
}

void afxXM_BaseData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  stream->read(&ignore_time_factor);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_Base::afxXM_Base(afxXM_BaseData* db, afxEffectWrapper* fxw)
{
  fx_wrapper = fxw;
  time_factor = (db->ignore_time_factor) ? 1.0f : fxw->getTimeFactor();
  datablock = db;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CO_DATABLOCK_V1(afxXM_WeightedBaseData);

afxXM_WeightedBaseData::afxXM_WeightedBaseData()
{
  delay         = 0;
  lifetime      = afxEffectDefs::INFINITE_LIFETIME;
  fade_in_time  = 0;
  fade_out_time = 0;
}

bool afxXM_WeightedBaseData::hasFixedWeight() const
{
  return (delay == 0 && lifetime == afxEffectDefs::INFINITE_LIFETIME && fade_in_time == 0 &&
          fade_out_time == 0);
}

F32 afxXM_WeightedBaseData::getWeightFactor() const
{
  return 1.0f;
}

void afxXM_WeightedBaseData::initPersistFields()
{
  Parent::initPersistFields();
  addField("delay",         TypeF32,      Offset(delay,         afxXM_WeightedBaseData));
  addField("lifetime",      TypeF32,      Offset(lifetime,      afxXM_WeightedBaseData));
  addField("fadeInTime",    TypeF32,      Offset(fade_in_time,  afxXM_WeightedBaseData));
  addField("fadeOutTime",   TypeF32,      Offset(fade_out_time, afxXM_WeightedBaseData));
}

void afxXM_WeightedBaseData::packData(BitStream* stream)
{
  Parent::packData(stream);
  stream->write(delay);
  stream->write(lifetime);
  stream->write(fade_in_time);
  stream->write(fade_out_time);
}

void afxXM_WeightedBaseData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
  stream->read(&delay);
  stream->read(&lifetime);
  stream->read(&fade_in_time);
  stream->read(&fade_out_time);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxXM_WeightedBase::afxXM_WeightedBase(afxXM_WeightedBaseData* db, afxEffectWrapper* fxw)
: afxXM_Base(db, fxw) 
{
  wt_fadein = db->fade_in_time;
  wt_fadeout = db->fade_out_time;
  wt_start_time = db->delay;
  wt_full_time = wt_start_time + wt_fadein;
  wt_fade_time = wt_start_time + db->lifetime;
  wt_done_time = wt_fade_time + wt_fadeout;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//


