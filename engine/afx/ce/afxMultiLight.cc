
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "lightingSystem/sgLightObject.h"

#include "afx/ce/afxLight.h"
#include "afx/ce/afxMultiLight.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxMultiLightData

IMPLEMENT_CONSOLETYPE(afxMultiLightData)
IMPLEMENT_GETDATATYPE(afxMultiLightData)
IMPLEMENT_SETDATATYPE(afxMultiLightData)
IMPLEMENT_CO_DATABLOCK_V1(afxMultiLightData);

afxMultiLightData::afxMultiLightData()
{
  dMemset(lights, 0, sizeof(lights));
  do_id_convert = false;
}

#define myOffset(field) Offset(field, afxMultiLightData)

void afxMultiLightData::initPersistFields()
{
  Parent::initPersistFields();

  addField("lights",    TypeGameBaseDataPtr,   myOffset(lights),    4);
}

bool afxMultiLightData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;

  return true;
}

void afxMultiLightData::packData(BitStream* stream)
{
	Parent::packData(stream);

  S32 n_lights = 0;
  for (S32 i = 0; i < 4; i++)
  {
    if (lights[i])
      n_lights++;
    else
      break;
  }

  stream->writeInt(n_lights, 3);
  for (int i = 0; i < n_lights; i++)
    writeDatablockID(stream, lights[i], packed);
}

void afxMultiLightData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  do_id_convert = true;
  S32 n_lights = stream->readInt(3);
  for (int i = 0; i < n_lights; i++)
    lights[i] = (GameBaseData*) readDatablockID(stream);
}

bool afxMultiLightData::preload(bool server, char errorBuffer[256])
{
  if (!Parent::preload(server, errorBuffer))
    return false;
  
  // Resolve objects transmitted from server
  if (!server) 
  {
    if (do_id_convert)
    {
      for (int i = 0; i < 4; i++)
      {
        SimObjectId db_id = (SimObjectId)lights[i];
        if (db_id != 0)
        {
          // try to convert id to pointer
          if (!Sim::findObject(db_id, lights[i]))
          {
            Con::errorf("afxMultiLightData::preload() -- bad datablockId: 0x%x (lights[%d])",
              db_id, i);
          }
        }
        do_id_convert = false;
      }
    }
  }
  
  return true;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
