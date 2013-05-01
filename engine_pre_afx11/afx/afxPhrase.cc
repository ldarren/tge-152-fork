
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "afx/afxEffectVector.h"
#include "afx/afxPhrase.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxPhrase

void
afxPhrase::init_fx()
{
  fx->ev_init(init_chor, *init_fx_list, on_server, will_stop, init_time_factor, init_dur); 
}

//~~~~~~~~~~~~~~~~~~~~//

afxPhrase::afxPhrase(bool on_server, bool will_stop)
{
  this->on_server = on_server;
  this->will_stop = will_stop;

  init_fx_list = NULL;
  init_dur = 0.0f;
  init_chor = NULL;
  init_time_factor = 1.0f;

  fx = new afxEffectVector;
  fx2 = NULL;
  starttime = 0;
  dur = 0;

  n_loops = 1;
  loop_cnt = 1;

  extra_time = 0.0f;
  extra_stoptime = 0.0f;
}

afxPhrase::~afxPhrase()
{
  delete fx;
  delete fx2;
};

void 
afxPhrase::init(afxEffectList& fx_list, F32 dur, afxChoreographer* chor, F32 time_factor, 
                S32 n_loops, F32 extra_time)
{
  init_fx_list = &fx_list;
  init_dur = dur;
  init_chor = chor;
  init_time_factor = time_factor;

  this->n_loops = n_loops;
  this->extra_time = extra_time;
  this->dur = (init_dur < 0) ? init_dur : init_dur*init_time_factor;

  init_fx();
}

void
afxPhrase::start(F32 startstamp, F32 timestamp)
{
  starttime = startstamp;

  F32 loopstart = timestamp - startstamp;

  if (dur > 0 && loopstart > dur)
  {
    loop_cnt += (S32) (loopstart/dur);
    loopstart = mFmod(loopstart, dur);
  }

  if (!fx->empty())
    fx->start(loopstart);
}

void
afxPhrase::update(F32 dt, F32 timestamp)
{
  if (fx->isActive())
    fx->update(dt);

  if (fx2 && fx2->isActive())
    fx2->update(dt);

  if (extra_stoptime > 0 && timestamp > extra_stoptime)
  {
    stop(timestamp);
  }
}

void
afxPhrase::stop(F32 timestamp)
{
  if (extra_time > 0 && !(extra_stoptime > 0))
  {
    extra_stoptime = timestamp + extra_time;
    return;
  }

  if (fx->isActive())
    fx->stop();

  if (fx2 && fx2->isActive())
    fx2->stop();
}

bool
afxPhrase::expired(F32 timestamp)
{
  if (dur < 0)
    return false;

  return ((timestamp - starttime) > loop_cnt*dur);
}

F32
afxPhrase::elapsed(F32 timestamp)
{
  return (timestamp - starttime);
}

bool
afxPhrase::recycle(F32 timestamp)
{
  if (n_loops < 0 || loop_cnt < n_loops)
  {
    if (fx2)
      delete fx2;

    fx2 = fx;

    fx = new afxEffectVector;
    init_fx();

    if (fx2 && !fx2->empty())
      fx2->stop();

    if (!fx->empty())
      fx->start(0.0F);

    loop_cnt++;
    return true;
  }

  return false;
}

void
afxPhrase::interrupt(F32 timestamp)
{
  if (fx->isActive())
    fx->interrupt();

  if (fx2 && fx2->isActive())
    fx2->interrupt();
}

F32 afxPhrase::calcDoneTime()
{
  return starttime + fx->getTotalDur();
}

F32 afxPhrase::calcAfterLife()
{
  return fx->getAfterLife();
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//