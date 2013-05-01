//-----------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
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
// Note: Some values derived from BadGuy's example
//

datablock WheeledVehicleTire( BoxCarTire )
{
   shapeFile              = "~/data/GPGTBase/shapes/vehicles/boxwheeled/boxcartire.dts";

   staticFriction         = 10.0;
   kineticFriction        = 6.5;

   longitudinalForce      = 6000;
   longitudinalDamping    = 400;
   longitudinalRelaxation = 1;

   lateralForce           = 6000;
   lateralDamping         = 400;
   lateralRelaxation      = 1;
};

datablock WheeledVehicleSpring( BoxCarSpring )
{
   force         = 3000;
   damping       = 600;
   antiSwayForce = 1000; 

   length        = 1;
};


datablock WheeledVehicleData( BoxCar )
{
   category              = "baseVehicles";
   shapeFile             = "~/data/GPGTBase/shapes/vehicles/boxwheeled/boxcar.dts";
   emap                  = false;

   mountable             = true;
   swappable             = false;

   mountPose[0]          = "sitting"; 
   numMountPoints        = 1;

   maxSteeringAngle      = 0.785;  
   integration           = 4; 

   mass                  = 200;
   drag                  = 0.6;
   bodyFriction          = 0.6;
   bodyRestitution       = 0.4;

   engineTorque          = 5000; 
   engineBrake           = 500;  
   brakeTorque           = 2000;  
   maxWheelSpeed         = 55;  

   cameraDefaultFov      = 90.0;
   cameraMinFov          = 45.0;
   cameraMaxFov          = 120.0;

   cameraMinDist         = 45.0;
   cameraMaxDist         = 60.0;

   observeThroughObject  = true; 
   useEyePoint	          = false; 

   minLookAngle          = -1.57;
   maxLookAngle          = 1.57;
   maxFreelookAngle      = 2.1; 

   cameraOffset          = 5.0;


   minImpactSpeed				= 1;
   groundImpactMinSpeed		= 100;
   groundImpactShakeAmp		= "1.0 1.0 1.0";
   groundImpactShakeDuration	= 0.1;
   groundImpactShakeFalloff	= 10.0;
   groundImpactShakeFreq		= "4.0 4.0 4.0";
};
