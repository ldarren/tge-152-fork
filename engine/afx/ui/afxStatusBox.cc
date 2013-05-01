
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "console/consoleTypes.h"
#include "afx/ui/afxStatusBox.h"

IMPLEMENT_CONOBJECT(afxStatusBox);

afxStatusBox::afxStatusBox()
{
}

void afxStatusBox::onMouseDown(const GuiEvent &event)
{
  Parent::onMouseDown(event);
  Con::executef(this, 1, "onMouseDown");
}

void afxStatusBox::onSleep()
{
  Parent::onSleep();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//