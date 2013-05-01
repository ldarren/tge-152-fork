//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

if ( isObject( moveMap ) )
   moveMap.delete();
new ActionMap(moveMap);

if ( isObject( racingMap ) )
   racingMap.delete();
new ActionMap(racingMap);

//------------------------------------------------------------------------------
// Non-remapable binds
//------------------------------------------------------------------------------

function escapeFromGame()
{
   if ( $Server::ServerType $= "SinglePlayer" )
      MessageBoxYesNo( "Quit Mission", "Exit from this Mission?", "disconnect();", "");
   else
      MessageBoxYesNo( "Disconnect", "Disconnect from the server?", "disconnect();", "");
}

moveMap.bindCmd(keyboard, "escape", "", "escapeFromGame();");

function showPlayerList(%val)
{
   if (%val)
      PlayerListGui.toggle();
}

moveMap.bind( keyboard, F2, showPlayerList );

//------------------------------------------------------------------------------
// Movement Keys
//------------------------------------------------------------------------------

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

function moveup(%val)
{
   $mvUpAction = %val * $movementSpeed;
}

function movedown(%val)
{
   $mvDownAction = %val * $movementSpeed;
}

function accelerate(%val)
{
   $mvForwardAction = %val * $movementSpeed;
}

function brake(%val)
{
   $mvBackwardAction = %val * $movementSpeed;
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

function handbrake(%val)
{
   $mvTriggerCount2++;
}

racingMap.bind( keyboard, a, turnleft );
racingMap.bind( keyboard, d, turnright );
racingMap.bind( keyboard, left, turnleft );
racingMap.bind( keyboard, right, turnright );

moveMap.bind( keyboard, a, moveleft );
moveMap.bind( keyboard, d, moveright );
moveMap.bind( keyboard, left, moveleft );
moveMap.bind( keyboard, right, moveright );

moveMap.bind( keyboard, w, accelerate );
moveMap.bind( keyboard, s, brake );
moveMap.bind( keyboard, up, accelerate );
moveMap.bind( keyboard, down, brake );

moveMap.bind( keyboard, space, handbrake );
moveMap.bind( mouse, xaxis, yaw );
moveMap.bind( mouse, yaxis, pitch );


//------------------------------------------------------------------------------
// Mouse Trigger
//------------------------------------------------------------------------------

function mouseFire(%val)
{
   $mvTriggerCount0++;
}

function altTrigger(%val)
{
   $mvTriggerCount1++;
}

moveMap.bind( mouse, button0, mouseFire );
//moveMap.bind( mouse, button1, altTrigger );


//------------------------------------------------------------------------------
// Zoom and FOV functions
//------------------------------------------------------------------------------

if($Pref::player::CurrentFOV $= "")
   $Pref::player::CurrentFOV = 45;

function setZoomFOV(%val)
{
   if(%val)
      toggleZoomFOV();
}
      
function toggleZoom( %val )
{
   if ( %val )
   {
      $ZoomOn = true;
      setFov( $Pref::player::CurrentFOV );
   }
   else
   {
      $ZoomOn = false;
      setFov( $Pref::player::DefaultFov );
   }
}

moveMap.bind(keyboard, r, setZoomFOV);
moveMap.bind(keyboard, e, toggleZoom);


//------------------------------------------------------------------------------
// Camera & View functions
//------------------------------------------------------------------------------

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
   {
      // Since we want the free cam to strafe but the WheeledVehicle
      // to turn we need to do a little ActionMap wizardry
      %control = ServerConnection.getControlObject();
      if (%control.getClassName() $= "Camera")
         racingMap.push();
      else
         racingMap.pop();
   
      commandToServer('ToggleCamera');
   }
}

moveMap.bind( keyboard, z, toggleFreeLook );
moveMap.bind(keyboard, tab, toggleFirstPerson );
moveMap.bind(keyboard, "alt c", toggleCamera);

//------------------------------------------------------------------------------
// Misc. Player stuff
//------------------------------------------------------------------------------

moveMap.bindCmd(keyboard, "ctrl r", "commandToServer('reset');", "");

//------------------------------------------------------------------------------
// Item manipulation
//------------------------------------------------------------------------------

moveMap.bindCmd(keyboard, "h", "commandToServer('use',\"HealthKit\");", "");
moveMap.bindCmd(keyboard, "1", "commandToServer('use',\"Rifle\");", "");
moveMap.bindCmd(keyboard, "2", "commandToServer('use',\"Crossbow\");", "");

//------------------------------------------------------------------------------
// Message HUD functions
//------------------------------------------------------------------------------

function pageMessageHudUp( %val )
{
   if ( %val )
      pageUpMessageHud();
}

function pageMessageHudDown( %val )
{
   if ( %val )
      pageDownMessageHud();
}

function resizeMessageHud( %val )
{
   if ( %val )
      cycleMessageHudSize();
}

moveMap.bind(keyboard, u, toggleMessageHud );
//moveMap.bind(keyboard, y, teamMessageHud );
moveMap.bind(keyboard, "pageUp", pageMessageHudUp );
moveMap.bind(keyboard, "pageDown", pageMessageHudDown );
moveMap.bind(keyboard, "p", resizeMessageHud );


//------------------------------------------------------------------------------
// Demo recording functions
//------------------------------------------------------------------------------

function startRecordingDemo( %val )
{
   if ( %val )
      startDemoRecord();
}

function stopRecordingDemo( %val )
{
   if ( %val )
      stopDemoRecord();
}

moveMap.bind( keyboard, F3, startRecordingDemo );
moveMap.bind( keyboard, F4, stopRecordingDemo );


//------------------------------------------------------------------------------
// Helper Functions
//------------------------------------------------------------------------------

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

function bringUpOptions(%val)
{
   if (%val)
      Canvas.pushDialog(OptionsDlg);
}

moveMap.bind(keyboard, "ctrl o", bringUpOptions);
moveMap.bindCmd(keyboard, "n", "NetGraph::toggleNetGraph();", "");

//------------------------------------------------------------------------------
// Dubuging Functions
//------------------------------------------------------------------------------

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


//------------------------------------------------------------------------------
// Misc.
//------------------------------------------------------------------------------

GlobalActionMap.bind(keyboard, "tilde", toggleConsole);
GlobalActionMap.bindCmd(keyboard, "alt k", "cls();","");
GlobalActionMap.bindCmd(keyboard, "alt enter", "", "toggleFullScreen();");
GlobalActionMap.bindCmd(keyboard, "F1", "", "contextHelp();");

