//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// This file contains Weapon and Ammo Class/"namespace" helper methods
// as well as hooks into the inventory system. These functions are not
// attached to a specific C++ class or datablock, but define a set of
// methods which are part of dynamic namespaces "class". The Items
// include these namespaces into their scope using the  ItemData and
// ItemImageData "className" variable.

// All ShapeBase images are mounted into one of 8 slots on a shape.
// This weapon system assumes all primary weapons are mounted into
// this specified slot:
$WeaponSlot = 0;


//-----------------------------------------------------------------------------
// Audio profiles

datablock AudioProfile(WeaponUseSound)
{
   filename = "~/data/sound/Weapon_switch.wav";
   description = AudioClose3d;
	preload = true;
};

datablock AudioProfile(WeaponPickupSound)
{
   filename = "~/data/sound/Weapon_pickup.wav";
   description = AudioClose3d;
	preload = true;
};

datablock AudioProfile(AmmoPickupSound)
{
   filename = "~/data/sound/Ammo_pickup.wav";
   description = AudioClose3d;
	preload = true;
};

//-----------------------------------------------------------------------------
// Weapon Class
//-----------------------------------------------------------------------------

function Weapon::onUse(%data,%obj)
{
   // Default behavoir for all weapons is to mount it into the
   // this object's weapon slot, which is currently assumed
   // to be slot 0

   if (%obj.getMountedImage($WeaponSlot) != %data.image.getId()) {
      ServerPlay3D(WeaponUseSound,%obj.getTransform());
      %obj.mountImage(%data.image, $WeaponSlot);

	//if (%data.itemType $= "melee")
   //    commandToClient(%obj.client, 'force3rdPerson', 1);
	//else
   //    commandToClient(%obj.client, 'force3rdPerson', 0);
   }
   return "Weapon";
}

function Weapon::onPickup(%this, %obj, %shape, %amount)
{
   // The parent Item method performs the actual pickup.
   // For player's we automatically use the weapon if the
   // player does not already have one in hand.
   if (Parent::onPickup(%this, %obj, %shape, %amount)) {
      ServerPlay3D(WeaponPickupSound,%obj.getTransform());
      if (%shape.getClassName() $= "Player" &&
            %shape.getMountedImage($WeaponSlot) == 0)  {
         %shape.use(%this, %shape.client);
      }
   }
}

function Weapon::onInventory(%this,%obj,%amount)
{
   // Weapon inventory has changed, make sure there are no weapons
   // of this type mounted if there are none left in inventory.
   if (!%amount && (%slot = %obj.getMountSlot(%this.image)) != -1)
      %obj.unmountImage(%slot);
}


//-----------------------------------------------------------------------------
// Weapon Image Class
//-----------------------------------------------------------------------------

// phdana hth ->
// a 'hand to hand attack' is a sequence that gets played
// as a "play once look anim". Hand to Hand weapons, such
// as an axe, can have one or more 'hand to hand attacks'
// that they can play

//RULER
//*****

datablock GameBaseData(RulerOneHandedAttackSlice)
{
   seqName = "h1slice";
   timeScale = 1.1;
   damageAmount = 5;
   //startDamage = 0.3;
   //endDamage = 0.7;
   startDamage = 0.1;
   endDamage = 0.3;
};

datablock GameBaseData(RulerOneHandedAttackSwing)
{
   seqName = "h1swing";
   timeScale = 1.1;
   damageAmount = 5;
   //startDamage = 0.2;
   //endDamage = 0.6;
   startDamage = 0.1;
   endDamage = 0.5;
};

datablock GameBaseData(RulerOneHandedJumpAttack)
{
   seqName = "h1jumpattack";
   timeScale = 1.1;
   damageAmount = 5;
   //startDamage = 0.4;
   //endDamage = 0.8;
   startDamage = 0.1;
   endDamage = 0.5;
};

//PILLOW
//*****

datablock GameBaseData(PillowOneHandedAttackSlice)
{
   seqName = "h1slice";
   timeScale = 1.1;
   damageAmount = 10;
   //startDamage = 0.3;
   //endDamage = 0.7;
   startDamage = 0.1;
   endDamage = 0.3;
};

datablock GameBaseData(PillowOneHandedAttackSwing)
{
   seqName = "h1swing";
   timeScale = 1.1;
   damageAmount = 10;
   //startDamage = 0.2;
   //endDamage = 0.6;
   startDamage = 0.1;
   endDamage = 0.5;
};

datablock GameBaseData(PillowOneHandedJumpAttack)
{
   seqName = "h1jumpattack";
   timeScale = 1.1;
   damageAmount = 10;
   //startDamage = 0.4;
   //endDamage = 0.8;
   startDamage = 0.1;
   endDamage = 0.5;
};

//PLUNGER
//*****

datablock GameBaseData(PlungerOneHandedAttackSlice)
{
   seqName = "h1slice";
   timeScale = 1.1;
   damageAmount = 15;
   //startDamage = 0.3;
   //endDamage = 0.7;
   startDamage = 0.1;
   endDamage = 0.3;
};

datablock GameBaseData(PlungerOneHandedAttackSwing)
{
   seqName = "h1swing";
   timeScale = 1.1;
   damageAmount = 15;
   //startDamage = 0.2;
   //endDamage = 0.6;
   startDamage = 0.1;
   endDamage = 0.5;
};

datablock GameBaseData(PlungerOneHandedJumpAttack)
{
   seqName = "h1jumpattack";
   timeScale = 1.1;
   damageAmount = 15;
   //startDamage = 0.4;
   //endDamage = 0.8;
   startDamage = 0.1;
   endDamage = 0.5;
};

//AXE
//*****

datablock GameBaseData(AxeOneHandedAttackSlice)
{
   seqName = "h1slice";
   timeScale = 1.1;
   damageAmount = 20;
   //startDamage = 0.3;
   //endDamage = 0.7;
   startDamage = 0.1;
   endDamage = 0.3;
};

datablock GameBaseData(AxeOneHandedAttackSwing)
{
   seqName = "h1swing";
   timeScale = 1.1;
   damageAmount = 20;
   //startDamage = 0.2;
   //endDamage = 0.6;
   startDamage = 0.1;
   endDamage = 0.5;
};

datablock GameBaseData(AxeOneHandedJumpAttack)
{
   seqName = "h1jumpattack";
   timeScale = 1.1;
   damageAmount = 20;
   //startDamage = 0.4;
   //endDamage = 0.8;
   startDamage = 0.1;
   endDamage = 0.5;
};

//SWORD
//*****

datablock GameBaseData(SwordOneHandedAttackSlice)
{
   seqName = "h1slice";
   timeScale = 1.1;
   damageAmount = 25;
   //startDamage = 0.3;
   //endDamage = 0.7;
   startDamage = 0.1;
   endDamage = 0.3;
};

datablock GameBaseData(SwordOneHandedAttackSwing)
{
   seqName = "h1swing";
   timeScale = 1.1;
   damageAmount = 25;
   //startDamage = 0.2;
   //endDamage = 0.6;
   startDamage = 0.1;
   endDamage = 0.5;
};

datablock GameBaseData(SwordOneHandedJumpAttack)
{
   seqName = "h1jumpattack";
   timeScale = 1.1;
   damageAmount = 25;
   //startDamage = 0.4;
   //endDamage = 0.8;
   startDamage = 0.1;
   endDamage = 0.5;
};


//-------------------------------------------------------------------------------------------------------------

// this is the default function to call when firing a hand-to-hand weapon
function WeaponImage::onFireHandToHand(%this, %obj, %slot, %action)
{
    if(%obj.hthStun || %obj.shielded)
       return;
   
	// there was code here for special attacks
   switch$(%action)
   {
       case "Combo0":
			%attack = %this.hthAttack[0];
       case "Combo1":
			%attack = %this.hthAttack[1];
		 case "Combo2":
			%attack = %this.hthAttack[2];
	    case "JumpAttack":
         %attack = %this.hthAttack[2];			// HARDCODED FOR TESTING
    }
	
    // setup the "play once look anim"
    %obj.hthDamageAttack = %attack;
    %obj.hthDamageSeqPlaying = 1;
    %obj.hthDamageStartMS =  $sim::Time;
    %obj.hthDamageLastId = -1;

	//echo("attack = "@%attack.seqName);
	//echo("timescale = "@%attack.timeScale);
    if (!%obj.setArmThreadPlayOnce(%attack.seqName))
       echo("ERROR in setArmThreadPlayOnce()");
	 else
    	return;
}
function WeaponImage::onFire(%this, %obj, %slot)
{
    if(%obj.hthStun || %obj.hthDamageSeqPlaying)
       return;
       
    %obj.shielded = true;
    return;
}

// default weapon intersect
function WeaponImage::onImageIntersect(%this,%player,%slot,%startvec,%endvec)
{
    // if damage sequence is not playing then dont do damage
    if (!%player.hthDamageSeqPlaying || %player.getState() $= "Dead")
       return;

    // determine if damage is active or if we can say the seq is done playing
    // based on current server time
    %offset = $sim::Time - %player.hthDamageStartMS;

    // depending on which attack is playing...
    %attack = %player.hthDamageAttack;
    %startOffset = %attack.startDamage;
    %endOffset = %attack.endDamage;

    // how long until the last damage is done
    // at which point we can say the seq has "Stopped playing"
    if (%offset > %endOffset)
    {
       %player.hthDamageSeqPlaying = 0;
       %player.hthDamageActive = 0;
	   //echo("seq stopping (all damage done) %offset = " @ %offset);
       return;
    }

    // how long it takes for damage to start...for now we just
    // have one interval and damage is active all during that interval
    if (%offset >%startOffset)
       %player.hthDamageActive = 1;

    // no damage yet?
    if (!%player.hthDamageActive)
    {
	   //echo("seq playing (no damage) %offset = " @ %offset);
       return;
    }

    // search for just players to damage
    %searchMasks = $TypeMasks::PlayerObjectType | $TypeMasks::StaticShapeObjectType;
    // search for objects within the damage rays that fit the masks above
    %scanTarg = ContainerRayCast(%startvec, %endvec, %searchMasks, %slot);

    if(%scanTarg && (%scanTarg.getType() & $TypeMasks::PlayerObjectType))
    {
        // a target in range was found
        %target = firstWord(%scanTarg);

        // store end point from raycast return buffer
        %pos = getWords(%scanTarg, 1, 3);

        // if we have hit this person already...apply no more damage
        if (%target == %player.hthDamageLastId)
           return;

        // save who we last damaged
        %player.hthDamageLastId = %target;

        // Apply damage targetted object
   		// Works for all shapebase objects.
        if (%target.getState() !$= "Dead" && %target.getId() !$= %player.getId())
   		{
           if(%target.shielded || %target.hthDamageSeqPlaying)
           {
              if(%target.hthDamageSeqPlaying)
                 %swordBlock = 1;
              		%block = 1;
              // Now we see if we hit from behind...
              %forwardVec = %target.getEyeVector();
              %objDir2D   = getWord(%forwardVec, 0) @ " " @ getWord(%forwardVec,1) @ " " @ "0";
              %objPos     = %target.getPosition();
              %dif        = VectorSub(%objPos, %player.getPosition());
              %dif        = getWord(%dif, 0) @ " " @ getWord(%dif, 1) @ " 0";
              %dif        = VectorNormalize(%dif);
              %dot        = VectorDot(%dif, %objDir2D);
              // 120 Deg angle test...
              // 1.05 == 60 degrees in radians
              if (%dot >= mCos(1.05))
                 %block = 0;
           }
           if(%block == 1)
           {
              //error("attack blocked!!");
              //%player.playAudio(0,ShieldImpactSound);
              //makeSparks(%pos);
              stunPlayer(%player,%attack);
              pushPlayerBack(%player,%pos,%target,%attack);
              if(%swordBlock == 1)
              {
                 stunPlayer(%target,%attack);
                 pushPlayerBack(%target,%pos,%player,%attack);
              }
              return;
           }

           %damage = %attack.damageAmount;
           %damageType = %this.item; // example: Axe / Sword etc

           %damLoc = firstWord(%target.getDamageLocation(%pos));
           // you can use this to add limited loacational damage, but the head is whats hit the most - TF
           //if(%damLoc $= "head")
              //error("object sliced on head");
           //else if(%damLoc $= "torso")
              //error("object sliced on torso");
           //else if(%damLoc $= "legs")
              //error("object sliced on legs");

           // code ripped from Armor::damgae
			%tmpDamage = %damage * %player.getDamageMod();
			%target.applyDamage(%tmpDamage);
  		   //%target.applyDamage(%damage);

           // this is in the Armor::damage
           //%location = "Body";

           // Deal with client callbacks here because we don't have this
           // information in the onDamage or onDisable methods
           %client = %target.client;
           %sourceObject = %player;//%this;
           %sourceClient = %sourceObject ? %sourceObject.client : 0;

           if (%target.getState() $= "Dead")
           {
              if(%client){
              %client.onDeath(%sourceObject, %sourceClient, %damageType, %pos);}
           }
           // phdana stun ->
           else
           {
              stunPlayer(%target,%attack);

              pushPlayerBack(%target,%pos,%player,%attack);
           }
           // <- phdana stun
      	}
    }
    else if(%scanTarg && (%scanTarg.getType() & $TypeMasks::StaticShapeObjectType))
    {
       %damage = %attack.damageAmount;
       %object = firstWord(%scanTarg);
		%tmpDamage = %damage * %player.getDamageMod();
		%object.applyDamage(%tmpDamage);
      // %object.applyDamage(%damage);
    }

}

// phdana stun ->
// call when %victim is hit with %attack but does not die
function stunPlayer(%vplayer, %attack)
{
   // for now we stun every time...

   // get the player for this object
   //if (!%victim.client || !%victim.client.player)
   if(!%vplayer.getType() & $TypeMasks::PlayerObjectType)
   {
      error("ATTEMPTING to STUN a non-player");
      return;
   }

   // if this player is in the middle of a hth swing themself, then
   // their swing is aborted. firstly we have to make sure they dont
   // do any damage, secondly we must blend their swing anim into
   // the stun anim
   if (%vplayer.hthDamageSeqPlaying)
   {
      // make sure they wont do damage....
      %vplayer.hthDamageSeqPlaying = false;

      // blend into the stun animation
      //error("STUN: victim: " @ %vplayer @ " DOING transition once...");
      //%vplayer.setArmThreadTransitionOnce("h1stun");
   }
   else
   {
      // just start the stun animation
      //error("STUN: victim: " @ %vplayer @ " only doing a playonce...");
      if(%vplayer.shielded)// not while stuned!
         %vplayer.setImageTrigger(1,false);
      //%vplayer.setArmThreadPlayOnce("h1stun");
   }

   // the victim is now in a stun state
   //%vplayer.hthStunSequencePlaying = true;
   %vplayer.hthStun = true;
   schedule(1000, %vplayer, "resetStun", %vplayer);
   //%vplayer.hthStunStartMS = $sim::Time;
}

function resetStun(%obj)
{
   %obj.hthStun = false;
}

function pushPlayerBack(%victim, %pos, %attacker, %attack)
{
  // the push back is relative to the attacker
  // a straight push back would be along the attackers
  // Y axis....

  // right now we always push the victim at his center
  // we could explore what happnes if we push at the
  // point of contact instead (might turn or do something intersting)

  // get the usual direction to push...we could get the Y axis of
  // the attacker with getTransform() then grabbing the rotation part
  // and passing that to VectorOrthoBasis() and then using column 1
  // whichi would be words 3,4,5 (couting from 0)...but that's overkill
  // for something that can be approximated pretty good by a line drawn
  // from attacker to victim...so let's use that instead
  %vpos = %victim.getWorldBoxCenter();
  %pushDirection = VectorSub(%vpos,%attacker.getWorldBoxCenter());
  %pushDirection = VectorNormalize(%pushDirection);

  // hardoded impluse
  %impulse = 1.0;

  // ok apply impulse to victim's center
  %mass = %victim.getDataBlock().mass;
  %pushVec = VectorScale(%pushDirection,%impulse * %mass);

  //error("Applying, to player " @ %victim @ " of mass " @ %mass @ ", an impulseVec: " @ %pushVec);

  %victim.applyImpulse(%vpos, %pushVec);
}

// <- phdana hth

function WeaponImage::onMount(%this,%obj,%slot)
{
   // Images assume a false ammo state on load.  We need to
   // set the state according to the current inventory.
   if (%obj.getInventory(%this.ammo))
      %obj.setImageAmmo(%slot,true);

   if (%this.customLookAnim !$= "")
   {
      %obj.setArmThread(%this.customLookAnim);
   }
   else
   {
      %obj.setArmThread("look");
   }
}


//-----------------------------------------------------------------------------
// Ammmo Class
//-----------------------------------------------------------------------------

function Ammo::onPickup(%this, %obj, %shape, %amount)
{
   // The parent Item method performs the actual pickup.
   if (Parent::onPickup(%this, %obj, %shape, %amount)) {
      ServerPlay3D(AmmoPickupSound,%obj.getTransform());
   }
}

function Ammo::onInventory(%this,%obj,%amount)
{
   // The ammo inventory state has changed, we need to update any
   // mounted images using this ammo to reflect the new state.
   for (%i = 0; %i < 8; %i++) {
      if ((%image = %obj.getMountedImage(%i)) > 0)
         if (isObject(%image.ammo) && %image.ammo.getId() == %this.getId())
            %obj.setImageAmmo(%i,%amount != 0);
   }
}
