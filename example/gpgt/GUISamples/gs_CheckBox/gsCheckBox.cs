//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiCheckBoxCtrl Samples ---------");
//--------------------------------------------------------------------------
// gsCheckBox.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// 
// In this file, we demonstrate, the following GuiCheckBoxCtrl features:
//
// 1. Normal, Highlighted, Depressed, and Inactive button states
// 2. PushButton, ToggleButton, and Radio Button
// 3. setBitmap()
// 4. onAction() callback
// 5. Use of $thisControl global

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

if(!isObject(wincheck)) new GuiControlProfile (wincheck : gsDefaultProfile)
{
	hasBitmapArray = true;
	bitmap = "./wincheck";
};

if(!isObject(osxcheck)) new GuiControlProfile (osxcheck : wincheck)
{
	hasBitmapArray = true;
	bitmap = "./osxcheck";
};

if(!isObject(gemcheck)) new GuiControlProfile (gemcheck : wincheck)
{
	hasBitmapArray = true;
//	bitmap = "./gemcheck";
};

if(!isObject(coolcheck)) new GuiControlProfile (coolcheck : wincheck)
{
	hasBitmapArray = true;
//	bitmap = "./coolcheck";
};

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

// 
// This method is in the namespace of CheckBoxActivator. This control
// uses this command definition to call this method:
//
// command = "$thisControl.toggleButtonsActive();";
//
// When a command method is called by a GUI, it sets the global variable:
// $thisControl to the calling GUI prior to calling the command.
//

function CheckBoxActivator::toggleButtonsActive( %theControl ) {

	TestCheckBoxPushButton.setActive(! TestCheckBoxPushButton.isActive() );

	TestCheckBoxToggleButton.setActive(! TestCheckBoxToggleButton.isActive() );

	TestCheckBoxRadioButton0.setActive(! TestCheckBoxRadioButton0.isActive() );
	TestCheckBoxRadioButton1.setActive(! TestCheckBoxRadioButton1.isActive() );
	TestCheckBoxRadioButton2.setActive(! TestCheckBoxRadioButton2.isActive() );
}


//
//    The onAction() method is called for all buttons when they are clicked.
//    We want to target this onAction method to a named control, because
//    we do not want to pollute the parent(s) namespace.  Unforutnately,
//    ass, clasname, and superclass DO NOT WORK for GUI controls. Thus,
//    all the theme buttons have been given the same name.  Normally,
//    this is a no no, but because we want to have a onAction() callback that 
//    responds to a key press from any of them, we MUST give them the same name.

function ChangeCheckBoxTheme::onAction( %theControl ) {
	TestCheckBoxPushButton.setProfile(%theControl.theme);

	TestCheckBoxToggleButton.setProfile(%theControl.theme);

	TestCheckBoxRadioButton0.setProfile(%theControl.theme);
	TestCheckBoxRadioButton1.setProfile(%theControl.theme);
	TestCheckBoxRadioButton2.setProfile(%theControl.theme);
}

//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./gsCheckBox.gui");

