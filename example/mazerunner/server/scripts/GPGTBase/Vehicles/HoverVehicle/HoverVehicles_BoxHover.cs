//-----------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-----------------------------------------------------------------------------

datablock HoverVehicleData(BoxHover)
{
    mountable              = true;
    swappable              = false;
    mountPose[0]           = sitting;
    numMountPoints         = 1;

    category               = "baseVehicles";
    shapeFile              = "~/data/GPGTBase/shapes/vehicles/boxhover/boxhover.dts";
    computeCRC             = false;

    integration            = 4;
    collisionTol           = 0.1;
    contactTol             = 0.1;

    drag                   = 0.0;
    density                = 0.3; // Will float in water

    mass                   = 40;
    bodyFriction           = 0;
    bodyRestitution        = 0.5;

    mainThrustForce        = 1000;
    reverseThrustForce     = 700;
    strafeThrustForce      = 800;

    stabSpringConstant     = 100;
    stabDampingConstant    = 80;

    dragForce              = 10;

    vertFactor             = 1;
    floatingThrustFactor   = 1;

    turboFactor            = 1.5;

    stabLenMin             = 6.50;
    stabLenMax             = 7.25;

    normalForce            = 1000;
    restorativeForce       = 0;

    steeringForce          = 600;
    gyroDrag               = 40;

    rollForce              = 100;
    pitchForce             = 100;

    floatingGravMag        = 1;

    brakingForce           = 600;
    brakingActivationSpeed = 10;
};




