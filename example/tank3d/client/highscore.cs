// ============================================================
// Project            :  TankAttack3D
// File               :  .\tank3d\client\highscore.cs
// Copyright          :  
// Author             :  Ezham
// Created on         :  Thursday, June 07, 2007 4:16 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

function clientCmdclearHighScore()	// Clear High Score List
{
	HighScoreList.clear();
}

function clientCmdSetScore(%id, %name, %score)	// Put Scores into High Score List
{
   // Copy the current scores from the player list into the
   // end game gui (bit of a hack for now).
   HighScoreList.addRow(%id, %name TAB %score);
   HighScoreList.sortNumerical(1,false);
}
