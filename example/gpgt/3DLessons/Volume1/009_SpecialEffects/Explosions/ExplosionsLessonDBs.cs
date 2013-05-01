//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// Full Standalone Explosion (all effects, excluding sub-explosions)
//-----------------------------------------------------------------------------

datablock ExplosionData(FieryExplosion)
{
	lifeTimeMS = 1200;

	// Volume particles
	particleEmitter = PEDExplosionSmoke;
	particleDensity = 150;
	particleRadius = 10;

	// Point emission
	emitter[0] = PEDExplosionSmoke;
	emitter[1] = PEDExplosionSparks;
   
	// Camera Shaking
	shakeCamera = true;
	camShakeFreq = "10.0 10.0 15.0";
	camShakeAmp = "50.0 50.0 80.0";
	camShakeDuration = 3.0;
	camShakeRadius = 40.0;
	camShakeFalloff = 35.0;

	// Exploding debris
	debris = FieryExplosionDebris;
	debrisThetaMin			= 0;
	debrisThetaMax			= 90;
	debrisPhiMin			= 0;
	debrisPhiMax			= 360;
	debrisNum				= 25;
	debrisNumVariance		= 5;
	debrisVelocity			= 3;
	debrisVelocityVariance	= 0.5;
   
	// Dynamic light
	lightStartRadius		= 10;
	lightEndRadius			= 2;
	lightStartColor			= "0.8 0.5 0.2";
	lightEndColor			= "0 0 0";
};
//-----------------------------------------------------------------------------
// Confetti Explosion
//-----------------------------------------------------------------------------

datablock ExplosionData(ConfettiSubExplosion2)
{

	offset = 10.0;

	// Volume particles
	particleEmitter = PEDConfettiSparksTiny;
	particleDensity = 250;
	particleRadius = 25;
};

datablock ExplosionData(ConfettiSubExplosion1)
{
	offset = 15.0;

	// Volume particles
	particleEmitter = PEDConfettiSparks;
	particleDensity = 150;
	particleRadius = 15;

	// Sub explosion objects
	subExplosion[0] = ConfettiSubExplosion2;
};


datablock ExplosionData(ConfettiExplosion)
{
	lifeTimeMS = 1200;

	// Volume particles
	particleEmitter = PEDExplosionFire;
	particleDensity = 15;
	particleRadius = 15;

	// Point emission
	emitter[0] = PEDConfettiSparks;

	// Sub explosion objects
	subExplosion[0] = ConfettiSubExplosion1;
	subExplosion[1] = ConfettiSubExplosion2;
	subExplosion[2] = ConfettiSubExplosion1;
	subExplosion[3] = ConfettiSubExplosion2;

	delayMS			= 200;
	delayVariance	= 50;
   
	// Camera Shaking
	shakeCamera = false;

	// Dynamic light
	lightStartRadius = 20;
	lightEndRadius = 0;
	lightStartColor = "1 1 1";
	lightEndColor = "0.2 0 0";
};
