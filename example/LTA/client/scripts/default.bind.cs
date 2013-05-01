// ============================================================
// Project            :  LTA: VR-20
// File               :  .\lta\client\scripts\default.bind.cs
// Copyright          :  
// Author             :  ldarren
// Created on         :  Tuesday, June 19, 2007 7:25 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

if ( isObject( moveMap ) )
   moveMap.delete();
new ActionMap(moveMap);

if ( isObject( racingMap ) )
   racingMap.delete();
new ActionMap(racingMap);

//------------------------------------------------------------------------------
// Non-remapable binds
//------------------------------------------------------------------------------
/* replace my quit button
function escapeFromGame()
{
   if ( $Server::ServerType $= "SinglePlayer" )
      MessageBoxYesNo( "Quit Game", "Exit from the Game?", "disconnect();", "");
   else
      MessageBoxYesNo( "Disconnect", "Disconnect from the server?", "disconnect();", "");
}

moveMap.bindCmd(keyboard, "escape", "", "escapeFromGame();");
*/
function showPlayerList(%val)
{
   if(%val)
      PlayerListGui.toggle();
}

moveMap.bind( keyboard, F2, showPlayerList );
moveMap.bind( keyboard, F5, toggleParticleEditor);

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
   $mvLeftAction = %val;
}

function moveright(%val)
{
   $mvRightAction = %val;
}

function moveforward(%val)
{
   $mvForwardAction = %val*$pref::Input::KeyboardForwardSpeed;
}

function movebackward(%val)
{
   $mvBackwardAction = %val*$pref::Input::KeyboardBackwardSpeed;
}

function moveup(%val)
{
   $mvUpAction = %val;
}

function movedown(%val)
{
   $mvDownAction = %val;
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

racingMap.bind( keyboard, a, turnleft );
racingMap.bind( keyboard, d, turnright );
racingMap.bind( keyboard, left, turnleft );
racingMap.bind( keyboard, right, turnright );

//moveMap.bind( keyboard, a, moveleft );
//moveMap.bind( keyboard, d, moveright );
//moveMap.bind( keyboard, left, moveleft );
//moveMap.bind( keyboard, right, moveright );

moveMap.bind( keyboard, w, moveforward );
moveMap.bind( keyboard, s, movebackward );

moveMap.bind( keyboard, space, jump );
//moveMap.bind( mouse, xaxis, yaw );
//moveMap.bind( mouse, yaxis, pitch );

//------------------------------------------------------------------------------
// Advanced Camera Movement
//------------------------------------------------------------------------------
$cameraYawSpeed = -100.0;
$cameraPitchSpeed = -50.0;
$cameraZoomSpeed = -5.0;
function rotateCameraHorizontal(%val)
{     
	$advCamera::Yaw = getMouseAdjustAmount(%val)*$cameraYawSpeed ;
}

function rotateCameraHorizontalLeft(%val)
{     
   $advCamera::Yaw = %val ? $Pref::Input::KeyboardTurnSpeed : 0;
}

function rotateCameraHorizontalRight( %val )
{
   $advCamera::Yaw = %val ? $Pref::Input::KeyboardTurnSpeed : 0;
}

function rotateCameraVertical(%val)
{     
	$advCamera::Pitch = getMouseAdjustAmount(%val)*$cameraPitchSpeed;
}

// Azimuth and decline are absolute value
// Pitch and yaw are increment value
$ScheduleDeclineUp = -1;
$ScheduleDeclineDown = -1;
$ScheduleAzimuthLeft = -1;
$ScheduleAzimuthRight = -1;
$DeclineSpeed = 0;
$AzimuthAngle = 90;

function zoomCamera(%val)
{     
	$advCamera::Zoom = getMouseAdjustAmount(%val)*$cameraZoomSpeed;
}

function declineUp(%val)
{
	if ($ScheduleDeclineDown != -1) 
	{
		cancel($ScheduleDeclineDown);
		$ScheduleDeclineDown = -1;
		$DeclineSpeed = 0;
	}
	$DeclineSpeed -= 5;
	if ($DeclineSpeed < -100) $DeclineSpeed = -100;
	changeCameraPitch($DeclineSpeed);
	$ScheduleDeclineUp = schedule(500, 0, "declineUp" );
}

function declineDown(%val)
{
	if ($ScheduleDeclineUp != -1)
	{
		cancel($ScheduleDeclineUp);
		$ScheduleDeclineUp = -1;
		$DeclineSpeed = 0;
	}
	$DeclineSpeed += 5;
	if ($DeclineSpeed > 100) $DeclineSpeed = 100;
	changeCameraPitch($DeclineSpeed);
	$ScheduleDeclineDown = schedule(500, 0, "declineDown" );
}

function cancelCameraDecline()
{
	if ($ScheduleDeclineUp != -1)
	{
		cancel($ScheduleDeclineUp);
		$ScheduleDeclineUp = -1;
	}
	if ($ScheduleDeclineDown != -1)
	{
		cancel($ScheduleDeclineDown);
		$ScheduleDeclineDown = -1;
	}
	$DeclineSpeed = 0;
}

function changeCameraPitch(%val)
{
	$advCamera::pitch = %val;
}

function azimuthLeft(%val)
{
	if ($ScheduleAzimuthRight != -1) 
	{
		cancel($ScheduleAzimuthRight);
		$ScheduleAzimuthRight = -1;
	}
	$AzimuthAngle -= 10;
	if ($AzimuthAngle > 360) $AzimuthAngle -= 360;
	changeCameraYaw($AzimuthAngle);
	$ScheduleAzimuthLeft = schedule(200, 0, "azimuthLeft" );
}

function azimuthRight(%val)
{
	if ($ScheduleAzimuthLeft != -1) 
	{
		cancel($ScheduleAzimuthLeft);
		$ScheduleAzimuthLeft = -1;
	}
	$AzimuthAngle += 10;
	if ($AzimuthAngle < -360) $AzimuthAngle += 360;
	changeCameraYaw($AzimuthAngle);
	$ScheduleAzimuthRight = schedule(200, 0, "azimuthRight" );
}

function cancelCameraAzimuth()
{
	if ($ScheduleAzimuthRight != -1) 
	{
		cancel($ScheduleAzimuthRight);
		$ScheduleAzimuthRight = -1;
	}
	if ($ScheduleAzimuthLeft != -1) 
	{
		cancel($ScheduleAzimuthLeft);
		$ScheduleAzimuthLeft = -1;
	}
}

function changeCameraYaw(%val)
{
	$advCamera::azimuth = %val;
}

//moveMap.bind( mouse, xaxis, rotateCameraHorizontal);
//moveMap.bind( mouse, yaxis, rotateCameraVertical );
moveMap.bind( mouse, zaxis, zoomCamera );
moveMap.bind( keyboard, a, turnLeft );
moveMap.bind( keyboard, d, turnRight );

moveMap.bindCmd(keyboard, "left", "azimuthLeft();", "cancelCameraAzimuth();");
moveMap.bindCmd(keyboard, "right", "azimuthRight();", "cancelCameraAzimuth();");
moveMap.bindCmd(keyboard, "up", "declineUp();", "cancelCameraDecline();");
moveMap.bindCmd(keyboard, "down", "declineDown();", "cancelCameraDecline();");
//moveMap.bind( keyboard, left, azimuthLeft );
//moveMap.bind( keyboard, right, azimuthRight );
//moveMap.bind( keyboard, up, declineUp );
//moveMap.bind( keyboard, down, declineDown );

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// AFX MOUSE CONTROL

//------------------------------------------------------------------------------
// Mouse Trigger
//------------------------------------------------------------------------------

//function mouseFire(%val)
//{
//   $mvTriggerCount0++;
//}

//function altTrigger(%val)
//{
//   $mvTriggerCount1++;
//}

// not use, call directly from afxTSCtrl
function mouse_btn0(%val)
{   
	if (%val)
	{
		ServerConnection.setPreSelectedObjFromRollover();
	}
	else
	{
		ServerConnection.setSelectedObjFromPreSelected();
	}
}

//moveMap.bind( mouse, button0, mouseFire );
//moveMap.bind( mouse, button1, altTrigger );
//moveMap.bind(mouse,     button0,    mouse_btn0);
//moveMap.bind(mouse,     button1,    mouse_btn1);
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

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

$firstPerson = false;
function toggleFirstPerson(%val)
{
	if (%val)
	{
		$firstPerson = !$firstPerson;
		ServerConnection.setFirstPerson($firstPerson);
		
		//moveMap.unbind( keyboard, a);
		//moveMap.unbind( keyboard, d );
		moveMap.unbind( keyboard, up );
		moveMap.unbind( keyboard, down );
		moveMap.unbind( keyboard, left );
		moveMap.unbind( keyboard, right );
		moveMap.unbind( mouse, xaxis );
		moveMap.unbind( mouse, yaxis );
		
		if ($firstPerson)
		{
			//moveMap.bind( keyboard, a, moveleft );
			//moveMap.bind( keyboard, d, moveright );
			//moveMap.bind( keyboard, left, moveleft );
			//moveMap.bind( keyboard, right, moveright );
			moveMap.bind( mouse, xaxis, yaw );
			moveMap.bind( mouse, yaxis, pitch );
		}
		else
		{
			//moveMap.bind( keyboard, a, turnLeft );
			//moveMap.bind( keyboard, d, turnRight );
			moveMap.bindCmd(keyboard, "left", "azimuthLeft();", "cancelCameraAzimuth();");
			moveMap.bindCmd(keyboard, "right", "azimuthRight();", "cancelCameraAzimuth();");
			moveMap.bindCmd(keyboard, "up", "declineUp();", "cancelCameraDecline();");
			moveMap.bindCmd(keyboard, "down", "declineDown();", "cancelCameraDecline();");
		}
	}
}
 
function toggleMouseLook(%val)
{
 if( %val )
 {
  if( Canvas.isCursorOn())
      Canvas.cursorOff();
  else
      Canvas.cursorOn();
 }
}

moveMap.bind( keyboard, "m", "toggleMouseLook" );

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

function celebrationWave(%val)
{
   if(%val)
      commandToServer('playCel', "wave");
}

function celebrationSalute(%val)
{
   if(%val)
      commandToServer('playCel', "salute");
}

function suicide(%val)
{
   if(%val)
      commandToServer('suicide');
}

moveMap.bind(keyboard, "ctrl w", celebrationWave);
moveMap.bind(keyboard, "ctrl s", celebrationSalute);
moveMap.bind(keyboard, "ctrl k", suicide);

//------------------------------------------------------------------------------
// Item manipulation
//------------------------------------------------------------------------------

moveMap.bindCmd(keyboard, "1", "commandToServer('use',\"Crossbow\");", "");

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
//    if ( %val )
//       beginDemoRecord();
}

function stopRecordingDemo( %val )
{
//    if ( %val )
//       stopRecord();
}

//moveMap.bind( keyboard, F3, startRecordingDemo );
//moveMap.bind( keyboard, F4, stopRecordingDemo );


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
   if(%val)
      Canvas.pushDialog(OptionsDlgGui);
}

moveMap.bind(keyboard, "ctrl o", bringUpOptions);


//----------------------------------------------------------------
//RPGDialog Code Begin
//----------------------------------------------------------------   
function TalkTo(%val)
{
   if(%val)
      commandToServer('RPGDialogRay');
}

moveMap.bind(keyboard, "q", TalkTo);

function MapHudVisibility()
{
	if (!isObject(MapHud)) return;
	if (MapHud.isVisible())
		MapHud.setVisible(false);
	else
		MapHud.setVisible(true);
}

moveMap.bindCmd(keyboard, "c","", "MapHudVisibility();");

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

