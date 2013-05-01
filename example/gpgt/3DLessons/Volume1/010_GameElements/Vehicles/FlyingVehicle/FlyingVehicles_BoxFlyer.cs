//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written, modfied, or otherwise interpreted by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
 //-----------------------------------------------------------------------------

datablock FlyingVehicleData(BoxFlyer)  
{
   category               = "baseVehicles";
   shapeFile              = "~/data/GPGTBase/shapes/vehicles/boxflying/boxflyer.dts";

   renderWhenDestroyed    = false;

   cameraRoll             = false; // Roll the camera with the vehicle
   cameraMaxDist          = 30;    // Far distance from vehicle
   cameraOffset           = 8;     // Vertical offset from camera mount point
   cameraLag              = 0.1;   // Velocity lag of camera
   cameraDecay            = 0.75;  // Decay per sec. rate of velocity lag

   observeThroughObject   = true; 
   useEyePoint            = true;  // Use the eye point of the player mounted to the vehicle, 
                                   // not the vehicle's eye point
   drag                   = 0.0;
   density                = 0.4;

   MinDrag                = 88;    // Linear Drag (eventually slows you down when not thrusting...constant drag)
   RotationalDrag         = 10;    // Anguler Drag (dampens the drift after you stop moving the mouse...also tumble drag)
   RechargeRate           = 0.0;
   
   minDrag                = 85;    // Linear Drag (eventually slows you down when not thrusting...constant drag)

   rotationalDrag         = 15;    // Anguler Drag (dampens the drift after you stop moving the mouse...also tumble drag)

   rechargeRate           = 50.0;

   maxAutoSpeed           = 15000; // Autostabilizer kicks in when less than this speed. (world units/second)
   autoAngularForce       = 1000;   // Angular stabilizer force (this force levels you out when autostabilizer kicks in)
   autoLinearForce        = 0;     // Linear stabilzer force (this slows you down when autostabilizer kicks in)
   autoInputDamping       = 0.95;  // Dampen control input so you don't` whack out at very slow speeds

   // Maneuvering
   maxSteeringAngle       = 1.2;   // Max radians you can rotate the wheel. Smaller number is more maneuverable.
   horizontalSurfaceForce = 2400;  // Horizontal center "wing" (provides "bite" into the wind for climbing/diving and turning)
   verticalSurfaceForce   = 4000;  // Vertical center "wing" (controls side slip. lower numbers make MORE slide.)
   maneuveringForce       = 24000; // Horizontal jets (W,S,D,A key thrust)

   steeringForce          = 2400;  // Steering jets (force applied when you move the mouse)

   steeringRollForce      = 400;   // Steering jets (how much you heel over when you turn)
 
   rollForce              = 100;   // Auto-roll (self-correction to right you after you roll/invert)
   hoverHeight            = 2.5;   // Height off the ground at rest
   createHoverHeight      = 2.5;   // Height off the ground when created

   gravityMod             = 1;

   integration            = 60;    // Physics integration: TickSec/Rate
   collisionTol           = 0.1;   // Collision distance tolerance
   contactTol             = 0.1;   // Contact velocity tolerance

   mass                   = 500;   // Mass of the vehicle

   bodyFriction           = 0;     // Don't mess with this.
   bodyRestitution        = 0.6;   // When you hit the ground, how much you rebound. (between 0 and 1)
   minRollSpeed           = 0;     // Don't mess with this.
   minImpactSpeed         = 1;     // If hit ground at speed above this then it's an impact. world units/second
   softImpactSpeed        = 10;    // Sound hooks. This is the soft hit.
   hardImpactSpeed        = 25;    // Sound hooks. This is the hard hit.

   collDamageThresholdVel = 20.0;
   collDamageMultiplier   = 0.02;

   mountable = true;
};


