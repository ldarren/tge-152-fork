//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Channel assignments (channel 0 is unused in-game).

$GuiAudioType     = 1;
$SimAudioType     = 2;
$MessageAudioType = 3;
	
if ($pref::LTA::soundOn == 0) alxSetChannelVolume(2, 0);
if ($pref::LTA::effectOn == 0) alxSetChannelVolume(1, 0);

new AudioDescription(AudioGui)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = $GuiAudioType;
};

new AudioDescription(AudioMessage)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = $MessageAudioType;
};

new AudioProfile(AudioStartup)
{
   filename = "LTA/data/sound/startup.ogg";
   description = "AudioGui";
   preload = true;
};

new AudioProfile(LTAButtonHoverAudio)
{
   fileName = "LTA/data/sound/buttonOver.ogg";
   description = "AudioGui";
   preload = true;
};

new AudioProfile(LTAButtonDownAudio)
{
   fileName = "LTA/data/sound/buttonDown.ogg";
   description = "AudioGui";
   preload = true;
};

new AudioProfile(LTACheckDownAudio)
{
   fileName = "LTA/data/sound/ButtonCheck.ogg";
   description = "AudioGui";
   preload = true;
};
