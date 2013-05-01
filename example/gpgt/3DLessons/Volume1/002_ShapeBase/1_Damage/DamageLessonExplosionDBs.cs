//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
//					Explosion Datablocks for Exploding Gear
//-----------------------------------------------------------------------------
datablock ParticleData(GearsExplosionSmoke : baseSmokePD0 )
{
   dragCoeffiecient     = 100.0;
   gravityCoefficient   = 0;
   inheritedVelFactor   = 0.25;
   constantAcceleration = -0.30;
   lifetimeMS           = 1200;
   lifetimeVarianceMS   = 300;
   useInvAlpha =  true;
   spinRandomMin = -80.0;
   spinRandomMax =  80.0;

   colors[0]     = "0.56 0.36 0.26 1.0";
   colors[1]     = "0.2 0.2 0.2 1.0";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 4.0;
   sizes[1]      = 2.5;
   sizes[2]      = 1.0;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(GearsExplosionSmokeEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;
   ejectionVelocity = 4;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles = "GearsExplosionSmoke";
};

datablock ParticleData(GearsExplosionFire : baseFirePD0 )
{
   dragCoeffiecient     = 100.0;
   gravityCoefficient   = 0;
   inheritedVelFactor   = 0.25;
   constantAcceleration = 0.1;
   lifetimeMS           = 1200;
   lifetimeVarianceMS   = 300;
   useInvAlpha =  false;
   spinRandomMin = -80.0;
   spinRandomMax =  80.0;

   colors[0]     = "0.8 0.4 0 0.8";
   colors[1]     = "0.2 0.0 0 0.8";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 1.5;
   sizes[1]      = 0.9;
   sizes[2]      = 0.5;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(GearsExplosionFireEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;
   ejectionVelocity = 0.8;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles = "GearsExplosionFire";
};

datablock ParticleData(GearsExplosionSparks : baseSparkPD0 )
{
   dragCoefficient      = 1;
   gravityCoefficient   = 0.0;
   inheritedVelFactor   = 0.2;
   constantAcceleration = 0.0;
   lifetimeMS           = 500;
   lifetimeVarianceMS   = 350;

   colors[0]     = "0.60 0.40 0.30 1.0";
   colors[1]     = "0.60 0.40 0.30 1.0";
   colors[2]     = "1.0 0.40 0.30 0.0";

   sizes[0]      = 0.25;
   sizes[1]      = 0.15;
   sizes[2]      = 0.15;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(GearsExplosionSparkEmitter)
{
   ejectionPeriodMS = 3;
   periodVarianceMS = 0;
   ejectionVelocity = 5;
   velocityVariance = 1;
   ejectionOffset   = 0.0;
   thetaMin         = 0;
   thetaMax         = 180;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvances = false;
   orientParticles  = true;
   lifetimeMS       = 100;
   particles = "GearsExplosionSparks";
};

datablock ExplosionData(GearsSubExplosion1)
{
   offset = 0;
   emitter[0] = GearsExplosionSmokeEmitter;
   emitter[1] = GearsExplosionSparkEmitter;
};

datablock ExplosionData(GearsSubExplosion2)
{
   offset = 1.0;
   emitter[0] = GearsExplosionSmokeEmitter;
   emitter[1] = GearsExplosionSparkEmitter;
};

datablock ExplosionData(GearsExplosion)
{
   lifeTimeMS = 1200;

   // Volume particles
   particleEmitter = GearsExplosionFireEmitter;
   particleDensity = 75;
   particleRadius = 2;

   // Point emission
   emitter[0] = GearsExplosionSmokeEmitter;
   emitter[1] = GearsExplosionSparkEmitter;

   // Sub explosion objects
   subExplosion[0] = GearsSubExplosion1;
   subExplosion[1] = GearsSubExplosion2;

   // Camera Shaking
   shakeCamera = true;
   camShakeFreq = "10.0 11.0 10.0";
   camShakeAmp = "1.0 1.0 1.0";
   camShakeDuration = 0.5;
   camShakeRadius = 10.0;

   // Dynamic light
   lightStartRadius = 6;
   lightEndRadius = 3;
   lightStartColor = "0.5 0.5 0";
   lightEndColor = "0 0 0";
};

