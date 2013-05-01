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
	mPeriod(2000),
	mLoop(false),
	mCurFrame(0),
	mLastTime(Sim::getCurrentTime()),
	mState(PLAY)
{
}

void GuiAniBitmapCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	if (mTextureHandle)
	{	
		if (mState == PLAY || mState == REVERSE)
		{
			S32 thisTime = Sim::getCurrentTime();

			S32 timeDelta = thisTime - mLastTime;

			if (timeDelta > mPeriod)
			{
				mLastTime = Sim::getCurrentTime();
				if (mState == PLAY)
				{
					mCurFrame ++;
					if (mCurFrame >= mFrameNum)
					{
						mCurFrame = 0;
						if (!mLoop) 
						{
							// hold at last frame
							mCurFrame = mFrameNum - 1;
							mState = STOP;
							Con::executef(this, 1, "onStop");
							if (!mTextureHandle) return; // check mTextureHandle again, onStop may remove bitmap
						}
					}
				}
				else // reverse
				{
					mCurFrame --;
					if (mCurFrame < 0)
					{
						mCurFrame = mFrameNum - 1;
						if (!mLoop) 
						{
							// hold at last frame
							mCurFrame = 0;
							mState = STOP;
							Con::executef(this, 1, "onStop");
							if (!mTextureHandle) return; // check mTextureHandle again, onStop may remove bitmap
						}
					}
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
			int cWidth = mTextureHandle.getWidth() / mColDiv;
			int cHeight = mTextureHandle.getHeight() / mRowDiv;

			int cLeft = mCurFrame%mColDiv*cWidth;
			int cTop  = mCurFrame/mColDiv*cHeight;

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

void GuiAniBitmapCtrl::gotoFrame(S32  frame)
{
	if(frame >=  mFrameNum) return ;

	mState = PAUSE;
	mCurFrame = frame;
}

void GuiAniBitmapCtrl::play(S32 dir)
{
	if (dir == 1) mState = PLAY;
	else mState = REVERSE;
}

void GuiAniBitmapCtrl::stop()
{
	mState = STOP;
	mCurFrame = 0;
}

void GuiAniBitmapCtrl::pause()
{
	mState = PAUSE;
}

bool GuiAniBitmapCtrl::resetBitmap(const char *name, S32 dir)
{
	play(dir);
	if (dir == 1) mCurFrame = -1; // onRender will increase by one
	else mCurFrame = mFrameNum;
	mTextureHandle.set(name);
	//setBitmap(name);
	return mTextureHandle.isValid();
}

ConsoleMethod(GuiAniBitmapCtrl, play, void, 2,3,"start playing animation")
{
	object->play(argc==3 ? dAtoi(argv[2]) : 1);
}

ConsoleMethod( GuiAniBitmapCtrl, pause, void, 2, 2, "stop animation playback, stay at current frame")
{
	object->pause();
}

ConsoleMethod( GuiAniBitmapCtrl, stop, void, 2, 2, "stop animation playback, reset frame to 1st frame")
{
   object->stop();
}

ConsoleMethod( GuiAniBitmapCtrl, gotoFrame, void, 3, 3, "display specified frame as static image")
{
   object->gotoFrame(dAtoi(argv[2]));
}

ConsoleMethod( GuiAniBitmapCtrl, resetBitmap, bool, 3, 4, "display new bitmap and reset frame to 0")
{
   return object->resetBitmap(argv[2], argc==4 ? dAtoi(argv[3]) : 1);
}
