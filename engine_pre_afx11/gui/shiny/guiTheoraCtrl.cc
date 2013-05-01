//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/core/guiControl.h"
#include "gui/shiny/guiTheoraCtrl.h"
#include "console/consoleTypes.h"
#include "dgl/dgl.h"

//----------------------------------------------------------------------------
IMPLEMENT_CONOBJECT(GuiTheoraCtrl);

GuiTheoraCtrl::GuiTheoraCtrl()
{
   mFilename      = StringTable->insert("");
   mSeekFileName  = StringTable->insert("");
   mSaveSeekInfo  = false;
   mDone          = false;
   mLoop          = false;
   mStopOnSleep   = false;
   mBackgroundColor.set(0,0,0);
}
//----------------------------------------------------------------------------
GuiTheoraCtrl::~GuiTheoraCtrl()
{
}
//----------------------------------------------------------------------------
void GuiTheoraCtrl::initPersistFields()
{
	Parent::initPersistFields();

	addGroup("Playback");
	   addField("theoraFile",      TypeFilename,  Offset(mFilename,         GuiTheoraCtrl));
      addField("seekFile",        TypeFilename,  Offset(mSeekFileName,     GuiTheoraCtrl));
	   addField("done",            TypeBool,      Offset(mDone,             GuiTheoraCtrl));	
      addField("saveSeekFile",    TypeBool,      Offset(mSaveSeekInfo,     GuiTheoraCtrl));	
	   addField("stopOnSleep",     TypeBool,      Offset(mStopOnSleep,      GuiTheoraCtrl));
	   addField("backgroundColor", TypeColorI,    Offset(mBackgroundColor,  GuiTheoraCtrl));	   
      addProtectedField("loop",   TypeBool,      Offset(mLoop,             GuiTheoraCtrl),  &setLoopProtected, &defaultProtectedGetFn, "" );
   endGroup("Playback");
}
//----------------------------------------------------------------------------
ConsoleMethod( GuiTheoraCtrl, setFile, void, 3, 5, "( string filename, string seekfilename, bool saveseek ) Set an Ogg Theora file to play and optional seek file and a flag to generate seek index data and save to seek file." )
{
	object->setFile( argv[ 2 ], argv[ 3 ], argv[ 4 ] );
}
//----------------------------------------------------------------------------
ConsoleMethod( GuiTheoraCtrl, play, void, 2, 2, "() Start/continue playback." )
{
   object->play();
}
//----------------------------------------------------------------------------
ConsoleMethod( GuiTheoraCtrl, stop, void, 2, 2, "() Stop playback.")
{
	object->stop();
}
//----------------------------------------------------------------------------
ConsoleMethod( GuiTheoraCtrl, getCurrentFrame, F32, 2, 2, "() Return the current playback frame.")
{
   return object->getCurrentFrame();
}
//----------------------------------------------------------------------------
ConsoleMethod( GuiTheoraCtrl, setCurrentFrame, bool, 3, 3, "( frame ) Set current playhead frame." )
{
   return object->setCurrentFrame( dAtoi( argv[ 2 ] ) );
}
//----------------------------------------------------------------------------
ConsoleMethod( GuiTheoraCtrl, getCurrentTime, F32, 2, 2, "() Return the elapsed time since playback start, in seconds.")
{
   return object->getCurrentTime();
}
//----------------------------------------------------------------------------
ConsoleMethod( GuiTheoraCtrl, setCurrentTime, bool, 3, 3, "( time ) Set current playhead time marker (in seconds)." )
{
   return object->setCurrentTime( dAtof( argv[ 2 ] ) );
}
//----------------------------------------------------------------------------
ConsoleMethod( GuiTheoraCtrl, getCurrentVTime, F32, 2, 2, "() Return the elapsed (video) time since playback start, in seconds.")
{
   return object->getCurrentVTime();
}
//----------------------------------------------------------------------------
bool GuiTheoraCtrl::setFile( const char* szFilename, const char *seekFile, bool saveSeek )
{
	mDone = false;
	
   if( szFilename && szFilename[0] )
   {
	   mFilename = StringTable->insert( szFilename );

		return mTheoraTexture.setFile( szFilename, true, true, 0, seekFile, saveSeek );
   }

   return false;
}
//----------------------------------------------------------------------------
void GuiTheoraCtrl::play()
{
   setFile( mFilename, mSeekFileName, mSaveSeekInfo );
}
//----------------------------------------------------------------------------
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

	if( mTheoraTexture.isReady() && mTheoraTexture.isPlaying() )
	{
      mTheoraTexture.refresh();
		
      dglClearBitmapModulation();
		
      dglDrawBitmapStretch(mTheoraTexture, rect);
	}
	else
	{
		if( mTheoraTexture.isReady() )
      {
  	      mDone = true;
         
         dglDrawBitmapStretch(mTheoraTexture, rect);
       }
       else
 		   dglDrawRectFill(rect, mBackgroundColor); // black rect      
	}
   
   if( mTheoraTexture.frameHasChanged() && isMethod( "onMovieFrame" ) )
   {
      // Reset flag
      mTheoraTexture.resetFrameChangedFlag();

      S32 frame = mTheoraTexture.getCurrentFrame();
      F32 time  = mTheoraTexture.getCurrentTime();

      Con::executef( this, 3, "onMovieFrame", Con::getIntArg( frame ), Con::getFloatArg( time ) );
   }

   if( mTheoraTexture.hasLooped() )
   {
      Con::executef( this, 1, "onMovieLoop" );

      mTheoraTexture.resetLoopFlag();
   }

	renderChildControls( offset, updateRect );
}

//----------------------------------------------------------------------------
void GuiTheoraCtrl::inspectPostApply()
{
   stop();
   setFile(mFilename);
}
bool GuiTheoraCtrl::onKeyDown(const GuiEvent &event)
{
	switch (event.keyCode)
	{
	case KEY_ESCAPE:
		Con::executef(this, 1, "onEscape");
		return true;
	}
	return Parent::onKeyDown(event);
}
