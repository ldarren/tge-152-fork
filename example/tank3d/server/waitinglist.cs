// ============================================================
// Project            :  TankAttack3D
// File               :  .\tank3d\server\waitinglist.cs
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

	// Struct for waiting list
	$arrWaitList		=	new ScriptObject();
	$WaitListCount		=	0;
	$WaitListBaseIndex	=	0;
	// $arrWaitList.id[x] -> php_mobile_number
	// $arrWaitList.name[x] -> php_nickname

function addToWaitList(%php_mobile_number, %php_nickname)
{
	if((%php_mobile_number $= "")||(%php_nickname $= ""))
		return false;
		
	$arrWaitList.id[$WaitListCount] = %php_mobile_number;
	$arrWaitList.name[$WaitListCount] = %php_nickname;
   $WaitListCount++;

	echo("Added to waiting list = "@$arrWaitList.name[$WaitListCount - 1]);
		
	return true;
}

function moveWaitListIntoGame(%client)
{
    %mobile_number = $arrWaitList.id[$WaitListBaseIndex];
    %nickname = $arrWaitList.name[$WaitListBaseIndex];
    $WaitListBaseIndex++;
	
	 spawnPlayer(%client, %mobile_number,%nickname);
    echo("Added to game = "@$arrWaitList.name[$WaitListBaseIndex - 1]);
	 guiMsgEntersGame(%nickname);	 // Show enter game msg (message.cs)
}

function updateWaitList(%client)
{
   if(SpawnPointAvailable())
	{
      if($WaitListBaseIndex < $WaitListCount)
         moveWaitListIntoGame(%client);
	}
}
