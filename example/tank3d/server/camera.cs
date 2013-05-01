//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Global movement speed that affects all cameras.
$Camera::movementSpeed = 20;//40;

//-----------------------------------------------------------------------------
// Define a datablock class to use for our observer camera
//-----------------------------------------------------------------------------

datablock CameraData(Observer)
{
   mode = "Observer";
};

function Observer::onCollision( %this, %obj, %col, %fade, %pos, %normal )
{
   stopRotateOrbitMap();
   stopZoomSpawnPlayer();
}

// Enum Camera Modes
$camStaticMap = 1;
$camOrbitMap = 2;
$camGodPlayer = 3;
$camSpawnPlayer = 4;
$camShootProjectile = 5;

$cameraLock = -1;		// camera locking mechanism

//-------------------------------------------------------------
// Camera Modes
//-------------------------------------------------------------

$StaticCameraPoint0 = "0 0 300 1 0 0 1.571";
$StaticCameraPoint1 = "-94.6177 -260.408 74.9807 -0.6025 0.0757073 -0.79452 17.974";
$StaticCameraPoint2 = "-254.322 -14.1592 77.4264 0.15356 0.0578006 0.986447 70.6367";
$StaticCameraPoint3 = "-3.15803 -44.5327 74.8483 0.310928 -0.125622 0.942095 51.1291";
$StaticCameraPoint4 = "47.1128 -0.604045 42.9513 0.325871 0.081462 -0.941898 53.5109";

///////////////////////////////////////////////////////////////////////////////

function cameraSpawnPlayer(%client, %objectHandle)
{
   stopRotateOrbitMap();
   stopZoomSpawnPlayer();
	
    %xfrm = %objectHandle.getTransform();
    %lx = getword(%xfrm,0); // first, get the current transform values
    %ly = getword(%xfrm,1);
    %lz = getword(%xfrm,2);
    %rx = getword(%xfrm,3);
    %ry = getword(%xfrm,4);
    %rz = getword(%xfrm,5);
    %rd = getword(%xfrm,6);
	
	%randYaw = getRandom(0,$PI);
		
	%cameraTransform = %lx SPC %ly SPC %lz SPC 0 SPC 0 SPC %rz SPC mAbs(%randYaw);
	
	%client.camera.setOrbitMode(%objectHandle, %cameraTransform, 10, 30, 50, 0);

    schedule(100, 0, startZoomSpawnPlayer, %client);
}

function startZoomSpawnPlayer(%client)
{
   %client.camera.setFlyMode();
 	%client.setControlObject(%client.camera);
	%client.setCameraObject(%client.camera);

   $mvPitch = 0.2;
   $mvForwardAction += 0.10;
    
   schedule(3000, 0, stopZoomSpawnPlayer, %client);
}

function stopZoomSpawnPlayer(%client)
{
	$mvForwardAction = 0;
	if(%client)
		schedule(1000, 0, cameraManager, %client, 6, 0);
}

///////////////////////////////////////////////////////////////////////////////
function cameraKillPlayerTimer(%client, %mode, %objectHandle)
{
	if($cameraLock < 0)
		cameraManager(%client, $camOrbitMap, %objectHandle);
}

function cameraOrbitMap(%client, %objectHandle)
{
   stopRotateOrbitMap();
   stopZoomSpawnPlayer();
	
	%CameraAngleMode = mCeil(getRandom(0,1));
	
	if(%CameraAngleMode)
	{
		%pitchAngle = 0.45;
		%camDistance = 30;
	}
	else
	{
		%pitchAngle = -0.25;
		%camDistance = 10;
	}
	
   %client.camera.setOrbitMode(%objectHandle, %objectHandle.getTransform(), 1, %camDistance, %camDistance, 0);
	%client.setCameraObject(%client.camera);
	%client.setControlObject(%client.camera);
	
	$mvPitch = %pitchAngle;	
	
	startRotateOrbitMap(%client);
}

function startRotateOrbitMap(%client)
{
	$mvYawLeftSpeed += 0.005;
	
   schedule(3000, 0, stopRotateOrbitMap, %client);
}

function stopRotateOrbitMap(%client)
{
	$mvYawLeftSpeed = 0;
	if(%client)
		schedule(1000, 0, cameraManager, %client, 6, 0);
}

////////////////////////////////////////////////////////////////////////////////
function cameraShootProjectileTimer(%client, %mode, %objectHandle)
{
	if($cameraLock != $camShootProjectile)
		cameraManager(%client, $camShootProjectile, %objectHandle);
}

function cameraShootProjectile(%client, %projectile)
{	
   stopRotateOrbitMap();
   stopZoomSpawnPlayer();
	
   %client.camera.setOrbitMode(%projectile, %projectile.getName().getTransform(), 1, 20, 20, 0);
   %client.setCameraObject(%client.camera);
   %client.setControlObject(%client.camera);
		
	startShootProjectile(%client.camera);
}

function startShootProjectile()
{
	$mvPitch = 0.65;	
	//$mvYawLeftSpeed += 0.005;
}

///////////////////////////////////////////////////////////////////////////////

function cameraMapOverview(%client)
{
   stopRotateOrbitMap();
   stopZoomSpawnPlayer();
	
   %CameraNum = mCeil(getRandom(1,4));
	
	switch(%CameraNum)
	{
		case 1:
			%client.camera.setTransform($StaticCameraPoint1);
		case 2:
			%client.camera.setTransform($StaticCameraPoint2);
		case 3:
			%client.camera.setTransform($StaticCameraPoint3);
		case 4:
			%client.camera.setTransform($StaticCameraPoint4);
	}
	
	%client.camera.setFlyMode();
	%client.setCameraObject(%client.camera);
	%client.setControlObject(%client.player);
	%client.setFirstPerson(false);
}

////////////////////////////////////////////////////////////////////////////////

function cameraGodModePlayer(%client, %objectHandle, %angle)
{
   stopRotateOrbitMap();
   stopZoomSpawnPlayer();
   
   %xfrm = %objectHandle.getTransform();
   %lx = getword(%xfrm,0); // first, get the current transform values
   %ly = getword(%xfrm,1);
   %lz = getword(%xfrm,2);
   %rx = getword(%xfrm,3);
   %ry = getword(%xfrm,4);
   %rz = getword(%xfrm,5);
   %rd = getword(%xfrm,6);

	//%randYaw = getRandom(0,$PI);
	%rand = mAbs(mCeil(getRandom(0, 1)));
	%randCorrection = (%rand > 0)?-0.25:0.25;
	%Yaw = %randCorrection + (%angle * $RADIANS);
	
	echo("THIS IS YAW = "@%Yaw);
		
	//%cameraTransform = %lx SPC %ly SPC %lz SPC 0 SPC 0 SPC %rz SPC mAbs(%randYaw);
	%cameraTransform = %lx SPC %ly SPC %lz SPC 0 SPC 0 SPC %rz SPC %Yaw;
	
	%client.camera.setOrbitMode(%objectHandle, %cameraTransform, 0, 10, 20, 0);
	%client.setControlObject(%client.player);
	%client.setCameraObject(%client.camera);
}

////////////////////////////////////////////////////////////////////////////////

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
      %this.speed = 10;
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


////////////////////////////////////////////////////////////////////////////////
$DurationMapOverview = 2;
$DurationOrbitMap = 3;
$DurationGodMode = 3;
$DurationSpawnPlayer = 3;
$DurationShootProjectile = 3;
$DurationPathCamera = 3;

function cameraManager(%client, %mode, %objectHandle, %angle)
{
	$cameraLock = %mode;
	%client.setControlObject(%client.camera);
	
	switch(%mode)
	{
		case 1:
			schedule($DurationMapOverview * 1000, 0, cameraUnlock, %client);
			cameraMapOverview(%client);
		case 2:
			schedule($DurationOrbitMap * 1000, 0, cameraUnlock, %client);
			cameraOrbitMap(%client, %objectHandle);
		case 3:
			schedule($DurationGodMode * 1000, 0, cameraUnlock, %client);
			cameraGodModePlayer(%client, %objectHandle, %angle);
		case 4:
			schedule(DurationSpawnPlayer * 1000, 0, cameraUnlock, %client);
			cameraSpawnPlayer(%client, %objectHandle);
		case 5:
			schedule($DurationShootProjectile * 1000, 0, cameraUnlock, %client);
			cameraShootProjectile(%client, %objectHandle);
		default:															// DEFAULT CAMERA
			schedule($DurationPathCamera * 1000, 0, cameraUnlock, %client);
			%client.setControlObject(%client.PathCamera);
	}
}

function cameraUnlock(%client)
{
	$cameraLock = -1;
}

