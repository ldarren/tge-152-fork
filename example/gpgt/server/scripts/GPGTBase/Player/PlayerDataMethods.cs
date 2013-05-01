//-------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//				PlayerData::create()
//-------------------------------------------------------------------------
//
// This method is required by the Mission Creator in order to create an 
// object.  All ShapeBase derived classes require one if you wish to 
// be able to place them with the Mission Creator.
// 
// Objects placed by scripts and by the mission loader do not need this.
//
// Note: It is currently NOT POSSIBLE to make an object specific version of
// create().  i.e. Making a MyVersion::create() will not override the parent.
//
//-------------------------------------------------------------------------
function PlayerData::create(%DB)
{
   echo("PlayerData::create("@%DB@")");
   %Obj = new Player() 
   {
      dataBlock	= %DB;
   };
   return %Obj;
}

// ******************************************************************
//                   PlayerData::onAdd()
// ******************************************************************
// 1. Call onAdd() in parent first to get benefit of code specified there
// 2. Allow players to mount vehicles by default.
// 3. Give all players and inventory by default
// ******************************************************************
function PlayerData::onAdd(%DB,%Obj)
{
   //%callerDBName = %DB.getName();
   //echo("PlayerData::onAdd( " @%callerDBName @ " , " @ %Obj @ " )");

   // 1
   Parent::onAdd(%DB,%Obj);

   // 2
   %Obj.enableMountVehicle = true;

   // 3.
   %Obj.myInventory = newSimpleInventory();
   %Obj.myInventory.setOwner(%Obj); 

}

// ******************************************************************
//                   PlayerData::onRemove()
// ******************************************************************
// 1. Destroy the players inventory
// 2. Call into the parent onRemove()
// ******************************************************************
function PlayerData::onRemove(%DB,%Obj)
{
   //%callerDBName = %DB.getName();
   //	echo("PlayerData::onRemove( " @%callerDBName @ " , " @ %Obj @ " )");

   // 1
   if( isObject( %Obj.myInventory )  ) %Obj.myInventory.delete();

   // 2
   Parent::onRemove(%DB,%Obj);
}

// ******************************************************************
//                   PlayerData::onCollision()
// ******************************************************************
// 1. Ignore collisions if this player is "Dead". i.e. a Corpse
//    Note - This will probably need to be removed for RPG style games
//           where you pick up a corpse's inventory.
// 2. Call into the parent callback allowing it to do any work it should do first.
// 3. Handle collisions with vehicles.  This is accomplished by checking 
//    whether a player has collided with a vehicle, then whether the vehicle
//    is mountable.  Then, the player is checked to see if it can mount
//    vehicles, and finally whether it is mounted already.
// 3a. In the VOL1 version of this callback, players only mount to node 0 on 
//     the vehicle.  We'll talk about how to expand this in Volume 2.
// 
// ******************************************************************
function PlayerData::onCollision( %colliderDB , %colliderObj , %collidedObj, %vec, %speed)
{
   //%callerDBName = %colliderDB.getName();
   //echo("PlayerData::onCollision( " @%callerDBName @" , " @ %colliderObj @ " , " @%collidedObj @ " , \"" @%vec @ "\" , " @%speed @ " )");

   // 1
   if (%colliderObj.getState() $= "Dead") return;

   // 2
   Parent::onCollision( %colliderDB , %colliderObj , %collidedObj, %vec, %speed);

   // 3
   %isVehicle    = ( %collidedObj.getType() & $TypeMasks::VehicleObjectType ) ? 1 : 0;
   %isMountable  = %collidedObj.getDatablock().mountable;
   %canMount     = %colliderObj.enableMountVehicle; 
   %amNotMounted = ! %colliderObj.isMounted();

   //echo("\c4  %isVehicle    == ", %isVehicle);
   //echo("\c4  %isMountable  == ", %isMountable);
   //echo("\c4  %canMount     == ", %canMount);
   //echo("\c4  %amNotMounted == ", %amNotMounted);

   if ( %isVehicle && %isMountable && %canMount && %amNotMounted ) 
   {
      %collidedObj.mountObject( %colliderObj, 0 ); // 3a
   }
}

// ******************************************************************
//                   PlayerData::onMount()
// ******************************************************************
// 1. Clean the player's transform to get a nice mount.  Experiment w/ this
//    for variations on a flat mount.
// 2. Attempt to play the mounting object's mounted thread. Should be specified in the datablock
// 3. Make the vehicle the control object.  This is a Volume 2 topic.  Please see that guide for
//    more details.
// 
// ******************************************************************
function PlayerData::onMount( %mounterDB , %mounterObj , %mountToObject , %mountNode)
{
   //%callerDBName = %mounterDB.getName();
   //echo("PlayerData::onMount( " @%callerDBName @" , " @ %mounterObj @ " , " @%mountToObject @ " , " @ %mountNode @ " )");

   if (%mountNode == 0) // Only handles mounting to node 0 - See onCollision() above.
   {
      // 1
      %mounterObj.setTransform("0 0 0 0 0 1 0"); 

      //2
      %mounterObj.setActionThread(%mounterObj.getDatablock().mountPose[%mountNode],true,true);

      //3
      %mounterObj.setControlObject(%mountToObject);
   }
}

// ******************************************************************
//                   PlayerData::onUnmount()
// ******************************************************************
// 1. Assume that the player is dismounting a shape that was until
//    now the control object.  Again, this is a Volume 2 topic.  Please
//    refer to that guide for more details.
// 
// ******************************************************************
function PlayerData::onUnmount( %DB, %Obj, %mount, %node )
{
   // 1
   %Obj.setControlObject( %Obj );
}

// ******************************************************************
//                   PlayerData::onAdd()
// ******************************************************************
//
// The doDismount() console method is called by TGE when the jump trigger
// is pressed AND the player is mounted.
//
// This will dismount the player from any node it is mounted to.
// 
// Warning! - This method must be ammended to fit your needs.  The difficulty
// in making this routine is in creating an algorithm to find a legal dismount
// point, where 'legal' is defined as not having the player appear inside the 
// geometry of the object it is mounted to while dismounting.
// For non-trivial mounting positions, the current method can fail.
//
// ******************************************************************
// 0. Ignore this call if the player is not mounted.
// 
// Otherwise, Dismount the player by:
// 
// 1. Determining the player's current position.
// 2. Seek a dismount position where the player won't be embedded in the 
//    object it is mounted to. (iterative)
// 3. Temporarily disable mounting.
// 4. Unmounting the player from the current mount node and reasigning control 
//    to the player.
// 5. Giving the player a small ammount of kick to move it away from the 
//    dismount point.
// 
// ******************************************************************
function PlayerData::doDismount( %DB , %Obj , %forced )
// %forced	- Force dismount to be done in current player's position (no offset)
{
   // 0
   if ( !%Obj.isMounted() ) return; 

   // 1
   %pos    = getWords(%Obj.getTransform(), 0, 2);
   %oldPos = %pos;

   // 2
   %vec[0] = " 0  0  1";
   %vec[1] = " 0  0  1";
   %vec[2] = " 0  0 -1";
   %vec[3] = " 1  0  0";
   %vec[4] = "-1  0  0";
   %impulseVec  = "0 0 0";
   %vec[0] = MatrixMulVector( %Obj.getTransform(), %vec[0]);

   // Make sure the point is valid
   %pos = "0 0 0";
   %numAttempts = 5;
   %success     = -1;
   for ( %i = 0 ; %i < %numAttempts ; %i++ ) 
   {
      %pos = VectorAdd( %oldPos , VectorScale( %vec[%i] , 3 ) );
      if ( %Obj.checkDismountPoint( %oldPos , %pos ) ) {
         %success = %i;
         %impulseVec = %vec[%i];
         break;
      }
   }

   // Note: This code is a last ditch effort in the case of
   // failure.  We will just 'try' to dismount from the original 
   // player position and hope for the best.
   if ( %forced && %success == -1 ) %pos = %oldPos;

   // 3
   %Obj.enableMountVehicle = false;
   %Obj.schedule( 4000 , "enableMountVehicles"  );

   // 4
   %Obj.unMount();

   // 5
   %Obj.setTransform( %pos );
   %Obj.applyImpulse( %pos , VectorScale( %impulseVec , %Obj.getDataBlock().mass ) );
}

// ******************************************************************
//                   PlayerData::onTrigger()
// ******************************************************************
//
// This callback is called every time the player receives one of the 6
// predefined trigger move events. i.e. $mvTriggerCount0, etc.
// It can be used for any numer of things, but is now only provided for 
// debug purposes.
//
// ******************************************************************
function PlayerData::onTrigger( %DB , %Obj , %triggerNum, %triggerVal)
{
   //echo("\c3 PlayerData::onTrigger( ", %DB , " , " , %Obj , " , " , %triggerNum, " , " , %triggerVal, " )");
}
