
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"
#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/ce/afxAnimClip.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_AnimClip 

class afxEA_AnimClip : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxAnimClipData*  clip_data;
  bool              started;
  F32               anim_lifetime;
  U32               anim_tag;
  U32               lock_tag;

public:
  /*C*/             afxEA_AnimClip();

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_AnimClip::afxEA_AnimClip()
{
  clip_data = 0;
  started = false;
  anim_lifetime = 0;
  anim_tag = 0;
  lock_tag = 0;
}

void afxEA_AnimClip::ea_set_datablock(SimDataBlock* db)
{
  clip_data = dynamic_cast<afxAnimClipData*>(db);
}

bool afxEA_AnimClip::ea_start()
{
  if (!clip_data)
  {
    Con::errorf("afxEA_AnimClip::ea_start() -- missing or incompatible datablock.");
    return false;
  }

  afxConstraint* pos_constraint = getPosConstraint();
  if (full_lifetime == INFINITE_LIFETIME && pos_constraint != 0)
    anim_lifetime = pos_constraint->getAnimClipDuration(clip_data->clip_name);
  else 
    anim_lifetime = full_lifetime;

  anim_tag = 0;
  lock_tag = 0;

  return true;
}

bool afxEA_AnimClip::ea_update(F32 dt)
{
  afxConstraint* pos_constraint = getPosConstraint();
  if (!started && pos_constraint != 0)
  {
    bool go_for_it = true;

    if (pos_constraint->getDamageState() == ShapeBase::Enabled)
      go_for_it = !clip_data->ignore_enabled;
    else if (pos_constraint->getDamageState() == ShapeBase::Disabled)
      go_for_it = !clip_data->ignore_disabled;

    if (go_for_it && (clip_data->ignore_first_person || clip_data->ignore_third_person))
    {
      ShapeBase* shape = dynamic_cast<ShapeBase*>(pos_constraint->getObject());
      if (shape)
      {
        bool is_first_person = shape->isFirstPerson();
        if (clip_data->ignore_first_person && is_first_person)
          go_for_it = false;
        if (clip_data->ignore_third_person && !is_first_person)
          go_for_it = false;
      }
    }

    if (go_for_it)
    {
      F32 rate = clip_data->rate/prop_time_factor;
      F32 pos = mFmod(life_elapsed, anim_lifetime)/anim_lifetime;
      pos = mFmod(pos + clip_data->pos_offset, 1.0);
      if (clip_data->rate < 0) 
        pos = 1.0f - pos;
      anim_tag = pos_constraint->setAnimClip(clip_data->clip_name, pos, rate, clip_data->trans, 
                                             clip_data->is_death_anim);
      if (clip_data->lock_anim)
        lock_tag = pos_constraint->lockAnimation();
    }
    started = true;
  }

  return true;
}

void afxEA_AnimClip::ea_finish(bool was_stopped)
{
  afxConstraint* pos_constraint = getPosConstraint();
  if (pos_constraint && anim_tag != 0)
    pos_constraint->resetAnimation(anim_tag, clip_data->is_death_anim);
  if (pos_constraint && lock_tag != 0)
    pos_constraint->unlockAnimation(lock_tag);

  started = false;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_AnimClipDesc

class afxEA_AnimClipDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_AnimClipDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return true; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return true; }

  virtual afxEffectWrapper* create() const { return new afxEA_AnimClip; }
};

afxEA_AnimClipDesc afxEA_AnimClipDesc::desc;

bool afxEA_AnimClipDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(afxAnimClipData) == typeid(*db));
}

bool afxEA_AnimClipDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->timing.lifetime < 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//