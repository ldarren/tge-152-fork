//-----------------------------------------------------------------------------
// To create a Bot or A.I. driven player, we'll simply create a new PlayerData
// datablock called "MyBOT" which derives all of its functionality from the
// "PlayerShape" datablock. This way our Bot can do anything that a human 
// player can do. The only difference is, our Bot is being controlled by the 
// computer so we need to add a few extra functions so it can think for it 
// self.
//-----------------------------------------------------------------------------
datablock PlayerData( MyBot : PlayerBody )
{
	// TO DO: Add extra stuff here to manage new A.I. functionality...
	patrol = true;
	attack = false;
};

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
	%player.setTransform( %spawnPoint );
    %player.mountImage( CrossbowImage, 0 );
    %player.setImageAmmo( 0, 1 );

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

function AIPlayer::moveToNode( %this, %index )
{
   // Move to the given path node index
   %this.currentNode = %index;
   %node = %this.path.getObject(%index);
   %this.setMoveDestination( %node.getTransform(), %index == %this.targetNode );
}

//-----------------------------------------------------------------------------
// AIManager static functions
//-----------------------------------------------------------------------------

function AIManager::think( %this )
{
   if( !isObject( %this.player ) )
      %this.player = %this.spawn();

   %this.schedule( 5000, think );
}

function AIManager::spawn( %this )
{
	// Bot_1 simply spawns without a path to follow. Since he has no path to 
	// follow, he'll simply spawn at the same spot that you do. This means that 
	// you'll have to move around to see him when the game first starts because
	// he'll be right on top of you.

	//%bot = AIPlayer::spawn( "Bot_1", pickSpawnPoint() );

	// Bot_2 has a path to follow, so we'll help him out by having him spawn
	// by the Path's first Path Marker. Once he spawns, he'll begin to follow
	// the path using the A.I. helper functions defined here in this file.

	%bot = AIPlayer::spawnOnPath( "Bot_2", "MissionGroup/AI_Path1" );
	%bot.followPath( "MissionGroup/AI_Path1", -1 );

   return %bot;
}
