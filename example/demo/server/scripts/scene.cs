//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Functions that implement game-play
//-----------------------------------------------------------------------------

package DemoSceneGame {

//-----------------------------------------------------------------------------

function GameConnection::onClientEnterGame(%this)
{
   commandToClient(%this,'SyncClock', $Sim::Time - $Game::StartTime);
   commandToClient(%this,'SetGameGUI',"SceneGui");

   // Create a new camera object.
   %this.camera = new Camera() {
      dataBlock = Observer;
   };
   MissionCleanup.add( %this.camera );
   %this.camera.scopeToClient(%this);
   
   // Create path camera
   %this.player = new PathCamera() {
      dataBlock = LoopingCam;
      position = Scene::getStartPos();
   };
   MissionCleanup.add( %this.player );
   %this.player.scopeToClient(%this);

   // Start up the room gui system
   $Server::Client = %this;
   %this.setControlObject(%this.player);
   SceneGui.setSceneNumber(0);
   
   // Force detail settings
   $Pref::TS::detailAdjust = 1;
   $Pref::TS::screenError = 10;
   
   // Start NPC orcs
   newNPCOrc("MissionGroup/Paths/OrcNPC1",0.75);   
   newNPCOrc("MissionGroup/Paths/OrcNPC2",0.75);   
   newNPCOrc("MissionGroup/Paths/OrcNPC3",0.40);   
}

//-----------------------------------------------------------------------------

function GameConnection::createPlayer(%this, %spawnPoint)
{
}


//-----------------------------------------------------------------------------

function serverCmdSuicide(%client)
{
   if (isObject(%client.player)) {
      %client.player.delete();
      %client.player = 0;
   }
   %client.spawnPlayer();
}

};


//-----------------------------------------------------------------------------
// Scene Class
//-----------------------------------------------------------------------------

$Server::CurrentScene = 0;

// Scene activation is driven from the DemoGui and related scripts.
// (see demo/client/scripts/demoGui.cs)
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
// Orc Animation
//-----------------------------------------------------------------------------

function newNPCOrc(%path,%speed)
{
   %orc = AIPlayer::spawnOnPath("NPC",%path);
   %orc.setMoveSpeed(%speed);
   %orc.mountImage(CrossbowImage,0);
   %orc.pushTask("followPath(\""@%path@"\",-1)");
}


function newAnimationOrc()
{
   if (isObject($AnimationOrc))
      $AnimationOrc.clearTasks();
   else {
      $AnimationOrc = AIPlayer::spawnOnPath("Tarkof","MissionGroup/Paths/OrcAnimation");
      $AnimationOrc.setMoveSpeed(0.75);
      $AnimationOrc.mountImage(CrossbowImage,0);
      $AnimationOrc.setInventory(CrossbowAmmo,1000);
   }
   $AnimationOrc.pushTask("followPath(\"MissionGroup/Paths/OrcAnimation\",100)");
}

function deleteAnimationOrc()
{
   if (isObject($AnimationOrc)) {
      $AnimationOrc.clearTasks();
      $AnimationOrc.pushTask("followPath(\"MissionGroup/Paths/OrcAnimation\",0)");
      $AnimationOrc.pushTask("done()");
   }
}

function AnimationOrcPlay(%anim)
{
   $AnimationOrc.clearTasks();
   $AnimationOrc.pushTask("animate(\""@%anim@"\")");
}

function newDancingOrc()
{
   if (isObject($DancingOrc))
      $DancingOrc.clearTasks();
   else {
      $DancingOrc = AIPlayer::spawnOnPath("Dance","MissionGroup/Paths/OrcDance");
      $DancingOrc.setMoveSpeed(0.75);
   }
   $DancingOrc.pushTask("followPath(\"MissionGroup/Paths/OrcDance\",100)");
   $DancingOrc.pushTask("animate(\"dance\")");
}

function deleteDancingOrc()
{
   if (isObject($DancingOrc)) {
      $DancingOrc.clearTasks();
      $DancingOrc.pushTask("followPath(\"MissionGroup/Paths/OrcDance\",0)");
      $DancingOrc.pushTask("done()");
   }
}

function newDetailOrc()
{
   if (isObject($DetailOrc)) {
      $DetailOrc.clearTasks();
      $DetailOrc.unmountImage(0);
      $DetailOrc.unmountImage(1);
      $DetailOrc.setArmThread("looknw");
   }
   else {
      $DetailOrc = AIPlayer::spawnOnPath("Detail","MissionGroup/Paths/OrcDetail");
      $DetailOrc.setMoveSpeed(0.75);
      $DetailOrc.setArmThread("looknw");
   }
   $DetailOrc.pushTask("followPath(\"MissionGroup/Paths/OrcDetail\",100)");
}

function deleteDetailOrc()
{
   if (isObject($DetailOrc)) {
      $DetailOrc.clearTasks();
      $DetailOrc.pushTask("followPath(\"MissionGroup/Paths/OrcDetail\",0)");
      $DetailOrc.pushTask("done()");
   }
}

function mountDetailOrc(%object,%slot)
{
   if ($DetailOrc.getMountedImage(%slot) == %object.getId()) {
      $DetailOrc.unmountImage(%slot);
      if (%slot == 0)
         $DetailOrc.setArmThread("looknw");
   }
   else {
      $DetailOrc.mountImage(%object,%slot);
      if (%slot == 0)
         $DetailOrc.setArmThread("look");
   }
}

function newShootingOrc()
{
   if (isObject($ShootingOrc))
      $ShootingOrc.clearTasks();
   else {
      $ShootingOrc = AIPlayer::spawnOnPath("Detail","MissionGroup/Paths/OrcShooting");
      $ShootingOrc.mountImage(CrossbowImage,0);
      $ShootingOrc.setInventory(CrossbowAmmo,500000);
      $ShootingOrc.setMoveSpeed(0.75);
   }
   $ShootingOrc.pushTask("followPath(\"MissionGroup/Paths/OrcShooting\",100)");
   $ShootingOrc.pushTask("aimAt(\"MissionGroup/Scenes/ProjectileScene/target\")");
   $ShootingOrc.pushTask("wait(1)");   
   $ShootingOrc.pushTask("fire(true)");
}

function deleteShootingOrc()
{
   if (isObject($ShootingOrc)) {
      $ShootingOrc.clearTasks();
      $ShootingOrc.pushTask("fire(false)");
      $ShootingOrc.pushTask("followPath(\"MissionGroup/Paths/OrcShooting\",0)");
      $ShootingOrc.pushTask("done()");
   }
}


//-----------------------------------------------------------------------------
// Environment effects
//-----------------------------------------------------------------------------

function startRain()
{
   if (!isObject($Rain))
      $Rain = new Precipitation() {
          dataBlock = "HeavyRain";
      minSpeed = "10";
      maxSpeed = "15";
      minMass = "0.05";
      maxMass = "5";
      maxTurbulence = "0.1";
      turbulenceSpeed = "0.2";
      rotateWithCamVel = "1";
      useTurbulence = "1";
      numDrops = "500";
      boxWidth = "30";
      boxHeight = "50";
      doCollision = "1";

      };
if (!isObject($Rain2))
      $Rain2 = new Precipitation() {
         dataBlock = "HeavyRain3";
      minSpeed = "0.3";
      maxSpeed = "0.6";
      minMass = "0.01";
      maxMass = "1";
      maxTurbulence = "0.05";
      turbulenceSpeed = "0.15";
      rotateWithCamVel = "1";
      useTurbulence = "1";
      numDrops = "200";
      boxWidth = "65";
      boxHeight = "100";
      doCollision = "0";
      };

}

function startLeaves()
{
   if (!isObject($Leaves))
   {
      $Leaves = new Precipitation() {
         dataBlock = "Leaves";
         minSpeed = "0.05";
         maxSpeed = "0.1";
         minMass = "0.1";
         maxMass = "0.3";
         maxTurbulence = "1";
         turbulenceSpeed = "0.02";
         rotateWithCamVel = "0";
         useTurbulence = "1";
         numDrops = "100";
         boxWidth = "70";
         boxHeight = "100";
         doCollision = "0";
      };
      
      MissionCleanup.add($Leaves);
   }
}

function stopLeaves()
{
   if (isObject($Leaves))
      $Leaves.delete();
   $Leaves = "";
}

function stopRain()
{
   if (isObject($Rain))
      $Rain.delete();
   $Rain = "";

if (isObject($Rain2))
      $Rain2.delete();
   $Rain2 = "";

}

function startLightning()
{
   if (!isObject($Lightning))
      $Lightning = new Lightning() {
         position = "350 300 180";
         scale = "250 400 500";
         dataBlock = "LightningStorm";
         strikesPerMinute = "50";
         strikeWidth = "2.5";
         chanceToHitTarget = "100";
         strikeRadius = "50";
         boltStartRadius = "20";
         color = "1.000000 1.000000 1.000000 1.000000";
         fadeColor = "0.400000 0.400000 0.100000 1.000000";
         useFog = "1";
         locked = "false";
      };
}

function stopLightning()
{
   if (isObject($Lightning))
      $Lightning.delete();
   $Lightning = "";
}


//-----------------------------------------------------------------------------
// Scenes
//-----------------------------------------------------------------------------
function WelcomeScene::open(%this)
{
   Parent::open(%this);
   startLeaves();
}

function TerrainEngineScene::open(%this)
{
   Parent::open(%this);
   stopLeaves();
}

function SkyScene::open(%this)
{
   Parent::open(%this);
   startLeaves();
}

function BakedShadowsScene::open(%this)
{
   Parent::open(%this);
   startLeaves();
}

function InteriorSpaces::open(%this)
{
   Parent::open(%this);
   newAnimationOrc();
   stopLeaves();
}

function InteriorSpaces::close(%this)
{
   Parent::close(%this);
   deleteAnimationOrc();
}

function OrcAnimationScene::open(%this)
{
   Parent::open(%this);
   newAnimationOrc();
   deleteDancingOrc();
   $AnimationOrc.pushTask("wait(0.5)");
   $AnimationOrc.pushTask("animate(\"celwave\")");
}

function OrcDanceScene::open(%this)
{
   Parent::open(%this);
   deleteAnimationOrc();
   newDancingOrc();
}

function OrcShadowScene::open(%this)
{
   Parent::open(%this);
   newDancingOrc();
   deleteDetailOrc();
}

function OrcDetailScene::open(%this)
{
   Parent::open(%this);
   deleteDancingOrc();
   newDetailOrc();
   %this.detailSetting = $Pref::TS::detailAdjust;
   %this.screenError = $pref::TS::screenError; 
   DetailSceneSlider.setValue($Pref::TS::detailAdjust);
}

function OrcDetailScene::close(%this)
{
   $Pref::TS::detailAdjust = %this.detailSetting;
   $Pref::TS::detailAdjust = %this.screenError;
   DetailSceneSlider.setValue(%this.detailSetting);
   Parent::close(%this);
}

function OrcDetailScene::slider(%this)
{
   $Pref::TS::detailAdjust = DetailSceneSlider.getValue();
   $Pref::TS::screenError = 1 + 35 * (1 - $Pref::TS::detailAdjust); 
}

function OrcMountingScene::open(%this)
{
   Parent::open(%this);
   newDetailOrc();
   deleteShootingOrc();
   stopLeaves();
}

function ProjectileScene::open(%this)
{
   Parent::open(%this);
   deleteDetailOrc();
   newShootingOrc();
   startLeaves();
}

function ExplosionScene::open(%this)
{
   Parent::open(%this);
   newShootingOrc();
   startLeaves();
}

function FXLightScene::open(%this)
{
   Parent::open(%this);
   deleteShootingOrc();
   stopLeaves();
}

function MirrorScene::open(%this)
{
   Parent::open(%this);
   stopLightning();
   stopLeaves();
   stopRain();
   Sky.setFieldValue(fogDistance, "500");
   Sky.setFieldValue(fogColor, "0.75 0.75 0.75 0.5");
   Sky.setFieldValue(useSkyTextures, "1");
   Sky.setFieldValue(SkySolidColor, "0.64 0.148 0.215 0");
   Sky.setFieldValue(WindVelocity, "0.01 0.01 0");
   Sky.setFieldValue(VisibleDistance, "800");
   Sky.applySkyChanges();
}

function PrecipitationScene::open(%this)
{
   Parent::open(%this);
   startLeaves();
   startRain();
   Sky.setFieldValue(fogDistance, "0");
   Sky.setFieldValue(fogColor, "0.45 0.45 0.45 1");
   Sky.setFieldValue(useSkyTextures, "0");
   Sky.setFieldValue(SkySolidColor, "1 1 1 1");
   Sky.setFieldValue(WindVelocity, "0.11 0.11 0");
   Sky.setFieldValue(VisibleDistance, "300");
   Sky.applySkyChanges();
   stopLightning();
}

function LightningScene::open(%this)
{
   Parent::open(%this);
   startLightning();
   Sky.stormFog(0,3);
   startRain();
   Sky.setFieldValue(fogDistance, "0");
   Sky.setFieldValue(fogColor, "0.45 0.45 0.45 1");
   Sky.setFieldValue(useSkyTextures, "0");
   Sky.setFieldValue(SkySolidColor, "1 1 1 1");
   Sky.setFieldValue(WindVelocity, "0.11 0.11 0");
   Sky.setFieldValue(VisibleDistance, "300");
   Sky.applySkyChanges();
}

function WaterScene::open(%this)
{
   stopLeaves();
   Parent::open(%this);
   stopLightning();
   stopRain();
   Sky.setFieldValue(fogDistance, "500");
   Sky.setFieldValue(fogColor, "0.75 0.75 0.75 0.5");
   Sky.setFieldValue(useSkyTextures, "1");
   Sky.setFieldValue(SkySolidColor, "0.64 0.148 0.215 0");
   Sky.setFieldValue(WindVelocity, "0.01 0.01 0");
   Sky.setFieldValue(VisibleDistance, "800");
   Sky.applySkyChanges();
    
}

function FxReplicatorScene::open(%this)
{
   startLeaves();
   Parent::open(%this);
   Sky.stormFog(0,3);
    
}

function FogScene::open(%this)
{
   Parent::open(%this);
   Sky.stormFog(1.25,1.25);
}

function InFogScene::open(%this)
{
   Parent::open(%this);
   Sky.stormFog(1.25,1.25);
}

function GoodbyeScene::open(%this)
{
   Parent::open(%this);
   Sky.stormFog(0,3);
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
