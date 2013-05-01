//-----------------------------------------------------------------------------
// Torque Game Engine
// 
// GuiTweenBitmapCtrl - See GuiTweenBitmapCtrl.cc for description.
//
// Jesse Benson, Filament Games http://www.garagegames.com/index.php?sec=mg&mod=resource&page=view&qid=13031
//-----------------------------------------------------------------------------


#ifndef _GUITWEENBITMAPCTRL_H
#define _GUITWEENBITMAPCTRL_H
#endif

#ifndef _DGL_H_
#include "dgl/dgl.h"
#endif

#ifndef _CONSOLE_H_
#include "console/console.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _MPOINT_H_
#include "math/mPoint.h"
#endif

#ifndef _MRECT_H_
#include "math/mRect.h"
#endif

#ifndef _GUIBITMAPCTRL_H_
#include "gui/controls/guiBitmapCtrl.h"
#endif

class GuiTweenBitmapCtrl : public GuiBitmapCtrl
{
   typedef GuiBitmapCtrl Parent;

protected:
   RectI mStartBounds;
   RectI mTweenBounds;
   F32 mStartAlpha; // Starting alpha value
   F32 mAlpha; // Current alpha value
   F32 mTweenAlpha; // Tween alpha value
   U32 mStartTime;
   bool mDone;
   bool mCenter; // Is the Tween Position specified as a center point?
   F32 mDistance; // DARREN MOD: accumulation of mSpeed, when reach 1 mean completed
   bool mStartSpeedMode;

public:
   U32 mWaitTime;
   U32 mTweenTime;
   F32 mSpeed; // DARREN MOD animation speed, if speed set larger than 0, mTweenTime will be ignore

   GuiTweenBitmapCtrl();
   DECLARE_CONOBJECT(GuiTweenBitmapCtrl);

   static void initPersistFields();
   void onPreRender();
   void onRender(Point2I offset, const RectI &updateRect);

   // Set the desired tween variables
   void setTweenPosition(Point2I position, bool center = false);
   void setTweenExtent(Point2I extent);
   void setTweenRect(RectI rect, bool center = false);
   void setTweenAlpha(F32 alpha); // Desired end alpha (between 0.0 and 1.0)
   void setTween(RectI rect, F32 alpha, bool center = false);
   void setTweenZoom(RectI rect); // Zoom TweenCtrl in to this Rect

   // Tween time variables must be set through methods to assert the Tween is done
   void setWaitTime(U32 time);
   void setTweenTime(U32 time);

   void startTween(); // Begin the tweening process
   void endTween(); // Immediately skip to the end of the tween
   bool isDone() { return mDone; }
};
