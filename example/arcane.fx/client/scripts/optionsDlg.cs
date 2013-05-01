function optionsDlg::setPane(%this, %pane)
{
   OptAudioPane.setVisible(false);
   OptGraphicsPane.setVisible(false);
   OptControlsPane.setVisible(false);
   ("Opt" @ %pane @ "Pane").setVisible(true);
   OptRemapList.fillList();
}

function OptionsDlg::onWake(%this)
{
   OptGraphicsButton.performClick();
   %buffer = getDisplayDeviceList();
   %count = getFieldCount( %buffer );
   OptGraphicsDriverMenu.clear();
   OptScreenshotMenu.init();
   OptScreenshotMenu.setValue($pref::Video::screenShotFormat);
   OptFXDetailMenu.init();
   OptFXDetailMenu.setSelected($pref::AFX::fxLevelOfDetail);

   for(%i = 0; %i < %count; %i++)
      OptGraphicsDriverMenu.add(getField(%buffer, %i), %i);
   %selId = OptGraphicsDriverMenu.findText( $pref::Video::displayDevice );
	if ( %selId == -1 )
		%selId = 0; // How did THAT happen?
	OptGraphicsDriverMenu.setSelected( %selId );
	OptGraphicsDriverMenu.onSelect( %selId, "" );

   // Audio 
   OptAudioUpdate();
   OptAudioVolumeMaster.setValue($pref::Audio::masterVolume);
   OptAudioVolumeShell.setValue( $pref::Audio::channelVolume[$GuiAudioType]);
   OptAudioVolumeSim.setValue(   $pref::Audio::channelVolume[$SimAudioType]);
   OptAudioDriverList.clear();
   OptAudioDriverList.add("OpenAL", 1);
   OptAudioDriverList.add("none", 2);
   %selId = OptAudioDriverList.findText($pref::Audio::driver);
	if ( %selId == -1 )
		%selId = 0; // How did THAT happen?
	OptAudioDriverList.setSelected( %selId );
	OptAudioDriverList.onSelect( %selId, "" );
}

function OptionsDlg::onSleep(%this)
{
   // write out the control config into the rw/config.cs file
   ////moveMap.save( "./client/config.cs" );
}

function OptGraphicsDriverMenu::onSelect( %this, %id, %text )
{
	// Attempt to keep the same res and bpp settings:
	if ( OptGraphicsResolutionMenu.size() > 0 )
		%prevRes = OptGraphicsResolutionMenu.getText();
	else
		%prevRes = getWords( $pref::Video::resolution, 0, 1 );

	// Check if this device is full-screen only:
	if ( isDeviceFullScreenOnly( %this.getText() ) )
	{
		OptGraphicsFullscreenToggle.setValue( true );
		OptGraphicsFullscreenToggle.setActive( false );
		OptGraphicsFullscreenToggle.onAction();
	}
	else
		OptGraphicsFullscreenToggle.setActive( true );

	if ( OptGraphicsFullscreenToggle.getValue() )
	{
		if ( OptGraphicsBPPMenu.size() > 0 )
			%prevBPP = OptGraphicsBPPMenu.getText();
		else
			%prevBPP = getWord( $pref::Video::resolution, 2 );
	}

	// Fill the resolution and bit depth lists:
	OptGraphicsResolutionMenu.init( %this.getText(), OptGraphicsFullscreenToggle.getValue() );
	OptGraphicsBPPMenu.init( %this.getText() );

	// Try to select the previous settings:
	%selId = OptGraphicsResolutionMenu.findText( %prevRes );
	if ( %selId == -1 )
		%selId = 0;
	OptGraphicsResolutionMenu.setSelected( %selId );

	if ( OptGraphicsFullscreenToggle.getValue() )
	{
		%selId = OptGraphicsBPPMenu.findText( %prevBPP );
		if ( %selId == -1 )
			%selId = 0;
		OptGraphicsBPPMenu.setSelected( %selId );
		OptGraphicsBPPMenu.setText( OptGraphicsBPPMenu.getTextById( %selId ) );
	}
	else
		OptGraphicsBPPMenu.setText( "Default" );

}

function OptGraphicsResolutionMenu::init( %this, %device, %fullScreen )
{
	%this.clear();
	%resList = getResolutionList( %device );
	%resCount = getFieldCount( %resList );
	%deskRes = getDesktopResolution();

   %count = 0;
	for ( %i = 0; %i < %resCount; %i++ )
	{
		%res = getWords( getField( %resList, %i ), 0, 1 );

		if ( !%fullScreen )
		{
			if ( firstWord( %res ) >= firstWord( %deskRes ) )
				continue;
			if ( getWord( %res, 1 ) >= getWord( %deskRes, 1 ) )
				continue;
		}

		// Only add to list if it isn't there already:
		if ( %this.findText( %res ) == -1 )
      {
			%this.add( %res, %count );
         %count++;
      }
	}
}

function OptGraphicsFullscreenToggle::onAction(%this)
{
   Parent::onAction();
   %prevRes = OptGraphicsResolutionMenu.getText();

   // Update the resolution menu with the new options
   OptGraphicsResolutionMenu.init( OptGraphicsDriverMenu.getText(), %this.getValue() );

   // Set it back to the previous resolution if the new mode supports it.
   %selId = OptGraphicsResolutionMenu.findText( %prevRes );
   if ( %selId == -1 )
   	%selId = 0;
 	OptGraphicsResolutionMenu.setSelected( %selId );
}


function OptGraphicsBPPMenu::init( %this, %device )
{
	%this.clear();

	if ( %device $= "Voodoo2" )
		%this.add( "16", 0 );
	else
	{
		%resList = getResolutionList( %device );
		%resCount = getFieldCount( %resList );
      %count = 0;
		for ( %i = 0; %i < %resCount; %i++ )
		{
			%bpp = getWord( getField( %resList, %i ), 2 );

			// Only add to list if it isn't there already:
			if ( %this.findText( %bpp ) == -1 )
         {
				%this.add( %bpp, %count );
            %count++;
         }
		}
	}
}

function OptScreenshotMenu::init( %this )
{
   if( %this.findText("PNG") == -1 )
      %this.add("PNG", 0);
   if( %this.findText("JPEG") == - 1 )
      %this.add("JPEG", 1);
}

function OptFXDetailMenu::init( %this )
{
   if( %this.findText("High") == -1 )
      %this.add("High", 2);
   if( %this.findText("Medium") == - 1 )
      %this.add("Medium", 1);
   if( %this.findText("Low") == - 1 )
      %this.add("Low", 0);
}

function optionsDlg::applyGraphics( %this )
{
	%newDriver = OptGraphicsDriverMenu.getText();
	%newRes = OptGraphicsResolutionMenu.getText();
	%newBpp = OptGraphicsBPPMenu.getText();
	%newFullScreen = OptGraphicsFullscreenToggle.getValue();
	$pref::Video::screenShotFormat = OptScreenshotMenu.getText();
	$pref::AFX::fxLevelOfDetail = OptFXDetailMenu.getSelected();

	if ( %newDriver !$= $pref::Video::displayDevice )
	{
		setDisplayDevice( %newDriver, firstWord( %newRes ), getWord( %newRes, 1 ), %newBpp, %newFullScreen );
		//OptionsDlg::deviceDependent( %this );
	}
	else
		setScreenMode( firstWord( %newRes ), getWord( %newRes, 1 ), %newBpp, %newFullScreen );
}

$RemapCount = 0;

$RemapName[$RemapCount] = "MOVEMENT KEYS";
$RemapCmd[$RemapCount] = "";
$RemapCount++;
$RemapName[$RemapCount] = "Forward";
$RemapCmd[$RemapCount] = "moveforward_1";
$RemapCount++;
$RemapName[$RemapCount] = "Backward";
$RemapCmd[$RemapCount] = "movebackward_1";
$RemapCount++;
$RemapName[$RemapCount] = "Sidestep Left";
$RemapCmd[$RemapCount] = "moveleft_1";
$RemapCount++;
$RemapName[$RemapCount] = "Sidestep Right";
$RemapCmd[$RemapCount] = "moveright_1";
$RemapCount++;
$RemapName[$RemapCount] = "Turn Left";
$RemapCmd[$RemapCount] = "turnleft_1";
$RemapCount++;
$RemapName[$RemapCount] = "Turn Right";
$RemapCmd[$RemapCount] = "turnright_1";
$RemapCount++;
$RemapName[$RemapCount] = "Jump";
$RemapCmd[$RemapCount] = "jump";
$RemapCount++;

$RemapName[$RemapCount] = "";
$RemapCmd[$RemapCount] = "";
$RemapCount++;
$RemapName[$RemapCount] = "MOVEMENT KEYS (ALT)";
$RemapCmd[$RemapCount] = "";
$RemapCount++;
$RemapName[$RemapCount] = "Forward";
$RemapCmd[$RemapCount] = "moveforward_2";
$RemapCount++;
$RemapName[$RemapCount] = "Backward";
$RemapCmd[$RemapCount] = "movebackward_2";
$RemapCount++;
$RemapName[$RemapCount] = "Turn Left";
$RemapCmd[$RemapCount] = "turnleft_2";
$RemapCount++;
$RemapName[$RemapCount] = "Turn Right";
$RemapCmd[$RemapCount] = "turnright_2";
$RemapCount++;

$RemapName[$RemapCount] = "";
$RemapCmd[$RemapCount] = "";
$RemapCount++;
$RemapName[$RemapCount] = "AFX SPECIAL KEYS";
$RemapCmd[$RemapCount] = "";
$RemapCount++;
$RemapName[$RemapCount] = "Previous Spell Bank";
$RemapCmd[$RemapCount] = "spellbank_pageup";
$RemapCount++;
$RemapName[$RemapCount] = "Next Spell Bank";
$RemapCmd[$RemapCount] = "spellbank_pagedn";
$RemapCount++;
$RemapName[$RemapCount] = "Interrupt Spellcasting";
$RemapCmd[$RemapCount] = "interruptSpellcastingAction";
$RemapCount++;
$RemapName[$RemapCount] = "Cast Great Ball of Fire Spell";
$RemapCmd[$RemapCount] = "castGreatBall";
$RemapCount++;
$RemapName[$RemapCount] = "Damage Player";
$RemapCmd[$RemapCount] = "inflictDamageAction";
$RemapCount++;

$RemapName[$RemapCount] = "";
$RemapCmd[$RemapCount] = "";
$RemapCount++;
$RemapName[$RemapCount] = "CAMERA KEYS";
$RemapCmd[$RemapCount] = "";
$RemapCount++;
$RemapName[$RemapCount] = "Toggle Player/Free Camera";
$RemapCmd[$RemapCount] = "toggleCamera";
$RemapCount++;
$RemapName[$RemapCount] = "Toggle Zoom";
$RemapCmd[$RemapCount] = "toggleZoom";
$RemapCount++;
$RemapName[$RemapCount] = "Closer 3rd Person Camera";
$RemapCmd[$RemapCount] = "stepTo";
$RemapCount++;
$RemapName[$RemapCount] = "Farther 3rd Person Camera";
$RemapCmd[$RemapCount] = "stepFro";
$RemapCount++;

$RemapName[$RemapCount] = "";
$RemapCmd[$RemapCount] = "";
$RemapCount++;
$RemapName[$RemapCount] = "INTERFACE KEYS";
$RemapCmd[$RemapCount] = "";
$RemapCount++;
$RemapName[$RemapCount] = "Bring up Options Dialog";
$RemapCmd[$RemapCount] = "bringUpOptions";
$RemapCount++;
$RemapName[$RemapCount] = "Next Selectron Style";
$RemapCmd[$RemapCount] = "changeSelectronStyle";
$RemapCount++;
$RemapName[$RemapCount] = "Toggle Interface Controls";
$RemapCmd[$RemapCount] = "toggle_ui_controls";
$RemapCount++;
$RemapName[$RemapCount] = "Show Scores";
$RemapCmd[$RemapCount] = "showPlayerList";
$RemapCount++;
$RemapName[$RemapCount] = "";
$RemapCmd[$RemapCount] = "";
$RemapCount++;

$RemapName[$RemapCount] = "OTHER KEYS";
$RemapCmd[$RemapCount] = "";
$RemapCount++;
$RemapName[$RemapCount] = "Animation - Wave";
$RemapCmd[$RemapCount] = "celebrationWave";
$RemapCount++;
$RemapName[$RemapCount] = "Animation - Salute";
$RemapCmd[$RemapCount] = "celebrationSalute";
$RemapCount++;
$RemapName[$RemapCount] = "Self-Destruct";
$RemapCmd[$RemapCount] = "self_destruct";
$RemapCount++;
$RemapName[$RemapCount] = "Toggle Network Graph";
$RemapCmd[$RemapCount] = "net_graph";
$RemapCount++;


function getMapDisplayName( %device, %action )
{
	if ( %device $= "keyboard" )
		return( %action );		
	else if ( strstr( %device, "mouse" ) != -1 )
	{
		// Substitute "mouse" for "button" in the action string:
		%pos = strstr( %action, "button" );
		if ( %pos != -1 )
		{
			%mods = getSubStr( %action, 0, %pos );
			%object = getSubStr( %action, %pos, 1000 );
			%instance = getSubStr( %object, strlen( "button" ), 1000 );
			return( %mods @ "mouse" @ ( %instance + 1 ) );
		}
		else
			error( "Mouse input object other than button passed to getDisplayMapName!" );
	}
	else if ( strstr( %device, "joystick" ) != -1 )
	{
		// Substitute "joystick" for "button" in the action string:
		%pos = strstr( %action, "button" );
		if ( %pos != -1 )
		{
			%mods = getSubStr( %action, 0, %pos );
			%object = getSubStr( %action, %pos, 1000 );
			%instance = getSubStr( %object, strlen( "button" ), 1000 );
			return( %mods @ "joystick" @ ( %instance + 1 ) );
		}
		else
	   { 
	      %pos = strstr( %action, "pov" );
         if ( %pos != -1 )
         {
            %wordCount = getWordCount( %action );
            %mods = %wordCount > 1 ? getWords( %action, 0, %wordCount - 2 ) @ " " : "";
            %object = getWord( %action, %wordCount - 1 );
            switch$ ( %object )
            {
               case "upov":   %object = "POV1 up";
               case "dpov":   %object = "POV1 down";
               case "lpov":   %object = "POV1 left";
               case "rpov":   %object = "POV1 right";
               case "upov2":  %object = "POV2 up";
               case "dpov2":  %object = "POV2 down";
               case "lpov2":  %object = "POV2 left";
               case "rpov2":  %object = "POV2 right";
               default:       %object = "??";
            }
            return( %mods @ %object );
         }
         else
            error( "Unsupported Joystick input object passed to getDisplayMapName!" );
      }
	}
		
	return( "??" );		
}

function buildFullMapString( %index )
{
   %name       = $RemapName[%index];
   %cmd        = $RemapCmd[%index];

	%temp = moveMap.getBinding( %cmd );
   %device = getField( %temp, 0 );
   %object = getField( %temp, 1 );
   if ( %device !$= "" && %object !$= "" )
	   %mapString = getMapDisplayName( %device, %object );
   else
      %mapString = "";

	return( %name TAB %mapString );
}

function OptRemapList::fillList( %this )
{
	%this.clear();
   for ( %i = 0; %i < $RemapCount; %i++ )
      %this.addRow( %i, buildFullMapString( %i ) );
}

// Audio 
function OptAudioUpdate()
{
   // set the driver text
   %text =   "Vendor: " @ alGetString("AL_VENDOR") @
           "\nVersion: " @ alGetString("AL_VERSION") @
           "\nRenderer: " @ alGetString("AL_RENDERER") @
           "\nExtensions: " @ "see console log";
   OptAudioInfo.setText(%text);

}


// Channel 0 is unused in-game, but is used here to test master volume.

new AudioDescription(AudioChannel0)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = 0;
};

new AudioDescription(AudioChannel1)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = 1;
};

new AudioDescription(AudioChannel2)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = 2;
};

new AudioDescription(AudioChannel3)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = 3;
};

new AudioDescription(AudioChannel4)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = 4;
};

new AudioDescription(AudioChannel5)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = 5;
};

new AudioDescription(AudioChannel6)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = 6;
};

new AudioDescription(AudioChannel7)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = 7;
};

new AudioDescription(AudioChannel8)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = 8;
};

$AudioTestHandle = 0;

function OptAudioUpdateMasterVolume(%volume)
{
   if (%volume == $pref::Audio::masterVolume)
      return;
   alxListenerf(AL_GAIN_LINEAR, %volume);
   $pref::Audio::masterVolume = %volume;
   if (!alxIsPlaying($AudioTestHandle))
   {
      $AudioTestHandle = alxCreateSource("AudioChannel0", expandFilename("~/data/sound/testing.wav"));
      alxPlay($AudioTestHandle);
   }
}

function OptAudioUpdateChannelVolume(%channel, %volume)
{
   if (%channel < 1 || %channel > 8)
      return;
         
   if (%volume == $pref::Audio::channelVolume[%channel])
      return;

   alxSetChannelVolume(%channel, %volume);
   $pref::Audio::channelVolume[%channel] = %volume;
   if (!alxIsPlaying($AudioTestHandle))
   {
      $AudioTestHandle = alxCreateSource("AudioChannel"@%channel, expandFilename("~/data/sound/testing.wav"));
      alxPlay($AudioTestHandle);
   }
}


function OptAudioDriverList::onSelect( %this, %id, %text )
{
   if (%text $= "")
      return;
   
   if ($pref::Audio::driver $= %text)
      return;

   $pref::Audio::driver = %text;
   OpenALInit();
}
