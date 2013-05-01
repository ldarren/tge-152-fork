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


//-----------------------------------------------------------------------------
// Package overrides to initialize the game
package MooMoo {

function onStart()
{
   // Initialize the client and the server
   Parent::onStart();
   initServer();
   initClient();
   $Editor::newMissionOverride = "moomoo/data/missions/moomoo.mis";
}

function onExit()
{
   // Save off our current preferences for next time
   echo("Exporting prefs");
   export("$Pref::*", "./prefs.cs", False);
   Parent::onExit();
}

}; // Client package
activatePackage(MooMoo);


//-----------------------------------------------------------------------------

function initServer()
{
   echo("\n--------- Initializing MooMoo: Server ---------");

   // The common module provides the basic server functionality
   initBaseServer();

   // Load up game server support scripts
   exec("./server/game.cs");
}


//-----------------------------------------------------------------------------

function initClient()
{
   echo("\n--------- Initializing MooMoo: Client ---------");

   // The common module provides basic client functionality
   initBaseClient();

   // InitCanvas starts up the graphics system.
   // The canvas needs to be constructed before the gui scripts are
   // run because many of the controls assume the canvas exists at
   // load time.
   initCanvas("Moo Moo Saviour");

   // Load client-side Audio Profiles/Descriptions
   exec("./client/audioProfiles.cs");

   // Load up the shell and game GUIs
   exec("./client/ui/PlayGui.gui");
   //exec("./client/ui/mainMenuGui.gui");
   exec("./client/ui/optionsDlg.gui");
   exec("./client/ui/loadingGui.gui");
   exec("./client/ui/mmSplashScreen.gui");

   // Client scripts
   exec("./client/optionsDlg.cs");
   exec("./client/missionDownload.cs");
   exec("./client/serverConnection.cs");
   exec("./client/loadingGui.cs");
   exec("./client/playGui.cs");
   exec("./client/Score.cs");
	exec("./client/timer.cs");

   // Default player key bindings
   exec("./client/default.bind.cs");

   // Copy saved script prefs into C++ code.
   setShadowDetailLevel( $pref::shadows );
   setDefaultFov( $pref::Player::defaultFov );
   setZoomSpeed( $pref::Player::zoomSpeed );

   //MooMoo Splash Screen
	loadMyMission();
	//loadSplashScreen();
	Canvas.setCursor("DefaultCursor");
}

//-----------------------------------------------------------------------------
// FUNCTION LOAD MAIN MENU
function loadMainMenu()
{
   // Startup the client with the Main menu...
   Canvas.setContent( MainMenuGui );

   // Make sure the audio initialized.
   if($Audio::initFailed) {
      MessageBoxOK("Audio Initialization Failed", 
         "The OpenAL audio system failed to initialize.  " @
         "You can get the most recent OpenAL drivers <a:www.garagegames.com/docs/torque/gstarted/openal.html>here</a>.");
   }
 }



//-----------------------------------------------------------------------------
// LOAD MY MISSION

function loadMyMission()
{
   // make sure we are not connected to a server already
   disconnect();
   
   // Create the server and load the mission
   createServer("SinglePlayer", expandFilename("./data/missions/moomoo.mis"));

   // Make a local connection
   %conn = new GameConnection(ServerConnection);
   RootGroup.add(ServerConnection);
   %conn.setConnectArgs("");
   %conn.setJoinPassword("None");
   %conn.connectLocal();
}
