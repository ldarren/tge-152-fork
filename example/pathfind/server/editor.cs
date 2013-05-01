//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// This file contains shape declarations and functions used by the mission
// editor. The mission editor invokes datablock create methods when it
// wants to create an object of that type.

//-----------------------------------------------------------------------------
// Declare a static shape create method.  This allows DTS objects to be loaded
// using the StaticShape simulation object.

function StaticShapeData::create(%data)
{
   %obj = new StaticShape() {
      dataBlock = %data;
   };
   return %obj;
}


//-----------------------------------------------------------------------------
// Declare two marker objects used to place waypoints

datablock MissionMarkerData(WayPointMarker)
{
   category = "Misc";
   shapeFile = "~/data/shapes/markers/octahedron.dts";
};

datablock MissionMarkerData(SpawnSphereMarker)
{
   category = "Misc";
   shapeFile = "~/data/shapes/markers/octahedron.dts";
};

function MissionMarkerData::create(%block)
{
   switch$(%block)
   {
      case "WayPointMarker":
         %obj = new WayPoint() {
            dataBlock = %block;
         };
         return(%obj);
      case "SpawnSphereMarker":
         %obj = new SpawnSphere() {
            datablock = %block;
         };
         return(%obj);
   }
   return(-1);
}


//-----------------------------------------------------------------------------
// Misc. server commands used for editing
//-----------------------------------------------------------------------------

function serverCmdToggleCamera(%client)
{
   if ($Server::ServerType $= "SinglePlayer") {
      %control = %client.getControlObject();
      if (%control == %client.player) {
         %control = %client.camera;
         %control.mode = toggleCameraFly;
      }
      else {
         %control = %client.player;
         %control.mode = observerFly;
      }
      %client.setControlObject(%control);
   }
}

function serverCmdDropPlayerAtCamera(%client)
{
   if (!%client.player.isMounted())
      %client.player.setTransform(%client.camera.getTransform());
   %client.player.setVelocity("0 0 0");
   %client.setControlObject(%client.player);
}

function serverCmdDropCameraAtPlayer(%client)
{
   %client.camera.setTransform(%client.player.getEyeTransform());
   %client.camera.setVelocity("0 0 0");
   %client.setControlObject(%client.camera);
}


function dropFreakinCameraAtPlayer()
{
   $dropcameracount++;
   %cl = ClientGroup.getObject(0);
   if (%cl.camera) dropCameraAtPlayer(1);
   else if ($dropcameracount<100) schedule(100,0,dropFreakinCameraAtPlayer);
}

