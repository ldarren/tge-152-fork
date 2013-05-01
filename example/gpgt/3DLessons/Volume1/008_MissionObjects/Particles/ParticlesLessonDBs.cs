//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// Common PEND
//-----------------------------------------------------------------------------
datablock ParticleEmitterNodeData(PENDParticlesLesson) {
	//
	// Use 1:1 time ratio
	//
	timeMultiple = 1.0;
};




//-----------------------------------------------------------------------------
// Sparks 0
//-----------------------------------------------------------------------------
datablock ParticleData(PDSpark0 : baseSparkPD0 )
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

datablock ParticleEmitterData(PEDSpark0)
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
	particles				= "PDSpark0";
};


//-----------------------------------------------------------------------------
// Bubbles 0
//-----------------------------------------------------------------------------
datablock ParticleData(PDBubble0 : baseBubblePD0 )
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

	sizes[0]				= 0.15;
	sizes[1]				= 0.25;
	sizes[2]				= 0.45;

	times[0]				= 0.0;
	times[1]				= 0.5;
	times[2]				= 1.0;
};

datablock ParticleEmitterData(PEDBubble0)
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
	particles				= "PDBubble0";
};

//-----------------------------------------------------------------------------
// Smoke 0
//-----------------------------------------------------------------------------

datablock ParticleData(PDSmoke0 : baseSmokePD0 )
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

datablock ParticleEmitterData(PEDSmoke0)
{
	ejectionPeriodMS		= 10;
	periodVarianceMS		= 0;
	ejectionVelocity		= 4;
	velocityVariance		= 0.5;
	thetaMin				= 0.0;
	thetaMax				= 180.0;
	lifetimeMS				= 0;
	particles				= "PDSmoke0";
};

//-----------------------------------------------------------------------------
// Rising Smoke
//-----------------------------------------------------------------------------
datablock ParticleData(PDRisingSmoke : baseSmokePD0 )
{
   dragCoefficient      = 0.0;
   gravityCoefficient   = -0.2;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 3000;
   lifetimeVarianceMS   = 250;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.6 0.6 0.6 0.1";
   colors[1]     = "0.6 0.6 0.6 0.1";
   colors[2]     = "0.6 0.6 0.6 0.0";

   sizes[0]      = 0.5;
   sizes[1]      = 0.75;
   sizes[2]      = 1.5;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(PEDRisingSmoke)
{
   ejectionPeriodMS = 20;
   periodVarianceMS = 5;

   ejectionVelocity = 0.25;
   velocityVariance = 0.10;

   thetaMin         = 0.0;
   thetaMax         = 90.0;  

   particles = PDRisingSmoke;
};



//-----------------------------------------------------------------------------
// Fire 0
//-----------------------------------------------------------------------------
datablock ParticleData(PDFire0 : baseFirePD0 )
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

datablock ParticleEmitterData(PEDFire0)
{
	ejectionPeriodMS		= 10;
	periodVarianceMS		= 0;
	ejectionVelocity		= 0.8;
	velocityVariance		= 0.5;
	thetaMin				= 0.0;
	thetaMax				= 180.0;
	lifetimeMS				= 0;
	particles				= "PDFire0";
};



//-----------------------------------------------------------------------------
// Nice Fire
//-----------------------------------------------------------------------------
datablock ParticleData(PDNiceFire : baseAnimatedFirePD0 )
{
   dragCoefficient     = 0.0;
   gravityCoefficient   = 0.01; // sink slowly to mask bottom of flame 
   inheritedVelFactor   = 0;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 500;
   useInvAlpha = false;
   spinRandomMin = 0.0;
   spinRandomMax = 0.0;

   colors[0]     = "1.0 1.0 1.0 1.0";
   colors[1]     = "0.7 0.7 0.7 0.7";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 2.0;
   sizes[1]      = 1.9;
   sizes[2]      = 1.8;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(PEDNiceFire)
{
   ejectionPeriodMS = 200;
   periodVarianceMS = 100;

   ejectionVelocity = 0.2;
   velocityVariance = 0.0;


   thetaMin         = 120.0;
   thetaMax         = 120.0; 
   
   phiReferenceVel  = 90;

   particles = "PDNiceFire";
};






