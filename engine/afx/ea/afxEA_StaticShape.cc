
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"
#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/afxChoreographer.h"
#include "afx/ce/afxStaticShape.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_StaticShape 

class afxEA_StaticShape : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  StaticShapeData*  shape_data;
  afxStaticShape*   static_shape;
  bool              fade_out_started;

public:
  /*C*/             afxEA_StaticShape();
  /*D*/             ~afxEA_StaticShape();

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
  virtual void      ea_set_scope_status(bool flag);
  virtual void      onDeleteNotify(SimObject*);

  virtual void              getUpdatedBoxCenter(Point3F& pos);
  virtual TSShape*          getTSShape();
  virtual TSShapeInstance*  getTSShapeInstance();
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_StaticShape::afxEA_StaticShape()
{
  shape_data = 0;
  static_shape = 0;
  fade_out_started = false;
}

afxEA_StaticShape::~afxEA_StaticShape()
{
  if (static_shape)
    static_shape->deleteObject();
}

void afxEA_StaticShape::ea_set_datablock(SimDataBlock* db)
{
  shape_data = dynamic_cast<StaticShapeData*>(db);
}

bool afxEA_StaticShape::ea_start()
{
  if (!shape_data)
  {
    Con::errorf("afxEA_StaticShape::ea_start() -- missing or incompatible datablock.");
    return false;
  }

  // fades are handled using startFade() calls.
  do_fades = false;

  return true;
}

bool afxEA_StaticShape::ea_update(F32 dt)
{
  if (!static_shape)
  {
    // create and register effect
    static_shape = new afxStaticShape();
    if (datablock->use_ghost_as_cons_obj && datablock->effect_name != ST_NULLSTRING)
      static_shape->init(choreographer->getChoreographerId(), datablock->effect_name);

    static_shape->onNewDataBlock(shape_data);
    if (!static_shape->registerObject())
    {
      delete static_shape;
      static_shape = 0;
      Con::errorf("afxEA_StaticShape::ea_update() -- effect failed to register.");
      return false;
    }
    deleteNotify(static_shape);

    if (ew_timing.fade_in_time > 0.0f)
      static_shape->startFade(ew_timing.fade_in_time, 0, false);
  }

  if (static_shape)
  {
    if (!fade_out_started && elapsed > fade_out_start)
    {
      if (ew_timing.fade_out_time > 0.0f)
        static_shape->startFade(ew_timing.fade_out_time, 0, true);
      fade_out_started = true;
    }
    static_shape->setTransform(updated_xfm);
    static_shape->setScale(updated_scale);
  }

  return true;
}

void afxEA_StaticShape::ea_finish(bool was_stopped)
{
  if (!static_shape)
    return;
  
  static_shape->deleteObject();
  static_shape = 0;
}

void afxEA_StaticShape::ea_set_scope_status(bool in_scope)
{
  if (static_shape)
    static_shape->setVisibility(in_scope);
}

void afxEA_StaticShape::onDeleteNotify(SimObject* obj)
{
  if (static_shape == dynamic_cast<afxStaticShape*>(obj))
    static_shape = 0;

  Parent::onDeleteNotify(obj);
}

void afxEA_StaticShape::getUpdatedBoxCenter(Point3F& pos)
{
  if (static_shape)
    pos = static_shape->getBoxCenter();
}


TSShape* afxEA_StaticShape::getTSShape()
{
  return (static_shape) ? ((TSShape*)static_shape->getShape()) : 0;
}

TSShapeInstance* afxEA_StaticShape::getTSShapeInstance()
{
  return (static_shape) ? static_shape->getShapeInstance() : 0;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_StaticShapeDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_StaticShapeDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return true; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return false; }

  virtual afxEffectWrapper* create() const { return new afxEA_StaticShape; }
};

afxEA_StaticShapeDesc afxEA_StaticShapeDesc::desc;

bool afxEA_StaticShapeDesc::testEffectType(const SimDataBlock* db) const
{
  if (typeid(StaticShapeData) == typeid(*db))
     return true;
  if (typeid(afxStaticShapeData) == typeid(*db))
     return true;
  return false;
}

bool afxEA_StaticShapeDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->timing.lifetime < 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//