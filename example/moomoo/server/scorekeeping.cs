// ============================================================
// Project            :  projMoomoo
// File               :  .\moomoo\server\scorekeeping.cs
// Copyright          :  
// Author             :  Ezham
// Created on         :  Thursday, February 22, 2007 2:23 PM
//
// Editor             :  Codeweaver v. 1.2.2595.6430
//
// Description        :  
//                    :  
//                    :  
// ============================================================

	// Struct for score keeping
	$arrScore			=	new ScriptObject();
	$playerCount		=	0;
	$HitScore			=	6;								// SCORE MARK
	// $arrScore.id[x] -> php_mobile_number
	// $arrScore.name[x] -> php_nickname
	// $arrScore.score[x] -> cumulative score
	
function addPlayer(%php_mobile_number, %php_nickname)
{
	if((%php_mobile_number $= "")||(%php_nickname $= ""))
		return false;
		
	$arrScore.id[$playerCount] = %php_mobile_number;
	$arrScore.name[$playerCount] = %php_nickname;
	$arrScore.score[$playerCount] = 0;
	$playerCount++;
	
	echo("Added new player = "@$arrScore.name[$playerCount - 1]);
		
	return true;
}

function getPlayer(%php_mobile_number)
{
	for(%i = 0; %i < $playerCount; %i++)
	{
		if(%php_mobile_number $= $arrScore.id[%i])
		{
			//echo("PLAYER IN ARRAY: HP:"@%php_mobile_number);
			return true;
		}
	}
	//echo("PLAYER NOT IN ARRAY: HP:"@%php_mobile_number);
	return false;
}

function getPlayerName(%php_mobile_number)
{
	for(%i = 0; %i < $playerCount; %i++)
	{
		if(%php_mobile_number $= $arrScore.id[%i])
		{
			//echo("PLAYER IN ARRAY: HP:"@%php_mobile_number);
			return ($arrScore.name[%i]);
		}
	}
	//echo("PLAYER NOT IN ARRAY: HP:"@%php_mobile_number);
	return (-1);
}

function checkNickChange(%php_mobile_number, %php_nickname)
{
	for(%i = 0; %i < $playerCount; %i++)
	{
		if(strcmp(%php_mobile_number, $arrScore.id[%i]) == 0)
		{
			if(strcmp($arrScore.name[%i], %php_nickname) != 0)
			{
				$arrScore.name[%i] = %php_nickname;
				echo("NICKCHANGE: HP:"@$arrScore.id[%i]@" is now "@$arrScore.name[%i]);
				break;
			}
		}
	}
	//echo("ERROR: Player "@%php_mobile_number@" not in array!");
}

function addScore(%php_mobile_number)
{
	for(%i = 0; %i < $playerCount; %i++)
	{
		if(%php_mobile_number $= $arrScore.id[%i])
		{
			$arrScore.score[%i]  += $HitScore;
			echo("HIT:Player = "@$arrScore.name[%i]@" have "@$arrScore.score[%i]@" points!");
			break;
		}
	}
	//echo("ERROR: Player "@%php_mobile_number@" not in array!");
}

// Submit Score to Database
function submitScore()
{
	for(%i = 0; %i < $playerCount; %i++)
	{
		%Sent = sendScore($arrScore.id[%i], $arrScore.name[%i], $arrScore.score[%i]);
		if(!%Sent)
		{
			echo("Did not manage to send :");
			echo("HP = "@$arrScore.id[%i]);
			echo("nickName = "@$arrScore.name[%i]);
			echo("score = "@$arrScore.score[%i]);
			echo("RESENDING...");
			continue;
		}
	}
}