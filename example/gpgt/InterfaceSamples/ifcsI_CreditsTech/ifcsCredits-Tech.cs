//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading Credits Tech   ---------");

//--------------------------------------------------------------------------
// ifcsCreditsTech.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------
new AudioProfile(AudioCreditsTech)
{
   filename = "./credits.ogg";
   description = "AudioGuiLooping";
   preload = true;
};


// This profile has been placed here to make it easy for you to experiment
// with various profile features and how they affect GuiMLTextCtrl controls.

if(!isObject(gsTechCreditsScrollProfile)) new GuiControlProfile (gsTechCreditsScrollProfile : GuiScrollProfile)
{
	border = 0;
	opaque    = false;
};



if(!isObject(gsTechCreditsMLTextProfile)) new GuiControlProfile (gsTechCreditsMLTextProfile)
{
	opaque    = false;
};

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

function ifcsCreditsTechMLText::onWake( %this ) {
	%this.reload();
	%this.musicID = alxPlay(AudioCreditsTech);
}

function ifcsCreditsTechMLText::onSleep( %this ) {
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
function ifcsCreditsTechMLText::reload( %this ) {
	%this.setValue(""); // Clear it

	%file = new FileObject();

	%fileName = expandFileName( "./gsMLCreditsTechTextContent.txt" );

	echo( "Attempt to open " , %fileName );

	%fileIsOpen = %file.openForRead( %fileName );

	echo( "Open for read " , (%fileIsOpen ? "succeeded" : "failed" ) );

	if( %fileIsOpen ) {
		while(!%file.isEOF()) {

			%currentLine = %file.readLine();

			echo(%currentLine);

			%this.addText( %currentLine, true );

		}
	}

	if( %this.isVisible() ) %this.forceReflow();

	%file.close();
	%file.delete();
}



//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./ifcsCreditsTech.gui");



