//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUISLIDERCTRL_H_
#define _GUISLIDERCTRL_H_

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

class GuiSliderCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;

protected:
   Point2F mRange;
   U32  mTicks;
   F32  mValue;
   RectI   mThumb;
   Point2I mThumbSize;
   void updateThumb( F32 value, bool onWake = false );
   S32 mShiftPoint;
   S32 mShiftExtent;
   bool mDisplayValue;
   bool mDepressed;
   bool mMouseOver;
   bool mHasTexture;

   enum
   {
	   SliderLineLeft = 0,
	   SliderLineCenter,
	   SliderLineRight,
	   SliderButtonNormal,
	   SliderButtonHighlight,
	   NumBitmaps
   };
   	RectI *mBitmapBounds;

public:
   //creation methods
   DECLARE_CONOBJECT(GuiSliderCtrl);
   GuiSliderCtrl();
   static void initPersistFields();

   //Parental methods
   bool onWake();

   void onMouseDown(const GuiEvent &event);
   void onMouseDragged(const GuiEvent &event);
   void onMouseUp(const GuiEvent &);
   void onMouseLeave(const GuiEvent &);
   void onMouseEnter(const GuiEvent &);
   F32 getValue() { return mValue; }
   void setScriptValue(const char *val);

   void onRender(Point2I offset, const RectI &updateRect);
};

#endif
