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

protected:
	// DARREN MOD: smooth transitio and color change
	F32 mTarget;
	F32 mStartPoint;
	U32 mTweenTime;
	U32 mStartTime;

	ColorI mStartColor;
	ColorI mEndColor;
	ColorI mCurrColor;

public:
   //creation methods
   DECLARE_CONOBJECT(GuiProgressCtrl);
   GuiProgressCtrl();

   // DARREN MOD
   static void initPersistFields();
   void setTargetVal(F32 t, U32 time);
   //console related methods
   virtual const char *getScriptValue();
   virtual void setScriptValue(const char *value);

   void onPreRender();
   void onRender(Point2I offset, const RectI &updateRect);
};

#endif
