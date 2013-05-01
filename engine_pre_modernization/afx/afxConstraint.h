
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _AFX_CONSTRAINT_H_
#define _AFX_CONSTRAINT_H_

#include "core/tVector.h"
#include "game/shapeBase.h"

#include "afxEffectDefs.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxConstraintDef

struct afxConstraintDef : public afxEffectDefs
{
  enum DefType
  {
    CONS_UNDEFINED,
    CONS_PREDEFINED,
    CONS_SCENE,
    CONS_EFFECT,
    CONS_GHOST
  };

  DefType           def_type;

  StringTableEntry  cons_src_name;
  StringTableEntry  cons_node_name;
  F32               history_time;
  U8                sample_rate;

  bool              runs_on_server;
  bool              runs_on_client;
  bool              pos_at_box_center;

  /*C*/             afxConstraintDef();

  bool              isDefined();

  bool              isArbitraryObject();
  void              reset();
  bool              parseSpec(const char* spec, bool runs_on_server, bool runs_on_client);

  static StringTableEntry  SCENE_CONS_KEY;
  static StringTableEntry  EFFECT_CONS_KEY;
  static StringTableEntry  GHOST_CONS_KEY;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxConstraint
//  Abstract base-class for a simple constraint mechanism used to constrain
//  special effects to spell related objects such as the spellcaster, target,
//  projectile, or impact location.
//
//  note -- the direction vectors don't really fit... should probably consider separate
//    constraint types for position, orientation, and possibly a look-at constraint.
//

class SceneObject;
class afxConstraintMgr;
  
class afxConstraint : public SimObject, public afxEffectDefs
{
  friend class afxConstraintMgr;
  typedef SimObject Parent;

protected:
  afxConstraintMgr* mgr;
  afxConstraintDef  cons_def;
  bool              is_defined;
  bool              is_valid;
  Point3F           last_pos;
  MatrixF           last_xfm;
  F32               history_time;
  bool              is_alive;
  bool              gone_missing;

public:
  /*C*/             afxConstraint(afxConstraintMgr*);
  virtual           ~afxConstraint();

  virtual bool      getPosition(Point3F& pos, F32 hist=0.0f) 
                      { pos = last_pos; return is_valid; }
  virtual bool      getTransform(MatrixF& xfm, F32 hist=0.0f) 
                      { xfm = last_xfm; return is_valid;}

  virtual bool      isDefined() { return is_defined; }
  virtual bool      isValid() { return is_valid; }

  virtual U32       setAnimClip(const char* clip, F32 pos, F32 rate, F32 trans, bool is_death_anim) 
                                  { return 0; };
  virtual void      resetAnimation(U32 tag, bool is_death_anim) { };
  virtual U32       lockAnimation() { return 0; }
  virtual void      unlockAnimation(U32 tag) { }
  virtual F32       getAnimClipDuration(const char* clip) { return 0.0f; }

  virtual S32       getDamageState() { return -1; }
  virtual bool      getTriggerState(U32 stateNum, bool clearState = true) { return false; }

  virtual void      setLivingState(bool state) { is_alive = state; };
  virtual bool      getLivingState() { return is_alive; };

  virtual void      sample(F32 dt, U32 elapsed_ms)=0;

  virtual SceneObject* getObject()=0;
  virtual void      restoreObject(SceneObject*)=0;
  virtual U16       getScopeId()=0;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxConstraintMgr

class ShapeBase;
class afxEffectWrapper;
class afxShapeConstraint;
class afxObjectConstraint;
class BitStream;
class NetConnection;

struct afxConstraintID
{
  S16 index;
  S16 sub_index;

  afxConstraintID() { index = -1; sub_index = 0; }
  afxConstraintID(S16 idx, S16 sub=0) { index = idx; sub_index = sub; }
  bool undefined() const { return (index < 0); }
};
 
typedef Vector<afxConstraint*> afxConstraintList;

class afxConstraintMgr : public afxEffectDefs
{
  typedef SimObject Parent;

  struct preDef
  {
    StringTableEntry  name;
    U32               type;
  };

  Vector<afxConstraintList*>  constraints_v;

  Vector<StringTableEntry>  names_on_server;
  Vector<S32>               ghost_ids;
  Vector<preDef>            predefs;
  U32                       starttime;
  bool                      on_server;
  bool                      initialized;

  SceneObject*        find_object_from_name(StringTableEntry);
  S32                 find_cons_idx_from_name(StringTableEntry);
  S32                 find_effect_cons_idx_from_name(StringTableEntry);

  void                create_constraint(const afxConstraintDef&);    
  void                set_ref_shape(afxConstraintID which_id, ShapeBase*);
  void                set_ref_shape(afxConstraintID which_id, U16 scope_id);

public:
  /*C*/               afxConstraintMgr();
  /*D*/               ~afxConstraintMgr();

  void                defineConstraint(U32 type, StringTableEntry);

  afxConstraintID     setReferencePoint(StringTableEntry which, Point3F point);
  afxConstraintID     setReferencePoint(StringTableEntry which, Point3F point, Point3F vector);
  afxConstraintID     setReferenceObject(StringTableEntry which, SceneObject*);
  afxConstraintID     setReferenceObjectByScopeId(StringTableEntry which, U16 scope_id, bool is_shape);
  afxConstraintID     setReferenceEffect(StringTableEntry which, afxEffectWrapper*);

  void                setReferencePoint(afxConstraintID which_id, Point3F point);
  void                setReferencePoint(afxConstraintID which_id, Point3F point, Point3F vector);
  void                setReferenceObject(afxConstraintID which_id, SceneObject*);
  void                setReferenceObjectByScopeId(afxConstraintID which_id, U16 scope_id, bool is_shape);
  void                setReferenceEffect(afxConstraintID which_id, afxEffectWrapper*);
                      
  afxConstraintID     getConstraintId(const afxConstraintDef&);
  afxConstraint*      getConstraint(afxConstraintID cons_id);

  void                sample(F32 dt, U32 now);

  void                setStartTime(U32 timestamp) { starttime = timestamp; }
  void                initConstraintDefs(Vector<afxConstraintDef>&, bool on_server); 
  void                packConstraintNames(NetConnection* conn, BitStream* stream);
  void                unpackConstraintNames(BitStream* stream);

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// scope-tracking
private:
  Vector<SceneObject*>  scopeable_objs;
  Vector<U16>           scopeable_ids;
  Vector<afxConstraint*>* missing_objs;
  Vector<afxConstraint*>* missing_objs2;
  Vector<afxConstraint*> missing_objs_a;
  Vector<afxConstraint*> missing_objs_b;

public:
  void                  addScopeableObject(SceneObject*);
  void                  removeScopeableObject(SceneObject*);
  void                  clearAllScopeableObjs();

  void                  postMissingConstraintObject(afxConstraint*, bool is_deleting=false);
  void                  restoreScopedObject(SceneObject*, afxChoreographer* ch);
};

inline afxConstraintID afxConstraintMgr::setReferencePoint(StringTableEntry which, Point3F point)
{
  return setReferencePoint(which, point, Point3F(0,0,1));
}

inline void afxConstraintMgr::setReferencePoint(afxConstraintID which, Point3F point) 
{
  setReferencePoint(which, point, Point3F(0,0,1));
}



//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxPointConstraint
//  This constrains to a specific 3D position such as an impact location.
//

class afxPointConstraint : public afxConstraint
{
  typedef afxConstraint  Parent;

protected:
  Point3F           point;
  Point3F           vector;

public:
  /*C*/             afxPointConstraint(afxConstraintMgr*);
  virtual           ~afxPointConstraint();

  virtual void      set(Point3F point, Point3F vector);
  virtual void      sample(F32 dt, U32 elapsed_ms);

  virtual SceneObject* getObject() { return 0; }
  virtual void      restoreObject(SceneObject*) { }
  virtual U16       getScopeId() { return 0; }
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxShapeConstraint
//  This constrains to a hierarchical shape (subclasses of ShapeBase), such as a 
//  Player or a Vehicle. You can also constrain to named sub-nodes of a shape.

class ShapeBase;
class SceneObject;

class afxShapeConstraint : public afxConstraint
{
  friend class afxConstraintMgr;
  typedef afxConstraint  Parent;

protected:
  StringTableEntry  arb_name;
  ShapeBase*        shape;
  U16               scope_id;

public:
  /*C*/             afxShapeConstraint(afxConstraintMgr*);
  /*C*/             afxShapeConstraint(afxConstraintMgr*, StringTableEntry arb_name);
  virtual           ~afxShapeConstraint();

  virtual void      set(ShapeBase* shape);
  virtual void      set_scope_id(U16 scope_id);;
  virtual void      sample(F32 dt, U32 elapsed_ms);

  virtual U32       setAnimClip(const char* clip, F32 pos, F32 rate, F32 trans, bool is_death_anim);
  virtual void      resetAnimation(U32 tag, bool is_death_anim);
  virtual U32       lockAnimation();
  virtual void      unlockAnimation(U32 tag);
  virtual F32       getAnimClipDuration(const char* clip);

  virtual S32       getDamageState();

  virtual bool      getTriggerState(U32 stateNum, bool clearState = true);

  virtual SceneObject* getObject() { return shape; }
  virtual void      restoreObject(SceneObject*);
  virtual U16       getScopeId() { return scope_id; }

  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxShapeNodeConstraint

class afxShapeNodeConstraint : public afxShapeConstraint
{
  friend class afxConstraintMgr;
  typedef afxShapeConstraint  Parent;

protected:
  StringTableEntry  arb_node;
  S32               shape_node_ID;

public:
  /*C*/             afxShapeNodeConstraint(afxConstraintMgr*);
  /*C*/             afxShapeNodeConstraint(afxConstraintMgr*, StringTableEntry arb_name, StringTableEntry arb_node);

  virtual void      set(ShapeBase* shape);
  virtual void      set_scope_id(U16 scope_id);;
  virtual void      sample(F32 dt, U32 elapsed_ms);
  virtual void      restoreObject(SceneObject*);

  S32               getNodeID() const { return shape_node_ID; }

  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxObjectConstraint
//  This constrains to a simple 3D object (subclasses of SceneObject), such as an 
//  afxMagicMissile or a Projectile. You cannot constrain to sub-nodes with an
//  afxObjectConstraint, use afxShapeConstraint instead.

class SceneObject;

class afxObjectConstraint : public afxConstraint
{
  friend class afxConstraintMgr;
  typedef afxConstraint  Parent;

protected:
  StringTableEntry  arb_name;
  SceneObject*      obj;
  U16               scope_id;

public:
                    afxObjectConstraint(afxConstraintMgr*);
                    afxObjectConstraint(afxConstraintMgr*, StringTableEntry arb_name);
  virtual           ~afxObjectConstraint();

  virtual void      set(SceneObject* obj);
  virtual void      set_scope_id(U16 scope_id);;
  virtual void      sample(F32 dt, U32 elapsed_ms);

  virtual SceneObject* getObject() { return obj; }
  virtual void      restoreObject(SceneObject*);
  virtual U16       getScopeId() { return scope_id; }

  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEffectConstraint
//  This constrains to a hierarchical shape (subclasses of ShapeBase), such as a 
//  Player or a Vehicle. You can also constrain to named sub-nodes of a shape.

class afxEffectWrapper;

class afxEffectConstraint : public afxConstraint
{
  friend class afxConstraintMgr;
  typedef afxConstraint  Parent;

protected:
  StringTableEntry  effect_name;
  afxEffectWrapper* effect;

public:
  /*C*/             afxEffectConstraint(afxConstraintMgr*);
  /*C*/             afxEffectConstraint(afxConstraintMgr*, StringTableEntry effect_name);
  virtual           ~afxEffectConstraint();

  virtual bool      getPosition(Point3F& pos, F32 hist=0.0f); 
  virtual bool      getTransform(MatrixF& xfm, F32 hist=0.0f);

  virtual void      set(afxEffectWrapper* effect);
  virtual void      sample(F32 dt, U32 elapsed_ms) { }

  virtual SceneObject* getObject() { return 0; }
  virtual void      restoreObject(SceneObject*) { }
  virtual U16       getScopeId() { return 0; }
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEffectNodeConstraint

class afxEffectNodeConstraint : public afxEffectConstraint
{
  friend class afxConstraintMgr;
  typedef afxEffectConstraint  Parent;

protected:
  StringTableEntry  effect_node;
  S32               effect_node_ID;

public:
  /*C*/             afxEffectNodeConstraint(afxConstraintMgr*);
  /*C*/             afxEffectNodeConstraint(afxConstraintMgr*, StringTableEntry name, StringTableEntry node);

  virtual bool      getPosition(Point3F& pos, F32 hist=0.0f); 
  virtual bool      getTransform(MatrixF& xfm, F32 hist=0.0f);

  virtual void      set(afxEffectWrapper* effect);

  S32               getNodeID() const { return effect_node_ID; }
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxSampleBuffer

class afxSampleBuffer
{
protected:
  U32               buffer_sz;
  U32               buffer_ms;
  U32               ms_per_sample;
  U32               elapsed_ms;
  U32               last_sample_ms;
  U32               next_sample_num;
  U32               n_samples;

  virtual void      recSample(U32 idx, void* data) = 0;
  bool              compute_idx_from_lag(F32 lag, U32& idx);
  bool              compute_idx_from_lag(F32 lag, U32& idx1, U32& idx2, F32& t);

public:
  /*C*/             afxSampleBuffer();
  virtual           ~afxSampleBuffer();

  virtual void      configHistory(F32 hist_len, U8 sample_rate);
  void              recordSample(F32 dt, U32 elapsed_ms, void* data);
  virtual void      getSample(F32 lag, void* data, bool& oob) = 0;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxSampleXfmBuffer

class afxSampleXfmBuffer : public afxSampleBuffer
{
  typedef afxSampleBuffer  Parent;

protected:
  MatrixF*          xfm_buffer;

  virtual void      recSample(U32 idx, void* data);

public:
  /*C*/             afxSampleXfmBuffer();
  virtual           ~afxSampleXfmBuffer();

  virtual void      configHistory(F32 hist_len, U8 sample_rate);
  virtual void      getSample(F32 lag, void* data, bool& oob);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxPointHistConstraint
//  This class extends afxPointConstraint to remember its values for a period of time.

class afxPointHistConstraint : public afxPointConstraint
{
  friend class afxConstraintMgr;
  typedef afxPointConstraint  Parent;

protected:
  afxSampleBuffer*  samples;

public:
  /*C*/             afxPointHistConstraint(afxConstraintMgr*);
  virtual           ~afxPointHistConstraint();

  virtual void      set(Point3F point, Point3F vector);
  virtual void      sample(F32 dt, U32 elapsed_ms);

  virtual bool      getPosition(Point3F& pos, F32 hist=0.0f);
  virtual bool      getTransform(MatrixF& xfm, F32 hist=0.0f);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxShapeHistConstraint
//  This class extends afxShapeConstraint to remember its values for a period of time.

class afxShapeHistConstraint : public afxShapeConstraint
{
  friend class afxConstraintMgr;
  typedef afxShapeConstraint  Parent;

protected:
  afxSampleBuffer*  samples;

public:
  /*C*/             afxShapeHistConstraint(afxConstraintMgr*);
  /*C*/             afxShapeHistConstraint(afxConstraintMgr*, StringTableEntry arb_name);
  virtual           ~afxShapeHistConstraint();

  virtual void      set(ShapeBase* shape);
  virtual void      set_scope_id(U16 scope_id);;
  virtual void      sample(F32 dt, U32 elapsed_ms);

  virtual bool      getPosition(Point3F& pos, F32 hist=0.0f);
  virtual bool      getTransform(MatrixF& xfm, F32 hist=0.0f);

  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxShapeNodeHistConstraint
//  This class extends afxShapeConstraint to remember its values for a period of time.

class afxShapeNodeHistConstraint : public afxShapeNodeConstraint
{
  friend class afxConstraintMgr;
  typedef afxShapeNodeConstraint  Parent;

protected:
  afxSampleBuffer*  samples;

public:
  /*C*/             afxShapeNodeHistConstraint(afxConstraintMgr*);
  /*C*/             afxShapeNodeHistConstraint(afxConstraintMgr*, StringTableEntry arb_name, StringTableEntry arb_node);
  virtual           ~afxShapeNodeHistConstraint();

  virtual void      set(ShapeBase* shape);
  virtual void      set_scope_id(U16 scope_id);;
  virtual void      sample(F32 dt, U32 elapsed_ms);

  virtual bool      getPosition(Point3F& pos, F32 hist=0.0f);
  virtual bool      getTransform(MatrixF& xfm, F32 hist=0.0f);

  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxObjectHistConstraint
//  This class extends afxObjectConstraint to remember its values for a period of time.

class SceneObject;

class afxObjectHistConstraint : public afxObjectConstraint
{
  friend class afxConstraintMgr;
  typedef afxObjectConstraint  Parent;

protected:
  afxSampleBuffer*  samples;

public:
                    afxObjectHistConstraint(afxConstraintMgr*);
                    afxObjectHistConstraint(afxConstraintMgr*, StringTableEntry arb_name);
  virtual           ~afxObjectHistConstraint();

  virtual void      set(SceneObject* obj);
  virtual void      set_scope_id(U16 scope_id);;
  virtual void      sample(F32 dt, U32 elapsed_ms);

  virtual bool      getPosition(Point3F& pos, F32 hist=0.0f);
  virtual bool      getTransform(MatrixF& xfm, F32 hist=0.0f);

  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_CONSTRAINT_H_

