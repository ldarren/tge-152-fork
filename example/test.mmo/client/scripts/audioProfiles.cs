//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Channel assignments (channel 0 is unused in-game).

$GuiAudioType     = 1;
$SimAudioType     = 2;
$MessageAudioType = 3;
	
if ($pref::Game::soundOn == 0) alxSetChannelVolume(2, 0);
if ($pref::Game::effectOn == 0) alxSetChannelVolume(1, 0);

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
   filename = "~/data/sound/startup.ogg";
   description = "AudioGui";
   preload = true;
};

new AudioProfile(GameButtonHoverAudio)
{
   fileName = "~/data/sound/buttonOver.ogg";
   description = "AudioGui";
   preload = true;
};

new AudioProfile(GameButtonDownAudio)
{
   fileName = "~/data/sound/buttonDown.ogg";
   description = "AudioGui";
   preload = true;
};

new AudioProfile(GameCheckDownAudio)
{
   fileName = "~/data/sound/ButtonCheck.ogg";
   description = "AudioGui";
   preload = true;
};
