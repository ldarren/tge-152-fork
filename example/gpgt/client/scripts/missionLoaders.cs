//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
$Missions::FirstMission = expandFileName("~/data/missions/arena/arena.mis");


//-----------------------------------------------------------------------------
// StartSinglePlayer() - Provided to simplify starting a single-player game.
//-----------------------------------------------------------------------------
//
// 1. Start on the mission loading interface.
// 2. Attempt to start on a mission specified as 'the first'. (Not Optional) See above.
// 3. Insure that we are running non-dedicated and the server type is single player.
// 4. Use the generic server creation function to start up the game server.
// 5. Create the server game connection appropriately named 'ServerConnection'.  Also,
//    store the game connection's ID in RootGroup so we can find it with tree().
// 6. Set the connection player name and password.
// 7. Connect to the local server.
// 
//
function StartSinglePlayer( )
{

   // 1
   Canvas.setContent( LoadingGui );

   // 2 
   %mission = $Missions::FirstMission;

   // 3
   $Server::Dedicated = false;
   %serverType = "SinglePlayer";

   // 4
   createServer(%serverType, %mission); 

   // 5
   %conn = new GameConnection(ServerConnection);
   RootGroup.add(ServerConnection);

   // 6
   %conn.setConnectArgs($pref::Player::Name);
   %conn.setJoinPassword($Client::Password);

   // 7
   %conn.connectLocal();
}

//-----------------------------------------------------------------------------
// HostMultiplayer() - Provided to simplify starting a multi-player game.
//-----------------------------------------------------------------------------
//
// 1. Start on the mission loading interface.
// 2. Attempt to start on a mission specified as 'the first'. (Not Optional) See above.
// 3. Insure that we are running non-dedicated and the server type is multi-player.
// 4. Use the generic server creation function to start up the game server.
// 5. Create the server game connection appropriately named 'ServerConnection'.  Also,
//    store the game connection's ID in RootGroup so we can find it with tree().
// 6. Set the connection player name and password.
// 7. Connect to the local server.
// 
//
function HostMultiplayer( )
{
  
   // 1
   Canvas.setContent( LoadingGui );

   // 2
   %mission = $Missions::FirstMission;

   // 3
   $Server::Dedicated = false;
   %serverType = "MultiPlayer";

   // 4
   createServer(%serverType, %mission); 

   // 5
   %conn = new GameConnection(ServerConnection);
   RootGroup.add(ServerConnection);

   // 6
   %conn.setConnectArgs($pref::Player::Name);
   %conn.setJoinPassword($Client::Password);

   // 7
   %conn.connectLocal();
}

//-----------------------------------------------------------------------------
// connectToServer() - This helper function is used for connecting to a remote
// game server.
//-----------------------------------------------------------------------------
//
// 1. Cancel any outstanding server queries.
// 2. Start on the mission loading interface.
// 3. Find the server which is selected or get the first server on the list if none
//    is selected.
// 4. Get the info for the chosen server, 
// 4a. Create the server game connection appropriately named 'ServerConnection'.  Also,
//    store the game connection's ID in RootGroup so we can find it with tree().
// 4b. Set the connection player name and password.
// 4c. Connect to the remote server.
//
function connectToServer( )
{
   // 1
   cancelServerQuery();

   // 2
   Canvas.setContent( LoadingGui );

   // 3
   %selectedServer = serverQueryTextList.getSelectedID();
   %selectedServer = (-1 == %selectedServer) ? 0 : %selectedServer;

   // 4
   if (setServerInfo( %selectedServer )) 
   {
      // 4a
      %conn = new GameConnection(ServerConnection);
      RootGroup.add(ServerConnection);

      // 4b
      %conn.setConnectArgs($pref::Player::Name);
      %conn.setJoinPassword($Client::Password);

      // 4c
      %conn.connect($ServerInfo::Address);
   }
}

//-----------------------------------------------------------------------------
// queryServer()
//-----------------------------------------------------------------------------
//
function queryServer( %serverType )
{
   switch$( %serverType )
   {
      case "lan":
         queryLANServers(
            28000,      // lanPort for local queries
            0,          // Query flags
            $Client::GameTypeQuery,       // gameTypes
            $Client::MissionTypeQuery,    // missionType
            0,          // minPlayers
            100,        // maxPlayers
            0,          // maxBots
            2,          // regionMask
            0,          // maxPing
            100,        // minCPU
            0           // filterFlags
            );

      case "master":
         queryMasterServer(
            0,          // Query flags
            $Client::GameTypeQuery,       // gameTypes
            $Client::MissionTypeQuery,    // missionType
            0,          // minPlayers
            100,        // maxPlayers
            0,          // maxBots
            2,          // regionMask
            0,          // maxPing
            100,        // minCPU
            0           // filterFlags
            );
   }
}

//-----------------------------------------------------------------------------
// getServerAddress()
//-----------------------------------------------------------------------------
function getServerAddress( %index )
{
   if( %index > getServerCount() ) return "";

   setServerInfo( %index );

   return $ServerInfo::Address;
}


//-----------------------------------------------------------------------------
// refreshServerInfo()
//-----------------------------------------------------------------------------
function refreshServerInfo()
{
   querySingleServer( $ServerInfo::Address, 0 );
}