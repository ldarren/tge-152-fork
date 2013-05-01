/*
datablock StaticShapeData(hairaData)
{
	category = "Accessories";
	shapeFile = "~/data/shapes/players/DancingGirl/haira.dts";
	
	mass = 0;
	drag = 0;
	density = 100;
};

datablock StaticShapeData(hairbData)
{
	category = "Accessories";
	shapeFile = "~/data/shapes/players/DancingGirl/hairb.dts";
};
*/
datablock ShapeBaseImageData(hairaImage)
{
	shapeFile = "~/data/shapes/players/DancingGirl/haira.dts";
	mountPoint = 0;
};

datablock ShapeBaseImageData(hairbImage)
{
	shapeFile = "~/data/shapes/players/DancingGirl/hairb.dts";
	mountPoint = 0;
};

datablock ParticleData(ChimneyFire1)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.3;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 800;
   lifetimeVarianceMS   = 250;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.8 0.6 0.0 0.1";
   colors[1]     = "0.8 0.6 0.0 0.1";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 1.0;
   sizes[1]      = 3.0;
   sizes[2]      = 8.0;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleData(ChimneyFire2)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.5;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 1200;
   lifetimeVarianceMS   = 150;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.6 0.6 0.0 0.1";
   colors[1]     = "0.6 0.6 0.0 0.1";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 0.5;
   sizes[1]      = 0.5;
   sizes[2]      = 0.5;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(ChimneyFireEmitter)
{
   ejectionPeriodMS = 15;
   periodVarianceMS = 5;

   ejectionVelocity = 0.25;
   velocityVariance = 0.10;

   thetaMin         = 0.0;
   thetaMax         = 90.0;  

   particles = "ChimneyFire1" TAB "ChimneyFire2";
};

datablock ParticleEmitterNodeData(ChimneyFireEmitterNode)
{
   timeMultiple = 1;
};

datablock ParticleData(FairyDustParticle)
{
   dragCoefficient      = 10.0;
   gravityCoefficient   = 0;
   windCoefficient = 0;
   inheritedVelFactor   = 1.0;
   constantAcceleration = 0.5;
   lifetimeMS           = 1000;
   lifetimeVarianceMS   = 500;
   useInvAlpha          = false;
   spinRandomMin        = -90.0;
   spinRandomMax        = 500.0;
   textureName          = "~/data/shapes/particles/sparkle";
   colors[0]     = "0.4 0.6 1.0 0.80";
   colors[1]     = "0.4 0.6 1.0 0.80";
   colors[2]     = "0.4 0.6 1.0 0.50";
   sizes[0]      = 0.2;
   sizes[1]      = 0.4;
   sizes[2]      = 0.4;
   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(FairyDustEmitter)
{
   ejectionPeriodMS = 50;
   periodVarianceMS = 10;
   ejectionVelocity = 0;
   velocityVariance = 0;
   ejectionOffset   = 0;
   thetaMin         = 85;
   thetaMax         = 85;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   //lifetimeMS = 100;
   //lifetimeVarianceMS = 100;
   particles = "FairyDustParticle";
};

datablock ParticleEmitterNodeData( FairyDustParticleNode )
{
	timeMultiple = 1;
};

datablock ShapeBaseImageData(LeftEmitterBlockImage)
{
	// Basic Item properties
	shapeFile = "~/data/shapes/players/DancingGirl/emitter_dummy.dts";
	emap = true;
	
	// Specify mount point & offset for 3rd person, and eye offset
	// for first person rendering.
	mountPoint = 2;
	eyeOffset = "-1 0.4 -1.9";
	
	// When firing from a point offset from the eye, muzzle correction
	// will adjust the muzzle vector to point to the eye LOS point.
	// Since this weapon doesn't actually fire from the muzzle point,
	// we need to turn this off.  
	correctMuzzleVector = false;
	
	// Images have a state system which controls how the animations
	// are run, which sounds are played, script callbacks, etc. This
	// state system is downloaded to the client so that clients can
	// predict state changes and animate accordingly.  The following
	// system supports basic ready->fire->reload transitions as
	// well as a no-ammo->dryfire idle state.
	
	// Initial start up state
	stateName[0]                     = "Preactivate";
	stateTransitionOnLoaded[0]       = "Ready";
	stateTransitionOnNoAmmo[0]       = "NoAmmo";
	
	// Ready to fire, just waiting for the trigger
	stateName[1]                     = "Ready";
	stateTransitionOnNoAmmo[1]       = "NoAmmo";
	stateTransitionOnTriggerDown[1]  = "Fire";
	
	// Fire the weapon. Calls the fire script which does 
	// the actual work.
	stateName[2]                     = "Fire";
	stateTransitionOnTimeout[2]      = "Ready";
	stateTimeoutValue[2]             = 3.0;
	stateFire[2]                     = true;
	stateEmitter[2]                  = FairyDustEmitter;   
	stateEmitterTime[2]              = 3.0;   
	
	// No ammo in the weapon, just idle until something
	// shows up. Play the dry fire sound if the trigger is
	// pulled.
	stateName[3]                     = "NoAmmo";
	stateTransitionOnAmmo[3]         = "Ready";
	stateTransitionOnTriggerDown[3]  = "DryFire";
	
	// No ammo dry fire
	stateName[4]                     = "DryFire";
	stateTimeoutValue[4]             = 1.0;
	stateTransitionOnTimeout[4]      = "NoAmmo";
	stateEmitter[4]                  = FairyDustEmitter;   
	stateEmitterTime[4]              = 3.0;   
};

datablock ShapeBaseImageData(RightEmitterBlockImage)
{
	shapeFile = "~/data/shapes/players/DancingGirl/emitter_dummy.dts";
	emap = true;
	
	mountPoint = 1;
	eyeOffset = "0.78 0.4 -1.9";
	
	correctMuzzleVector = false;
	
	stateName[0]                     = "Preactivate";
	stateTransitionOnLoaded[0]       = "Ready";
	stateTransitionOnNoAmmo[0]       = "NoAmmo";
	
	stateName[1]                     = "Ready";
	stateTransitionOnNoAmmo[1]       = "NoAmmo";
	stateTransitionOnTriggerDown[1]  = "Fire";
	
	stateName[2]                     = "Fire";
	stateTransitionOnTimeout[2]      = "Ready";
	stateTimeoutValue[2]             = 3.0;
	stateFire[2]                     = true;
	stateEmitter[2]                  = FairyDustEmitter;   
	stateEmitterTime[2]              = 3.0;   
	
	stateName[3]                     = "NoAmmo";
	stateTransitionOnAmmo[3]         = "Ready";
	stateTransitionOnTriggerDown[3]  = "DryFire";
	
	stateName[4]                     = "DryFire";
	stateTimeoutValue[4]             = 1.0;
	stateTransitionOnTimeout[4]      = "NoAmmo";
	stateEmitter[4]                  = FairyDustEmitter;   
	stateEmitterTime[4]              = 3.0;   
};
