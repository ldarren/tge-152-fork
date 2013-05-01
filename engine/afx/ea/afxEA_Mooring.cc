
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"
#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/afxChoreographer.h"
#include "afx/ce/afxMooring.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Mooring 

class afxEA_Mooring : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxMooringData*   mooring_data;
  afxMooring*       obj;

public:
  /*C*/             afxEA_Mooring();
  /*D*/             ~afxEA_Mooring();

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_Mooring::afxEA_Mooring()
{
  mooring_data = 0;
  obj = 0;
}

afxEA_Mooring::~afxEA_Mooring()
{
  if (obj)
    obj->deleteObject();
}

void afxEA_Mooring::ea_set_datablock(SimDataBlock* db)
{
  mooring_data = dynamic_cast<afxMooringData*>(db);
}

bool afxEA_Mooring::ea_start()
{
  if (!mooring_data)
  {
    Con::errorf("afxEA_Mooring::ea_start() -- missing or incompatible datablock.");
    return false;
  }
  return true;
}

bool afxEA_Mooring::ea_update(F32 dt)
{
  if (!obj)
  {
    if (datablock->use_ghost_as_cons_obj && datablock->effect_name != ST_NULLSTRING)
    {
      obj = new afxMooring(mooring_data->networking, 
                           choreographer->getChoreographerId(), 
                           datablock->effect_name);
    }
    else
    {
      obj = new afxMooring(mooring_data->networking, 0, ST_NULLSTRING);
    }

    obj->onNewDataBlock(mooring_data);
    if (!obj->registerObject())
    {
      delete obj;
      obj = 0;
      Con::errorf("afxEA_Mooring::ea_update() -- effect failed to register.");
      return false;
    }
    deleteNotify(obj);
  }

  if (obj)
  {
    obj->setTransform(updated_xfm);
  }

  return true;
}

void afxEA_Mooring::ea_finish(bool was_stopped)
{
}

void afxEA_Mooring::onDeleteNotify(SimObject* obj)
{
  if (this->obj == obj)
    obj = 0;

  Parent::onDeleteNotify(obj);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_MooringDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_MooringDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const;
  virtual bool  runsOnClient(const afxEffectWrapperData*) const;

  virtual afxEffectWrapper* create() const { return new afxEA_Mooring; }
};

afxEA_MooringDesc afxEA_MooringDesc::desc;

bool afxEA_MooringDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(afxMooringData) == typeid(*db));
}

bool afxEA_MooringDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->timing.lifetime < 0);
}

bool afxEA_MooringDesc::runsOnServer(const afxEffectWrapperData* ew) const
{
  U8 networking = ((const afxMooringData*)ew->effect_data)->networking;
  return ((networking & CLIENT_ONLY) == 0);
}

bool afxEA_MooringDesc::runsOnClient(const afxEffectWrapperData* ew) const
{ 
  U8 networking = ((const afxMooringData*)ew->effect_data)->networking;
  return ((networking & CLIENT_ONLY) != 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//