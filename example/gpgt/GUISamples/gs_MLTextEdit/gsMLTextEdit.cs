//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiMLTextEditCtrl Samples ---------");
//--------------------------------------------------------------------------
// gsMLTextEdit.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// 
// In this file, we demonstrate use of a GuiMLTextEditCtrl:
//
// The premise of this sample is simple.  There are three areas to type in data 
// and a button.
//
// Line A - Type a test format string here.
// Line B - Type some optional data here
//
// 'Apply' Button - Pressing this copies A + B to C
//
// Line C - Feel free to type both formatting and data text directly into 
//          the control to test it out.

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

if(!isObject(gsMLTextEditTestProfile)) new GuiControlProfile (gsMLTextEditTestProfile) 
{ 
	fontColorLink	= "255 96 96"; 
	fontColorLinkHL = "0 0 255"; 

	fillColor		= "255 255 255"; 
	fillColorHL		= "128 128 128"; 

	fontColor		= "0 0 0"; 
	fontColorHL		= "255 255 255"; 
	fontColorNA		= "128 128 128"; 

	autoSizeWidth	= true; 
	autoSizeHeight	= true; 
	tab				= true; 
	canKeyFocus		= true; 
}; 

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

function gsMLTextEdit::onWake( %theControl ) {

	TestMLTextEditFormat.setText("<Font:Arial Bold:16>");

	TestMLTextEditContent.setText("Sample content.");

	TestMLTextEdit.TestMLTextEditUpdate( );
}


function TestMLTextEdit::TestMLTextEditUpdate( %theControl ) {
	%theControl.setText ( TestMLTextEditFormat.getValue() @ TestMLTextEditContent.getValue() );
	//%theControl.forceReflow();
}

//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./gsMLTextEdit.gui");



