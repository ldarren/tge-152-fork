//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Global movement speed that affects all cameras.
$Camera::movementSpeed = 40;

//-----------------------------------------------------------------------------
// Define a datablock class to use for our observer camera
//-----------------------------------------------------------------------------

datablock CameraData(Observer)
{
   mode = "Observer";
};

//-----------------------------------------------------------------------------
// Path Camera
//-----------------------------------------------------------------------------

datablock PathCameraData(LoopingCamData)
{
   mode = "";
};

function LoopingCamData::onNode(%this,%camera,%node)
{
   if (%node == %camera.loopNode) {
      %camera.pushPath(%camera.path);
      %camera.loopNode += %camera.path.getCount();
   }
}

function PathCamera::followPath(%this,%path)
{
   %this.path = %path;
   if (!(%this.speed = %path.speed))
      %this.speed = 3;
   if (%path.isLooping)
      %this.loopNode = %path.getCount() - 2;
   else
      %this.loopNode = -1;
   
   %this.pushPath(%path);   
   %this.popFront();
}

function PathCamera::pushPath(%this,%path)
{
   for (%i = 0; %i < %path.getCount(); %i++)
      %this.pushNode(%path.getObject(%i));
}

function PathCamera::pushNode(%this,%node)
{
   if (!(%speed = %node.speed))
      %speed = %this.speed;
   if ((%type = %node.type) $= "")
      %type = "Normal";
   if ((%smoothing = %node.smoothing) $= "")
      %smoothing = "Linear";
   %this.pushBack(%node.getTransform(),%speed,%type,%smoothing);
}
