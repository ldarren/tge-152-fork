//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//echo("\n\c3--------- Loading loading Tech   ---------");

//--------------------------------------------------------------------------
// loading.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------
new AudioProfile(loadingAP)
{
   filename = "./loading.ogg";
   description = "AudioGuiLooping";
   preload = true;
};


// This profile has been placed here to make it easy for you to experiment
// with various profile features and how they affect GuiMLTextCtrl controls.

if(!isObject(loadingScrollProfile)) new GuiControlProfile (loadingScrollProfile : GuiScrollProfile)
{
	border = 0;
	opaque    = false;
};



if(!isObject(loadingMLTextProfile)) new GuiControlProfile (loadingMLTextProfile)
{
	opaque    = false;
};

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

function loadingMLText::onWake( %this ) {
	%this.reload();
	%this.musicID = alxPlay(loadingAP);
}

function loadingMLText::onSleep( %this ) {
	if(0 != %this.musicID) {
		alxStop( %this.musicID );
		%this.musicID = 0;
	}
}

//
// 1. Clear all content.
// 2. Open the file gsMLTextContent.txt (abort if not found)
// 3. Read the file and push the contents into this GuiMLTextCtrl
//
function loadingMLText::reload( %this ) {
	%this.setValue(""); // Clear it

	%file = new FileObject();

	%fileName = expandFileName( "./loading.txt" );

	//echo( "Attempt to open " , %fileName );

	%fileIsOpen = %file.openForRead( %fileName );

	//echo( "Open for read " , (%fileIsOpen ? "succeeded" : "failed" ) );

	if( %fileIsOpen ) {
		while(!%file.isEOF()) {

			%currentLine = %file.readLine();

			//echo(%currentLine);

			%this.addText( %currentLine, true );

		}
	}

	if( %this.isVisible() ) %this.forceReflow();

	%file.close();
	%file.delete();
}


