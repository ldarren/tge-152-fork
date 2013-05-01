//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiButtonCtrl Samples ---------");
//--------------------------------------------------------------------------
// gsButton.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// 
// In this file, we demonstrate, the following GuiButtonCtrl features:
//
// 1. Normal, Highlighted, Depressed, and Inactive button states
// 2. PushButton, ToggleButton, and Radio Button
// 3. setBitmap()
// 4. onAction() callback
// 5. Use of $thisControl global

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------
if(!isObject(gsButtonProfile0)) new GuiControlProfile (gsButtonProfile0 : gsDefaultProfile) {
	justify = "center";
	tab = true;
};

if(!isObject(gsButtonProfile1)) new GuiControlProfile (gsButtonProfile1 : gsButtonProfile0)
{

   // fill color
   opaque = false;
   fillColor	= "0 0 0";
   fillColorHL	= "64 64 64";
   fillColorNA	= "255 255 255";

   // border color
   border = 1;
   borderColor   = "0 255 0";
   borderColorHL = "255 0 0";
   borderColorNA = "128 128 128";
};

if(!isObject(gsButtonProfile2)) new GuiControlProfile (gsButtonProfile2 : gsButtonProfile0)
{

   // fill color
   opaque = false;
   fillColor	= "255 255 255";
   fillColorHL	= "200 200 200";
   fillColorNA	= "136 136 136";

   // border color
   border = 3;
   borderColor   = "0 0 0";
   borderColorHL = "64 64 64";
   borderColorNA = "32 32 32";

};

if(!isObject(gsButtonProfile3)) new GuiControlProfile (gsButtonProfile3 : gsButtonProfile0)
{

   // fill color
   opaque = false;
   fillColor	= "255 192 128";
   fillColorHL	= "255 0 255";
   fillColorNA	= "0 255 0";

   // border color
   border = 4;
   borderColor   = "128 192 255";
   borderColorHL = "100 200 100";
   borderColorNA = "64 128 255";

};

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

// 
// This method is in the namespace of ButtonActivator. This control
// uses this command definition to call this method:
//
// command = "$thisControl.toggleButtonsActive();";
//
// When a command method is called by a GUI, it sets the global variable:
// $thisControl to the calling GUI prior to calling the command.
//

function ButtonActivator::toggleButtonsActive( %theControl ) {

	TestPushButton.setActive(! TestPushButton.isActive() );

	TestToggleButton.setActive(! TestToggleButton.isActive() );

	TestRadioButton0.setActive(! TestRadioButton0.isActive() );
	TestRadioButton1.setActive(! TestRadioButton1.isActive() );
	TestRadioButton2.setActive(! TestRadioButton2.isActive() );
}


//
//    The onAction() method is called for all buttons when they are clicked.
//    We want to target this onAction method to a named control, because
//    we do not want to pollute the parent(s) namespace.  Unforutnately,
//    ass, clasname, and superclass DO NOT WORK for GUI controls. Thus,
//    all the theme buttons have been given the same name.  Normally,
//    this is a no no, but because we want to have a onAction() callback that 
//    responds to a key press from any of them, we MUST give them the same name.

function ChangeButtonTheme::onAction( %theControl ) {
	TestPushButton.setProfile(%theControl.theme);

	TestToggleButton.setProfile(%theControl.theme);

	TestRadioButton0.setProfile(%theControl.theme);
	TestRadioButton1.setProfile(%theControl.theme);
	TestRadioButton2.setProfile(%theControl.theme);
}

//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./gsButton.gui");

