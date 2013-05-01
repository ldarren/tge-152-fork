
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "game/gameBase.h"
#include "game/projectile.h"

#include "afx/ce/afxMachineGun.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxMachineGunData

IMPLEMENT_CONSOLETYPE(afxMachineGunData)
IMPLEMENT_GETDATATYPE(afxMachineGunData)
IMPLEMENT_SETDATATYPE(afxMachineGunData)
IMPLEMENT_CO_DATABLOCK_V1(afxMachineGunData);

afxMachineGunData::afxMachineGunData()
{
  projectile_data = 0;
  rounds_per_minute = 60;
}

#define myOffset(field) Offset(field, afxMachineGunData)

void afxMachineGunData::initPersistFields()
{
  Parent::initPersistFields();

#ifdef AFX_TGE_1_3
  Con::registerType("AfxMachineGunDataPtr", TypeafxMachineGunPtr, sizeof(afxMachineGunData*),
                    REF_GETDATATYPE(afxMachineGunData), REF_SETDATATYPE(afxMachineGunData));
#endif

  addField("projectile",        TypeProjectileDataPtr,  myOffset(projectile_data));
  addField("roundsPerMinute",   TypeS32,                myOffset(rounds_per_minute));
}

bool afxMachineGunData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;

  if (projectile_data)
  { 
    if (getId() >= DataBlockObjectIdFirst && getId() <= DataBlockObjectIdLast)
    {
      SimObjectId pid = projectile_data->getId();
      if (pid < DataBlockObjectIdFirst || pid > DataBlockObjectIdLast)
      {
        Con::errorf(ConsoleLogEntry::General,"afxMachineGunData: bad ProjectileData datablock.");
        return false;
      }
    }
  }

  return true;
}

void afxMachineGunData::packData(BitStream* stream)
{
	Parent::packData(stream);

  if (stream->writeFlag(projectile_data))
    stream->writeRangedU32(projectile_data->getId(), DataBlockObjectIdFirst, DataBlockObjectIdLast);

  stream->write(rounds_per_minute);
}

void afxMachineGunData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  if (stream->readFlag()) 
  {
    SimObjectId id = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
    Sim::findObject(id, projectile_data);
  }
  
  stream->read(&rounds_per_minute);
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
