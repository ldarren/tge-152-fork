//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleTypes.h"
#include "dgl/dgl.h"

#include "gui/game/guiProgressCtrl.h"

IMPLEMENT_CONOBJECT(GuiProgressCtrl);

GuiProgressCtrl::GuiProgressCtrl()
:	mStartColor(0, 0, 0, 0),
	mEndColor(0, 0, 0, 0),
	mCurrColor(0, 0, 0, 0)
{
   mProgress = 0.0f;
   mTarget = -1.0f;
   mStartPoint = -1.0f;
   mStartTime = 0;
   mTweenTime = 0;
}

void GuiProgressCtrl::initPersistFields()
{
	Parent::initPersistFields();
	
	addGroup("Transition");
	addField("startColor",          TypeColorI, Offset(mStartColor, GuiProgressCtrl)); // between 0 and 0.1
	addField("endColor",            TypeColorI, Offset(mEndColor, GuiProgressCtrl)); // between 0 and 0.01
	endGroup("Transition");	
}

// DARREN MOD: set trasition target value
void GuiProgressCtrl::setTargetVal(F32 t, U32 time)
{
	if (mTarget != mProgress) mProgress = mTarget;
	mStartPoint = mProgress;
	mTarget = mClampF(t, 0.f, 1.f);
	mTweenTime = time;
	mStartTime = Platform::getRealMilliseconds();
}

const char* GuiProgressCtrl::getScriptValue()
{
   char * ret = Con::getReturnBuffer(64);
   dSprintf(ret, 64, "%g", mProgress);
   return ret;
}

void GuiProgressCtrl::setScriptValue(const char *value)
{
   //set the value
   if (! value)
      mProgress = 0.0f;
   else
      mProgress = dAtof(value);

   //validate the value
   mProgress = mClampF(mProgress, 0.f, 1.f);
   mTarget = mProgress; // disable smooth progress
   setUpdate();
}

void GuiProgressCtrl::onPreRender()
{
   const char * var = getVariable();
   
   if(var)
   {
      F32 value = mClampF(dAtof(var), 0.f, 1.f);
      if(value != mProgress)
      {
         mProgress = value;
		 mTarget = mProgress; // disable smooth progress
         setUpdate();
      }
   }
   // DARREN MOD: smooth transition and color change
   else if (mTarget >= 0 && mProgress != mTarget)
   {
	   	U32 elapsed = Platform::getRealMilliseconds() - mStartTime;
		if (elapsed < mTweenTime)
		{
			F32 percent = F32(elapsed) / F32(mTweenTime);
			mProgress = (F32) (F32(mStartPoint) + F32(mTarget - mStartPoint) * percent);
		}
		else
		{
			mProgress = mTarget;
			mStartPoint = mProgress;
		}
   }
   
   if (mStartColor.alpha || mEndColor.alpha)
   {
	  mCurrColor.red = mStartColor.red + (mEndColor.red - mStartColor.red)*mProgress;
	  mCurrColor.green = mStartColor.green + (mEndColor.green - mStartColor.green)*mProgress;
	  mCurrColor.blue = mStartColor.blue + (mEndColor.blue - mStartColor.blue)*mProgress;
	  mCurrColor.alpha = mStartColor.alpha + (mEndColor.alpha - mStartColor.alpha)*mProgress;
   }
   else
	  mCurrColor = mProfile->mFillColor;
}

void GuiProgressCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   RectI ctrlRect(offset, mBounds.extent);

   //draw the progress
   S32 width = (S32)((F32)mBounds.extent.x * mProgress);
   if (width > 0)
   {
      RectI progressRect = ctrlRect;
      progressRect.extent.x = width;

      dglDrawRectFill(progressRect, mCurrColor);
   }

   //now draw the border
   if (mProfile->mBorder)
      dglDrawRect(ctrlRect, mProfile->mBorderColor);

   Parent::onRender( offset, updateRect );

   //render the children
   renderChildControls(offset, updateRect);
}

ConsoleMethod( GuiProgressCtrl, setTargetVal, void, 4, 4, "void setTargetValue(F32 target, U32 time)")
{
	object->setTargetVal(dAtof(argv[2]), dAtoi(argv[3]));
}
