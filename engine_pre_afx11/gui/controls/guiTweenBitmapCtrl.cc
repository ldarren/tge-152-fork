//-----------------------------------------------------------------------------
// Torque Game Engine
// 
// GuiTweenBitmapCtrl - This is a Tween bitmap control similar to the tweening
//   effect in Flash.  The desired end position, extent, and alpha can be set.
//   The end position can be specified as either a top-left or center point.
//
// The Tween Control will begin tweening when startTween() is called on the
//   control.  The Tween Control will first wait mWaitTime milliseconds and
//   then proceed to tween the position, extent, and alpha in mTweenTime 
//   milliseconds.
//
// The Tween Control will ignore any tween requests until mDone = true.
//  Tween requests include setting the desired end position, extent, alpha,
//  or a request to startTween.
//
// Jesse Benson, Filament Games
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleTypes.h"
#include "dgl/dgl.h"
#include "math/mPoint.h"
#include "math/mRect.h"
#include "guiBitmapCtrl.h"
#include "guiTweenBitmapCtrl.h"

IMPLEMENT_CONOBJECT(GuiTweenBitmapCtrl);

GuiTweenBitmapCtrl::GuiTweenBitmapCtrl()
{
	mWaitTime       = 0;
	mTweenTime      = 0;
	mStartTime      = 0;
	mStartAlpha     = 1.0f;
	mTweenAlpha     = 1.0f;
	mStartBounds    = mBounds;
	mTweenBounds    = mBounds;
	mCenter         = false;
	mDone           = true;
	mBitmapFade     = true;
	// DARREN MOD
	mSpeed			= 0;
	mDistance		= 0;
	mStartSpeedMode	= false;
}

void GuiTweenBitmapCtrl::initPersistFields()
{
	Parent::initPersistFields();
	
	addGroup("Transition");
	addField("speed",               TypeF32, Offset(mSpeed, GuiTweenBitmapCtrl));
	addField("tweentime",           TypeS32, Offset(mTweenTime, GuiTweenBitmapCtrl));
	addField("waittime",            TypeS32, Offset(mWaitTime, GuiTweenBitmapCtrl));
	endGroup("Transition");	
}

void GuiTweenBitmapCtrl::onPreRender()
{
	Parent::onPreRender();
	setUpdate();
}

void GuiTweenBitmapCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	U32 elapsed = Platform::getRealMilliseconds() - mStartTime;

	if (elapsed < mWaitTime)
	{
		// in wait time, No change
		mAlpha = mStartAlpha;
	}
	else if ((mStartSpeedMode && mDistance < 1.0f) || (!mStartSpeedMode && elapsed < mWaitTime + mTweenTime))
	{
		// Do motion, extent, and alpha tweening
		F32 percent;
		if (mStartSpeedMode) 
		{
			mDistance += mSpeed;
			percent = mDistance;
		}
		else percent = F32(elapsed - mWaitTime) / F32(mTweenTime);
		mBounds.point.x = (S32) (F32(mStartBounds.point.x) + F32(mTweenBounds.point.x) * percent);
		mBounds.point.y = (S32) (F32(mStartBounds.point.y) + F32(mTweenBounds.point.y) * percent);
		mBounds.extent.x = (S32) (F32(mStartBounds.extent.x) + F32(mTweenBounds.extent.x) * percent);
		mBounds.extent.y = (S32) (F32(mStartBounds.extent.y) + F32(mTweenBounds.extent.y) * percent);
		mAlpha = mStartAlpha + mTweenAlpha * percent;
		resize(mBounds.point, mBounds.extent);
	}
	else if(!mDone)
	{
		// Done state
		mBounds.point = mStartBounds.point + mTweenBounds.point;
		mBounds.extent = mStartBounds.extent + mTweenBounds.extent;
		mTweenBounds = mBounds;
		// If the position is specified as a center point, adjust the tween point
		if(mCenter)
		{
			mTweenBounds.point -= (mBounds.extent / 2);
		}
		mAlpha = mStartAlpha + mTweenAlpha;
		mTweenAlpha = mStartAlpha = mAlpha;
		resize(mBounds.point, mBounds.extent);
		mDone = true;
		mDistance = 0;
		mStartSpeedMode = false;
		Con::executef(this, 1, "onCompleted");
	}
	else
	{
		mAlpha = mStartAlpha;
	}

	ColorI color(255, 255, 255, (U8) (255 * mAlpha));
	dglSetBitmapModulation(color);
	
	Parent::onRender(offset, updateRect);
}

void GuiTweenBitmapCtrl::startTween()
{
	if(!mDone)
	{
		return; // Ignore additional tween requests until the current tween is finished
	}

	mStartTime = Platform::getRealMilliseconds();
	mStartBounds = mBounds;
	if(mCenter)
	{
		mTweenBounds.point = mTweenBounds.point - (mTweenBounds.extent / 2) - mStartBounds.point;
	}
	else
	{
		mTweenBounds.point = mTweenBounds.point - mStartBounds.point;
	}
	mTweenBounds.extent = mTweenBounds.extent - mStartBounds.extent;
	mTweenAlpha = mTweenAlpha - mStartAlpha;
	mDone = false;
	if (mSpeed > 0) 
	{
		mStartSpeedMode = true;
		mDistance = 0;
	}
}

void GuiTweenBitmapCtrl::endTween()
{
	if(mDone)
	{
		return; // We are already done Tweening
	}

	// Done state
	mStartTime -= mWaitTime + mTweenTime;
	mBounds.point = mStartBounds.point + mTweenBounds.point;
	mBounds.extent = mStartBounds.extent + mTweenBounds.extent;
	mTweenBounds = mStartBounds = mBounds;
	// If the position is specified as a center point, adjust the tween point
	if(mCenter)
	{
		mTweenBounds.point -= (mBounds.extent / 2);
	}
	mAlpha = mStartAlpha + mTweenAlpha;
	mTweenAlpha = mStartAlpha = mAlpha;
	resize(mBounds.point, mBounds.extent);
	mDone = true;
	mStartSpeedMode = false;
}

void GuiTweenBitmapCtrl::setTweenPosition(Point2I position, bool center)
{
	if(!mDone)
	{
		return; // Ignore Tween requests until we are done with the current tweening
	}

	mTweenBounds.point = position;
	mCenter = center;
}

void GuiTweenBitmapCtrl::setTweenExtent(Point2I extent)
{
	if(!mDone)
	{
		return; // Ignore Tween requests until we are done with the current tweening
	}

	AssertFatal(extent.x > 0 && extent.y > 0, "GuiTweenBitmapCtrl::setTweenExtent -- extent out of range");
	mTweenBounds.extent = extent;
}

void GuiTweenBitmapCtrl::setTweenRect(RectI rect, bool center)
{
	setTweenPosition(rect.point, center);
	setTweenExtent(rect.extent);
}

void GuiTweenBitmapCtrl::setTweenAlpha(F32 alpha)
{
	if(!mDone)
	{
		return; // Ignore Tween requests until we are done with the current tweening
	}

	AssertFatal(alpha >= 0.0 && alpha <= 1.0, "GuiTweenBitmapCtrl::setTweenAlpha -- alpha out of range");
    mTweenAlpha = alpha;
}

void GuiTweenBitmapCtrl::setTween(RectI rect, F32 alpha, bool center)
{
	setTweenPosition(rect.point, center);
	setTweenExtent(rect.extent);
	setTweenAlpha(alpha);
}

void GuiTweenBitmapCtrl::setTweenZoom(RectI rect)
{
	if(!mDone)
	{
		return; // Ignore Tween requests until we are done with the current tweening
	}

	GuiControl* parentCtrl = getParent();
	AssertFatal(parentCtrl != NULL, "GuiTweenBitmapCtrl::setTweenZoom -- NULL parent");

	RectI bounds = parentCtrl->mBounds;
    F32 multiplierX = ((F32) bounds.extent.x) / rect.extent.x;
	F32 multiplierY = ((F32) bounds.extent.y) / rect.extent.y;

	S32 newPosX = (S32) -(multiplierX * rect.point.x);
	S32 newPosY = (S32) -(multiplierY * rect.point.y);
	S32 newWidth = (S32) (multiplierX * bounds.extent.x);
	S32 newHeight = (S32) (multiplierY * bounds.extent.y);

	RectI newBounds(newPosX, newPosY, newWidth, newHeight);
	setTweenRect(newBounds);
}

void GuiTweenBitmapCtrl::setWaitTime(U32 time)
{
	if(!mDone)
	{
		return; // Ignore Tween requests until we are done with the current tweening
	}

	mWaitTime = time;
}

void GuiTweenBitmapCtrl::setTweenTime(U32 time)
{
	if(!mDone)
	{
		return; // Ignore Tween requests until we are done with the current tweening
	}

	mTweenTime = time;
}

ConsoleMethod(GuiTweenBitmapCtrl, startTween, void, 2, 2, "")
{
	object->startTween();
}

ConsoleMethod(GuiTweenBitmapCtrl, endTween, void, 2, 2, "")
{
	object->endTween();
}

ConsoleMethod(GuiTweenBitmapCtrl, setTweenPosition, void, 4, 5, "(int x, int y, bool center)")
{
	if(argc == 4)
	{
		object->setTweenPosition(Point2I(dAtoi(argv[2]), dAtoi(argv[3])));
	}
	else
	{
		object->setTweenPosition(Point2I(dAtoi(argv[2]), dAtoi(argv[3])), dAtoi(argv[4]) != 0);
	}
}

ConsoleMethod(GuiTweenBitmapCtrl, setTweenExtent, void, 4, 4, "(int width, int height)")
{
	object->setTweenExtent(Point2I(dAtoi(argv[2]), dAtoi(argv[3])));
}

ConsoleMethod(GuiTweenBitmapCtrl, setTweenRect, void, 6, 7, "(int x, int y, int width, int height, bool center)")
{
	if(argc == 6)
	{
		object->setTweenRect(RectI(dAtoi(argv[2]), dAtoi(argv[3]), dAtoi(argv[4]), dAtoi(argv[5])));
	}
	else
	{
		object->setTweenRect(RectI(dAtoi(argv[2]), dAtoi(argv[3]), dAtoi(argv[4]), dAtoi(argv[5])), dAtoi(argv[6]) != 0);
	}
}

ConsoleMethod(GuiTweenBitmapCtrl, setTweenAlpha, void, 3, 3, "(float alpha)")
{
	object->setTweenAlpha(dAtof(argv[2]));
}

ConsoleMethod(GuiTweenBitmapCtrl, setTween, void, 7, 8, "(int x, int y, int width, int height, float alpha, bool center)")
{
	if(argc == 7)
	{
		object->setTween(RectI(dAtoi(argv[2]), dAtoi(argv[3]), dAtoi(argv[4]), dAtoi(argv[5])), dAtof(argv[6]));
	}
	else
	{
		object->setTween(RectI(dAtoi(argv[2]), dAtoi(argv[3]), dAtoi(argv[4]), dAtoi(argv[5])), dAtof(argv[6]), dAtoi(argv[7]) != 0);
	}
}

ConsoleMethod(GuiTweenBitmapCtrl, setTweenZoom, void, 6, 6, "(int x, int y, int width, int height)")
{
	object->setTweenZoom(RectI(dAtoi(argv[2]), dAtoi(argv[3]), dAtoi(argv[4]), dAtoi(argv[5])));
}

ConsoleMethod(GuiTweenBitmapCtrl, setWaitTime, void, 3, 3, "(int milliseconds >= 0)")
{
	S32 time = dAtoi(argv[2]);
	if(time < 0)
	{
		Con::printf("GuiTweenBitmapCtrl::setWaitTime -- Time out of range: %i", time);
		return;
	}
	object->setWaitTime((U32) time);
}

ConsoleMethod(GuiTweenBitmapCtrl, setTweenTime, void, 3, 3, "(int milliseconds >= 0)")
{
	S32 time = dAtoi(argv[2]);
	if(time < 0)
	{
		Con::printf("GuiTweenBitmapCtrl::setTweenTime -- Time out of range: %i", time);
		return;
	}
	object->setTweenTime((U32) time);
}

ConsoleMethod(GuiTweenBitmapCtrl, isDone, bool, 2, 2, "()")
{
	return object->isDone();
}
