//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Load dts shapes and merge animations
exec("~/data/shapes/Player/models/human_male/player.cs");
exec("~/data/shapes/Player/models/human_female/player.cs");

// Timeouts for corpse deletion.
$CorpseTimeoutValue = 22 * 1000;

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
// Player Audio Profiles
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

datablock AudioProfile(DeathCrySound)
{
   fileName = "~/data/sound/orc_death.ogg";
   description = AudioClose3d;
   preload = true;
};

datablock AudioProfile(PainCrySound)
{
   fileName = "~/data/sound/orc_pain.ogg";
   description = AudioClose3d;
   preload = true;
};


//----------------------------------------------------------------------------

datablock AudioProfile(FootLightSoftSound)
{
   filename    = "~/data/sound/footstep_soft.ogg";
   description = AudioClosest3d;
   preload = true;
};

datablock AudioProfile(FootLightHardSound)
{
   filename    = "~/data/sound/footstep_hard.ogg";
   description = AudioClose3d;
   preload = true;
};

datablock AudioProfile(FootLightMetalSound)
{
   filename    = "~/data/sound/footstep_hard.ogg";
   description = AudioClose3d;
   preload = true;
};

datablock AudioProfile(FootLightSnowSound)
{
   filename    = "~/data/sound/footstep_soft.ogg";
   description = AudioClosest3d;
   preload = true;
};

datablock AudioProfile(FootLightShallowSplashSound)
{
   filename    = "~/data/sound/footstep_water.ogg";
   description = AudioClose3d;
   preload = true;
};

datablock AudioProfile(FootLightWadingSound)
{
   filename    = "~/data/sound/footstep_water.ogg";
   description = AudioClose3d;
   preload = true;
};

datablock AudioProfile(FootLightUnderwaterSound)
{
   filename    = "~/data/sound/footstep_water.ogg";
   description = AudioClosest3d;
   preload = true;
};

datablock AudioProfile(FootLightBubblesSound)
{
   filename    = "~/data/sound/replaceme.ogg";
   description = AudioClose3d;
   preload = true;
};

datablock AudioProfile(ArmorMoveBubblesSound)
{
   filename    = "~/data/sound/replaceme.ogg";
   description = AudioCloseLooping3d;
   preload = true;
};

datablock AudioProfile(WaterBreathMaleSound)
{
   filename    = "~/data/sound/replaceme.ogg";
   description = AudioClosestLooping3d;
   preload = true;
};


//----------------------------------------------------------------------------

datablock AudioProfile(ImpactLightSoftSound)
{
   filename    = "~/data/sound/replaceme.ogg";
   description = AudioClose3d;
   preload = true;
   effect = ImpactSoftEffect;
};

datablock AudioProfile(ImpactLightHardSound)
{
   filename    = "~/data/sound/replaceme.ogg";
   description = AudioClose3d;
   preload = true;
   effect = ImpactHardEffect;
};

datablock AudioProfile(ImpactLightMetalSound)
{
   filename    = "~/data/sound/replaceme.ogg";
   description = AudioClose3d;
   preload = true;
   effect = ImpactMetalEffect;
};

datablock AudioProfile(ImpactLightSnowSound)
{
   filename    = "~/data/sound/replaceme.ogg";
   description = AudioClosest3d;
   preload = true;
   effect = ImpactSnowEffect;
};

datablock AudioProfile(ImpactLightWaterEasySound)
{
   filename    = "~/data/sound/replaceme.ogg";
   description = AudioClose3d;
   preload = true;
};

datablock AudioProfile(ImpactLightWaterMediumSound)
{
   filename    = "~/data/sound/replaceme.ogg";
   description = AudioClose3d;
   preload = true;
};

datablock AudioProfile(ImpactLightWaterHardSound)
{
   filename    = "~/data/sound/replaceme.ogg";
   description = AudioDefault3d;
   preload = true;
};

datablock AudioProfile(ExitingWaterLightSound)
{
   filename    = "~/data/sound/replaceme.ogg";
   description = AudioClose3d;
   preload = true;
};


//----------------------------------------------------------------------------
// Splash
//----------------------------------------------------------------------------

datablock ParticleData(PlayerSplashMist)
{
   dragCoefficient      = 2.0;
   gravityCoefficient   = -0.05;
   inheritedVelFactor   = 0.0;
   constantAcceleration = 0.0;
   lifetimeMS           = 400;
   lifetimeVarianceMS   = 100;
   useInvAlpha          = false;
   spinRandomMin        = -90.0;
   spinRandomMax        = 500.0;
   textureName          = "~/data/shapes/player/splash";
   colors[0]     = "0.7 0.8 1.0 1.0";
   colors[1]     = "0.7 0.8 1.0 0.5";
   colors[2]     = "0.7 0.8 1.0 0.0";
   sizes[0]      = 0.5;
   sizes[1]      = 0.5;
   sizes[2]      = 0.8;
   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(PlayerSplashMistEmitter)
{
   ejectionPeriodMS = 5;
   periodVarianceMS = 0;
   ejectionVelocity = 3.0;
   velocityVariance = 2.0;
   ejectionOffset   = 0.0;
   thetaMin         = 85;
   thetaMax         = 85;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   lifetimeMS       = 250;
   particles = "PlayerSplashMist";
};


datablock ParticleData(PlayerBubbleParticle)
{
   dragCoefficient      = 0.0;
   gravityCoefficient   = -0.50;
   inheritedVelFactor   = 0.0;
   constantAcceleration = 0.0;
   lifetimeMS           = 400;
   lifetimeVarianceMS   = 100;
   useInvAlpha          = false;
   textureName          = "~/data/shapes/player/splash";
   colors[0]     = "0.7 0.8 1.0 0.4";
   colors[1]     = "0.7 0.8 1.0 0.4";
   colors[2]     = "0.7 0.8 1.0 0.0";
   sizes[0]      = 0.1;
   sizes[1]      = 0.3;
   sizes[2]      = 0.3;
   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(PlayerBubbleEmitter)
{
   ejectionPeriodMS = 1;
   periodVarianceMS = 0;
   ejectionVelocity = 2.0;
   ejectionOffset   = 0.5;
   velocityVariance = 0.5;
   thetaMin         = 0;
   thetaMax         = 80;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   particles = "PlayerBubbleParticle";
};

datablock ParticleData(PlayerFoamParticle)
{
   dragCoefficient      = 2.0;
   gravityCoefficient   = -0.05;
   inheritedVelFactor   = 0.0;
   constantAcceleration = 0.0;
   lifetimeMS           = 400;
   lifetimeVarianceMS   = 100;
   useInvAlpha          = false;
   spinRandomMin        = -90.0;
   spinRandomMax        = 500.0;
   textureName          = "~/data/shapes/player/splash";
   colors[0]     = "0.7 0.8 1.0 0.20";
   colors[1]     = "0.7 0.8 1.0 0.20";
   colors[2]     = "0.7 0.8 1.0 0.00";
   sizes[0]      = 0.2;
   sizes[1]      = 0.4;
   sizes[2]      = 1.6;
   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(PlayerFoamEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;
   ejectionVelocity = 3.0;
   velocityVariance = 1.0;
   ejectionOffset   = 0.0;
   thetaMin         = 85;
   thetaMax         = 85;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   particles = "PlayerFoamParticle";
};


datablock ParticleData( PlayerFoamDropletsParticle )
{
   dragCoefficient      = 1;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.2;
   constantAcceleration = -0.0;
   lifetimeMS           = 600;
   lifetimeVarianceMS   = 0;
   textureName          = "~/data/shapes/player/splash";
   colors[0]     = "0.7 0.8 1.0 1.0";
   colors[1]     = "0.7 0.8 1.0 0.5";
   colors[2]     = "0.7 0.8 1.0 0.0";
   sizes[0]      = 0.8;
   sizes[1]      = 0.3;
   sizes[2]      = 0.0;
   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData( PlayerFoamDropletsEmitter )
{
   ejectionPeriodMS = 7;
   periodVarianceMS = 0;
   ejectionVelocity = 2;
   velocityVariance = 1.0;
   ejectionOffset   = 0.0;
   thetaMin         = 60;
   thetaMax         = 80;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   orientParticles  = true;
   particles = "PlayerFoamDropletsParticle";
};


datablock ParticleData( PlayerSplashParticle )
{
   dragCoefficient      = 1;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.2;
   constantAcceleration = -0.0;
   lifetimeMS           = 600;
   lifetimeVarianceMS   = 0;
   colors[0]     = "0.7 0.8 1.0 1.0";
   colors[1]     = "0.7 0.8 1.0 0.5";
   colors[2]     = "0.7 0.8 1.0 0.0";
   sizes[0]      = 0.5;
   sizes[1]      = 0.5;
   sizes[2]      = 0.5;
   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData( PlayerSplashEmitter )
{
   ejectionPeriodMS = 1;
   periodVarianceMS = 0;
   ejectionVelocity = 3;
   velocityVariance = 1.0;
   ejectionOffset   = 0.0;
   thetaMin         = 60;
   thetaMax         = 80;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   orientParticles  = true;
   lifetimeMS       = 100;
   particles = "PlayerSplashParticle";
};

datablock SplashData(PlayerSplash)
{
   numSegments = 15;
   ejectionFreq = 15;
   ejectionAngle = 40;
   ringLifetime = 0.5;
   lifetimeMS = 300;
   velocity = 4.0;
   startRadius = 0.0;
   acceleration = -3.0;
   texWrap = 5.0;

   texture = "~/data/shapes/player/splash";

   emitter[0] = PlayerSplashEmitter;
   emitter[1] = PlayerSplashMistEmitter;

   colors[0] = "0.7 0.8 1.0 0.0";
   colors[1] = "0.7 0.8 1.0 0.3";
   colors[2] = "0.7 0.8 1.0 0.7";
   colors[3] = "0.7 0.8 1.0 0.0";
   times[0] = 0.0;
   times[1] = 0.4;
   times[2] = 0.8;
   times[3] = 1.0;
};


//----------------------------------------------------------------------------
// Foot puffs
//----------------------------------------------------------------------------

datablock ParticleData(LightPuff)
{
   dragCoefficient      = 2.0;
   gravityCoefficient   = -0.01;
   inheritedVelFactor   = 0.6;
   constantAcceleration = 0.0;
   lifetimeMS           = 800;
   lifetimeVarianceMS   = 100;
   useInvAlpha          = true;
   spinRandomMin        = -35.0;
   spinRandomMax        = 35.0;
   colors[0]     = "1.0 1.0 1.0 1.0";
   colors[1]     = "1.0 1.0 1.0 0.0";
   sizes[0]      = 0.1;
   sizes[1]      = 0.8;
   times[0]      = 0.3;
   times[1]      = 1.0;
};

datablock ParticleEmitterData(LightPuffEmitter)
{
   ejectionPeriodMS = 35;
   periodVarianceMS = 10;
   ejectionVelocity = 0.2;
   velocityVariance = 0.1;
   ejectionOffset   = 0.0;
   thetaMin         = 20;
   thetaMax         = 60;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   useEmitterColors = true;
   particles = "LightPuff";
};

//----------------------------------------------------------------------------
// Liftoff dust
//----------------------------------------------------------------------------

datablock ParticleData(LiftoffDust)
{
   dragCoefficient      = 1.0;
   gravityCoefficient   = -0.01;
   inheritedVelFactor   = 0.0;
   constantAcceleration = 0.0;
   lifetimeMS           = 1000;
   lifetimeVarianceMS   = 100;
   useInvAlpha          = true;
   spinRandomMin        = -90.0;
   spinRandomMax        = 500.0;
   colors[0]     = "1.0 1.0 1.0 1.0";
   sizes[0]      = 1.0;
   times[0]      = 1.0;
};

datablock ParticleEmitterData(LiftoffDustEmitter)
{
   ejectionPeriodMS = 5;
   periodVarianceMS = 0;
   ejectionVelocity = 2.0;
   velocityVariance = 0.0;
   ejectionOffset   = 0.0;
   thetaMin         = 90;
   thetaMax         = 90;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   useEmitterColors = true;
   particles = "LiftoffDust";
};


//----------------------------------------------------------------------------

datablock DecalData(PlayerFootprint)
{
   sizeX       = 0.25;
   sizeY       = 0.25;
   textureName = "~/data/shapes/player/footprint";
};

datablock DebrisData( PlayerDebris )
{
   explodeOnMaxBounce = false;

   elasticity = 0.15;
   friction = 0.5;

   lifetime = 4.0;
   lifetimeVariance = 0.0;

   minSpinSpeed = 40;
   maxSpinSpeed = 600;

   numBounces = 5;
   bounceVariance = 0;

   staticOnMaxBounce = true;
   gravModifier = 1.0;

   useRadiusMass = true;
   baseRadius = 1;

   velocity = 20.0;
   velocityVariance = 12.0;
};             

datablock PlayerData(StandardBody)
{
	renderFirstPerson = false;
	emap = true;
	
	className = Actor;
	shapeFile = "~/data/shapes/player/models/human_male/human_male_1.dts";
	cameraMaxDist = 3;
	computeCRC = true;
	
	canObserve = true;
	cmdCategory = "Clients";
	
	cameraMaxDist = 30.0;
	cameraMinDist  = 20.0;
	
	cameraDefaultFov = 90.0;
	cameraMinFov = 5.0;
	cameraMaxFov = 120.0;
	
	debrisShapeName = "~/data/shapes/player/debris_player.dts";
	debris = playerDebris;
	
	aiAvoidThis = true;
	
	minLookAngle = -1.4;
	maxLookAngle = 1.4;
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
	
	boundingBox = "1.2 1.2 2.3";
	pickupRadius = 0.75;
	
	// Damage location details
	boxNormalHeadPercentage       = 0.83;
	boxNormalTorsoPercentage      = 0.49;
	boxHeadLeftPercentage         = 0;
	boxHeadRightPercentage        = 1;
	boxHeadBackPercentage         = 0;
	boxHeadFrontPercentage        = 1;
	
	// Foot Prints
	decalData   = PlayerFootprint;
	decalOffset = 0.25;
	
	footPuffEmitter = LightPuffEmitter;
	footPuffNumParts = 10;
	footPuffRadius = 0.25;
	
	dustEmitter = LiftoffDustEmitter;
	
	splash = PlayerSplash;
	splashVelocity = 4.0;
	splashAngle = 67.0;
	splashFreqMod = 300.0;
	splashVelEpsilon = 0.60;
	bubbleEmitTime = 0.4;
	splashEmitter[0] = PlayerFoamDropletsEmitter;
	splashEmitter[1] = PlayerFoamEmitter;
	splashEmitter[2] = PlayerBubbleEmitter;
	mediumSplashSoundVelocity = 10.0;   
	hardSplashSoundVelocity = 20.0;   
	exitSplashSoundVelocity = 5.0;
	
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
	
	footstepSplashHeight = 0.35;
	
	//NOTE:  some sounds commented out until wav's are available
	
	// Footstep Sounds
	FootSoftSound        = FootLightSoftSound;
	FootHardSound        = FootLightHardSound;
	FootMetalSound       = FootLightMetalSound;
	FootSnowSound        = FootLightSnowSound;
	FootShallowSound     = FootLightShallowSplashSound;
	FootWadingSound      = FootLightWadingSound;
	FootUnderwaterSound  = FootLightUnderwaterSound;
	
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
	
	groundImpactMinSpeed    = 10.0;
	groundImpactShakeFreq   = "4.0 4.0 4.0";
	groundImpactShakeAmp    = "1.0 1.0 1.0";
	groundImpactShakeDuration = 0.8;
	groundImpactShakeFalloff = 10.0;
	
	//exitingWater         = ExitingWaterLightSound;
	
	observeParameters = "0.5 4.5 4.5";
	
	// Allowable Inventory Items
	maxInv[BulletAmmo] = 20;
	maxInv[HealthKit] = 1;
	maxInv[RifleAmmo] = 100;
	maxInv[CrossbowAmmo] = 50;
	maxInv[Crossbow] = 1;
	maxInv[Rifle] = 1;
};

