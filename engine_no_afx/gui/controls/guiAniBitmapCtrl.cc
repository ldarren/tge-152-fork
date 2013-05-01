#include <console/console.h>
#include <console/consoleTypes.h>
#include <dgl/dgl.h>
#include "./guiAniBitmapCtrl.h"

IMPLEMENT_CONOBJECT(GuiAniBitmapCtrl);

void GuiAniBitmapCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Animation");		// MM: Added Group Header.

   addField( "rowDiv",	TypeS32,		Offset(mRowDiv,		GuiAniBitmapCtrl));
   addField( "colDiv",	TypeS32,		Offset(mColDiv,		GuiAniBitmapCtrl));
   addField( "frameNum",TypeS32,		Offset(mFrameNum,	GuiAniBitmapCtrl));
   addField( "period",	TypeS32,		Offset(mPeriod,		GuiAniBitmapCtrl));
   addField( "loop",	TypeBool,		Offset(mLoop,		GuiAniBitmapCtrl));
 
   endGroup("Animation");		// MM: Added Group Footer.
}

GuiAniBitmapCtrl::GuiAniBitmapCtrl()
:	mRowDiv(1),
	mColDiv(1),
	mFrameNum(1),
	mPeriod(20),
	mLoop(false),
	mCurFrame(0),
	mLastTime(Sim::getCurrentTime())
{
}

void GuiAniBitmapCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	if (mTextureHandle)
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

		dglClearBitmapModulation();
		if(mWrap)
		{
			TextureObject* texture = (TextureObject *) mTextureHandle;
			RectI srcRegion;
			RectI dstRegion;
			float xdone = ((float)mBounds.extent.x/(float)texture->bitmapWidth)+1;
			float ydone = ((float)mBounds.extent.y/(float)texture->bitmapHeight)+1;

			int xshift = startPoint.x%texture->bitmapWidth;
			int yshift = startPoint.y%texture->bitmapHeight;
			for(int y = 0; y < ydone; ++y)
				for(int x = 0; x < xdone; ++x)
				{
					srcRegion.set(0,0,texture->bitmapWidth,texture->bitmapHeight);
					dstRegion.set( ((texture->bitmapWidth*x)+offset.x)-xshift,
					((texture->bitmapHeight*y)+offset.y)-yshift,
					texture->bitmapWidth,	
					texture->bitmapHeight);
					dglDrawBitmapStretchSR(texture,dstRegion, srcRegion, false);
				}
		}
		else
		{        
			int cWidth = mTextureHandle.getWidth() / mRowDiv;
			int cHeight = mTextureHandle.getHeight() / mColDiv;

			int cLeft = mCurFrame%mRowDiv*cWidth;
			int cTop  = mCurFrame/mRowDiv*cHeight;

			RectI rect(offset, mBounds.extent);
			RectI  subRegion(cLeft, cTop, cWidth, cHeight);

			dglDrawBitmapStretchSR(mTextureHandle, rect, subRegion,	0);
		}
	}

	if (mProfile->mBorder || !mTextureHandle)
	{
		RectI rect(offset.x, offset.y, mBounds.extent.x, mBounds.extent.y);
		dglDrawRect(rect, mProfile->mBorderColor);
	}

	renderChildControls(offset, updateRect);
}

void GuiAniBitmapCtrl::setFrame(S32  frame)
{
	if(frame >=  mFrameNum) return ;

	mLoop = false;
	mCurFrame = frame;
}
