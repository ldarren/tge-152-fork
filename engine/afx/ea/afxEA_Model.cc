
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"
#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/afxResidueMgr.h"
#include "afx/ce/afxModel.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Model -- This is the adapter for afxModel, a lightweight animated model effect.

class afxEA_Model : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxModelData*     model_data;
  afxModel*         model;

public:
  /*C*/             afxEA_Model();
  /*D*/             ~afxEA_Model();

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
  virtual void      ea_set_scope_status(bool flag);
  virtual void      onDeleteNotify(SimObject*);

  virtual void      getUpdatedBoxCenter(Point3F& pos);
  virtual TSShape*  getTSShape();
  virtual TSShapeInstance*  getTSShapeInstance();
};


afxEA_Model::afxEA_Model()
{
  model_data = 0;
  model = 0;
}

afxEA_Model::~afxEA_Model()
{
  if (model)
    model->deleteObject();
}

void afxEA_Model::ea_set_datablock(SimDataBlock* db)
{
  model_data = dynamic_cast<afxModelData*>(db);
}

bool afxEA_Model::ea_start()
{
  if (!model_data)
  {
    Con::errorf("afxEA_Model::ea_start() -- missing or incompatible datablock.");
    return false;
  }
  return true;
}

bool afxEA_Model::ea_update(F32 dt)
{
  if (!model)
  {
    // create and register effect
    model = new afxModel();
    model->onNewDataBlock(model_data);
    if (!model->registerObject())
    {
      delete model;
      model = 0;
      Con::errorf("afxEA_Model::ea_update() -- effect failed to register.");
      return false;
    }
    deleteNotify(model);

    model->setSequenceRateFactor(datablock->rate_factor/prop_time_factor);
    model->setSortPriority(datablock->sort_priority);
  }

  if (model)
  {
    if (do_fades)
    {
      model->setFadeAmount(fade_value);
    }
    model->setTransform(updated_xfm);
    model->setScale(updated_scale);
  }

  return true;
}

void afxEA_Model::ea_finish(bool was_stopped)
{
  if (!model)
    return;
  
  if (in_scope && ew_timing.residue_lifetime > 0)
  {
    clearNotify(model);
    afxResidueMgr::add(ew_timing.residue_lifetime, ew_timing.residue_fadetime, model);
    model = 0;
  }
  else
  {
    model->deleteObject();
    model = 0;
  }
}

void afxEA_Model::ea_set_scope_status(bool in_scope)
{
  if (model)
    model->setVisibility(in_scope);
}

void afxEA_Model::onDeleteNotify(SimObject* obj)
{
  if (model == dynamic_cast<afxModel*>(obj))
    model = 0;

  Parent::onDeleteNotify(obj);
}

void afxEA_Model::getUpdatedBoxCenter(Point3F& pos)
{
  if (model)
    pos = model->getBoxCenter();
}

TSShape* afxEA_Model::getTSShape()
{
  return (model) ? model->getTSShape() : 0;
}

TSShapeInstance* afxEA_Model::getTSShapeInstance()
{
  return (model) ? model->getTSShapeInstance() : 0;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_ModelDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_ModelDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return true; }

  virtual afxEffectWrapper* create() const { return new afxEA_Model; }
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_ModelDesc afxEA_ModelDesc::desc;

bool afxEA_ModelDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(afxModelData) == typeid(*db));
}

bool afxEA_ModelDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->timing.lifetime < 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//