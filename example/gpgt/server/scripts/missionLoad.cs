//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

//-----------------------------------------------------------------------------
// loadMission()
//-----------------------------------------------------------------------------
//
// 1. End any prior mission first.
// 2. Increment mission sequence.  (Used to prevent incorrect ghost sequencing.
//    By incrementing this value on subsequent loads and then tracking it, we 
//    can be assured that we won't pick up a stray sequence that got 'hung in 
//    the ether'.  i.e. We are protecting against weird networking latency stuff.)
// 3. Mark the mission as NOT running and store the mission name we will be loading.
// 4. Extract mission info from the mission file,  including the display name and stuff 
//    to send to the client. (Note: We do not actually send this data in the GPGT
//    MP starter kit, but we still extract it in case you want to do so in your own
//    game).
// 5. Move on to the next stage of the load.  (See note below).
//
function loadMission( %missionName, %isFirstMission ) 
{
   // ("\c5 loadMission( " , %missionName , " , " , %isFirstMission , " )");

   // 1
   endMission();

   //echo("\c5 *** Stage 1 load");

   // 2
   $missionSequence++;

   // 3
   $missionRunning = false;
   $Server::MissionFile = %missionName;

   // 4
   buildLoadInfo( %missionName );

   // Note: If you do choose to send mission info to the clients for display,
   // simply parse over the clientGroup and send the data to each client.
   // Then, pause at the end of this script to allow sufficient time for that
   // data to be transmitted.  Since we are not doing this, the script
   // moves right to the next stage of the load w/o pausing.

   // 5
   loadMissionStage2();
}

//-----------------------------------------------------------------------------
// loadMissionStage2()
//-----------------------------------------------------------------------------
//
// 1. Set instant group to serverGroup (so all objects are automatically inserted
//    into the serverGroup when created).
// 2. Check for the existence of the mission file.
// 3. Set the mission CRC.  Clients will use this data to determine if the mission
//    has been modified.  If the mission has not been modified or if they have a 
//    mission lighting file (.ML) matching this CRC, then they can skip the lighting
//    phase that will come later.
// 4. Load the mission file.
// 5. If the mission load failed, try to load the next mission.
// 6. Create the MissionCleanup simGroup.
// 7. Build path lists (from all loaded interiors).
// 8. Tell each attached client to start the mission loading process.
// 9. Launch the game and purge resources to start clean.
//
function loadMissionStage2() 
{
   //echo("\c5 loadMissionStage2( )");

   //echo("\c5 *** Stage 2 load");

   // 1
   $instantGroup = ServerGroup;


   // 2
   %file = $Server::MissionFile;
   if( !isFile( %file ) ) {
      error( "Could not find mission " @ %file );
      return;
   }

   // 3
   $missionCRC = getFileCRC( %file );

   // 4
   exec(%file);

   // 5
   if( !isObject(MissionGroup) ) {
      error( "No 'MissionGroup' found in mission \"" @ $missionName @ "\"." );
      schedule( 3000, ServerGroup, CycleMissions );
      return;
   }

   // 6
   new SimGroup( MissionCleanup );
   $instantGroup = MissionCleanup;

   // 7
   pathOnMissionLoadDone();

   //echo("\c5 *** Mission loaded");

   // 8
   $missionRunning = true;
   for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ )
   {
      ClientGroup.getObject(%clientIndex).loadMission();
   }

   // 9
   onMissionLoaded();
   purgeResources();
}


//-----------------------------------------------------------------------------
// endMission()
//-----------------------------------------------------------------------------
// 
// 1. If not mission is loaded, abort.
// 2. Communicate to the game code (game.cs) that the mission is over/ending.
// 3. For each attached client, inform the client that the mission is ending,
//    stop ghosting and clear the pathing information.
// 4. Delete the MissionGroup and MissionCleanup simGroups, thereby automatically
//    deleting all objects they contain.
// 5. Do the same for the server group, and then create a new instance.
//
//
function endMission()
{
   //echo("\c5 endMission( )");

   // 1
   if ( !isObject( MissionGroup ) ) return;

   // 2
   onMissionEnded();

   // 3
   for( %clientIndex = 0; %clientIndex < ClientGroup.getCount(); %clientIndex++ ) 
   {
      %cl = ClientGroup.getObject( %clientIndex );
      %cl.endMission();
      %cl.resetGhosting();
      %cl.clearPaths();
   }

   // 4
   MissionGroup.delete();
   MissionCleanup.delete();

   // 5
   $ServerGroup.delete();
   $ServerGroup = new SimGroup(ServerGroup);
}

//-----------------------------------------------------------------------------
// resetMission()
//-----------------------------------------------------------------------------
//
// 1. Delete the MissionCleanup simGroup, automatically deleting all objects
//    it contains, then create a new one and point instant group to it.
// 2. Inform the game logic (game.cs) that the mission was reset.
//
function resetMission()
{
   //echo("\c5 resetMission( )");   

   // 1
   MissionCleanup.delete();
   $instantGroup = ServerGroup;
   new SimGroup( MissionCleanup );
   $instantGroup = MissionCleanup;

   // 2
   onMissionReset();
}
