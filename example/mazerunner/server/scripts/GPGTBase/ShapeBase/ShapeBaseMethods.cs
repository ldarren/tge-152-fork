//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// ShapeBase Object Console Methods
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//				Damage Methods
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
function ShapeBase::damage(%this, %sourceObject, %position, %damage, %damageType)
{
   // All damage applied by one object to another should go through this
   // method. This function is provided to allow objects some chance of
   // overriding or processing damage values and types.  As opposed to
   // having weapons call ShapeBase::applyDamage directly.
   // Damage is redirected to the datablock, this is standard proceedure
   // for many built in callbacks.
   %this.getDataBlock().damage(%this, %sourceObject, %position, %damage, %damageType);
}


//-----------------------------------------------------------------------------

function ShapeBase::setDamageDt(%this, %damageAmount, %damageType)
{
   // This function is used to apply damage over time.  The damage
   // is applied at a fixed rate (50 ms).  Damage could be applied
   // over time using the built in ShapBase C++ repair functions
   // (using a neg. repair), but this has the advantage of going
   // through the normal script channels.
   if (%this.getState() !$= "Dead") {
      %this.damage(0, "0 0 0", %damageAmount, %damageType);
      %this.damageSchedule = %this.schedule(50, "setDamageDt", %damageAmount, %damageType);
   }
   else
      %this.damageSchedule = "";
}

function ShapeBase::clearDamageDt(%this)
{
   if (%this.damageSchedule !$= "") {
      cancel(%this.damageSchedule);
      %this.damageSchedule = "";
   }
}

return;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//				Inventory Callbacks
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// ******************************************************************
//			onInventory() - Not Used
// ******************************************************************
function ShapeBase::onInventory( %ownerObj , %pickupDB, %amount )
{
	//echo("ShapeBase::onInventory( " @ %ownerObj @ " , " @ %pickupDB @ " , " @ %amount @ " )");

	// Does nothing, so just return success
	return true;
}

