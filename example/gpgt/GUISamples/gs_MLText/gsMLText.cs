//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiMLTextCtrl Samples ---------");
//--------------------------------------------------------------------------
// gsMLText.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// 
// In this file, we demonstrate GuiMLTextCtrl.
//
// This is a very cool control that allows us to make all kinds of nicely
// formatted in-game documents.  These documents can be files stored on disk
// or dynamic documents containing game information/feedback/etc.  Use your
// imagination.  
//
// In this sample, we will simply load some predefined content from a file on
// disk.  
//
// This sample only barely scratches the total set of GuiMLTextCtrl features.
//
// You can use this sample to try your ideas out easily and quickly.  Just follow
// these steps:
// 
// 1. Open the file: gsMLTextContent.txt in your favorite editor.
// 2. Start the GPGT kit.
// 3. Add your new content to gsMLTextContent.txt (feel free to erase it).
// 4. Open this sample click 'Gui Sampler (button) -> GuiMLTextCtrl (button)'
//
// Now, to make changes, simply do this:
//
// 1. Make your change(s) to gsMLTextContent.txt and save them.
// 2. Press the button labelled 'Reload File'
//
// Also, every time this lesson is opened it re-loads the file contents.  This
// makes it possible to test your changes right away.  The only thing that 
// won't work is adding graphics that were not present at the time you
// started the GPGT kit. 
// 

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

// This profile has been placed here to make it easy for you to experiment
// with various profile features and how they affect GuiMLTextCtrl controls.

if(!isObject(gsMLTextProfile)) new GuiControlProfile (gsMLTextProfile)
{
	opaque    = true;
};

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

function TestMLText::onWake( %theControl ) {
	%theControl.reload();
}

//
// 1. Clear all content.
// 2. Open the file gsMLTextContent.txt (abort if not found)
// 3. Read the file and push the contents into this GuiMLTextCtrl
//
function TestMLText::reload( %theControl ) {
	%theControl.setValue(""); // Clear it

	%file = new FileObject();

	%fileName = expandFileName( "./gsMLTextContent.txt" );

	echo( "Attempt to open " , %fileName );

	%fileIsOpen = %file.openForRead( %fileName );

	echo( "Open for read " , (%fileIsOpen ? "succeeded" : "failed" ) );

	if( %fileIsOpen ) {
		while(!%file.isEOF()) {

			%currentLine = %file.readLine();

			echo(%currentLine);

			%theControl.addText( %currentLine, true );

		}
	}

	if( %theControl.isVisible() ) %theControl.forceReflow();

	%file.close();
	%file.delete();
}

function TestMLText::onURL( %theControl , %url )
{
   // Game Links are for internal 'browsing'/'surfing', while
   // non-game links should go the the web browser.
   
   if( 0 == strstr( %url , "gamelink" ) )
   { 
      // It's a game link
      echo("got a game link ", %url);
      // Do what you want with this info
   }   
   else 
   { 
      echo("got a web link ", %url);
      gotoWebPage( %url );// It's an external URL
   }   
   
}


//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./gsMLText.gui");


