//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

function loadItem(%source, %index, %transform)
{
	%indexInfo = getxmldata(%source, %index, 0);
	if (%indexInfo $= "") return;
	
	%name = getxmldata(%indexInfo, "name", 0);
	%shape = getxmldata(%indexInfo, "shape", 0);
	%value = getxmldata(%indexInfo, "value", 0);
	%scale = getxmldata(%indexInfo, "scale", 0);
	
	//echo("######################### "@%name@" ## "@%avatar@" ## "@%speech);
	
	%item = new Item(%name) 
	{
		dataBlock = %shape;
			inventoryValue = %value;
	};
	
	MissionCleanup.add( %item );
	
	%item.setTransform(%transform);
	if (%scale !$= "") %item.setScale(%scale);
	%item.setShapeName(%name);
}

function getItemSpawnGroup()
{
	%groupName = "MissionGroup/ItemSpawnPoints";
	return nameToID(%groupName);
}

function pickItemSpawnPoint() 
{
   %group = getNPCSpawnGroup();

   if (%group != -1) {
      %count = %group.getCount();
      if (%count != 0) {
         %index = getRandom(%count-1);
         %spawn = %group.getObject(%index);
         return %spawn.getTransform();
      }
      else
         error("No spawn points found in " @ %groupName);
   }
   else
      error("Missing spawn points group " @ %groupName);

   // Could be no spawn points, in which case we'll stick the
   // player at the center of the world.
   return "0 0 300 1 0 0 0";
}

function spawnItems(%config)
{
	%source = loadXMLtoString(%config);
	%group = getItemSpawnGroup();
	if (%group == -1) return;
	
	%count = %group.getCount();
	
	for (%i = 0; %i < %count; %i++)
	{
		%spawn = %group.getObject(%i);
		loadItem(%source,%i+1,%spawn.getTransform());
	}
}

//-----------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//				Inventory Console Methods
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
$Item::RespawnTime = 5000;
$Item::PopTime     = 5000;

$Item::FadeInTime  = 1000;
$Item::FadeOutTime = 1000;

// ******************************************************************
//					respawn()
// ******************************************************************
//
// For the lesson(s) we want a way for the objects that get
// picked up to be re-spawned so we can pick up more. The simplest way
// to accomplish this is to provide a method that will 'respawn' a 
// the object after some delay.  Contrary to the name, the object is
// only being hidden for a time.  No new objects are created.
//
// Note: This is entirely optional.  Depending on your game type, you
//       may choose to allow items to accumulate.  For example, if a game
//       has a fixed inventory system (zero-growth), then items should neither
//       be respawned nor popped (see below).
//

function Item::respawn( %Item )
{
   //echo("ItemData::respawn( " @ %Item @ " )");
   // 1. Fade it out and hide it.
   //
   // Note: When a item is hidden, it is temporarily disabled and not
   // rendered.
   //
   %Item.startFade(0, 0, true); // Instantly fade
   %Item.setHidden(true);

   // 2. Un-hide and fade into view 
   %Item.schedule($Item::RespawnTime, "setHidden", false);
   %Item.schedule($Item::RespawnTime, "startFade", $Item::FadeInTime , 0, false);
}   


// ******************************************************************
//					schedulePop()
// ******************************************************************
//
// We don't want objects accumulating in the world so we use this 
// method to cause an object to be automatically deleted some time 
// after being 'thrown'.
//
// Note: This is entirely optional.  Depending on your game type, you
//       may choose to allow items to accumulate.  For example, if a game
//       has a fixed inventory system (zero-growth), then items should neither
//       be respawned (see above) nor popped.
//

function Item::schedulePop( %Item )
{
   //echo("ItemData::schedulePop( " @ %Item @ " )");

   // 1. Fade out of view
   %Item.schedule($Item::PopTime , "startFade", $Item::FadeOutTime , 0, true);

   // 2. Self-delete
   %Item.schedule( $Item::PopTime + $Item::FadeOutTime , "delete");
}
