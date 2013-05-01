//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading Tech Splash Screen  ---------");

//--------------------------------------------------------------------------
// ifcsSplashTech.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
function ifcsSplashTechFadeinBitmap::onAdd( %this ) {
}

function ifcsSplashTechFadeinBitmap::onRemove( %this ) {
}


function ifcsSplashTechFadeinBitmap::onWake( %this ) {
	%this.done = false;

	%this.taskMgr = newTaskManager();
	%this.taskMgr.setTarget(%this);
	%this.taskMgr.setDefaultTaskDelay(100); 

	// add a repeating task to the task manager and start it running
	%this.taskMgr.addTask( "checkIsDone();", -1 );

	%this.taskMgr.selfExecuteTasks( true );
}

function ifcsSplashTechFadeinBitmap::onSleep( %this ) {
	%this.taskMgr.stopSelfExecution();
	%this.taskMgr.clearTasks();
	%this.taskMgr.delete();
}

function ifcsSplashTechFadeinBitmap::checkIsDone( %this ) {
	if( %this.done ) {
		%this.taskMgr.stopSelfExecution();
		Canvas.setContent(ifcsMainMenuTech);
	}
}

function ifcsSplashTechFadeinBitmap::click( %this ) {
	%this.done  = true;
}



//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./ifcsSplashTech.gui");



