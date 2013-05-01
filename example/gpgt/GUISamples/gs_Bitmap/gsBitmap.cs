//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiBitmapButtonCtrl Samples ---------");

//--------------------------------------------------------------------------
// gsBitmapButton.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// 
// In this file, we demonstrate, the following GuiBitmapButtonCtrl features:
//
// 1. Wrapping
// 2. setValue() 
// 3. setBitmap()

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

// *************************************************************************
//  Wrapping Sample 
// *************************************************************************
//
// No code required for this sample.  See GUI definition.
//


// *************************************************************************
//  Scrolling Sample - setValue() 
// *************************************************************************
// This sample scrolls a bitmap diagonally over time.  This is accomplished
// by using the onWake() callback to start a scrolling action which then
// re-schedules itself continually.
//
// We want to save processing power, so when the control goes to sleep it
// should stop scrolling.  This can be accomplished by cancelling the
// last schedule in onSleep().  
// 
// There is no need to cancel in onRemove() because the schedule is 
// tied to the 'existence' of this control and will automatically be
// cancelled when the control object is destroyed.
//
function TestBitmap2::onWake( %theControl ) 
{
   %theControl.scrollMe();
}

function TestBitmap2::onSleep( %theControl ) 
{
   cancel( %theConrol.lastSchedule );
}

function TestBitmap2::scrollMe( %theControl ) 
{
   // Prevent overlapping scrolls
   if( isEventPending(%theControl.lastSchedule) )
      return;

	%theControl.curX += 2;
	%theControl.curY += 2;

	if( %theControl.curX >= 256) 
   {
		%theControl.curX = 0;
		%theControl.curY = 0;
	}

	%theControl.setValue( %theControl.curX , %theControl.curY );

   %theControl.lastSchedule = %theControl.schedule( 32 , scrollMe );
}


// *************************************************************************
//  setBimap() Sample 
// *************************************************************************
// This sample changes the bitmap used by the control over time. This is 
// accomplished by using the various callbacks to first initialize and
// subsequently to change the bitmaps.
//
// The scheduling behavior for this sample is quite similar to the prior 
// scrolling sample.
//

function TestBitmap3::onAdd( %theControl ) 
{
	//
	// Here is a good example of the expandFilename() function.
	// We are storing the paths of the files we want to use later, in
	// our bitmapGUI as an array of dynamic fields.
	//
   %tmp = 0;
	%theControl.Image[%tmp]   = expandFilename("./GGRTSPackImages/rtsKitLogo");
	%theControl.Image[%tmp++] = expandFilename("./GGRTSPackImages/rtsKit1");
	%theControl.Image[%tmp++] = expandFilename("./GGRTSPackImages/rtsKit2");
	%theControl.Image[%tmp++] = expandFilename("./GGRTSPackImages/rtsKit3");
	%theControl.Image[%tmp++] = expandFilename("./GGRTSPackImages/rtsKit4");
	%theControl.Image[%tmp++] = expandFilename("./GGRTSPackImages/rtsKit5");
	%theControl.Image[%tmp++] = expandFilename("./GGRTSPackImages/rtsKit6");
	%theControl.Image[%tmp++] = expandFilename("./GGRTSPackImages/rtsKit7");
	%theControl.Image[%tmp++] = expandFilename("./GGRTSPackImages/rtsKit8");
	%theControl.Image[%tmp++] = expandFilename("./GGRTSPackImages/rtsKit9");
	%theControl.Image[%tmp++] = expandFilename("./GGRTSPackImages/rtsKit10");

	%theControl.currentImage = 0;
	%theControl.totalImages  = %tmp++;
}

function TestBitmap3::onWake( %theControl ) 
{
	// Always start on first frame
	%theControl.currentImage = 0;
	%theControl.setBitmap(%theControl.Image[%theControl.currentImage]);

   %theControl.lastSchedule = %theControl.schedule( 2000 , flipMe );
}

function TestBitmap3::onSleep( %theControl ) 
{
	cancel( %theConrol.lastSchedule );
}

function TestBitmap3::flipMe( %theControl ) 
{
   // Prevent overlapping scrolls
   if( isEventPending(%theControl.lastSchedule) )
      return;

   %theControl.currentImage++;

	if(%theControl.currentImage >= %theControl.totalImages) 
   {
		%theControl.currentImage = 0;
	}


	%theControl.setBitmap(%theControl.Image[%theControl.currentImage]);

   //echo("Loading new image ==> ",%theControl.Image[%theControl.currentImage]);

   %theControl.lastSchedule = %theControl.schedule( 2000 , flipMe );
}


//--------------------------------------------------------------------------
// Load Interface Definition
//---------------------------------------------------------------
exec("./gsBitmap.gui");

