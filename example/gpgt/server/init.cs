//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

function initBaseServer()
{
   echo("\n--------- initBaseServer(): GPGT Kit ---------");
   exec("./base/audio.cs");
   exec("./base/message.cs");

   // Make sure this variable reflects the correct state.
   $Server::Dedicated = false;


   // Always start with the ports CLOSED
   setNetPort(0);
}

//-----------------------------------------------------------------------------

function initServer()
{
   echo("\c3\n--------- initServer(): GPGT Kit ---------");

   // Server::Status is returned in the Game Info Query and represents the
   // current status of the server. This string sould be very short.
   $Server::Status = "Unknown";

   $Server::MissionFileSpec = "*/missions/*.mis";

   // Load up game server support scripts
   exec("./scripts/clientConnection.cs");
   exec("./scripts/game.cs");
   exec("./scripts/missionInfo.cs"); 
   exec("./scripts/missionLoad.cs");
   exec("./scripts/ports.cs");
   exec("./scripts/serverCmds.cs");
   exec("./scripts/serverMaintenance.cs");
   exec("./scripts/serverQueries.cs");
   exec("./scripts/serverSideMissionDownload.cs");
   
}


//-----------------------------------------------------------------------------

function initDedicated()
{
   echo("\c3\n--------- initDedicated(): GPGT Kit ---------");

   enableWinConsole(true);

   $Server::Dedicated = true;

   if ($missionArg !$= "") 
   {
      createServer("MultiPlayer", $missionArg);
   }
   else
   {
      echo("No mission specified (use -mission filename)");
   }
}

