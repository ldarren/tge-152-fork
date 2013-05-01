// ============================================================
// Project            :  TankAttack3D
// File               :  .\tank3d\server\engine.cs
// Copyright          :  
// Author             :  Ezham
// Created on         :  Thursday, June 07, 2007 2:23 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

	$isLooping			= false;							// Game Loop Switch
	$hIsLooping			= 0;								// Game Loop ID (handle)
	$Timer 				= $pref::Game::Duration;			// Game Duration
	$hTimer 			= 0;								// Timer thread ID (handle)
	$CurrPlayerNum 		= "";								// Name Display Search Purpose *********************SHOOTING
	
	$RegKeyword			= $pref::Game::RegKeyword;			// Key word for registering (STATIC)
	$cur_php_index		= 0;								// Message Validation, for update use
	
	// HARDCODED: Game Specific Variables - EDIT AT YOUR OWN RISK
	$GameLoopDelay		= 5.0;//3.5;					// Delay before the next game loop (seconds)
   $RotationFreq     = 100;                     // Loop Frequency in rotation (Hz)
   $RotationAng      = 1;                       // Angle of Rotation per Loop (degree)
	

function initGame(%client)
{
	startGameDisplayHighScoreList(%client);			// Make sure all GUIs are activated
	
	$isLooping = true;  										// Start waiting list loop
	$hIsLooping = schedule(500, 0, mainGameLoop, %client);		// SHOOTING FUNCTION (delay 0.5 second)
	
	commandToClient(%client, 'SetCDC', $Timer);		// START TIMER ON CLIENT
	commandToClient(%client, 'rotateInstruction');	// INSTRUCTION MSG DISPLAY THREAD
	
	$pHighScoreSchedule = schedule(22500, 0, alternateGameDisplayHighScoreList);			// FIX SINGAPORE: show alternatively high score
}

function serverCmdStopGameLoop(%client)
{
	$isLooping = false;
	cancel($hIsLooping);
	submitScore();											// Send Highscore to Database(user.cs)
	guiMsgInit(); 											// HACK PLANET: For singapore
	endGameDisplayHighScoreList(%client);					// END GAME GUI, clear all except high score (server/highscore.cs)
	
	schedule(7500, 0, loadSplashScreen);	// FIX SINGAPORE: Replay again & again & again
}

function mainGameLoop(%client)
{
	cancel($hIsLooping);	// OPTIMIZATION: Stop cascading schedules
	
	//if(($arrDbInput.php_done[$iArrDbInput] $= "")||($arrDbInput.php_done[$iArrDbInput] $= "no"))
	queryDb();		// Read from database	
	
	//echo("cur_php_id = "@$cur_php_index);
	//echo("iArrDbInput = "@$iArrDbInput);
	
	if($cur_php_index < $iArrDbInput)
	{
		// Store message for processing
		%php_uid = getDbInput("php_uid", $cur_php_index);
		%php_hpnumber = getDbInput("php_mobile_number", $cur_php_index);
		%php_nickname = getDbInput("php_nickname", $cur_php_index);
		%php_action = getDbInput("php_action", $cur_php_index);
		%php_command = getDbInput("php_command", $cur_php_index);
		%php_done = getDbInput("php_done", $cur_php_index);
	}
	
	// ************DEBUG********************
	//echo("DEBUG_uid = "@%php_uid);
	//echo("DEBUG_hpnumber = "@%php_hpnumber);
	//echo("DEBUG_nickname = "@%php_nickname);
	//echo("DEBUG_action = "@%php_action);
	//echo("DEBUG_command = "@%php_command);
	//echo("DEBUG_done = "@%php_done);
	
	if (%php_done $= "yes")	 // if completed whole message stream
	{			
		//
		// *******************NEW MESSAGE; PROCESS DATA*************************
		//
			
		if(%php_action $= $RegKeyword)	// IF CHANGING NAME OR ADDING NEW PLAYER
		{
			// ERROR: Somehow sometimes php_nickname is lost in transfer from php into the executable.
			// This is a backup when php_nickname is empty
			if(%php_nickname $= "")
				%php_nickname = %php_command;	// Supposedly command is the player name			
			
           	if(!isPlayer(%php_hpnumber))	// If not registered
			{
           		addToWaitList(%php_hpnumber, %php_nickname);
			}
			else	// Changing nickname
			{
				%oldNick = getPlayerName(%php_hpnumber);						// Temporary keep the old user name for message use
				changeNickname(%php_hpnumber, %php_nickname);				// Change player nickname
					
				if(getPlayerSpawnPoint(%php_hpnumber) < 0)						// If not spawned, add to wait list
				{
					addToWaitList(%php_hpnumber, %php_nickname);
				}
				else																		// else update model shape name
				{	
					%hObject = getPlayerObjectHandle(%php_hpnumber);
					%hObject.setShapeName(%php_nickname);
					guiMsgIsNameChange(%oldNick, %php_nickname);				// show message of nick change
				}
			}
		}
		else										// IF SHOOTING
		{			
			if(!isPlayer(%php_hpnumber))	// IF PLAYER NOT STARTED THE GAME BUT REGISTERED WITH DATABASE
			{
           		addToWaitList(%php_hpnumber, %php_nickname);
			}
			
			%angle = %php_command;		// set angle from sms message
				
			// Limit the input within 0 to 360 degrees
			if( (%angle < 0) || (%angle > 360) )
			{
				%angle = 0;
			}
			
// ********************************* START EDIT GAME SPECIFIC SECTION************************************************************			
			
			if(getPlayerSpawnPoint(%php_hpnumber) > 0)
			{
				%hObject = getPlayerObjectHandle(%php_hpnumber);
						
				if($cameraLock < 0)
					cameraManager(%client, $camGodPlayer, %hObject, %angle); // CAMERA FUNCTION
      
				guiMsgIsShooting(%php_nickname, %angle);			// Show shooting msg (message.cs)
           		schedule(500, 0, rotate, %hObject, %angle, $RotationFreq, $RotationAng, "Shoot");
			}
			else
			{
				addToWaitList(%php_hpnumber, %php_nickname);
			}
// ********************************* END EDIT GAME SPECIFIC SECTION************************************************************
		}
		//$arrDbInput.php_done[$cur_php_index] = "no";// RESET & READY FOR NEXT MESSAGE STREAM
		//$cur_php_index++;							// For message repeat delay checking 
													// BECAUSE BAD LATENCY, MOVED TO SPECIFIC RENDERING ACTION FOR VALIDATION
													// Spawn / Shoot / Change Nickname (message.cs)
	}							
	
	updateHighScoreList(%client);
	updateWaitList(%client);
    
   $hIsLooping = schedule($GameLoopDelay * 1000, 0, mainGameLoop, %client);				// LOOP
}

// This seems to be out of place but there is nowhere to put this function
// and not hiding it in some dark places
function Shoot(%player)
{
	%player.setImageTrigger(0, true);	// PLAYER SHAPE SHOOT
	alxPlay(InGameFxShoot);
	echo(%player.getShapeName()@" is shooting!");
}
