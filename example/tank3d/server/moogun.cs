//-----------------------------------------------------------------------------
//
// This object hierarchy exists for each rocket fired:
//
// MoogunProjectile -> MoogunExhaustParticleEmitter -> MoogunExhaustParticle
//                  -> MoogunExplosion (created upon impact, see below)
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
// If a rocket hits something, this object hierarchy will become active as
// each rocket will create an explosion effect upon impact:
//
// MoogunExplosion -> MoogunExplosionFireParticleEmitter   -> MoogunExplosionFireParticle
//				   -> MoogunExplosionParticleEmitter  -> MoogunExplosionSmokeParticle
//				   -> MoogunExplosionSparksParticleEmitter -> MoogunExplosionSparksParticles
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// When a rocket hits something, this is what it will sound like...
//-----------------------------------------------------------------------------
datablock AudioProfile(InGameFxShoot)
{
	filename = "~/data/sound/projectile_shoot.ogg";
	description = Audio2D;//AudioDefault3d;
	preload = true;
};

datablock AudioProfile(InGameFxExplodeMiss)
{
   filename    = "~/data/sound/projectile_miss.ogg";
   description = Audio2D;//AudioDefault3d;
   preload = true;
};

//-----------------------------------------------------------------------------
// Explosion Debris

// Debris "spark" explosion
datablock ParticleData(CrossbowDebrisSpark)
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

datablock ParticleEmitterData(CrossbowDebrisSparkEmitter)
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
   particles = "CrossbowDebrisSpark";
};

datablock ExplosionData(CrossbowDebrisExplosion)
{
   emitter[0] = CrossbowDebrisSparkEmitter;

   // Turned off..
   shakeCamera = false;
   impulseRadius = 0;
   lightStartRadius = 0;
   lightEndRadius = 0;
};

// Debris smoke trail
datablock ParticleData(CrossbowDebrisTrail)
{
   textureName          = "~/data/particles/smoke";
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

	colors[0] = "1 1 1 1";
	colors[1] = "1 1 0 0.5";
	colors[2] = "0 0 0 0.0";

	sizes[0] = 2;
	sizes[1] = 1;
	sizes[2] = 0.1;

   times[0]      = 0.1;
   times[1]      = 0.2;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(CrossbowDebrisTrailEmitter)
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
   particles = "CrossbowDebrisTrail";
};

// Debris object
datablock DebrisData(CrossbowExplosionDebris)
{
   shapeFile = "~/data/shapes/Moogun/Melon.dts";
   emitters = "CrossbowDebrisTrailEmitter";
   explosion = CrossbowDebrisExplosion;
   
   elasticity = 0.6;
   friction = 0.5;
   numBounces = 4;
   bounceVariance = 1;
   explodeOnMaxBounce = true;
   staticOnMaxBounce = false;
   snapOnMaxBounce = false;
   minSpinSpeed = 0;
   maxSpinSpeed = 700;
   render2D = true;
   lifetime = 4;
   lifetimeVariance = 0.4;
   velocity = 8;
   velocityVariance = 3;
   fade = true;
   useRadiusMass = true;
   baseRadius = 0.3;
   gravModifier = 0.65;
   terminalVelocity = 9;
   ignoreWater = true;
};


//-----------------------------------------------------------------------------
// Bolt Explosion

datablock ParticleData(CrossbowExplosionSmoke)
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

datablock ParticleEmitterData(CrossbowExplosionSmokeEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;
   ejectionVelocity = 4;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles = "CrossbowExplosionSmoke";
};


datablock ParticleData(CrossbowExplosionFire)
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

datablock ParticleEmitterData(CrossbowExplosionFireEmitter)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;
   ejectionVelocity = 0.8;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles = "CrossbowExplosionFire";
};

datablock ParticleData(CrossbowExplosionSparks)
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


datablock ParticleEmitterData(CrossbowExplosionSparkEmitter)
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
   particles = "CrossbowExplosionSparks";
};

//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
// When a rocket explodes upon impact, it will emit smoke like this...
//-----------------------------------------------------------------------------
datablock ParticleData( MoogunExplosionSmokeParticle )
{
	textureName = "~/data/particles/smoke";
	useInvAlpha =  true;

	dragCoeffiecient = 100.0;
	inheritedVelFactor = 0.3;
	constantAcceleration = -0.3;

	lifetimeMS = 1200;
	lifetimeVarianceMS = 300;

	times[0] = 0.0;
	times[1] = 0.5;
	times[2] = 1.0;

	colors[0] = "0.56 0.36 0.26 1.0";
	colors[1] = "0.2 0.2 0.2 1.0";
	colors[2] = "0.0 0.0 0.0 0.0";

	sizes[0] = 10.0;
	sizes[1] = 2.5;
	sizes[2] = 1.0;

};

datablock ParticleEmitterData( MoogunExplosionSmokeParticleEmitter )
{
	particles = "MoogunExplosionSmokeParticle";

	lifetimeMS = 250;
	lifetimeVarianceMS = 0;

	ejectionPeriodMS = 10;
	periodVarianceMS = 0;

	ejectionVelocity = 4;
	velocityVariance = 0.5;

};

//-----------------------------------------------------------------------------
// When a rocket explodes upon impact, it will emit fire like this...
//-----------------------------------------------------------------------------
datablock ParticleData( MoogunExplosionFireParticle )
{
	textureName = "~/data/particles/fire";
	useInvAlpha =  true;

	dragCoeffiecient = 100.0;
	inheritedVelFactor = 0.3;

	lifetimeMS = 800;
	lifetimeVarianceMS = 300;

	times[0] = 0.0;
	times[1] = 0.5;
	times[2] = 1.0;

	colors[0] = "1 1 1 1";
	colors[1] = "1 1 0 0.5";
	colors[2] = "0 0 0 0.0";

	sizes[0] = 1;
	sizes[1] = 8;
	sizes[2] = 10;
};

datablock ParticleEmitterData( MoogunExplosionFireParticleEmitter )
{
	particles = "MoogunExplosionFireParticle";

	lifetimeMS = 250;
	lifetimeVarianceMS = 0;

	ejectionPeriodMS = 10;
	periodVarianceMS = 0;

	ejectionVelocity = 0.8;
	velocityVariance = 0.5;
};

//-----------------------------------------------------------------------------
// When a rocket explodes upon impact, it will emit sparks like this...
//-----------------------------------------------------------------------------
datablock ParticleData( MoogunExplosionSparksParticles )
{
	textureName = "~/data/particles/spark";

	dragCoefficient      = 1;
	gravityCoefficient   = 0.0;
	inheritedVelFactor   = 0.2;
	constantAcceleration = 0.0;

	lifetimeMS = 500;
	lifetimeVarianceMS = 350;

	times[0] = 0.0;
	times[1] = 0.5;
	times[2] = 1.0;

	colors[0] = "1.0 0.0 0.0 1.0";
	colors[1] = "1.0 1.0 0.0 0.8";
	colors[2] = "1.0 1.0 1.0 0.1";

	sizes[0] = 0.20;
	sizes[1] = 0.15;
	sizes[2] = 0.5;
};

datablock ParticleEmitterData( MoogunExplosionSparksParticleEmitter )
{
	particles = "MoogunExplosionSparksParticles";

	lifetimeMS = 150;
    lifetimeVarianceMS = 0;

	ejectionPeriodMS = 2;
	periodVarianceMS = 0;

	ejectionVelocity = 30;
	velocityVariance = 10;
};

//-----------------------------------------------------------------------------
// When a rocket hits something, it will explode like this...
//-----------------------------------------------------------------------------
datablock ExplosionData( MoogunExplosion )
{
	soundProfile = InGameFxExplodeMiss;

	lifeTimeMS = 1200;
    lifetimeVarianceMS = 0;

	// Volume particles
	particleEmitter = MoogunExplosionFireParticleEmitter;
	particleDensity = 100;
	particleRadius  = 2;

	// Point emission particles
	emitter[0] = MoogunExplosionSmokeParticleEmitter;
	emitter[1] = MoogunExplosionSparksParticleEmitter;

	// This will make the camera shake when a player gets hit by a rocket.
    // Shoot your own feet to see this effect in action.
	shakeCamera      = true;
	camShakeFreq     = "10.0 11.0 10.0";
	camShakeAmp      = "1.0 1.0 1.0";
	camShakeDuration = 0.5;
	camShakeRadius   = 10.0;


   // Exploding debris
   debris = CrossbowExplosionDebris;
   debrisThetaMin = 0;
   debrisThetaMax = 60;
   debrisPhiMin = 0;
   debrisPhiMax = 360;
   debrisNum = 6;
   debrisNumVariance = 2;
   debrisVelocity = 1;
   debrisVelocityVariance = 0.5;

  
   // Impulse
   impulseRadius = 10;
   impulseForce = 15;

	// This will create a dynamic lighting effect in the vicinity of the 
    // rocket's explosion.
	lightStartRadius = 6;
	lightEndRadius   = 3;
	lightStartColor  = "0.5 0.5 0.0";
	lightEndColor    = "0.0 0.0 0.0";
};


//-----------------------------------------------------------------------------
// When a rocket is flying through the air, it will emit exhaust like this...
//-----------------------------------------------------------------------------
datablock ParticleData( MoogunExhaustParticle )
{
	textureName = "~/data/particles/smoke";
	useInvAlpha = true;

	lifetimeMS = 2000;
	lifetimeVarianceMS = 250;

	times[0] = 0.0;
	times[1] = 0.5;
	times[2] = 1.0;

	colors[0] = "1 1 1 1";
	colors[1] = "1 1 0 0.5";
	colors[2] = "0 0 0 0.0";

	sizes[0] = 1;
	sizes[1] = 0.5;
	sizes[2] = 0.1;
};

datablock ParticleEmitterData( MoogunExhaustParticleEmitter )
{
	particles = MoogunExhaustParticle;

	ejectionPeriodMS = 5;
	periodVarianceMS = 2;

	ejectionVelocity = 0.1;
	velocityVariance = 0.1;


};

//-----------------------------------------------------------------------------
// When the rocket launcher fires, it will emit a single rocket which behaves 
// like this...
//-----------------------------------------------------------------------------
datablock ProjectileData( MoogunProjectile )
{

   //projectileShapeName = "~/data/shapes/rocket_launcher/rocket.dts";
   projectileShapeName = "~/data/shapes/Moogun/Melon.dts";
	muzzleVelocity = 60;//30;
	armingDelay    = 0;
	lifetime       = 100000;
	fadeDelay      = 1000;
	isBallistic    = false;
	gravityMod     = 0.0;

    // The projectile will cast light on the ground as it travels.
	hasLight    = true;
	lightRadius = 5;
	lightColor  = "0.5 0.5 0.25";

    // The rocket will emit exhaust particles using this
	particleEmitter = MoogunExhaustParticleEmitter;

    // We'll create one of these if our rocket hits something
	explosion    = MoogunExplosion;
	directDamage = 200;
	radiusDamage = 100;
	damageRadius = 1.5;
};

function MoogunProjectile::onCollision( %this, %obj, %col, %fade, %pos, %normal )
{
   echo("MoogunProjectile::onCollision called! ----------------------------");
	

	if (%col.getType() & $TypeMasks::PlayerObjectType)	// If hit a player type
	{
		%col.kill(%this.directDamage, %obj.getName());
		alxPlay(InGameFxExplodeHit);
  	}
	else
	{
		%shooter = %obj.getName();
		guiMsgIsMiss(%shooter.getShapeName());			// show message the player misses
		if($cameraLock < 0)
			schedule(1000, 0, cameraManager, %obj.client, 6, 0);
		//cameraManager(%obj.client, $camOrbitMap, %obj.getName());
		//schedule(2000, 0, cameraManager, "CenterMark", $camOrbitMap, %obj.getName());
	}
}

//-----------------------------------------------------------------------------
// When the player uses the rocket launcher, it will behave like this...
//-----------------------------------------------------------------------------
datablock ShapeBaseImageData( MoogunImage )
{
	//shapeFile = "~/data/shapes/rocket_launcher/rocket_launcher.dts";
    shapeFile = "~/data/shapes/Moogun/Moogun.dts";

    projectile = MoogunProjectile;
    projectileType = Projectile;
    fireTimeout = 0;
	emap = true;

    // When firing from a point offset from the eye, muzzle correction will 
    // adjust the muzzle vector to point to the eye LOS point. Since this 
    // weapon doesn't actually fire from the muzzle point, we need to turn 
    // this off.
    correctMuzzleVector = false;

	// Specify mountPoint & offset for 3rd person, and eyeOffset for first 
    // person rendering.
	mountPoint = 0;
    offset = "0.0 0.0 0.0";
    //eyeOffset = "0.4 0.4 -0.6";
    eyeOffset = "0.0 0.0 0.0";

	// Images have a state system which controls how the animations are run, 
	// which sounds are played, script callbacks, etc. This state system is 
	// downloaded to the client so that clients can predict state changes and 
	// animate accordingly.  The following system supports basic 
	// ready->fire->reload transitions as well as a no-ammo->dryfire idle 
	// state.

	// Initial start up state
	stateName[0]                     = "Preactivate";
	stateTransitionOnLoaded[0]       = "Activate";
	stateTransitionOnNoAmmo[0]       = "NoAmmo";

	// Activating the gun. Called when the weapon is first mounted and there 
	// is ammo.
	stateName[1]                     = "Activate";
	stateTransitionOnTimeout[1]      = "Ready";
	stateTimeoutValue[1]             = 0.5;
	stateSequence[1]                 = "Activate";

	// Ready to fire, just waiting for the trigger
	stateName[2]                     = "Ready";
	stateTransitionOnNoAmmo[2]       = "NoAmmo";
	stateTransitionOnTriggerDown[2]  = "Fire";

	// Fire the weapon. Calls the onFire function when our player shoots.
	stateName[3]                     = "Fire";
	stateTransitionOnTimeout[3]      = "Reload";
	stateTimeoutValue[3]             = 0.1;
	stateFire[3]                     = true;
	stateRecoil[3]                   = LightRecoil;
	stateAllowImageChange[3]         = false;
	stateSequence[3]                 = "Fire";
	stateScript[3]                   = "onFire";
	stateEmitterTime[3]              = 0.3;
	stateSound[3]                    = InGameFxShoot;

	// Play the relead animation, and transition into
	stateName[4]                     = "Reload";
	stateTransitionOnNoAmmo[4]       = "NoAmmo";
	stateTransitionOnTimeout[4]      = "Ready";
	stateTimeoutValue[4]             = 0.1;
	stateAllowImageChange[4]         = false;
	stateSequence[4]                 = "Reload";
	stateEjectShell[4]               = true;

	// No ammo in the weapon, just idle until something shows up. Play the dry 
	// fire sound if the trigger is pulled.
	stateName[5]                     = "NoAmmo";
	stateTransitionOnAmmo[5]         = "Reload";
	stateSequence[5]                 = "NoAmmo";
	stateTransitionOnTriggerDown[5]  = "DryFire";

	// No ammo! Just perform a dry fire until the player finds more ammo.
	stateName[6]                     = "DryFire";
	stateTimeoutValue[6]             = 1.0;
	stateTransitionOnTimeout[6]      = "NoAmmo";
};

function MoogunImage::onFire( %this, %obj, %slot )
{	
	%projectile = %this.projectile;

	// Determine initial projectile velocity based on the gun's muzzle point 
	// and the object's current velocity...
	%muzzleVector   = %obj.getMuzzleVector( %slot );
	%objectVelocity = %obj.getVelocity();

	%muzzleVelocity = VectorAdd( VectorScale(%muzzleVector, %projectile.muzzleVelocity),
	                             VectorScale(%objectVelocity, %projectile.velInheritFactor) );

	// Create a new rocket projectile object...
	%p = new (%this.projectileType)()
	{
		dataBlock       = %projectile;
		initialVelocity = %muzzleVelocity;
		initialPosition = %obj.getMuzzlePoint(%slot);
		sourceObject    = %obj;
		sourceSlot      = %slot;
		client          = %obj.client;
	};

	MissionCleanup.add( %p );
	
	// Add name to projectile (scorekeeping and message purposes)
	%p.setName(%obj.getId());
	
	//if($cameraLock < 0)
   //	cameraManager(%obj.client, $camShootProjectile, %p);
	//	schedule(1500, 0, cameraManager, %obj.client, $camShootProjectile, %p);
	//schedule(1500, 0, cameraShootProjectileTimer, %obj.client, $camShootProjectile, %p);
		
	
	// Raycast to predict collision
	%startPos = %obj.getMuzzlePoint(%slot);
	%endPos = VectorAdd(%startPos, VectorScale(%obj.getMuzzleVector(%slot), 1000));
	%mask = $TypeMasks::PlayerObjectType;
	
	%target = ContainerRayCast(%startPos, %endPos, %mask, %obj);	// Raycast based on muzzle vector
	
	if(%target)	// If hit other player, use projectile camera
	{
		//if($cameraLock < 0)
   			cameraManager(%obj.client, $camShootProjectile, %p);
	}

	return %p;
}
