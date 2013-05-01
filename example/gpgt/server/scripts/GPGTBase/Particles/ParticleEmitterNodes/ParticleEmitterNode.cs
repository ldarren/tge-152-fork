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

datablock ParticleEmitterNodeData(basePEND) 
{
	//
	// Use 1:1 time ratio
	//
	timeMultiple = 1.0;
};

