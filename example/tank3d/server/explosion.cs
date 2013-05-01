// ============================================================
// Project            :  projMoomoo
// File               :  .\tankattack\server\explosion.cs
// Copyright          :  
// Author             :  Ezham
// Created on         :  Wednesday, February 21, 2007 4:54 PM
//
// Editor             :  Codeweaver v. 1.2.2595.6430
//
// Description        :  
//                    :  
//                    :  
// ============================================================

//-----------------------------------------------------------------------------
//
// EXPLOSION DATA NEED CUTTING DOWN....MEBBE NOT
//
//-----------------------------------------------------------------------------


//datablock AudioProfile(tankExplosionSound)
//{
//	filename = "~/data/sound/DeadAlien.ogg";//"~/data/sound/tank_explosion.ogg";
//	description = Audio2d;
//	preload = true;
//};

//-----------------------------------------------------------------------------
// Explosion Debris

// Debris "spark" explosion
datablock ParticleData(tankDebrisSpark)
{
   textureName          = "~/data/particles/fire";
   dragCoefficient      = 0;
   gravityCoefficient   = 0.0;
   windCoefficient      = 0;
   inheritedVelFactor   = 0.5;
   constantAcceleration = 0.0;
   lifetimeMS           = 500;
   lifetimeVarianceMS   = 50;
   spinRandomMin = -90.0;
   spinRandomMax =  90.0;
   useInvAlpha   = false;

   colors[0]     = "0.8 0.2 0 1.0";
   colors[1]     = "0.8 0.2 0 1.0";
   colors[2]     = "0 0 0 0.0";

   sizes[0]      = 0.2;
   sizes[1]      = 0.3;
   sizes[2]      = 0.1;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(tankDebrisSparkEmitter)
{
   ejectionPeriodMS = 20;
   periodVarianceMS = 0;
   ejectionVelocity = 0.5;
   velocityVariance = 0.25;
   ejectionOffset   = 0.0;
   thetaMin         = 0;
   thetaMax         = 90;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvances = false;
   orientParticles  = false;
   lifetimeMS       = 300;
   particles = "tankDebrisSpark";
};

datablock ExplosionData(tankDebrisExplosion)
{
   emitter[0] = tankDebrisSparkEmitter;

   // Turned off..
   shakeCamera = false;
   impulseRadius = 0;
   lightStartRadius = 0;
   lightEndRadius = 0;
};

// Debris smoke trail
datablock ParticleData(tankDebrisTrail)
{
   textureName          = "~/data/particles/fire";
   dragCoefficient      = 1;
   gravityCoefficient   = 0;
   inheritedVelFactor   = 0;
   windCoefficient      = 0;
   constantAcceleration = 0;
   lifetimeMS           = 800;
   lifetimeVarianceMS   = 100;
   spinSpeed     = 0;
   spinRandomMin = -90.0;
   spinRandomMax =  90.0;
   useInvAlpha   = true;

   colors[0]     = "0.8 0.3 0.0 1.0";
   colors[1]     = "0.1 0.1 0.1 0.7";
   colors[2]     = "0.1 0.1 0.1 0.0";

   sizes[0]      = 3.0;
   sizes[1]      = 3.0;
   sizes[2]      = 3.0;

   times[0]      = 0.1;
   times[1]      = 0.2;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(tankDebrisTrailEmitter)
{
   ejectionPeriodMS = 30;
   periodVarianceMS = 0;
   ejectionVelocity = 0.0;
   velocityVariance = 0.0;
   ejectionOffset   = 0.0;
   thetaMin         = 170;
   thetaMax         = 180;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   //overrideAdvances = false;
   //orientParticles  = true;
   lifetimeMS       = 5000;
   particles = "tankDebrisTrail";
};

// Debris object
datablock DebrisData(tankExplosionDebris)
{
   shapeFile = "~/data/particles/tank_debris.dts";
   emitters = "tankDebrisTrailEmitter";
   explosion = tankDebrisExplosion;
   
   elasticity = 0.6;
   friction = 0.5;
   numBounces = 10;
   bounceVariance = 1;
   explodeOnMaxBounce = true;
   staticOnMaxBounce = false;
   snapOnMaxBounce = false;
   minSpinSpeed = 50;
   maxSpinSpeed = 100;
   render2D = false;
   lifetime = 4;
   lifetimeVariance = 0.4;
   velocity = 15;
   velocityVariance = 0.5;
   fade = true;
   useRadiusMass = true;
   baseRadius = 1.0;
   gravModifier = 2.0;
   terminalVelocity = 15;
   ignoreWater = true;
};


//-----------------------------------------------------------------------------
// Bolt Explosion

datablock ParticleData(tankExplosionSmoke)
{
   textureName          = "~/data/particles/smoke";
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

datablock ParticleData(tankExplosionBubble)
{
   textureName          = "~/data/particles/bubble";
   dragCoeffiecient     = 0.0;
   gravityCoefficient   = -0.25;
   inheritedVelFactor   = 0.0;
   constantAcceleration = 0.0;
   lifetimeMS           = 1500;
   lifetimeVarianceMS   = 600;
   useInvAlpha          = false;
   spinRandomMin        = -100.0;
   spinRandomMax        =  100.0;

   colors[0]     = "0.7 0.8 1.0 0.4";
   colors[1]     = "0.7 0.8 1.0 0.4";
   colors[2]     = "0.7 0.8 1.0 0.0";

   sizes[0]      = 0.3;
   sizes[1]      = 0.3;
   sizes[2]      = 0.3;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(tankExplosionSmokeEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;
   ejectionVelocity = 4;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles = "tankExplosionSmoke";
};

datablock ParticleEmitterData(tankExplosionBubbleEmitter)
{
   ejectionPeriodMS = 9;
   periodVarianceMS = 0;
   ejectionVelocity = 1;
   ejectionOffset   = 0.1;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 80.0;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvances = false;
   particles = "tankExplosionBubble";
};

datablock ParticleData(tankExplosionFire)
{
   textureName          = "~/data/particles/fire";
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

datablock ParticleEmitterData(tankExplosionFireEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;
   ejectionVelocity = 0.8;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles = "tankExplosionFire";
};

datablock ParticleData(tankExplosionSparks)
{
   textureName          = "~/data/particles/spark";
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

datablock ParticleEmitterData(tankExplosionSparkEmitter)
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
   particles = "tankExplosionSparks";
};

datablock ExplosionData(tankSubExplosion1)
{
   offset = 0;
   emitter[0] = tankExplosionSmokeEmitter;
   emitter[1] = tankExplosionSparkEmitter;
};

datablock ExplosionData(tankSubExplosion2)
{
   offset = 1.0;
   emitter[0] = tankExplosionSmokeEmitter;
   emitter[1] = tankExplosionSparkEmitter;
};

datablock ExplosionData(tankExplosion)
{
   //soundProfile = tankExplosionSound;
   lifeTimeMS = 1200;

   // Volume particles
   particleEmitter = tankExplosionFireEmitter;
   particleDensity = 75;
   particleRadius = 10;

   // Point emission
   emitter[0] = tankExplosionSmokeEmitter;
   emitter[1] = tankExplosionSparkEmitter;

   // Sub explosion objects
   subExplosion[0] = tankSubExplosion1;
   subExplosion[1] = tankSubExplosion2;
   
   // Camera Shaking
   shakeCamera = true;
   camShakeFreq = "10.0 11.0 10.0";
   camShakeAmp = "1.0 1.0 1.0";
   camShakeDuration = 0.5;
   camShakeRadius = 10.0;

   // Exploding debris
   debris = tankExplosionDebris;
   debrisThetaMin = 20;
   debrisThetaMax = 60;//60;
   debrisPhiMin = 0;
   debrisPhiMax = 360;
   debrisNum = 1;
   debrisNumVariance = 0;
   debrisVelocity = 5;
   debrisVelocityVariance = 1.0;
   
   // Impulse
   impulseRadius = 10;
   impulseForce = 15;

   // Dynamic light
   lightStartRadius = 6;
   lightEndRadius = 3;
   lightStartColor = "0.5 0.5 0";
   lightEndColor = "0 0 0";
};

//-----------------------------------------------------------------------------