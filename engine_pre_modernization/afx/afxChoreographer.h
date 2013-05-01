
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _AFX_CHOREOGRAPHER_H_
#define _AFX_CHOREOGRAPHER_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "game/gameBase.h"

#include "afxEffectDefs.h"
#include "afxMagicMissile.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxChoreographerData

class afxChoreographerData : public GameBaseData, public afxEffectDefs
{
  typedef GameBaseData  Parent;

public:
  U8            ranking;
  U8            lod;
  bool          exec_on_new_clients;

public:
  /*C*/         afxChoreographerData();

  virtual void  packData(BitStream*);
  virtual void  unpackData(BitStream*);

  static void   initPersistFields();

  DECLARE_CONOBJECT(afxChoreographerData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxChoreographer

class afxConstraint;
class afxConstraintMgr;
class afxEffectWrapper;
class SimSet;

class afxChoreographer : public GameBase, public afxEffectDefs, public afxMagicMissileCallback
{
  typedef GameBase  Parent;

public:
  enum 
  { 
    USER_EXEC_CONDS_MASK = 0x00ffffff
  };

protected:
  struct dynConstraintDef
  {
    StringTableEntry  cons_name;
    U8                cons_type;
    union
    {
      SceneObject* object;
      Point3F*     point;
      U16          scope_id;
    } cons_obj;
  };

private:
  afxChoreographerData* datablock;
  SimSet                named_effects;
  Vector<dynConstraintDef> dc_defs_a;
  Vector<dynConstraintDef> dc_defs_b;

protected:
  Vector<dynConstraintDef>* dyn_cons_defs;
  Vector<dynConstraintDef>* dyn_cons_defs2;
  afxConstraintMgr* constraint_mgr;
  U32               choreographer_id;
  U8                ranking;
  U8                lod;
  U32               exec_conds_mask;
  SimObject*        extra;

  virtual void      pack_constraint_info(NetConnection* conn, BitStream* stream);
  virtual void      unpack_constraint_info(NetConnection* conn, BitStream* stream);
  void              setup_dynamic_constraints();

public:
  /*C*/             afxChoreographer();
  virtual           ~afxChoreographer();

  static void       initPersistFields();

  virtual bool      onAdd();
  virtual void      onRemove();
  virtual void      onDeleteNotify(SimObject*);
  virtual bool      onNewDataBlock(GameBaseData* dptr);
  virtual U32       packUpdate(NetConnection*, U32, BitStream*);
  virtual void      unpackUpdate(NetConnection*, BitStream*);

  virtual void      sync_with_clients() { }
  
  afxConstraintMgr* getConstraintMgr() { return constraint_mgr; }

  void              setRanking(U8 value) { ranking = value; }
  U8                getRanking() const { return ranking; }
  bool              testRanking(U8 low, U8 high) { return (ranking <= high && ranking >= low); }
  void              setLevelOfDetail(U8 value) { lod = value; }
  U8                getLevelOfDetail() const { return lod; }
  bool              testLevelOfDetail(U8 low, U8 high) { return (lod <= high && lod >= low); }
  void              setExecConditions(U32 mask) { exec_conds_mask = mask; }
  U32               getExecConditions() const { return exec_conds_mask; }

  virtual void      executeScriptEvent(const char* method, afxConstraint*, 
                                       const Point3F& pos, const char* data);

  virtual void      inflictDamage(const char * label, const char* flavor, SimObjectId target,
                                  F32 amt, U8 count, F32 ad_amt, F32 rad, Point3F pos, F32 imp) { }

  void              addObjectConstraint(SceneObject*, const char* cons_name);
  void              addObjectConstraint(U16 scope_id, const char* cons_name, bool is_shape);
  void              addPointConstraint(Point3F&, const char* cons_name);
  bool              addConstraint(const char* object_name, const char* cons_name);

  void              addNamedEffect(afxEffectWrapper*);
  void              removeNamedEffect(afxEffectWrapper*);
  afxEffectWrapper* findNamedEffect(StringTableEntry);

  U32               getChoreographerId() { return choreographer_id; }
  void              setGhostConstraintObject(SceneObject*, StringTableEntry cons_name);
  void              setExtra(SimObject* extra) { this->extra = extra; }

  void              restoreScopedObject(SceneObject* obj);
  virtual void      restoreObject(SceneObject*) { };

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// missile watcher callbacks
public:
  virtual void  impactNotify(const Point3F& p, const Point3F& n, SceneObject*) { }

  DECLARE_CONOBJECT(afxChoreographer);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_CHOREOGRAPHER_H_
