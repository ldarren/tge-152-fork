//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Game duration in secs, no limit if the duration is set to 0
$Game::Duration = 20 * 60;

// When a client score reaches this value, the game is ended.
$Game::EndGameScore = 30;

// Pause while looking over the end game screen (in secs)
$Game::EndGamePause = 10;

$BlockSave = 0;
//-----------------------------------------------------------------------------
//  Functions that implement game-play
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function onServerCreated()
{
	// Server::GameType is sent to the master server.
	// This variable should uniquely identify your game and/or mod.
	$Server::GameType = "Educational";
	
	// Server::MissionType sent to the master server.  Clients can
	// filter servers based on mission type.
	$Server::MissionType = "Timed Quiz Game";
	
	// GameStartTime is the sim time the game started. Used to calculated
	// game elapsed time.
	$Game::StartTime = 0;
	
	// Load up basic datablocks, objects etc.
	exec("./audioProfiles.cs");
	exec("./camera.cs");
	exec("./markers.cs");

	exec("./base/init.cs");
	exec("./game/init.cs");
	
	exec("common/server/lightingSystem.cs");
	
	// Scene Management
	exec("./scene.cs");
	exec("./triggers.cs");

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
	
	// Determin what type of game play we have and activate the
	// associated package.
	echo("Mission Type: " @ MissionInfo.type);
	activatePackage(MissionInfo.type @ "Game");
	
	// Override the default game type
	if (MissionInfo.type !$= "") $Server::GameType = MissionInfo.type;
	if (MissionInfo.mission !$= "") $Server::MissionType = MissionInfo.mission;
	$Game::Duration = 0;
	$checkpointtoload = -1;
	
	// Spawn scripted objects
	spawnNPC("~/data/config/npc.xml");
	spawnItems("~/data/config/items.xml");
	
	// Start game play, this doesn't wait players...
	startGame();
}

function onMissionEnded()
{
   // Called by endMission(), right before the mission is destroyed

   // Normally the game should be ended first before the next
   // mission is loaded, this is here in case loadMission has been
   // called directly.  The mission will be ended if the server
   // is destroyed, so we only need to cleanup here.
   deactivatePackage($Server::GameType @ "Game");
   cancel($Game::Schedule);
   $Game::Running = false;
   $Game::Cycling = false;
}


//-----------------------------------------------------------------------------

function startGame()
{
   // Start up the game.  Normally only called once after a mission is loaded,
   // but theoretically a game can be stopped and restarted without reloading
   // the mission.
   if ($Game::Running) {
      error("startGame: End the game first!");
      return;
   }

	// Path Finder Building (Test case for A* path finding)
	//buildNodeGraph();

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
	
	Quest01.init();
	Quest02.init();
   
   $Game::Running = true;
}

function endGame()
{
   // Game specific cleanup...
   if (!$Game::Running)  {
      error("endGame: No game running!");
      return;
   }

	Quest01.remove();
	Quest02.remove();

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
	commandToClient(%this, 'SetGameGUI',"PlayGUI");
	
	// Create a new camera object.
	%this.camera = new Camera() {
		dataBlock = Observer;
	};
	MissionCleanup.add( %this.camera );
	%this.camera.scopeToClient(%this);
	
	// create advanced camera      
	%this.advCamera = new AdvancedCamera() 
	{      
		dataBlock = Adv3rdCameraData; //AdvOrbitCameraData 
	};   
	MissionCleanup.add(%this.advCamera);   
	%this.advCamera.scopeToClient(%this);	
	
	// Spawn the player
	//%this.spawnPlayer();
	
	// place here because TGE required a controller to start, TODO: move this object to some where less obstructive
	%this.camera.setTransform(pickSpawnPoint());
	%this.camera.setVelocity("0 0 0");
	%this.setControlObject(%this.advCamera);
	
	if (%this.cash $= "") %this.cash = 0;
	if (%this.bank $= "") %this.bank = 0;
	if (%this.fare $= "") %this.fare = 0;
	if (%this.card $= "") %this.card = 0;
}

function GameConnection::onClientLeaveGame(%this)
{
	if (isObject(%this.camera))
		%this.camera.delete();
	if (isObject(%this.advCamera))      
		%this.advCamera.delete();   
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
	%this.advCamera.clearPlayerObject();   
	%this.advCamera.clearTargetObject();   
	%this.clearCameraObject();	
	
	// Clear out the name on the corpse
	%this.player.setShapeName("");
	
	// Switch the client over to the death cam and unhook the player object.
	if (isObject(%this.camera) && isObject(%this.player)) {
		%this.camera.setMode("Corpse",%this.player);
		%this.setControlObject(%this.camera);
	}
	%this.player = 0;
	
	// Doll out points and display an appropriate message
	if (%damageType $= "Suicide" || %sourceClient == %this) {
		%this.incScore(-1);
		messageAll('MsgClientKilled','%1 takes his own life!',%this.name);
	}
	else {
		%sourceClient.incScore(1);
		messageAll('MsgClientKilled','%1 gets nailed by %2!',%this.name,%sourceClient.name);
		if (%sourceClient.score >= $Game::EndGameScore)
		cycleGame();
	}
}


//-----------------------------------------------------------------------------

function serverCmdspawnPlayer(%client, %model, %headS, %bodyS, %ArmsS, %handsS, %legsS, %feetS)
{
    echo ("got a server command to spawn a player\n");
    %client.spawnPlayer( %model, %headS, %bodyS, %ArmsS, %handsS, %legsS, %feetS);
}

// don't remove this may use in other places
function GameConnection::spawnPlayer(%this, %model, %headS, %bodyS, %ArmsS, %handsS, %legsS, %feetS)
{
   // Combination create player and drop him somewhere
   %spawnPoint = pickSpawnPoint();
   %this.createPlayer(%spawnPoint, %model, %headS, %bodyS, %ArmsS, %handsS, %legsS, %feetS);
}   

//-----------------------------------------------------------------------------

function GameConnection::createPlayer(%this, %spawnPoint, %model, %headS, %bodyS, %ArmsS, %handsS, %legsS, %feetS)
{
	if (%this.player > 0)  {
	// The client should not have a player currently
	// assigned.  Assigning a new one could result in 
	// a player ghost.
	error( "Attempting to create an angus ghost!" );
	}
	
	// Create the player object
	switch(%model)
	{
		case 1: // Male fighter
			%player = new AIPlayer() {
			dataBlock = MaleBody;
			client = %this;
			};
		case 2: // Female fighter
			%player = new AIPlayer() {
			dataBlock = FemaleBody;
			client = %this;
			};
/*		case 3: // Alien fighter
			%player = new Player() {
			dataBlock = AlienBody;
			client = %this;
			};
			// Player setup...
			%player.setScale("0.1 0.1 0.1");
		case 4: // Thom fighter
			%player = new Player() {
			dataBlock = ThomBody;
			client = %this;
			};
*/	}
	MissionCleanup.add(%player);
	%player.setTransform(%spawnPoint);
	%player.setEnergyLevel(60);
	%player.setShapeName(%this.name);
	%player.setMoveSpeed(0.8);
	//%player.setMoveTolerance(0.25); // size of destination, take center at mouse click
	%player.quest = 0;
	
	// Update the camera to start with the player
//	%this.camera.setTransform(%player.getEyeTransform());

	// set player skins
	%player.addSkinModifier(0, "head", %headS);
	%player.addSkinModifier(1, "body", %bodyS, "mask", "", "255, 0, 0, 255");
	%player.addSkinModifier(2, "arms", %armsS);
	%player.addSkinModifier(3, "hands", %handsS);
	%player.addSkinModifier(4, "legs", %legsS);
	%player.addSkinModifier(5, "feet", %feetS);
	
	//%player.removeSkinModifier(1);
	
	// Give the client control of the player
	%this.player = %player;
	//%this.setControlObject(%player); // disable: mouse control indirect control, enable: keyboard direct control

// only for ThirdPersonMode: start. to prevent camera spawns from 0,0,0
	// We set the camera system to run in 3rd person mode around the %player
	%cameraPos = MatrixMulVector(%spawnPoint, %this.advCamera.getDataBlock().thirdPersonOffset);
	//%cameraPos = VectorAdd(%cameraPos, "0 0 -1.193");
	%cameraPos = VectorAdd(getWords(%spawnPoint, 0, 2), %cameraPos);
	%this.advCamera.setCameraPosition(%cameraPos);
// 	only for ThirdPersonMode: end

	%this.advCamera.setPlayerObject(%player);  
	%this.advCamera.setFollowTerrainMode(false);  
	%this.advCamera.setVerticalFreedomMode(false);
	%this.setCameraObject(%this.advCamera);
	%this.advCamera.setThirdPersonMode();  
    //%this.advCamera.setOrbitMode();
    $advCamera::Zoom = 5;

	// Start the AIManager (Test case for path finding)
/*	new ScriptObject(AIManager) {};
	MissionCleanup.add(AIManager);
	AIManager.think();*/
}

//-----------------------------------------------------------------------------
// object selection additions
//-----------------------------------------------------------------------------

function GameConnection::onServerObjectSelected(%this, %targetObject)
{
	if (%targetObject.getType() & $TypeMasks::PlayerObjectType)
	{
		CreateRPGDialogBox(%this, %targetObject);
	}
	else if (%targetObject.getType() & $TypeMasks::ItemObjectType)
	{
		%this.player.getDatablock().doPickup( %this.player, %targetObject );
	}
}

// retBuffer-> 0: objectId, 1~3:Position, 4~6: normal
function GameConnection::onMoveToSignal(%this, %retBuffer)
{
	//%targetObject = firstWord(%retBuffer);// future use
	if (%this.player.quest==0 && getWord(%retBuffer, 6) > 0.6)
	{
		%this.player.setMoveDestination(getWords(%retBuffer, 1,3), getWord(%retBuffer, 7)); // slow down speed b4 stopping
	}
}

//-----------------------------------------------------------------------------
// Support functions
//-----------------------------------------------------------------------------

function pickSpawnPoint() 
{
	if ($checkpointtoload != -1) return getCheckPoint();
	
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

function getCheckPoint() 
{   
	// No if's beacuse we assume everything is okay,   
	// it saved okay so it must still be fine    
	%checkpoint = $checkpointtoload - 1;   
	%groupName = "MissionGroup/CheckPoints";   
	%group = nameToID(%groupName);
	if (%group == -1)
	%index = %checkpoint;   
	%spawn = %group.getObject(%index);   
	return %spawn.getTransform();
}