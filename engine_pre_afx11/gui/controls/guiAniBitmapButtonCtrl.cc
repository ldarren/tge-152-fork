//-----------------------------------------------------------------------------
// Torque Game Engine
//
// Copyright (c) 2001 GarageGames.Com
//-----------------------------------------------------------------------------


//-------------------------------------
//
// Bitmap Button Contrl
// Set 'bitmap' comsole field to base name of bitmaps to use.  This control will
// append '_n' for normal
// append '_h' for hilighted
// append '_d' for depressed
//
// if bitmap cannot be found it will use the default bitmap to render.
//
// if the extent is set to (0,0) in the gui editor and appy hit, this control will
// set it's extent to be exactly the size of the normal bitmap (if present)
//


#include "console/console.h"
#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "platform/platformAudio.h"
#include "gui/core/guiCanvas.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gui/controls/GuiAniBitmapButtonCtrl.h"

IMPLEMENT_CONOBJECT(GuiAniBitmapButtonCtrl);

//-------------------------------------
GuiAniBitmapButtonCtrl::GuiAniBitmapButtonCtrl()
:	mRowDiv(1),
	mColDiv(1),
	mFrameNum(1),
	mPeriod(20),
	mLoop(false),
	mCurFrame(0),
	mLastTime(Sim::getCurrentTime())
{
   mBitmapName = StringTable->insert("");
   mBounds.extent.set(140, 30);
}

//-------------------------------------
void GuiAniBitmapButtonCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Animation");		// MM: Added Group Header.

   addField( "rowDiv",	TypeS32,		Offset(mRowDiv,		GuiAniBitmapButtonCtrl));
   addField( "colDiv",	TypeS32,		Offset(mColDiv,		GuiAniBitmapButtonCtrl));
   addField( "frameNum",TypeS32,		Offset(mFrameNum,	GuiAniBitmapButtonCtrl));
   addField( "period",	TypeS32,		Offset(mPeriod,		GuiAniBitmapButtonCtrl));
   addField( "loop",	TypeBool,		Offset(mLoop,		GuiAniBitmapButtonCtrl));

   endGroup("Animation");		// MM: Added Group Footer.
}

//-------------------------------------
void GuiAniBitmapButtonCtrl::onRender(Point2I offset, const RectI& updateRect)
{
	enum {
	  NORMAL,
	  HILIGHT,
	  DEPRESSED,
	  INACTIVE
	} state = NORMAL;

	if (mActive)
	{
	  if (mMouseOver) state = HILIGHT;
	  if (mDepressed || mStateOn) state = DEPRESSED;
	}
	else
	  state = INACTIVE;

	switch (state)
	{
	  case NORMAL:      renderButton(mTextureNormal, offset, updateRect); break;
	  case HILIGHT:     renderButton(mTextureHilight ? mTextureHilight : mTextureNormal, offset, updateRect); break;
	  case DEPRESSED:   renderButton(mTextureDepressed, offset, updateRect); break;
	  case INACTIVE:    renderButton(mTextureInactive ? mTextureInactive : mTextureNormal, offset, updateRect); break;
	}
}

//------------------------------------------------------------------------------

void GuiAniBitmapButtonCtrl::renderButton(TextureHandle &texture, Point2I &offset, const RectI& updateRect)
{
   if (texture)
   {
		if(mLoop)
		{
			S32 thisTime = Sim::getCurrentTime();

			S32 timeDelta = (thisTime - mLastTime)/10;

			if (timeDelta > mPeriod)
			{
				mLastTime = Sim::getCurrentTime();
				mCurFrame ++;
				if (mCurFrame >= mFrameNum)
				{
					mCurFrame = 0;
				}
			}
		}
	  
		RectI rect(offset, mBounds.extent);

		//
		int width ,height;
		width = texture.getWidth();
		height = texture.getHeight(); 
		int  sWidth,sHeight;
		sWidth = width /mRowDiv;
		sHeight = height / mColDiv;

		int sposx = mCurFrame % mRowDiv*sWidth;
		int sposy = mCurFrame / mRowDiv*sHeight;

		RectI  subRegion(sposx, sposy, sWidth , sHeight);
		dglClearBitmapModulation();
		dglDrawBitmapStretchSR(texture, rect, subRegion, 0);

		renderChildControls( offset, updateRect);
   }
   else
      Parent::onRender(offset, updateRect);
}
