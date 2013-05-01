
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/simBase.h"
#include "game/gameConnection.h"
#include "math/mathIO.h"

#include "afx/afxConstraint.h"
#include "afx/afxChoreographer.h"
#include "afx/afxEffectWrapper.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CONSOLETYPE(afxChoreographerData)
IMPLEMENT_GETDATATYPE(afxChoreographerData)
IMPLEMENT_SETDATATYPE(afxChoreographerData)
IMPLEMENT_CO_DATABLOCK_V1(afxChoreographerData);

afxChoreographerData::afxChoreographerData()
{
  // conditionals
  ranking = 0;
  lod = 0;
  exec_on_new_clients = false;
}

#define myOffset(field) Offset(field, afxChoreographerData)

void afxChoreographerData::initPersistFields()
{
  Parent::initPersistFields();

#ifdef AFX_TGE_1_3
  Con::registerType("AfxChoreographerData", TypeafxChoreographerData, sizeof(afxChoreographerData*),
                    REF_GETDATATYPE(afxChoreographerData), REF_SETDATATYPE(afxChoreographerData));
#endif

  // conditionals
  addField("ranking",           TypeS8,         myOffset(ranking));
  addField("levelOfDetail",     TypeS8,         myOffset(lod));
  addField("execOnNewClients",  TypeBool,       myOffset(exec_on_new_clients));
}

void afxChoreographerData::packData(BitStream* stream)
{
  Parent::packData(stream);

  stream->write(ranking);
  stream->write(lod);
  stream->write(exec_on_new_clients);
}

void afxChoreographerData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  stream->read(&ranking);
  stream->read(&lod);
  stream->read(&exec_on_new_clients);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CO_DATABLOCK_V1(afxChoreographer);

afxChoreographer::afxChoreographer()
{
  datablock = NULL;

  // create the constraint manager
  constraint_mgr = new afxConstraintMgr();

  ranking = 0;
  lod = 0;
  exec_conds_mask = 0;
  choreographer_id = 0;
  extra = NULL;

  dyn_cons_defs = &dc_defs_a;
  dyn_cons_defs2 = &dc_defs_b;
}

afxChoreographer::~afxChoreographer()
{
  for (S32 i = 0; i < dyn_cons_defs->size(); i++)
    if ((*dyn_cons_defs)[i].cons_type == POINT_CONSTRAINT && (*dyn_cons_defs)[i].cons_obj.point != NULL)
      delete (*dyn_cons_defs)[i].cons_obj.point;

  constraint_mgr->clearAllScopeableObjs();

  delete constraint_mgr;
}

void afxChoreographer::initPersistFields()
{
  Parent::initPersistFields();

  // conditionals
  addField("extra",   TypeSimObjectPtr,   Offset(extra, afxChoreographer));
}

bool afxChoreographer::onAdd()
{
  if (!Parent::onAdd()) 
    return(false);

  if (isServerObject())
    choreographer_id = arcaneFX::registerChoreographer(this);

  return(true);
}

void afxChoreographer::onRemove()
{
  if (isServerObject())
    arcaneFX::unregisterChoreographer(this);
  else
    arcaneFX::unregisterClientChoreographer(this);

  Parent::onRemove();
}

void afxChoreographer::onDeleteNotify(SimObject* obj)
{
  if (dynamic_cast<SceneObject*>(obj))
  { 
    SceneObject* scn_obj = (SceneObject*)(obj);
    for (S32 i = 0; i < dyn_cons_defs->size(); i++)
      if ((*dyn_cons_defs)[i].cons_type != OBJECT_CONSTRAINT || (*dyn_cons_defs)[i].cons_obj.object != scn_obj)
        dyn_cons_defs2->push_back((*dyn_cons_defs)[i]);

    Vector<dynConstraintDef>* tmp = dyn_cons_defs;
    dyn_cons_defs = dyn_cons_defs2;
    dyn_cons_defs2 = tmp;
    dyn_cons_defs2->clear();

    if (isServerObject() && scn_obj->isScopeable())
        constraint_mgr->removeScopeableObject(scn_obj);
  }

  Parent::onDeleteNotify(obj);
}

bool afxChoreographer::onNewDataBlock(GameBaseData* dptr)
{
  datablock = dynamic_cast<afxChoreographerData*>(dptr);
  if (!datablock || !Parent::onNewDataBlock(dptr))
    return false;

  return true;
}

void afxChoreographer::pack_constraint_info(NetConnection* conn, BitStream* stream)
{
  stream->write(dyn_cons_defs->size());
  for (S32 i = 0; i < dyn_cons_defs->size(); i++)
  {
    if ((*dyn_cons_defs)[i].cons_type == OBJECT_CONSTRAINT && (*dyn_cons_defs)[i].cons_obj.object != NULL)
    {
      stream->writeString((*dyn_cons_defs)[i].cons_name);
      stream->write((*dyn_cons_defs)[i].cons_type);
      SceneObject* object = (*dyn_cons_defs)[i].cons_obj.object;
      S32 ghost_idx = conn->getGhostIndex(object); 
      if (stream->writeFlag(ghost_idx != -1))
      {
        stream->writeRangedU32(U32(ghost_idx), 0, NetConnection::MaxGhostCount);
      }
      else
      {
        if (stream->writeFlag(object->getScopeId() > 0))
        {
          stream->writeInt(object->getScopeId(), NetObject::SCOPE_ID_BITS);
          stream->writeFlag(dynamic_cast<ShapeBase*>(object) != NULL);
        }
      }
    }
    else if ((*dyn_cons_defs)[i].cons_type == POINT_CONSTRAINT && (*dyn_cons_defs)[i].cons_obj.point != NULL)
    {
      stream->writeString((*dyn_cons_defs)[i].cons_name);
      stream->write((*dyn_cons_defs)[i].cons_type);
      mathWrite(*stream, *(*dyn_cons_defs)[i].cons_obj.point);
    }
  }
      
  constraint_mgr->packConstraintNames(conn, stream);
}

void afxChoreographer::unpack_constraint_info(NetConnection* conn, BitStream* stream)
{
  S32 n_defs;
  stream->read(&n_defs);
  dyn_cons_defs->clear();
  for (S32 i = 0; i < n_defs; i++)
  {
    StringTableEntry cons_name = stream->readSTString();
    U8 cons_type; stream->read(&cons_type);
    if (cons_type == OBJECT_CONSTRAINT)
    {
      SceneObject* scn_obj = NULL;
      if (stream->readFlag())
      {
        S32 ghost_idx = stream->readRangedU32(0, NetConnection::MaxGhostCount);
        scn_obj = dynamic_cast<SceneObject*>(conn->resolveGhost(ghost_idx));
        if (scn_obj)
        {
          addObjectConstraint(scn_obj, cons_name); 
        }
        else
          Con::errorf("CANNOT RESOLVE GHOST %d %s", ghost_idx, cons_name);
      }
      else
      {
        if (stream->readFlag())
        {
          U16 scope_id = stream->readInt(NetObject::SCOPE_ID_BITS);
          bool is_shape = stream->readFlag();
          addObjectConstraint(scope_id, cons_name, is_shape);                                                                   
        }
      }
    }
    else if (cons_type == POINT_CONSTRAINT)
    {
      Point3F point;
      mathRead(*stream, &point);
      addPointConstraint(point, cons_name);
    }    
  }
  
  constraint_mgr->unpackConstraintNames(stream);
}

void afxChoreographer::setup_dynamic_constraints()
{
  for (S32 i = 0; i < dyn_cons_defs->size(); i++)
  {
    switch ((*dyn_cons_defs)[i].cons_type)
    {
    case OBJECT_CONSTRAINT:
      constraint_mgr->setReferenceObject((*dyn_cons_defs)[i].cons_name, (*dyn_cons_defs)[i].cons_obj.object);
      break;
    case POINT_CONSTRAINT:
      constraint_mgr->setReferencePoint((*dyn_cons_defs)[i].cons_name, *(*dyn_cons_defs)[i].cons_obj.point);
      break;
    case OBJECT_CONSTRAINT_SANS_OBJ:
      constraint_mgr->setReferenceObjectByScopeId((*dyn_cons_defs)[i].cons_name, (*dyn_cons_defs)[i].cons_obj.scope_id, false);
      break;
    case OBJECT_CONSTRAINT_SANS_SHAPE:
      constraint_mgr->setReferenceObjectByScopeId((*dyn_cons_defs)[i].cons_name, (*dyn_cons_defs)[i].cons_obj.scope_id, true);
      break;
    }
  }
}

U32 afxChoreographer::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
{
  U32 retMask = Parent::packUpdate(conn, mask, stream);
  
  if (stream->writeFlag(mask & InitialUpdateMask))      //-- INITIAL UPDATE ?
  {
    stream->write(ranking);
    stream->write(lod);
    stream->write(exec_conds_mask);
    stream->write(choreographer_id);
  }

  return retMask;
}

void afxChoreographer::unpackUpdate(NetConnection * conn, BitStream * stream)
{
  Parent::unpackUpdate(conn, stream);
  
  // InitialUpdate Only
  if (stream->readFlag())
  {
    stream->read(&ranking);
    stream->read(&lod);
    stream->read(&exec_conds_mask);
    stream->read(&choreographer_id);

    arcaneFX::registerClientChoreographer(this);
  }
}

void afxChoreographer::executeScriptEvent(const char* method, afxConstraint* cons, 
                                          const Point3F& pos, const char* data)
{
  SceneObject* cons_obj = (cons) ? cons->getObject() : NULL;

  char *posArg = Con::getArgBuffer(64);
  dSprintf(posArg, 64, "%f %f %f", pos.x, pos.y, pos.z);

  // CALL SCRIPT afxChoreographerData::method(%choreographer, %constraint, %pos, %data)
  Con::executef(datablock, 5, method, 
                scriptThis(),
                (cons_obj) ? cons_obj->scriptThis() : "",
                posArg,
                data);
}

void afxChoreographer::addObjectConstraint(SceneObject* object, const char* cons_name)
{
  if (!object || !cons_name)
    return;

  dynConstraintDef dyn_def;
  dyn_def.cons_name = StringTable->insert(cons_name);
  dyn_def.cons_type = OBJECT_CONSTRAINT;
  dyn_def.cons_obj.object = object;
  dyn_cons_defs->push_back(dyn_def);

  if (isServerObject() && object->isScopeable())
    constraint_mgr->addScopeableObject(object);

  constraint_mgr->defineConstraint(OBJECT_CONSTRAINT, dyn_def.cons_name);

  if (object)
    deleteNotify(object);
}

void afxChoreographer::addObjectConstraint(U16 scope_id, const char* cons_name, bool is_shape)
{
  if (!cons_name)
    return;

  dynConstraintDef dyn_def;
  dyn_def.cons_name = StringTable->insert(cons_name);
  dyn_def.cons_type = (is_shape) ? OBJECT_CONSTRAINT_SANS_SHAPE : OBJECT_CONSTRAINT_SANS_OBJ;
  dyn_def.cons_obj.scope_id = scope_id;
  dyn_cons_defs->push_back(dyn_def);

  constraint_mgr->defineConstraint(OBJECT_CONSTRAINT, dyn_def.cons_name);
}

void afxChoreographer::addPointConstraint(Point3F& point, const char* cons_name)
{
  dynConstraintDef dyn_def;
  dyn_def.cons_name = StringTable->insert(cons_name);
  dyn_def.cons_type = POINT_CONSTRAINT;
  dyn_def.cons_obj.point = new Point3F(point);
  dyn_cons_defs->push_back(dyn_def);

  constraint_mgr->defineConstraint(POINT_CONSTRAINT, dyn_def.cons_name);
}

bool afxChoreographer::addConstraint(const char* object_name, const char* cons_name)
{
  VectorF pos(0,0,0);
  if (dSscanf(object_name, "%g %g %g", &pos.x, &pos.y, &pos.z) == 3)
  {
    addPointConstraint(pos, cons_name);
    return true;
  }

  SimObject* cons_sim_obj = Sim::findObject(object_name);

  SceneObject* scn_obj = dynamic_cast<SceneObject*>(cons_sim_obj);
  if (scn_obj)
  {
    addObjectConstraint(scn_obj, cons_name);
    return true;
  }

  return false;
}

void afxChoreographer::addNamedEffect(afxEffectWrapper* ew)
{
  named_effects.addObject(ew);
}

void afxChoreographer::removeNamedEffect(afxEffectWrapper* ew)
{
  named_effects.removeObject(ew);
}

afxEffectWrapper* afxChoreographer::findNamedEffect(StringTableEntry name)
{
  return (afxEffectWrapper*) named_effects.findObject(name);
}

void afxChoreographer::setGhostConstraintObject(SceneObject* obj, StringTableEntry cons_name)
{
  if (constraint_mgr)
    constraint_mgr->setReferenceObject(cons_name, obj);
}

void afxChoreographer::restoreScopedObject(SceneObject* obj)
{
  constraint_mgr->restoreScopedObject(obj, this);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

ConsoleMethod(afxChoreographer, setRanking, void, 3, 3, "(U8 ranking)") 
{
  object->setRanking((U8)dAtoi(argv[2]));
}

ConsoleMethod(afxChoreographer, setLevelOfDetail, void, 3, 3, "(U8 lod)") 
{
  object->setLevelOfDetail((U8)dAtoi(argv[2]));
}

ConsoleMethod(afxChoreographer, setExecConditions, void, 3, 3, "(U32 mask)") 
{
  U32 mask = (U32)dAtoi(argv[2]);
  object->setExecConditions(afxChoreographer::USER_EXEC_CONDS_MASK & mask);
}

ConsoleMethod(afxChoreographer, addConstraint, void, 4, 4, "(string object, string constraint_name)") 
{
  if (!object->addConstraint(argv[2], argv[3]))
    Con::errorf("afxChoreographer::addConstraint() -- failed to find constraint object [%s].", argv[2]);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//