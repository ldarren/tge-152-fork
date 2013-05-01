//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------


//-----------------------------------------------------------------------------
// Vehicle Smoke 0
//-----------------------------------------------------------------------------

datablock ParticleData(PDVehicleSmoke0)
{
	textureName				= "./images/smoke";
	dragCoeffiecient		= 100.0;
	gravityCoefficient		= 0.1;
	inheritedVelFactor		= 0.25;
	constantAcceleration	= -0.30;
	lifetimeMS				= 3000;
	lifetimeVarianceMS		= 300;
	useInvAlpha				= true;
	spinRandomMin			= -80.0;
	spinRandomMax			=  80.0;

	colors[0]				= "0.4 0.4 0.4 0.8";
	colors[1]				= "0.4 0.4 0.4 0.4";
	colors[2]				= "0.4 0.4 0.4 0.0";

	sizes[0]				= 0.5;
	sizes[1]				= 3.0;
	sizes[2]				= 5.5;

	times[0]				= 0.0;
	times[1]				= 0.5;
	times[2]				= 1.0;
};

datablock ParticleEmitterData(PEDVehicleSmoke0)
{
	ejectionPeriodMS		= 10;
	periodVarianceMS		= 2;
	ejectionVelocity		= 4;
	velocityVariance		= 0.5;
	thetaMin				= 0.0;
	thetaMax				= 180.0;
	lifetimeMS				= 0;
	particles				= "PDVehicleSmoke0";
};
