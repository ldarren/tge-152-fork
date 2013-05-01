//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading Game GUIs HUDS 0 - Counters ---------");

//--------------------------------------------------------------------------
// ifcsHUDs0.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------


function guiControl::getCounterValue( %this ) {
	return %this.currentCount;
}


function guiControl::setCounterValue( %this , %newCount ) {

	// Check to be sure that the required fields have been set:
	//
	// numDigits     - Number of digits in this counter
	// digitTileName - Prefix for tile names used in this counter (i.e. names of the controls)
	// digitPath     - Path to tiles used in this counter
	//
	if( "" $= %this.numDigits )  return false;
	if( "" $= %this.digitTileName )  return false;
	if( "" $= %this.digitPath )  return false;
	
	// Store the currentCount
	%this.currentCount = %newCount;
	
	%newCountDigits = strlen( %newCount );
	
	if ( %newCountDigits > %this.numDigits ) { // Overflow
		for( %count = 0 ; %count < %this.numDigits ; %count++ ) {
			%tmpDigit[%count] = 9;
		}
	} else {

		// Pad with zeros so our 'newCount' string is exactly %this.numDigits wide

		%tmpNewCount = "";

		for( %count = %this.numDigits - %newCountDigits ; %count > 0 ; %count-- ) {
			%tmpNewCount = %tmpNewCount @ "0";
		}

		%tmpNewCount = %tmpNewCount @ %NewCount; 

		//echo("%tmpNewCount == " @ %tmpNewCount );

		// Get digits in reverse order and store them aside
		for( %count = 0 ; %count < %this.numDigits ; %count++ ) {
			%tmpDigit[%count] = getSubStr( %tmpNewCount , %this.numDigits - 1 - %count , 1 );

			//echo(%count @ " == " @ getSubStr( %tmpNewCount , %this.numDigits - 1 - %count , 1 ));
		}

	}

	// Change the bitmaps for each digit in the display
	for( %count = 0 ; %count < %this.numDigits ; %count++ ) {
		(%this.digitTileName  @ %count).setBitmap( %this.digitBitmap[%tmpDigit[%count]] );
		//echo((%this.digitTileName  @ %count) @ " == " @ %tmpDigit[%count]);
	}

	return true;
}

function guiControl::initializeBitmaps( %this ) {
	if( "" $= %this.digitPath )  return false;

	for( %count = 0; %count < 10 ; %count++ ) {
		%this.digitBitmap[%count] = expandFilename( %this.digitPath @ %count );

		//echo(%count @ " == " @ %this.digitBitmap[%count]);

	}
}



//
// skinnyFrame6DigitCounter
//
function skinnyFrame6DigitCounter::onAdd( %this ) {
	%this.initializeBitmaps();

	// Following is just for demonstration purposes and not needed if you re-use this:
	%this.maxValue = 999999;
	%this.taskMgr = newTaskManager();
	%this.taskMgr.setTarget(%this);
	%this.taskMgr.setDefaultTaskDelay(13); 
	%this.taskMgr.addTask( "advanceCounter(1);", -1 );
}

function skinnyFrame6DigitCounter::onRemove( %this ) {
	// Following is just for demonstration purposes and not needed if you re-use this:
	%this.taskMgr.stopSelfExecution();
	%this.taskMgr.clearTasks();
	%this.taskMgr.delete();
}

//
// FatFrame6DigitCounter
//
function FatFrame6DigitCounter::onAdd( %this ) {
	%this.initializeBitmaps();

	// Following is just for demonstration purposes and not needed if you re-use this:
	%this.maxValue = 999999;
	%this.taskMgr = newTaskManager();
	%this.taskMgr.setTarget(%this);
	%this.taskMgr.setDefaultTaskDelay(50); 
	%this.taskMgr.addTask( "advanceCounter(12357);", -1 );
}

function FatFrame6DigitCounter::onRemove( %this ) {
	// Following is just for demonstration purposes and not needed if you re-use this:
	%this.taskMgr.stopSelfExecution();
	%this.taskMgr.clearTasks();
	%this.taskMgr.delete();
}

//
// arcade5DigitCounter
//
function arcade5DigitCounter::onAdd( %this ) {
	%this.initializeBitmaps();

	// Following is just for demonstration purposes and not needed if you re-use this:
	%this.maxValue = 99999;
	%this.taskMgr = newTaskManager();
	%this.taskMgr.setTarget(%this);
	%this.taskMgr.setDefaultTaskDelay(200); 
	%this.taskMgr.addTask( "advanceCounter(13);", -1 );
}

function arcade5DigitCounter::onRemove( %this ) {
	// Following is just for demonstration purposes and not needed if you re-use this:
	%this.taskMgr.stopSelfExecution();
	%this.taskMgr.clearTasks();
	%this.taskMgr.delete();
}

//
// overlapping6DigitComicCounter
//
function overlapping6DigitComicCounter::onAdd( %this ) {
	%this.initializeBitmaps();

	// Following is just for demonstration purposes and not needed if you re-use this:
	%this.maxValue = 999999;
	%this.taskMgr = newTaskManager();
	%this.taskMgr.setTarget(%this);
	%this.taskMgr.setDefaultTaskDelay(100); 
	%this.taskMgr.addTask( "advanceCounter(1);", -1 );
}

function overlapping6DigitComicCounter::onRemove( %this ) {
	// Following is just for demonstration purposes and not needed if you re-use this:
	%this.taskMgr.stopSelfExecution();
	%this.taskMgr.clearTasks();
	%this.taskMgr.delete();
}


//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./ifcsHUDs0.gui");




/// STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  
//
// All the following scripts are used to demonstrate the various HUDS in this sample.
// You do not need these specific version of the onWake(), onSleep(), etc. methods
// for your game(s).
//
/// STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  STOP  

//
// skinnyFrame6DigitCounter
//
function skinnyFrame6DigitCounter::onWake( %this ) {
	%this.taskMgr.selfExecuteTasks( true );
}

function skinnyFrame6DigitCounter::onSleep( %this ) {
	%this.taskMgr.stopSelfExecution();
}

//
// FatFrame6DigitCounter
//
function FatFrame6DigitCounter::onWake( %this ) {
	%this.taskMgr.selfExecuteTasks( true );
}

function FatFrame6DigitCounter::onSleep( %this ) {
	%this.taskMgr.stopSelfExecution();
}

//
// arcade5DigitCounter
//
function arcade5DigitCounter::onWake( %this ) {
	%this.taskMgr.selfExecuteTasks( true );
}

function arcade5DigitCounter::onSleep( %this ) {
	%this.taskMgr.stopSelfExecution();
}

//
// overlapping6DigitComicCounter
//
function overlapping6DigitComicCounter::onWake( %this ) {
	%this.taskMgr.selfExecuteTasks( true );
}

function overlapping6DigitComicCounter::onSleep( %this ) {
	%this.taskMgr.stopSelfExecution();
}


function guiControl::advanceCounter( %this , %increment ) {
	if( %this.maxValue < %this.getCounterValue() ) { // Roll counter over

		%this.setCounterValue( %this.maxValue - %this.getCounterValue()); 

	} else { // Advance the counter

		%this.setCounterValue( %this.getCounterValue() + %increment );

	}
}


