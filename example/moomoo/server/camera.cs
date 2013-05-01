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
// Camera Related Functions (Similar with the functions in editor)
//-----------------------------------------------------------------------------

function serverCmdToggleCameraStatic(%client)
{
   if ($Server::ServerType $= "SinglePlayer") {		
		// Pitch player shot up
		$mvPitch -= 0.35;		
		
		//Static Mode
		%control = %client.getControlObject();
		%control = %client.camera;
		%control.setFlyMode();
		%client.setCameraObject(%control);
		%client.setFirstPerson(false);
		%control.setTransform("34 -133 31 1 0 0 -0.2");//("36 -150 50 1 0 0 -100");
		%client.setControlObject(%client.player);
	}
}

function serverCmdToggleProjectileCamera(%client, %object)
{
	%control = %client.camera;
	%control.setOrbitMode(%object, %client.player.getTransform(), 2, 10, 10, 0);
	%client.setCameraObject(%control);
	%client.setControlObject(%control);
}

function serverCmdToggleGodCamera(%client)
{
	%control = %client.camera;
	%control.setOrbitMode(%client.player,%client.player.getTransform(), 2, 5, 5, 0);
	%client.setControlObject(%client.player);
	%client.setCameraObject(%control);
}

function serverCmdToggleFirstPersonCamera(%client)
{
	%control = %client.player;
    %control.mode = observerFly;
	%client.setFirstPerson(true);
}

function serverCmdToggleThirdPersonCamera(%client)
{
	%control = %client.player;
    %control.mode = observerFly;
	%client.setFirstPerson(false);
	%client.setControlObject(%client.player);
	%client.setCameraObject(%control);
}

function serverCmdToggleOrbitLevel(%client, %object)
{
	%control = %client.camera;
	%control.setOrbitMode(%object, %client.player.getTransform(), 2, 10, 10, 0);
	%client.setCameraObject(%control);
	%client.setControlObject(%control);
	rotateOrbitCam();
}

function rotateOrbitCam()
{
	$mvPitch = 0.25;	
	$mvYawLeftSpeed += 0.01;
	
	$tagOrbitCam = schedule(100, 0, 'rotateOrbitCam');
}

function selectCameraRand()
{
	switch(mCeil(getRandom(0,6)))
	{
		case 1:
			commandToServer('ToggleGodCamera');
			//break;
		case 2:
			commandToServer('ToggleFirstPersonCamera');
			//break;
		case 3:
			commandToServer('ToggleThirdPersonCamera');
			//break;
		default:
			commandToServer('ToggleCameraStatic');
	}
}

function setSchToggleCameraStatic()
{
	commandToServer('ToggleCameraStatic');
}

