// ============================================================
// Project            :  VR-20
// File               :  .\LTA\server\init.cs
// Copyright          :  
// Author             :  ldarren
// Created on         :  Tuesday, June 19, 2007 6:10 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  Adapted from TGE 1.5.1 Demo and GRWC II 0406
//                    :  
//                    :  
// ============================================================

//-----------------------------------------------------------------------------

// Variables used by server scripts & code.  The ones marked with (c)
// are accessed from code.  Variables preceeded by Pref:: are server
// preferences and stored automatically in the ServerPrefs.cs file
// in between server sessions.
//
//    (c) Server::ServerType              {SinglePlayer, MultiPlayer}
//    (c) Server::GameType                Unique game name
//    (c) Server::Dedicated               Bool (initialized in <mod>/main.cs)
//    ( ) Server::MissionFile             Mission .mis file name
//    (c) Server::MissionName             DisplayName from .mis file
//    (c) Server::MissionType             Not used
//    (c) Server::PlayerCount             Current player count
//    (c) Server::GuidList                Player GUID (record list?)
//    (c) Server::Status                  Current server status
//
//    (c) Pref::Server::Name              Server Name
//    (c) Pref::Server::Password          Password for client connections
//    ( ) Pref::Server::AdminPassword     Password for client admins
//    (c) Pref::Server::Info              Server description
//    (c) Pref::Server::MaxPlayers        Max allowed players
//    (c) Pref::Server::RegionMask        Registers this mask with master server
//    ( ) Pref::Server::BanTime           Duration of a player ban
//    ( ) Pref::Server::KickBanTime       Duration of a player kick & ban
//    ( ) Pref::Server::MaxChatLen        Max chat message len
//    ( ) Pref::Server::FloodProtectionEnabled Bool

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

function initServer()
{
   echo("\n--------- Initializing MOD: "@$modDesc@" Server ---------");

   // Server::Status is returned in the Game Info Query and represents the
   // current status of the server. This string sould be very short.
   $Server::Status = "Unknown";

   // Turn on testing/debug script functions
   $Server::TestCheats = false;

   // Specify where the mission files are.
   $Server::MissionFileSpec = "*/missions/*.mis";

   // The common module provides the basic server functionality
   initBaseServer();

   // Load up game server support scripts
   exec("./scripts/commands.cs");	// server command for clients
   //exec("./scripts/centerPrint.cs");	// print message on all clients
   exec("./scripts/game.cs");
}


//-----------------------------------------------------------------------------

function initDedicated()
{
	enableWinConsole(true);
	echo("\n--------- Starting Dedicated Server ---------");
	
	// Make sure this variable reflects the correct state.
	$Server::Dedicated = true;
	
	// The server isn't started unless a mission has been specified.
	if ($missionArg !$= "") 
	{
		createServer("MultiPlayer", $missionArg);
	}
	else echo("No mission specified (use -mission filename)");
}

