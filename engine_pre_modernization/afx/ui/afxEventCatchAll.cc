
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "dgl/dgl.h"
#ifdef AFX_TGE_1_3
#include "gui/guiControl.h"
#include "gui/guiTSControl.h"
#else
#include "gui/core/guiControl.h"
#include "gui/core/guiTSControl.h"
#endif
#include "console/consoleTypes.h"
#include "sceneGraph/sceneGraph.h"
#include "game/shapeBase.h"
#include "game/gameConnection.h"

class afxEventCatchAll : public GuiControl 
{
  typedef GuiControl Parent;
  
public:
  /* C */         afxEventCatchAll() { }
  
  virtual void    onMouseUp(const GuiEvent&);
  virtual void    onMouseDown(const GuiEvent&);
  virtual void    onMouseMove(const GuiEvent&);
  virtual void    onMouseDragged(const GuiEvent&);
  virtual void    onMouseEnter(const GuiEvent&);
  virtual void    onMouseLeave(const GuiEvent&);
  
  virtual bool    onMouseWheelUp(const GuiEvent&);
  virtual bool    onMouseWheelDown(const GuiEvent&);
  
  virtual void    onRightMouseDown(const GuiEvent&);
  virtual void    onRightMouseUp(const GuiEvent&);
  virtual void    onRightMouseDragged(const GuiEvent&);
  
  DECLARE_CONOBJECT(afxEventCatchAll);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CONOBJECT(afxEventCatchAll);

void afxEventCatchAll::onMouseUp(const GuiEvent& evt)
{   
  GuiTSCtrl* parent = dynamic_cast<GuiTSCtrl*>(getParent());   
  if (parent) parent->onMouseUp(evt);
}

void afxEventCatchAll::onMouseDown(const GuiEvent& evt)
{   
  GuiTSCtrl* parent = dynamic_cast<GuiTSCtrl*>(getParent());   
  if (parent) parent->onMouseDown(evt);
}

void afxEventCatchAll::onMouseMove(const GuiEvent& evt)
{   
  GuiTSCtrl* parent = dynamic_cast<GuiTSCtrl*>(getParent());   
  if (parent) parent->onMouseMove(evt);
}

void afxEventCatchAll::onMouseDragged(const GuiEvent& evt)
{   
  GuiTSCtrl* parent = dynamic_cast<GuiTSCtrl*>(getParent());   
  if (parent) parent->onMouseDragged(evt);
}

void afxEventCatchAll::onMouseEnter(const GuiEvent& evt)
{   
  GuiTSCtrl* parent = dynamic_cast<GuiTSCtrl*>(getParent());   
  if (parent) parent->onMouseEnter(evt);
}

void afxEventCatchAll::onMouseLeave(const GuiEvent& evt)
{   
  GuiTSCtrl* parent = dynamic_cast<GuiTSCtrl*>(getParent());   
  if (parent) parent->onMouseLeave(evt);
}

bool afxEventCatchAll::onMouseWheelUp(const GuiEvent& evt)
{   
  GuiTSCtrl* parent = dynamic_cast<GuiTSCtrl*>(getParent());   
  return (parent) ? parent->onMouseWheelUp(evt) : false;
}

bool afxEventCatchAll::onMouseWheelDown(const GuiEvent& evt)
{   
  GuiTSCtrl* parent = dynamic_cast<GuiTSCtrl*>(getParent());   
  return (parent) ? parent->onMouseWheelDown(evt) : false;
}

void afxEventCatchAll::onRightMouseDown(const GuiEvent& evt)
{   
  GuiTSCtrl* parent = dynamic_cast<GuiTSCtrl*>(getParent());   
  if (parent) parent->onRightMouseDown(evt);
}

void afxEventCatchAll::onRightMouseUp(const GuiEvent& evt)
{   
  GuiTSCtrl* parent = dynamic_cast<GuiTSCtrl*>(getParent());   
  if (parent) parent->onRightMouseUp(evt);
}

void afxEventCatchAll::onRightMouseDragged(const GuiEvent& evt)
{   
  GuiTSCtrl* parent = dynamic_cast<GuiTSCtrl*>(getParent());   
  if (parent) parent->onRightMouseDragged(evt);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//


