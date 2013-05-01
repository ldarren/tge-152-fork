 
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "console/consoleTypes.h"
#include "console/simBase.h"
#include "game/staticShape.h"

#include "afxChoreographer.h"
#include "afxEffectVector.h"
#include "afxConstraint.h"
#include "afxEffectWrapper.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

void afxEffectVector::filter_client_server()
{
  if (empty())
    return;

  for (S32 i = 0; i < fx_v->size(); i++)
  {
    if ((*fx_v)[i]->datablock->runsHere(on_server))
      fx_v2->push_back((*fx_v)[i]);
    else
    {
      delete (*fx_v)[i];
      (*fx_v)[i] = 0;
    }
  }

  swap_vecs();

  fx_v2->clear();
}

void afxEffectVector::calc_fx_dur_and_afterlife()
{
  total_fx_dur = 0.0f;
  after_life = 0.0f;

  if (empty())
    return;

  for (S32 i = 0; i < fx_v->size(); i++)
  {
    afxEffectWrapper* ew = (*fx_v)[i];
    if (ew)
    {
      F32 ew_dur;
      if (ew->ew_timing.lifetime < 0)
      {
        if (phrase_dur > ew->ew_timing.delay)
          ew_dur = phrase_dur + ew->afterStopTime();
        else
          ew_dur = ew->ew_timing.delay + ew->afterStopTime();
      }
      else
        ew_dur = ew->ew_timing.delay + ew->ew_timing.lifetime + ew->ew_timing.fade_out_time;

      if (ew_dur > total_fx_dur)
        total_fx_dur = ew_dur;

      F32 after = ew->afterStopTime();
      if (after > after_life)
        after_life = after;
    }
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

afxEffectVector::afxEffectVector()
{
  fx_v = 0;
  fx_v2 = 0;
  active = false;
  on_server = false;
  total_fx_dur = 0;
  after_life = 0;
}

afxEffectVector::~afxEffectVector()
{
  stop(true);
  delete fx_v;
  delete fx_v2;
}

void afxEffectVector::ev_init(afxChoreographer* chor, afxEffectList& effects, bool on_server, 
                              bool will_stop, F32 time_factor, F32 phrase_dur)
{
  afxConstraintMgr* cons_mgr = chor->getConstraintMgr();
  this->on_server = on_server;
  this->phrase_dur = phrase_dur;

  fx_v = new Vector<afxEffectWrapper*>;
 
  for (S32 i = 0; i < effects.size(); i++)
  {
    afxEffectWrapperData* e = dynamic_cast<afxEffectWrapperData*>(effects[i]);      
    if (e != 0)
    {
      if ((will_stop || !e->requiresStop()) &&
          chor->testRanking(e->ranking_range.low, e->ranking_range.high) &&
          chor->testLevelOfDetail(e->lod_range.low, e->lod_range.high) &&
          e->testExecConditions(chor->getExecConditions()))
      {
        afxEffectWrapper* effect;
        effect = afxEffectWrapper::ew_create(chor, e, cons_mgr, time_factor);
        if (effect)
          fx_v->push_back(effect);
      }
    }
  }

  fx_v2 = new Vector<afxEffectWrapper*>(fx_v->size());
}

void afxEffectVector::start(F32 timestamp)
{
  if (empty())
    return;

  // At this point both client and server effects are in the list.
  // Timing adjustments are made during prestart().
  for (S32 i = 0; i < fx_v->size(); i++)
    (*fx_v)[i]->prestart();

  // duration and afterlife values are pre-calculated here
  calc_fx_dur_and_afterlife();

  // now we filter out client-only or server-only effects that
  // don't belong here,
  filter_client_server();

  active = true;
  for (S32 j = 0; j < fx_v->size(); j++)
    (*fx_v)[j]->start(timestamp);
}

void afxEffectVector::update(F32 dt)
{
  if (empty())
  {
    active = false;
    return;
  }

  for (int i = 0; i < fx_v->size(); i++)
  {
    (*fx_v)[i]->update(dt);

    if ((*fx_v)[i]->isDone())
    {
      // effect has ended, cleanup and delete
      (*fx_v)[i]->cleanup();
      delete (*fx_v)[i];
      (*fx_v)[i] = 0;
    }
    else
    {
      // effect is still going, so keep it around
      fx_v2->push_back((*fx_v)[i]);
    }
  }

  swap_vecs();

  fx_v2->clear();

  if (empty())
  {
    active = false;
    delete fx_v; fx_v =0;
    delete fx_v2; fx_v2 = 0;
  }
}

void afxEffectVector::stop(bool force_cleanup)
{
  if (empty())
  {
    active = false;
    return;
  }

  for (int i = 0; i < fx_v->size(); i++)
  {
    (*fx_v)[i]->stop();

    if (force_cleanup || (*fx_v)[i]->deleteWhenStopped())
    {
      // effect is over when stopped, cleanup and delete 
      (*fx_v)[i]->cleanup();
      delete (*fx_v)[i];
      (*fx_v)[i] = 0;
    }
    else
    {
      // effect needs to fadeout or something, so keep it around
      fx_v2->push_back((*fx_v)[i]);
    }
  }

  swap_vecs();

  fx_v2->clear();

  if (empty())
  {
    active = false;
    delete fx_v; fx_v =0;
    delete fx_v2; fx_v2 = 0;
  }
}

void afxEffectVector::interrupt()
{
  if (empty())
  {
    active = false;
    return;
  }

  for (int i = 0; i < fx_v->size(); i++)
  {
    (*fx_v)[i]->stop();
    (*fx_v)[i]->cleanup();
    delete (*fx_v)[i];
    (*fx_v)[i] = 0;
  }

  swap_vecs();

  fx_v2->clear();

  if (empty())
  {
    active = false;
    delete fx_v; fx_v =0;
    delete fx_v2; fx_v2 = 0;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//


