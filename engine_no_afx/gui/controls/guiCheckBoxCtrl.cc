//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "dgl/dgl.h"
#include "gui/core/guiCanvas.h"
#include "gui/controls/guiCheckBoxCtrl.h"
#include "console/consoleTypes.h"

IMPLEMENT_CONOBJECT(GuiCheckBoxCtrl);

//---------------------------------------------------------------------------
GuiCheckBoxCtrl::GuiCheckBoxCtrl()
{
   mBounds.extent.set(140, 30);
	mStateOn = false;
   mIndent = 0;
   mButtonType = ButtonTypeCheck;
   mUseInactiveState = false;
}

//---------------------------------------------------------------------------

void GuiCheckBoxCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addField("useInactiveState", TypeBool, Offset(mUseInactiveState, GuiCheckBoxCtrl));
}

bool GuiCheckBoxCtrl::onWake()
{
   if(!Parent::onWake())
      return false;

   // make sure there is a bitmap array for this control type
   // if it is declared as such in the control
   mProfile->constructBitmapArray();

   return true;
}

void GuiCheckBoxCtrl::onMouseDown(const GuiEvent& event)
{
   if (!mUseInactiveState)
   {
      Parent::onMouseDown(event);
      return;
   }

   if (mProfile->mCanKeyFocus)
      setFirstResponder();

   if (mProfile->mSoundButtonDown)
   {
      F32 pan = (F32(event.mousePoint.x)/F32(Canvas->mBounds.extent.x)*2.0f-1.0f)*0.8f;
      AUDIOHANDLE handle = alxCreateSource(mProfile->mSoundButtonDown);
      alxPlay(handle);
   }

   //lock the mouse
   mouseLock();
   mDepressed = true;

   //update
   setUpdate();
}

void GuiCheckBoxCtrl::onMouseUp(const GuiEvent& event)
{
   if (!mUseInactiveState)
   {
      Parent::onMouseUp(event);
      return;
   }

   mouseUnlock();

   setUpdate();

   //if we released the mouse within this control, perform the action
   if (mDepressed)
      onAction();

   mDepressed = false;
}

void GuiCheckBoxCtrl::onAction()
{
   if (!mUseInactiveState)
   {
      Parent::onAction();
      return;
   }

   if(mButtonType == ButtonTypeCheck)
   {
      if (!mActive)
      {
         mActive = true;
         mStateOn = true;
      }
      else if (mStateOn)
         mStateOn = false;
      else if (!mStateOn)
         mActive = false;

      // Update the console variable:
      if ( mConsoleVariable[0] )
         Con::setBoolVariable( mConsoleVariable, mStateOn );
      // Execute the console command (if any)
      if( mConsoleCommand[0] )
      {
         char buf[16];
         dSprintf(buf, sizeof(buf), "%d", getId());
         Con::setVariable("$ThisControl", buf);
         Con::evaluate( mConsoleCommand, false );
      }
	}
   setUpdate();

   // Provide and onClick script callback.
   if( isMethod("onClick") )
      Con::executef( this, 2, "onClick" );
}

//---------------------------------------------------------------------------
void GuiCheckBoxCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   ColorI backColor = mActive ? mProfile->mFillColor : mProfile->mFillColorNA;
   ColorI fontColor = mMouseOver ? mProfile->mFontColorHL : mProfile->mFontColor;
	ColorI insideBorderColor = isFirstResponder() ? mProfile->mBorderColorHL : mProfile->mBorderColor;

   // just draw the check box and the text:
   S32 xOffset = 0;
	dglClearBitmapModulation();
   if(mProfile->mBitmapArrayRects.size() >= 4)
   {
      S32 index = mStateOn;
      if(!mActive)
         index = 2;
      else if(mDepressed)
         index += 2;
      xOffset = mProfile->mBitmapArrayRects[0].extent.x + 2 + mIndent;
      S32 y = (mBounds.extent.y - mProfile->mBitmapArrayRects[0].extent.y) / 2;
      dglDrawBitmapSR(mProfile->mTextureHandle, offset + Point2I(mIndent, y), mProfile->mBitmapArrayRects[index]);
   }
   
	if(mButtonText[0] != NULL)
	{
	   dglSetBitmapModulation( fontColor );
      renderJustifiedText(Point2I(offset.x + xOffset, offset.y),
                          Point2I(mBounds.extent.x - mBounds.extent.y, mBounds.extent.y),
                          mButtonText);
  	}
   //render the children
   renderChildControls(offset, updateRect);
}

ConsoleMethod(GuiCheckBoxCtrl, setStateOn, void, 3, 3, "(state)")
{
   if (dStricmp(argv[2], "true") == 0)
      object->setStateOn(1);
   else if (dStricmp(argv[2], "false") == 0)
      object->setStateOn(0);
   else
      object->setStateOn(dAtoi(argv[2]));
}

void GuiCheckBoxCtrl::setStateOn(S32 state)
{
   if (mUseInactiveState)
   {
      if (state < 0)
      {
         setActive(false);
         Parent::setStateOn(false);
      }
      else if (state == 0)
      {
         setActive(true);
         Parent::setStateOn(false);
      }
      else if (state > 0)
      {
         setActive(true);
         Parent::setStateOn(true);
      }
   }
   else
      Parent::setStateOn((bool)state);
}

const char* GuiCheckBoxCtrl::getScriptValue()
{
   if (mUseInactiveState)
   {
      if (isActive())
         if (mStateOn)
            return "1";
         else
            return "0";
      else
         return "-1";
   }
   else
      return Parent::getScriptValue();
}

//---------------------------------------------------------------------------
// EOF //


