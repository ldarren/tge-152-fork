
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "console/consoleTypes.h"
#include "afx/ui/afxStatusLabel.h"

IMPLEMENT_CONOBJECT(afxStatusLabel);

afxStatusLabel::afxStatusLabel()
{
}

void afxStatusLabel::onMouseDown(const GuiEvent &event)
{
  GuiControl *parent = getParent();
  if (parent)
    parent->onMouseDown(event);
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//