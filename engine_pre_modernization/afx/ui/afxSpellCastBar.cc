
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

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxSpellCastBar : public GuiControl
{
  typedef GuiControl Parent;

  bool              want_border;
  bool              want_background;
  ColorF            rgba_background;
  ColorF            rgba_border;
  ColorF            rgba_fill;

  F32               fraction;

public:
  /*C*/             afxSpellCastBar();

  virtual void      onRender(Point2I, const RectI&);

  void              setFraction(F32 frac);
  F32               getFraction() const { return fraction; }

  static void       initPersistFields();

  DECLARE_CONOBJECT(afxSpellCastBar);
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

IMPLEMENT_CONOBJECT(afxSpellCastBar);

afxSpellCastBar::afxSpellCastBar()
{
  want_border = true;
  want_background = true;
  rgba_background.set(0.0f, 0.0f, 0.0f, 0.5f);
  rgba_border.set(0.5f, 0.5f, 0.5f, 1.0f);
  rgba_fill.set(0.0f, 1.0f, 1.0f, 1.0f);

  fraction = 0.5f;
}

void afxSpellCastBar::setFraction(F32 frac)
{
  fraction = mClampF(frac, 0.0f, 1.0f);
}

// STATIC 
void afxSpellCastBar::initPersistFields()
{
  Parent::initPersistFields();

  addGroup("Colors");
  addField( "backgroundColor",  TypeColorF, Offset(rgba_background, afxSpellCastBar));
  addField( "borderColor",      TypeColorF, Offset(rgba_border, afxSpellCastBar));
  addField( "fillColor",        TypeColorF, Offset(rgba_fill, afxSpellCastBar));
  endGroup("Colors");
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

void afxSpellCastBar::onRender(Point2I offset, const RectI &updateRect)
{
  // draw the background
  if (want_background)
    dglDrawRectFill(updateRect, rgba_background);

  // set alpha value for the fill area
  rgba_fill.alpha = 1.0f;

  // calculate the rectangle dimensions
  RectI rect(updateRect);
  rect.extent.x = (S32)(rect.extent.x * fraction);

  // draw the filled part of bar
  dglDrawRectFill(rect, rgba_fill);

  // draw the border
  if (want_border)
    dglDrawRect(updateRect, rgba_border);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

ConsoleMethod(afxSpellCastBar, setProgress, void, 3, 3, "setProgress(percent_done)")
{
  object->setFraction(dAtof(argv[2]));
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
