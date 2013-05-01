
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "ts/tsShapeInstance.h"

#include "afx/afxChoreographer.h"
#include "afx/ce/afxStaticShape.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxStaticShapeData

IMPLEMENT_CO_DATABLOCK_V1(afxStaticShapeData);

afxStaticShapeData::afxStaticShapeData()
{
  sequence = ST_NULLSTRING;
  ignore_scene_amb = false; 
  use_custom_scene_amb = false;
  custom_scene_amb.set(0.5f, 0.5f, 0.5f);
}

#define myOffset(field) Offset(field, afxStaticShapeData)

void afxStaticShapeData::initPersistFields()
{
  Parent::initPersistFields();

  addField("sequence",              TypeFilename, myOffset(sequence));
  addField("ignoreSceneAmbient",    TypeBool,     myOffset(ignore_scene_amb));
  addField("useCustomSceneAmbient", TypeBool,     myOffset(use_custom_scene_amb));
  addField("customSceneAmbient",    TypeColorF,   myOffset(custom_scene_amb));
}

IMPLEMENT_CONSOLETYPE(afxStaticShapeData)
IMPLEMENT_GETDATATYPE(afxStaticShapeData)
IMPLEMENT_SETDATATYPE(afxStaticShapeData)

void afxStaticShapeData::packData(BitStream* stream)
{
  Parent::packData(stream);

  stream->writeString(sequence);
  stream->writeFlag(ignore_scene_amb);
  if (stream->writeFlag(use_custom_scene_amb))
    stream->write(custom_scene_amb);
}

void afxStaticShapeData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  sequence = stream->readSTString();
  ignore_scene_amb = stream->readFlag();
  if (use_custom_scene_amb = stream->readFlag())
    stream->read(&custom_scene_amb);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxStaticShape

IMPLEMENT_CO_NETOBJECT_V1(afxStaticShape);

afxStaticShape::afxStaticShape()
{
  afx_data = 0;
  is_visible = true;
  chor_id = 0;
  hookup_with_chor = false;
  ghost_cons_name = ST_NULLSTRING;
}

afxStaticShape::~afxStaticShape()
{
}

void afxStaticShape::init(U32 chor_id, StringTableEntry cons_name)
{
  this->chor_id = chor_id;
  ghost_cons_name = cons_name;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

bool afxStaticShape::onNewDataBlock(GameBaseData* dptr)
{
  mDataBlock = dynamic_cast<StaticShapeData*>(dptr);
  if (!mDataBlock || !Parent::onNewDataBlock(dptr))
    return false;

  afx_data = dynamic_cast<afxStaticShapeData*>(mDataBlock);

  if (!mShapeInstance)
    return true;

  const char* seq_name = 0;

  // if datablock is afxStaticShapeData we get the sequence setting 
  // directly from the datablock on the client-side only
  if (afx_data)
  {
    if (isClientObject())
      seq_name = afx_data->sequence;
  }
  // otherwise datablock is stock StaticShapeData and we look for
  // a sequence name on a dynamic field on the server.
  else
  {
    if (isServerObject())
      seq_name = mDataBlock->getDataField(StringTable->insert("sequence"),0);
  } 

  // if we have a sequence name, attempt to start a thread
  if (seq_name)
  {
    TSShape* shape = mShapeInstance->getShape();
    if (shape) 
    {
      S32 seq = shape->findSequence(seq_name);
      if (seq != -1)
        setThreadSequence(0,seq);
    }
  }

  return true;
}

bool afxStaticShape::getLightingAmbientColor(ColorF* col)
{
  if (!afx_data)
  {
    mLightingInfo.mDirty = false;
    return Parent::getLightingAmbientColor(col);
  }

  if (afx_data->ignore_scene_amb)
    return false;

  if (afx_data->use_custom_scene_amb)
  {
    mLightingInfo.mDirty = false;
    *col = afx_data->custom_scene_amb;
    return true;
  }

  return Parent::getLightingAmbientColor(col);
}

void afxStaticShape::advanceTime(F32 dt)
{
  Parent::advanceTime(dt);

  if (hookup_with_chor)
  {
    afxChoreographer* chor = arcaneFX::findClientChoreographer(chor_id);
    if (chor)
    {
      chor->setGhostConstraintObject(this, ghost_cons_name);
      hookup_with_chor = false;
    }
  }
}

U32 afxStaticShape::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
{
  U32 retMask = Parent::packUpdate(conn, mask, stream);

  // InitialUpdate
  if (stream->writeFlag(mask & InitialUpdateMask)) 
  {
    stream->write(chor_id);
    stream->writeString(ghost_cons_name);
  }

  return retMask;
}

//~~~~~~~~~~~~~~~~~~~~//

void afxStaticShape::unpackUpdate(NetConnection * conn, BitStream * stream)
{
  Parent::unpackUpdate(conn, stream);
  
  // InitialUpdate
  if (stream->readFlag())
  {
    stream->read(&chor_id);
    ghost_cons_name = stream->readSTString();

    if (chor_id != 0 && ghost_cons_name != ST_NULLSTRING)
      hookup_with_chor = true;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

bool afxStaticShape::prepRenderImage(SceneState* state, const U32 stateKey, const U32 startZone, 
                                     const bool modifyBaseState)
{
  if (!is_visible)
    return false;

  return Parent::prepRenderImage(state, stateKey, startZone, modifyBaseState);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//