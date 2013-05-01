//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiSliderCtrl Samples ---------");
//--------------------------------------------------------------------------
// gsSlider.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// 
// In this file, we demonstrate the GuiSliderCtrl.
//
// This sample is quite simple.  We have two progress bars as outputs and two 
// GuiSliderCtrl controls as inputs.  
//
// The top bar + GuiSliderCtrl demonstrate the fact that the 'command' is 
// executed after we release the mouse button.  We have programmed the top 
// GuiSliderCtrl control's 'command' with:
//
//		command = "SliderOutput0.setValue($thisControl.getValue());";
//
//
// The bottom bar + GuiSliderCtrl demonstrate the fact that the 'altCommand' is 
// executed continuously while the mouse is depressed in this control.  We have 
//  programmed the bottom GuiSliderCtrl control's 'altCommand' with:
//
//		altCommand = "SliderOutput1.setValue(TestSlider1.getValue());";
//
//
// Things to notice:
//
// 1. For the 'command' version, we can use  the $thisControl variable, but
//
// 2. For the 'altCommand' version, we cannot.  Instead we need to name the 
//    control directly.
// 

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./gsSlider.gui");

