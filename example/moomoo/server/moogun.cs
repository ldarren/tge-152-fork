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
datablock AudioProfile( MoogunExplosionSound )
{
	filename = "~/data/sound/BazookaShoot.ogg";
	description = Audio2d;
	preload = true;
};

datablock AudioProfile( MoogunShootSound )
{
	filename = "~/data/sound/explosionOG.ogg";
	description = AudioDefault3d;
	preload = true;
};

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

	sizes[0] = 4.0;
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
	useInvAlpha =  false;

	dragCoeffiecient = 100.0;
	inheritedVelFactor = 0.3;

	lifetimeMS = 1200;
	lifetimeVarianceMS = 300;

	times[0] = 0.0;
	times[1] = 0.5;
	times[2] = 1.0;

	colors[0] = "0.8 0.4 0.0 0.8";
	colors[1] = "0.2 0.0 0.0 0.8";
	colors[2] = "0.0 0.0 0.0 0.0";

	sizes[0] = 1.5;
	sizes[1] = 0.9;
	sizes[2] = 0.5;
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
	//soundProfile = MoogunExplosionSound;

	lifeTimeMS = 1200;
    //lifetimeVarianceMS = 0;

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
	useInvAlpha = false;

	lifetimeMS = 2000;
	lifetimeVarianceMS = 250;

	times[0] = 0.0;
	times[1] = 0.5;
	times[2] = 1.0;

	colors[0] = "0.8 0.3 0.0 1.0";
	colors[1] = "0.1 0.1 0.1 0.7";
	colors[2] = "0.1 0.1 0.1 0.0";

	sizes[0] = 0.1;
	sizes[1] = 0.5;
	sizes[2] = 1.0;
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
   projectileShapeName = "~/data/shapes/moogun/moobullet.dts";
	muzzleVelocity = 13;//15;
	armingDelay    = 0;
	lifetime       = 10000;
	fadeDelay      = 0;
	isBallistic    = true;
	gravityMod     = 0.20;

   // The projectile will cast light on the ground as it travels.
	hasLight    = true;
	lightRadius = 5;
	lightColor  = "0.5 0.5 0.25";

   // The rocket will emit exhaust particles using this
	particleEmitter = MoogunExhaustParticleEmitter;

   // We'll create one of these if our rocket hits something
	explosion    = MoogunExplosion;
	directDamage = 50;
	radiusDamage = 50;
	damageRadius = 3.0;
};

function MoogunProjectile::onCollision( %this, %obj, %col, %fade, %pos, %normal )
{
   echo("MoogunProjectile::onCollision called! ----------------------------");
	
	%name = getPlayerName(%obj.ProjectileName);		// Getting Player Name from HP number
	
	if (%col.getType() & $TypeMasks::PlayerObjectType)	// If hit a player type
	{
      %col.kill(%this.directDamage);
		commandToClient(%obj.client, 'CLTHitShotMsg', %name);
		addScore(%obj.ProjectileName);
  	}
	else
	{
		alxPlay(MoogunExplosionSound);
		commandToClient(%obj.client, 'CLTMissShotMsg', %name);
	}
	
	if(CDTime.visible)
	{
		schedule(750, 0, setSchToggleCameraStatic);
	}
}

//-----------------------------------------------------------------------------
// When the player uses the rocket launcher, it will behave like this...
//-----------------------------------------------------------------------------
datablock ShapeBaseImageData( MoogunImage )
{
    shapeFile = "~/data/shapes/moogun/moogun.dts";

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
	stateSound[3]                    = MoogunShootSound;

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
	                             VectorScale(%objectVelocity, %projectile.vellInheritFactor) );

	// Create a new rocket projectile object...
	%p = new (%this.projectileType)()
	{
		dataBlock       = %projectile;
		initialVelocity = %muzzleVelocity;
		initialPosition = %obj.getMuzzlePoint(%slot);
		sourceObject    = %obj;
		sourceSlot      = %slot;
		ProjectileName	 = $ShootingNumber;	// Attach Phone Number to projectile
		client          = %obj.client;
	};
	
	MissionCleanup.add( %p );
	
	if((mCeil(getRandom(0,5)) == 5)&&(CDTime.visible))		// Select wether to use projectile cam (probability = 1:4)
	{
		commandToServer('ToggleProjectileCamera', %p);
	}
	return %p;
}
