//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiFilterCtrl Samples ---------");
//--------------------------------------------------------------------------
// gsFilter.cs
//--------------------------------------------------------------------------
// 
// In this file, we demonstrate, the following GuiFilterCtrl features:
//
// 1. GuiFilterCtrl as input control
// 2. GuiFilterCtrl as output control
// 3. Setting new controlPoints value directly.
// 4. Setting knot values to identity

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------

// 1. GuiFilterCtrl as input control
// 2. GuiFilterCtrl as output control
//
// In this example, TestFilter0 is the input control and TestFilter1 is the 
// output control.  
function TestFilter0::onAdd( %theControl ) {
	%theControl.taskMgr = newTaskManager();

	%theControl.taskMgr.setTarget(%theControl);

	%theControl.taskMgr.setDefaultTaskDelay( 32 ); 

	%theControl.taskMgr.addTask( "shareValues();" , -1 );
}


function TestFilter0::onRemove( %theControl ) {
	%theControl.taskMgr.stopSelfExecution();
	%theControl.taskMgr.clearTasks();
	%theControl.taskMgr.delete();
}


function TestFilter0::onWake( %theControl ) {
	%theControl.taskMgr.selfExecuteTasks( true );
}


function TestFilter0::onSleep( %theControl ) {
	%theControl.taskMgr.stopSelfExecution();
}


function TestFilter0::shareValues( %theControl ) {
	TestFilter1.setValue( TestFilter0.getValue() );
}






//
// 3. Setting new controlPoints value directly.
//
// In order to show that you can in fact change the number of control
// points of a filter real-time, I've added four radio buttons that
// use the following methods to change the number of knots in 
// TestFilter0.  Note that when this is updated, the next value
// copy from TestFilter0 to TestFilter1 updates the number of knots
// in TestFilter1.  i.e. By adding more knots values to a setValue()
// call, you can adjust the number of knots in a filter.

function numKnots::onWake( %theControl ) {
	// Be sure we start with radio button 5 clicked
	if (5 == %theControl.text) %theControl.performClick();
}

function numKnots::updateKnots( %theControl ) {
	TestFilter0.controlPoints = %theControl.text;
}


//
// 4. Setting knot values to identity
//
// The only code for this is in the command for button resetFilterButton:
// command = "TestFilter0.identity();";

//
// STOP STOP STOP STOP STOP STOP STOP STOP STOP 
//
// The following methods are just used provide an animation
// and a little hint.  i.e. Inputs travel left to right.
// This is not part of the sample and all FilterEnergyStream::
// methods can be ignored, unless you are just curious.
// 

function FilterEnergyStream::onAdd( %theControl ) {
	%theControl.taskMgr = newTaskManager();

	%theControl.taskMgr.setTarget(%theControl);

	%theControl.taskMgr.setDefaultTaskDelay( 15 ); 

	%theControl.taskMgr.addTask( "scrollMe();" , -1 );
}


function FilterEnergyStream::onRemove( %theControl ) {
	%theControl.taskMgr.stopSelfExecution();
	%theControl.taskMgr.clearTasks();
	%theControl.taskMgr.delete();
}

function FilterEnergyStream::onWake( %theControl ) {
	%theControl.taskMgr.selfExecuteTasks( true );
}

function FilterEnergyStream::onSleep( %theControl ) {
	%theControl.taskMgr.stopSelfExecution();
}

function FilterEnergyStream::scrollMe( %theControl ) {
	%theControl.curX -= 16;

	if( %theControl.curX <= 0) {
		%theControl.curX = 128;
		%theControl.curY = 128;
	}

	%theControl.setValue( %theControl.curX ,  -16);
}

//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./gsFilter.gui");

