
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "dgl/dgl.h"
#ifdef AFX_TGE_1_3
#include "gui/guiControl.h"
#else
#include "gui/core/guiControl.h"
#endif
#include "console/consoleTypes.h"
#include "game/gameConnection.h"
#include "game/shapeBase.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxStatusBar : public GuiControl
{
  typedef GuiControl Parent;

  ColorF            rgba_fill;

  F32               fraction;
  ShapeBase*        shape;
  bool              show_energy;
  bool              monitor_player;

public:
  /*C*/             afxStatusBar();

  virtual void      onRender(Point2I, const RectI&);

  void              setFraction(F32 frac);
  F32               getFraction() const { return fraction; }

  void              setShape(ShapeBase* s);
  void              clearShape() { setShape(NULL); }

  virtual bool      onWake();
  virtual void      onSleep();
  virtual void      onMouseDown(const GuiEvent &event);
  virtual void      onDeleteNotify(SimObject*);

  static void       initPersistFields();

  DECLARE_CONOBJECT(afxStatusBar);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CONOBJECT(afxStatusBar);

afxStatusBar::afxStatusBar()
{
  rgba_fill.set(0.0f, 1.0f, 1.0f, 1.0f);

  fraction = 1.0f;
  shape = 0;
  show_energy = false;
  monitor_player = false;
}

void afxStatusBar::setFraction(F32 frac)
{
  fraction = mClampF(frac, 0.0f, 1.0f);
}

void afxStatusBar::setShape(ShapeBase* s) 
{ 
  if (shape)
    clearNotify(shape);
  shape = s;
  if (shape)
    deleteNotify(shape);
}

void afxStatusBar::onDeleteNotify(SimObject* obj)
{
  if (shape == (ShapeBase*)obj)
  {
    shape = NULL;
    return;
  }

  Parent::onDeleteNotify(obj);
}

bool afxStatusBar::onWake()
{
  if (!Parent::onWake())
    return false;

  return true;
}

void afxStatusBar::onSleep()
{
  //clearShape();
  Parent::onSleep();
}

// STATIC 
void afxStatusBar::initPersistFields()
{
  Parent::initPersistFields();

  addField("fillColor",      TypeColorF, Offset(rgba_fill, afxStatusBar));
  addField("displayEnergy",  TypeBool,   Offset(show_energy, afxStatusBar));
  addField("monitorPlayer",  TypeBool,   Offset(monitor_player, afxStatusBar));
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//


void afxStatusBar::onRender(Point2I offset, const RectI &updateRect)
{
  if (!shape)
    return;

  if (shape->getDamageState() != ShapeBase::Enabled)
    fraction = 0.0f;
  else
    fraction = (show_energy) ? shape->getEnergyValue() : (1.0f - shape->getDamageValue());

  // set alpha value for the fill area
  rgba_fill.alpha = 1.0f;

  // calculate the rectangle dimensions
  RectI rect(updateRect);
  rect.extent.x = (S32)(rect.extent.x*fraction);

  // draw the filled part of bar
  dglDrawRectFill(rect, rgba_fill);
}

void afxStatusBar::onMouseDown(const GuiEvent &event)
{
  GuiControl *parent = getParent();
  if (parent)
    parent->onMouseDown(event);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

ConsoleMethod(afxStatusBar, setProgress, void, 3, 3, "setProgress(percent_done)")
{
  object->setFraction(dAtof(argv[2]));
}

ConsoleMethod(afxStatusBar, setShape, void, 3, 3, "setShape(shape)")
{
  object->setShape(dynamic_cast<ShapeBase*>(Sim::findObject(argv[2])));
}

ConsoleMethod(afxStatusBar, clearShape, void, 2, 2, "clearShape()")
{
  object->clearShape();
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
