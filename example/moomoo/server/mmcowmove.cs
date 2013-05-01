// ============================================================
// Project            :  projMoomoo
// File               :  .\moomoo\server\mmcowmove.cs
// Copyright          :  
// Author             :  Ezham
// Created on         :  Monday, February 26, 2007 2:26 PM
//
// Editor             :  Codeweaver v. 1.2.2595.6430
//
// Description        :  
//                    :  
//                    :  
// ============================================================


datablock PlayerData( MooBot:PlayerShape )
{
   renderFirstPerson = false;
   emap = true;
   
   shapeFile = "~/data/shapes/moomoo/moomooani.dts";//"~/data/shapes/moomoo/moomooani.dts";
   computeCRC = true;
  
   aiAvoidThis = true;

   minLookAngle = 0.05;			// Overide Collision Box to 'Push' the model down
   maxLookAngle = 0.05;			// Overide Collision Box to 'Push' the model down
   maxFreelookAngle = 3.0;

   mass = 80;
   drag = 0.5;
   maxdrag = 0.7;
   density = 10;
   maxDamage = 10;
   maxEnergy =  60;
   repairRate = 0.33;
   energyPerDamagePoint = 75.0;

   rechargeRate = 0.256;

   runForce = 48 * 90;
   runEnergyDrain = 0;
   minRunEnergy = 0;
   maxForwardSpeed = 1;//14;
   maxBackwardSpeed = 1;//13;
   maxSideSpeed = 1;//13;

   maxUnderwaterForwardSpeed = 8.4;
   maxUnderwaterBackwardSpeed = 7.8;
   maxUnderwaterSideSpeed = 7.8;

   jumpForce = 8.3 * 90;
   jumpEnergyDrain = 0;
   minJumpEnergy = 0;
   jumpDelay = 15;

   recoverDelay = 9;
   recoverRunForceScale = 1.2;

   minImpactSpeed = 45;
   speedDamageScale = 0.4;

   boundingBox = "0.75 0.75 0.75";
   pickupRadius = 0.75;
   
   // Controls over slope of runnable/jumpable surfaces
   runSurfaceAngle  = 70;
   jumpSurfaceAngle = 80;

   minJumpSpeed = 20;
   maxJumpSpeed = 30;

   horizMaxSpeed = 68;
   horizResistSpeed = 33;
   horizResistFactor = 0.35;

   upMaxSpeed = 80;
   upResistSpeed = 25;
   upResistFactor = 0.3;
     
   observeParameters = "0.5 4.5 4.5";

};

datablock PlayerData( MooBot2:MooBot )
{
   shapeFile = "~/data/shapes/moogun/moomooani2.dts";//"~/data/shapes/moomoo/moomooani.dts";
};

//-----------------------------------------------------------------------------
// MooBot Function
//-----------------------------------------------------------------------------

function MooBot::onReachDestination( %this, %obj )
{
	// Moves to the next node on the path.
	if( %obj.path $= "" )
	{
		if( %obj.currentNode == %obj.targetNode )
			%this.onEndOfPath( %obj, %obj.path );
		else
			%obj.moveToNextNode();
	}
	else
		echo( "MooBot::onReachDestination warning - Path is blank!" );
}

function MooBot::onEndOfPath( %this, %obj, %path )
{
	%obj.nextTask();
}

function MooBot::onEndSequence(%this,%obj,%slot)
{
   echo("Sequence Done!");
   %obj.stopThread(%slot);
   %obj.nextTask();
}

//----------------------------------------------------------------------------
// MooBot2 Function
//-----------------------------------------------------------------------------

// **Funnily enough, MooBot2 did NOT inherit the MooBot methods. Private perhaps?

function MooBot2::onReachDestination( %this, %obj )
{
	// Moves to the next node on the path.
	if( %obj.path !$= "" )
	{
		if( %obj.currentNode == %obj.targetNode )
			%this.onEndOfPath( %obj, %obj.path );
		else
			%obj.moveToNextNode();
	}
	else
		echo( "MooBot::onReachDestination warning - Path is blank!" );
}

function MooBot2::onEndOfPath( %this, %obj, %path )
{
	%obj.nextTask();
}

function MooBot2::onEndSequence(%this,%obj,%slot)
{
   echo("Sequence Done!");
   %obj.stopThread(%slot);
   %obj.nextTask();
}

//-----------------------------------------------------------------------------
// AIPlayer static functions
//-----------------------------------------------------------------------------

function AIPlayer::MooSpawn( %name, %spawnPoint, %objType )
{
	// Create the A.I. driven bot object...
	if(%objType > 1)
		%datablockType = "MooBot2";
	else
		%datablockType = "MooBot";
	
	%player = new AIPlayer()
	{
		dataBlock = %datablockType;
		path = "";
	};

	MissionCleanup.add( %player );
	%player.setShapeName( %name );
	%player.setTransform( %spawnPoint );

	return %player;
}

function AIPlayer::MooSpawnOnPath( %name, %path, %objType )
{
	// Spawn a bot and place him on the first node of the path
	if( !isObject( %path ) )
	{
		echo( "AIPlayer::spawnOnPath failed - Bad Path!" );
		%this.path = "";
		return;
	}

	%node = %path.getObject(0);
	%player = AIPlayer::MooSpawn( %name, %node.getTransform(), %objType );

	return %player;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// AIManager static functions
//-----------------------------------------------------------------------------

function AIManager::MooThink( %this )
{
	%this.MooSpawn();
}
$mmCamPivot = 0;
function AIManager::MooSpawn( %this , %id)
{	
	$mmCamPivot = AIPlayer::MooSpawnOnPath( "Moo_1", "MissionGroup/MooPath1", 1 );
	//%bot.followPath( "MissionGroup/MooPath1", -1 );
	
	%bot = AIPlayer::MooSpawnOnPath( "Moo_2", "MissionGroup/MooPath2", 2 );
	//%bot.followPath( "MissionGroup/MooPath2", -1 );
	
	%bot = AIPlayer::MooSpawnOnPath( "Moo_3", "MissionGroup/MooPath3", 2 );
	//%bot.followPath( "MissionGroup/MooPath3", -1 );
	
	return (%bot);
}