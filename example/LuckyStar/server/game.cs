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
   $Server::GameType = "LuckyStar";
   $Server::MissionType = "Competition";

   // Load up all datablocks, objects etc.
   exec("./camera.cs");
   exec("./editor.cs");
   exec("./player.cs");
   exec("./environment.cs");
   exec("./commands.cs");
   exec("./accessories.cs");
   exec("./LuckyStar.cs");

   exec("common/server/lightingSystem.cs");
   exec("./afx/afxDemo.cs"); 
   exec("./afx/afxEffects.cs"); 
}

function onServerDestroyed()
{
   // This function is called as part of a server shutdown.
	if (!isObject(LuckyStarServer)) return;
	switch(LuckyStarServer.gameState)
	{
	case LuckyStarServer.Debug01State:
		LuckyStarServer.stopManualDance();
	case LuckyStarServer.Debug02State:
		LuckyStarServer.stopAutoDance();
	case LuckyStarServer.DanceState:
		LuckyStarServer.stopDemoGame();
	}
}


//-----------------------------------------------------------------------------

function onMissionLoaded()
{
   // Called by loadMission() once the mission is finished loading.
   $Server::MissionName = MissionInfo.name;
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
      dataBlock = LoopingCamData;
      position = "0 0 300 1 0 0 0";
   };
   %this.PathCamera.followPath("MissionGroup/CameraPath01");
   MissionCleanup.add( %this.PathCamera);
   %this.PathCamera.scopeToClient(%this);

   // Create a player object.
   %spawnPoint = pickSpawnPoint(0, 1);
   %this.createPlayer(%spawnPoint);
}

function GameConnection::onClientLeaveGame(%this)
{
   if (isObject(%this.camera))
      %this.camera.delete();
   if (isObject(%this.PathCamera))
      %this.PathCamera.delete();
   if (isObject(%this.player))
      %this.player.delete();
}


//-----------------------------------------------------------------------------

function GameConnection::createPlayer(%this, %spawnPoint)
{
	if (%this.player > 0)  {
		// The client should not have a player currently
		// assigned.  Assigning a new one could result in 
		// a player ghost.
		error( "Attempting to create an angus ghost!" );
	}
	
	// Create the player object
	%player = new Player() {
		dataBlock = PlayerBody;
		client = %this;
	};
/*	%player = new StaticShape() {
		dataBlock = PlayerShapeData;
		client = %this;
	};*/
	MissionCleanup.add(%player);
	%player.mountImage(LeftEmitterBlockImage, 2);
	%player.mountImage(RightEmitterBlockImage, 3);
	%player.setImageTrigger(2, false);
	%player.setImageTrigger(3, false);
	
	// Player setup...
	%player.setTransform(%spawnPoint);
	%player.setShapeName(%this.nameBase);
	
	// Update the camera to start with the player
	%this.camera.setTransform(%player.getEyeTransform());
	
	// Give the client control of the player
	%this.player = %player;
	%this.setupPlayer();
		
	%this.setControlObject(%this.PathCamera);
}

function GameConnection::setupPlayer(%this)
{
	%count = %this.getConnectArgsCount();
	for (%i = 1; %i < %count; %i++) // arg0 == playerName. see LuckyStarClient::loadMission
	{
		%attrib[%i] = %this.getConnectArgs(%i);
	}
	%player = %this.player;
/*
	%hair = new StaticShape()
	{
		dataBlock = (%attrib[1]@"Data");
	};*/
	%hair = (%attrib[1]@"Image");
	
	//%player.mountObject(%hair, 0);
	%player.mountImage(%hair, 0);
	
	%player.addSkinModifier(false, 0, "hair", %attrib[6], "mask", %attrib[2], "128 128 128 255");
	if (%attrib[7] $= "base")
		%player.addSkinModifier(true, 0, "head", %attrib[7], "mask1", %attrib[3]);
	else
		%player.addSkinModifier(true, 0, "head", %attrib[7], "mask2", %attrib[3]);
	%player.addSkinModifier(true, 4, "hands", "base", "mask", %attrib[4], "200 200 200 255");
	%player.addSkinModifier(true, 5, "legs", "base", "mask", %attrib[5]);
	%player.addSkinModifier(true, 1, "shirt", %attrib[8]);
	%player.addSkinModifier(true, 2, "skirt", %attrib[9]);
	%player.addSkinModifier(true, 3, "feet", %attrib[10]);
}

//-----------------------------------------------------------------------------

function pickSpawnPoint(%curr, %total) 
{
	%id = %curr;
	switch(%total) {
	case 2: %id = %curr + 1;
	case 4: %id = %curr + 1;
	}
   %groupName = "MissionGroup/PlayerDropPoints";
   %group = nameToID(%groupName);

   if (%group != -1) {
      %count = %group.getCount();
      if (%count != 0) {
         return %group.getObject(%id).getTransform();
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
