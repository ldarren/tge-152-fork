//-----------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//				Creation/Destruction Callbacks
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-------------------------------------------------------------------------
//				ItemData::create()
//-------------------------------------------------------------------------
//
// This method is required by the Mission Creator in order to create an 
// object.  All ShapeBase derived classes require one if you wish to 
// be able to place them with the Mission Creator.
// 
// Objects placed by scripts and by the mission loader do not need this.
//
// Note: It is currently NOT POSSIBLE to make an object specific version of
// create().  i.e. Making a BaseItem::create() will not override the parent.
//
//-------------------------------------------------------------------------
function ItemData::create(%DB)
{
   //echo("ItemData::create("@%DB@")");
   %obj = new Item() 
   {
      dataBlock	= %DB;
      collideable = false; // Legacy, not used
      static		= false; 
      rotate		= false;
   };
   return %obj;
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
// - Pickup objects' datablocks may contain the following fields to modify pickups:
// - InventoryItem  - Store this Datablock name instead
// - InventoryValue - Increment inventory by this ammount (default is 1)
// 
// It is this method's job to put the %pickupDB into the inventory.
//

function ItemData::onPickup( %pickupDB , %pickupObj , %ownerObj )
{
   %callerDBName = %pickupDB.getName();
   echo("ItemData::onPickup( " @%callerDBName @" , " @ %pickupObj @ " , " @%ownerObj @ " )");

   %actualPickupDB = %pickupDB;
   %pickupCount    = 1;

   if( "" !$= %pickupDB.inventoryItem ) {
      %actualPickupDB = %pickupDB.inventoryItem;
   }

   if( "" !$= %pickupDB.inventoryValue ) {
      %pickupCount = %pickupDB.inventoryValue;
   }

   if ( %ownerObj.myInventory.addObject( %actualPickupDB , %pickupCount ) ) 
   {
      if ( (%pickupDB.respawn == true ) ) 
      {
         %pickupObj.respawn();
         return true;
      }
      else 
      {
         %pickupObj.delete();
         return true;
      }
   }

   return false;
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
   %callerDBName = %inventoryDB.getName();
   //echo("ItemData::onInventory( " @%callerDBName @ " , " @ %ownerObj @ " , " @ %amount @ " )");

   // Does nothing, so just return success
   return true;
}

