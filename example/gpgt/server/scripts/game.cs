//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
$Game::Duration                  = 0;       

// GPGT - Reloader for this file ...
function rld() 
{
   exec("./game.cs");
   onServerCreated();
}

//-----------------------------------------------------------------------------

function onServerCreated()
{
   //GPGT// This variable tells the Sample Scripts NOT to print their warning:
$SampleScripts:OK = true; 

   // Server::GameType is sent to the master server.
   // This variable should uniquely identify your game and/or mod.
   $Server::GameType = "GPGT Lesson Kit";

   // Server::MissionType sent to the master server.  Clients can
   // filter servers based on mission type.
   $Server::MissionType = "Learning";

   // GameStartTime is the sim time the game started. Used to calculated
   // game elapsed time.
   $Game::StartTime = 0;

   echo("\c3******************** GPGT Base Scripts");
   exec("./GPGTBase/loadGPGTBaseClasses.cs");

   echo("\c3******************** Lesson Support Scripts");
   exec("./EGLessonManager/EGLessonManager.cs");
   exec("./EGLessonManager/NSEWSigns.cs");

   echo("\c3******************** Lesson Scripts");
   loadGPGTLessons();

   // Keep track of when the game started
   $Game::StartTime = $Sim::Time;
   
}


function onServerDestroyed()
{
   //GPGT// This variable tells the Sample Scripts to print their warning:
$SampleScripts:OK = false; 
}


//-----------------------------------------------------------------------------

function onMissionLoaded()
{
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

   $currentLessonPath = "";
}


//-----------------------------------------------------------------------------

function startGame()
{
   if ($Game::Running) {
      error("startGame: End the game first!");
      return;
   }

   // Inform the client we're starting up
   for( %clientConnIndex = 0; %clientConnIndex < ClientGroup.getCount(); %clientConnIndex++ ) {

      %cl = ClientGroup.getObject( %clientConnIndex );

      commandToClient(%cl, 'GameStart');
   }

   // Start the game timer
   if ($Game::Duration)
      $Game::Schedule = schedule($Game::Duration * 1000, 0, "onGameDurationEnd" );
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
   for( %clientConnIndex = 0; %clientConnIndex < ClientGroup.getCount(); %clientConnIndex++ ) {
      %cl = ClientGroup.getObject( %clientConnIndex );
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

function GameConnection::updateClientInfo( %clientConn ) {
   if( ! isObject(%clientConn.player) ) return ;

   %playerGhostIndex = %clientConn.getGhostID( %clientConn.player );

   commandToClient( %clientConn, 'setGhostID' , %playerGhostIndex );

   %data = %clientConn.getID();

   %data = %data @ ":" @ %clientConn.player.getID();

   %data = %data @ ":" @ %clientConn.camera.getID();

   %data = %data @ ":" @ %clientConn.camera.getDatablock().getName();

   %data = %data @ ":" @ $Pref::player::CurrentFOV;

   %data = %data @ ":" @ $pref::Player::zoomSpeed;
  
   %data = %data @ ":" @ %clientConn.updateCurrentLookText( );

   commandToClient( %clientConn, 'updateLessonHUDs', %data );

   //if(MissionGroup.isLessonsMission)
      %clientConn.schedule( 200 , updateClientInfo );
}



function GameConnection::updateCurrentLookText( %clientConn ) {

   %eyePoint  = %clientConn.player.getEyePoint();
   %eyeVector = %clientConn.player.getEyeVector();

   %mask	= 
      $TypeMasks::TerrainObjectType			   |
      $TypeMasks::InteriorObjectType			|
      $TypeMasks::ShapeBaseObjectType			|
      $TypeMasks::VehicleBlockerObjectType	|
      $TypeMasks::StaticTSObjectType;

   %endPoint = vectorAdd( %eyePoint , vectorScale( %eyeVector , 200 ) );

   %currentInViewObject = containerRayCast( %eyePoint , %endPoint , %mask , %clientConn.player );

   %currentInViewObject = getWord( %currentInViewObject , 0);

   if( 0 == %currentInViewObject ) 
   {
      %name        = "-";
      %className   = "-";
      %classDBName = "-";
      %curEnergy   = "-"; 
      %maxEnergy   = "-";  
      %curDamage   = "-";

   }
   else if( %currentInViewObject.getType() & $TypeMasks::TerrainObjectType ) 
   {

      %name        = "Terrain";
      %className   = "-";
      %classDBName = "-";
      %curEnergy   = "-"; 
      %maxEnergy   = "-";  
      %curDamage   = "-";


   } else if ( %currentInViewObject.getType() & $TypeMasks::ShapeBaseObjectType ) {

      %maxEnergy = %currentInViewObject.getDatablock().maxEnergy;
      %maxEnergy = %maxEnergy ? %maxEnergy : 0;
      %curEnergy = %currentInViewObject.getEnergyLevel();
      %curDamage = mfloatlength( %currentInViewObject.getDamagePercent() , 3);

      %name        = %currentInViewObject.getName(); 
      %className   = %currentInViewObject.getClassName(); 
      %classDBName = %currentInViewObject.getDatablock().getName();

      %name = ("" $= %name) ? "-" : %name;
      %className = ("" $= %className) ? "-" : %className;
      %classDBName = ("" $= %classDBName) ? "-" : %classDBName;

   } else {
      %name        = %currentInViewObject.getName(); 
      %className   = %currentInViewObject.getClassName(); 

      %name = ("" $= %name) ? "-" : %name;
      %className = ("" $= %className) ? "-" : %className;

      %classDBName = "-";
      %curEnergy   = "-"; 
      %maxEnergy   = "-";  
      %curDamage   = "-";

   }

   return %currentInViewObject @ ":" @
      %name @ ":" @ 
      %className @ ":" @ 
      %classDBName @ ":" @ 
      %curEnergy @ ":" @ 
      %maxEnergy  @ ":" @  
      %curDamage;

}





//-----------------------------------------------------------------------------
function GameConnection::onClientEnterGame(%clientConn)
{
   //echo("\c5 GameConnection::onClientEnterGame(", %clientConn , ")");
   //
   // 1. Enable lesson selector action map for admin client.
   // 2. Create a BaseCamera (and add to MissionCleanup).
   // 3. Scope the camera to the client.
   // 4. Spawn a player.
   // 5. If this is the Lessons Mission, start updating the client's HUDs
   //

   if(%clientConn.isAdmin && MissionGroup.isLessonsMission)
      commandToClient(%clientConn, 'ToggleSelectorBinding', true);

   %clientConn.camera = new Camera() {
      dataBlock = BaseCamera;
   };

   MissionCleanup.add( %clientConn.camera );

   %clientConn.camera.scopeToClient(%clientConn);

   %clientConn.spawnPlayer();

   //if(MissionGroup.isLessonsMission)
      %clientConn.schedule( 100 , updateClientInfo );

   if( ( "" !$= $currentLessonPath ) && MissionGroup.isLessonsMission )
   {
      schedule(2000, %clientConn, commandToClient , %clientConn, 'loadActionMap', $currentLessonPath ); 
   }      
}

function GameConnection::onClientLeaveGame(%clientConn)
{
   //echo("\c5 GameConnection::onClientLeaveGame(", %clientConn , ")");
   //
   // 1. Disable lesson selector action map on admin client.
   // 2. Destroy the camera.
   // 3. Destroy the player.
   //
   if(%clientConn.isAdmin && MissionGroup.isLessonsMission)
      commandToClient(%clientConn, 'ToggleSelectorBinding', false);

   if (isObject(%clientConn.camera))
      %clientConn.camera.delete();

   if (isObject(%clientConn.player))
      %clientConn.player.delete();
}

function GameConnection::onLeaveMissionArea(%clientConn)
{
   //echo("\c5 GameConnection::onLeaveMissionArea(", %clientConn , ")");
}

function GameConnection::onEnterMissionArea(%clientConn)
{
   //echo("\c5 GameConnection::onEnterMissionArea(", %clientConn , ")");
}

//GPGT*********************************************************************
//GPGT*********************************************************************
//GPGT** 
//GPGT*********************************************************************
//GPGT*********************************************************************
function GameConnection::spawnPlayer( %clientConn ) 
{
   //echo("\c5 GameConnection::spawnPlayer(", %clientConn , ")");

   %spawnPoint = pickSpawnPoint();
   %clientConn.createPlayer(%spawnPoint , %playerDB); 
}	

//GPGT*********************************************************************
//GPGT*********************************************************************
//GPGT** 
//GPGT*********************************************************************
//GPGT*********************************************************************
function GameConnection::createPlayer(%clientConn, %spawnPoint , %playerDB)
{
   //echo("\c5 GameConnection::createPlayer(", %clientConn , ")");
   if (%clientConn.player > 0)  {
      error( "Client already has a player!" );
      error( "Was player deleted without clearing client?" );
      error( "Check GameConnection::onDeath()." );
   }
   echo("createPlayer() ->" SPC %playerDB);

   if(0 == %playerDB) {
      %localDB = BasePlayer;
   } else {
      %localDB = %playerDB;
   }

   // Create the player object
   %player = new Player() {
      dataBlock = %localDB;
      client = %clientConn;
   };
   MissionCleanup.add(%player);

   // Player setup...
   %player.setTransform(%spawnPoint);
   %player.setShapeName(%clientConn.name);

   // Update the camera to start with the player
   %clientConn.camera.setTransform(%player.getEyeTransform());

   // Give the client control of the player

   %clientConn.player = %player;
   %clientConn.setControlObject(%player);

   $Game::Player = %clientConn.player; 
   $Game::Camera = %clientConn.camera; 

   $Game::Player.camera = $Game::Camera;
   $Game::Camera.player = $Game::Player;

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



