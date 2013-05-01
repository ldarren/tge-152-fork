//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------
if ( isObject( moveMap ) )
   moveMap.delete();
new ActionMap(moveMap);


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

moveMap.bind( keyboard, a, moveleft );
moveMap.bind( keyboard, d, moveright );
moveMap.bind( keyboard, w, moveforward );
moveMap.bind( keyboard, s, movebackward );
moveMap.bind( keyboard, space, jump );
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

$firstPerson = false;
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
moveMap.bind(keyboard, tab, toggleFirstPerson );
moveMap.bind(keyboard, "alt c", toggleCamera);


//------------------------------------------------------------------------------
// Misc. Player stuff
//------------------------------------------------------------------------------

moveMap.bindCmd(keyboard, "ctrl w", "commandToServer('playCel',\"wave\");", "");
moveMap.bindCmd(keyboard, "ctrl s", "commandToServer('playCel',\"salute\");", "");
moveMap.bindCmd(keyboard, "ctrl k", "commandToServer('suicide');", "");


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
GlobalActionMap.bindCmd(keyboard, "alt enter", "", "toggleFullScreen();");
GlobalActionMap.bindCmd(keyboard, "F1", "", "contextHelp();");
moveMap.bindCmd(keyboard, "n", "NetGraph::toggleNetGraph();", "");


//------------------------------------------------------------------------------
// Level Editor Action Map - Advanced
//------------------------------------------------------------------------------

if ( isObject( levelEditMap ) )
   levelEditMap.delete();
new ActionMap(levelEditMap);


levelEditMap.bind(mouse0, "button0", selectNew);

levelEditMap.bind(keyboard, "numpad5", goHome);

levelEditMap.bind(keyboard, "numpad8", moveBlockPlusX);
levelEditMap.bind(keyboard, "numpad2", moveBlockMinusX);

levelEditMap.bind(keyboard, "numpad4", moveBlockPlusY);
levelEditMap.bind(keyboard, "numpad6", moveBlockMinusY);

levelEditMap.bind(keyboard, "numpadadd", moveBlockPlusZ);
levelEditMap.bind(keyboard, "numpadminus", moveBlockMinusZ);

levelEditMap.bind(mouse0, "ctrl xaxis", moveBlockInY); 
levelEditMap.bind(mouse0, "shift yaxis", moveBlockInX);
levelEditMap.bind(mouse0, "alt yaxis", moveBlockInZ); 

levelEditMap.bind(keyboard, "numpadenter", dupSelected );

levelEditMap.bind(keyboard, "numpaddecimal", deleteSelected );

levelEditMap.bind(keyboard, "numpad0", cycleNextObjectInType );

levelEditMap.bind(keyboard, "shift numpad0", cyclePrevObjectInType );

levelEditMap.bind(keyboard, "numpad7", cycleNextObjectType );

levelEditMap.bind(keyboard, "shift numpad7", cyclePrevObjectType );

levelEditMap.bind(mouse0, "button1", cycleNextObjectInTypeMouse );

levelEditMap.bind(mouse0, "shift button1", cyclePrevObjectInTypeMouse );

levelEditMap.bind(keyboard, "ctrl s", saveLevel );
levelEditMap.bind(keyboard, "ctrl l", loadLevel );

levelEditMap.bind(keyboard, "space", shootFireBall );


/*

levelEditMap.bind(mouse0, "button0", selectNew);

levelEditMap.bind(keyboard, "backspace", goHome);

levelEditMap.bind(keyboard, "up", moveBlockPlusX);
levelEditMap.bind(keyboard, "down", moveBlockMinusX);

levelEditMap.bind(keyboard, "left", moveBlockPlusY);
levelEditMap.bind(keyboard, "right", moveBlockMinusY);

levelEditMap.bind(keyboard, "=", moveBlockPlusZ);
levelEditMap.bind(keyboard, "minus", moveBlockMinusZ);

levelEditMap.bind(mouse0, "ctrl xaxis", moveBlockInY); 
levelEditMap.bind(mouse0, "shift yaxis", moveBlockInX);
levelEditMap.bind(mouse0, "alt yaxis", moveBlockInZ); 

levelEditMap.bind(keyboard, "enter", dupSelected );

levelEditMap.bind(keyboard, "delete", deleteSelected );

levelEditMap.bind(keyboard, "pageup", cycleNextObjectInType );

levelEditMap.bind(keyboard, "pagedown", cyclePrevObjectInType );

levelEditMap.bind(keyboard, "home", cycleNextObjectType );

levelEditMap.bind(keyboard, "end", cyclePrevObjectType );

levelEditMap.bind(mouse0, "button1", cycleNextObjectInTypeMouse );

levelEditMap.bind(mouse0, "shift button1", cyclePrevObjectInTypeMouse );

levelEditMap.bind(keyboard, "ctrl s", saveLevel );
levelEditMap.bind(keyboard, "ctrl l", loadLevel );

levelEditMap.bind(keyboard, "space", shootFireBall );
*/

function shootFireBall( %val )
{
   if( ! %val ) return;
   
   if( ! isObject($Game::CurrentSelObj ) ) return;
   
   if( "FireEditBlock" !$= $Game::CurrentSelObj.getDatablock().className ) return;

   if( isObject( $Game::CurrentSelObj.bullet ) ) $Game::CurrentSelObj.bullet.delete();
   
   $Game::CurrentSelObj.doFire();
}

function saveLevel( %val )
{
   if( !%val ) return;
   getSaveFilename("*.txt", doSaveLevel);
}

function loadLevel( %val )
{
   if( !%val ) return;
   getLoadFilename("*.txt", doLoadLevel);
}


function doSaveLevel( %fileName )
{
   $Game::FileHandle = new FileObject();

   if(! $Game::FileHandle.openforWrite( %fileName ) ) 
   {
      $Game::FileHandle.delete();
      error("Failed to open file!  Could not save.");
      return false; 
   }
   
   currentBlocks.forEachStmt("$Game::FileHandle.writeLine( tok.getDatablock().getName() SPC tok.getPosition() );", tok);
   
   $Game::FileHandle.close();
   $Game::FileHandle.delete();   
  
   //currentBlocks.forEachStmt("echo( tok.getDatablock().getName() SPC tok.getPosition() );", tok);
}
function doloadLevel( %fileName )
{
   if( isObject( currentBlocks )  ) currentBlocks.delete(); // Advanced
   levelEditGroup.add( new SimGroup( currentBlocks ) ); // Advanced

   $Game::FileHandle = new FileObject();

   if(! $Game::FileHandle.openforRead( %fileName ) ) 
   {
      $Game::FileHandle.delete();
      error("Failed to open file!  Could not load level.");
      return false; 
   }
   
   while( !$Game::FileHandle.isEOF() )
   {
      %currentLine = $Game::FileHandle.readLine();
      
      echo("\c3", %currentLine );

      %blockDB       = getWord( %currentLine , 0 );
      %blockPosition = getWords( %currentLine , 1 , 3 );

      %block = new StaticShape()
      {
         position  = %blockPosition;
         rotation  = "1 0 0 0";
         scale     = "1 1 1";
         dataBlock = %blockDB; 
      };
      %block.setSkinName( %block.getDatablock().normalSkin );
   
      currentBlocks.add( %block );
   }
  
   $Game::FileHandle.close();
   $Game::FileHandle.delete();   

}




function cycleNextObjectInTypeMouse( %val )
{
   cycleNextObjectInType( %val );
}

function cyclePrevObjectInTypeMouse( %val )
{
   cyclePrevObjectInType( %val );
}


function cycleNextObjectType( %val )
{
   if( ! %val ) return;
   
   if( ! isObject($Game::CurrentSelObj ) ) return;
   
   %nextDB = $Game::CurrentSelObj.getDatablock().nextTypeDB;
      
   $Game::CurrentSelObj.setDataBlock( %nextDB );
   
   $Game::CurrentSelObj.setSkinName( $Game::CurrentSelObj.getDatablock().selectedSkin );
}

function cyclePrevObjectType( %val )
{
   if( ! %val ) return;
   
   if( ! isObject($Game::CurrentSelObj ) ) return;
   
   %prevDB = $Game::CurrentSelObj.getDatablock().prevTypeDB;
      
   $Game::CurrentSelObj.setDataBlock( %prevDB );
   
   $Game::CurrentSelObj.setSkinName( $Game::CurrentSelObj.getDatablock().selectedSkin );
}

function cycleNextObjectInType( %val )
{
   echo( "\c2cycleNextObjectInType");
   if( ! %val ) return;
   
   if( ! isObject($Game::CurrentSelObj ) ) return;
   
   echo("xxx", $Game::CurrentSelObj.getDatablock().nextDB );
   
   %nextDB = $Game::CurrentSelObj.getDatablock().nextDB;
      
   $Game::CurrentSelObj.setDataBlock( %nextDB );
   
   $Game::CurrentSelObj.setSkinName( $Game::CurrentSelObj.getDatablock().selectedSkin );
}

function cyclePrevObjectInType( %val )
{
   if( ! %val ) return;
   
   if( ! isObject($Game::CurrentSelObj ) ) return;
   
   %prevDB = $Game::CurrentSelObj.getDatablock().prevDB;
      
   $Game::CurrentSelObj.setDataBlock( %prevDB );
   
   $Game::CurrentSelObj.setSkinName( $Game::CurrentSelObj.getDatablock().selectedSkin );
}

function deleteSelected( %val )
{
   echo("A");
   if( ! %val ) return;
   echo("B");
   
   if( ! isObject($Game::CurrentSelObj ) ) return;
   echo("C");
   
   if( currentBlocks.getCount() < 2 ) return
   echo("D");
      
   $Game::CurrentSelObj.delete();
   $Game::CurrentSelObj = 0;
}

function dupSelected( %val )
{
   if( ! %val ) return;
   
   if( ! isObject($Game::CurrentSelObj ) ) return;
      
   %newBlock = $Game::CurrentSelObj.getDatablock().dup( $Game::CurrentSelObj );
   
   currentBlocks.add( %newBlock );
   
   $Game::CurrentSelObj.setSkinName( $Game::CurrentSelObj.getDatablock().normalSkin );
   $Game::CurrentSelObj.selected = false;

   $Game::CurrentSelObj = %newBlock;

   %newBlock.setSkinName( %newBlock.getDatablock().selectedSkin );
   %newBlock.selected = false;
   
}



function goHome( %val )
{
   if( ! %val ) return;
   
   if( ! isObject($Game::CurrentSelObj ) ) return;
      
   $Game::CurrentSelObj.setTransform( "0 0" SPC $BaseElevation );
   
}

function moveBlockPlusX( %val )
{
   moveBlockInX( -%val );
}

function moveBlockMinusX( %val )
{
   moveBlockInX( %val );
}

function moveBlockPlusY( %val )
{
   moveBlockInY( -%val );
}

function moveBlockMinusY( %val )
{
   moveBlockInY( %val );
}

function moveBlockPlusZ( %val )
{
   moveBlockInZ( -%val );
}

function moveBlockMinusZ( %val )
{
   moveBlockInZ( %val );
}


function moveBlockInX( %val )
{
   if( ! %val ) return;
   
   if( ! isObject($Game::CurrentSelObj ) ) return;
   
   %curPos = $Game::CurrentSelObj.getPosition( );
      
   if( %val < 0 )
   {
      %newPos = vectorAdd( %curPos , $LevelIncrement SPC "0 0" ); 
      
      if( getWord( %newPos , 0 ) < $MaxOffCenter )
      {
         $Game::CurrentSelObj.setTransform( %newPos );
      }
   }
   else 
   {
      %newPos = vectorAdd( %curPos , -$LevelIncrement SPC "0 0" );      
      if( getWord( %newPos , 0 ) > -$MaxOffCenter )
      {
         $Game::CurrentSelObj.setTransform( %newPos );
      }
      
   }
   echo("moveBlockInX( ", %val, " ) " );
}

function moveBlockInY( %val )
{
   if( ! %val ) return;
   
   if( ! isObject($Game::CurrentSelObj ) ) return;
   
   %curPos = $Game::CurrentSelObj.getPosition( );
      
   if( %val < 0 )
   {
      %newPos = vectorAdd( %curPos , 0 SPC $LevelIncrement SPC 0 );      
      if( getWord( %newPos , 1 ) < $MaxOffCenter )
      {
         $Game::CurrentSelObj.setTransform( %newPos );
      }
   }
   else 
   {
      %newPos = vectorAdd( %curPos , 0 SPC -$LevelIncrement SPC 0 );      
      if( getWord( %newPos , 1 ) > -$MaxOffCenter )
      {
         $Game::CurrentSelObj.setTransform( %newPos );
      }

   }
   echo("moveBlockInY( ", %val, " ) " );
}

function moveBlockInZ( %val )
{
   if( ! %val ) return;
   
   if( ! isObject($Game::CurrentSelObj ) ) return;
   
   %curPos = $Game::CurrentSelObj.getPosition( );
      
   if( %val < 0 )
   {
      %newPos = vectorAdd( %curPos , "0  0" SPC $LevelIncrement );      
      $Game::CurrentSelObj.setTransform( %newPos );
   }
   else 
   {
      %newPos = vectorAdd( %curPos , "0  0" SPC -$LevelIncrement );      
      if( getWord( %newPos , 2 ) >= $MinElevation )
      {
         $Game::CurrentSelObj.setTransform( %newPos );
         return true;
      }
   }
   echo("moveBlockInZ( ", %val, " ) " );
   return false;
}




function selectNew( %val )
{
   if( ! %val ) return;
   
   %hitObj = getCurrentLOSObject();
   
   if( isObject(%hitObj) )
   {
      echo("Hit object: ", %hitObj , " :: ", %hitObj.getDatablock().className );
      
      if( %hitObj.selected ) // De-select if selected.
      {
         %hitObj.setSkinName( $Game::CurrentSelObj.getDatablock().normalSkin );
         %hitObj.selected = false;
         $Game::CurrentSelObj = 0;
      }
      else 
      {
         if( isObject($Game::CurrentSelObj) )
         {
            $Game::CurrentSelObj.setSkinName( $Game::CurrentSelObj.getDatablock().normalSkin );
            $Game::CurrentSelObj.selected = false;
         }
         $Game::CurrentSelObj = %hitObj;
         %hitObj.setSkinName( %hitObj.getDatablock().selectedSkin );
         %hitObj.selected = true;
      }
      
   }
   else 
   {
      echo("Hit object: NOTHING" );
      
      if( isObject($Game::CurrentSelObj) )
      {
          $Game::CurrentSelObj.setSkinName( $Game::CurrentSelObj.getDatablock().normalSkin );
          $Game::CurrentSelObj.selected = false;
      }
      $Game::CurrentSelObj = 0;

   }
}

//moveMap.bind(keyboard, "f7", dropPlayerAtCamera);
//moveMap.bind(keyboard, "ctrl o", bringUpOptions);
//moveMap.bindCmd(keyboard, "n", "NetGraph::toggleNetGraph();", "");
//moveMap.bind(mouse0, "xaxis", yaw);
//moveMap.bind(mouse0, "yaxis", pitch);

function getCurrentLOSObject( ) {
   
   %co = $Game::Client.getControlObject();

   %eyePoint  = %co.getWorldBoxCenter();
   %eyeVector = %co.getEyeVector();

   %mask	= $TypeMasks::ShapeBaseObjectType | $TypeMasks::StaticTSObjectType;

   %endPoint = vectorAdd( %eyePoint , vectorScale( %eyeVector , 500 ) );

   %currentInViewObject = containerRayCast( %eyePoint , %endPoint , %mask , %clientConn.player );

   %currentInViewObject = getWord( %currentInViewObject , 0);
   
   return %currentInViewObject;
}
