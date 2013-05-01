//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/core/guiControl.h"
#include "gui/shiny/guiTheoraCtrl.h"
#include "console/consoleTypes.h"
#include "dgl/dgl.h"

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiTheoraCtrl);

GuiTheoraCtrl::GuiTheoraCtrl()
{
   mFilename      = StringTable->insert("");
   mDone          = false;
   mStopOnSleep   = false;
   mBackgroundColor.set(0,0,0);
}

//----------------------------------------------------------------------------
GuiTheoraCtrl::~GuiTheoraCtrl()
{
}

void GuiTheoraCtrl::initPersistFields()
{
	Parent::initPersistFields();

   addGroup("Playback");

	addField("theoraFile",  TypeFilename,  Offset(mFilename,          GuiTheoraCtrl));
	addField("done",        TypeBool,      Offset(mDone,              GuiTheoraCtrl));
   addField("stopOnSleep", TypeBool,      Offset(mStopOnSleep,       GuiTheoraCtrl));
   addField("backgroundColor", TypeColorI,Offset(mBackgroundColor,   GuiTheoraCtrl));

   endGroup("Playback");
}

ConsoleMethod( GuiTheoraCtrl, setFile, void, 3, 3, "(string filename) Set an Ogg Theora file to play.")
{
	object->setFile(argv[2]);
}

ConsoleMethod( GuiTheoraCtrl, stop, void, 2, 2, "() Stop playback.")
{
	object->stop();
}

ConsoleMethod( GuiTheoraCtrl, getCurrentTime, F32, 2, 2, "() Return the time elapsed in playback, in seconds.")
{
   return object->getCurrentTime();
}

void GuiTheoraCtrl::setFile(const char* szFilename)
{
	mDone = false;
	if(szFilename && szFilename[0])
		mTheoraTexture.setFile(szFilename, true);
}

void GuiTheoraCtrl::stop()
{
	mTheoraTexture.stop();
	mDone = true;
}

//----------------------------------------------------------------------------
bool GuiTheoraCtrl::onWake()
{
	if (!Parent::onWake()) return false;

	if(mTheoraTexture.isReady())
		return true;

	setFile(mFilename);

	return true;
}

//----------------------------------------------------------------------------
void GuiTheoraCtrl::onSleep()
{
	Parent::onSleep();

   if(mStopOnSleep)
      stop();
}

//----------------------------------------------------------------------------
void GuiTheoraCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   const RectI rect(offset, mBounds.extent);

	if(mTheoraTexture.isReady() && mTheoraTexture.isPlaying())
	{
		mTheoraTexture.refresh();
		dglClearBitmapModulation();
		dglDrawBitmapStretch(mTheoraTexture, rect);
	}
	else
	{
		if(mTheoraTexture.isReady())
			mDone = true;

 		dglDrawRectFill(rect, mBackgroundColor); // black rect
	}

	renderChildControls(offset, updateRect);
}

//----------------------------------------------------------------------------
void GuiTheoraCtrl::inspectPostApply()
{
   stop();
   setFile(mFilename);
}
