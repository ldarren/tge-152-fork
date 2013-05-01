
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _AFX_PHRASE_H_
#define _AFX_PHRASE_H_

#include "afxEffectVector.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxPhrase

class afxChoreographer;
class afxConstraintMgr;
class afxEffectVector;

class afxPhrase
{
protected:
  afxEffectList*    init_fx_list;
  F32               init_dur;
  afxChoreographer* init_chor;
  F32               init_time_factor;
  F32               extra_time;

  afxEffectVector*  fx;
  afxEffectVector*  fx2;

  bool              on_server;
  bool              will_stop;

  F32               starttime;
  F32               dur;
  S32               n_loops;
  S32               loop_cnt;
  F32               extra_stoptime;

  void              init_fx();

public:
  /*C*/             afxPhrase(bool on_server, bool will_stop);
  virtual           ~afxPhrase();

  virtual void      init(afxEffectList&, F32 dur, afxChoreographer*, F32 time_factor, 
                         S32 n_loops, F32 extra_time=0.0f);

  virtual void      start(F32 startstamp, F32 timestamp);
  virtual void      update(F32 dt, F32 timestamp);
  virtual void      stop(F32 timestamp);
  virtual void      interrupt(F32 timestamp);
  virtual bool      expired(F32 timestamp);
  virtual bool      recycle(F32 timestamp);
  virtual F32       elapsed(F32 timestamp);

  bool              isEmpty() { return fx->empty(); }
  bool              isInfinite() { return (init_dur < 0); }
  F32               calcDoneTime();
  F32               calcAfterLife();
  bool              willStop() { return will_stop; }
  bool              onServer() { return on_server; }
  S32               count() { return fx->count(); }
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_PHRASE_H_
