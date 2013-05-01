//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Timeouts for corpse deletion.
$CorpseTimeoutValue = 5 * 1000;

// Damage Rate for entering Liquid
$DamageLava       = 0.01;
$DamageHotLava    = 0.01;
$DamageCrustyLava = 0.01;

//
$PlayerDeathAnim::TorsoFrontFallForward = 1;
$PlayerDeathAnim::TorsoFrontFallBack = 2;
$PlayerDeathAnim::TorsoBackFallForward = 3;
$PlayerDeathAnim::TorsoLeftSpinDeath = 4;
$PlayerDeathAnim::TorsoRightSpinDeath = 5;
$PlayerDeathAnim::LegsLeftGimp = 6;
$PlayerDeathAnim::LegsRightGimp = 7;
$PlayerDeathAnim::TorsoBackFallForward = 8;
$PlayerDeathAnim::HeadFrontDirect = 9;
$PlayerDeathAnim::HeadBackFallForward = 10;
$PlayerDeathAnim::ExplosionBlowBack = 11;


//----------------------------------------------------------------------------
// RTSUnitData Datablock methods
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

function RTSUnitData::onAdd(%this,%obj)
{
   // Vehicle timeout
   %obj.mountVehicle = true;

   // Default dynamic RTSUnitData stats
   %obj.setRechargeRate(%this.rechargeRate);
   %obj.setRepairRate(0);
   
   // Check to see if this should have a projectile, so look at the datablock
   if( !%this.isMelee )
      %obj.setProjectileDatablock(CrossbowProjectile);

//-----------------------Begin Bug Fix: http://garagegames.com/mg/forums/result.thread.php?qt=23339	  
   %obj.addModifier(BaseStats);
//-----------------------End Bug Fix 
}

function RTSUnitData::onRemove(%this, %obj)
{
   if (%obj.client.player == %obj)
      %obj.client.player = 0;
}

function RTSUnitData::onNewDataBlock(%this,%obj)
{
}


//----------------------------------------------------------------------------

function RTSUnitData::onMount(%this,%obj,%vehicle,%node)
{
   if (%node == 0)  {
      %obj.setTransform("0 0 0 0 0 1 0");
      %obj.setActionThread(%vehicle.getDatablock().mountPose[%node],true,true);
      %obj.lastWeapon = %obj.getMountedImage($WeaponSlot);

      %obj.unmountImage($WeaponSlot);
      %obj.setControlObject(%vehicle);
      %obj.client.setObjectActiveImage(%vehicle, 2);
   }
}

function RTSUnitData::onUnmount( %this, %obj, %vehicle, %node )
{
   if (%node == 0)
      %obj.mountImage(%obj.lastWeapon, $WeaponSlot);
}

function RTSUnitData::doDismount(%this, %obj, %forced)
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
      if (%obj.checkDismountPoint(%oldPos, %pos)) 
      {
         %success = %i;
         %impulseVec = %vec[%i];
         break;
      }
   }
   if (%forced && %success == -1)
      %pos = %oldPos;

   %obj.mountVehicle = false;
   %obj.schedule(4000, "setMountVehicle", true);

   // Position above dismount point
   %obj.setTransform(%pos);
   %obj.applyImpulse(%pos, VectorScale(%impulseVec, %obj.getDataBlock().mass));
   %obj.setPilot(false);
   %obj.vehicleTurret = "";
}


//----------------------------------------------------------------------------

function RTSUnitData::onCollision(%this,%obj,%col)
{
   //error("RTSUnitData - collision");

   if (%obj.getState() $= "Dead")
      return;

   // Try and pickup all items
   if (%col.getClassName() $= "Item")
      %obj.pickup(%col);
   if (%col.getClassName() $= "Resource")
      echo("collide with resource");

   // Mount vehicles
   %this = %col.getDataBlock();
   if ((%this.className $= WheeledVehicleData) && %obj.mountVehicle &&
         %obj.getState() $= "Move" && %col.mountable) {

      // Only mount drivers for now.
      %node = 0;
      %col.mountObject(%obj,%node);
      %obj.mVehicle = %col;
   }
}

function RTSUnitData::onImpact(%this, %obj, %collidedObject, %vec, %vecLen)
{
   //error("RTSUnitData - IMPACT");
   %obj.damage(0, VectorAdd(%obj.getPosition(),%vec),
      %vecLen * %this.speedDamageScale, "Impact");
}


//----------------------------------------------------------------------------

function RTSUnitData::damage(%this, %obj, %sourceObject, %position, %damage, %damageType)
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
   {
      %obj.onDeath(%sourceObject, %sourceClient, %damageType, %location);
      %client.onDeath(%sourceObject, %sourceClient, %damageType, %location);
   }
}

function RTSUnitData::onDamage(%this, %obj, %delta)
{
   messageClient(%obj.client, 'MsgUnitAttacked', "", getWords(%obj.getPosition(), 0, 2));
}

function RTSUnitData::onDisabled(%this,%obj,%state)
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

   // hopefully clean up any pending suppy auto-adds
   cancel(%obj.supplyAddEventId);
   cancel(%obj.TrainUnitEventId);
   // Schedule corpse removal.  Just keeping the place clean.
   %obj.schedule($CorpseTimeoutValue - 1000, "startFade", 1000, 0, true);
   %obj.schedule($CorpseTimeoutValue, "delete");
}

//-----------------------------------------------------------------------------

function RTSUnitData::onLeaveMissionArea(%this, %obj)
{
   // Inform the client
   %obj.client.onLeaveMissionArea();
}

function RTSUnitData::onEnterMissionArea(%this, %obj)
{
   // Inform the client
   %obj.client.onEnterMissionArea();
}

//-----------------------------------------------------------------------------

function RTSUnitData::onEnterLiquid(%this, %obj, %coverage, %type)
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

function RTSUnitData::onLeaveLiquid(%this, %obj, %type)
{
   %obj.clearDamageDt();
}


//-----------------------------------------------------------------------------

function RTSUnitData::onTrigger(%this, %obj, %triggerNum, %val)
{
   // This method is invoked when the player receives a trigger
   // move event.  The player automatically triggers slot 0 and
   // slot one off of triggers # 0 & 1.  Trigger # 2 is also used
   // as the jump key.
}


//-----------------------------------------------------------------------------
// Player methods
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------

function Player::kill(%this, %damageType)
{
   %this.damage(0, %this.getPosition(), 10000, %damageType);
}


//----------------------------------------------------------------------------

function Player::mountVehicles(%this,%bool)
{
   // If set to false, this variable disables vehicle mounting.
   %this.mountVehicle = %bool;
}

function Player::isPilot(%this)
{
   %vehicle = %this.getObjectMount();
   // There are two "if" statements to avoid a script warning.
   if (%vehicle)
      if (%vehicle.getMountNodeObject(0) == %this)
         return true;
   return false;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

function Player::playDeathAnimation(%this)
{
   // This will choose from 11 possible death animations.
   //if (%this.deathIdx++ > 11)
   //   %this.deathIdx = 1;
   //%this.setActionThread("death" @ %this.deathIdx);
   
   // The RTS example unit has "die" as it's animation name
   // The bot has no death animation, the building is a different name
   if( strstr( %this.getDatablock().shapeFile, "building" ) != -1 )
      %this.setActionThread("destroy", true);
   else
      %this.setActionThread("die", true);
   
}

function Player::playAttackAnimation(%this)
{
   // Lame hack because of inconsistant animation names
   // omgwtfbbq
   if( strstr( %this.getDatablock().shapeFile, "rifleman" ) != -1 )
      %this.setActionThread("fire");
   else // It's the shocker or the bot
      %this.setActionThread("attack");
}

function Player::playRunAnimation(%this)
{
   %this.setActionThread("run");
}

function Player::playRootAnimation(%this)
{
   // omgwtfbbq
   // Again another lame hack because of inconsistant animation names
   if( strstr( %this.getDatablock().shapeFile, "building" ) != -1 )
      %this.setActionThread( "idle" );
   else
      %this.setActionThread( "root" );
}

function Player::playCelAnimation(%this,%anim)
{
   if (%this.getState() !$= "Dead")
      %this.setActionThread("cel"@%anim);
}


//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

function Player::playDeathCry( %this )
{
   %this.playAudio(0,DeathCrySound);
}

function Player::playPain( %this )
{
   %this.playAudio(0,PainCrySound);
}

//----------------------------------------------------------------------------

// Horrible AI code.
function Player::onAdd(%this)
{
   %this.setProjectileDatablock(CrossbowProjectile);
}

function Player::onReachDestination(%this, %dest)
{

   if(%this.status $= "Collecting" && %this.curGoal $= "")
   {

    //%this.setActionThread("gather", true);
echo("Player::onReachDestination--(" @ %this @ ") is currently collecting.");    
     %this.collectResource(%this.collecting, 2000);
   }
   if(%this.status $= "Full" && %this.curGoal $= "")
   {
    //Gold.count +=  %this.currentAmt;
    echo("Player::onReachDestination-- (" @ %this @ ") is dropping off (" @ %this.resourceType @ ")");
    resourceStore::requestAddSupplies(%this.client, "LOCAL", "", %this.resourceType SPC %this.currentAmt, false);
    //resourceStore::SetSupply(%this.client, );
    %this.currentAmt = "";
    if (!(%this.resourcePos $= "" ) )
    {
      %this.setMoveDestination(%this.resourcePos);
      %this.curGoal = %this.resourcePos;
      %this.status = "Collecting";
      echo("Player::onReachDestination-- (" @ %this @ ") now moving to (" @ %this.resourcePos @ ")");

      return;
    }
    else
    {
      echo("Player::onReachDestination--peon caught trying to harvest at a spot that doesn't exist!");
      %this.setMoveGoal("");
      %this.status = "Idle";
      return;
    }
   }

   if(%this.curGoal $= "")
   {
      echo(%this @ " - Arrived!");
      return;
   }

   %nextWP = PathManager::getNextWaypoint(%this.getPosition(), %this.curGoal);
   
   %this.setMoveDestination(%nextWP);
   
   if(VectorDist(%nextWP, %this.curGoal) > 0.01)
   {
      echo(%this @ " - waypoint = " @ %nextWP);
   }
   else
   {
      %this.curGoal = "";
   }
   
   

}
function Player::collectResource(%this, %type ,%timeout)
{
// NOTE: The tracking of resource being carried (type and amount) isn't very authoritative.
// Currently, the player can "cheat" by setting a villager to collect an almost full carry amount
// on one resource, then manually change the villager to another resource, and in one gather cycle,
// fill up on the -new- resource and return. Best implementation (suggested) is to either allow the
// villager to track the amount for each possible resource type, and/or have their carryAmt set to 0
// if the currently harvesting resource type is not the same as currently carrying resource type (not implemented)

  echo("Villager (" @ %this @ ") is collecting (" @ %this.resourceType @ ") every (" @ %timeout/1000 @ ") seconds");
  %this.currentAmt +=5;
  if(%this.currentAmt > %this.maxAmt)
  {
   echo("Villager (" @ %this @ ") is heading to Town Center carrying (" @ %this.resourceType @ ")");
   %TC = findTC(%this.client);
   echo("which is at (" @ %TC.getPosition() @ ")"); 
   if (%TC.getPosition() $= "")
   {
     echo("Player::collectResource--No TC Found! Idling");
     %this.status = "Idle";
     if(isEventPending(%this.resourceID) )
     {
       cancel(%this.resourceID);
     }
     return;
   }
   %this.TCLoc = %TC.getPosition();

   if(isEventPending(%this.resourceID))
   {
     cancel(%this.resourceID);
   }
   %this.status = "Full";

   %this.setMoveDestination(%this.TCLoc);
   %this.curGoal = %this.TCLoc;
 }
 else
 {
   %this.resourceID = %this.schedule (%timeout,"collectResource" , %type,  %timeout);
 }
}


function findTC(%client){
// echo("looknig 4 TC");
 for(%i = 0; %i < %client.buildings.getCount(); %i++)
 {
    %bl = %client.buildings.getObject(%i);
// echo("findTC--current suspect building is (" @ %bl @
//     ") datablock is (" @ %bl.getDataBlock().getName() @ ")");
    if(%bl.getDataBlock().getName() $= "townCenterBlock")
    {
      echo("Villager is looking for closest drop off point. Found (" @ %bl @ ") at (" @ %bl.getPosition() @ ")");
      return %bl;
    }
 }
echo("findTC--could not find a TC, HANDLE THIS STATE");
}

function Player::onReachTarget(%this, %target)
{
   //error("Player CALLBACK! " @ %target);   
}

function Player::setMoveGoal(%this, %dest)
{
   %this.curGoal = %dest;
   echo("Player::setMoveGoal--object (" @ %this @ ") now moving to (" @ %this.curGoal @ ")");   
   // Kick off the movement.
   %this.onReachDestination(%this.getPosition());
}

function Player::stopAll(%this)
{
   %this.stop();
   %this.clearAim();
}

exec("~/data/players/player/player.cs");
// Pull the base in.
exec ("~/server/scripts/avatars/base.cs");
// Load dts shapes and merge animations

// Load unit datablocks
exec ("~/server/scripts/avatars/bot.cs");
exec ("~/server/scripts/avatars/rifleman.cs");
exec ("~/server/scripts/avatars/shocker.cs");
exec ("~/server/scripts/avatars/peon.cs");
// Load the rest of the stuff.
exec ("~/server/scripts/avatars/pathManager.cs");
