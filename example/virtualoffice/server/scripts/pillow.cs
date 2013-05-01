//-----------------------------------------------------------------------------
// Torque Game Engine
// 
// Copyright (c) 2001 GarageGames.Com
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Sounds profiles
/*
datablock AudioProfile(CrossbowReloadSound)
{
filename = "~/data/sound/crossbow_reload.ogg";
description = AudioClose3d;
preload = true;
};
*/
datablock AudioProfile(PillowFireSound)
{
	filename = "~/data/sound/pillowFire.wav";
	description = AudioClose3d;
	preload = true;
};
/*
datablock AudioProfile(CrossbowFireEmptySound)
{
filename = "~/data/sound/crossbow_firing_empty.ogg";
description = AudioClose3d;
preload = true;
};

datablock AudioProfile(CrossbowExplosionSound)
{
filename = "~/data/sound/crossbow_explosion.ogg";
description = AudioDefault3d;
preload = true;
};
*/
//-----------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Weapon Item.  This is the item that exists in the world, i.e. when it's
// been dropped, thrown or is acting as re-spawnable item.  When the weapon
// is mounted onto a shape, the CrossbowImage is used.
datablock ItemData(Pillow)
{
   // Mission editor category
   category = "Weapon";

   // Hook into Item Weapon class hierarchy. The weapon namespace
   // provides common weapon handling functions in addition to hooks
   // into the inventory system.
   className = "Weapon";

   // Basic Item properties
   shapeFile = "~/data/shapes/sword/pillow.dts";
   mass = 1;
   elasticity = 0.2;
   friction = 0.6;
   emap = true;

	// Dynamic properties defined by the scripts
	pickUpName = "a pillow";
	image = PillowImage;

	itemType= "melee";
};


//--------------------------------------------------------------------------
// Sword image which does all the work.  Images do not normally exist in
// the world, they can only be mounted on ShapeBase objects.

// phdana hth ->
datablock ShapeBaseImageData(PillowImage)
{
   // Basic Item properties
   shapeFile = "~/data/shapes/sword/pillow.dts";
   emap = true;

   // Specify mount point & offset for 3rd person, and eye offset
   // for first person rendering.
   mountPoint = 0;
	//eyeOffset = "0.3 0.5 -0.7";


   // When firing from a point offset from the eye, muzzle correction
   // will adjust the muzzle vector to point to the eye LOS point.
   // Since this weapon doesn't actually fire from the muzzle point,
   // we need to turn this off.  
   correctMuzzleVector = false;


   // Add the WeaponImage namespace as a parent, WeaponImage namespace
   // provides some hooks into the inventory system.
   className = "WeaponImage";

   // Projectile && Ammo.
   item = Pillow;
   ammo = BulletAmmo;
   //customLookAnim = "looknw";
	customLookAnim = "h1root";

   // Here are the Attacks we support
   //hthNumAttacks = 3;
   hthAttack[0]                     = PillowOneHandedAttackSlice;
   hthAttack[1]                     = PillowOneHandedAttackSwing;
   //hthAttack[2]                     = OneHandedAttackThrust;
	hthAttack[2]                     = PillowOneHandedJumpAttack;

   // Initial start up state
   stateName[0]                     = "Preactivate";
   stateTransitionOnLoaded[0]       = "Activate";

   // Activating the gun.  Called when the weapon is first mounted
   stateName[1]                     = "Activate";
   stateTransitionOnTimeout[1]      = "Ready";
   stateTimeoutValue[1]             = 0.6;
   //stateSequence[1]                 = "Activate";

   // Ready to fire, just waiting for the trigger
   stateName[2]                     = "Ready";
   stateTransitionOnTriggerDown[2]  = "Fire";

   // Fire the weapon. Calls the fire script which does the actual work.
   stateName[3]                     = "Fire";
   stateTransitionOnTimeout[3]      = "Reload";
   stateTimeoutValue[3]             = 0.2;
   stateFire[3]                     = true;
   stateAllowImageChange[3]         = false;
   //stateSequence[3]                 = "Fire";
   stateScript[3]                   = "onFire";
   stateSound[3]                    = PillowFireSound;

   // Play the relead animation, and transition into
   stateName[4]                     = "Reload";
   stateTransitionOnTimeout[4]      = "Ready";
   stateTimeoutValue[4]             = 0.8;
   stateAllowImageChange[4]         = false;
   //stateSequence[4]                 = "Reload";
   stateEjectShell[4]               = false;
   //stateSound[4]                    = CrossbowReloadSound;
};
//-----------------------------------------------------------------------------

function PillowImage::onFire(%this, %obj, %slot)
{
    // default hand to hand weapon code
	if (!%obj.canJump())
	{
    	WeaponImage::onFireHandToHand(%this, %obj, %slot, "JumpAttack");
	}
	else
	{
		%offset = $sim::Time - %obj.hthDamageStartMS;
		//echo("offset = "@%offset);
		%attack = %obj.hthDamageAttack;
    	%endOffset = %attack.timeScale;
		//echo("endOffset = "@%endOffset);
		
		if (%offset > %endOffset)
      {
			WeaponImage::onFireHandToHand(%this, %obj, %slot, "Combo0");
		}
		else
		{
			if (%attack $= %this.hthAttack[0])
				WeaponImage::onFireHandToHand(%this, %obj, %slot, "Combo1");
			else
				if (%attack $= %this.hthAttack[1])
					WeaponImage::onFireHandToHand(%this, %obj, %slot, "Combo2");
		}
	} 
	return;
} 
 
