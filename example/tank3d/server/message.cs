// ============================================================
// Project            :  TankAttack3D
// File               :  .\tank3d\server\message.cs
// Copyright          :  
// Author             :  Ezham
// Created on         :  Thursday, June 07, 2007 3:42 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

function guiMsgEntersGame(%name)	// DISPLAY PLAYER ENTER GAME MESSAGE
{
	GameMsg.addText("<color:ffffff><just:center><br>"@%name@" has entered the game!", true);
					
	$cur_php_index++;				// For message repeat delay checking 
									// BECAUSE BAD LATENCY, MOVED TO SPECIFIC RENDERING ACTION FOR VALIDATION
				
}

function guiMsgIsShooting(%name, %angle)	// DISPLAY SHOOTING MESSAGE
{
	GameMsg.addText("<color:ffffff><just:center><br>"@%name@" is shooting at "@%angle@"!", true);
	
					
	$cur_php_index++;				// For message repeat delay checking 
									// BECAUSE BAD LATENCY, MOVED TO SPECIFIC RENDERING ACTION FOR VALIDATION
				
}

function guiMsgIsHit(%shooter, %victim)	// DISPLAY PLAYER IS HIT MESSAGE
{
	GameMsg.addText("<just:center><br><color:ffff00>"@%victim@" was destroyed by "@%shooter@"!<color:ffffff>", true);
}

function guiMsgIsMiss(%shooter)	// DISPLAY PLAYER MISSES MESSAGE
{
	GameMsg.addText("<just:center><br><color:ff0000>"@%shooter@" misses!<color:ffffff>", true);
}

function guiMsgInit()	// DISPLAY EMPTY (INITIALIZE Cause .clear() clearly did not work)
{
	GameMsg.addText("<br>", true);
}

function guiMsgIsNameChange(%oldname, %newname)	// DISPLAY PLAYER CHANGES NICK MESSAGE
{
	GameMsg.addText("<just:center><br>"@%oldname@" is now known as "@%newname, true);
					
	$cur_php_index++;				// For message repeat delay checking 
									// BECAUSE BAD LATENCY, MOVED TO SPECIFIC RENDERING ACTION FOR VALIDATION
				
}