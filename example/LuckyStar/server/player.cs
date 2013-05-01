//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Load dts shapes and merge animations
exec("~/data/shapes/players/DancingGirl/player.cs");

datablock PlayerData(PlayerBody)
{
	renderFirstPerson = false;
	shapeFile = "~/data/shapes/players/DancingGirl/dancinggirl.dts";
	
	maxForwardSpeed = 1;
	maxBackwardSpeed = 0.5;
	maxSideSpeed = 0.75;
	
	jumpForce = 15.0;
};

datablock StaticShapeData (PlayerShapeData)
{
	shapeFile = "~/data/shapes/players/DancingGirl/dancinggirl.dts";
};

//----------------------------------------------------------------------------
// PlayerBody Datablock methods
//----------------------------------------------------------------------------

function PlayerBody::onAdd(%this,%obj)
{
   // Called when the PlayerData datablock is first 'read' by the engine (executable)
}

function PlayerBody::onRemove(%this, %obj)
{
   if (%obj.client.player == %obj)
      %obj.client.player = 0;
}

function PlayerBody::onNewDataBlock(%this,%obj)
{
   // Called when this PlayerData datablock is assigned to an object
}

function PlayerBody::onEndSequence(%this, %obj, %thread)
{
	%client = %obj.client;
	if (%client $= "") return;
	
	if (LuckyStarServer.cbActivated[%thread] == -1) return; // return if starttime or delay mode
	LuckyStarServer.cbActivated[%thread] += 10; // prevent endless recursion and attached a 100 tag to it

	if (LuckyStarServer.cbActivated[%thread] < ClientGroup.getCount()*10) return; // make sure all are ready
		
	LuckyStarServer.cbActivated[%thread] = -1; // prevent endless callback

	LuckyStarServer.replayDance(
		%thread, 
		LuckyStarServer.cbNextCount[%thread], 
		LuckyStarServer.cbMoveID[%thread], 
		LuckyStarServer.cbSpeed[%thread], 
		LuckyStarServer.cbDirection[%thread], 
		LuckyStarServer.cbSeqMode[%thread]);

	//echo("####### Thread "@%thread@" sequence end");
}

function PlayerBody::animationDone(%this, %obj)
{
	%cl = %obj.client;
	if (%cl $= "" || !%cl.badMood) return;
	
	%obj.setActionThread("fail", false, true, true, true); // modified version, last parameter is wait, to get animationDone
	
	//error("Action Thread Ended");
}