function OptionsDlg::onWake(%this)
{
	OptSoloGamePage.setActive(false);
	OptNetworkGamePage.setActive(false);
	OptOnlineGamePage.setActive(false);
	switch(LuckyStarClient.GameMode)
	{
	case -1: // lobby
		OptBook.selectPage(3);
	case 1: // solo
		OptBook.selectPage(0);
	case 2: // network
		OptBook.selectPage(1);
	case 3: // online
		OptBook.selectPage(2);
	}
   // Graphics
   OptGraphicsResolutionMenu.init(OptGraphicsFullscreenToggle.getValue());
   OptGraphicsBPPMenu.init();
   OptScreenshotMenu.init();
   OptScreenshotMenu.setValue($pref::Video::screenShotFormat);

   // Audio 
   OptAudioVolumeMaster.setValue($pref::Audio::masterVolume);
   OptAudioVolumeShell.setValue( $pref::Audio::channelVolume[$GuiAudioType]);
   OptAudioVolumeSim.setValue(   $pref::Audio::channelVolume[$SimAudioType]);
   OptAudioVolumeMsg.setValue(   $pref::Audio::channelVolume[$MessageAudioType]);
}

function OptGraphicsResolutionMenu::init( %this, %fullScreen )
{
	%this.clear();
	%resList = getResolutionList( $pref::Video::displayDevice );
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

	%selId = %this.findText( getWords( $pref::Video::resolution, 0, 1 ) );
	if ( %selId == -1 ) %selId = 0;
	%this.setSelected( %selId );
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


function OptGraphicsBPPMenu::init( %this )
{
	%this.clear();

	%resList = getResolutionList( $pref::Video::displayDevice );
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
	%selId = %this.findText( getWord( $pref::Video::resolution, 2 ) );
	if ( %selId == -1 )	%selId = 0;
	%this.setSelected( %selId );
	//%this.setText( %this.getTextById( %selId ) );
}

function OptScreenshotMenu::init( %this )
{
   if( %this.findText("PNG") == -1 )
      %this.add("PNG", 0);
   if( %this.findText("JPEG") == - 1 )
      %this.add("JPEG", 1);
}

function optionsDlg::applyGraphics( %this )
{
	%newRes = OptGraphicsResolutionMenu.getText();
	%newBpp = OptGraphicsBPPMenu.getText();
	%newFullScreen = OptGraphicsFullscreenToggle.getValue();
	$pref::Video::screenShotFormat = OptScreenshotMenu.getText();

	setScreenMode( firstWord( %newRes ), getWord( %newRes, 1 ), %newBpp, %newFullScreen );
}

// Audio 


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
   if (%volume $= "" || %volume == $pref::Audio::masterVolume)
      return;
   alxListenerf(AL_GAIN_LINEAR, %volume);
   $pref::Audio::masterVolume = %volume;
   if (!alxIsPlaying($AudioTestHandle))
   {
      $AudioTestHandle = alxCreateSource("AudioChannel0", expandFilename("~/sound/testing.wav"));
      alxPlay($AudioTestHandle);
   }
}

function OptAudioUpdateChannelVolume(%channel, %volume)
{
   if (%channel $= "" || %channel < 1 || %channel > 8)
      return;
         
   if (%volume $= "" || %volume == $pref::Audio::channelVolume[%channel])
      return;

   alxSetChannelVolume(%channel, %volume);
   $pref::Audio::channelVolume[%channel] = %volume;
   if (!alxIsPlaying($AudioTestHandle))
   {
      $AudioTestHandle = alxCreateSource("AudioChannel"@%channel, expandFilename("~/sound/testing.wav"));
      alxPlay($AudioTestHandle);
   }
}

