//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Load dts shapes and merge animations
datablock TSShapeConstructor(PlayerDts)
{
   baseShape = "~/data/shapes/player/Player.dts";
   sequence0 = "~/data/shapes/player/Run.dsq Run";
   sequence1 = "~/data/shapes/player/Root.dsq Root";
   sequence2 = "~/data/shapes/player/Back.dsq Back";
   sequence3 = "~/data/shapes/player/Side.dsq Side";
   sequence4 = "~/data/shapes/player/Fall.dsq Fall";
   sequence5 = "~/data/shapes/player/Land.dsq Land";
   sequence6 = "~/data/shapes/player/Jump.dsq Jump";
   sequence7 = "~/data/shapes/player/Standjump.dsq Standjump";
   sequence8 = "~/data/shapes/player/Look.dsq Look";
   sequence9 = "~/data/shapes/player/Head.dsq Head";
   sequence10 = "~/data/shapes/player/Headside.dsq Headside";
   //sequence11 = "~/data/shapes/player/player_celwave.dsq celwave";
};

datablock PlayerData(PlayerShape)
{
   renderFirstPerson = false;
   emap = true;
   
   className = Armor;
   shapeFile = "~/data/shapes/player/Player.dts";
   cameraMaxDist = 3;
   computeCRC = true;
  
   canObserve = true;
   cmdCategory = "Clients";

   cameraDefaultFov = 90.0;
   cameraMinFov = 5.0;
   cameraMaxFov = 120.0;
   
   //debrisShapeName = "~/data/shapes/player/debris_player.dts";
   //debris = playerDebris;

   aiAvoidThis = true;

   minLookAngle = -0.48;		// EDIT: Shoot Angle
   maxLookAngle = -0.48;		// EDIT: Shoot Angle
   maxFreelookAngle = 3.0;

   mass = 90;
   drag = 0.3;
   maxdrag = 0.4;
   density = 10;
   maxDamage = 100;
   maxEnergy =  60;
   repairRate = 0.33;
   energyPerDamagePoint = 75.0;

   rechargeRate = 0.256;

   runForce = 48 * 90;
   runEnergyDrain = 0;
   minRunEnergy = 0;
   maxForwardSpeed = 14;
   maxBackwardSpeed = 13;
   maxSideSpeed = 13;

   maxUnderwaterForwardSpeed = 8.4;
   maxUnderwaterBackwardSpeed = 7.8;
   maxUnderwaterSideSpeed = 7.8;

   jumpForce = 8.3 * 90;
   jumpEnergyDrain = 0;
   minJumpEnergy = 0;
   jumpDelay = 15;

   recoverDelay = 9;
   recoverRunForceScale = 1.2;

   minImpactSpeed = 45;
   speedDamageScale = 0.4;

   boundingBox = "1.0 1.0 2.0";
   pickupRadius = 0.75;
   
   // Foot Prints
   //decalData   = PlayerFootprint;
   //decalOffset = 0.25;
   
   //footPuffEmitter = LightPuffEmitter;
   //footPuffNumParts = 10;
   //footPuffRadius = 0.25;

   //dustEmitter = LiftoffDustEmitter;

   //splash = PlayerSplash;
   //splashVelocity = 4.0;
   //splashAngle = 67.0;
   //splashFreqMod = 300.0;
   //splashVelEpsilon = 0.60;
   //bubbleEmitTime = 0.4;
   //splashEmitter[0] = PlayerFoamDropletsEmitter;
   //splashEmitter[1] = PlayerFoamEmitter;
   //splashEmitter[2] = PlayerBubbleEmitter;
   //mediumSplashSoundVelocity = 10.0;   
   //hardSplashSoundVelocity = 20.0;   
   //exitSplashSoundVelocity = 5.0;

   // Controls over slope of runnable/jumpable surfaces
   runSurfaceAngle  = 70;
   jumpSurfaceAngle = 80;

   minJumpSpeed = 20;
   maxJumpSpeed = 30;

   horizMaxSpeed = 68;
   horizResistSpeed = 33;
   horizResistFactor = 0.35;

   upMaxSpeed = 80;
   upResistSpeed = 25;
   upResistFactor = 0.3;
   
   //footstepSplashHeight = 0.35;

   //NOTE:  some sounds commented out until wav's are available

   // Footstep Sounds
   //FootSoftSound        = FootLightSoftSound;
   //FootHardSound        = FootLightHardSound;
   //FootMetalSound       = FootLightMetalSound;
   //FootSnowSound        = FootLightSnowSound;
   //FootShallowSound     = FootLightShallowSplashSound;
   //FootWadingSound      = FootLightWadingSound;
   //FootUnderwaterSound  = FootLightUnderwaterSound;

   //FootBubblesSound     = FootLightBubblesSound;
   //movingBubblesSound   = ArmorMoveBubblesSound;
   //waterBreathSound     = WaterBreathMaleSound;

   //impactSoftSound      = ImpactLightSoftSound;
   //impactHardSound      = ImpactLightHardSound;
   //impactMetalSound     = ImpactLightMetalSound;
   //impactSnowSound      = ImpactLightSnowSound;
   
   //impactWaterEasy      = ImpactLightWaterEasySound;
   //impactWaterMedium    = ImpactLightWaterMediumSound;
   //impactWaterHard      = ImpactLightWaterHardSound;
   
   //groundImpactMinSpeed    = 10.0;
   //groundImpactShakeFreq   = "4.0 4.0 4.0";
   //groundImpactShakeAmp    = "1.0 1.0 1.0";
   //groundImpactShakeDuration = 0.8;
   //groundImpactShakeFalloff = 10.0;
   
   //exitingWater         = ExitingWaterLightSound;
   
   observeParameters = "0.5 4.5 4.5";
};


//----------------------------------------------------------------------------
// PlayerShape Datablock methods
//----------------------------------------------------------------------------

function PlayerShape::onAdd(%this,%obj)
{
   // Called when the PlayerData datablock is first 'read' by the engine (executable)

   //MOOMOO
   //parent::onAdd( %this, %obj );
   %obj.mountImage( MoogunImage, 0 );
   %obj.setImageAmmo( 0, 1 );
	//%obj.setCloaked(true); // CLOAKING ***********disabled
}

function PlayerShape::onRemove(%this, %obj)
{
   if (%obj.client.player == %obj)
      %obj.client.player = 0;
}

function PlayerShape::onNewDataBlock(%this,%obj)
{
   // Called when this PlayerData datablock is assigned to an object
}          

