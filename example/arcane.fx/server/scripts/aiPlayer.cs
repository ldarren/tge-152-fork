//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//    Changes:
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

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

// AFX <<
//  Name of base player data is now OrcWarriorAvatar instead of PlayerBody.
datablock PlayerData(DemoPlayer : OrcWarriorAvatar)
{
   shootingDelay = 2000;
};
// AFX >>

function DemoPlayer::onReachDestination(%this,%obj)
{
   // Moves to the next node on the path.
   // Override for all player. Normally we'd override this for only
   // a specific player datablock or class of players.
   if (%obj.path !$= "") {
      if (%obj.currentNode == %obj.targetNode)
         %this.onEndOfPath(%obj,%obj.path);
      else
         %obj.moveToNextNode();
   }
}

function DemoPlayer::onEndOfPath(%this,%obj,%path)
{
   %obj.nextTask();
}

function DemoPlayer::onEndSequence(%this,%obj,%slot)
{
   echo("Sequence Done!");
   %obj.stopThread(%slot);
   %obj.nextTask();
}


//-----------------------------------------------------------------------------
// AIPlayer static functions
//-----------------------------------------------------------------------------

function AIPlayer::spawn(%name,%spawnPoint)
{
   // Create the demo player object
   %player = new AiPlayer() {
      dataBlock = DemoPlayer;
      path = "";
   };
   MissionCleanup.add(%player);
   %player.setShapeName(%name);
   %player.setTransform(%spawnPoint);
   return %player;
}

// AFX <<
//  This function is modified to take a boolean %dead argument.
//  When true it causes the AI to be immediatly killed so
//  that it starts as a corpse. 
function AIPlayer::spawnOnPath(%name, %path, %dead)
{
   // Spawn a player and place him on the first node of the path
   if (!isObject(%path))
      return;
   %node = %path.getObject(0);

   return AIPlayer::spawnAt(%name, %node.getTransform(), %dead);
}
// AFX >>

// AFX <<
//  This function is added to allow AI spawning at a predetermined
//  location. It's useful for spawning a new AI at the location of
//  a corpse as part of a resurrection.
//  The boolean %dead argument, when true causes the AI to be
//  immediatly killed so that it starts as a corpse. 
//  The AI's movement speed is slowed down a little.
function AIPlayer::spawnAt(%name, %xfm, %dead)
{
  %ai_player = AIPlayer::spawn(%name, %xfm);
  %ai_player.setMoveSpeed(0.4);

  // start as corpse
  if (%dead)
    %ai_player.kill("DOA");

  return %ai_player;
}
// AFX >>

//-----------------------------------------------------------------------------
// AIPlayer methods 
//-----------------------------------------------------------------------------

function AIPlayer::followPath(%this,%path,%node)
{
   // Start the player following a path
   %this.stopThread(0);
   if (!isObject(%path)) {
      %this.path = "";
      return;
   }
   if (%node > %path.getCount() - 1)
      %this.targetNode = %path.getCount() - 1;
   else
      %this.targetNode = %node;
   if (%this.path $= %path)
      %this.moveToNode(%this.currentNode);
   else {
      %this.path = %path;
      %this.moveToNode(0);
   }
}

// AFX <<
//  This function has been modified to recognize two types of path
//  traversal: sequential or randomized.
function AIPlayer::moveToNextNode(%this)
{
  if (%this.pathTraversal $= "randomized")
  {
    %next_waypoint = %this.currentNode;
    while (%next_waypoint == %this.currentNode)
      %next_waypoint = getRandom(%this.path.getCount() - 1);
    %this.moveToNode(%next_waypoint);
  }
  else // if (%this.pathTraversal $= "sequential")
  {
    if (%this.targetNode < 0 || %this.currentNode < %this.targetNode) 
    {
      if (%this.currentNode < %this.path.getCount() - 1)
        %this.moveToNode(%this.currentNode + 1);
      else
        %this.moveToNode(0);
    }
    else
    {
      if (%this.currentNode == 0)
        %this.moveToNode(%this.path.getCount() - 1);
      else
        %this.moveToNode(%this.currentNode - 1);
    }
  }
}
// AFX >>

function AIPlayer::moveToNode(%this,%index)
{
   // Move to the given path node index
   %this.currentNode = %index;
   %node = %this.path.getObject(%index);
   %this.setMoveDestination(%node.getTransform(), %index == %this.targetNode);
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

function AIPlayer::pushTask(%this,%method)
{
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
   %this.taskIndex = 0;
   %this.taskCurrent = -1;
}

function AIPlayer::nextTask(%this)
{
   if (%this.taskCurrent != -1)
      if (%this.taskCurrent < %this.taskIndex - 1)
         %this.executeTask(%this.taskCurrent++);
      else
         %this.taskCurrent = -1;
}

function AIPlayer::executeTask(%this,%index)
{
   %this.taskCurrent = %index;
   eval(%this.getId() @ "." @ %this.task[%index] @ ";");
}


//-----------------------------------------------------------------------------

function AIPlayer::singleShot(%this)
{
   // The shooting delay is used to pulse the trigger
   %this.setImageTrigger(0,true);
   %this.setImageTrigger(0,false);
   %this.trigger = %this.schedule(%this.shootingDelay,singleShot);
}


//-----------------------------------------------------------------------------

function AIPlayer::wait(%this,%time)
{
   %this.schedule(%time * 1000,"nextTask");
}

function AIPlayer::done(%this,%time)
{
   %this.schedule(0,"delete");
}

function AIPlayer::fire(%this,%bool)
{
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
   echo("Aim: " @ %object);
   %this.setAimObject(%object);
   %this.nextTask();
}

function AIPlayer::animate(%this,%seq)
{
   //%this.stopThread(0);
   //%this.playThread(0,%seq);
   %this.setActionThread(%seq);
}


//-----------------------------------------------------------------------------

// AFX <<
//  The AIManager has been greatly modified for the ArcaneFX DEMO
//  in a way that's fairly hardwired according to the specific needs
//  of this demo.
//  It now manages two different AI orcs, one that starts out dead, and
//  another that patrols around the cluster of buildings.
//  It also now handles corpse resurrection and destruction.
function startAIManager()
{
  new ScriptObject(AIManager) {
    corpse_waits = -1;
  };
  MissionCleanup.add(AIManager);
  AIManager.init();
}

function stopAIManager()
{
  AIManager.delete();
}

function AIManager::init(%this)
{
  %this.spawn_ai_01(true);
  %this.spawn_ai_02(true);
}

function AIManager::spawn_ai_01(%this, %first, %xfm)
{
  %resurrect = (%xfm !$= "");
  %path = "MissionGroup/Paths/Path1";
  if (!%resurrect)
  {
    if (%first)
      %xfm = %path.getObject(0).getTransform();
    else
      %xfm = %path.getObject(getRandom(%path.getCount()-1)).getTransform();
    %this.ai_player_01 = %this.spawn("dead orc", %xfm, %path, true, false);
    %this.ai_player_01.startFade(1000, 4000, false);
  }
  else
  {
    %this.ai_player_01 = %this.spawn("undead orc", %xfm, %path, false, false);
    %this.ai_player_01.startFade(1000, 0, false);
  }
}

function AIManager::spawn_ai_02(%this, %first, %xfm)
{
  %resurrect = (%xfm !$= "");
  %path = "MissionGroup/Paths/RouteNPC01";
  if (!%resurrect)
  {
    if (%first)
      %xfm = %path.getObject(0).getTransform();
    else
      %xfm = %path.getObject(getRandom(%path.getCount()-1)).getTransform();
    %this.ai_player_02 = %this.spawn("orc", %xfm, %path, false, true);
    %this.ai_player_02.startFade(1000, 0, false);
  }
  else
  {
    %this.ai_player_02 = %this.spawn("orc", %xfm, %path, false, true);
    %this.ai_player_02.startFade(1000, 0, false);
  }
}

function AIManager::spawn(%this, %name, %xfm, %path, %dead, %equip)
{
  %player = AIPlayer::spawnAt(%name, %xfm, %dead);
  %player.pathTraversal = "randomized";
  %player.followPath(%path, -1);
  
  if (%equip)
  {
    %player.mountImage(CrossbowImage,0);
    %player.setInventory(CrossbowAmmo,1000);
  }
  
  return %player;
}

function AIManager::burnCorpse(%this, %corpse)
{
  if (%corpse == %this.ai_player_01)
  {
    %corpse.schedule(0, "startFade", 1000, 0, true);
    %corpse.schedule(1000+1000, "delete");
    %this.schedule(getRandom(5000, 6000), "spawn_ai_01", false);
  }
  else if (%corpse == %this.ai_player_02)
  {
    %corpse.schedule(0, "startFade", 1000, 0, true);
    %corpse.schedule(1000+1000, "delete");
    %this.schedule(getRandom(5000, 6000), "spawn_ai_02", false);
  }
  else // must be a player's corpse
  {
    %corpse.schedule(0, "startFade", 1000, 0, true);
    %corpse.schedule(1000+1000, "delete");
    // player is still attached to corpse, so spawn a new one
    if (%corpse.client)
      %corpse.client.schedule(2000, "spawnPlayer", false);
  }
}

function AIManager::resurrect(%this, %corpse)
{
  if (%corpse == %this.ai_player_01)
  {
    %corpse.schedule(0, "startFade", 500, 0, true);
    %corpse.schedule(500, "delete");
    %this.spawn_ai_01(false, %corpse.getTransform());
  }
  else if (%corpse == %this.ai_player_02)
  {
    %corpse.schedule(0, "startFade", 500, 0, true);
    %corpse.schedule(500, "delete");
    %this.spawn_ai_02(false, %corpse.getTransform());
  }
  else // must be a player's corpse
  {
    %corpse.schedule(0, "startFade", 500, 0, true);
    %corpse.schedule(500, "delete");
    // player is still attached to corpse, so spawn a new one
    if (%corpse.client)
      %corpse.client.schedule(250, "spawnPlayer", false, %corpse.getTransform());
  }
}

function DemoPlayer::onRemove(%this, %player)
{
  if (isFunction(unequipSciFiOrc))
    unequipSciFiOrc(%player);
  if (isObject(%player.lingering_spell))
    %player.lingering_spell.interrupt();
  Parent::onRemove(%this, %player);
}

// AFX >>


