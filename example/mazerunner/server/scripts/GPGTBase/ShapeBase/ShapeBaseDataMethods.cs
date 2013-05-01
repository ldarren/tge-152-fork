//-----------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//				Creation/Destruction Callbacks
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// ******************************************************************
//                   ShapeBaseData::onAdd()
// ******************************************************************
// This callback is called for all ShapeBase objects when they are 
// called.  It is scoped to the shape's datablock.  It is called
// 'after' values from the initialization block (see 'new' in GPGT)
// are applied.
//
// 1. Set the repair rate if specified.
// 2. Set the recharge rate if specified.
// 
// ******************************************************************
function ShapeBaseData::onAdd( %DB , %Obj )
{
	//%callerDBName = %DB.getName();
	//echo("ShapeBaseData::onAdd( "@ %callerDBName @ " , " @ %Obj @ " )");

   // 1
   if( %DB.enableAutoRepair ) %Obj.setRepairRate( %DB.repairRate );

   // 2
   if( %DB.enableAutoRecharge ) %Obj.setRechargeRate( %DB.rechargeRate ); 
}

// ******************************************************************
//                   ShapeBaseData::onRemove()
// ******************************************************************
function ShapeBaseData::onRemove( %DB , %Obj )
{
	//%callerDBName = %DB.getName();
	//echo("ShapeBaseData::onRemove( "@ %callerDBName @ " , " @ %Obj @ " )");
}

// ******************************************************************
//                   ShapeBaseData::onNewDataBlock()
// ******************************************************************
function ShapeBaseData::onNewDataBlock( %DB , %Obj )
{
	//%callerDBName = %DB.getName();
	//echo("ShapeBaseData::onNewDataBlock( "@ %callerDBName @ " , " @ %Obj @ " )");
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//				Collision Callbacks
//-----------------------------------------------------------------------------
// 
// 1. If %collidedObj is an Item, try to pick it up.
//
//-----------------------------------------------------------------------------

// ******************************************************************
//                   ShapeBaseData::onCollision()
// ******************************************************************
function ShapeBaseData::onCollision( %colliderDB , %colliderObj , %collidedObj, %vec, %speed)
{
	//%callerDBName = %colliderDB.getName();
	//echo("ShapeBaseData::onCollision( " @%callerDBName @" , " @ %colliderObj @ " , " @%collidedObj @ " , \"" @%vec @ "\" , " @%speed @ " )");

   // 1
	if( "Item" $= %collidedObj.getClassName() )
	{
		%colliderDB.doPickup( %colliderObj, %collidedObj );
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//				Inventory Callbacks
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// ******************************************************************
//                   ShapeBaseData::doPickup()
// ******************************************************************
// 
// 1. If this %ownerObj does not have an inventory, then abort the pickup.
// 2. Do the pickup
//
// ******************************************************************
function ShapeBaseData::doPickup( %ownerDB, %ownerObj , %pickupObj )
{
	//%callerDBName = %ownerDB.getName();
	//echo("ShapeBaseData::doPickup( " @ %callerDBName @" , " @ %ownerObj @ " , " @ %pickupObj @ " )");

   // 1
	if( ! isObject(%ownerObj.myInventory))
      return false;

   // 2
   return %pickupObj.getDatablock().onPickup( %pickupObj , %ownerObj );
}

// ******************************************************************
//                   ShapeBaseData::onRemove()
// ******************************************************************
//
// 1. If this %ownerObj does not have an inventory, then abort the throw.
// 2. Use the inventory onThrow() method to extract the needed item from
//    inventory.  Abort throw if no object is found.
// 3. Use a datablock scoped method (throwObject()) to throw the returned 
//    item.
//
// ******************************************************************
function ShapeBaseData::doThrow( %ownerDB, %ownerObj , %throwDB )
{
	//%callerDBName = %ownerDB.getName();
	//echo("ShapeBaseData::doThrow( " @ %callerDBName @" , " @ %ownerObj @ " , " @ %throwDB @  " )");

	// 1
	if( ! isObject(%ownerObj.myInventory) )
      return false;

   // 2 
   if( ! ( %throwObj = %throwDB.onThrow( %ownerObj ) ) )
   {
      return false;
   }

   // 3
   %ownerDB.throwObject( %ownerObj , %throwObj );

   return true;
}


// ******************************************************************
//                   ShapeBaseData::onRemove()
// ******************************************************************
//
// Unlike the FPS kit, the GPGT kit scopes the throw method to 
// a datablock, allowing for the introduction of different throw
// behaviors based on the thrower's datablock type.
//
// -- Comments are embedded -- (see below)
//
// ******************************************************************
function ShapeBaseData::throwObject( %ownerDB , %ownerObj , %throwObj, %throwModifier )
{
	//%callerDBName = %ownerDB.getName();
	//echo("ShapeBaseData::throwObject( " @ %callerDBName @" , " @ %ownerObj @ " , " @ %throwObj @ " , " @ %throwModifier @ " )");

	// Since the object is thrown from the center of the
	// shape, the object needs to avoid colliding with it's
	// owner.
   if( %throwObj.getType() & $TypeMasks::ItemObjectType )
   {
      // causes item to not render (in buggy v_1_3, item.cc unpack_update() is wrong) 
      %throwObj.setCollisionTimeout(%ownerObj); 
   }

	// Throw the given object in the direction the shape is looking/facing.
	// The force value is hardcoded according to the current default
	// object mass and mission gravity (20m/s^2).
	%throwForce = %ownerObj.throwForce;
	if (!%throwForce) {
		%throwForce = 20;
	}

	//
	// Allow throw force to be modified (for 'throw charging')
	//
	if(%throwModifier) {
		%throwForce = %throwForce * %throwModifier;
	}


	// Because we want this force to be 'relative' lets use
	// the mass to modify our total throw force.
	%throwForce = %throwForce * %throwObj.getDatablock().mass;


	// To make the throwObject method flexible, make it throw differently, if this shape is
	// the current control object and in 1st or 3rd POV.  The rules for this throwing logic are:
	//
	// %ownerObj != ControlObject => Throw from object's center using forward vector
	// %ownerObj == ControlObject && 1st POV => Throw from object's center using eye vector
	// %ownerObj == ControlObject && 3rd POV => Throw from object's center using forward vector
	// 
	if( ($Game::ClientHandle && ( %ownerObj != $Game::ClientHandle.getControlObject() ) ) || 
	    !$firstPerson ) {
		%throwVec = %ownerObj.getForwardVector();
	} else {
		%throwVec = %ownerObj.getEyeVector();
	}

	%vec = vectorScale(%throwVec, %throwForce);
   
	// Add a vertical component to give the object a better arc
	%verticalForce = %throwForce / 6;
	%dot = vectorDot("0 0 1",%throwVec);
	if (%dot < 0) %dot = -%dot;
	%vec = vectorAdd(%vec,vectorScale("0 0 " @ %verticalForce,1 - %dot));

	// Add the shape's velocity
	%vec = vectorAdd(%vec,%ownerObj.getVelocity());

	// Set the object's position and initial velocity
	%pos = getBoxCenter(%ownerObj.getWorldBox());
	%transform = %pos @ "1 0 0 0"; 
	%throwObj.setTransform(%transform);

	%throwObj.applyImpulse(%pos,%vec);

	//echo("Initial Position == ", %throwObj.getPosition());
	//echo("Initial Transform == ", %transform);
	//echo("Throw vector      == ", %throwVec);
	//echo("Throw throwForce  == ", %throwForce);
}


// ******************************************************************
//                   ShapeBaseData::doUse()
// ******************************************************************
function ShapeBaseData::doUse( %ownerDB, %ownerObj , %useDB )
{
	//%callerDBName = %ownerDB.getName();
	//echo("ShapeBaseData::doUse( " @ %callerDBName @" , " @ %ownerObj @ " , " @ %useDB @ " )");

	// 
	// If this %ownerObj does not have an inventory, then abort the use.
	//
	if(isObject(%ownerObj.myInventory))
	{
	
		if(!%useDB.onUse( %ownerObj )) 
		{
			//echo("ShapeBaseData::doUse():: WARNING:: Did not use object " @ "\"" @ %useDB.getName()@"\"");
			return false;
		}

		return true;
	} else {
		//echo(%ownerObj @ " has no Inventory!");
	}
	return false;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//				Damage Callback  
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// Note: More damage callbacks/methods are added in Volume 2 scripts

// ******************************************************************
//                   ShapeBaseData::onDamage()
// ******************************************************************
function ShapeBaseData::onDamage( %damageDB , %damageObj , %totalDamage)
{
	//%callerDBName = %damageDB.getName();
	//echo("ShapeBaseData::onDamage( " @%callerDBName @" , " @ %damageObj @ " , " @%totalDamage @  " )");
	//echo("Total damage == ", %damageObj.getDamageLevel());
}

