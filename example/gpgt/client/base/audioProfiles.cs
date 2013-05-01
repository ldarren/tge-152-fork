//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
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

new AudioDescription(AudioMessage)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = $MessageAudioType;
};

new AudioProfile(AudioButtonOver)
{
   filename = "~/data/sound/buttonOver.wav";
   description = "AudioGui";
	preload = true;
};

if(!isObject(AudioGuiLooping)) new AudioDescription(AudioGuiLooping)
{
   volume   = 1.0;
   isLooping= true;
   is3D     = false;
   type     = $GuiAudioType;
};

if(!isObject(AudioSim3DLooping)) new AudioDescription(AudioSim3DLooping)
{
   volume      = 1.0;
   isLooping   = true;
   is3D        = true;
   type        = $SimAudioType;
};

if(!isObject(AudioSim3D)) new AudioDescription(AudioSim3D)
{
   volume      = 1.0;
   isLooping   = false;
   is3D        = true;
   type        = $SimAudioType;
};


