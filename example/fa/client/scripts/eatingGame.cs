// ============================================================
// Project            :  Frontal_Assault
// File               :  .\fa\client\scripts\eatingGame.cs
// Copyright          :  
// Author             :  Darren
// Created on         :  Thursday, May 24, 2007 2:57 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

function clientCmdSetScoreCounter(%score)
{
	score.setText("Score:" SPC %score);
}

function clientCmdShowVictory(%score)
{
	MessageBoxYesNo("You Win!",
	"Would you like to restart the game ?",
	"callFirstMission();",
	"loadMainMenu();");
}