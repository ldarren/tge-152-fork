//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading Toon Splash Screen  ---------");

//--------------------------------------------------------------------------
// ifcsSplashToon.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Profiles
//--------------------------------------------------------------------------
new AudioProfile(AudioStartupToon)
{
   filename = "./childrenLaugh.ogg";
   description = "AudioGui";
   preload = true;
};


//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
function ifcsSplashToonFadeinBitmap::onAdd( %this ) {
}

function ifcsSplashToonFadeinBitmap::onRemove( %this ) {
}


function ifcsSplashToonFadeinBitmap::onWake( %this ) {
	%this.done = false;

	%this.taskMgr = newTaskManager();
	%this.taskMgr.setTarget(%this);
	%this.taskMgr.setDefaultTaskDelay(100); 

	// add a repeating task to the task manager and start it running
	%this.taskMgr.addTask( "checkIsDone();", -1 );
	
	%this.taskMgr.selfExecuteTasks( true );

	alxPlay(AudioStartupToon);
}

function ifcsSplashToonFadeinBitmap::onSleep( %this ) {
	%this.taskMgr.stopSelfExecution();
	%this.taskMgr.clearTasks();
	%this.taskMgr.delete();
}

function ifcsSplashToonFadeinBitmap::checkIsDone( %this ) {
	if( %this.done ) {
		%this.taskMgr.stopSelfExecution();
		Canvas.setContent(ifcsMainMenuToon);
	}
}

function ifcsSplashToonFadeinBitmap::click( %this ) {
	%this.done  = true;
}

//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./ifcsSplashToon.gui");



