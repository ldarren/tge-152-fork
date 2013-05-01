//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUIPROGRESSCTRL_H_
#define _GUIPROGRESSCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef _GUITEXTCTRL_H_
#include "gui/controls/guiTextCtrl.h"
#endif


class GuiProgressCtrl : public GuiTextCtrl
{
private:
   typedef GuiTextCtrl Parent;

   F32 mProgress;

public:
   //creation methods
   DECLARE_CONOBJECT(GuiProgressCtrl);
   GuiProgressCtrl();

   //console related methods
   virtual const char *getScriptValue();
   virtual void setScriptValue(const char *value);

   void onPreRender();
   void onRender(Point2I offset, const RectI &updateRect);
};

#endif
