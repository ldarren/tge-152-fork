// ============================================================
// Project            :  VR20
// File               :  .\LTA\server\scripts\player\PlayerDataMethods.cs
// Copyright          :  
// Author             :  Darren
// Created on         :  Monday, July 16, 2007 5:18 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

//----------------------------------------------------------------------------
// Actor (PlayerData) Datablock methods
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

function Actor::onAdd(%this,%obj)
{
//	Parent::onAdd(%this,%obj);
	
	// Vehicle timeout
	%obj.mountVehicle = true;
	
	// Default dynamic Actor stats
	%obj.setRechargeRate(%this.rechargeRate);
	%obj.setRepairRate(0);
	
	// Simple Inventory System
	%obj.myInventory = newSimpleInventory();
	%obj.myInventory.setOwner(%obj); 
	%obj.myInventory.onLoading();
}

function Actor::onRemove(%this, %obj)
{
	if( isObject( %Obj.myInventory )  ) %Obj.myInventory.delete();
	if (%obj.client.player == %obj)	%obj.client.player = 0;
	
	Parent::onRemove(%DB,%Obj);
}

function Actor::onNewDataBlock(%this,%obj)
{
}


//----------------------------------------------------------------------------

function Actor::onMount(%this,%obj,%vehicle,%node)
{
   if (%node == 0)  {
      %obj.setTransform("0 0 0 0 0 1 0");
      %obj.setActionThread(%vehicle.getDatablock().mountPose[%node],true,true);
      %obj.lastWeapon = %obj.getMountedImage($WeaponSlot);

      %obj.unmountImage($WeaponSlot);
      %obj.setControlObject(%vehicle);
   }
}

function Actor::onUnmount( %this, %obj, %vehicle, %node )
{
   if (%node == 0)
      %obj.mountImage(%obj.lastWeapon, $WeaponSlot);
}

function Actor::doDismount(%this, %obj, %forced)
{
   // This function is called by player.cc when the jump trigger
   // is true while mounted
   if (!%obj.isMounted())
      return;

   // Position above dismount point
   %pos    = getWords(%obj.getTransform(), 0, 2);
   %oldPos = %pos;
   %vec[0] = " 0  0  1";
   %vec[1] = " 0  0  1";
   %vec[2] = " 0  0 -1";
   %vec[3] = " 1  0  0";
   %vec[4] = "-1  0  0";
   %impulseVec  = "0 0 0";
   %vec[0] = MatrixMulVector( %obj.getTransform(), %vec[0]);

   // Make sure the point is valid
   %pos = "0 0 0";
   %numAttempts = 5;
   %success     = -1;
   for (%i = 0; %i < %numAttempts; %i++) {
      %pos = VectorAdd(%oldPos, VectorScale(%vec[%i], 3));
      if (%obj.checkDismountPoint(%oldPos, %pos)) {
         %success = %i;
         %impulseVec = %vec[%i];
         break;
      }
   }
   if (%forced && %success == -1)
      %pos = %oldPos;

   %obj.mountVehicle = false;
   %obj.schedule(4000, "mountVehicles", true);
   
   // Unmount from node, and give player control.
   %obj.unMount();
   %this.onUnMount(%obj);
   %obj.setControlObject(%obj);   

   // Position above dismount point
   %obj.setTransform(%pos);
   %obj.applyImpulse(%pos, VectorScale(%impulseVec, %obj.getDataBlock().mass));
}


//----------------------------------------------------------------------------

function Actor::onCollision(%this,%obj,%col,%dir,%speed)
{
	if (%obj.getState() $= "Dead") return;
	
	if( "Item" $= %col.getClassName() )	%this.doPickup( %obj,%col );

	// Mount vehicles
	if (%col.getDataBlock().className $= WheeledVehicleData && %obj.mountVehicle &&
		%obj.getState() $= "Move" && %col.mountable) 
	{
		// Only mount drivers for now.
		%node = 0;
		%col.mountObject(%obj,%node);
		%obj.mVehicle = %col;
	}
}

function Actor::onImpact(%this, %obj, %collidedObject, %vec, %vecLen)
{
//   %obj.damage(0, VectorAdd(%obj.getPosition(),%vec),
//      %vecLen * %this.speedDamageScale, "Impact");
}

//-----------------------------------------------------------------------------

function Actor::doPickup(%this, %obj, %col)
{
	if( ! isObject(%obj.myInventory))	return false;
	
	return %col.getDatablock().onPickup( %col , %obj );
}

//----------------------------------------------------------------------------

function Actor::damage(%this, %obj, %sourceObject, %position, %damage, %damageType)
{
   if (%obj.getState() $= "Dead")
      return;
   %obj.applyDamage(%damage);
   %location = "Body";

   // Deal with client callbacks here because we don't have this
   // information in the onDamage or onDisable methods
   %client = %obj.client;
   %sourceClient = %sourceObject ? %sourceObject.client : 0;

   if (%obj.getState() $= "Dead")
      %client.onDeath(%sourceObject, %sourceClient, %damageType, %location);
}

function Actor::onDamage(%this, %obj, %delta)
{
   // This method is invoked by the ShapeBase code whenever the 
   // object's damage level changes.
   if (%delta > 0 && %obj.getState() !$= "Dead") {

      // Increment the flash based on the amount.
      %flash = %obj.getDamageFlash() + ((%delta / %this.maxDamage) * 2);
      if (%flash > 0.75)
         %flash = 0.75;
      %obj.setDamageFlash(%flash);

      // If the pain is excessive, let's hear about it.
      if (%delta > 10)
         %obj.playPain();
   }
}

function Actor::onDisabled(%this,%obj,%state)
{
   // The player object sets the "disabled" state when damage exceeds
   // it's maxDamage value.  This is method is invoked by ShapeBase
   // state mangement code.

   // If we want to deal with the damage information that actually
   // caused this death, then we would have to move this code into
   // the script "damage" method.
   %obj.playDeathCry();
   %obj.playDeathAnimation();
   %obj.setDamageFlash(0.75);

   // Release the main weapon trigger
   %obj.setImageTrigger(0,false);

   // Schedule corpse removal.  Just keeping the place clean.
   %obj.schedule($CorpseTimeoutValue - 1000, "startFade", 1000, 0, true);
   %obj.schedule($CorpseTimeoutValue, "delete");
}


//-----------------------------------------------------------------------------

function Actor::onLeaveMissionArea(%this, %obj)
{
   // Inform the client
   %obj.client.onLeaveMissionArea();
}

function Actor::onEnterMissionArea(%this, %obj)
{
   // Inform the client
   %obj.client.onEnterMissionArea();
}

//-----------------------------------------------------------------------------

function Actor::onEnterLiquid(%this, %obj, %coverage, %type)
{
   switch(%type)
   {
      case 0: //Water
      case 1: //Ocean Water
      case 2: //River Water
      case 3: //Stagnant Water
      case 4: //Lava
         %obj.setDamageDt(%this, $DamageLava, "Lava");
      case 5: //Hot Lava
         %obj.setDamageDt(%this, $DamageHotLava, "Lava");
      case 6: //Crusty Lava
         %obj.setDamageDt(%this, $DamageCrustyLava, "Lava");
      case 7: //Quick Sand
   }
}

function Actor::onLeaveLiquid(%this, %obj, %type)
{
   %obj.clearDamageDt();
}


//-----------------------------------------------------------------------------

function Actor::onTrigger(%this, %obj, %triggerNum, %val)
{
   // This method is invoked when the player receives a trigger
   // move event.  The player automatically triggers slot 0 and
   // slot one off of triggers # 0 & 1.  Trigger # 2 is also used
   // as the jump key.
}
