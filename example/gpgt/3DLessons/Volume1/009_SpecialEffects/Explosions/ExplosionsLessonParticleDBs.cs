//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// Explosion Sparks - Used by FieryExplosion
//-----------------------------------------------------------------------------
datablock ParticleData(PDExplosionSparks : baseSparkPD0 )
{
	dragCoefficient			= 1;
	gravityCoefficient		= 0.0;
	inheritedVelFactor		= 0.2;
	constantAcceleration	= 0.0;
	lifetimeMS				= 500;
	lifetimeVarianceMS		= 350;

	colors[0]				= "0.60 0.40 0.30 1.0";
	colors[1]				= "0.60 0.40 0.30 1.0";
	colors[2]				= "1.0 0.40 0.30 0.0";

	sizes[0]				= 0.25;
	sizes[1]				= 0.15;
	sizes[2]				= 0.15;

	times[0]				= 0.0;
	times[1]				= 0.5;
	times[2]				= 1.0;
};

datablock ParticleEmitterData(PEDExplosionSparks)
{
	ejectionPeriodMS		= 3;
	periodVarianceMS		= 0;
	ejectionVelocity		= 5;
	velocityVariance		= 1;
	ejectionOffset			= 0.0;
	thetaMin				= 0;
	thetaMax				= 180;
	phiReferenceVel			= 0;
	phiVariance				= 360;
	overrideAdvances		= false;
	orientParticles			= true;
	lifetimeMS				= 0;
	particles				= "PDExplosionSparks";
};


//-----------------------------------------------------------------------------
// Explosion Smoke - Used by FieryExplosion
//-----------------------------------------------------------------------------

datablock ParticleData(PDExplosionSmoke : baseSmokePD0 )
{
	dragCoeffiecient		= 100.0;
	gravityCoefficient		= 0;
	inheritedVelFactor		= 0.25;
	constantAcceleration	= -0.30;
	lifetimeMS				= 1200;
	lifetimeVarianceMS		= 300;
	useInvAlpha				=  true;
	spinRandomMin			= -80.0;
	spinRandomMax			=  80.0;

	colors[0]				= "0.56 0.36 0.26 1.0";
	colors[1]				= "0.2 0.2 0.2 1.0";
	colors[2]				= "0.0 0.0 0.0 0.0";

	sizes[0]				= 4.0;
	sizes[1]				= 2.5;
	sizes[2]				= 1.0;

	times[0]				= 0.0;
	times[1]				= 0.5;
	times[2]				= 1.0;
};

datablock ParticleEmitterData(PEDExplosionSmoke)
{
	ejectionPeriodMS		= 10;
	periodVarianceMS		= 0;
	ejectionVelocity		= 4;
	velocityVariance		= 0.5;
	thetaMin				= 0.0;
	thetaMax				= 180.0;
	lifetimeMS				= 0;
	particles				= "PDExplosionSmoke";
};


//-----------------------------------------------------------------------------
// Explosion Fire - Used by FieryExplosion
//-----------------------------------------------------------------------------
datablock ParticleData(PDExplosionFire : baseFirePD0 )
{
	dragCoeffiecient		= 100.0;
	gravityCoefficient		= 0;
	inheritedVelFactor		= 0.25;
	constantAcceleration	= 0.1;
	lifetimeMS				= 1200;
	lifetimeVarianceMS		= 300;
	useInvAlpha				= false;
	spinRandomMin			= -80.0;
	spinRandomMax			= 80.0;

	colors[0]				= "0.8 0.4 0 0.8";
	colors[1]				= "0.2 0.0 0 0.8";
	colors[2]				= "0.0 0.0 0.0 0.0";

	sizes[0]				= 1.5;
	sizes[1]				= 0.9;
	sizes[2]				= 0.5;

	times[0]				= 0.0;
	times[1]				= 0.5;
	times[2]				= 1.0;
};

datablock ParticleEmitterData(PEDExplosionFire)
{
	ejectionPeriodMS		= 10;
	periodVarianceMS		= 0;
	ejectionVelocity		= 0.8;
	velocityVariance		= 0.5;
	thetaMin				= 0.0;
	thetaMax				= 180.0;
	lifetimeMS				= 0;
	particles				= "PDExplosionFire";
};


//-----------------------------------------------------------------------------
// Confetti Sparks - Used by ConfettiExplosion and sub-explosions
//-----------------------------------------------------------------------------
datablock ParticleData(PDConfettiSparksRed : baseSparkPD0 )
{
	dragCoefficient			= 0.3;
	gravityCoefficient		= 0.0;
	inheritedVelFactor		= 0.8;
	constantAcceleration	= 0.1;
	lifetimeMS				= 2500;
	lifetimeVarianceMS		= 350;

	colors[0]				= "1 0 0 1";
	colors[1]				= "1 0 0 1";
	colors[2]				= "1 0 0 0";

	sizes[0]				= 1.0;
	sizes[1]				= 2.0;
	sizes[2]				= 0.5;

	times[0]				= 0.0;
	times[1]				= 0.5;
	times[2]				= 1.0;
};

datablock ParticleData(PDConfettiSparksGreen : PDConfettiSparksRed)
{
	colors[0]				= "0 1 0 1";
	colors[1]				= "0 1 0 1";
	colors[2]				= "0 1 0 0";
};

datablock ParticleData(PDConfettiSparksBlue : PDConfettiSparksRed)
{
	colors[0]				= "0 0 1 1";
	colors[1]				= "0 0 1 1";
	colors[2]				= "0 0 1 0";
};

datablock ParticleData(PDConfettiSparksWhite : PDConfettiSparksRed)
{
	colors[0]				= "1 1 1 1";
	colors[1]				= "1 1 1 1";
	colors[2]				= "1 1 1 0";
};

datablock ParticleData(PDConfettiSparksRedTiny : PDConfettiSparksRed)
{
	lifetimeMS				= 3500;
	lifetimeVarianceMS		= 350;

	sizes[0]				= 0.5;
	sizes[1]				= 0.5;
	sizes[2]				= 0.5;
};

datablock ParticleData(PDConfettiSparksGreenTiny : PDConfettiSparksGreen)
{
	lifetimeMS				= 3500;
	lifetimeVarianceMS		= 350;

	sizes[0]				= 0.5;
	sizes[1]				= 0.5;
	sizes[2]				= 0.5;
};

datablock ParticleData(PDConfettiSparksBlueTiny : PDConfettiSparksBlue)
{
	lifetimeMS				= 3500;
	lifetimeVarianceMS		= 350;

	sizes[0]				= 0.5;
	sizes[1]				= 0.5;
	sizes[2]				= 0.5;
};

datablock ParticleData(PDConfettiSparksWhiteTiny : PDConfettiSparksWhite)
{
	lifetimeMS				= 3500;
	lifetimeVarianceMS		= 350;

	sizes[0]				= 0.5;
	sizes[1]				= 0.5;
	sizes[2]				= 0.5;
};


datablock ParticleEmitterData(PEDConfettiSparks)
{
	ejectionPeriodMS		= 500;
	periodVarianceMS		= 50;
	ejectionVelocity		= 10;
	velocityVariance		= 3;
	ejectionOffset			= 5.0;
	thetaMin				= 0;
	thetaMax				= 180;
	phiReferenceVel			= 0;
	phiVariance				= 360;
	overrideAdvances		= false;
	orientParticles			= true;
	lifetimeMS				= 200;
	particles				= "PDConfettiSparksRed PDConfettiSparksGreen PDConfettiSparksBlue PDConfettiSparksWhite";
};

datablock ParticleEmitterData(PEDConfettiSparksTiny)
{
	ejectionPeriodMS		= 750;
	periodVarianceMS		= 50;
	ejectionVelocity		= 10;
	velocityVariance		= 1;
	ejectionOffset			= 1.0;
	thetaMin				= 0;
	thetaMax				= 180;
	phiReferenceVel			= 0;
	phiVariance				= 360;
	overrideAdvances		= false;
	orientParticles			= true;
	lifetimeMS				= 1000;
	particles				= "PDConfettiSparksRedTiny PDConfettiSparksGreenTiny PDConfettiSparksBlueTiny PDConfettiSparksWhiteTiny";
};



