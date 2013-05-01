
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"
#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/afxChoreographer.h"
#include "afx/ce/afxScriptEvent.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_ScriptEvent 

class afxEA_ScriptEvent : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxScriptEventData* script_data;
  bool              ran_script;

public:
  /*C*/             afxEA_ScriptEvent();

  virtual bool      isDone() { return ran_script; }

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_ScriptEvent::afxEA_ScriptEvent()
{
  script_data = 0;
  ran_script = false;
}

void afxEA_ScriptEvent::ea_set_datablock(SimDataBlock* db)
{
  script_data = dynamic_cast<afxScriptEventData*>(db);
}

bool afxEA_ScriptEvent::ea_start()
{
  if (!script_data)
  {
    Con::errorf("afxEA_ScriptEvent::ea_start() -- missing or incompatible datablock.");
    return false;
  }

  ran_script = (script_data->method_name == ST_NULLSTRING);

  return true;
}

bool afxEA_ScriptEvent::ea_update(F32 dt)
{
  if (!ran_script && choreographer != NULL)
  {
    afxConstraint* pos_constraint = getPosConstraint();
    choreographer->executeScriptEvent(script_data->method_name, pos_constraint, updated_xfm, 
                                      script_data->script_data);
    ran_script = true;
  }

  return true;
}

void afxEA_ScriptEvent::ea_finish(bool was_stopped)
{
  ran_script = false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_ScriptEventDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_ScriptEventDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return true; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return false; }

  virtual afxEffectWrapper* create() const { return new afxEA_ScriptEvent; }
};

afxEA_ScriptEventDesc afxEA_ScriptEventDesc::desc;

bool afxEA_ScriptEventDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(afxScriptEventData) == typeid(*db));
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//