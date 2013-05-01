//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Load up common script base
loadDir("common");

//-----------------------------------------------------------------------------

// Load up default console values.
exec("./client/defaults.cs");
exec("./server/defaults.cs");

// Preferences (overide defaults)
exec("./prefs.cs");

$mod::name = "LuckyStar";
$mod::desc = "LuckyStar: Dancing Game";

//-----------------------------------------------------------------------------
// Package overrides to initialize the game
package lsd {

function displayHelp() {
   Parent::displayHelp();
   error(
      "LuckyStar:Dance options:\n"@
      "  -dedicated             Start as dedicated server\n"@
      "  -connect <address>     For non-dedicated: Connect to a game at <address>\n" @
      "  -mission <filename>    For dedicated: Load the mission\n"
   );
}

function parseArgs()
{
   Parent::parseArgs();

   // Make sure this variable reflects the correct state.
   $Server::Dedicated = false;

   // Arguments, which override everything else.
   for (%i = 1; %i < $Game::argc ; %i++)
   {
      %arg = $Game::argv[%i];
      %nextArg = $Game::argv[%i+1];
      %hasNextArg = $Game::argc - %i > 1;
   
      switch$ (%arg)
      {
         //--------------------
         case "-dedicated":
            $Server::Dedicated = true;
            enableWinConsole(true);
            $argUsed[%i]++;

         //--------------------
         case "-mission":
            $argUsed[%i]++;
            if (%hasNextArg) {
               $missionArg = %nextArg;
               $argUsed[%i+1]++;
               %i++;
            }
            else
               error("Error: Missing Command Line argument. Usage: -mission <filename>");

         //--------------------
         case "-connect":
            $argUsed[%i]++;
            if (%hasNextArg) {
               $JoinGameAddress = %nextArg;
               $argUsed[%i+1]++;
               %i++;
            }
            else
               error("Error: Missing Command Line argument. Usage: -connect <ip_address>");
      }
   }
}

function onStart()
{
   // Initialize the client and the server
   Parent::onStart();
   initServer();
   if ($Server::Dedicated) initDedicated();
   else initClient();

   $Editor::newMissionOverride = $mod::name@"/data/missions/flat.mis";
}

function onExit()
{
   // Save off our current preferences for next time
   echo("Exporting prefs");
   export("$Pref::*", "./prefs.cs", False);
   Parent::onExit();
}

}; // Client package
activatePackage(lsd);


//-----------------------------------------------------------------------------

function initServer()
{
   echo("\n--------- Initializing "@$mod::name@": Server ---------");

   // The common module provides the basic server functionality
   initBaseServer();

   // Load up game server support scripts
   exec("./server/game.cs");
}

//-----------------------------------------------------------------------------

function initDedicated()
{
   enableWinConsole(true);
   echo("\n--------- Starting "@$mod::name@": Dedicated Server ---------");

   // The server isn't started unless a mission has been specified.
   if ($missionArg !$= "") {
      createServer("MultiPlayer", $missionArg);
   }
   else
      echo("No mission specified (use -mission filename)");
}

//-----------------------------------------------------------------------------

function initClient()
{
	echo("\n--------- Initializing "@$mod::name@": Client ---------");
	
	// Load client-side Audio Profiles/Descriptions
	exec("./client/audioProfiles.cs");
	
	//exec("./client/ui/skins/xp/defaultProfiles.cs");
	exec("./client/ui/skins/tgb/profiles.cs");
	
	// The common module provides basic client functionality
	initBaseClient();
	
	// InitCanvas starts up the graphics system.
	// The canvas needs to be constructed before the gui scripts are
	// run because many of the controls assume the canvas exists at
	// load time.
	initCanvas($mod::desc);
	
	// rewirte system default dull 3d cursor
	exec("./client/ui/skins/tgb/cursors.cs");
	
	// Load up the shell and game GUIs
	exec("./client/ui/PlayGui.gui");
	exec("./client/ui/mainMenuGui.gui");
	exec("./client/ui/optionsDlg.gui");
	exec("./client/ui/loadingGui.gui");
	exec("./client/ui/SpellDlg.gui");
	exec("./client/ui/ScoreGui.gui");
	exec("./client/ui/TestingToolGui.gui");
	exec("./client/ui/CustomiseGui.gui");
	exec("./client/ui/CustomColorPickerDlg.gui");
	exec("./client/ui/MapSelectorGui.gui");
	exec("./client/ui/NetworkGui.gui");
	exec("./client/ui/loginGui.gui");
	exec("./client/ui/LobbyGui.gui");
	exec("./client/ui/ControlPanelGui.gui");
	
	// Client scripts
	exec("./client/optionsDlg.cs");
	exec("./client/missionDownload.cs");
	exec("./client/serverConnection.cs");
	exec("./client/playGui.cs");
	exec("./client/LuckyStar.cs");
	
	// Default player key bindings
	exec("./client/default.bind.cs");
	
	// Network port for connecting to a remote server, 
	// or host a multi-player game.
	//setNetPort($pref::Net::Port);
	// Really shouldn't be starting the networking no matter what, give a invalid port number
	setNetPort(0);
	
	// Copy saved script prefs into C++ code.
	setShadowDetailLevel( $pref::shadows ); // 0: low quality, 1: high quality
	setDefaultFov( $pref::Player::defaultFov );
	setZoomSpeed( $pref::Player::zoomSpeed );
	
	// Start up the main menu...
	Canvas.setContent(MainMenuGui);
	Canvas.setCursor("DefaultCursor");
}

