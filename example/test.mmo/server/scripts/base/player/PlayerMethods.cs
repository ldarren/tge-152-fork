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
