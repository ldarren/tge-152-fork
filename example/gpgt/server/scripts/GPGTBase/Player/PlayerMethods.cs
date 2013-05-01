//-------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-------------------------------------------------------------------------
// ******************************************************************
//					enableMountVehicles()
// ******************************************************************
//
// This method is used in conjuction with dismounting to enable 
// mounting again after a short period.  i.e. Players are temporarily
// told they cannot mount so so that if they collide with the vehicle 
// they are dismounting during the dismount they won't automatically
// re-mount it.
//
// There are other ways to do this, but this is sufficient for most
// cases.  If not, consider storing the last mounted shape for a bit
// and disallowing mounts to it alone.  You can use a similar method
// to this for clearing that value.
//
// ******************************************************************
function Player::enableMountVehicles( %Obj )
{
   %Obj.enableMountVehicle = true;
}


function Player::playDeathAnimation(%this)
{
   if (%this.deathIdx++ > 11)
      %this.deathIdx = 1;
   %this.setActionThread("Death" @ %this.deathIdx);
}

function Player::playCelAnimation(%this,%anim)
{
   if (%this.getState() !$= "Dead")
      %this.setActionThread("cel"@%anim);
}
//----------------------------------------------------------------------------

function Player::playDeathCry( %this )
{
   %this.playAudio(0,DeathCrySound);
}

function Player::playPain( %this )
{
   %this.playAudio(0,PainCrySound);
}
function Player::kill(%this, %damageType)
{
   %this.damage(0, %this.getPosition(), 10000, %damageType);
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
