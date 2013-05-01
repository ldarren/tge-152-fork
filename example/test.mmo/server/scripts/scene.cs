//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Functions that implement game-play
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Scene Class
//-----------------------------------------------------------------------------

$Server::CurrentScene = 0;

// Scene activation is driven from the DemoGui and related scripts.
// (see $modName@"/client/scripts/demoGui.cs)
function Scene::activateScene(%number)
{
   if (isObject($Server::CurrentScene))
      $Server::CurrentScene.close();

   $Server::CurrentScene = "MissionGroup/Scenes".getObject(%number);
   echo("Activating Scene #"@%number);
   $Server::CurrentScene.open();
   return $Server::CurrentScene;
}

function Scene::open(%this)
{
   echo("Scene " @ %this.getName() @ " open");
   %client = $Server::Client;

   // Push any scene specific controls
   if (isObject(%this.gui))
      Canvas.pushDialog(%this.gui);
      
   // Set the path to follow, the camera will start at
   // the first node in the path.
   %path = %this.getId() @ "/path";
   if (isObject(%path)) {
      %client.player.reset(0);
      %client.player.followPath(%path);
   }
   else {
      // Check for a static camera
      %start = %this.getId() @ "/start";
      if (isObject(%start)) {
         echo("Static Camera");
         %client.player.reset(0);
         %client.player.pushNode(%start);
         %client.player.popFront();
      }
   }
}

function Scene::getStartPos()
{
   %scene = "MissionGroup/Scenes".getObject(0);
   %start = %scene.getId() @ "/start";
   if (isObject(%start))
      return %start.getTransform();
   return "0 0 100";
}

function Scene::close(%this)
{
   %client = $Server::Client;
   
   // Pop any scene specific controls
   if (isObject(%this.gui))
      Canvas.popDialog(%this.gui);
   
   echo("Scene " @ %this.getName() @ " closed");
}


//-----------------------------------------------------------------------------
// Path Camera
//-----------------------------------------------------------------------------

datablock PathCameraData(LoopingCam)
{
   mode = "";
};

function LoopingCam::onNode(%this,%camera,%node)
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
      %this.speed = 100;
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
