// ============================================================
// Project            :  TankAttack3D
// File               :  .\tank3d\server\user.cs
// Copyright          :  
// Author             :  Ezham
// Created on         :  Thursday, June 07, 2007 2:43 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

	// Struct for score keeping
	$arrScore			=	new ScriptObject();
	$playerCount		=	0;
	$HitScore			=	$pref::Game::HitScore;
	// $arrScore.id[x] -> php_mobile_number
	// $arrScore.name[x] -> php_nickname
	// $arrScore.score[x] -> cumulative score
    // $arrScore.objectHandle[x] -> handle to the world object
    // $arrScore.spawnPoint[x] -> id of spawn point occupied
	
function addPlayer(%php_mobile_number, %php_nickname, %objectHandle, %spawnPoint)
{
	if((%php_mobile_number $= "")||(%php_nickname $= ""))
		return false;
		
	$arrScore.id[$playerCount] = %php_mobile_number;
	$arrScore.name[$playerCount] = %php_nickname;
	$arrScore.score[$playerCount] = 0;
   $arrScore.objectHandle[$playerCount] = %objectHandle;
   $arrScore.spawnPoint[$playerCount] = %spawnPoint;
	$playerCount++;
	
	echo("Added new player = "@$arrScore.name[$playerCount - 1]);
		
	return true;
}

function isPlayer(%php_mobile_number)
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

function getPlayerObjectHandle(%php_mobile_number)
{
	for(%i = 0; %i < $playerCount; %i++)
	{
		if(%php_mobile_number $= $arrScore.id[%i])
		{
			//echo("PLAYER IN ARRAY: HP:"@%php_mobile_number);
			return ($arrScore.objectHandle[%i]);
		}
	}
	//echo("PLAYER NOT IN ARRAY: HP:"@%php_mobile_number);
	return (-1);
}

function getPlayerSpawnPoint(%php_mobile_number)
{
	for(%i = 0; %i < $playerCount; %i++)
	{
		if(%php_mobile_number $= $arrScore.id[%i])
		{
			//echo("PLAYER IN ARRAY: HP:"@%php_mobile_number);
			return ($arrScore.spawnPoint[%i]);
		}
	}
	//echo("PLAYER NOT IN ARRAY: HP:"@%php_mobile_number);
	return (-1);
}

function clearPlayerSpawnPoint(%php_mobile_number)
{
	for(%i = 0; %i < $playerCount; %i++)
	{
		if(%php_mobile_number $= $arrScore.id[%i])
		{
			//echo("PLAYER IN ARRAY: HP:"@%php_mobile_number);
			$arrScore.spawnPoint[%i] = -1;
			return true;
		}
	}
	//echo("PLAYER NOT IN ARRAY: HP:"@%php_mobile_number);
	return false;
}

function changeNickname(%php_mobile_number, %php_nickname)
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

function getPhoneNumber(%objectHandle)
{
	for(%i = 0; %i < $playerCount; %i++)
	{
		if(%objectHandle $= $arrScore.objectHandle[%i])
		{
			//echo("PLAYER IN ARRAY: HP:"@%php_mobile_number);
			return ($arrScore.id[%i]);
		}
	}
	//echo("PLAYER NOT IN ARRAY: HP:"@%php_mobile_number);
	return (-1);
}

function resetPlayerObject(%php_mobile_phone, %hPlayer, %spawnPoint)
{
	for(%i = 0; %i < $playerCount; %i++)
	{
		if(%php_mobile_phone $= $arrScore.id[%i])
		{
			//echo("PLAYER IN ARRAY: HP:"@%php_mobile_number);
			$arrScore.objectHandle[%i] = %hPlayer;
			$arrScore.spawnPoint[%i] = %spawnPoint;
			break;
		}
	}
	//echo("PLAYER NOT IN ARRAY: HP:"@%php_mobile_number);
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
