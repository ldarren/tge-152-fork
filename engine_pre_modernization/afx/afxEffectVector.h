
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _AFX_EFFECT_VECTOR_H_
#define _AFX_EFFECT_VECTOR_H_

#include "afx/afxEffectWrapper.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEffectVector

class afxConstraintMgr;
class afxEffectWrapper;
class afxChoreographer;
class afxEffectWrapperData;

class afxEffectVector
{
  Vector<afxEffectWrapper*>*  fx_v;
  Vector<afxEffectWrapper*>*  fx_v2;

  bool          active;
  bool          on_server;
  F32           phrase_dur;
  F32           total_fx_dur;
  F32           after_life;

  void          swap_vecs();
  void          filter_client_server();
  void          calc_fx_dur_and_afterlife();

public:
  /*C*/         afxEffectVector();
  /*D*/         ~afxEffectVector();

  void          ev_init(afxChoreographer*, afxEffectList&, bool on_server, bool will_stop, 
                        F32 time_factor, F32 phrase_dur);

  void          start(F32 timestamp);
  void          update(F32 dt);
  void          stop(bool force_cleanup=false);
  void          interrupt();
  bool          empty() { return (!fx_v || fx_v->empty()); }
  bool          isActive() { return active; }
  S32           count() { return (fx_v) ? fx_v->size() : 0; }

  F32           getTotalDur() { return total_fx_dur; }
  F32           getAfterLife() { return after_life; }

  Vector<afxEffectWrapper*>* getFX() { return fx_v; }
};

inline void afxEffectVector::swap_vecs()
{
  Vector<afxEffectWrapper*>* tmp = fx_v;
  fx_v = fx_v2;
  fx_v2 = tmp;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_EFFECT_VECTOR_H_
