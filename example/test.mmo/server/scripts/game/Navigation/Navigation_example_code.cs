// This file re-implements the file aiPlayer.cs in the standard torque engine.
// All it does is to use the precompiled node resource for the navigation
// and selecting new destinations at random.
//
//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// AIPlayer callbacks
// The AIPlayer class implements the following callbacks:
//
//    PlayerData::onStuck(%this,%obj)
//    PlayerData::onUnStuck(%this,%obj)
//    PlayerData::onStop(%this,%obj)
//    PlayerData::onMove(%this,%obj)
//    PlayerData::onReachDestination(%this,%obj)
//    PlayerData::onTargetEnterLOS(%this,%obj)
//    PlayerData::onTargetExitLOS(%this,%obj)
//    PlayerData::onAdd(%this,%obj)
//
// Since the AIPlayer doesn't implement it's own datablock, these callbacks
// all take place in the PlayerData namespace.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Demo Pathed AIPlayer.
//-----------------------------------------------------------------------------

$NUMBER_OF_BOTS_TO_SPAWN = 1;
AIManager.botsSpawned = 0;

datablock PlayerData(DemoPlayer : StandardBody)
{
   shootingDelay = 2000;
};

function DemoPlayer::onReachDestination(%this, %obj)
{
   navigationTrace(%this, "onReachDestination(" @ %obj @ ")", 5);
   
   // Moves to the next node on the path.
   // Override for all player. Normally we'd override this for only
   // a specific player datablock or class of players.

   if (%obj.currentNode == %obj.targetNode) {
      %this.onEndOfPath(%obj);
   } else {
      %obj.moveToNextNode();
   }
}

function getRandomNodeIndex()
{
   %nodeCount = NodeGraph.nodeCount();
   %randomPosition = getRandom(%nodeCount - 1);
   
   navigationTrace(%this, "Orc selected a random node: " @ %randomPosition, 5);
   
   return %randomPosition;
}

function isNodeReachable(%fromId, %toId)
{
   return NodeGraph.getNextNodeIndexOnPath(%fromId, %toId) != -1;
}

function DemoPlayer::onEndOfPath(%this, %obj)
{
   navigationTrace(%this, "onEndOfPath(" @ %obj @ ", " @ %obj @ ")", 5);
   
   %nextTarget = getRandomNodeIndex();
   while(%nextTarget != %obj.targetNode) {
      navigationTrace(%this, "Next random node is the same as the current location, re-trying.", 5);
      %nextTarget = getRandomNodeIndex();
   }
   %obj.followPath(getRandomNodeIndex());
   
   %obj.nextTask();
}

function DemoPlayer::onEndSequence(%this, %obj)
{
   navigationTrace(%this, "onEndSequence(" @ %obj @ ", " @ %obj @ ", " @ %slot @  ")", 5);
   
   %obj.stopThread(%slot);
   %obj.nextTask();
}


//-----------------------------------------------------------------------------
// AIPlayer static functions
//-----------------------------------------------------------------------------

function AIPlayer::spawn(%name,%spawnPoint)
{
   navigationTrace("", "spawn(" @ %name @ ", " @ %spawnPoint @ ")", 5);
   
   // Create the demo player object
   %player = new AiPlayer() {
      dataBlock = DemoPlayer;
   };
   MissionCleanup.add(%player);
   %player.setShapeName(%name);
   %player.setTransform(%spawnPoint);
   return %player;
}

function AIPlayer::spawnOnPath(%name, %index)
{
   navigationTrace("", "spawnOnPath(" @ %name @ ", " @ %index @ ")", 5);
   
   // Spawn a player and place him on the first node of the path
   if (%index < 0) {
      navigationTrace("", "WARNING: Index " @ %index @ " considered bad, changing to 0", 5);
      %index = 0;
   }
   %place = NodeGraph.getNode(%index);
   %player = AIPlayer::spawn(%name, %place);
   %player.currentNode = %index;
   return %player;
}


//-----------------------------------------------------------------------------
// AIPlayer methods 
//-----------------------------------------------------------------------------

function AIPlayer::followPath(%this, %node)
{
   navigationTrace(%this, "followPath(" @ %node @ ")", 5);
   
   // Start the player following a path
   %this.stopThread(0);
   %this.currentNode = NodeGraph.getClosestNodeIndex(%this.getPosition());
   
   if(%node < 0) {
      navigationTrace("", "WARNING: Index " @ %node @ " considered bad, changing to 0", 5);
      %node = 0;
   }
   %this.targetNode = %node;
   
   // Update the name to reflect new position
   %this.setShapeName("Ork -> NodeID: " @ %node);
   
   %this.moveToNextNode();
}

function AIPlayer::moveToNextNode(%this)
{
   navigationTrace(%this, "moveToNextNode()", 5);
   
   %nextNode = NodeGraph.getNextNodeIndexOnPath(%this.currentNode, %this.targetNode);
   if(%nextNode == -1) {
      return false;
   }
   navigationTrace(%this, "Next node on path from " @ %this.currentNode @ " to " @ %this.targetNode @ " is " @ %nextNode, 5);
   %this.currentNode = %nextNode;
   %this.moveToNode(%this.currentNode);
   return true;
}

function AIPlayer::moveToNode(%this, %index)
{
   navigationTrace(%this, "moveToNode(" @ %index @ ")", 5);
   
   %this.currentNode = %index;
   %node = NodeGraph.getNode(%this.currentNode);
   %this.setMoveDestination(%node, %this.currentNode == %this.targetNode);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

function AIPlayer::pushTask(%this, %method)
{
   navigationTrace(%this, "pushTask(" @ %method @ ")", 5);
   
   if (%this.taskIndex $= "") {
      %this.taskIndex = 0;
      %this.taskCurrent = -1;
   }
   %this.task[%this.taskIndex] = %method; 
   %this.taskIndex++;
   if (%this.taskCurrent == -1)
      %this.executeTask(%this.taskIndex - 1);
}

function AIPlayer::clearTasks(%this)
{
   navigationTrace(%this, "clearTasks()", 5);
   
   %this.taskIndex = 0;
   %this.taskCurrent = -1;
}

function AIPlayer::nextTask(%this)
{
   navigationTrace(%this, "nextTask()", 5);
   
   if (%this.taskCurrent != -1)
      if (%this.taskCurrent < %this.taskIndex - 1)
         %this.executeTask(%this.taskCurrent++);
      else
         %this.taskCurrent = -1;
}

function AIPlayer::executeTask(%this,%index)
{
   navigationTrace(%this, "executeTask(" @ %index @ ")", 5);
   
   %this.taskCurrent = %index;
   eval(%this.getId() @ "." @ %this.task[%index] @ ";");
}


//-----------------------------------------------------------------------------

function AIPlayer::singleShot(%this)
{
   navigationTrace(%this, "singleShot()", 5);
   
   // The shooting delay is used to pulse the trigger
   %this.setImageTrigger(0,true);
   %this.setImageTrigger(0,false);
   %this.trigger = %this.schedule(%this.shootingDelay,singleShot);
}


//-----------------------------------------------------------------------------

function AIPlayer::wait(%this,%time)
{
   navigationTrace(%this, "wait(" @ %time @ ")", 5);
   
   %this.schedule(%time * 1000,"nextTask");
}

function AIPlayer::done(%this,%time)
{
   navigationTrace(%this, "done(" @ %time @ ")", 5);
   
   %this.schedule(0,"delete");
}

function AIPlayer::fire(%this,%bool)
{
   navigationTrace(%this, "fire(" @ %bool @ ")", 5);
   
   if (%bool) {
      cancel(%this.trigger);
      %this.singleShot();
   }
   else
      cancel(%this.trigger);
   %this.nextTask();
}

function AIPlayer::aimAt(%this,%object)
{
   navigationTrace(%this, "aimAt(" @ %object @ ")", 5);
   
   %this.setAimObject(%object);
   %this.nextTask();
}

function AIPlayer::animate(%this,%seq)
{
   navigationTrace(%this, "animate(" @ %seq @ ")", 5);
   
   //%this.stopThread(0);
   //%this.playThread(0,%seq);
   %this.setActionThread(%seq);
}


//-----------------------------------------------------------------------------

function AIManager::think(%this)
{
   navigationTrace(%this, "think()", 5);
   
   if( %this.botsSpawned < $NUMBER_OF_BOTS_TO_SPAWN ) {
	AIManager.botsSpawned++;
	%this.spawn();
   }
   %this.schedule(500,think);
}

function AIManager::spawn(%this)
{
   navigationTrace(%this, "spawn()", 5);
   
   %randomPosition = getRandomNodeIndex();
   
   %player = AIPlayer::spawnOnPath("Ork -> NodeID: " @ %randomPosition, %randomPosition);
   
   %toNode = getRandomNodeIndex();
	%i = 0;
   while(!isNodeReachable(%player.currentNode, %toNode)) {
      navigationTrace(%this, "Node " @ %toNode @ " not reachable from node " @ %player.currentNode , 5);
      %toNode = getRandomNodeIndex();
	%i++;
	if (%i > 50) return %player;
   }
   
   %player.followPath(%toNode);

   %player.mountImage(CrossbowImage,0);
   %player.setInventory(CrossbowAmmo,1000);
   return %player;
}