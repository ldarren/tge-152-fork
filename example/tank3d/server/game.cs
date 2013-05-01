//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Server and mission initialization
//-----------------------------------------------------------------------------

function onServerCreated()
{
   // This function is called when a server is constructed.

   // Master server information for multiplayer games
   $Server::GameType = "Tank Attack 3D TTB";
   $Server::MissionType = "None";
	
	// Audio Profiles
	exec("./audioProfiles.cs");
	
	// Camera
	exec("./camera.cs");
	
	// Weapon
 	exec("./moogun.cs");

	// Load up all datablocks, objects etc.
	exec("./shapebase.cs");
	exec("./editor.cs");
    exec("./player.cs");
    exec("./tank01.cs");       // Tank  1 (Player Object)
    exec("./tank02.cs");       // Tank  2 (Player Object)
	exec("./tank03.cs");       // Tank  3 (Player Object)
	exec("./particles.cs");
	exec("./environment.cs");
	
	
	//Cell Shading the crap below
	exec("./explode.cs");
	exec("./explosion.cs");
	exec("./build01.cs");
	exec("./build02.cs");
    exec("./tangki.cs");
	exec("./tianglampu.cs");
	exec("./tree01.cs");
	exec("./tree02.cs");
	exec("./fern01.cs");
	exec("./adbanner.cs");
	exec("./cautionlight.cs");
	
	//Game Engine script
	exec("./dbconnection.cs");
    exec("./spawn.cs");
    exec("./waitinglist.cs");
	exec("./user.cs");
	exec("./rotate.cs");
	exec("./highscore.cs");
	exec("./message.cs");
	exec("./engine.cs");
}

function onServerDestroyed()
{
   // This function is called as part of a server shutdown.
}


//-----------------------------------------------------------------------------

function onMissionLoaded()
{
   // Called by loadMission() once the mission is finished loading.
}

function onMissionEnded()
{
   // Called by endMission(), right before the mission is destroyed
}


//-----------------------------------------------------------------------------
// Dealing with client connections
// These methods are extensions to the GameConnection class. Extending
// GameConnection make is easier to deal with some of this functionality,
// but these could also be implemented as stand-alone functions.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function GameConnection::onClientEnterGame(%this)
{
   // Every client get's a camera object.
   %this.camera = new Camera() {
      dataBlock = Observer;
   };
   MissionCleanup.add( %this.camera );
   %this.camera.scopeToClient(%this);
	
   // Create path camera
   %this.PathCamera = new PathCamera() {
      dataBlock = LoopingCam;
      position = "0 0 300 1 0 0 0";
   };
   %this.PathCamera.followPath("MissionGroup/CameraPath01");
   MissionCleanup.add( %this.PathCamera);
   %this.PathCamera.scopeToClient(%this);

   // Create a player object.
   %this.createPlayer($StaticCameraPoint3);
}

function GameConnection::onClientLeaveGame(%this)
{
   if (isObject(%this.camera))
      %this.camera.delete();
   if (isObject(%this.PathCamera))
      %this.PathCamera.delete();
   if (isObject($arrDbInput))
		$arrDbInput.delete();
   if (isObject($arrWaitList))
		$arrWaitList.delete();
}


//-----------------------------------------------------------------------------

function GameConnection::createPlayer(%this, %spawnPoint)
{
   if (%this.player > 0)  {
      // The client should not have a player currently
      // assigned.  Assigning a new one could result in 
      // a player ghost.
      error( "Attempting to create an invalid ghost!" );
   }

   // Update the camera to start with the static position
	%this.camera.setTransform(%spawnPoint);

	$mvPitch = 0.45;

	// Give the client control of the camera
	//%this.setControlObject(%this.camera);
	%this.setControlObject(%this.PathCamera);
	
	// Start Game Input Loop (REFER dbconnection.cs & engine.cs)
	resetDb();
	initGame(%this);
}

//-----------------------------------------------------------------------------

