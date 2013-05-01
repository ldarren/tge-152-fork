//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
// Projectile trail emitter
datablock ParticleData(RifleSmokeParticle : baseSmokePD1 )
{
   dragCoeffiecient     = 0.0;
   gravityCoefficient   = -0.2;  // rises
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 300;   // time in ms
   lifetimeVarianceMS   = 150;   // ...more or less
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;
   colors[0]     = "0 0.2 1 1.0";
   colors[1]     = "0 0.2 1 1.0";
   colors[2]     = "0 0 0 0";
   sizes[0]      = 0.25;
   sizes[1]      = 0.4;
   sizes[2]      = 0.6;
   times[0]      = 0.0;
   times[1]      = 0.3;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(RifleSmokeEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 5;
   ejectionVelocity = 0.25;
   velocityVariance = 0.10;
   thetaMin         = 0.0;
   thetaMax         = 90.0;  
   particles = RifleSmokeParticle;
};


//-----------------------------------------------------------------------------
// Projectile Explosion
datablock ParticleData(RifleExplosionParticle : baseSmokePD1 )
{
   dragCoefficient      = 2;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.2;
   constantAcceleration = 0.0;
   lifetimeMS           = 750;
   lifetimeVarianceMS   = 150;
   colors[0]     = "0 0.2 1 1.0";
   colors[1]     = "0 0.2 1 0.0";
   sizes[0]      = 0.5;
   sizes[1]      = 1.0;
};
datablock ParticleEmitterData(RifleExplosionEmitter)
{
   ejectionPeriodMS = 7;
   periodVarianceMS = 0;
   ejectionVelocity = 1;
   velocityVariance = 1.0;
   ejectionOffset   = 0.0;
   thetaMin         = 0;
   thetaMax         = 60;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvances = false;
   particles = "RifleExplosionParticle";
};


datablock ExplosionData(RifleExplosion)
{
   lifeTimeMS = 1500;
   particleEmitter = RifleExplosionEmitter;
   particleDensity = 50;
   particleRadius = 0.2;
   faceViewer     = true;
   explosionScale = "1 1 1";
   shakeCamera = true;
   camShakeFreq = "10.0 11.0 10.0";
   camShakeAmp = "1.0 1.0 1.0";
   camShakeDuration = 0.5;
   camShakeRadius = 10.0;
   // Dynamic light
   lightStartRadius = 6;
   lightEndRadius = 0;
   lightStartColor = "0 0 0.6";
   lightEndColor = "0 0 0";
};

//-----------------------------------------------------------------------------
// Projectile Object

datablock ProjectileData(BlueEnergyProjectile)
{
	projectileShapeName = "./data/blueprojectile.dts";

   explosion			= RifleExplosion;

   particleEmitter		= RifleSmokeEmitter;

   velInheritFactor		= 1;
   armingDelay			= 0;
   lifetime				= 2000;
   fadeDelay			= 1500;
   bounceElasticity		= 0;
   bounceFriction		= 0;
   isBallistic			= false;
   gravityMod			= 0.10;
   hasLight				= true;
   lightRadius			= 3.0;
   lightColor			= "0 0 0.5";
};


datablock ProjectileData(BlueEnergyProjectileLongDelay)
{
	projectileShapeName = "./data/blueprojectile.dts";

   explosion			= RifleExplosion;

   particleEmitter		= RifleSmokeEmitter;

   velInheritFactor		= 1;
   armingDelay			= 4500;
   lifetime				= 7000;
   fadeDelay			= 2500;
   bounceElasticity		= 0.3;
   bounceFriction		= 0.3;
   isBallistic			= true;
   gravityMod			= 0.10;
   hasLight				= true;
   lightRadius			= 3.0;
   lightColor			= "0 0 0.5";
};

datablock ProjectileData(BlueEnergyProjectileBallistic)
{
	projectileShapeName = "./data/blueprojectile.dts";

   explosion			= RifleExplosion;

   particleEmitter		= RifleSmokeEmitter;

   velInheritFactor		= 1;
   armingDelay			= 0;
   lifetime				= 6000;
   fadeDelay			= 4500;
   bounceElasticity		= 0;
   bounceFriction		= 0;
   isBallistic			= true;
   gravityMod			= 1.0;
   hasLight				= true;
   lightRadius			= 3.0;
   lightColor			= "0 0 0.5";
};
