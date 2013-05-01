
#ifndef _AFX_TS_CTRLL_H_
#define _AFX_TS_CTRLL_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "dgl/dgl.h"
#include "game/game.h"
#ifdef AFX_TGE_1_3
#include "gui/guiTSControl.h"
#else
#include "gui/core/guiTSControl.h"
#endif

class GameBase;
class afxSpellBook;

//----------------------------------------------------------------------------
class afxTSCtrl : public GuiTSCtrl
{
private:
  typedef GuiTSCtrl   Parent;

  Point3F             mMouse3DVec;
  Point3F             mMouse3DPos;

  U32                 mouse_dn_timestamp;
  afxSpellBook*       spellbook;

public:
  /*C*/               afxTSCtrl();

  virtual bool        processCameraQuery(CameraQuery *query);
  virtual void        renderWorld(const RectI &updateRect);
  virtual void        onRender(Point2I offset, const RectI &updateRect);

  virtual void        onMouseUp(const GuiEvent&);
  virtual void        onMouseDown(const GuiEvent&);
  virtual void        onMouseMove(const GuiEvent&);
  virtual void        onMouseDragged(const GuiEvent&);
  virtual void        onMouseEnter(const GuiEvent&);
  virtual void        onMouseLeave(const GuiEvent&);

  virtual bool        onMouseWheelUp(const GuiEvent&);
  virtual bool        onMouseWheelDown(const GuiEvent&);

  virtual void        onRightMouseDown(const GuiEvent&);
  virtual void        onRightMouseUp(const GuiEvent&);
  virtual void        onRightMouseDragged(const GuiEvent&);

  Point3F             getMouse3DVec() {return mMouse3DVec;};   
  Point3F             getMouse3DPos() {return mMouse3DPos;};

  void                setSpellBook(afxSpellBook* book);

  void				  raycastMouse(const GuiEvent &evt);// DARREN MOD: mouse click movement

  DECLARE_CONOBJECT(afxTSCtrl);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_TS_CTRLL_H_
