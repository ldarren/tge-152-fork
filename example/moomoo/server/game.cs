//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Server and mission initialization
//-----------------------------------------------------------------------------

function onServerCreated()
{
   // This function is called when a server is constructed.

   // Master server information for multiplayer games
   $Server::GameType = "Torque TTB";
   $Server::MissionType = "None";
	
   exec("./audioProfiles.cs");

   // Load up all datablocks, objects etc.
	exec("./moogun.cs");
   exec("./camera.cs");
   exec("./editor.cs");
   exec("./player.cs");
   exec("./logoitem.cs");
   
   //MOOMO ADD ON
   exec("./mmbarn.cs");
   exec("./mmwindmill.cs");
   exec("./mmmoomoo.cs");
	exec("./mmcowmove.cs");
   exec("./mmweather.cs");
   exec("./mmfx.cs");
	exec("./explosion.cs");
   exec("./bot.cs");
	exec("./scorekeeping.cs");
   exec("./connectDb.cs");
   exec("./plRotate.cs");
}

function onServerDestroyed()
{
   // This function is called as part of a server shutdown.
}


//-----------------------------------------------------------------------------

function onMissionLoaded()
{
   // Called by loadMission() once the mission is finished loading.
	new ScriptObject(AIManager) {};
	MissionCleanup.add(AIManager);
	
	AIManager.think();
	AIManager.MooThink();	// Added for moomoos
}

function onMissionEnded()
{
   // Called by endMission(), right before the mission is destroyed
	AIManager.delete();
}


//-----------------------------------------------------------------------------
// Dealing with client connections
// These methods are extensions to the GameConnection class. Extending
// GameConnection make is easier to deal with some of this functionality,
// but these could also be implemented as stand-alone functions.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function GameConnection::onClientEnterGame(%this)
{
	optionsDlg::applyGraphics();
   
	// Every client get's a camera object.
   %this.camera = new Camera() {
      dataBlock = Observer;
   };
   MissionCleanup.add( %this.camera );
   %this.camera.scopeToClient(%this);

   // Create a player object.
   %spawnPoint = pickSpawnPoint();
   %this.createPlayer(%spawnPoint);
}

function GameConnection::onClientLeaveGame(%this)
{
   if (isObject(%this.camera))
      %this.camera.delete();
   if (isObject(%this.player))
      %this.player.delete();
}


//-----------------------------------------------------------------------------

function GameConnection::createPlayer(%this, %spawnPoint)
{
   if (%this.player > 0)  {
      // The client should not have a player currently
      // assigned.  Assigning a new one could result in 
      // a player ghost.
      error( "Attempting to create an moomoo ghost!" );
   }

   // Create the player object
   %player = new Player() {
      dataBlock = PlayerShape;
      client = %this;
   };
   MissionCleanup.add(%player);

   // Player setup...
   %player.setTransform(%spawnPoint);
   %player.setShapeName(%this.name);
   
   // Update the camera to start with the player
   %this.camera.setTransform(%player.getEyeTransform());	//NORMALLY
 	//commandToServer('ToggleCameraStatic');					// SET INITIAL CAM

   // Give the client control of the player
   %this.player = %player;
   %this.setControlObject(%player);

	commandToServer('initmmShoot');	// START SHOOTING THREAD
	
	startDbInput();		// CLEAR OLD SMS FROM DATABASE
	ShowScoreBoard(0);	// START SCOREBOARD THREAD
}


//-----------------------------------------------------------------------------

function pickSpawnPoint() 
{
   // Pick the first object in drop point group and use it's
   // location as a spawn point.
   %group = nameToID("MissionGroup/PlayerDropPoints");
   if (%group != -1 && %group.getCount() != 0)
      return %group.getObject(0).getTransform();

   // If no object was found, return a point near the center of the world
   error("Missing spawn point object and/or mission group " @ %groupName);
   return "0 0 300 1 0 0 0";
}


//************************************************************************
//
// game specific functions
// MooMoo Saviour
// Programmer : Ezham
// Date	:		12 Jan 07
//
//************************************************************************

$mmshooting		=	false;						// Game Switch
$ShootingNumber 	= "";								// Name Display Purpose
$timer 			= $pref::Game::Duration;	// Game Duration
$tagsch 			= 0;								// Shooting schedule ID
$tagTimer 		= 0;								// Timer thread ID
$NICK_KEY		= "START";						// Key word for registering (STATIC)
$cur_php_uid	= "";

function serverCmdInitMmShoot(%client)
{
	$mmshooting = true;
	
	mmStartShoot(%client, %baseMsg);					// SHOOTING FUNCTION
	
	commandToClient(%client, 'SetCDC', $timer);	// START TIMER ON CLIENT
	commandToClient(%client, 'RotateMsg');			// INSTRUCTION MSG DISPLAY THREAD
}

function serverCmdStopShoot(%client)
{
	$mmshooting = false;
	cancel($tagsch);
	StopScoreBoard(%client);
}

function mmStartShoot(%client)
{	
	if($mmshooting == true)
	{
		echo("SHOOTING - initializing schedule");
		$tagsch = schedule(1000, 0, PlayerShoot, %client);
	}
	else
	{
		echo("shooting - cancelling schedule");
		cancel($tagsch);
		return;
	}
}
function PlayerShoot(%client)
{
	readDbInput();		// Read from database
	//EmptyShootMsg(%client);	// add breakpoint to messages
	
	// ************DEBUG********************
	//echo(getDbInput(1));
	//echo(getDbInput(2));
	//echo(getDbInput(3));
	//echo(getDbInput(4));
	//echo(getDbInput(5));
	
	if($arrDbInput.obj[5] $= "yes")	// if completed whole message stream
	{
		%php_hpnumber = getDbInput(1);
		%php_nickname = getDbInput(2);
		%php_command = getDbInput(3);
		%php_action = getDbInput(4);
		%php_uid = getDbInput(6);
		
		if(strcmp($cur_php_uid, %php_uid) != 0)	// Not same message from server 
		{
			if(%php_action $= $NICK_KEY)	// IF CHANGING NAME OR ADDING NEW PLAYER
			{
				if(!getPlayer(%php_hpnumber))
				{
					addPlayer(%php_hpnumber, %php_nickname);
					echo("addPlayer("@%php_hpnumber@","@%php_nickname@");");
					commandToClient(%client, 'CLTAddPlayerMsg', %php_nickname);
				}
				else
				{
					checkNickChange(%php_hpnumber, %php_nickname);
					echo("checkNickChange("@%php_hpnumber@","@%php_nickname@");");
				}
				$arrDbInput.obj[5] = "";
			}
			else										// IF SHOOTING
			{
				selectCameraRand();				// SELECT CAMERA AT RANDOM						
			
				if(!getPlayer(%php_hpnumber))	// IF PLAYER NOT STARTED THE GAME BUT REGISTERED WITH DATABASE
				{
					addPlayer(%php_hpnumber, %php_nickname);
					echo("addPlayer("@%php_hpnumber@","@%php_nickname@");");
				}
			
				//%client.player.schedule(1000, "setCloaked", false);	// CLOAKING ****DISABLED
			
				%angle = %php_command;	
				if(%angle < 0)
				{
					%angle = 0;
				}
				else
				{
					if(%angle > 180)
						%angle = 180;
				}
				
		
				DisplayShootMsg(%client, %angle);
				
				if(%angle < 90)
					%angle += 360;
			
				rotate(%client.player, %angle-90, 100, 5, "Shoot");
				//rot(%client.player, %angle-90, 1, 0.05);
				$ShootingNumber = %php_hpnumber;
			
				//schedule(1000*2, 0, Shoot, %client, %php_hpnumber);	// SHOOT
				$arrDbInput.obj[5] = "";										// RESET & READY FOR NEXT MESSAGE STREAM
			}
			$cur_php_uid = %php_uid;	// For message repeat delay checking
		}
	}
	
	DispScore(%client);		// SET SCOREBOARD (NOT DISPLAY...SET! SORRY NAMING SUCK)
	$tagsch = schedule(3500, 0, PlayerShoot, %client);				// LOOP
}
/*
function Shoot(%client, %php_hpnumber)
{
	$ShootingNumber = %php_hpnumber;
	%client.player.setImageTrigger(0, true);	// PLAYER SHAPE SHOOT
	
	//%shape.schedule(1000, "setCloaked", true);	// CLOAKING *********DISABLED
}
*/

function Shoot(%player)
{
	%player.setImageTrigger(0, true);	// PLAYER SHAPE SHOOT
	
	//%shape.schedule(1000, "setCloaked", true);	// CLOAKING *********DISABLED
}

//----------------------------------------------------------------------------
// GUI FUNCTIONS (SERVER SIDE)
//----------------------------------------------------------------------------

function DispScore(%client)		// SHOW IN SCORE BOARD
{
	commandToClient(%client, 'clearScore');
	
	//if($playerCount < 9)
	//{
		for(%i = 0; %i < $playerCount; %i++)
			if(stricmp($arrScore.name[%i], "") != 0)
				commandToClient(%client, 'SetScore', %i, $arrScore.name[%i], $arrScore.score[%i]);
	//}
	//else
	//{
	//	for(%i = 0; %i < 9; %i++)
	//		if(stricmp($arrScore.name[%i], "") != 0)
	//			commandToClient(%client, 'SetScore', %i, $arrScore.name[%i], $arrScore.score[%i]);
	//}
}

function DisplayShootMsg(%client, %angle)	// DISPLAY SHOOTING MESSAGE
{
	commandToClient(%client, 'CLTDispShootMsg', $arrDbInput.obj[2], %angle);
}

//function ClearShootMsg(%client)				// CLEARING SHOOTING MESSAGE
//{
//	commandToClient(%client, 'CLTClearShootMsg');
//}

function EmptyShootMsg(%client)
{
	commandToClient(%client, 'CLTEmptyShootMsg');
}


function ShowScoreBoard(%switch)			// SCOREBOARD HIDE/SHOW
{
	if($tagTimer)
	{
		if(%switch)
		{
			Scoreboard.Visible = 1;
			%switch = 0;
			%duration = 5000;
		}
		else
		{
			Scoreboard.Visible = 0;
			%switch = 1;
			%duration = 25000;
		}
	}
	
	//$tagTimer = schedule(%duration, 0, ShowScoreBoard, %switch);
}

function StopScoreBoard(%client)				// END SCOREBOARD BASED ON TIME
{
	cancel($tagTimer);
	Scoreboard.visible = 1;
	AutoScrollMsg.visible = 0;
	AutoScrollGameMsg.visible = 0;
	GameMsg.visible = 0;
	msgbmp.visible = 0;
	InstructBmp.visible = 0;
	MsgBox.visible = 0;
	$ShootingNumber = "";
	
	submitScore();
	commandToServer('ToggleOrbitLevel', %client.player);
	freeShooting(%client);
	
	schedule(15000, 0, resetGame, %client);
}

function freeShooting(%client)
{
	%angle = mCeil(getRandom(60,120));
	
	if(%angle < 90)
		%angle += 360;
	
	rotate(%client.player, %angle-90, 100, 1, "Shoot");
	//schedule(1000*2, 0, Shoot, %client, %php_hpnumber);	// SHOOT
	
	$tagFree = schedule(4000, 0, FreeShooting, %client);
}

function resetGame(%client)
{
	//commandToClient(%client, 'ClearScore');
	restartScoreboard();
	
	cancel($tagFree);
	cancel($tagOrbitCam);
	cancel($tagInst);
	
	GameMsg.clear();
	
	$playerCount	=	0;
	
	commandToServer('ToggleCameraStatic');					// SET INITIAL CAM
	
	commandToServer('initmmShoot');	// START SHOOTING THREAD
	
	startDbInput();		// CLEAR OLD SMS FROM DATABASE
	ShowScoreBoard(0);	// START SCOREBOARD THREAD
}

function restartScoreboard()
{
	$mvYawLeftSpeed = 0.00;
	Scoreboard.visible = 0;
	AutoScrollMsg.visible = 1;
	AutoScrollGameMsg.visible = 1;
	GameMsg.visible = 1;
	msgbmp.visible = 1;
	InstructBmp.visible = 1;
	MsgBox.visible = 1;
	$ShootingNumber = "";
}
