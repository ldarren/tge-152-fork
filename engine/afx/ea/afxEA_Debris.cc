
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"

#include "game/debris.h"

#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_Debris 

class afxEA_Debris : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  DebrisData*       debris_data;
  Debris*           debris;
  bool              exploded;
  bool              debris_done;

public:
  /*C*/             afxEA_Debris();
  /*D*/             ~afxEA_Debris();

  virtual bool      isDone();

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);

  virtual void      onDeleteNotify(SimObject*);
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_Debris::afxEA_Debris()
{
  debris = 0;
  exploded = false;
  debris_done = false;
}

afxEA_Debris::~afxEA_Debris()
{
  if (debris)
    clearNotify(debris);
}

bool afxEA_Debris::isDone()
{
  return (datablock->use_as_cons_obj) ? debris_done : exploded;
}

void afxEA_Debris::ea_set_datablock(SimDataBlock* db)
{
  debris_data = dynamic_cast<DebrisData*>(db);
}

bool afxEA_Debris::ea_start()
{
  if (!debris_data)
  {
    Con::errorf("afxEA_Debris::ea_start() -- missing or incompatible datablock.");
    return false;
  }

  debris = new Debris();
  debris->onNewDataBlock(debris_data);

  return true;
}

bool afxEA_Debris::ea_update(F32 dt)
{
  if (exploded && debris)
  {
    if (in_scope)
    {
      updated_xfm = debris->getRenderTransform();
      updated_xfm.getColumn(3, &updated_pos);
    }
  }

  if (!exploded && debris)
  {
    if (in_scope)
    {     
      Point3F dir_vec(0,1,0);
      updated_xfm.mulV(dir_vec);

      debris->init(updated_pos, dir_vec);
      if (!debris->registerObject())
      {
        delete debris;
        debris = 0;
        Con::errorf("afxEA_Debris::ea_update() -- effect failed to register.");
        return false;
      }
      deleteNotify(debris);
    }
    exploded = true;
  }

  return true;
}

void afxEA_Debris::ea_finish(bool was_stopped)
{
  if (debris)
  {
    clearNotify(debris);
    debris = 0;
  }
  exploded = false;
}

void afxEA_Debris::onDeleteNotify(SimObject* obj)
{
  // debris deleted?
  Debris* del_debris = dynamic_cast<Debris*>(obj);
  if (del_debris == debris)
  {
    debris = NULL;
    debris_done = true;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_DebrisDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_DebrisDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;// { return false; }
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return true; }

  virtual afxEffectWrapper* create() const { return new afxEA_Debris; }
};

afxEA_DebrisDesc afxEA_DebrisDesc::desc;

bool afxEA_DebrisDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(DebrisData) == typeid(*db));
}

bool afxEA_DebrisDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->use_as_cons_obj && ew->timing.lifetime < 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//