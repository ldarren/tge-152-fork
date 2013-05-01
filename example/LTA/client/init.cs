// ============================================================
// Project            :  VR-20
// File               :  .\LTA\client\init.cs
// Copyright          :  
// Author             :  ldarren
// Created on         :  Tuesday, June 19, 2007 6:10 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

//-----------------------------------------------------------------------------

// Variables used by client scripts & code.  The ones marked with (c)
// are accessed from code.  Variables preceeded by Pref:: are client
// preferences and stored automatically in the ~/client/prefs.cs file
// in between sessions.
//
//    (c) Client::MissionFile             Mission file name
//    ( ) Client::Password                Password for server join

//    (?) Pref::Player::CurrentFOV
//    (?) Pref::Player::DefaultFov
//    ( ) Pref::Input::KeyboardTurnSpeed

//    (c) pref::Master[n]                 List of master servers
//    (c) pref::Net::RegionMask     
//    (c) pref::Client::ServerFavoriteCount
//    (c) pref::Client::ServerFavorite[FavoriteCount]
//    .. Many more prefs... need to finish this off

// Moves, not finished with this either...
//    (c) firstPerson
//    $mv*Action...

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function initClient()
{
	echo("\n--------- Initializing MOD: "@$modDesc@" Client ---------");
	
	// Game information used to query the master server
	//$CLient::GameType = "";
	$Client::GameTypeQuery = "Any";
	$Client::MissionTypeQuery = "Any";
	$Client::SavedFileLocation = "LTA/data/locate/";
	$saveInfo::mission = "";
	$saveInfo::lastcheckpoint = 0;
	$saveInfo::hostGame = 0; // 0: join, 1: host
	$saveInfo::gameMode = 0; // 0: single player, 1: lan, 2: wan
	
	switch$($lang)
	{
	case "en":
		exec("./ui/langs/playersetup.en.cs");
	case "ch":
		exec("./ui/langs/playersetup.ch.cs");
	case "kr":
		exec("./ui/langs/playersetup.kr.cs");
	case "th":
		exec("./ui/langs/playersetup.th.cs");
	}
	
	//fileEncDec("LTA/data/dialogs/dlq/question.txt", "LTA/data/dialogs/dlq/q005.dlq");
	//fileEncDec("LTA/data/dialogs/dlq/q005.dlq", "LTA/data/dialogs/dlq/q006.txt");
	
	// The common module provides basic client functionality
	initBaseClient();
	
	// Always start up the shell in 800x600. If the pref is not set, then it
	// will defaul to 800x600 in the platform code.
	if ($Pref::Video::Resolution !$= "") 
	{
		$width = getWord($pref::Video::resolution,0);
		if ($width < 800) 
		{
			$pref::Video::resolution = "800 600" SPC getWord($pref::Video::resolution,2);
			echo("Forcing resolution to 800 x 600");
		}
	}
	
	// InitCanvas starts up the graphics system.
	// The canvas needs to be constructed before the gui scripts are
	// run because many of the controls assume the canvas exists at
	// load time.
	initCanvas($modDesc);
	
	// Load client-side Audio Profiles/Descriptions
	exec("./scripts/audioProfiles.cs");
	
	// Load up the Game GUIs profiles
	exec("./ui/customProfiles.cs"); // override the base profiles if necessary
	exec("./ui/defaultGameProfiles.cs"); // default game profiles, non-base profiles
	
	// Load up the shell GUIs
	exec("./ui/StartupGui.gui");
	exec("./ui/openingGui.gui");
	exec("./ui/mainMenuGui.gui");
	exec("./ui/playerSetupDlg.gui");
	exec("./ui/optionsDlg.gui");
	exec("./ui/startMissionGui.gui");
	exec("./ui/joinServerGui.gui");
	exec("./ui/loadGameGui.gui");
	exec("./ui/loadingGui.gui");
	exec("./ui/playerSelection.gui");
	exec("./ui/subPanelsGui.gui");
	exec("./ui/playGui.gui");
	//exec("./ui/playerList.gui");
	exec("./ui/dialogBox.gui");
	exec("./ui/inputBoxGui.gui");
	exec("./ui/RPGDialog.gui");
	exec("./ui/CommentGui.gui");
	//exec("./ui/ChatHud.gui");
	
	// Game page & scene gui
	//exec("./ui/overview_main.gui");
	
	// Load client scripts
	exec("./scripts/openingGui.cs");
	exec("./scripts/client.cs");
	exec("./scripts/game.cs");
	exec("./scripts/missionDownload.cs");
	exec("./scripts/serverConnection.cs");
	exec("./scripts/playerList.cs");
	exec("./scripts/optionsDlg.cs");
	exec("./scripts/loadGameGui.cs");
	//exec("./scripts/chatHud.cs");
	exec("./scripts/messageHud.cs");
	exec("./scripts/mainMenuGui.cs");
	exec("./scripts/playerSetupDlg.cs");
	exec("./scripts/playerSelection.cs");
	exec("./scripts/subPanelsGui.cs");
	exec("./scripts/playGui.cs");
	exec("./scripts/dialogBox.cs");
	exec("./scripts/inputBoxGui.cs");
	exec("./scripts/RPGDialogAudioProfiles.cs");
	exec("./scripts/RPGDialog.cs");
//	exec("./scripts/centerPrint.cs");
	
	// Game specific
	exec("./scripts/LTAGame.cs");
	
	// Default player key bindings
	exec("./scripts/default.bind.cs");
	exec("./config.cs");
	
	// Network port for connecting to a remote server, 
	// or host a multi-player game.
	//setNetPort($pref::Net::Port);
	// Really shouldn't be starting the networking no matter what, give a invalid port number
	setNetPort(0);
	
	// Copy saved script prefs into C++ code.
	setShadowDetailLevel( $pref::shadows );
	setDefaultFov( $pref::Player::defaultFov );
	setZoomSpeed( $pref::Player::zoomSpeed );
	
	// Start up the main menu... this is separated out into a 
	// method for easier mod override.
	
	if ($JoinGameAddress !$= "") 
	{
		// If we are instantly connecting to an address, load the
		// main menu then attempt the connect.
		loadMainMenu();
		connect($JoinGameAddress, "", $Pref::Player::Name);
	}
	else 
	{
		// Otherwise go to the splash screen.
		Canvas.setCursor("DefaultCursor");
		loadStartup();
	}
}

//-----------------------------------------------------------------------------

function loadMainMenu()
{
	// Startup the client with the Main menu...
	Canvas.setContent( MainMenuGui );
	checkAudioInit();
}

//-----------------------------------------------------------------------------
// Load Mission
// %gameType - “SinglePlayer” or “MultiPlayer”
// %mission - File path of mission to load, e.g. ~/data/missions/features.mis
function callMission(%gameType, %mission, %saveInfo)
{
	// make sure we are not connected to a server already
	disconnect();
	
	// Create the server and load the mission
	$saveInfo::mission = %mission;
	createServer(%gameType, $saveInfo::mission);
	
	// Make a local connection
	%conn = new GameConnection(ServerConnection);
	RootGroup.add(ServerConnection);
	%conn.setConnectArgs($pref::Player::Name, %saveInfo);
	%conn.setJoinPassword($Client::Password);
	%conn.connectLocal();
    %conn.setFirstPerson(false);
}

function callFirstMission()
{
	callMission("SinglePlayer",expandFileName("~/data/missions/one_new.mis"));
}

function checkAudioInit()
{
	if($Audio::initFailed)
	{
		MessageBoxOK("Audio Initialization Failed", 
			"The OpenAL audio system failed to initialize.  " @
			"You can get the most recent OpenAL drivers <a:http://www.openal.org/downloads.html>here</a>.");
	}
}   


