
#ifndef _AFX_STATUS_LABEL_H_
#define _AFX_STATUS_LABEL_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#ifdef AFX_TGE_1_3
#include "gui/guiMLTextCtrl.h"
#else
#include "gui/controls/guiMLTextCtrl.h"
#endif

class afxStatusLabel : public GuiMLTextCtrl
{
private:
  typedef GuiMLTextCtrl Parent;

public:   
  /*C*/         afxStatusLabel();

  virtual void  onMouseDown(const GuiEvent &event);

  DECLARE_CONOBJECT(afxStatusLabel);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif //_AFX_STATUS_LABEL_H_
