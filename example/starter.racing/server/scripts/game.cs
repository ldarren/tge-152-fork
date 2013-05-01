//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Pause while looking over the end game screen (in secs)
$Game::EndGamePause = 10;

// Pause until race starts.
$Game::WaitTime = 5000;

// The amount of laps to race through.
$Game::Laps = 2;

//-----------------------------------------------------------------------------
//  Functions that implement game-play
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function onServerCreated()
{
   // Server::GameType is sent to the master server.
   // This variable should uniquely identify your game and/or mod.
   $Server::GameType = "Racing Starter Kit";

   // Server::MissionType sent to the master server.  Clients can
   // filter servers based on mission type.
   $Server::MissionType = "Racing";

   // GameStartTime is the sim time the game started. Used to calculated
   // game elapsed time.
   $Game::StartTime = 0;

   // Load up all datablocks, objects etc.  This function is called when
   // a server is constructed.
   exec("./audioProfiles.cs");
   exec("./camera.cs");
   exec("./markers.cs"); 
   exec("./triggers.cs"); 
   exec("./shapeBase.cs");
   exec("./staticShape.cs");
   exec("./radiusDamage.cs");
   exec("./car.cs");
   exec("./checkpoint.cs");

   // Keep track of when the game started
   $Game::StartTime = $Sim::Time;
}

function onServerDestroyed()
{
   // This function is called as part of a server shutdown.
}


//-----------------------------------------------------------------------------

function onMissionLoaded()
{
   // Called by loadMission() once the mission is finished loading.
   // Nothing special for now, just start up the game play.
   startGame();
}

function onMissionEnded()
{
   // Called by endMission(), right before the mission is destroyed

   // Normally the game should be ended first before the next
   // mission is loaded, this is here in case loadMission has been
   // called directly.  The mission will be ended if the server
   // is destroyed, so we only need to cleanup here.
   cancel($Game::Schedule);
   $Game::Running = false;
   $Game::Cycling = false;
}


//-----------------------------------------------------------------------------

function startGame()
{
   if ($Game::Running) {
      error("startGame: End the game first!");
      return;
   }

    schedule($Game::WaitTime, 0, "countThree");
}

function countThree()
{
   // Tell the client to count.
   for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
      %cl = ClientGroup.getObject( %clientIndex );
      commandToClient(%cl, 'SetCounter', 3);
   }

	schedule(1000, 0, "countTwo");
}

function countTwo()
{
   // Tell the client to count.
   for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
      %cl = ClientGroup.getObject( %clientIndex );
      commandToClient(%cl, 'SetCounter', 2);
   }

	schedule(1000, 0, "countOne");
}

function countOne()
{
   // Tell the client to count.
   for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
      %cl = ClientGroup.getObject( %clientIndex );
      commandToClient(%cl, 'SetCounter', 1);
   }

	schedule(1000, 0, "startRace");
}

function startRace()
{
   // Inform the client we're starting up
   for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
      %cl = ClientGroup.getObject( %clientIndex );
      commandToClient(%cl, 'GameStart');

      // Other client specific setup..
      %cl.lap = 0;
      %cl.nextCheck = 1;
      %cl.setControlObject(%cl.car);
      %cl.camera.setFlyMode();
   }

   // Start the game timer
//   if ($Game::Duration)
//      $Game::Schedule = schedule($Game::Duration * 1000, 0, "onGameDurationEnd" );
   $Game::Running = true;
   
   $Game::Running = true;
}

function endGame()
{
   if (!$Game::Running)  {
      error("endGame: No game running!");
      return;
   }
   
   // Stop any game timers
   cancel($Game::Schedule);

   // Inform the client the game is over
   for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
      %cl = ClientGroup.getObject( %clientIndex );
      commandToClient(%cl, 'GameEnd');
   }

   // Delete all the temporary mission objects
   resetMission();
   $Game::Running = false;
}

function onGameDurationEnd()
{
   // This "redirect" is here so that we can abort the game cycle if
   // the $Game::Duration variable has been cleared, without having
   // to have a function to cancel the schedule.
   if ($Game::Duration && !isObject(EditorGui))
      cycleGame();
}


//-----------------------------------------------------------------------------

function cycleGame()
{
   // This is setup as a schedule so that this function can be called
   // directly from object callbacks.  Object callbacks have to be
   // carefull about invoking server functions that could cause
   // their object to be deleted.
   if (!$Game::Cycling) {
      $Game::Cycling = true;
      $Game::Schedule = schedule(0, 0, "onCycleExec");
   }
}

function onCycleExec()
{
   // End the current game and start another one, we'll pause for a little
   // so the end game victory screen can be examined by the clients.
   endGame();
   $Game::Schedule = schedule($Game::EndGamePause * 1000, 0, "onCyclePauseEnd");
}

function onCyclePauseEnd()
{
   $Game::Cycling = false;

   // Just cycle through the missions for now.
   %search = $Server::MissionFileSpec;
   for (%file = findFirstFile(%search); %file !$= ""; %file = findNextFile(%search)) {
      if (%file $= $Server::MissionFile) {
         // Get the next one, back to the first if there
         // is no next.
         %file = findNextFile(%search);
         if (%file $= "")
           %file = findFirstFile(%search);
         break;
      }
   }
   loadMission(%file);
}


//-----------------------------------------------------------------------------
// GameConnection Methods
// These methods are extensions to the GameConnection class. Extending
// GameConnection make is easier to deal with some of this functionality,
// but these could also be implemented as stand-alone functions.
//-----------------------------------------------------------------------------

function GameConnection::onClientEnterGame(%this)
{
   commandToClient(%this, 'SyncClock', $Sim::Time - $Game::StartTime);
   commandToClient(%this, 'SetMaxLaps', $Game::Laps);
   
   // Create a new camera object.
   %this.camera = new Camera() {
      dataBlock = Observer;
   };
   MissionCleanup.add( %this.camera );
   %this.camera.scopeToClient(%this);

   // Client controls the camera by default.
   %this.setControlObject(%this.camera);

	if(!$Game::Running)
	{
		// Create a car object.
		%this.spawnCar();

         // Orbit the camera around the car
		%this.camera.setOrbitMode(%this.car, %this.car.getTransform(), 0.5, 4.5, 4.5);
	}
}

function GameConnection::onClientLeaveGame(%this)
{
   if (isObject(%this.camera))
      %this.camera.delete();
   if (isObject(%this.car))
      %this.car.delete();
}
              
//-----------------------------------------------------------------------------

function GameConnection::onLeaveMissionArea(%this)
{
   // The control objects invoked this method when they
   // move out of the mission area.
}

function GameConnection::onEnterMissionArea(%this)
{
   // The control objects invoked this method when they
   // move back into the mission area.
}

//-----------------------------------------------------------------------------

function GameConnection::spawnCar(%this)
{
   // Combination create player and drop him somewhere
   %spawnPoint = pickSpawnPoint();
   %this.createCar(%spawnPoint);
}   

//-----------------------------------------------------------------------------

function GameConnection::createCar(%this, %spawnPoint)
{
   if (%this.car > 0)  {
      // The client should not have a player currently
      // assigned.  Assigning a new one could result in 
      // a player ghost.
      error( "Attempting to create an angus ghost!" );
   }

   // Create the player object
   %car = new WheeledVehicle() {
      dataBlock = DefaultCar;
      client = %this;
   };
   MissionCleanup.add(%car);

   // Player setup...
   %car.setTransform(%spawnPoint);
   %car.setShapeName(%this.name);
   
   // Update the camera to start with the player
   %this.camera.setTransform(%car.getEyeTransform());

   // Give the client control of the player
   %this.car = %car;
}


//-----------------------------------------------------------------------------
// Support functions
//-----------------------------------------------------------------------------

function pickSpawnPoint() 
{
   %groupName = "MissionGroup/PlayerDropPoints";
   %group = nameToID(%groupName);

   if (%group != -1) {
      %count = %group.getCount();
      if (%count != 0) {
         %index = getRandom(%count-1);
         %spawn = %group.getObject(%index);
         return %spawn.getTransform();
      }
      else
         error("No spawn points found in " @ %groupName);
   }
   else
      error("Missing spawn points group " @ %groupName);

   // Could be no spawn points, in which case we'll stick the
   // player at the center of the world.
   return "0 0 300 1 0 0 0";
}
