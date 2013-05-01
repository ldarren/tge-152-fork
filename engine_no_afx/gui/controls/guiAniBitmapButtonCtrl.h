//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GUIANIBITMAPBUTTON_H_
#define _GUIANIBITMAPBUTTON_H_

#ifndef _GUIBITMAPBUTTON_H_
#include "./guiBitmapButtonCtrl.h"
#endif
#ifndef _GTEXMANAGER_H_
#include "dgl/gTexManager.h"
#endif

///-------------------------------------
/// Bitmap Button Contrl
/// Set 'bitmap' comsole field to base name of bitmaps to use.  This control will
/// append '_n' for normal
/// append '_h' for hilighted
/// append '_d' for depressed
///
/// if bitmap cannot be found it will use the default bitmap to render.
///
/// if the extent is set to (0,0) in the gui editor and apply hit, this control will
/// set it's extent to be exactly the size of the normal bitmap (if present)
///
class GuiAniBitmapButtonCtrl : public GuiBitmapButtonCtrl
{
private:
   typedef GuiBitmapButtonCtrl Parent;

protected:
	S32 mCurFrame;
	S32 mLastTime;
	bool mLoop;

	S32	mRowDiv, mColDiv;
	S32	mFrameNum;
	S32	mPeriod;

   void renderButton(TextureHandle &texture, Point2I &offset, const RectI& updateRect);

public:
   DECLARE_CONOBJECT(GuiAniBitmapButtonCtrl);
   GuiAniBitmapButtonCtrl();

   static void initPersistFields();

   void setBitmap(const char *name);

   void onRender(Point2I offset, const RectI &updateRect);
};

#endif //_GUIANIBITMAPBUTTON_H_
