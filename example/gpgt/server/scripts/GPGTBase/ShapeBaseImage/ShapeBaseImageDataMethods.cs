//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
return; 
function ShapeBaseImageData::onFire(%data, %obj, %slot)
{
	%projectile = %data.projectile;
	
	%muzzleVector = %obj.getMuzzleVector(%slot);
   %objectVelocity = %obj.getVelocity();
   
   %muzzleVelocity = VectorAdd(
      VectorScale(%muzzleVector, %projectile.muzzleVelocity),
      VectorScale(%objectVelocity, %projectile.velInheritFactor));
   
   %vehicle = 0;
   
       %p = new (%data.projectileType)() {
         dataBlock        = %data.projectile;
         initialVelocity  = %muzzleVelocity;
         initialDirection = %obj.getMuzzleVector(%slot);
         initialPosition  = %obj.getMuzzlePoint(%slot);
         sourceObject     = %obj;
         sourceSlot       = %slot;
         vehicleObject    = %vehicle;
      };
 
   if (isObject(%obj.lastProjectile) && %obj.deleteLastProjectile)
      %obj.lastProjectile.delete();

   %obj.lastProjectile = %p;
   %obj.deleteLastProjectile = %data.deleteLastProjectile;
   
   MissionCleanup.add(%p);
 
   return %p;
}

function ShapeBaseImageData::onMount(%this,%obj,%slot)
{
   // Images assume a false ammo state on load.  We need to
   // set the state according to the current inventory.
   //if (%obj.getInventory(%this.ammo))
   //   %obj.setImageAmmo(%slot,true);

   if (%obj.getInventory(%this.ammo)) {
     %obj.setImageAmmo(%slot,true);
     %currentAmmo = %obj.getInventory(%this.ammo);
     } else {
      %currentAmmo = 0;
     }
	 if ( isObject( %obj.client ) ) {
		%obj.client.setAmmoAmountHud(%currentAmmo);
	 }
}
