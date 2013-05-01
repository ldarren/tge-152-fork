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
   $Server::GameType = "Torque TTB";
   $Server::MissionType = "None";

   // Load up all datablocks, objects etc.
   exec("./camera.cs");
   exec("./editor.cs");
   exec("./player.cs");
   exec("./logoitem.cs");

   exec("common/server/lightingSystem.cs");
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

   // Create a player object.
   %spawnPoint = pickSpawnPoint();
   %this.createPlayer(%spawnPoint);
}

function GameConnection::onClientLeaveGame(%this)
{
   if (isObject(%this.camera))
      %this.camera.delete();
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
   MissionCleanup.add(%player);

   // Player setup...
   %player.setTransform(%spawnPoint);
   %player.setShapeName(%this.name);
   
   // Update the camera to start with the player
   %this.camera.setTransform(%player.getEyeTransform());

   // Give the client control of the player
   %this.player = %player;
   %this.setControlObject(%player);
}


//-----------------------------------------------------------------------------

function pickSpawnPoint() 
{
   // Pick the first object in drop point group and use it's
   // location as a spawn point.
   %group = nameToID("MissionGroup/PlayerDropPoints");
   if (%group != -1 && %group.getCount() != 0)
      return %group.getObject(0).getTransform();

   // If no object was found, return a point near the center of the world
   error("Missing spawn point object and/or mission group " @ %groupName);
   return "0 0 300 1 0 0 0";
}
