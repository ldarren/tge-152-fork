//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
 
//-----------------------------------------------------------------------------
// createServer()
//-----------------------------------------------------------------------------
//
// 1. If no mission was specified, abort.
// 2. Destroy any currently running server.
// 3. Initialize sequence info, player counts, and the server type.
// 4. If running a multi-player game,
// 4a. Attempt to initialize the port and abort if that fails.
// 4b. Enable external connections.
// 4c. Start master server polling unless explicitly instructed to NOT to do so.
// 5. Create ServerGroup simGroup.
// 6. Inform the game logic (game.cs) that the server has been created. This will
//    load all the datablocks and scripts we need to load the mission.
// 7. Load the mission.
//
function createServer(%serverType, %mission)
{
   // 1
   if (%mission $= "") 
   {
      error("createServer: mission name unspecified");
      return;
   }

   // 2
   destroyServer();

   // 3
   $missionSequence = 0;
   $Server::PlayerCount = 0;
   $Server::ServerType = %serverType;

   // 4
   if (%serverType $= "MultiPlayer") 
   {
      // 4a
      if ( !portInit($Pref::Server::Port) ) 
      {
         error("createServer: Failed to initialize port");
         return;
      }

      // 4b
      allowConnections(true);

      // 4c
      if ($pref::Net::DisplayOnMaster !$= "Never" )
      {
         schedule(0,0,startHeartbeat);
      }
   }

   // 5
   $ServerGroup = new SimGroup(ServerGroup);

   // 6
   onServerCreated();

   // 7
   loadMission( %mission , true );
}


//-----------------------------------------------------------------------------
// destroyServer()
//-----------------------------------------------------------------------------
//
// 1. Reset the server type, disallow connections, stop master server polling,
//    and mark the mission as NOT running.
// 2. Inform the mission logic (missionLoad.cs) that then mission is ending.
// 3. Inform the game logic (game.cs) that the server is being destroyed.
// 4. Delete all server objects by destroying missionGroup, missionCleanup, and 
//    ServerGroup, in that order.  Hint: No game objects should be able to escape
//    destruction unless you purposely move them to a 'persistent' group.
// 5. Disconnect all clients by deleting their connection.
// 6. Clear the GUID list.
// 7. Destroy all currently loaded datablocks.
// 8. Export our current server preferences
// 9. Purge resources.
//
function destroyServer()
{
   // 1
   $Server::ServerType = "";
   allowConnections(false);
   stopHeartbeat();
   $missionRunning = false;
   
   // 2
   endMission();

   // 3
   onServerDestroyed();

   // 4
   if ( isObject(MissionGroup) )   MissionGroup.delete();
   if ( isObject(MissionCleanup) ) MissionCleanup.delete();
   if ( isObject($ServerGroup) )   $ServerGroup.delete();

   // 5
   while (ClientGroup.getCount())
   {
      %client = ClientGroup.getObject(0);
      %client.delete( "destroyServer" );
   }

   // 6
   $Server::GuidList = "";

   // 7
   deleteDataBlocks();
   
   // 8
   export( "$Pref::Server::*", "~/prefs.cs", false );

   // 9
   purgeResources();
}


//-----------------------------------------------------------------------------
// resetServerDefaults()
//-----------------------------------------------------------------------------
//
// 1. Re-load the defaults followed by preferences.
// 2. Re-load the current mission.
//
function resetServerDefaults()
{
   // 1
   exec( "~/defaults.cs" );
   exec( "~/prefs.cs" );

   // 2
   loadMission( $Server::MissionFile );
}


//-----------------------------------------------------------------------------
// addToServerGuidList() - Currently, TGE doesn't supply all the facilities
// to acquire a players GUID, but you can build this functionality in on your
// own, so the GUID tracking code has been retained.
//-----------------------------------------------------------------------------
//
// 1. If the specified GUID is already in the master list, abort.
// 2. Add the new GUID to the master GUID list.
//
function addToServerGuidList( %guid )
{
   // 1
   %count = getFieldCount( $Server::GuidList );
   for ( %i = 0; %i < %count; %i++ )
   {
      if ( getField( $Server::GuidList, %i ) == %guid ) return;
   }

   // 2
   $Server::GuidList = $Server::GuidList $= "" ? %guid : $Server::GuidList TAB %guid;
}

//-----------------------------------------------------------------------------
// addToServerGuidList() - See addToServerGuidList() above.
//-----------------------------------------------------------------------------
//
// 1. Remove the specified GUID from our master list.
//
function removeFromServerGuidList( %guid )
{
   // 1
   %count = getFieldCount( $Server::GuidList );
   for ( %i = 0; %i < %count; %i++ )
   {
      if ( getField( $Server::GuidList, %i ) == %guid )
      {
         $Server::GuidList = removeField( $Server::GuidList, %i );
         return;
      }
   }
}
