
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "arcaneFX.h"

#include "sim/sceneObject.h"
#include "game/shapeBase.h"
#include "game/player.h"
#include "game/aiPlayer.h"
#include "game/game.h"
#include "console/simBase.h"
#include "sim/netConnection.h"
#include "ts/tsShape.h"
#include "ts/tsShapeInstance.h"
#include "math/mathUtils.h"
#include "core/tVector.h"

#include "afxConstraint.h"
#include "afxChoreographer.h"
#include "afxEffectWrapper.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxConstraintDef

// static
StringTableEntry  afxConstraintDef::SCENE_CONS_KEY;
StringTableEntry  afxConstraintDef::EFFECT_CONS_KEY;
StringTableEntry  afxConstraintDef::GHOST_CONS_KEY;

afxConstraintDef::afxConstraintDef()
{
  if (SCENE_CONS_KEY == 0)
  {
    SCENE_CONS_KEY = StringTable->insert("#scene");
    EFFECT_CONS_KEY = StringTable->insert("#effect");
    GHOST_CONS_KEY = StringTable->insert("#ghost");
  }

  reset();
}

bool afxConstraintDef::isDefined() 
{
  return (def_type != CONS_UNDEFINED); 
}

bool afxConstraintDef::isArbitraryObject() 
{ 
  return ((cons_src_name != ST_NULLSTRING) && (def_type == CONS_SCENE)); 
}

void afxConstraintDef::reset()
{
  cons_src_name = ST_NULLSTRING;
  cons_node_name = ST_NULLSTRING;
  def_type = CONS_UNDEFINED;
  history_time = 0;
  sample_rate = 30;
  runs_on_server = false;
  runs_on_client = false;
  pos_at_box_center = false;
}

bool afxConstraintDef::parseSpec(const char* spec, bool runs_on_server, 
                                 bool runs_on_client)
{
  reset();

  if (spec == 0 || spec[0] == '\0')
    return false;

  history_time = 0.0f;
  sample_rate = 30;

  this->runs_on_server = runs_on_server;
  this->runs_on_client = runs_on_client;

  // spec should be in one of these forms:
  //    CONSTRAINT_NAME (only)
  //    CONSTRAINT_NAME.NODE (shapeBase objects only)
  //    CONSTRAINT_NAME.#center
  //    object.OBJECT_NAME
  //    object.OBJECT_NAME.NODE (shapeBase objects only)
  //    object.OBJECT_NAME.#center
  //    effect.EFFECT_NAME
  //    effect.EFFECT_NAME.NODE
  //    effect.EFFECT_NAME.#center
  //    #ghost.EFFECT_NAME
  //    #ghost.EFFECT_NAME.NODE
  //    #ghost.EFFECT_NAME.#center
  //

  // create scratch buffer by duplicating spec.
  char special = '\b';
  char* buffer = dStrdup(spec);

  // substitute a dots not inside parens with special character
  S32 n_nested = 0;
  for (char* b = buffer; (*b) != '\0'; b++)
  {
    if ((*b) == '(')
      n_nested++;
    else if ((*b) == ')')
      n_nested--;
    else if ((*b) == '.' && n_nested == 0)
      (*b) = special;
  }

  // divide name into '.' separated tokens (up to 8)
  char* words[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  char* dot = buffer;
  int wdx = 0;
  while (wdx < 8)
  {
    words[wdx] = dot;
    dot = dStrchr(words[wdx++], special);
    if (!dot)
      break;
    *(dot++) = '\0';
    if ((*dot) == '\0')
      break;
  }

  int n_words = wdx;

  // at this point the spec has been split into words. 
  // n_words indicates how many words we have.

  // no words found (must have been all whitespace)
  if (n_words < 1)
  {
    dFree(buffer);
    return false;
  }

  char* hist_spec = 0;
  char* words2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int n_words2 = 0;

  // move words to words2 while extracting #center and #history
  for (S32 i = 0; i < n_words; i++)
  {
    if (dStrcmp(words[i], "#center") == 0)
      pos_at_box_center = true;
    else if (dStrncmp(words[i], "#history(", 9) == 0)
      hist_spec = words[i];
    else
      words2[n_words2++] = words[i];
  }

  // words2[] now contains just the constraint part

  // no words found (must have been all #center and #history)
  if (n_words2 < 1)
  {
    dFree(buffer);
    return false;
  }

  if (hist_spec)
  {
    char* open_paren = dStrchr(hist_spec, '(');
    if (open_paren)
    {
      hist_spec = open_paren+1;
      if ((*hist_spec) != '\0')
      {
        char* close_paren = dStrchr(hist_spec, ')');
        if (close_paren)
          (*close_paren) = '\0';
        char* slash = dStrchr(hist_spec, '/');
        if (slash)
          (*slash) = ' ';

        F32 hist_age = 0.0;
        U32 hist_rate = 30;
        S32 args = dSscanf(hist_spec,"%g %d", &hist_age, &hist_rate);

        if (args > 0)
          history_time = hist_age;
        if (args > 1)
          sample_rate = hist_rate;
      }
    }
  }

  StringTableEntry cons_name_key = StringTable->insert(words2[0]);

  // must be in CONSTRAINT_NAME (only) form
  if (n_words2 == 1)
  {
    // arbitrary object/effect constraints must have a name
    if (cons_name_key == SCENE_CONS_KEY || cons_name_key == EFFECT_CONS_KEY)
    {
      dFree(buffer);
      return false;
    }

    cons_src_name = cons_name_key;
    def_type = CONS_PREDEFINED;
    dFree(buffer);
    return true;
  }

  // "#scene.NAME" or "#scene.NAME.NODE""
  if (cons_name_key == SCENE_CONS_KEY)
  {
    cons_src_name = StringTable->insert(words2[1]);
    if (n_words2 > 2)
      cons_node_name = StringTable->insert(words2[2]);
    def_type = CONS_SCENE;
    dFree(buffer);
    return true;
  }

  // "#effect.NAME" or "#effect.NAME.NODE"
  if (cons_name_key == EFFECT_CONS_KEY)
  {
    cons_src_name = StringTable->insert(words2[1]);
    if (n_words2 > 2)
      cons_node_name = StringTable->insert(words2[2]);
    def_type = CONS_EFFECT;
    dFree(buffer);
    return true;
  }

  // "#ghost.NAME" or "#ghost.NAME.NODE"
  if (cons_name_key == GHOST_CONS_KEY)
  {
    if (runs_on_server)
    {
      dFree(buffer);
      return false;
    }

    cons_src_name = StringTable->insert(words2[1]);
    if (n_words2 > 2)
      cons_node_name = StringTable->insert(words2[2]);
    def_type = CONS_GHOST;
    dFree(buffer);
    return true;
  }

  // "CONSTRAINT_NAME.NODE"
  if (n_words2 == 2)
  {
    cons_src_name = cons_name_key;
    cons_node_name = StringTable->insert(words2[1]);
    def_type = CONS_PREDEFINED;
    dFree(buffer);
    return true;
  }

  // must be in unsupported form
  dFree(buffer); 
  return false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxConstraint

afxConstraint::afxConstraint(afxConstraintMgr* mgr)
{
  this->mgr = mgr;
  is_defined = false;
  is_valid = false;
  last_pos.zero();
  last_xfm.identity();
  history_time = 0.0f;
  is_alive = true;
  gone_missing = false;
}

afxConstraint::~afxConstraint()
{
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

inline afxPointConstraint* newPointCons(afxConstraintMgr* mgr, bool hist)
{
  return (hist) ? new afxPointHistConstraint(mgr) : new afxPointConstraint(mgr);
}

inline afxShapeConstraint* newShapeCons(afxConstraintMgr* mgr, bool hist)
{
  return (hist) ? new afxShapeHistConstraint(mgr) : new afxShapeConstraint(mgr);
}

inline afxShapeConstraint* newShapeCons(afxConstraintMgr* mgr, StringTableEntry name, bool hist)
{
  return (hist) ? new afxShapeHistConstraint(mgr, name) : new afxShapeConstraint(mgr, name);
}

inline afxShapeNodeConstraint* newShapeNodeCons(afxConstraintMgr* mgr, StringTableEntry name, StringTableEntry node, bool hist)
{
  return (hist) ? new afxShapeNodeHistConstraint(mgr, name, node) : new afxShapeNodeConstraint(mgr, name, node);
}

inline afxObjectConstraint* newObjectCons(afxConstraintMgr* mgr, bool hist)
{
  return (hist) ? new afxObjectHistConstraint(mgr) : new afxObjectConstraint(mgr);
}

inline afxObjectConstraint* newObjectCons(afxConstraintMgr* mgr, StringTableEntry name, bool hist)
{
  return (hist) ? new afxObjectHistConstraint(mgr, name) : new afxObjectConstraint(mgr, name);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxConstraintMgr

#define CONS_BY_ID(id) ((*constraints_v[(id).index])[(id).sub_index])
#define CONS_BY_IJ(i,j) ((*constraints_v[(i)])[(j)])

afxConstraintMgr::afxConstraintMgr()
{
  starttime = 0;
  on_server = false;
  initialized = false;
  missing_objs = &missing_objs_a;
  missing_objs2 = &missing_objs_b;
}

afxConstraintMgr::~afxConstraintMgr()
{
  for (S32 i = 0; i < constraints_v.size(); i++)
  {
    for (S32 j = 0; j < (*constraints_v[i]).size(); j++)
      delete CONS_BY_IJ(i,j);
    delete constraints_v[i];
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

S32 afxConstraintMgr::find_cons_idx_from_name(StringTableEntry which)
{
  for (S32 i = 0; i < constraints_v.size(); i++)
  {
    afxConstraint* cons = CONS_BY_IJ(i,0);
    if (cons && afxConstraintDef::CONS_EFFECT != cons->cons_def.def_type && 
        which == cons->cons_def.cons_src_name)
    {
        return i;
    }
  }

  return -1;
}

S32 afxConstraintMgr::find_effect_cons_idx_from_name(StringTableEntry which)
{
  for (S32 i = 0; i < constraints_v.size(); i++)
  {
    afxConstraint* cons = CONS_BY_IJ(i,0);
    if (cons && afxConstraintDef::CONS_EFFECT == cons->cons_def.def_type && 
        which == cons->cons_def.cons_src_name)
    {
      return i;
    }
  }

  return -1;
}

// Defines a predefined constraint with given name and type
void afxConstraintMgr::defineConstraint(U32 type, StringTableEntry name)
{
  preDef predef = { name, type };
  predefs.push_back(predef);
}

afxConstraintID afxConstraintMgr::setReferencePoint(StringTableEntry which, Point3F point, 
                                                    Point3F vector)
{
  S32 idx = find_cons_idx_from_name(which);
  if (idx < 0)
    return afxConstraintID();

  afxConstraintID id = afxConstraintID(idx);
  setReferencePoint(id, point, vector); 

  return id;
}

// Assigns an existing scene-object to the named constraint
afxConstraintID afxConstraintMgr::setReferenceObject(StringTableEntry which, SceneObject* obj)
{
  S32 idx = find_cons_idx_from_name(which);
  if (idx < 0)
    return afxConstraintID();

  afxConstraintID id = afxConstraintID(idx);
  setReferenceObject(id, obj); 

  return id;
}

// Assigns an un-scoped scene-object by scope_id to the named constraint
afxConstraintID afxConstraintMgr::setReferenceObjectByScopeId(StringTableEntry which, U16 scope_id, bool is_shape)
{
  S32 idx = find_cons_idx_from_name(which);
  if (idx < 0)
    return afxConstraintID();

  afxConstraintID id = afxConstraintID(idx);
  setReferenceObjectByScopeId(id, scope_id, is_shape); 

  return id;
}

afxConstraintID afxConstraintMgr::setReferenceEffect(StringTableEntry which, afxEffectWrapper* ew)
{
  S32 idx = find_effect_cons_idx_from_name(which);
  if (idx < 0)
    return afxConstraintID();

  afxConstraintID id = afxConstraintID(idx);
  setReferenceEffect(id, ew); 

  return id;
}

void afxConstraintMgr::setReferencePoint(afxConstraintID id, Point3F point, Point3F vector)
{
  afxPointConstraint* pt_cons = dynamic_cast<afxPointConstraint*>(CONS_BY_ID(id));
  // need to change type
  if (!pt_cons)
  {
    afxConstraint* cons = CONS_BY_ID(id);
    pt_cons = newPointCons(this, cons->cons_def.history_time > 0.0f);
    pt_cons->cons_def = cons->cons_def;
    CONS_BY_ID(id) = pt_cons;
    delete cons;
  }

  pt_cons->set(point, vector);

  // nullify all subnodes
  for (S32 j = 1; j < (*constraints_v[id.index]).size(); j++)
  {
    afxConstraint* cons = CONS_BY_IJ(id.index,j);
    afxShapeNodeConstraint* sub = dynamic_cast<afxShapeNodeConstraint*>(cons);
    if (sub)
      sub->set(0);
  }
}

void afxConstraintMgr::set_ref_shape(afxConstraintID id, ShapeBase* shape)
{
  id.sub_index = 0;

  afxShapeConstraint* shape_cons = dynamic_cast<afxShapeConstraint*>(CONS_BY_ID(id));
  // need to change type
  if (!shape_cons)
  {
    afxConstraint* cons = CONS_BY_ID(id);
    shape_cons = newShapeCons(this, cons->cons_def.history_time > 0.0f);
    shape_cons->cons_def = cons->cons_def;
    CONS_BY_ID(id) = shape_cons;
    delete cons;
  }

  // set new shape on root 
  shape_cons->set(shape);

  // set new shape on all subnodes
  for (S32 j = 1; j < (*constraints_v[id.index]).size(); j++)
  {
    afxConstraint* cons = CONS_BY_IJ(id.index,j);
    afxShapeNodeConstraint* sub = dynamic_cast<afxShapeNodeConstraint*>(cons);
    if (sub)
      sub->set(shape);
  }
}

void afxConstraintMgr::set_ref_shape(afxConstraintID id, U16 scope_id)
{
  id.sub_index = 0;

  afxShapeConstraint* shape_cons = dynamic_cast<afxShapeConstraint*>(CONS_BY_ID(id));
  // need to change type
  if (!shape_cons)
  {
    afxConstraint* cons = CONS_BY_ID(id);
    shape_cons = newShapeCons(this, cons->cons_def.history_time > 0.0f);
    shape_cons->cons_def = cons->cons_def;
    CONS_BY_ID(id) = shape_cons;
    delete cons;
  }

  // set new shape on root 
  shape_cons->set_scope_id(scope_id);

  // set new shape on all subnodes
  for (S32 j = 1; j < (*constraints_v[id.index]).size(); j++)
  {
    afxConstraint* cons = CONS_BY_IJ(id.index,j);
    afxShapeNodeConstraint* sub = dynamic_cast<afxShapeNodeConstraint*>(cons);
    if (sub)
      sub->set_scope_id(scope_id);
  }
}

// Assigns an existing scene-object to the constraint matching the given constraint-id.
void afxConstraintMgr::setReferenceObject(afxConstraintID id, SceneObject* obj)
{
  if (!initialized)
    Con::errorf("afxConstraintMgr::setReferenceObject() -- constraint manager not initialized");

  ShapeBase* shape = dynamic_cast<ShapeBase*>(obj);
  if (shape)
  {
    set_ref_shape(id, shape);
    return;
  }

  afxObjectConstraint* obj_cons = dynamic_cast<afxObjectConstraint*>(CONS_BY_ID(id));
  // need to change type
  if (!obj_cons)
  {
    afxConstraint* cons = CONS_BY_ID(id);
    obj_cons = newObjectCons(this, cons->cons_def.history_time > 0.0f);
    obj_cons->cons_def = cons->cons_def;
    CONS_BY_ID(id) = obj_cons;
    delete cons;
  }

  obj_cons->set(obj);

  // nullify all subnodes
  for (S32 j = 1; j < (*constraints_v[id.index]).size(); j++)
  {
    afxConstraint* cons = CONS_BY_IJ(id.index,j);
    afxShapeNodeConstraint* sub = dynamic_cast<afxShapeNodeConstraint*>(cons);
    if (sub)
      sub->set(0);
  }
}

// Assigns an un-scoped scene-object by scope_id to the constraint matching the 
// given constraint-id.
void afxConstraintMgr::setReferenceObjectByScopeId(afxConstraintID id, U16 scope_id, bool is_shape)
{
  if (!initialized)
    Con::errorf("afxConstraintMgr::setReferenceObject() -- constraint manager not initialized");

  if (is_shape)
  {
    set_ref_shape(id, scope_id);
    return;
  }

  afxObjectConstraint* obj_cons = dynamic_cast<afxObjectConstraint*>(CONS_BY_ID(id));
  // need to change type
  if (!obj_cons)
  {
    afxConstraint* cons = CONS_BY_ID(id);
    obj_cons = newObjectCons(this, cons->cons_def.history_time > 0.0f);
    obj_cons->cons_def = cons->cons_def;
    CONS_BY_ID(id) = obj_cons;
    delete cons;
  }

  obj_cons->set_scope_id(scope_id);

  // nullify all subnodes
  for (S32 j = 1; j < (*constraints_v[id.index]).size(); j++)
  {
    afxConstraint* cons = CONS_BY_IJ(id.index,j);
    afxShapeNodeConstraint* sub = dynamic_cast<afxShapeNodeConstraint*>(cons);
    if (sub)
      sub->set(0);
  }
}

void afxConstraintMgr::setReferenceEffect(afxConstraintID id, afxEffectWrapper* ew)
{
  afxEffectConstraint* eff_cons = dynamic_cast<afxEffectConstraint*>(CONS_BY_ID(id));
  if (!eff_cons)
    return;

  eff_cons->set(ew);

  // nullify all subnodes
  for (S32 j = 1; j < (*constraints_v[id.index]).size(); j++)
  {
    afxConstraint* cons = CONS_BY_IJ(id.index,j);
    afxEffectNodeConstraint* sub = dynamic_cast<afxEffectNodeConstraint*>(cons);
    if (sub)
      sub->set(ew);
  }
}

void afxConstraintMgr::create_constraint(const afxConstraintDef& def)
{
  if (def.def_type == afxConstraintDef::CONS_UNDEFINED)
    return;

  //Con::printf("CON - %s [%s] [%s] h=%g", def.cons_type_name, def.cons_src_name, def.cons_node_name, def.history_time);

  bool want_history = (def.history_time > 0.0f);

  // constraint is an arbitrary named scene object
  //
  if (def.def_type == afxConstraintDef::CONS_SCENE)
  {
    if (def.cons_src_name == ST_NULLSTRING)
      return;

    // find the arbitrary object by name
    SceneObject* arb_obj = find_object_from_name(def.cons_src_name);

    // if it's a shapeBase object, create a Shape or ShapeNode constraint
    if (dynamic_cast<ShapeBase*>(arb_obj))
    {
      if (def.cons_node_name == ST_NULLSTRING)
      {
        afxShapeConstraint* cons = newShapeCons(this, def.cons_src_name, want_history);
        cons->cons_def = def;
        cons->set((ShapeBase*)arb_obj); 
        afxConstraintList* list = new afxConstraintList();
        list->push_back(cons);
        constraints_v.push_back(list);
      }
      else
      {
        afxShapeNodeConstraint* sub = newShapeNodeCons(this, def.cons_src_name, def.cons_node_name, want_history);
        sub->cons_def = def;
        sub->set((ShapeBase*)arb_obj); 
        afxConstraintList* list = constraints_v[constraints_v.size()-1];
        if (list && (*list)[0])
          list->push_back(sub);
      }
    }
    // if it's not a shapeBase object, create an Object constraint
    else if (arb_obj)
    {
      afxObjectConstraint* cons = newObjectCons(this, def.cons_src_name, want_history);
      cons->cons_def = def;
      cons->set(arb_obj);
      afxConstraintList* list = new afxConstraintList();
      list->push_back(cons);
      constraints_v.push_back(list);
    }
  }

  // constraint is an arbitrary named effect
  //
  else if (def.def_type == afxConstraintDef::CONS_EFFECT)
  {
    if (def.cons_src_name == ST_NULLSTRING)
      return;

    // create an Effect constraint
    if (def.cons_node_name == ST_NULLSTRING)
    {
      afxEffectConstraint* cons = new afxEffectConstraint(this, def.cons_src_name);
      cons->cons_def = def;
      afxConstraintList* list = new afxConstraintList();
      list->push_back(cons);
      constraints_v.push_back(list);
    }
    // create an EffectNode constraint
    else
    {
      afxEffectNodeConstraint* sub = new afxEffectNodeConstraint(this, def.cons_src_name, def.cons_node_name);
      sub->cons_def = def;
      afxConstraintList* list = constraints_v[constraints_v.size()-1];
      if (list && (*list)[0])
        list->push_back(sub);
    }
  }

  // constraint is a predefined constraint
  //
  else
  {
    afxConstraint* cons = 0;
    afxConstraint* sub = 0;

    if (def.def_type == afxConstraintDef::CONS_GHOST)
    {
      for (S32 i = 0; i < predefs.size(); i++)
      {
        if (predefs[i].name == def.cons_src_name)
        {
          if (def.cons_node_name == ST_NULLSTRING)
          {
            cons = newShapeCons(this, want_history);
            cons->cons_def = def;
          }
          else
          {
            sub = newShapeNodeCons(this, ST_NULLSTRING, def.cons_node_name, want_history);
            sub->cons_def = def;
          }
          break;
        }
      }
    }
    else
    {
      for (S32 i = 0; i < predefs.size(); i++)
      {
        if (predefs[i].name == def.cons_src_name)
        {
          switch (predefs[i].type)
          {
          case POINT_CONSTRAINT:
            cons = newPointCons(this, want_history);
            cons->cons_def = def;
            break;
          case OBJECT_CONSTRAINT:
            if (def.cons_node_name == ST_NULLSTRING)
            {
              cons = newShapeCons(this, want_history);
              cons->cons_def = def;
            }
            else
            {
              sub = newShapeNodeCons(this, ST_NULLSTRING, def.cons_node_name, want_history);
              sub->cons_def = def;
            }
            break;
          }
          break;
        }
      }
    }

    if (cons)
    {
      afxConstraintList* list = new afxConstraintList();
      list->push_back(cons);
      constraints_v.push_back(list);
    }
    else if (sub && constraints_v.size() > 0)
    {
      afxConstraintList* list = constraints_v[constraints_v.size()-1];
      if (list && (*list)[0])
        list->push_back(sub);
    }
    else
      Con::printf("predef not found %s", def.cons_src_name);
  }
}

afxConstraintID afxConstraintMgr::getConstraintId(const afxConstraintDef& def)
{
  if (def.def_type == afxConstraintDef::CONS_UNDEFINED)
    return afxConstraintID();

  if (def.cons_src_name != ST_NULLSTRING)
  {
    for (S32 i = 0; i < constraints_v.size(); i++)
    {
      afxConstraintList* list = constraints_v[i];
      afxConstraint* cons = (*list)[0];
      if (def.cons_src_name == cons->cons_def.cons_src_name)
      {
        for (S32 j = 0; j < list->size(); j++)
        {
          afxConstraint* sub = (*list)[j];
          if (def.cons_node_name == sub->cons_def.cons_node_name && 
              def.cons_src_name == sub->cons_def.cons_src_name)
          {
            return afxConstraintID(i, j);
          }
        }
        break;
      }
    }
  }

  return afxConstraintID();
}

afxConstraint* afxConstraintMgr::getConstraint(afxConstraintID id)
{
  if (id.undefined())
    return 0;

  SceneObject* arb_obj = 0;
  afxConstraint* cons = CONS_BY_IJ(id.index,id.sub_index);
  if (cons && !cons->isDefined())
    return NULL;

  return cons;
}

void afxConstraintMgr::sample(F32 dt, U32 now)
{
  U32 elapsed = now - starttime;

  for (S32 i = 0; i < constraints_v.size(); i++)
  {
    afxConstraintList* list = constraints_v[i];
    for (S32 j = 0; j < list->size(); j++)
      (*list)[j]->sample(dt, elapsed);
  }
}

S32 QSORT_CALLBACK cmp_cons_defs(const void* a, const void* b)
{
  afxConstraintDef* def_a = (afxConstraintDef*) a;
  afxConstraintDef* def_b = (afxConstraintDef*) b;

  if (def_a->def_type == def_b->def_type)
  {
    if (def_a->cons_src_name == def_b->cons_src_name)
      return (def_a->cons_node_name - def_b->cons_node_name);
    return (def_a->cons_src_name - def_b->cons_src_name);
  }

  return (def_a->def_type - def_b->def_type);
}

void afxConstraintMgr::initConstraintDefs(Vector<afxConstraintDef>& all_defs, bool on_server)
{
  initialized = true;
  this->on_server = on_server;

  if (all_defs.size() < 1)
    return;

  // find effect ghost constraints
  if (!on_server)
  {
    Vector<afxConstraintDef> ghost_defs;

    for (S32 i = 0; i < all_defs.size(); i++)
      if (all_defs[i].def_type == afxConstraintDef::CONS_GHOST && all_defs[i].cons_src_name != ST_NULLSTRING)
        ghost_defs.push_back(all_defs[i]);
    
    if (ghost_defs.size() > 0)
    {
      // sort the defs
      if (ghost_defs.size() > 1)
        dQsort(ghost_defs.address(), ghost_defs.size(), sizeof(afxConstraintDef), cmp_cons_defs);
      
      S32 last = 0;
      defineConstraint(OBJECT_CONSTRAINT, ghost_defs[0].cons_src_name);

      for (S32 i = 1; i < ghost_defs.size(); i++)
      {
        if (ghost_defs[last].cons_src_name != ghost_defs[i].cons_src_name)
        {
          defineConstraint(OBJECT_CONSTRAINT, ghost_defs[i].cons_src_name);
          last++;
        }
      }
    }
  }

  Vector<afxConstraintDef> defs;

  // collect defs that run here (server or client)
  if (on_server)
  {
    for (S32 i = 0; i < all_defs.size(); i++)
      if (all_defs[i].runs_on_server)
        defs.push_back(all_defs[i]);
  }
  else
  {
    for (S32 i = 0; i < all_defs.size(); i++)
      if (all_defs[i].runs_on_client)
        defs.push_back(all_defs[i]);
  }

  // create unique set of constraints.
  //
  if (defs.size() > 0)
  {
    // sort the defs
    if (defs.size() > 1)
      dQsort(defs.address(), defs.size(), sizeof(afxConstraintDef), cmp_cons_defs);
    
    Vector<afxConstraintDef> unique_defs;
    S32 last = 0;
    
    // manufacture root-object def if absent
    if (defs[0].cons_node_name != ST_NULLSTRING)
    {
      afxConstraintDef root_def = defs[0];
      root_def.cons_node_name = ST_NULLSTRING;
      unique_defs.push_back(root_def);
      last++;
    }

    unique_defs.push_back(defs[0]);
    
    for (S32 i = 1; i < defs.size(); i++)
    {
      if (unique_defs[last].cons_node_name != defs[i].cons_node_name ||
          unique_defs[last].cons_src_name != defs[i].cons_src_name || 
          unique_defs[last].def_type != defs[i].def_type)
      {
        // manufacture root-object def if absent
        if (defs[i].cons_src_name != ST_NULLSTRING && unique_defs[last].cons_src_name != defs[i].cons_src_name)
        {
          if (defs[i].cons_node_name != ST_NULLSTRING)
          {
            afxConstraintDef root_def = defs[i];
            root_def.cons_node_name = ST_NULLSTRING;
            unique_defs.push_back(root_def);
            last++;
          }
        }
        unique_defs.push_back(defs[i]);
        last++;
      }
      else
      {
        if (defs[i].history_time > unique_defs[last].history_time)
          unique_defs[last].history_time = defs[i].history_time;
        if (defs[i].sample_rate > unique_defs[last].sample_rate)
          unique_defs[last].sample_rate = defs[i].sample_rate;
      }
    }
    
    //Con::printf("\nConstraints on %s", (on_server) ? "server" : "client");
    for (S32 i = 0; i < unique_defs.size(); i++)
      create_constraint(unique_defs[i]);
  }

  // collect the names of all the arbitrary object constraints
  // that run on clients and store in names_on_server array.
  //
  if (on_server)
  {
    names_on_server.clear();
    defs.clear();

    for (S32 i = 0; i < all_defs.size(); i++)
      if (all_defs[i].runs_on_client && all_defs[i].isArbitraryObject())
        defs.push_back(all_defs[i]);

    if (defs.size() < 1)
      return;

    // sort the defs
    if (defs.size() > 1)
      dQsort(defs.address(), defs.size(), sizeof(afxConstraintDef), cmp_cons_defs);

    S32 last = 0;
    names_on_server.push_back(defs[0].cons_src_name);

    for (S32 i = 1; i < defs.size(); i++)
    {
      if (names_on_server[last] != defs[i].cons_src_name)
      {
        names_on_server.push_back(defs[i].cons_src_name);
        last++;
      }
    }
  }
}

void afxConstraintMgr::packConstraintNames(NetConnection* conn, BitStream* stream)
{
  // pack any named constraint names and ghost indices
  if (stream->writeFlag(names_on_server.size() > 0)) //-- ANY NAMED CONS_BY_ID?
  {
    stream->write(names_on_server.size());
    for (S32 i = 0; i < names_on_server.size(); i++)
    {
      stream->writeString(names_on_server[i]);
      NetObject* obj = dynamic_cast<NetObject*>(Sim::findObject(names_on_server[i]));
      if (!obj)
      {
        //Con::printf("CONSTRAINT-OBJECT %s does not exist.", names_on_server[i]);
        stream->write((S32)-1);
      }
      else
      {
        S32 ghost_id = conn->getGhostIndex(obj);
        /*
        if (ghost_id == -1)
          Con::printf("CONSTRAINT-OBJECT %s does not have a ghost.", names_on_server[i]);
        else
          Con::printf("CONSTRAINT-OBJECT %s name to server.", names_on_server[i]);
         */
        stream->write(ghost_id);
      }
    }
  }
}

void afxConstraintMgr::unpackConstraintNames(BitStream* stream)
{
  if (stream->readFlag())                                         //-- ANY NAMED CONS_BY_ID?
  {
    names_on_server.clear();
    S32 sz; stream->read(&sz);
    for (S32 i = 0; i < sz; i++)
    {
      names_on_server.push_back(stream->readSTString());
      S32 ghost_id; stream->read(&ghost_id);
      ghost_ids.push_back(ghost_id);
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

SceneObject* afxConstraintMgr::find_object_from_name(StringTableEntry name)
{
  if (names_on_server.size() > 0)
  {
    for (S32 i = 0; i < names_on_server.size(); i++)
      if (names_on_server[i] == name)
      {
        if (ghost_ids[i] == -1)
          return 0;
        NetConnection* conn = NetConnection::getConnectionToServer();
        if (!conn)
          return 0;
        return dynamic_cast<SceneObject*>(conn->resolveGhost(ghost_ids[i]));
      }
    return 0;
  }

  return dynamic_cast<SceneObject*>(Sim::findObject(name));
}

void afxConstraintMgr::addScopeableObject(SceneObject* object)
{
  for (S32 i = 0; i < scopeable_objs.size(); i++)
  {
    if (scopeable_objs[i] == object)
      return;
  }

  object->addScopeRef();
  scopeable_objs.push_back(object);
}

void afxConstraintMgr::removeScopeableObject(SceneObject* object)
{
  for (S32 i = 0; i < scopeable_objs.size(); i++)
    if (scopeable_objs[i] == object)
    {
      object->removeScopeRef();
      scopeable_objs.erase_fast(i);
      return;
    }
}

void afxConstraintMgr::clearAllScopeableObjs()
{
  for (S32 i = 0; i < scopeable_objs.size(); i++)
    scopeable_objs[i]->removeScopeRef();
  scopeable_objs.clear();
}

void afxConstraintMgr::postMissingConstraintObject(afxConstraint* cons, bool is_deleting)
{
  if (cons->gone_missing)
    return;

  if (!is_deleting)
  {
    SceneObject* obj = arcaneFX::findScopedObject(cons->getScopeId());
    if (obj)
    {
      cons->restoreObject(obj);
      return;
    }
  }

  cons->gone_missing = true;
  missing_objs->push_back(cons);
}

void afxConstraintMgr::restoreScopedObject(SceneObject* obj, afxChoreographer* ch)
{
  for (S32 i = 0; i < missing_objs->size(); i++)
  {
    if ((*missing_objs)[i]->getScopeId() == obj->getScopeId())
    {
      (*missing_objs)[i]->gone_missing = false;
      (*missing_objs)[i]->restoreObject(obj);
      if (ch)
        ch->restoreObject(obj);
    }
    else
      missing_objs2->push_back((*missing_objs)[i]);
  }

  Vector<afxConstraint*>* tmp = missing_objs;
  missing_objs = missing_objs2;
  missing_objs2 = tmp;
  missing_objs2->clear();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxPointConstraint

afxPointConstraint::afxPointConstraint(afxConstraintMgr* mgr) 
  : afxConstraint(mgr)
{
  point.zero();
  vector.set(0,0,1);
}

afxPointConstraint::~afxPointConstraint()
{
}

void afxPointConstraint::set(Point3F point, Point3F vector)
{
  this->point = point;
  this->vector = vector;
  is_defined = true;
  is_valid = true;
  sample(0.0f, 0);
}

void afxPointConstraint::sample(F32 dt, U32 elapsed_ms)
{
  last_pos = point;
  last_xfm.identity();
  last_xfm.setPosition(point);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxShapeConstraint

afxShapeConstraint::afxShapeConstraint(afxConstraintMgr* mgr) 
  : afxConstraint(mgr)
{
  arb_name = ST_NULLSTRING;
  shape = 0;
  scope_id = 0;
}

afxShapeConstraint::afxShapeConstraint(afxConstraintMgr* mgr, StringTableEntry arb_name) 
  : afxConstraint(mgr)
{
  this->arb_name = arb_name;
  shape = 0;
  scope_id = 0;
}

afxShapeConstraint::~afxShapeConstraint()
{
  if (shape)
    clearNotify(shape);
}

void afxShapeConstraint::set(ShapeBase* shape)
{
  if (this->shape)
  {
    scope_id = 0;
    clearNotify(this->shape);
  }

  this->shape = shape;

  if (this->shape)
  {
    deleteNotify(this->shape);
    scope_id = this->shape->getScopeId();
  }

  if (this->shape != NULL)
  {
    is_defined = true;
    is_valid = true;
    sample(0.0f, 0);
  }
  else
    is_valid = false;
}

void afxShapeConstraint::set_scope_id(U16 scope_id)
{
  if (shape)
    clearNotify(shape);

  shape = 0;
  this->scope_id = scope_id;

  is_defined = (this->scope_id > 0);
  is_valid = false;
  mgr->postMissingConstraintObject(this);
}

void afxShapeConstraint::sample(F32 dt, U32 elapsed_ms)
{
  if (gone_missing)
    return;

  if (shape)
  {
    last_xfm = shape->getRenderTransform();
    if (cons_def.pos_at_box_center)
      last_pos = shape->getBoxCenter();
    else
      last_pos = shape->getRenderPosition();
  }
}

void afxShapeConstraint::restoreObject(SceneObject* obj) 
{ 
  if (this->shape)
  {
    scope_id = 0;
    clearNotify(this->shape);
  }

  this->shape = (ShapeBase* )obj;

  if (this->shape)
  {
    deleteNotify(this->shape);
    scope_id = this->shape->getScopeId();
  }

  is_valid = (this->shape != NULL);
}

void afxShapeConstraint::onDeleteNotify(SimObject* obj)
{
  if (shape == dynamic_cast<ShapeBase*>(obj))
  {
    shape = 0;
    is_valid = false;
    if (scope_id > 0)
      mgr->postMissingConstraintObject(this, true);
  }

  Parent::onDeleteNotify(obj);
}

U32 afxShapeConstraint::setAnimClip(const char* clip, F32 pos, F32 rate, F32 trans, bool is_death_anim)
{
  Player* player = dynamic_cast<Player*>(shape);
  if (!player)
    return 0;

  if (player->isServerObject())
  {
    AIPlayer* ai_player = dynamic_cast<AIPlayer*>(player);
    if (ai_player && !ai_player->isBlendAnimation(clip))
    {
      ai_player->saveMoveState();
      ai_player->stopMove();
    }
  }

  //F32 pos = (rate < 0) ? 1.0f : 0.0f;
  return player->playAnimation(clip, pos, rate, trans, false/*hold*/, true/*wait*/, is_death_anim);
}

void afxShapeConstraint::resetAnimation(U32 tag, bool is_death_anim)
{
  Player* player = dynamic_cast<Player*>(shape);
  if (!player)
    return;
  
  if (player->isClientObject())
  {
    player->restoreAnimation(tag, is_death_anim);
  }
  else
  {
    AIPlayer* ai_player = dynamic_cast<AIPlayer*>(player);
    if (ai_player)
      ai_player->restartMove(tag, is_death_anim);
    else
      player->restoreAnimation(tag, is_death_anim);
  }
}

U32 afxShapeConstraint::lockAnimation()
{
  Player* player = dynamic_cast<Player*>(shape);
  if (!player)
    return 0;

  return player->lockAnimation();
}

void afxShapeConstraint::unlockAnimation(U32 tag)
{
  Player* player = dynamic_cast<Player*>(shape);
  if (!player)
    return;
  
  player->unlockAnimation(tag);
}

F32 afxShapeConstraint::getAnimClipDuration(const char* clip)
{
  Player* player = dynamic_cast<Player*>(shape);
  return (player) ? player->getAnimationDuration(clip) : 0.0f;
}

S32 afxShapeConstraint::getDamageState()
{
  return (shape) ? shape->getDamageState() : -1;
}

bool afxShapeConstraint::getTriggerState(U32 stateNum, bool clearState)
{
  return (shape) ? shape->getShapeInstance()->getTriggerState(stateNum, clearState) : false;
}



//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxShapeNodeConstraint

afxShapeNodeConstraint::afxShapeNodeConstraint(afxConstraintMgr* mgr)  
  : afxShapeConstraint(mgr)
{
  arb_node = ST_NULLSTRING;
  shape_node_ID = -1;
}

afxShapeNodeConstraint::afxShapeNodeConstraint(afxConstraintMgr* mgr, StringTableEntry arb_name, StringTableEntry arb_node)
  : afxShapeConstraint(mgr, arb_name)
{
  this->arb_node = arb_node;
  shape_node_ID = -1;
}

void afxShapeNodeConstraint::set(ShapeBase* shape)
{
  shape_node_ID = (shape) ? shape->getShape()->findNode(arb_node) : -1;

  Parent::set(shape);
}

void afxShapeNodeConstraint::set_scope_id(U16 scope_id)
{
  shape_node_ID = -1;
  Parent::set_scope_id(scope_id);
}

void afxShapeNodeConstraint::sample(F32 dt, U32 elapsed_ms)
{
  if (shape && shape_node_ID != -1)
  {
    last_xfm.mul(shape->getRenderTransform(), shape->getShapeInstance()->mNodeTransforms[shape_node_ID]);
    last_pos = last_xfm.getPosition();
  }
}

void afxShapeNodeConstraint::restoreObject(SceneObject* obj) 
{ 
  ShapeBase* shape = dynamic_cast<ShapeBase*>(obj);
  shape_node_ID = (shape) ? shape->getShape()->findNode(arb_node) : -1;
  Parent::restoreObject(obj);
}

void afxShapeNodeConstraint::onDeleteNotify(SimObject* obj)
{
  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxObjectConstraint

afxObjectConstraint::afxObjectConstraint(afxConstraintMgr* mgr) 
  : afxConstraint(mgr)
{
  arb_name = ST_NULLSTRING;
  obj = 0;
  scope_id = 0;
}

afxObjectConstraint::afxObjectConstraint(afxConstraintMgr* mgr, StringTableEntry arb_name) 
  : afxConstraint(mgr)
{
  this->arb_name = arb_name;
  obj = 0;
  scope_id = 0;
}

afxObjectConstraint::~afxObjectConstraint()
{
  if (obj)
    clearNotify(obj);
}

void afxObjectConstraint::set(SceneObject* obj)
{
  if (this->obj)
  {
    scope_id = 0;
    clearNotify(this->obj);
  }

  this->obj = obj;

  if (this->obj)
  {
    deleteNotify(this->obj);
    scope_id = this->obj->getScopeId();
  }

  if (this->obj != NULL)
  {
    is_defined = true;
    is_valid = true;
    sample(0.0f, 0);
  }
  else
    is_valid = false;
}

void afxObjectConstraint::set_scope_id(U16 scope_id)
{
  if (obj)
    clearNotify(obj);

  obj = 0;
  this->scope_id = scope_id;

  is_defined = (scope_id > 0);
  is_valid = false;
  mgr->postMissingConstraintObject(this);
}

void afxObjectConstraint::sample(F32 dt, U32 elapsed_ms)
{
  if (gone_missing)
    return;

  if (obj)
  {
    last_xfm = obj->getRenderTransform();
    if (cons_def.pos_at_box_center)
      last_pos = obj->getBoxCenter();
    else
      last_pos = obj->getRenderPosition();
  }
}

void afxObjectConstraint::restoreObject(SceneObject* obj)
{
  if (this->obj)
  {
    scope_id = 0;
    clearNotify(this->obj);
  }

  this->obj = obj;

  if (this->obj)
  {
    deleteNotify(this->obj);
    scope_id = this->obj->getScopeId();
  }

  is_valid = (this->obj != NULL);
}

void afxObjectConstraint::onDeleteNotify(SimObject* obj)
{
  if (this->obj == dynamic_cast<SceneObject*>(obj))
  {
    this->obj = 0;
    is_valid = false;
    if (scope_id > 0)
      mgr->postMissingConstraintObject(this, true);
  }

  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEffectConstraint

afxEffectConstraint::afxEffectConstraint(afxConstraintMgr* mgr) 
  : afxConstraint(mgr)
{
  effect_name = ST_NULLSTRING;
  effect = 0;
}

afxEffectConstraint::afxEffectConstraint(afxConstraintMgr* mgr, StringTableEntry effect_name) 
  : afxConstraint(mgr)
{
  this->effect_name = effect_name;
  effect = 0;
}

afxEffectConstraint::~afxEffectConstraint()
{
}

bool afxEffectConstraint::getPosition(Point3F& pos, F32 hist) 
{ 
  if (!effect)
    return false;
 
  if (cons_def.pos_at_box_center)
    effect->getUpdatedBoxCenter(pos);
  else
    effect->getUpdatedPosition(pos);
  
  return true;
}

bool afxEffectConstraint::getTransform(MatrixF& xfm, F32 hist) 
{ 
  if (!effect)
    return false;
  
  effect->getUpdatedTransform(xfm);
  return true;
}

void afxEffectConstraint::set(afxEffectWrapper* effect)
{
  this->effect = effect;

  if (this->effect != NULL)
  {
    is_defined = true;
    is_valid = true;
  }
  else
    is_valid = false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEffectNodeConstraint

afxEffectNodeConstraint::afxEffectNodeConstraint(afxConstraintMgr* mgr) 
  : afxEffectConstraint(mgr)
{
  effect_node = ST_NULLSTRING;
  effect_node_ID = -1;
}

afxEffectNodeConstraint::afxEffectNodeConstraint(afxConstraintMgr* mgr, StringTableEntry name, StringTableEntry node)
: afxEffectConstraint(mgr, name)
{
  this->effect_node = node;
  effect_node_ID = -1;
}



bool afxEffectNodeConstraint::getPosition(Point3F& pos, F32 hist) 
{ 
  if (!effect)
    return false;
  
  TSShapeInstance* ts_shape_inst = effect->getTSShapeInstance();
  if (!ts_shape_inst)
    return false;

  if (effect_node_ID == -1)
  {
    TSShape* ts_shape = effect->getTSShape();
    effect_node_ID = (ts_shape) ? ts_shape->findNode(effect_node) : -1;
  }

  if (effect_node_ID == -1)
    return false;

  effect->getUpdatedTransform(last_xfm);

  Point3F scale;
  effect->getUpdatedScale(scale);

  MatrixF gag = ts_shape_inst->mNodeTransforms[effect_node_ID];
  // scale must be "baked-in" to matrix positions for later call to xfm.getPosition()
  //   gag.scale(scale) won't work!
  gag.setPosition( gag.getPosition()*scale );

  MatrixF xfm;
  xfm.mul(last_xfm, gag);
  //
  pos = xfm.getPosition();

  return true;
}

bool afxEffectNodeConstraint::getTransform(MatrixF& xfm, F32 hist) 
{ 
  if (!effect)
    return false;
  
  TSShapeInstance* ts_shape_inst = effect->getTSShapeInstance();
  if (!ts_shape_inst)
    return false;

  if (effect_node_ID == -1)
  {
    TSShape* ts_shape = effect->getTSShape();
    effect_node_ID = (ts_shape) ? ts_shape->findNode(effect_node) : -1;
  }

  if (effect_node_ID == -1)
    return false;

  effect->getUpdatedTransform(last_xfm);

  Point3F scale;
  effect->getUpdatedScale(scale);

  MatrixF gag = ts_shape_inst->mNodeTransforms[effect_node_ID];
  // scale must be "baked-in" to matrix positions for later call to xfm.getPosition()
  //   gag.scale(scale) won't work!
  gag.setPosition( gag.getPosition()*scale );

  xfm.mul(last_xfm, gag);

  return true;
}

void afxEffectNodeConstraint::set(afxEffectWrapper* effect)
{
  if (effect)
  {
    TSShape* ts_shape = effect->getTSShape();
    effect_node_ID = (ts_shape) ? ts_shape->findNode(effect_node) : -1;
  }
  else
    effect_node_ID = -1;

  Parent::set(effect);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxSampleBuffer

afxSampleBuffer::afxSampleBuffer()
{
  buffer_sz = 0;
  buffer_ms = 0;
  ms_per_sample = 33;
  elapsed_ms = 0;
  last_sample_ms = 0;
  next_sample_num = 0;
  n_samples = 0;
}

afxSampleBuffer::~afxSampleBuffer()
{
}

void afxSampleBuffer::configHistory(F32 hist_len, U8 sample_rate)
{
  buffer_sz = mCeil(hist_len*sample_rate) + 1;
  ms_per_sample = mCeil(1000.0f/sample_rate);
  buffer_ms = buffer_sz*ms_per_sample;
}

void afxSampleBuffer::recordSample(F32 dt, U32 elapsed_ms, void* data)
{
  this->elapsed_ms = elapsed_ms;

  if (!data)
    return;

  U32 now_sample_num = elapsed_ms/ms_per_sample;
  if (next_sample_num <= now_sample_num)
  {
    last_sample_ms = elapsed_ms;
    while (next_sample_num <= now_sample_num)
    {
      recSample(next_sample_num % buffer_sz, data);
      next_sample_num++;
      n_samples++;
    }
  }
}

inline bool afxSampleBuffer::compute_idx_from_lag(F32 lag, U32& idx) 
{ 
  bool in_bounds = true;

  U32 lag_ms = lag*1000.0f;
  U32 rec_ms = (elapsed_ms < buffer_ms) ? elapsed_ms : buffer_ms;
  if (lag_ms > rec_ms)
  {
    // hasn't produced enough history
    lag_ms = rec_ms;
    in_bounds = false;
  }

  U32 latest_sample_num = last_sample_ms/ms_per_sample;
  U32 then_sample_num = (elapsed_ms - lag_ms)/ms_per_sample;

  if (then_sample_num > latest_sample_num)
  {
    // latest sample is older than lag
    then_sample_num = latest_sample_num;
    in_bounds = false;
  }

  idx = then_sample_num % buffer_sz;
  return in_bounds;
}

inline bool afxSampleBuffer::compute_idx_from_lag(F32 lag, U32& idx1, U32& idx2, F32& t) 
{ 
  bool in_bounds = true;

  F32 lag_ms = lag*1000.0f;
  F32 rec_ms = (elapsed_ms < buffer_ms) ? elapsed_ms : buffer_ms;
  if (lag_ms > rec_ms)
  {
    // hasn't produced enough history
    lag_ms = rec_ms;
    in_bounds = false;
  }

  F32 per_samp = ms_per_sample;
  F32 latest_sample_num = last_sample_ms/per_samp;
  F32 then_sample_num = (elapsed_ms - lag_ms)/per_samp;

  U32 latest_sample_num_i = latest_sample_num;
  U32 then_sample_num_i = then_sample_num;

  if (then_sample_num_i >= latest_sample_num_i)
  {
    if (latest_sample_num_i < then_sample_num_i)
      in_bounds = false;
    t = 0.0;
    idx1 = then_sample_num_i % buffer_sz;
    idx2 = idx1;
  }
  else
  {
    t = then_sample_num - then_sample_num_i;
    idx1 = then_sample_num_i % buffer_sz;
    idx2 = (then_sample_num_i+1) % buffer_sz;
  }

  return in_bounds;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxSampleXfmBuffer

afxSampleXfmBuffer::afxSampleXfmBuffer()
{
  xfm_buffer = 0;
}

afxSampleXfmBuffer::~afxSampleXfmBuffer()
{
  delete [] xfm_buffer;
}

void afxSampleXfmBuffer::configHistory(F32 hist_len, U8 sample_rate)
{
  if (!xfm_buffer)
  {
    afxSampleBuffer::configHistory(hist_len, sample_rate);
    if (buffer_sz > 0)
      xfm_buffer = new MatrixF[buffer_sz];
  }  
}

void afxSampleXfmBuffer::recSample(U32 idx, void* data)
{
  xfm_buffer[idx] = *((MatrixF*)data);
}

void afxSampleXfmBuffer::getSample(F32 lag, void* data, bool& in_bounds) 
{ 
  U32 idx1, idx2;
  F32 t;
  in_bounds = compute_idx_from_lag(lag, idx1, idx2, t);

  if (idx1 == idx2)
  {
    MatrixF* m1 = &xfm_buffer[idx1];
    *((MatrixF*)data) = *m1;
  }
  else
  {
    MatrixF* m1 = &xfm_buffer[idx1];
    MatrixF* m2 = &xfm_buffer[idx2];

    Point3F p1 = m1->getPosition();
    Point3F p2 = m2->getPosition();
    Point3F p; p.interpolate(p1, p2, t);

    if (t < 0.5f)
      *((MatrixF*)data) = *m1;
    else
      *((MatrixF*)data) = *m2;

    ((MatrixF*)data)->setPosition(p);
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxPointHistConstraint

afxPointHistConstraint::afxPointHistConstraint(afxConstraintMgr* mgr)
  : afxPointConstraint(mgr)
{
  samples = 0;
}

afxPointHistConstraint::~afxPointHistConstraint()
{
  delete samples;
}

void afxPointHistConstraint::set(Point3F point, Point3F vector)
{
  if (!samples)
  {
    samples = new afxSampleXfmBuffer;
    samples->configHistory(cons_def.history_time, cons_def.sample_rate);
  }
  
  Parent::set(point, vector);
}

void afxPointHistConstraint::sample(F32 dt, U32 elapsed_ms)
{
  Parent::sample(dt, elapsed_ms);

  if (isDefined())
  {
    if (isValid())
      samples->recordSample(dt, elapsed_ms, &last_xfm);
    else
      samples->recordSample(dt, elapsed_ms, 0);
  }
}

bool afxPointHistConstraint::getPosition(Point3F& pos, F32 hist) 
{ 
  bool in_bounds;

  MatrixF xfm;
  samples->getSample(hist, &xfm, in_bounds);

  pos = xfm.getPosition();

  return in_bounds;
}

bool afxPointHistConstraint::getTransform(MatrixF& xfm, F32 hist) 
{ 
  bool in_bounds;

  samples->getSample(hist, &xfm, in_bounds);

  return in_bounds; 
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxShapeHistConstraint

afxShapeHistConstraint::afxShapeHistConstraint(afxConstraintMgr* mgr)
  : afxShapeConstraint(mgr)
{
  samples = 0;
}

afxShapeHistConstraint::afxShapeHistConstraint(afxConstraintMgr* mgr, StringTableEntry arb_name)
  : afxShapeConstraint(mgr, arb_name)
{
  samples = 0;
}

afxShapeHistConstraint::~afxShapeHistConstraint()
{
  delete samples;
}

void afxShapeHistConstraint::set(ShapeBase* shape)
{
  if (shape && !samples)
  {
    samples = new afxSampleXfmBuffer;
    samples->configHistory(cons_def.history_time, cons_def.sample_rate);
  }
  
  Parent::set(shape);
}

void afxShapeHistConstraint::set_scope_id(U16 scope_id)
{
  if (scope_id > 0 && !samples)
  {
    samples = new afxSampleXfmBuffer;
    samples->configHistory(cons_def.history_time, cons_def.sample_rate);
  }
  
  Parent::set_scope_id(scope_id);
}

void afxShapeHistConstraint::sample(F32 dt, U32 elapsed_ms)
{
  Parent::sample(dt, elapsed_ms);

  if (isDefined())
  {
    if (isValid())
      samples->recordSample(dt, elapsed_ms, &last_xfm);
    else
      samples->recordSample(dt, elapsed_ms, 0);
  }
}

bool afxShapeHistConstraint::getPosition(Point3F& pos, F32 hist) 
{ 
  bool in_bounds;

  MatrixF xfm;
  samples->getSample(hist, &xfm, in_bounds);

  pos = xfm.getPosition();

  return in_bounds;
}

bool afxShapeHistConstraint::getTransform(MatrixF& xfm, F32 hist) 
{ 
  bool in_bounds;

  samples->getSample(hist, &xfm, in_bounds);

  return in_bounds; 
}

void afxShapeHistConstraint::onDeleteNotify(SimObject* obj)
{
  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxShapeNodeHistConstraint

afxShapeNodeHistConstraint::afxShapeNodeHistConstraint(afxConstraintMgr* mgr)
  : afxShapeNodeConstraint(mgr)
{
  samples = 0;
}

afxShapeNodeHistConstraint::afxShapeNodeHistConstraint(afxConstraintMgr* mgr, StringTableEntry arb_name,
                                                       StringTableEntry arb_node)
  : afxShapeNodeConstraint(mgr, arb_name, arb_node)
{
  samples = 0;
}

afxShapeNodeHistConstraint::~afxShapeNodeHistConstraint()
{
  delete samples;
}

void afxShapeNodeHistConstraint::set(ShapeBase* shape)
{
  if (shape && !samples)
  {
    samples = new afxSampleXfmBuffer;
    samples->configHistory(cons_def.history_time, cons_def.sample_rate);
  }
  
  Parent::set(shape);
}

void afxShapeNodeHistConstraint::set_scope_id(U16 scope_id)
{
  if (scope_id > 0 && !samples)
  {
    samples = new afxSampleXfmBuffer;
    samples->configHistory(cons_def.history_time, cons_def.sample_rate);
  }
  
  Parent::set_scope_id(scope_id);
}

void afxShapeNodeHistConstraint::sample(F32 dt, U32 elapsed_ms)
{
  Parent::sample(dt, elapsed_ms);

  if (isDefined())
  {
    if (isValid())
      samples->recordSample(dt, elapsed_ms, &last_xfm);
    else
      samples->recordSample(dt, elapsed_ms, 0);
  }
}

bool afxShapeNodeHistConstraint::getPosition(Point3F& pos, F32 hist) 
{ 
  bool in_bounds;

  MatrixF xfm;
  samples->getSample(hist, &xfm, in_bounds);

  pos = xfm.getPosition();

  return in_bounds;
}

bool afxShapeNodeHistConstraint::getTransform(MatrixF& xfm, F32 hist) 
{ 
  bool in_bounds;

  samples->getSample(hist, &xfm, in_bounds);

  return in_bounds; 
}

void afxShapeNodeHistConstraint::onDeleteNotify(SimObject* obj)
{
  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxObjectHistConstraint

afxObjectHistConstraint::afxObjectHistConstraint(afxConstraintMgr* mgr)
  : afxObjectConstraint(mgr)
{
  samples = 0;
}

afxObjectHistConstraint::afxObjectHistConstraint(afxConstraintMgr* mgr, StringTableEntry arb_name)
  : afxObjectConstraint(mgr, arb_name)
{
  samples = 0;
}

afxObjectHistConstraint::~afxObjectHistConstraint()
{
  delete samples;
}

void afxObjectHistConstraint::set(SceneObject* obj)
{
  if (obj && !samples)
  {
    samples = new afxSampleXfmBuffer;
    samples->configHistory(cons_def.history_time, cons_def.sample_rate);
  }
  
  Parent::set(obj);
}

void afxObjectHistConstraint::set_scope_id(U16 scope_id)
{
  if (scope_id > 0 && !samples)
  {
    samples = new afxSampleXfmBuffer;
    samples->configHistory(cons_def.history_time, cons_def.sample_rate);
  }
  
  Parent::set_scope_id(scope_id);
}

void afxObjectHistConstraint::sample(F32 dt, U32 elapsed_ms)
{
  Parent::sample(dt, elapsed_ms);

  if (isDefined())
  {
    if (isValid())
      samples->recordSample(dt, elapsed_ms, &last_xfm);
    else
      samples->recordSample(dt, elapsed_ms, 0);
  }
}

bool afxObjectHistConstraint::getPosition(Point3F& pos, F32 hist) 
{ 
  bool in_bounds;

  MatrixF xfm;
  samples->getSample(hist, &xfm, in_bounds);

  pos = xfm.getPosition();

  return in_bounds;
}

bool afxObjectHistConstraint::getTransform(MatrixF& xfm, F32 hist) 
{ 
  bool in_bounds;

  samples->getSample(hist, &xfm, in_bounds);

  return in_bounds; 
}

void afxObjectHistConstraint::onDeleteNotify(SimObject* obj)
{
  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

