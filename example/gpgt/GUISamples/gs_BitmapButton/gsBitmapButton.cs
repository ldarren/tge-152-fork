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
// 1. Normal, Highlighted, Depressed, and Inactive button states
// 2. PushButton, ToggleButton, and Radio Button
// 3. setBitmap()
// 4. onAction() callback
// 5. Use of $thisControl global

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------

// 
// This method is in the namespace of BitmapButtonActivator. This control
// uses this command definition to call this method:
//
// command = "$thisControl.toggleButtonsActive();";
//
// When a command method is called by a GUI, it sets the global variable:
// $thisControl to the calling GUI prior to calling the command.
//

function BitmapButtonActivator::toggleButtonsActive( %theControl ) {

	TestBitmapPushButton.setActive(! TestBitmapPushButton.isActive() );

	TestBitmapToggleButton.setActive(! TestBitmapToggleButton.isActive() );

	TestBitmapRadioButton0.setActive(! TestBitmapRadioButton0.isActive() );
	TestBitmapRadioButton1.setActive(! TestBitmapRadioButton1.isActive() );
	TestBitmapRadioButton2.setActive(! TestBitmapRadioButton2.isActive() );
}


//
// The 'theme' name is located in each control.  
// We have the control find the right bitmap when the control is added
// to its parent:
//

function ChangeBitmapButtonTheme::onAdd( %theControl ) {
	%theControl.logoPath = expandFilename("./" @ %theControl.theme);
}


//
//    The onAction() method is called for all buttons when they are clicked.
//    We want to target this onAction method to a named control, because
//    we do not want to pollute the parent(s) namespace.  Unforutnately,
//    ass, clasname, and superclass DO NOT WORK for GUI controls. Thus,
//    all the theme buttons have been given the same name.  Normally,
//    this is a no no, but because we want to have a onAction() callback that 
//    responds to a key press from any of them, we MUST give them the same name.

function ChangeBitmapButtonTheme::onAction( %theControl ) {
	TestBitmapPushButton.setBitmap(%theControl.logoPath);

	TestBitmapToggleButton.setBitmap(%theControl.logoPath);

	TestBitmapRadioButton0.setBitmap(%theControl.logoPath);
	TestBitmapRadioButton1.setBitmap(%theControl.logoPath);
	TestBitmapRadioButton2.setBitmap(%theControl.logoPath);
}


//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
exec("./gsBitmapButton.gui");

