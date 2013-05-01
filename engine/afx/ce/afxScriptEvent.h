
#ifndef _AFX_SCRIPT_EVENT_H_
#define _AFX_SCRIPT_EVENT_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxScriptEventData

struct afxScriptEventData : public GameBaseData
{
  typedef GameBaseData Parent;

public:
  StringTableEntry      method_name;
  StringTableEntry      script_data;

public:
  /*C*/                 afxScriptEventData();

  void                  packData(BitStream* stream);
  void                  unpackData(BitStream* stream);

  static void           initPersistFields();

  DECLARE_CONOBJECT(afxScriptEventData);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_SCRIPT_EVENT_H_
