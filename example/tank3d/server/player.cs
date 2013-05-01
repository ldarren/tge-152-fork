//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Load dts shapes and merge animations
datablock TSShapeConstructor(PlayerDts)
{
   baseShape = "~/data/shapes/player/player.dts";
  // sequence0 = "~/data/shapes/tank01/Root1.dsq root";
  // sequence1 = "~/data/shapes/tank01/recoilde1.dsq light_recoil";
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

datablock PlayerData(PlayerShape)
{
   renderFirstPerson = false;
   shapeFile = "~/data/shapes/player/player.dts";
   computeCRC = true;
};


//----------------------------------------------------------------------------
// PlayerShape Datablock methods
//----------------------------------------------------------------------------

function PlayerShape::onAdd(%this,%obj)
{
   // Called when the PlayerData datablock is first 'read' by the engine (executable)
   parent::onAdd( %this, %obj );
}

function PlayerShape::onRemove(%this, %obj)
{
   if (%obj.client.player == %obj)
      %obj.client.player = 0;
}

function PlayerShape::onNewDataBlock(%this,%obj)
{
   // Called when this PlayerData datablock is assigned to an object
}

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------

function Player::blowup(%this)
{
	%pos = %this.getPosition();

	%x = new explosion() {
		dataBlock = "tankExplosion";
		position = %pos;
	};
	MissionCleanup.add(%x);
}

function Player::damage(%this, %obj, %sourceObject, %position, %damage, %damageType)
{
   %obj.applyDamage(%damage);
	
	%hpnumber = getPhoneNumber(%obj);
	%SpawnPoint = getPlayerSpawnPoint(%hpnumber);
	clearPlayerSpawnPoint(%hpnumber);
	clearSpawnPoint(%SpawnPoint);
	
	%shooterNum = getPhoneNumber(%sourceObject);
	addScore(%shooterNum);
	
	guiMsgIsHit(getPlayerName(%shooterNum), getPlayerName(%hpnumber));
	
	schedule(2000, 0, cameraManager, %obj.client, $camOrbitMap, %sourceObject); // Directly override camera locking mechanism
	
	%obj.blowup();
	%obj.schedule(10, "delete");
}

function Player::kill(%this, %damageType, %SourceObject)
{
   %this.damage(%this, %SourceObject, %this.getPosition(), 1000, %damageType);
}
