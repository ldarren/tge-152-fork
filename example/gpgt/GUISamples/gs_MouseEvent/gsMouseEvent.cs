//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading TestMouseEvent Samples ---------");
//--------------------------------------------------------------------------
// gsMouseEvent.cs
//--------------------------------------------------------------------------

// 
// In this file, we demonstrate, the following GuiChunkedBitmapCtrl features:
//
// 1. Tiling

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
function gsMouseEvent::onAdd( %theControl ) {
	%theControl.lightOn = expandFileName("./lighton");
	%theControl.lightOff = expandFileName("./lightoff");

	%theControl.ctrlPress = expandFileName("./ctrl_press");
	%theControl.ctrlUnpress = expandFileName("./ctrl_unpress");

	%theControl.altPress = expandFileName("./alt_press");
	%theControl.altUnpress = expandFileName("./alt_unpress");

	%theControl.shiftPress = expandFileName("./shift_press");
	%theControl.shiftUnpress = expandFileName("./shift_unpress");

	%theControl.lightBarOn = expandFileName("./lightbarOn");
	%theControl.lightBarOff = expandFileName("./lightbarOff");

}


function gsMouseEvent::onWake( %theControl ) {
	LeftPressMouse.setVisible(false);
	MiddlePressMouse.setVisible(false);
	RightPressMouse.setVisible(false);

	ctrlKey.setBitmap(gsMouseEvent.ctrlUnpress);
	altKey.setBitmap(gsMouseEvent.altUnpress);
	shiftKey.setBitmap(gsMouseEvent.shiftUnpress);

	onEnterLight.setBitmap(gsMouseEvent.lightOff);
	onLeaveLight.setBitmap(gsMouseEvent.lightOff);
	onMouseMoveLight.setBitmap(gsMouseEvent.lightOff);

	onMouseDragLight.setBitmap(gsMouseEvent.lightOff);
	onMouseRightDragLight.setBitmap(gsMouseEvent.lightOff);

	mouseFollower.setBitmap(gsMouseEvent.lightOff);

	clickBar1.setBitmap(gsMouseEvent.lightBarOff);
	clickBar2.setBitmap(gsMouseEvent.lightBarOff);
	clickBar3.setBitmap(gsMouseEvent.lightBarOff);
	clickBar4.setBitmap(gsMouseEvent.lightBarOff);
	clickBar5.setBitmap(gsMouseEvent.lightBarOff);


	%theControl.lastSchedule = %theControl.schedule( 750 , turnOffLights );
}

function gsMouseEvent::onSleep( %theControl ) {
	cancel(%theControl.lastSchedule);
}

function gsMouseEvent::turnOffLights( %theControl ) {
	onEnterLight.setBitmap(gsMouseEvent.lightOff);
	onLeaveLight.setBitmap(gsMouseEvent.lightOff);
	onMouseMoveLight.setBitmap(gsMouseEvent.lightOff);

	ctrlKey.setBitmap(gsMouseEvent.ctrlUnpress);
	altKey.setBitmap(gsMouseEvent.altUnpress);
	shiftKey.setBitmap(gsMouseEvent.shiftUnpress);

	onMouseDragLight.setBitmap(gsMouseEvent.lightOff);
	onMouseRightDragLight.setBitmap(gsMouseEvent.lightOff);

	mouseFollower.setBitmap(gsMouseEvent.lightOff);

	clickBar1.setBitmap(gsMouseEvent.lightBarOff);
	clickBar2.setBitmap(gsMouseEvent.lightBarOff);
	clickBar3.setBitmap(gsMouseEvent.lightBarOff);
	clickBar4.setBitmap(gsMouseEvent.lightBarOff);
	clickBar5.setBitmap(gsMouseEvent.lightBarOff);

	%theControl.lastSchedule = %theControl.schedule( 750 , turnOffLights );
}

function TestMouseEvent::moveFollower( %theControl , %XY ) {

   %tmpControl = %theControl.getGroup();

   %Offset = %XY;

   while( isObject( %tmpControl ) ) 
   {
      %Offset = vectorSub( %Offset , %tmpControl.position );
      %tmpControl = %tmpControl.getGroup();
   }

   mouseFollower.position = %Offset;

}


function TestMouseEvent::showModifierKeys( %theControl , %eventModifier , %numMouseClicks ) {

	%ctrlMask = ($EventModifier::LCTRL | $EventModifier::RCTRL | $EventModifier::CTRL);
	%altMask = ($EventModifier::LALT | $EventModifier::RALT | $EventModifier::ALT);
	%shiftMask = ($EventModifier::LSHIFT | $EventModifier::RSHIFT | $EventModifier::SHIFT);

	// CTRL
	if( %ctrlMask & %eventModifier ) {
		ctrlKey.setBitmap(gsMouseEvent.ctrlPress);
	}

	// ALT
	if( %altMask & %eventModifier ) {
		altKey.setBitmap(gsMouseEvent.altPress);
	}

	// SHIFT
	if( %shiftMask & %eventModifier ) {
		shiftKey.setBitmap(gsMouseEvent.shiftPress);
	}

	echo("\c3" @ "%numMouseClicks == " @ %numMouseClicks);

	switch( %numMouseClicks ) {
	case 0:
		return;
	case 1:
		clickBar1.setBitmap(gsMouseEvent.lightBarOn);
	case 2:
		clickBar1.setBitmap(gsMouseEvent.lightBarOn);
		clickBar2.setBitmap(gsMouseEvent.lightBarOn);
	case 3:
		clickBar1.setBitmap(gsMouseEvent.lightBarOn);
		clickBar2.setBitmap(gsMouseEvent.lightBarOn);
		clickBar3.setBitmap(gsMouseEvent.lightBarOn);
	case 4:
		clickBar1.setBitmap(gsMouseEvent.lightBarOn);
		clickBar2.setBitmap(gsMouseEvent.lightBarOn);
		clickBar3.setBitmap(gsMouseEvent.lightBarOn);
		clickBar4.setBitmap(gsMouseEvent.lightBarOn);
	default:
		clickBar1.setBitmap(gsMouseEvent.lightBarOn);
		clickBar2.setBitmap(gsMouseEvent.lightBarOn);
		clickBar3.setBitmap(gsMouseEvent.lightBarOn);
		clickBar4.setBitmap(gsMouseEvent.lightBarOn);
		clickBar5.setBitmap(gsMouseEvent.lightBarOn);
	}

}


function TestMouseEvent::onMouseDown( %theControl , %eventModifier , %XY, %numMouseClicks ) {
	LeftPressMouse.setVisible(true);
	
	echo("\c3" @ %eventModifier SPC "::" SPC %XY SPC "::" SPC %numMouseClicks );

	%theControl.showModifierKeys( %eventModifier , %numMouseClicks);
}

function TestMouseEvent::onMouseUp( %theControl , %eventModifier , %XY, %numMouseClicks ) {
	LeftPressMouse.setVisible(false);

	%theControl.showModifierKeys( %eventModifier , %numMouseClicks);
}

function TestMouseEvent::onRightMouseDown( %theControl , %eventModifier , %XY, %numMouseClicks ) {
	RightPressMouse.setVisible(true);

	%theControl.showModifierKeys( %eventModifier , %numMouseClicks);
}

function TestMouseEvent::onRightMouseUp( %theControl , %eventModifier , %XY, %numMouseClicks ) {
	RightPressMouse.setVisible(false);

	%theControl.showModifierKeys( %eventModifier , %numMouseClicks);
}

function TestMouseEvent::onMouseMove( %theControl , %eventModifier , %XY, %numMouseClicks ) {

	onMouseMoveLight.setBitmap(gsMouseEvent.lightOn);

	mouseFollower.setBitmap(gsMouseEvent.lightOn);

	%theControl.showModifierKeys( %eventModifier , %numMouseClicks);

	%theControl.moveFollower( %XY );
}

function TestMouseEvent::onMouseDragged( %theControl , %eventModifier , %XY, %numMouseClicks ) {
	onMouseDragLight.setBitmap(gsMouseEvent.lightOn);

	%theControl.showModifierKeys( %eventModifier , %numMouseClicks);
}

function TestMouseEvent::onRightMouseDragged( %theControl , %eventModifier , %XY, %numMouseClicks ) {
	onMouseRightDragLight.setBitmap(gsMouseEvent.lightOn);

	%theControl.showModifierKeys( %eventModifier , %numMouseClicks);
}

function TestMouseEvent::onMouseEnter( %theControl , %eventModifier , %XY, %numMouseClicks ) {
	onEnterLight.setBitmap(gsMouseEvent.lightOn);

	%theControl.showModifierKeys( %eventModifier , %numMouseClicks);

	%theControl.moveFollower( %XY );
}

function TestMouseEvent::onMouseLeave( %theControl , %eventModifier , %XY, %numMouseClicks ) {
	onLeaveLight.setBitmap(gsMouseEvent.lightOn);

	%theControl.showModifierKeys( %eventModifier , %numMouseClicks);
}

//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
exec("./gsMouseEvent.gui");

