//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Key bindings for the in-game action Map
//------------------------------------------------------------------------------

// IMPORTANT NOTE!!! BEFORE SHIPPING OUT PLEASE COMMENT OUT UNNECESSARY BINDINGS.

if ( isObject( moveMap ) )
   moveMap.delete();
new ActionMap(moveMap);

//------------------------------------------------------------------------------
// Misc. in-game keys

function escapeFromGame()
{
	exec("~/client/ui/mainMenuGui.gui");
   if ( $Server::ServerType $= "SinglePlayer" )
      MessageBoxYesNo( "Quit Mission", "Exit from this Game?", "disconnect();", "");
   else
      MessageBoxYesNo( "Disconnect", "Disconnect from the server?", "disconnect();", "");
}

moveMap.bindCmd(keyboard, "escape", "", "escapeFromGame();");

//------------------------------------------------------------------------------
// Movement Keys

$movementSpeed = 1; // m/s

function setSpeed(%speed)
{
   if(%speed)
      $movementSpeed = %speed;
}

function moveleft(%val)
{
   $mvLeftAction = %val * $movementSpeed;
}

function moveright(%val)
{
   $mvRightAction = %val * $movementSpeed;
}

function moveforward(%val)
{
   $mvForwardAction = %val * $movementSpeed;
}

function movebackward(%val)
{
   $mvBackwardAction = %val * $movementSpeed;
}

function moveup(%val)
{
   $mvUpAction = %val * $movementSpeed;
}

function movedown(%val)
{
   $mvDownAction = %val * $movementSpeed;
}

function turnLeft( %val )
{
   $mvYawRightSpeed = %val ? $Pref::Input::KeyboardTurnSpeed : 0;
}

function turnRight( %val )
{
   $mvYawLeftSpeed = %val ? $Pref::Input::KeyboardTurnSpeed : 0;
}

function panUp( %val )
{
   $mvPitchDownSpeed = %val ? $Pref::Input::KeyboardTurnSpeed : 0;
}

function panDown( %val )
{
   $mvPitchUpSpeed = %val ? $Pref::Input::KeyboardTurnSpeed : 0;
}

function getMouseAdjustAmount(%val)
{
   // based on a default camera fov of 90'
   return(%val * ($cameraFov / 90) * 0.01);
}

function yaw(%val)
{
   $mvYaw += getMouseAdjustAmount(%val);
}

function pitch(%val)
{
   $mvPitch += getMouseAdjustAmount(%val);
}

function jump(%val)
{
   $mvTriggerCount2++;
}

function mouseTrigger(%val)
{
   $mvTriggerCount0++;
}

moveMap.bind( keyboard, a, moveleft );
moveMap.bind( keyboard, d, moveright );
moveMap.bind( keyboard, w, moveforward );
moveMap.bind( keyboard, s, movebackward );
moveMap.bind( keyboard, space, jump );
moveMap.bind( keyboard, q, mouseTrigger );


moveMap.bind( keyboard, o, turnLeft);
moveMap.bind( keyboard, p, turnRight);

moveMap.bind( mouse, xaxis, yaw );
moveMap.bind( mouse, yaxis, pitch );
moveMap.bind( mouse, button0, mouseTrigger );


//------------------------------------------------------------------------------
// Camera & View functions

function toggleFreeLook( %val )
{
   if ( %val )
      $mvFreeLook = true;
   else
      $mvFreeLook = false;
}

$firstPerson = true;
function toggleFirstPerson(%val)
{
   if (%val)
   {
      $firstPerson = !$firstPerson;
      ServerConnection.setFirstPerson($firstPerson);
   }
}

function toggleCamera(%val)
{
   if (%val)
      commandToServer('ToggleCamera');
}

moveMap.bind( keyboard, z, toggleFreeLook );
moveMap.bind( keyboard, tab, toggleFirstPerson );
moveMap.bind( keyboard, "alt c", toggleCamera);


//------------------------------------------------------------------------------
// Helper functions used by the mission editor

function dropCameraAtPlayer(%val)
{
   if (%val)
      commandToServer('dropCameraAtPlayer');
}

function dropPlayerAtCamera(%val)
{
   if (%val)
      commandToServer('DropPlayerAtCamera');
}

moveMap.bind(keyboard, "F8", dropCameraAtPlayer);
moveMap.bind(keyboard, "F7", dropPlayerAtCamera);


//------------------------------------------------------------------------------
// Key bindings for the Global action map available everywhere
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Misc.

GlobalActionMap.bind(keyboard, "tilde", toggleConsole);
GlobalActionMap.bindCmd(keyboard, "alt enter", "", "toggleFullScreen();");


//------------------------------------------------------------------------------
// Dubuging Functions

$MFDebugRenderMode = 0;
function cycleDebugRenderMode(%val)
{
   if (!%val)
      return;

   if (getBuildString() $= "Debug")
   {
      if($MFDebugRenderMode == 0)
      {
         // Outline mode, including fonts so no stats
         $MFDebugRenderMode = 1;
         GLEnableOutline(true);
      }
      else if ($MFDebugRenderMode == 1)
      {
         // Interior debug mode
         $MFDebugRenderMode = 2;
         GLEnableOutline(false);
         setInteriorRenderMode(7);
         showInterior();
      }
      else if ($MFDebugRenderMode == 2)
      {
         // Back to normal
         $MFDebugRenderMode = 0;
         setInteriorRenderMode(0);
         GLEnableOutline(false);
         show();
      }
   }
   else
   {
      echo("Debug render modes only available when running a Debug build.");
   }
}

GlobalActionMap.bind(keyboard, "F9", cycleDebugRenderMode);

// This should be a command to the server but for
// a simple example this will do

// Track if we are in orbitMode
$orbitMode = false;

// Toggle the Orbit Mode by pushing a button
function toggleOrbitMode(%val)
{
   // If the button is pushed
   if (%val)
   {
      // If we ar not in orbit mode
      if (!$orbitMode)
      {
         // The camera can be a bit fickle so we have to
         // pay attention to what we are doing with it

         // Put the camera object into orbit mode around the player
         // This mode takes a game base object, transform, min distance,
         // max distance, current distance, and if the client owns the object
         LocalClientConnection.camera.setOrbitMode(
            LocalClientConnection.player,
            LocalClientConnection.player.getTransform(),
            2, 20, 10, 0);
         // We need to tell the client that the camera object is now the
         // camera versus the player object
         LocalClientConnection.setCameraObject(
            LocalClientConnection.camera);
         // We want to be able to control the camera so we will tell the client
         // the control object is the camera
         LocalClientConnection.setControlObject(
            LocalClientConnection.camera);
         // we have just set up the orbit mode so set the variable to true.
         $orbitMode = true;
      }
      else
      {
         // We are in orbit mode so we want to set things back to normal.
         // Set the camera into fly mode
         LocalClientConnection.camera.setFlyMode();
         // Tell the client we want to view the scene through the player.
         LocalClientConnection.setCameraObject(
            LocalClientConnection.player);
         // Tell the client we want to control the player again.
         LocalClientConnection.setControlObject(
            LocalClientConnection.player);
         // We are now out of orbit mode.
         $orbitMode = false;
      }
   }
}

moveMap.bind(keyboard, c, toggleOrbitMode);



