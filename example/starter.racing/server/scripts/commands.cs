//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Misc. server commands avialable to clients
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function serverCmdToggleCamera(%client)
{
   %control = %client.getControlObject();
   if (%control == %client.car)
   {
      %control = %client.camera;
      %control.mode = toggleCameraFly;
   }
   else
   {
      %control = %client.car;
      %control.mode = observerFly;
   }
   %client.setControlObject(%control);
}

function serverCmdDropPlayerAtCamera(%client)
{
   if ($Server::TestCheats || isObject(EditorGui))
   {
      %client.player.setTransform(%client.camera.getTransform());
      %client.player.setVelocity("0 0 0");
      %client.setControlObject(%client.player);
   }
}

function serverCmdDropCameraAtPlayer(%client)
{
   %client.camera.setTransform(%client.player.getEyeTransform());
   %client.camera.setVelocity("0 0 0");
   %client.setControlObject(%client.camera);
}


//-----------------------------------------------------------------------------

function serverCmdReset(%client)
{
   if (isObject(%client.car))
   {
      %client.car.delete();
      %client.spawnCar();
      %client.setControlObject(%client.car);
      %client.nextCheck = 1;
   }
}