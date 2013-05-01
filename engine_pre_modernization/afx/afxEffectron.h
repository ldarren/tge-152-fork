
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _AFX_COMPOSITE_EFFECT_H_
#define _AFX_COMPOSITE_EFFECT_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "console/typeValidators.h"

#include "afxChoreographer.h"
#include "afxEffectWrapper.h"
#include "afxPhrase.h"

class afxChoreographerData;
class afxEffectWrapperData;

class afxEffectronData : public afxChoreographerData
{
  typedef afxChoreographerData Parent;

  class ewValidator : public TypeValidator
  {
    U32 id;
  public:
    ewValidator(U32 id) { this->id = id; }
    void validateType(SimObject *object, void *typePtr);
  };

  bool          do_id_convert;

public:
  afxEffectList fx_list;
  F32           duration;
  S32           n_loops;

  afxEffectWrapperData* dummy_fx_entry;
  
private:
  void          pack_fx(BitStream* stream, const afxEffectList& fx, bool packed);
  void          unpack_fx(BitStream* stream, afxEffectList& fx);
  void          gather_cons_defs(Vector<afxConstraintDef>& defs, afxEffectList& fx);

public:
  /*C*/         afxEffectronData();

  void          reset();

  virtual bool  onAdd();
  virtual void  packData(BitStream*);
  virtual void  unpackData(BitStream*);
  virtual bool  preload(bool server, char errorBuffer[256]);

  void          gatherConstraintDefs(Vector<afxConstraintDef>&); 

  static void   initPersistFields();

  DECLARE_CONOBJECT(afxEffectronData);
};
DECLARE_CONSOLETYPE(afxEffectronData);

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEffectron

class afxEffectron : public afxChoreographer
{
  typedef afxChoreographer Parent;

public:
  enum MaskBits 
  {
    StateEventMask    = Parent::NextFreeMask << 0,
    SyncEventMask     = Parent::NextFreeMask << 1,
    NextFreeMask      = Parent::NextFreeMask << 2
  };

  enum
  {
    NULL_EVENT,
    ACTIVATE_EVENT,
    SHUTDOWN_EVENT,
    DEACTIVATE_EVENT,
    INTERRUPT_EVENT
  };

  enum
  {
    INACTIVE_STATE,
    ACTIVE_STATE,
    CLEANUP_STATE,
    DONE_STATE,
    LATE_STATE
  };

  enum {
    MARK_ACTIVATE   = BIT(0),
    MARK_SHUTDOWN   = BIT(1),
    MARK_DEACTIVATE = BIT(2),
    MARK_INTERRUPT  = BIT(3),
  };

  class ObjectDeleteEvent : public SimEvent
  {
  public:
    void process(SimObject *obj) { if (obj) obj->deleteObject(); }
  };

private:
  static StringTableEntry  CAMERA_CONS;
  static StringTableEntry  LISTENER_CONS;

private:
  afxEffectronData*  datablock;

  bool          constraints_initialized;

  U8            effect_state;
  F32           effect_elapsed;
  U8            marks_mask;
  afxConstraintID listener_cons_id;
  afxPhrase*    active_phrase;
  F32           time_factor;

private:
  void          init();
  bool          state_expired();
  void          init_constraints();
  void          setup_active_fx();
  bool          cleanup_over();

public:
  /*C*/         afxEffectron();
  /*D*/         ~afxEffectron();

    // STANDARD OVERLOADED METHODS //
  virtual bool  onNewDataBlock(GameBaseData* dptr);
  virtual void  processTick(const Move*);
  virtual void  advanceTime(F32 dt);
  virtual bool  onAdd();
  virtual U32   packUpdate(NetConnection*, U32, BitStream*);
  virtual void  unpackUpdate(NetConnection*, BitStream*);

  virtual void  sync_with_clients();
  void          finish_startup();

  DECLARE_CONOBJECT(afxEffectron);

private:
  void          process_server();
  //
  void          change_state_s(U8 pending_state);
  //
  void          enter_active_state_s();
  void          leave_active_state_s();
  void          enter_cleanup_state_s();
  void          enter_done_state_s();

private:
  void          process_client(F32 dt);
  //
  void          change_state_c(U8 pending_state);
  //
  void          enter_active_state_c(F32 starttime);
  void          leave_active_state_c();

  void          sync_client(U16 marks, U8 state, F32 elapsed);

public:
  void          postEvent(U8 event);
  void          setTimeFactor(F32 f) { time_factor = (f > 0) ? f : 1.0f; }
  F32           getTimeFactor() { return time_factor; }

public:
  static afxEffectron*  start_effect(afxEffectronData*);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
#endif // _AFX_COMPOSITE_EFFECT_H_
