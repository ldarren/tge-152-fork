//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// Information extacted from the shape.
//
// Wheel Sequences
//    spring#        Wheel spring motion: time 0 = wheel fully extended,
//                   the hub must be displaced, but not directly animated
//                   as it will be rotated in code.
// Other Sequences
//    steering       Wheel steering: time 0 = full right, 0.5 = center
//    breakLight     Break light, time 0 = off, 1 = breaking
//
// Wheel Nodes
//    hub#           Wheel hub, the hub must be in it's upper position
//                   from which the springs are mounted.
//
// The steering and animation sequences are optional.
// The center of the shape acts as the center of mass for the car.

//-----------------------------------------------------------------------------
datablock AudioProfile(buggyEngineSound)
{
   filename    = "~/data/sound/vehicles/buggy/engine_idle.wav";
   description = AudioClosestLooping3d;
   preload = true;
};

datablock ParticleData(TireParticle)
{
   textureName          = "~/data/shapes/buggy/dustParticle";
   dragCoefficient      = 2.0;
   gravityCoefficient   = -0.1;
   inheritedVelFactor   = 0.1;
   constantAcceleration = 0.0;
   lifetimeMS           = 1000;
   lifetimeVarianceMS   = 400;
   colors[0]     = "0.46 0.36 0.26 1.0";
   colors[1]     = "0.46 0.46 0.36 0.0";
   sizes[0]      = 1.0;
   sizes[1]      = 4.0;
};

datablock ParticleEmitterData(TireEmitter)
{
   ejectionPeriodMS = 20;
   periodVarianceMS = 10;
   ejectionVelocity = 1;
   velocityVariance = 1.0;
   ejectionOffset   = 0.0;
   thetaMin         = 0;
   thetaMax         = 60;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance = false;
   particles = "TireParticle";
};


//----------------------------------------------------------------------------

datablock WheeledVehicleTire(DefaultCarTire)
{
   // Tires act as springs and generate lateral and longitudinal
   // forces to move the vehicle. These distortion/spring forces
   // are what convert wheel angular velocity into forces that
   // act on the rigid body.
   shapeFile = "~/data/shapes/buggy/wheel.dts";
   staticFriction = 4.2;
   kineticFriction = 3.15;

   // Spring that generates lateral tire forces
   lateralForce = 18000;
   lateralDamping = 6000;
   lateralRelaxation = 1;

   // Spring that generates longitudinal tire forces
   longitudinalForce = 18000;
   longitudinalDamping = 4000;
   longitudinalRelaxation = 1;
};

datablock WheeledVehicleSpring(DefaultCarSpring)
{
   // Wheel suspension properties
   length = 0.85;             // Suspension travel
   force = 2800;              // Spring force
   damping = 3600;             // Spring damping
   antiSwayForce = 3;         // Lateral anti-sway force
};

datablock WheeledVehicleData(DefaultCar)
{
   category = "Vehicles";
   shapeFile = "~/data/shapes/buggy/buggy.dts";
   emap = true;

   maxDamage = 1.0;
   destroyedLevel = 0.5;

   maxSteeringAngle = 0.385;  // Maximum steering angle, should match animation
   tireEmitter = TireEmitter; // All the tires use the same dust emitter

   // 3rd person camera settings
   cameraRoll = true;         // Roll the camera with the vehicle
   cameraMaxDist = 4.8;         // Far distance from vehicle
   cameraOffset = 1.5;        // Vertical offset from camera mount point
   cameraLag = 0.26;           // Velocity lag of camera
   cameraDecay = 1.25;        // Decay per sec. rate of velocity lag

   // Rigid Body
   mass = 380;
   massCenter = "0 -0.2 0";    // Center of mass for rigid body
   massBox = "0 0 0";         // Size of box used for moment of inertia,
                              // if zero it defaults to object bounding box
   drag = 0.6;                // Drag coefficient
   bodyFriction = 0.6;
   bodyRestitution = 0.4;
   minImpactSpeed = 5;        // Impacts over this invoke the script callback
   softImpactSpeed = 5;       // Play SoftImpact Sound
   hardImpactSpeed = 15;      // Play HardImpact Sound
   integration = 8;           // Physics integration: TickSec/Rate
   collisionTol = 0.1;        // Collision distance tolerance
   contactTol = 0.1;          // Contact velocity tolerance

   // Engine
   engineTorque = 3300;       // Engine power
   engineBrake = 600;         // Braking when throttle is 0
   brakeTorque = 8000;        // When brakes are applied
   maxWheelSpeed = 50;        // Engine scale by current speed / max speed

   // Energy
   maxEnergy = 100;
   jetForce = 3000;
   minJetEnergy = 30;
   jetEnergyDrain = 2;

   // Sounds
//   jetSound = ScoutThrustSound;
//   engineSound = BuggyEngineSound;
//   squealSound = ScoutSquealSound;
//   softImpactSound = SoftImpactSound;
//   hardImpactSound = HardImpactSound;
//   wheelImpactSound = WheelImpactSound;

//   explosion = VehicleExplosion;
};


//-----------------------------------------------------------------------------

function WheeledVehicleData::create(%block)
{
   %obj = new WheeledVehicle() {
      dataBlock = %block;
   };
   return(%obj);
}

//-----------------------------------------------------------------------------

function WheeledVehicleData::onAdd(%this,%obj)
{
   // Setup the car with some defaults tires & springs
   for (%i = %obj.getWheelCount() - 1; %i >= 0; %i--) {
      %obj.setWheelTire(%i,DefaultCarTire);
      %obj.setWheelSpring(%i,DefaultCarSpring);
      %obj.setWheelPowered(%i,false);
   }
   
   // Steer front tires
   %obj.setWheelSteering(0,1);
   %obj.setWheelSteering(1,1);

   // Only power the two rear wheels...
   %obj.setWheelPowered(2,true);
   %obj.setWheelPowered(3,true);
}

function WheeledVehicleData::onCollision(%this,%obj,%col,%vec,%speed)
{
   // Collision with other objects, including items
}   
