//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Global movement speed that affects all cameras.  This should be moved
// into the camera datablock.
$Camera::movementSpeed = 40;

//-----------------------------------------------------------------------------
// Define a datablock class to use for our observer camera
//-----------------------------------------------------------------------------

datablock CameraData(Observer)
{
   mode = "Observer";
};

datablock AdvancedCameraData(Adv3rdCameraData)
{   
	lookAtOffset = "0 0 2";
	thirdPersonOffset = "0 -3 3";
	godViewOffset = "0 -20 20";
	maxTerrainDiff = 2;
	orbitMinMaxZoom = "20 100";	// for orbitmode only
	orbitMinMaxDeclination = "10 80";
	damping = 0.25;
};

datablock AdvancedCameraData(AdvOrbitCameraData)
{   
    lookAtOffset = "0 0 0";   // applicable to thrid person, orbit (and all?)
    thirdPersonOffset = "0 -3 3";   
    godViewOffset = "45 45 45";   
    maxTerrainDiff = 2;   
    orbitMinMaxZoom = "2 20";   	// for orbitmode only
    orbitMinMaxDeclination = "15 75";   
    damping = 0.25;
};
//-----------------------------------------------------------------------------

function Observer::onTrigger(%this,%obj,%trigger,%state)
{
   // state = 0 means that a trigger key was released
   if (%state == 0)
      return;

   // Default player triggers: 0=fire 1=altFire 2=jump
   %client = %obj.getControllingClient();
   switch$ (%obj.mode)
   {
      case "Observer":
         // Do something interesting.

      case "Corpse":
         // Viewing dead corpse, so we probably want to respawn.
         %client.spawnPlayer();

         // Set the camera back into observer mode, since in
         // debug mode we like to switch to it.
         %this.setMode(%obj,"Observer");
   }
}

function Observer::setMode(%this,%obj,%mode,%arg1,%arg2,%arg3)
{
   switch$ (%mode)
   {
      case "Observer":
         // Let the player fly around
         %obj.setFlyMode();

      case "Corpse":
         // Lock the camera down in orbit around the corpse,
         // which should be arg1
         %transform = %arg1.getTransform();
         %obj.setOrbitMode(%arg1, %transform, 0.5, 4.5, 4.5);

   }
   %obj.mode = %mode;
}


//-----------------------------------------------------------------------------
// Camera methods
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function Camera::onAdd(%this,%obj)
{
   // Default start mode
   %this.setMode(%this.mode);
}

function Camera::setMode(%this,%mode,%arg1,%arg2,%arg3)
{
   // Punt this one over to our datablock
   %this.getDatablock().setMode(%this,%mode,%arg1,%arg2,%arg3);
}
