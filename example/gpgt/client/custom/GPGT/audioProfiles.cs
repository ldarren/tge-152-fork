//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

echo("\n\c3--------- loading GPGT Volume 1 Audio Profiles ---------"); 
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
   isLooping   = fales;
   is3D        = true;
   type        = $SimAudioType;
};



