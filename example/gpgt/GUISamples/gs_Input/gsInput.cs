//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiInputCtrl Samples ---------");
//--------------------------------------------------------------------------
// gsInput.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// 
// In this file, we demonstrates the GuiInputCtrl control
//
// This example is rather straight-forward and is actually a utility of 
// sorts.  There is a button at the top of the screen that can be pressed
// to activate the GuiInputCtrl in this interface.  When active, the 
// GuiInputCtrl (gsTestInputCtrl) will capture all inputs, including keyboard
// and mouse clicks.  These will fire the callback onInputEvent().  Then, for
// the sake of the example/utility, we print all the arguments we have 
// received.  This way, you can see what any key device input will produce.
// 
// To exit this demo, simply hit the escape key twice in a row.  gsTestInputCtrl
// watches for this and disables itself if two escape presses are recorded, but
// only if they come back to back.
// 

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

function gsInput::onWake( %theControl ) {
	deviceString.setText("<NULL>");
	actionString.setText("<NULL>");
	makeOrBreak.setText("<NULL>");
}


// 
// When the button 'captureInputs' is pressed, it will create a new
// GuiInputCtrl named gsTestInputCtrl.  This control immediately begins
// capturing all inputs.
//
function captureInputs::activateInputCtrl( %theControl ) {
	new GuiInputCtrl(gsTestInputCtrl) {
		profile = "GuiInputCtrlProfile";
		visible = "1";
	};

	gsInput.Add(gsTestInputCtrl);
}

//
// When the guiInputCtrl is active, we don't want the global 
// action map catching our inputs, so we will use the onAdd
// and onRemove callbacks to insure that this does not occur.
//
function gsTestInputCtrl::onAdd( %theControl ) 
{
   GlobalActionMap.unbind( keyboard, "f10");
   GlobalActionMap.unbind( keyboard, "f11");
   
}

function gsTestInputCtrl::onRemove( %theControl ) 
{
   GlobalActionMap.bind(keyboard, "f10", GuiEdit);
   GlobalActionMap.bind(keyboard, "f11", toggleEditor);
}

//
// This method (a callback) is responsible for printing all the inputs to 
// the screen and for watching for escape presses.  When we get two 
// escape presses (in a row), we delete the GuiInputCtrl.  Merely disabling
// this control is not enough to make it stop capturing.  
//
// Note:  If it is not active, the onInputEvent() callback will not fire, but
// the control will sink inputs.
//
function gsTestInputCtrl::onInputEvent( %theControl, %deviceString, %actionString, %makeOrBreak ) {
echo("%makeOrBreak == ", %makeOrBreak);
	deviceString.getID().setText(%deviceString);
	actionString.getID().setText(%actionString);
	makeOrBreak.getID().setText(%makeOrBreak);

	if( (%actionString $= "escape") && (1 == %makeOrBreak) &&
	    (%theControl.lastActionString $= "escape") ) {
		deviceString.setText("<NULL>");
		actionString.setText("<NULL>");
		makeOrBreak.setText("<NULL>");
		%theControl.delete();
	} 

 if( 1 == %makeOrBreak )
    %theControl.lastActionString = %actionString;

}



//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
exec("./gsInput.gui");

