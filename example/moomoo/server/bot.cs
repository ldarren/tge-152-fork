//-----------------------------------------------------------------------------
// To create a Bot or A.I. driven player, we'll simply create a new PlayerData
// datablock called "MyBOT" which derives all of its functionality from the
// "PlayerShape" datablock. This way our Bot can do anything that a human 
// player can do. The only difference is, our Bot is being controlled by the 
// computer so we need to add a few extra functions so it can think for it 
// self.
//-----------------------------------------------------------------------------

//GLOBALS
//-------
$arrBot = new ScriptObject() {};
$botCount = 7; //BOT NUMBER BERY IMPORTANT!!!

//-----------------------------------------------------------------------------
// Audio Datablock
//-----------------------------------------------------------------------------

datablock AudioProfile(UFOHoverSound)
{
	filename = "~/data/sound/UFOhovers.ogg";//"~/data/sound/crossbow_explosion.ogg";
	description = Audio2DUFO;
	preload = true;
};

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Bot Datablock
//-----------------------------------------------------------------------------

datablock PlayerData( MyBot:PlayerShape)//PlayerData:PLayerSHape
{
   renderFirstPerson = false;
   emap = true;
	shapeFile = "~/data/shapes/moomoo/ufo1.dts"; //**MODEL BUG, NEED MODELLER**
   computeCRC = true;
   
	aiAvoidThis = true;
	
   minLookAngle = -0.10;
   maxLookAngle = 0.10;			// EDIT SPEED COMING DOWN
   maxFreelookAngle = 3.0;

   mass = 100;
   drag = 0.9;
   maxdrag = 1.0;
   density = 10;
   maxDamage = 1;
   maxEnergy =  60;
   repairRate = 0.33;
   energyPerDamagePoint = 75.0;

   rechargeRate = 0.256;

   runForce = 48 * 90;
   runEnergyDrain = 0;
   minRunEnergy = 0;
   maxForwardSpeed = 3;//14;
   maxBackwardSpeed = 2;//13;
   maxSideSpeed = 2;//13;

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

   boundingBox = "3.75 3.75 3.75";
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

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// AIPlayer Path Following Functions
//-----------------------------------------------------------------------------

function MyBot::onReachDestination( %this, %obj )
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
		echo( "MyBot::onReachDestination warning - Path is blank!" );
}

function MyBot::onEndOfPath( %this, %obj, %path )
{
	%obj.nextTask();
}

function MyBot::onEndSequence(%this,%obj,%slot)
{
   echo("Sequence Done!");
   %obj.stopThread(%slot);
   %obj.nextTask();
}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// AIPlayer static functions
//-----------------------------------------------------------------------------

function AIPlayer::spawn( %name, %spawnPoint )
{
	// Create the A.I. driven bot object...
	%player = new AIPlayer()
	{
		dataBlock = MyBot;
		path = "";
	};

	MissionCleanup.add( %player );
	
	%player.setShapeName( %name );
	//%player.setTransform( %spawnPoint );	// Normally, but spawned ABOVE the spawn point
	
	%lx = getword(%spawnPoint,0); // first, get the current transform values
  	%ly = getword(%spawnPoint,1);
  	%lz = getword(%spawnPoint,2);
	
	//%player.setVelocity("0 0 0");
	//%player.clearAim();

	%player.setTransform(%lx SPC %ly SPC %lz + 50);
	
		
	if($mmshooting == true)
		schedule(5000, 0, "alxPlay", UFOHoverSound);		// Play in game music

	return %player;
}

function AIPlayer::spawnOnPath( %name, %path )
{
	// Spawn a bot and place him on the first node of the path
	if( !isObject( %path ) )
	{
		echo( "AIPlayer::spawnOnPath failed - Bad Path!" );
		%this.path = "";
		return;
	}

	%node = %path.getObject(0);
	%player = AIPlayer::spawn( %name, %node.getTransform() );

	return %player;
}

function AIPlayer::followPath( %this, %path, %node )
{
   // Start the bot following a path
	%this.stopThread(0);
	
	if( !isObject( %path ) )
	{
		echo( "AIPlayer::followPath failed - Bad Path!" );
		%this.path = "";
		return;
	}

	if( %node > %path.getCount() - 1 )
		%this.targetNode = %path.getCount() - 1;
	else
		%this.targetNode = %node;

	if( %this.path $= %path )
		%this.moveToNode(%this.currentNode);
	else 
	{
		%this.path = %path;
		%this.moveToNode(0);
	}
}

function AIPlayer::moveToNextNode( %this )
{
	if( %this.targetNode < 0 || %this.currentNode < %this.targetNode )
	{
		if( %this.currentNode < %this.path.getCount() - 1 )
			%this.moveToNode( %this.currentNode + 1 );
		else
			%this.moveToNode( 0 );
	}
	else
	{
		if( %this.currentNode == 0 )
			%this.moveToNode( %this.path.getCount() - 1 );
		else
			%this.moveToNode( %this.currentNode - 1 );
	}
}

function AIPlayer::moveToNode( %this, %index ) // PROBLEM
{
   // Move to the given path node index
   %this.currentNode = %index;
   %node = %this.path.getObject(%index);
	%this.setAimLocation(%node.getTransform());
   //%this.schedule(getRandom(5,10) * 100, setMoveDestination, %node.getTransform(), %index == %this.targetNode);
	%this.setMoveDestination( %node.getTransform(), %index == %this.targetNode );
}

function AIPlayer::animate(%this,%seq)
{
   %this.setActionThread(%seq);
}

function AIPlayer::blowup(%this)
{
	%pos = %this.getPosition();

	%x = new explosion() {
		dataBlock = "CrossbowExplosion";
		position = %pos;
	};
	MissionCleanup.add(%x);
}

function AIPlayer::damage(%this, %obj, %sourceObject, %position, %damage, %damageType)
{
   %obj.applyDamage(%damage);
   
	for(%i = 0; %i < $botCount; %i++)	// Find which bot to delete
		if(%obj $= $arrBot.id[%i])
			$arrBot.id[%i] = -1;
	
	%obj.blowup();
	%obj.schedule(10, "delete");
}

function AIPlayer::kill(%this, %damageType)
{
   %this.damage(%this, %this.getPosition(), 1000, %damageType);
   echo("KILLED A BOT");
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// AIManager static functions
//-----------------------------------------------------------------------------

function AIManager::think( %this )
{
   //if (!isObject(%this.player))			// NORMALLY
   //   %this.player = %this.spawn();
   //%this.schedule(500,think);
	
	for(%i = 0; %i < $botCount; %i++)
   	if( !isObject( $arrBot.id[%i] ) )
			%this.player = %this.spawn(%i);
	
   %this.schedule( 500, think );
}

function AIManager::spawn( %this , %id)
{
	%PathName = "MissionGroup/myPath"@%id + 1;
	%BotName = "Bot_"@%id + 1;
	
	$arrBot.id[%id] = AIPlayer::spawnOnPath( %BotName, %PathName );
	$arrBot.id[%id].followPath( %PathName, -1 );
	
	return ($arrBot.id[%id]);

}