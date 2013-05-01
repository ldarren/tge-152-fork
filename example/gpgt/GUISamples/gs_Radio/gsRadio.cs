//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiRadioCtrl Samples ---------");
//--------------------------------------------------------------------------
// gsRadio.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// 
// In this file, we demonstrate, the following GuiRadioCtrl features:
//
// 1. Normal, Highlighted, Depressed, and Inactive button states
// 2. PushButton, ToggleButton, and Radio Button
// 3. setBitmap()
// 4. onAction() callback
// 5. Use of $thisControl global

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

if(!isObject(winradio)) new GuiControlProfile (winradio : gsDefaultProfile)
{
	hasBitmapArray = true;
	bitmap = "./torqueRadio";
};

if(!isObject(osxradio)) new GuiControlProfile (osxradio : winradio)
{
	hasBitmapArray = true;
	bitmap = "./osxRadio";
};

if(!isObject(gemradio)) new GuiControlProfile (gemradio : winradio)
{
	hasBitmapArray = true;
//	bitmap = "./gemradio";
};

if(!isObject(coolradio)) new GuiControlProfile (coolradio : winradio)
{
	hasBitmapArray = true;
//	bitmap = "./coolradio";
};

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

// 
// This method is in the namespace of RadioActivator. This control
// uses this command definition to call this method:
//
// command = "$thisControl.toggleButtonsActive();";
//
// When a command method is called by a GUI, it sets the global variable:
// $thisControl to the calling GUI prior to calling the command.
//

function RadioActivator::toggleButtonsActive( %theControl ) {

	TestRadioPushButton.setActive(! TestRadioPushButton.isActive() );

	TestRadioToggleButton.setActive(! TestRadioToggleButton.isActive() );

	TestRadioRadioButton0.setActive(! TestRadioRadioButton0.isActive() );
	TestRadioRadioButton1.setActive(! TestRadioRadioButton1.isActive() );
	TestRadioRadioButton2.setActive(! TestRadioRadioButton2.isActive() );
}


//
//    The onAction() method is called for all buttons when they are clicked.
//    We want to target this onAction method to a named control, because
//    we do not want to pollute the parent(s) namespace.  Unforutnately,
//    ass, clasname, and superclass DO NOT WORK for GUI controls. Thus,
//    all the theme buttons have been given the same name.  Normally,
//    this is a no no, but because we want to have a onAction() callback that 
//    responds to a key press from any of them, we MUST give them the same name.

function ChangeRadioTheme::onAction( %theControl ) {
	TestRadioPushButton.setProfile(%theControl.theme);

	TestRadioToggleButton.setProfile(%theControl.theme);

	TestRadioRadioButton0.setProfile(%theControl.theme);
	TestRadioRadioButton1.setProfile(%theControl.theme);
	TestRadioRadioButton2.setProfile(%theControl.theme);
}

//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./gsRadio.gui");

