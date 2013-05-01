//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Load dts shapes and merge animations
datablock TSShapeConstructor(PlayerDts)
{
   baseShape = "~/data/shapes/player/player.dts";
   sequence0 = "~/data/shapes/player/player_root.dsq root";
   sequence1 = "~/data/shapes/player/player_forward.dsq run";
   sequence2 = "~/data/shapes/player/player_back.dsq back";
   sequence3 = "~/data/shapes/player/player_side.dsq side";
   sequence4 = "~/data/shapes/player/player_fall.dsq fall";
   sequence5 = "~/data/shapes/player/player_land.dsq land";
   sequence6 = "~/data/shapes/player/player_jump.dsq jump";
   sequence7 = "~/data/shapes/player/player_standjump.dsq standjump";
   sequence8 = "~/data/shapes/player/player_lookde.dsq look";
   sequence9 = "~/data/shapes/player/player_head.dsq head";
   sequence10 = "~/data/shapes/player/player_headside.dsq headside";
   sequence11 = "~/data/shapes/player/player_celwave.dsq celwave";
};

datablock PlayerData(PlayerBody)
{
   renderFirstPerson = false;
   shapeFile = "~/data/shapes/player/player.dts";
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
