//-------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-------------------------------------------------------------------------

//
// Note: The order in which the particle system datablocks must be defined is:
// 1. ParticleData datablocks used by ParticleEmitter datablockss in next step.
// 2. ParticleEmitterData datablocks used by ParticleEmitterNode datablocks in next step.
// 3. ParticleEmitterNode datablocks.
//
// This is because the subsequent datablocks are dependent upon the prior and need
// the datablocks to be defined for initialization purposes.
//
// So, if you make a new set of particle datablocks and they malfunction, please
// check your loading order. 90% of the time, that is where the problem will exist.

datablock ParticleEmitterData(basePED) 
{
	//
	// Particles to emit
	//
	particles			= "baseSmokePD0";
    
	//
	// Eject with no offset
	//
	ejectionOffset		= 0.0;

	//
    // Eject with a velocity of 1 m/s (no variance)
	//
	ejectionVelocity	= 1.0;
	velocityVariance	= 0.0; 

	//
    // Eject new particle every 100..200 ms
	//
	ejectionPeriodMS	= 150; // 150 ms base time
	periodVarianceMS	= 50;  // +/- 50 ms delta time

	//
	// Ejects particles forever
	//
	lifetimeMS			   = 0; 
	lifetimeVarianceMS	= 0;

	//
    // Particles are screen oriented
	//
	orientParticles		= true;
	orientOnVelocity	= false;

	//
	// Shoot particles straight up
	//
	phiReferenceVel		= 0.0;
	phiVariance			= 0.0;

	//
    // up-down direction
	//
	thetaMax			= 0.0;
	thetaMin			= 0.0;

	//
    // Not used for particle emitter nodes
	//
	useEmitterColors	= false;
	useEmitterSizes		= false;

	//
	// Legacy field - always false
	//
	overrideAdvance		= false;
};


