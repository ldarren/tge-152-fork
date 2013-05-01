//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading Credits Toon   ---------");

//--------------------------------------------------------------------------
// workArea.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

// This profile has been placed here to make it easy for you to experiment
// with various profile features and how they affect GuiMLTextCtrl controls.

if(!isObject(gsToonCreditsScrollProfile)) new GuiControlProfile (gsToonCreditsScrollProfile : GuiScrollProfile)
{
	border = 0;
	opaque    = false;
};



if(!isObject(gsToonCreditsMLTextProfile)) new GuiControlProfile (gsToonCreditsMLTextProfile)
{
	opaque    = false;
};

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

function ifcsCreditsToonMLText::onWake( %this ) {
	%this.reload();
}

//
// 1. Clear all content.
// 2. Open the file gsMLTextContent.txt (abort if not found)
// 3. Read the file and push the contents into this GuiMLTextCtrl
//
function ifcsCreditsToonMLText::reload( %this ) {
	%this.setValue(""); // Clear it

	%file = new FileObject();

	%fileName = expandFileName( "./gsMLCreditsToonTextContent.txt" );

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
exec("./ifcsCreditsToon.gui");



