//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Load dts shapes and merge animations
datablock TSShapeConstructor(PlayerDts2)
{
   baseShape = "~/data/shapes/tank02/player2.dts";
   sequence0 = "~/data/shapes/tank02/Root2.dsq root";
	sequence1 = "~/data/shapes/tank02/recoilde2.dsq light_recoil";
  // sequence1 = "~/data/shapes/player/player_forward.dsq run";
  // sequence2 = "~/data/shapes/player/player_back.dsq back";
  // sequence3 = "~/data/shapes/player/player_side.dsq side";
  // sequence4 = "~/data/shapes/player/player_fall.dsq fall";
  // sequence5 = "~/data/shapes/player/player_land.dsq land";
  // sequence6 = "~/data/shapes/player/player_jump.dsq jump";
  // sequence7 = "~/data/shapes/player/player_standjump.dsq standjump";
  // sequence8 = "~/data/shapes/player/player_lookde.dsq look";
  // sequence9 = "~/data/shapes/player/player_head.dsq head";
  // sequence10 = "~/data/shapes/player/player_headside.dsq headside";
  // sequence11 = "~/data/shapes/player/player_celwave.dsq celwave";
};

datablock PlayerData(PlayerShape2 : PlayerShape)
{
   renderFirstPerson = false;
   shapeFile = "~/data/shapes/tank02/player2.dts";
   computeCRC = true;
	
	minLookAngle = 0.0;		// EDIT: Shoot Angle
   maxLookAngle = 0.0;		// EDIT: Shoot Angle
   maxFreelookAngle = 3.0;
	
	maxDamage = 10;
	
	boundingBox = "5.0 5.0 4.0";
};


//----------------------------------------------------------------------------
// PlayerShape Datablock methods
//----------------------------------------------------------------------------

function PlayerShape2::onAdd(%this,%obj)
{
   // Called when the PlayerData datablock is first 'read' by the engine (executable)
   //parent::onAdd( %this, %obj );
   %obj.mountImage( MoogunImage, 0 );
   %obj.setImageAmmo( 0, 1 );
}

function PlayerShape2::onRemove(%this, %obj)
{
   if (%obj.client.player == %obj)
      %obj.client.player = 0;
}

function PlayerShape2::onNewDataBlock(%this,%obj)
{
   // Called when this PlayerData datablock is assigned to an object
}