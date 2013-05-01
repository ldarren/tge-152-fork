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

//
// *********************** SMOKE PARTICLES
//
datablock ParticleData(baseSmokePD0)
{
	//
	// Do not use animated texture feature
	//
	animateTexture			= false;
	//animTexName[0]		= some_filename;
	// ...
	//animTexName[49]		= some_filename;
	//framesPerSec			= desired_FPS;


	//
	// Use a single texture (instead of anim)
	//
   textureName          = "~/data/GPGTBase/ParticleTextures/smoke";

	//
    // Particle does not accelerte
	//
	constantAcceleration	= 0.0;

	//
	// Some drag is applied
	//
	dragCoefficient			= 0.1;

     
	//
	// Wind does not affect this at all
	//
	windCoefficient			= 0.0;

	//
	// Float lightly upwards
	//
	gravityCoefficient		= -0.1;

	//
	// 100% of the PED.ejectionVelocity is added to this particle's velocity
	//
	inheritedVelFactor		= 1.0;

	//
	// Particles live for 2..3 seconds
	//
	lifetimeMS				= 2500;
	lifetimeVarianceMS		= 500;


	//
	// Particles has no spin
	//
	spinRandomMin			= 0.0;
	spinRandomMax			= 0.0;
	spinSpeed				= 0.0;

	//
	// Smooth keyframe progression
	//
	times[0]				= "0.0";
	times[1]				= "0.33";
	times[2]				= "0.66";
	times[3]				= "1.0";

	//
	// Color does not vary over time, but 
	// It becomes more and more transparent over time (no pop)
	//
	colors[0]				= "1.0 1.0 1.0 1.0";
	colors[1]				= "1.0 1.0 1.0 0.7";
	colors[2]				= "1.0 1.0 1.0 0.1";
	colors[3]				= "1.0 1.0 1.0 0.05";

	//
	// Sizes do not vary over time
	//
	sizes[0]				= "1.0";
	sizes[1]				= "1.0";
	sizes[2]				= "1.0";
	sizes[3]				= "1.0";

	//
	// Interpret alpha normally
	//
	useInvAlpha				= false;

};

datablock ParticleData(baseSmokePD1)
{
   textureName          = "~/data/GPGTBase/ParticleTextures/smokeParticle";
};

//
// *********************** FIRE PARTICLES
//
datablock ParticleData(baseFirePD0)
{
   textureName          = "~/data/GPGTBase/ParticleTextures/fire";
};

datablock ParticleData(baseAnimatedFirePD0)
{
   animTexName[0]      = "~/data/GPGTBase/ParticleTextures/niceflame0";
   animTexName[1]      = "~/data/GPGTBase/ParticleTextures/niceflame1";
   animTexName[2]      = "~/data/GPGTBase/ParticleTextures/niceflame2";
   animTexName[3]      = "~/data/GPGTBase/ParticleTextures/niceflame7";
   animTexName[4]      = "~/data/GPGTBase/ParticleTextures/niceflame3";
   animTexName[5]      = "~/data/GPGTBase/ParticleTextures/niceflame4";
   animTexName[6]      = "~/data/GPGTBase/ParticleTextures/niceflame5";
   animTexName[7]      = "~/data/GPGTBase/ParticleTextures/niceflame6";
   framesPerSec        = 4;
};


//
// *********************** SPARK PARTICLES
//
datablock ParticleData(baseSparkPD0)
{
   textureName          = "~/data/GPGTBase/ParticleTextures/spark";
};


//
// *********************** BUBBLE PARTICLES
//
datablock ParticleData(baseBubblePD0)
{
   textureName          = "~/data/GPGTBase/ParticleTextures/bubble";
};

//
// *********************** DUST PARTICLES
//
datablock ParticleData(baseDustPD0)
{
   textureName          = "~/data/GPGTBase/ParticleTextures/dust";
};


//
// *********************** MISC PARTICLES
//

