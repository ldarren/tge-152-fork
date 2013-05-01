//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

datablock ItemData( BaseItem )
{
   //-------------------------------------------------------------------------
   // Parent Fields
   //-------------------------------------------------------------------------
   // From GameBaseData
   //-------------------------------------------------------------------------
   category				= "TestShapes";
   nametag				= "DefaultItem";

   //-------------------------------------------------------------------------
   // From ShapeBaseData
   //-------------------------------------------------------------------------
   //
   // We won't set all the values from ShapeBaseData.  Instead, we'll just 
   // set the few values that can be 'troublesome' if not set.
   // 
   shapeFile         = "~/data/shapes/markers/octahedron.dts";
   cloakTexture      = "~/data/shapes/markers/testcloakskin.png";
   emap              = false;

   mass              = 10.0;
   drag              = 0.0;
   density           = 0.1;
   computeCRC        = false;

   //-------------------------------------------------------------------------
   // ItemData Fields
   //-------------------------------------------------------------------------
   // dynamicType - Used to return additional type bits for getType() calls
   dynamicType			= 0;    

   // elasticity - How bouncy is this object.  A little elasticity is good, because
   // a value of zero may cause a dropped item to 'hesitate' before settling.
   // This is a calculation rounding issue I believe.
   elasticity			= 0.05;

   // friction - How much will this item 'slow' while sliding?
   // This item will arrest quickly.
   friction				= 0.7; 

   // gravityMod - Gravity affects this item normally.
   gravityMod			= 1.0;

   // lightColor - This item will emit a pure white light
   lightColor			= "1.0 1.0 1.0 1.0";

   // lightOnlyStatic - This item will only light objects/terrain/etc. when this item
   // is marked as static (in the object definition)
   lightOnlyStatic	= true;

   // lightRadius - This object will emit a light with a radius of 2 meters
   lightRadius			= 2.0;

   // lightType - This light is pulsing on-off-on-...
   lightType			= "NoLight"; // NoLight, PulsingLight, ConstantLight

   // lightTime - This 'pulsing' light will go from ON to OFF to ON in 3 seconds.
   // Only meaningful for pulsing lights
   lightTime			= 3000;

   // maxVelocity - This shape is velocity 'limited' to 10 meters per second
   maxVelocity			= 10.0;

   // pickupName - This (dynamic) field provides a message name for this shape when it
   // is picked up.
   pickupName			= "Default Item";

   // sticky - This item will not stick to the terrain or interiors when it hits. Instead,
   // it will slide.
   sticky				= false;

	respawn = false;
	inventoryItem = "unknown";
	
};

function ItemData::create(%data)
{
	// The mission editor invokes this method when it wants to create
	// an object of the given datablock type.
	%obj = new Item() {
		dataBlock = %data;
			inventoryValue = 1;	// dynamic field
	};
	return %obj;
}

function ItemData::onAdd(%db, %obj)
{
	//Parent::onAdd(%db, %obj);
	%obj.rotate = true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//				Inventory Callbacks
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// ******************************************************************
//					onPickup() 
// ******************************************************************
//
// The job of this callback is to:
//
// 1. Place this item in the owner's inventory.
// 
// Assumptions:
//
// - Owner has inventory.
// - Inventory field name is: myInventory
// 
// Objective: It is this method's job to put the %pickupDB into the inventory.
//

function ItemData::onPickup( %this , %obj , %player )
{
	echo("ItemData::onPickup( " @%this.getName() @" , " @ %obj @ " , " @%player @ " )");
	
	%pickupDB = %this;
	%pickupCount = 1;
	
	if( "" !$= %this.inventoryItem ) %pickupDB = %this.inventoryItem;
	
	if( "" !$= %obj.inventoryValue ) %pickupCount = %obj.inventoryValue;
	
	if ( %player.myInventory.addObject( %pickupDB , %pickupCount ) ) 
	{
		if ( %this.respawn ) %obj.respawn();
		else %obj.delete();
		return  %pickupDB;
	}
	
	return "";
}

// ******************************************************************
//					onThrow() 
// ******************************************************************
//
// The job of this callback is to:
// 
// 1. Remove one instance of this object from the inventory 
// 2. Re-create the object (build a new one).
// 3. Schedule a Pop().  Because we don't want items accumulting
//    in the game world, we will optionally schedule 'thrown'
//    items to Pop.  When an item is 'popped' it is removed from
//    the world.  (See schedulePop() console method below.)
// 4. Return the object to whatever object called this.
// 
// Assumptions:
//
// - Owner has inventory.
// - Inventory field name is: myInventory
//
// Notes:
//
// - It is up to the caller to decide what to do with this object.
//   i.e. the item doesn't 'self-throw'.
//

function ItemData::onThrow( %throwDB , %ownerObj )
{
   %callerDBName = %throwDB.getName();
   //echo("ItemData::onThrow( " @%callerDBName @" , " @ %ownerObj @ " )");
   // 1. Remove one of these items from the inventory.
   //    If none exists, just return 0.
   //
   if(0 == %ownerObj.myInventory.getInventoryCount( %throwDB )) return 0;
   %ownerObj.myInventory.removeObject( %throwDB );

   // 2. Create a new instance of this object.
   //
   //
   %newObject = new Item() {
      position	= %ownerObj.getWorldBoxCenter();
      datablock	= %throwDB;
      collideable	= true; 
      static		= false; // Can't throw a static.  
      rotate		= true;
   };


   if (%throwDB.respawn == true )
   {
      %newObject.schedulePop();
   }

   return %newObject;
}

// ******************************************************************
//					onUse() 
// ******************************************************************
//
// The job of this callback is to:
//
// 1. Remove one instance of this object from the inventory 
// 2. Do 'something' with it.
// 3. Return an indication that something was or was not done.
// 
// Assumptions:
//
// - Owner has inventory.
// - Inventory field name is: myInventory
// 
// Note: 
//
// - This may seem like a rather arbitrary function, but in a
//   sense, it has to be as inventory items can have any number
//   of purposes. The aim 'here and now' is to provide an example
//   of a way of implementing an onUse flow.  
//
// - I'm of the opinion that responsibilty for 'doing something' should
//   rest with the item, not the owner, but you are free to design
//   your game as you please.
//

function ItemData::onUse( %useDB , %ownerObj )
{
   %callerDBName = %useDB.getName();
   //echo("ItemData::onUse( " @%callerDBName @" , " @ %ownerObj @ " )");
   // 1. Remove one of these items from the inventory.
   //    If none exists, just return 0.
   //
   if(0 == %ownerObj.myInventory.getInventoryCount( %useDB )) {
      echo("ItemData::onUse() -> We ain't got no stinking \"" @ %callerDBName @ "\" in da inventory ... returning false.");
      return 0;
   }
   %ownerObj.myInventory.removeObject( %useDB );

   // 2. Do something with it.  We're not doing anything so we will 
   //    return false. i.e. onUse() failed!
   //
   echo("ItemData::onUse() -> Does nothing.  You must override this... Returning false.");
   return false;
}


// ******************************************************************
//			onInventory() 
// ******************************************************************
function ItemData::onInventory( %inventoryDB , %ownerObj, %amount )
{
   //%callerDBName = %inventoryDB.getName();
   //echo("ItemData::onInventory( " @%callerDBName @ " , " @ %ownerObj @ " , " @ %amount @ " )");

   // Does nothing, so just return success
   return true;
}
