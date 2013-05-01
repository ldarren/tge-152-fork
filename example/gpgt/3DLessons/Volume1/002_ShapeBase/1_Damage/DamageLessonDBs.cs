//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
//					Self-Healing Block
//-----------------------------------------------------------------------------
datablock StaticShapeData( SelfHealingBlock )
{
   category          = "LessonShapes";
   shapeFile         = "./data/DamageBlock.dts";
   maxDamage         = 10;

   // Damage Reference Values
   disabledLevel			= 100; // Cannot be disabled
   destroyedLevel			= 100; // Cannot be destroyed
   repairRate				= 0.1; // 3.2 points per second
};

//-----------------------------------------------------------------------------
//					Invincible Block
//-----------------------------------------------------------------------------
datablock StaticShapeData( InvincibleBlock : SelfHealingBlock)
{
   //
   // Use className to tell shapes derived from this datablock
   // to look in the "SelfHealingBlock" hierarchy for their functions
   //
   className				= "SelfHealingBlock";
   isInvincible			= true;
};


//-----------------------------------------------------------------------------
//					Disable-able Gears
//-----------------------------------------------------------------------------
datablock StaticShapeData( DisableGears : SelfHealingBlock )
{
   shapeFile				= "./data/ThreeGears.dts";

   // Choose to be 'disabled' at 5 points damage.
   disabledLevel			= 5;
};


//-----------------------------------------------------------------------------
//					Exploding Gears
//-----------------------------------------------------------------------------
datablock StaticShapeData( ExplodeGears : DisableGears )
{
   //
   // Use className to tell shapes derived from this datablock
   // to look in the "DisableGears" hierarchy for their functions
   //
   className				= "DisableGears";

   //
   // Set the destroyed level to just slightly below maxDamage because our weak weapon
   // can't keep up with the healing rate
   //
   destroyedLevel			= 9; 

   // 
   // Make this shape 'disappear' when destroyed.
   //
   // Note:  The shape collision box remains in place!  To complete the effect, you must
   //		  use: %theShape.setHidden(true);  We are not doing that in this lesson.
   //        so the collision box stays in place.
   //
   renderWhenDestroyed		= false;

   //
   // Use the following explosion datablock when these gears are 'destroyed'
   //
   explosion				= "GearsExplosion";
};


datablock ItemData( damageBall ) 
{
   category          = "LessonShapes";
   shapeFile         = "./data/damageball.dts";
   elasticity        = 0.0;
   gravityMod        = 0.04;
   damageAmmount     = 15.0;
};
