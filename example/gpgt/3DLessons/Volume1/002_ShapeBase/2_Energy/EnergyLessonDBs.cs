//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
//					Self-Recharging Block
//-----------------------------------------------------------------------------
datablock StaticShapeData( SelfRechargingBlock )
{
   category				= "LessonShapes";
   shapeFile				= "./data/DamageBlock.dts";

   maxEnergy				= 20;

   // Energy Reference Values
   rechargeRate			= 0.1; // 3.2 points per second
};


//-----------------------------------------------------------------------------
//					Disable-able Gears
//-----------------------------------------------------------------------------
datablock StaticShapeData( DisableEnergyGears : SelfRechargingBlock )
{
   shapeFile				= "./data/ThreeGears.dts";

   // dynamic field used by lesson scripts to stop/start animation
   disabledEnergyLevelPercent	= 0.35;
};



datablock ItemData( energyBall ) 
{
   category          = "LessonShapes";
   shapeFile         = "./data/energyBall.dts";
   elasticity        = 0.0;
   gravityMod        = 0.04;
   damageAmmount     = 15.0;
};



