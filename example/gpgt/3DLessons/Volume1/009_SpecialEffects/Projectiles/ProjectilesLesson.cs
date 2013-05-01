//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// *************************** LOAD DATABLOCKS FOR LESSON
//-----------------------------------------------------------------------------
exec("./ProjectilesLessonDBs.cs");

//-----------------------------------------------------------------------------
// *************************** LOAD METHODS and FUNCTIONS FOR LESSON
//-----------------------------------------------------------------------------
exec("./ProjectilesLessonMethods.cs");

//-----------------------------------------------------------------------------
// *************************** DEFINE THE *STANDARD* LESSON METHODS 
//-----------------------------------------------------------------------------

function ProjectilesLesson::onAdd(%this) 
{
	DefaultLessonPrep();

	%this.taskMgr = newTaskManager();
	%this.taskMgr.setDefaultTaskDelay(500); 
}

function ProjectilesLesson::onRemove(%this) 
{

	%this.taskMgr.stopSelfExecution();
	%this.taskMgr.clearTasks();
	%this.taskMgr.delete();
}

function ProjectilesLesson::ExecuteLesson(%this) 
{
	//
	// Non-Ballistic Projectile
	//

	%startPos		= CalculateObjectDropPosition( North10.getPosition() , "0 0 2" );

	%pointingVector = "0 1 0"; // North

	%velocity		= 10; // m/s


	%task = "standaloneProjectile( BlueEnergyProjectile,\"" @ 
	                               %startPos @ "\" , \"" @  
								   %pointingVector @ "\" , " @  
								   %velocity @ ");";

	%this.taskMgr.addTask( %task , -1 );


	//
	// Long Delay (Bouncy) Projectile
	//

	%startPos		= CalculateObjectDropPosition( South10.getPosition() , "0 0 4" );

	%pointingVector = "0 -1 -1.2"; // South

	%velocity		= 8; // m/s


	%task = "standaloneProjectile( BlueEnergyProjectileLongDelay,\"" @ 
	                               %startPos @ "\" , \"" @  
								   %pointingVector @ "\" , " @  
								   %velocity @ ");";

	%this.taskMgr.addTask( %task , -1 );
	

	//
	// Ballistic Projectile
	//

	%startPos		= CalculateObjectDropPosition( West10.getPosition() , "0 0 2" );

	%pointingVector = "-1 0 1"; // East and Up

	%velocity		= 10; // m/s


	%task = "standaloneProjectile( BlueEnergyProjectileBallistic,\"" @ 
	                               %startPos @ "\" , \"" @  
								   %pointingVector @ "\" , " @  
								   %velocity @ ");";

	%this.taskMgr.addTask( %task , -1 );

	//
	// Start up the task Manager
	//

	%this.taskMgr.selfExecuteTasks( true );

}

