
#ifndef _AFX_STATUS_BOX_H_
#define _AFX_STATUS_BOX_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#ifdef AFX_TGE_1_3
#include "gui/guiBitmapCtrl.h"
#else
#include "gui/controls/guiBitmapCtrl.h"
#endif

class afxStatusBox : public GuiBitmapCtrl
{
private:
  typedef GuiBitmapCtrl Parent;

public:   
  /*C*/         afxStatusBox();

  virtual void  onMouseDown(const GuiEvent &event);
  virtual void  onSleep();

  DECLARE_CONOBJECT(afxStatusBox);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif //_AFX_STATUS_BOX_H_
