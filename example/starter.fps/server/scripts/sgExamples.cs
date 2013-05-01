//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


// detail material mapping...
addMaterialMapping("Wall_filler101m" , "detail: starter.fps/data/interiors/lightingPack/wall_detail"); 


datablock DecalData(GlowDecal)
{
   sizeX       = 2;
   sizeY       = 2;
   textureName = "starter.fps/data/shapes/lightingPack/Arrow";
   SelfIlluminated = true;
   LifeSpan = 1000000000;
};

//sgCreateDecal(clientgroup.getobject(0).getcontrolobject().getposition(), "", "", "", GlowDecal);
//sgDropDecal(spotlightleft.getposition(), "", "", "", GlowDecal);

datablock ScopeAlwaysShapeData(PipeScopeAlwaysShape)
{
   category = "Pipe";
   className = "PipeClass";
   shapeFile = "~/data/shapes/lightingPack/pipe.dts";
};

//datablock ScopeAlwaysShapeData(SpotSwingScopeAlwaysShape)
//{
//   category = "Lights";
//   className = "SpotSwing";
//   shapeFile = "~/data/shapes/lightingPack/spotswing.dts";
//};

datablock StaticShapeData(SpotSwingStaticShape)
{
   category = "Lights";
   className = "SpotSwing";
   shapeFile = "~/data/shapes/lightingPack/spotswing.dts";
};

function SpotSwing::onAdd(%this, %obj)
{
	%obj.playthread(0, "ambient");
	
   %light = new volumeLight() {
      dataBlock = "sgMountLight";
            rotation = "-0.357694 0.933839 9.9834e-009 180";
            scale = "1 1 1";
            dataBlock = "sgMountLight";
            Enable = "1";
            IconSize = "1";
            ParticleColorAttenuation = "1";
            Texture = "common/lighting/lightFalloffMono.png";
            lpDistance = "0.35";
            ShootDistance = "5";
            Xextent = "0.6";
            Yextent = "0.6";
            SubdivideU = "4";
            SubdivideV = "4";
            FootColour = "1.000000 1.000000 1.000000 0.182000";
            TailColour = "0.000000 0.000000 0.000000 0.000000";
   };
   
   %light.attachtoobject(%obj);
}


datablock ItemData(sgCrossbow)
{
   // Mission editor category
   category = "Weapon";

   // Hook into Item Weapon class hierarchy. The weapon namespace
   // provides common weapon handling functions in addition to hooks
   // into the inventory system.
   className = "Weapon";

   // Basic Item properties
   shapeFile = "~/data/shapes/crossbow/weapon.dts";
   mass = 1;
   elasticity = 0.2;
   friction = 0.6;
   emap = true;

	// Dynamic properties defined by the scripts
	pickUpName = "a crossbow";
	image = sgCrossbowImage;
	
   lightType = ConstantLight;
   lightColor = "0.5 0.5 0.5";
   lightRadius = 4;
};


//--------------------------------------------------------------------------
// Crossbow image which does all the work.  Images do not normally exist in
// the world, they can only be mounted on ShapeBase objects.

datablock ShapeBaseImageData(sgCrossbowImage)
{
   // Basic Item properties
   shapeFile = "~/data/shapes/crossbow/weapon.dts";
   emap = true;

   // Specify mount point & offset for 3rd person, and eye offset
   // for first person rendering.
   mountPoint = 0;
   eyeOffset = "0.1 0.4 -0.6";

   // When firing from a point offset from the eye, muzzle correction
   // will adjust the muzzle vector to point to the eye LOS point.
   // Since this weapon doesn't actually fire from the muzzle point,
   // we need to turn this off.  
   correctMuzzleVector = false;

   // Add the WeaponImage namespace as a parent, WeaponImage namespace
   // provides some hooks into the inventory system.
   className = "WeaponImage";

   // Projectile && Ammo.
   item = sgCrossbow;
   ammo = CrossbowAmmo;
   projectile = CrossbowProjectile;
   projectileType = Projectile;

   // Images have a state system which controls how the animations
   // are run, which sounds are played, script callbacks, etc. This
   // state system is downloaded to the client so that clients can
   // predict state changes and animate accordingly.  The following
   // system supports basic ready->fire->reload transitions as
   // well as a no-ammo->dryfire idle state.

   // Initial start up state
   stateName[0]                     = "Preactivate";
   stateTransitionOnLoaded[0]       = "Activate";
   stateTransitionOnNoAmmo[0]       = "NoAmmo";

   // Activating the gun.  Called when the weapon is first
   // mounted and there is ammo.
   stateName[1]                     = "Activate";
   stateTransitionOnTimeout[1]      = "Ready";
   stateTimeoutValue[1]             = 0.6;
   stateSequence[1]                 = "Activate";

   // Ready to fire, just waiting for the trigger
   stateName[2]                     = "Ready";
   stateTransitionOnNoAmmo[2]       = "NoAmmo";
   stateTransitionOnTriggerDown[2]  = "Fire";

   // Fire the weapon. Calls the fire script which does 
   // the actual work.
   stateName[3]                     = "Fire";
   stateTransitionOnTimeout[3]      = "Reload";
   stateTimeoutValue[3]             = 0.2;
   stateFire[3]                     = true;
   stateRecoil[3]                   = LightRecoil;
   stateAllowImageChange[3]         = false;
   stateSequence[3]                 = "Fire";
   stateScript[3]                   = "onFire";
   stateSound[3]                    = CrossbowFireSound;

   // Play the relead animation, and transition into
   stateName[4]                     = "Reload";
   stateTransitionOnNoAmmo[4]       = "NoAmmo";
   stateTransitionOnTimeout[4]      = "Ready";
   stateTimeoutValue[4]             = 0.8;
   stateAllowImageChange[4]         = false;
   stateSequence[4]                 = "Reload";
   stateEjectShell[4]               = true;
   stateSound[4]                    = CrossbowReloadSound;

   // No ammo in the weapon, just idle until something
   // shows up. Play the dry fire sound if the trigger is
   // pulled.
   stateName[5]                     = "NoAmmo";
   stateTransitionOnAmmo[5]         = "Reload";
   stateSequence[5]                 = "NoAmmo";
   stateTransitionOnTriggerDown[5]  = "DryFire";

   // No ammo dry fire
   stateName[6]                     = "DryFire";
   stateTimeoutValue[6]             = 1.0;
   stateTransitionOnTimeout[6]      = "NoAmmo";
   stateSound[6]                    = CrossbowFireEmptySound;
   
   lightType = ConstantLight;
   lightColor = "0.5 0.5 0.5";
   lightRadius = 4;
};

datablock StaticShapeData(Tree2StaticShape)
{
   category = "Trees";
   className = "Tree2";
   shapeFile = "~/data/shapes/trees/tree2.dts";
};

datablock ScopeAlwaysShapeData(Tree2ScopeAlwaysShape)
{
   category = "Trees";
   className = "Tree2SA";
   shapeFile = "~/data/shapes/trees/tree2.dts";
};

datablock ParticleData(RealFireParticle)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.075;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 0;
   useInvAlpha = false;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;

   colors[0]     = "0.6 0.2 0.0 1.0";
   colors[1]     = "0.6 0.2 0.0 1.0";
   colors[2]     = "0.2 0.0 0.0 0.0";

   sizes[0]      = 0.9;
   sizes[1]      = 0.75;
   sizes[2]      = 0.3;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(RealFireEmitter)
{
   ejectionPeriodMS = 200;
   periodVarianceMS = 0;

   ejectionVelocity = 0.07;
   velocityVariance = 0.00;

   thetaMin         = 1.0;
   thetaMax         = 100.0;  

   particles = "RealFireParticle";
};

datablock ParticleData(RealFireSmallParticle)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.05;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 0;
   useInvAlpha = false;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;

   colors[0]     = "0.6 0.2 0.0 1.0";
   colors[1]     = "0.6 0.2 0.0 1.0";
   colors[2]     = "0.2 0.0 0.0 0.0";

   sizes[0]      = 0.6;
   sizes[1]      = 0.5;
   sizes[2]      = 0.1;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(RealFireSmallEmitter)
{
   ejectionPeriodMS = 200;
   periodVarianceMS = 0;

   ejectionVelocity = 0.07;
   velocityVariance = 0.00;

   thetaMin         = 1.0;
   thetaMax         = 100.0;  

   particles = "RealFireSmallParticle";
};

datablock ParticleData(RealFireBigParticle)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.15;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 0;
   useInvAlpha = false;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;

   colors[0]     = "0.6 0.2 0.0 1.0";
   colors[1]     = "0.6 0.2 0.0 1.0";
   colors[2]     = "0.2 0.0 0.0 0.0";

   sizes[0]      = 2.0;
   sizes[1]      = 1.6;
   sizes[2]      = 0.75;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(RealFireBigEmitter)
{
   ejectionPeriodMS = 200;
   periodVarianceMS = 0;

   ejectionVelocity = 0.07;
   velocityVariance = 0.00;

   thetaMin         = 2.0;
   thetaMax         = 200.0;  

   particles = "RealFireBigParticle";
};

datablock ParticleEmitterNodeData(RealFireNode)
{
   timeMultiple = 1;
};

datablock ParticleData(SparkParticle)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient     = 4.0;
   gravityCoefficient   = 0.2;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 1000;
   lifetimeVarianceMS   = 750;
   useInvAlpha = false;
   spinRandomMin = 0.0;
   spinRandomMax = 0.0;

   colors[0]     = "0.6 0.6 0.6 1.0";
   colors[1]     = "0.6 0.6 0.6 1.0";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 0.2;
   sizes[1]      = 0.15;
   sizes[2]      = 0.1;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(SparkEmitter)
{
   ejectionPeriodMS = 200;
   periodVarianceMS = 100;

   ejectionVelocity = 1.50;
   velocityVariance = 0.50;

   thetaMin         = 0.0;
   thetaMax         = 0.0;  

   particles = "SparkParticle";
};

datablock ParticleEmitterNodeData(SparkNode)
{
   timeMultiple = 1;
};

datablock ParticleData(TriggeredFireParticle)
{
   textureName          = "~/data/shapes/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.075;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 0;
   useInvAlpha = false;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;

   colors[0]     = "0.0 0.0 0.0 1.0";
   colors[1]     = "0.0 0.0 0.0 1.0";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 0.9;
   sizes[1]      = 0.75;
   sizes[2]      = 0.3;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(TriggeredFireEmitter)
{
   ejectionPeriodMS = 500;
   periodVarianceMS = 0;

   ejectionVelocity = 0.07;
   velocityVariance = 0.00;

   thetaMin         = 1.0;
   thetaMax         = 100.0;  

   particles = "TriggeredFireParticle";
};

datablock TriggerData(sgParticleEmitterTriggerDataBlock)
{
   // The period is value is used to control how often the console
   // onTriggerTick callback is called while there are any objects
   // in the trigger.  The default value is 100 MS.
   tickPeriodMS = 100;
};

function sgParticleEmitterTriggerDataBlock::sgResetTriggeredFireEmitter()
{
   TriggeredFireParticle.colors[0] = "0.0 0.0 0.0 1.0";
   TriggeredFireParticle.colors[1] = "0.0 0.0 0.0 1.0";
   TriggeredFireEmitter.ejectionPeriodMS = 750;
}

function sgParticleEmitterTriggerDataBlock::onEnterTrigger(%this,%trigger,%obj)
{

   TriggeredFireParticle.colors[0] = "0.6 0.2 0.0 1.0";
   TriggeredFireParticle.colors[1] = "0.6 0.2 0.0 1.0";
   TriggeredFireEmitter.ejectionPeriodMS = 200;
   Parent::onEnterTrigger(%this,%trigger,%obj);
}

function sgParticleEmitterTriggerDataBlock::onLeaveTrigger(%this,%trigger,%obj)
{
   TriggeredFireEmitter.ejectionPeriodMS = 3500;
   Parent::onLeaveTrigger(%this,%trigger,%obj);
   %this.schedule(3000, "sgResetTriggeredFireEmitter");
}

datablock ParticleData(SteamParticle)
{
  animateTexture = "0";
  animTexName[0] = "starter.fps/data/shapes/particles/smoke";
  colors[0] = "1.000000 1.000000 1.000000 0.551180";
  colors[1] = "1.000000 1.000000 1.000000 0.2055118";
  colors[2] = "1.000000 1.000000 1.000000 0.000000";
  colors[3] = "1.000000 1.000000 1.000000 1.000000";
  constantAcceleration = "0.1";
  dragCoefficient = "0.44";
  framesPerSec = "1";
  gravityCoefficient = "0.0";
  inheritedVelFactor = "0";
  lifetimeMS = "3022";
  lifetimeVarianceMS = "320";
  allowLighting = "1";
  sizes[0] = "0.6";
  sizes[1] = "0.74";
  sizes[2] = "1.49";
  sizes[3] = "1";
  spinRandomMax = "35";
  spinRandomMin = "-100";
  spinSpeed = "0.29";
  textureName = "starter.fps/data/shapes/particles/smoke";
  times[0] = "0";
  times[1] = "0.49";
  times[2] = "1";
  times[3] = "1";
  useInvAlpha = "1";
  windCoefficient = "1";
};

datablock ParticleEmitterData(SteamEmitter)
{
  className = "ParticleEmitterData";
  ejectionOffset = "0.09";
  ejectionPeriodMS = "40";
  ejectionVelocity = "2";
  lifetimeMS = "0";
  lifetimeVarianceMS = "0";
  orientOnVelocity = "1";
  orientParticles = "0";
  overrideAdvance = "0";
  particles = "SteamParticle";
  periodVarianceMS = "10";
  phiReferenceVel = "0";
  phiVariance = "360";
  thetaMax = "15.0";
  thetaMin = "0";
  useEmitterColors = "0";
  useEmitterSizes = "0";
  velocityVariance = "0";
};

datablock ParticleEmitterNodeData(SteamEmitterNode)
{
   timeMultiple = 1;
};

//-----------------------------------------------------------------------------
