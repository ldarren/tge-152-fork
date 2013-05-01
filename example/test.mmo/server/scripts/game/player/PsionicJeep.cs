//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
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


datablock WheeledVehicleTire(PsionicJeepTire0)
{
   shapeFile = "~/data/shapes/vehicles/Psionic/PsionicJeepTire.dts";
   staticFriction = 3.0;
   kineticFriction = 2.0;

   // Spring that generates lateral tire forces
   lateralForce = 6000;
   lateralDamping = 400;
   lateralRelaxation = 1;

   // Spring that generates longitudinal tire forces
   longitudinalForce = 6000;
   longitudinalDamping = 400;
   longitudinalRelaxation = 1;
};

datablock WheeledVehicleSpring(PsionicJeepSpring0)
{
   // Wheel suspension properties
   length = 0.40;             // Suspension travel
   force = 3000;              // Spring force
   damping = 600;             // Spring damping
   antiSwayForce = 3;         // Lateral anti-sway force
};


datablock WheeledVehicleData(PsionicJeep0)
{
    category              = "Vehicles";
    shapeFile             = "~/data/shapes/vehicles/Psionic/PsionicJeep.dts";
    emap                  = false;

    mountable             = true;
    swappable             = false;

    mountPose[0]          = "sitting"; 
    numMountPoints        = 1;

    maxSteeringAngle      = 0.55;  
    integration           = 4; 

    mass                  = 400;
    drag                  = 0.6;
    bodyFriction          = 0.6;
    bodyRestitution       = 0.4;

    engineTorque          = 9000; 
    engineBrake           = 750;  
    brakeTorque           = 2000;  
    maxWheelSpeed         = 55;  

    cameraDefaultFov      = 90.0;
    cameraMinFov          = 45.0;
    cameraMaxFov          = 120.0;

    cameraMinDist         = 50.0;
    cameraMaxDist         = 70.0;

    observeThroughObject  = true; 
    useEyePoint	          = false; // Use the eye point of the player mounted to the vehicle, 
                                   // not the vehicle's eye point

    minLookAngle          = -1.57;
    maxLookAngle          = 1.57;
    maxFreelookAngle      = 2.1; 

    cameraOffset          = 2.5;
   
    tireEmitter   = "PEDVehicleSmoke0";
};


function PsionicJeep0::onAdd(%this,%obj)
{
   for(%i = 2; %i >= 0; %i--) {
    %obj.setWheelTire(%i,PsionicJeepTire0);
    %obj.setWheelSpring(%i,PsionicJeepSpring0);
   }

   for(%i = 3; %i >= 2; %i--) {
    %obj.setWheelTire(%i,PsionicJeepTire0);
    %obj.setWheelSpring(%i,PsionicJeepSpring0);
   }
}
