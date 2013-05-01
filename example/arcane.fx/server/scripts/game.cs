//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Game duration in secs, no limit if the duration is set to 0
$Game::Duration = 0; // AFX

// When a client score reaches this value, the game is ended.
$Game::EndGameScore = 30;

// Pause while looking over the end game screen (in secs)
$Game::EndGamePause = 10;


//-----------------------------------------------------------------------------
//  Functions that implement game-play
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function onServerCreated()
{
   // Server::GameType is sent to the master server.
   // This variable should uniquely identify your game and/or mod.
   $Server::GameType = "Arcane-FX Demo"; // AFX

   // Server::MissionType sent to the master server.  Clients can
   // filter servers based on mission type.
   $Server::MissionType = "MageVSMage"; // AFX

   // GameStartTime is the sim time the game started. Used to calculated
   // game elapsed time.
   $Game::StartTime = 0;

   // Load up all datablocks, objects etc.  This function is called when
   // a server is constructed.
   exec("./audioProfiles.cs");
   exec("./camera.cs");
   exec("./markers.cs"); 
   exec("./triggers.cs"); 
   exec("./inventory.cs");
   exec("./shapeBase.cs");
   exec("./item.cs");
   exec("./staticShape.cs");
   exec("./weapon.cs");
   exec("./radiusDamage.cs");
   exec("./crossbow.cs");
   exec("./player.cs");
   exec("./aiPlayer.cs");

   // AFX CODE BLOCK <<
   if (isFile("common/synapseGaming/contentPacks/lightingPack/sgDeployClient.cs"))
   {
     echo("Initializing TLK Server support");
     exec("common/synapseGaming/contentPacks/lightingPack/sgDeployServer.cs");
   }

   if (isFile("common/server/lightingSystem.cs"))
   {
     echo("Initializing TLK Server support");
     exec("common/server/lightingSystem.cs");
   }
   // AFX CODE BLOCK >>

   // AFX CODE BLOCK <<
   // initialize AFX effects
   exec("./afxEffects.cs"); 
   // AFX CODE BLOCK >>

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

   // DATABLOCK CACHE CODE <<
   if ($Pref::Server::EnableDatablockCache)
     resetDatablockCache();
   // DATABLOCK CACHE CODE >>

   afxEndMissionNotify();
}


//-----------------------------------------------------------------------------

function startGame()
{
   if ($Game::Running) {
      error("startGame: End the game first!");
      return;
   }

   // Inform the client we're starting up
   for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) {
      %cl = ClientGroup.getObject( %clientIndex );
      commandToClient(%cl, 'GameStart');

      // Other client specific setup..
      %cl.score = 0;
   }

   // Start the game timer
   if ($Game::Duration)
      $Game::Schedule = schedule($Game::Duration * 1000, 0, "onGameDurationEnd" );
   $Game::Running = true;
   
   startAIManager();
}

function endGame()
{
   if (!$Game::Running)  {
      error("endGame: No game running!");
      return;
   }
   
   stopsAIManager();

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

//-----------------------------------------------------------------------------

function GameConnection::onClientEnterGame(%this)
{
   commandToClient(%this, 'SyncClock', $Sim::Time - $Game::StartTime);

   %this.camera = new afxCamera(DefaultCamera)
   {
      dataBlock = DefaultCameraData;
   };
   MissionCleanup.add( %this.camera );
   %this.camera.scopeToClient(%this);

   // Setup game parameters, the onConnect method currently starts
   // everyone with a 0 score.
   %this.score = 0;

   // Create a player object.
   %this.spawnPlayer(true);
}

function GameConnection::onClientLeaveGame(%this)
{
   if (isObject(%this.camera))
      %this.camera.delete();
   if (isObject(%this.player))
      %this.player.delete();
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

function GameConnection::onDeath(%this, %sourceObject, %sourceClient, %damageType, %damLoc)
{
  // clear connections camera
  %this.clearCameraObject();

  %corpse = %this.player;

  // interrupt active spellcasting
  if (%this.player.spellBeingCast != 0)
  {
    %spell = %this.player.spellBeingCast;
    %spell.interrupt();
  }
  
  // Clear out the name on the corpse
  %corpse.setShapeName("dead orc");
  %corpse.setEnergyLevel(0);

  // Switch the client over to the death cam and unhook the player object.
  if (isObject(%this.camera) && isObject(%corpse)) 
  {
    //%this.camera.setMode("Corpse",%corpse);
    %this.camera.setOrbitMode(%corpse, %corpse.getTransform(), 0.5, 4.5, 4.5);
    %this.setControlObject(%this.camera);
    snapAtlasGeometryMorph();
  }
  
  // Dole out points and display an appropriate message
  if (%damageType $= "SelfDestruction" || %sourceClient == %this) 
  {
    %this.incScore(-1);
    messageAll('MsgClientKilled','%1 takes his own life!',%this.name);
  }
  else if (%sourceClient != 0)
  {
    %sourceClient.incScore(1);
    messageAll('MsgClientKilled','%1 gets nailed by %2!',%this.name,%sourceClient.name);
    if (%sourceClient.score >= $Game::EndGameScore)
      cycleGame();
  }
}


//-----------------------------------------------------------------------------

function GameConnection::spawnPlayer(%this, %first_spawn, %xfm)
{
   // clear out old linkages between player and client
   if (%this.player)
   {
     %this.player.client = 0;
     %this.player = 0;
   }

   // create player and drop him somewhere
   if (%xfm != 0)
     %spawnPoint = %xfm;
   else
    %spawnPoint = pickSpawnPoint();
   %this.createPlayer(%spawnPoint, %first_spawn);
}   


//-----------------------------------------------------------------------------

function sendPlayerSpellBookToClient(%client, %spellbook)
{
  %ghost_idx = %client.GetGhostIndex(%spellbook);
  //echo("SPELLBOOK ON SERVER " @ %spellbook @ " " @ %ghost_idx);
  if (%ghost_idx == -1)
    schedule(100, 0, "sendPlayerSpellBookToClient", %client, %spellbook);
  else
    commandToClient(%client, 'SetPlayerSpellBook', %ghost_idx);
}

function sendClientPlayerToClient(%client, %player)
{
  %ghost_idx = %client.GetGhostIndex(%player);
  //echo("PLAYER ON SERVER " @ %player @ " " @ %ghost_idx);
  if (%ghost_idx == -1)
    schedule(100, 0, "sendClientPlayerToClient", %client, %player);
  else
    commandToClient(%client, 'SetClientPlayer', %ghost_idx);
}

function GameConnection::createPlayer(%this, %spawnPoint, %first_spawn)
{
  if (%this.player > 0)
    error("Attempted to create an extra player character!");

  // Create the player object
  %player = new Player("OrcMagePlayer") 
  {
    dataBlock = OrcMageAvatar;
    client = %this;
  };

  %player_name = stripChars(detag(getTaggedString(%this.name)),"\cp\co\c6\c7\c8\c9");
  %player.setShapeName(%player_name);
  %player.setEnergyLevel(50);
  %player.setLookAnimationOverride(true);
  %player.setTransform(%spawnPoint);
  MissionCleanup.add(%player);

  // Create player's spellbook
  %my_spellbook = new afxSpellBook(SpellBook) 
  {
    dataBlock = SpellBookData;
  };
  MissionCleanup.add(%my_spellbook);

  %this.spellbook = %my_spellbook;

  schedule(0, 0, "sendClientPlayerToClient", %this, %player);
  schedule(0, 0, "sendPlayerSpellBookToClient", %this, %my_spellbook);

  // Update the camera to start with the player
  %this.camera.setTransform(%player.getEyeTransform());

  // Give the client control of the player
  %this.player = %player;
  %this.setControlObject(%player);

  // We set the camera system to run in 3rd person mode around the %player
  %this.camera.setCameraSubject(%player);
  %this.camera.setThirdPersonMode();
  %this.camera.setThirdPersonOffset("0 -3 3");
  %this.camera.setThirdPersonSnap();
  %this.setCameraObject(%this.camera);

  if (%first_spawn)
    DisplayScreenMessage(%this, "Welcome to the Arcane-FX Demo.");
  else
    DisplayScreenMessage(%this, "Welcome back.");

}

function BroadcastClearObjectSelection(%match_obj)
{
  %count = ClientGroup.getCount();
  for (%i = 0; %i < %count; %i++)
  {
    %cl = ClientGroup.getObject(%i);
    if( !%cl.isAIControlled() )
    {
      if (isObject(%match_obj))
      {
        %sel_obj = %cl.getSelectedObj();
        if (%sel_obj $= %match_obj)
          %cl.clearSelectedObj();
      }
      else
      {
        %cl.clearSelectedObj();
      }
    }
  }
}

//-----------------------------------------------------------------------------
// Support functions
//-----------------------------------------------------------------------------

function pickSpawnPoint(%groupName) 
{
   if (!isObject(%groupName))
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
