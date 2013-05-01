//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// Debris Trailing Fire and Smoke - FieryExplosion
//-----------------------------------------------------------------------------
datablock ParticleData(PDTrailingSmoke : baseSmokePD0 )
{
   dragCoefficient      = 0.0;
   gravityCoefficient   = -0.1;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 250;
   useInvAlpha			= false;
   spinRandomMin		= -10.0;
   spinRandomMax		= 10.0;

   colors[0]			= "0.6 0.6 0.6 0.1";
   colors[1]			= "0.6 0.6 0.6 0.1";
   colors[2]			= "0.6 0.6 0.6 0.0";

   sizes[0]				= 0.5;
   sizes[1]				= 0.75;
   sizes[2]				= 1.5;

   times[0]				= 0.0;
   times[1]				= 0.5;
   times[2]				= 1.0;
};



//-----------------------------------------------------------------------------
//					Flaming Debris
//-----------------------------------------------------------------------------
datablock ParticleData(PDTrailingFire : baseFirePD0 )
{
	dragCoeffiecient		= 100.0;
	gravityCoefficient		= 0;
	inheritedVelFactor		= 0.25;
	constantAcceleration	= 0.1;
	lifetimeMS				= 1500;
	lifetimeVarianceMS		= 300;
	useInvAlpha				= false;
	spinRandomMin			= -80.0;
	spinRandomMax			= 80.0;

	colors[0]				= "0.8 0.4 0 0.8";
	colors[1]				= "0.2 0.0 0 0.8";
	colors[2]				= "0.0 0.0 0.0 0.0";

	sizes[0]				= 0.2;
	sizes[1]				= 0.6;
	sizes[2]				= 0.1;

	times[0]				= 0.0;
	times[1]				= 0.5;
	times[2]				= 1.0;
};

datablock ParticleEmitterData(PEDTrailingFire)
{
	ejectionPeriodMS		= 10;
	periodVarianceMS		= 0;
	ejectionVelocity		= 0.8;
	velocityVariance		= 0.5;
	thetaMin				= 0.0;
	thetaMax				= 180.0;
	lifetimeMS				= 2500;
	particles				= "PDTrailingFire";
};


datablock ParticleEmitterData(PEDTrailingSmoke)
{
	lifetime = 2000;

	ejectionPeriodMS = 20;
	periodVarianceMS = 5;

	ejectionVelocity = 0.25;
	velocityVariance = 0.10;

	thetaMin         = 0.0;
	thetaMax         = 180.0;  

	particles = PDTrailingSmoke;
};


datablock DebrisData(FieryExplosionDebris) {
	shapeFile			= "~/data/GPGTBase/shapes/Debris/3DDebrisShape.dts";
	lifetime			= 5.0;
	lifetimeVariance	= 1.5;

	velocity			= 20;
	velocityVariance	= 5;

	elasticity			= 0.6;

	friction			= 0.5;

	numBounces			= 5;
	bounceVariance		= 3;

	useRadiusMass		= true;
	baseRadius			= 0.3;

	gravModifier		= 0.5;

	terminalVelocity	= 25;

	fade = true;

	emitters[0] = "PEDTrailingFire";
	emitters[1] = "PEDTrailingFire";
};

