// ============================================================
// Project            :  TankAttack3D
// File               :  .\tank3d\server\spawn.cs
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

	$SpawnPointFlag = new ScriptObject();  // Flag Array for Occupied Spawn Point
   // $spawnPointFlag.index[x] <- spawn point number
    
   $MaxSpawnPoint = 20;        // HARDCODED: Edit when needed, and at own risk
   $MaxTankType   = 3;         // HARDCODED: Available tank type
	
	$TempSpawnPointIndex = 0;	// HACK!!!
										// It was well bloody late before i realized this basic mistake
										// Now i need to use a global Temp to compensate the stupid design.
										// Use to transmit the spawn point index from one function to another
    

function spawnPlayer(%client, %php_mobile_number,%php_nickname)
{
    // Assign Tank Type to Player
    %tankType = mCeil(getRandom(1,$MaxTankType));
    %datablockType = "PlayerShape"@%tankType;
    
    // Create new Player
    %hPlayer  = new Player(){
                    datablock = %datablockType;
                    client = %client;
                };
                
	MissionCleanup.add(%hPlayer);
 
   if($cameraLock < 0)
		cameraManager(%client, $camSpawnPlayer,  %hPlayer);  // CAMERA FUNCTION
	
    // Get Spawn Point
    %SpawnPoint = pickSpawnPoint();

	%hPlayer.setTransform(%SpawnPoint);
  	%hPlayer.setShapeName(%php_nickname); // Set Name For GUI use
	alxPlay(InGameFxSpawn);
   
   // Add Player Info to Array (user.cs)
	if(!isPlayer(%php_mobile_number))
	{
   		addPlayer(%php_mobile_number, %php_nickname, %hPlayer, $TempSpawnPointIndex);	// HACK!!!
	}
	else 
	{
		resetPlayerObject(%php_mobile_number, %hPlayer, $TempSpawnPointIndex);	// HACK!!!
	}
}

function pickSpawnPoint()
{
	%iSpawnPoint = mCeil(getRandom(1,$MaxSpawnPoint));

	if ($SpawnPointFlag.index[%iSpawnPoint] == 1)  // If Spawn Point Occupied
	{
		pickSpawnPoint(); // Rescan for unoccupied
	}
	else
	{
 		// Pick the first object in drop point group and use it's
		// location as a spawn point.
      %group = nameToID("MissionGroup/SpawnPoint"@%iSpawnPoint);
      $SpawnPointFlag.index[%iSpawnPoint] = 1; // Flag as occupied
		
		$TempSpawnPointIndex = %iSpawnPoint;	// HACK!!!
		
		if (%group != -1 && %group.getCount() != 0)
			return %group.getObject(0).getTransform();
   
   		// If no object was found, return a point near the center of the world
		error("Missing spawn point object and/or mission group " @ %groupName);
			return "0 0 300 1 0 0 0";
     }
}

function clearSpawnPoint(%SpawnPoint)
{
   $SpawnPointFlag.index[%SpawnPoint] = 0; // Flag as unoccupied
}

function SpawnPointAvailable()
{
     for(%i = 1; %i < $MaxSpawnPoint + 1; %i++)
     {
      if($SpawnPointFlag.index[%i] == 0)
         return true;
     }
     return false;
}
