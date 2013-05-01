//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Channel assignments (channel 0 is unused in-game).

$GuiAudioType     = 1;
$SimAudioType     = 2;
$MessageAudioType = 3;

new AudioDescription(AudioGui)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = $GuiAudioType;
};

new AudioDescription(MusicGui)
{
   volume   = 1.0;
   isLooping= true;
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
/*
new AudioProfile(AudioButtonOver)
{
   filename = "~/data/sound/buttonOver.wav";
   description = "AudioGui";
	preload = true;
};
*/

new AudioProfile(AudioStartup)
{
	filename = "~/data/sound/IntroMYGOlogo.ogg";
   description = "AudioGui";
	preload = false;
};
new AudioProfile(AudioMainMenu)
{
	filename = "~/data/sound/menuBGmusic.ogg";
   description = "MusicGui";
	preload = false;
};
new AudioProfile(AudioPlayGui)
{
	filename = "~/data/sound/GameMusic.ogg";
   description = "MusicGui";
	preload = false;
};
