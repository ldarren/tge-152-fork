//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Channel assignments (channel 0 is unused in-game).

$GuiAudioType     	= 1;
$SimAudioType     	= 2;
$MessageAudioType 	= 3;
$MusicAudioType		= 4;

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

new AudioDescription(AudioMusic)
 {
   volume   = 1.0;
   //isStreaming = true;
   isLooping= true;
   is3D     = false;
   type     = $MusicAudioType;
};

new AudioDescription(AudioMusic1)
 {
   volume   = 1.0;
   //isStreaming = true;
   isLooping= true;
   is3D     = false;
   type     = $MusicAudioType;
};


//----------------------------------------------------------------------
//	'Parked' here since looping sound is faster produced on the client side
//----------------------------------------------------------------------

new AudioProfile(InGameMusic)
 {
   filename    = "~/data/sound/BgMusic7.ogg";//"~/data/sound/BgMusic2.ogg";
   description = "AudioMusic";
   preload = false;
 };

new AudioProfile(InGameSFX)
 {
   filename    = "~/data/sound/ThunderStorm.ogg";
   description = "AudioMusic1";
   preload = false;
 };