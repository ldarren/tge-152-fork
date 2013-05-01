// ============================================================
// Project            :  TankAttack3D
// File               :  .\tank3d\server\highscore.cs
// Copyright          :  
// Author             :  Ezham
// Created on         :  Thursday, June 07, 2007 3:35 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

$pHighScoreSchedule = "";	// FIX SINGAPORE: pointer to the highscore rotating function

//-------------------------------------------------------------
//
//		HIGH SCORE LIST FUNCTIONS
//
//-------------------------------------------------------------

function updateHighScoreList(%client)		//UPDATE HIGH SCORE LIST
{
	commandToClient(%client, 'clearHighScore');			// reset HighScore List
	
	for(%i = 0; %i < $playerCount; %i++)
		if(stricmp($arrScore.name[%i], "") != 0)
			commandToClient(%client, 'SetScore', %i, $arrScore.name[%i], $arrScore.score[%i]);
}

/*
//
//			NOT IN USE - HIGH SCORE SHOWS ONLY AT THE END
//			ON SERVER SUMMORE, NEED REFACT
//
function displayHighScoreList(%switch)			// DISPLAY HIGH SCORE LIST
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
	
	$tagTimer = schedule(%duration, 0, ShowScoreBoard, %switch);
}
*/

function endGameDisplayHighScoreList(%client)				// END SCOREBOARD BASED ON TIME
{
	cancel($hTimer);
	cancel($hInstruction);
	
	HighScoreListBG.position = "289 123";
	HighScoreTxtCtrl.position = "29 63";
	HighScoreTxtCtrl.Extent = "164 258";
	HighScoreList.columns = "0 120 200";
	
	GameMsgBG.visible = 0;
	AutoScrollGameMsg.visible = 0;
	GameMsg.visible = 0;
	GameMsg.clear();
	
	InstructionMsgBG.visible = 0;
	AutoScrollInstructionMsg.visible = 0;
	InstructionMsg.visible = 0;
	InstructionMsg.clear();
	
	timerBG.visible = 0;
	CDTime.visible = 0;
	
	MapOverview.visible = 0;
	
	HighScoreListBG.visible = 1;
	HighScoreList.visible = 1;
}

function startGameDisplayHighScoreList(%client)				// FIX SINGAPORE: REDISPLAY GUI
{
	HighScoreListBG.visible = 0;
	HighScoreList.visible = 0;
	
	GameMsgBG.visible = 1;
	AutoScrollGameMsg.visible = 1;
	GameMsg.visible = 1;
	GameMsg.clear();
	
	InstructionMsgBG.visible = 1;
	AutoScrollInstructionMsg.visible = 1;
	InstructionMsg.visible = 1;
	InstructionMsg.clear();
	
	timerBG.visible = 1;
	CDTime.visible = 1;
	
	MapOverview.visible = 1;
}

function alternateGameDisplayHighScoreList()		// FIX SINGAPORE: High Score Display
{
	cancel($pHighScoreSchedule);
	
	if($isLooping)
	{
		if(HighScoreListBG.visible == "1")
		{
			HighScoreListBG.visible = "0";
			HighScoreTxtCtrl.visible = "0";
			HighScoreList.visible = "0";
			HighScoreListBG.position = "289 123";
			HighScoreTxtCtrl.position = "29 63";
			HighScoreTxtCtrl.Extent = "164 258";
			HighScoreList.columns = "0 120 200";
			$pHighScoreSchedule = schedule(22500, 0, alternateGameDisplayHighScoreList);
		}
		else
		{
			HighScoreListBG.position = "35 35";
			HighScoreTxtCtrl.position = "29 63";
			HighScoreTxtCtrl.Extent = "164 258";
			HighScoreList.columns = "0 120 200";
			HighScoreListBG.visible = "1";
			HighScoreTxtCtrl.visible = "1";
			HighScoreList.visible = "1";
			$pHighScoreSchedule = schedule(7500, 0, alternateGameDisplayHighScoreList);
		}
	}
}