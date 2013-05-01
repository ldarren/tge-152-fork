
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"
#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/ce/afxAnimLock.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_AnimLock 

class afxEA_AnimLock : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  bool              started;
  U32               lock_tag;

public:
  /*C*/             afxEA_AnimLock();

  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_AnimLock::afxEA_AnimLock()
{
  started = false;
  lock_tag = 0;
}

bool afxEA_AnimLock::ea_update(F32 dt)
{
  afxConstraint* pos_constraint = getPosConstraint();
  if (!started && pos_constraint != 0)
  {
    lock_tag = pos_constraint->lockAnimation();
    started = true;
  }

  return true;
}

void afxEA_AnimLock::ea_finish(bool was_stopped)
{
  afxConstraint* pos_constraint = getPosConstraint();
  if (pos_constraint && lock_tag != 0)
  {
    pos_constraint->unlockAnimation(lock_tag);
  }

  started = false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_AnimLockDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_AnimLockDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return true; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return true; }

  virtual afxEffectWrapper* create() const { return new afxEA_AnimLock; }
};

afxEA_AnimLockDesc afxEA_AnimLockDesc::desc;

bool afxEA_AnimLockDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(afxAnimLockData) == typeid(*db));
}

bool afxEA_AnimLockDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->timing.lifetime < 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//