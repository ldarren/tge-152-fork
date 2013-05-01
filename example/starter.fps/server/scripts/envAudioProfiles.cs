//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
// Environment Audio Profiles
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 3D Sounds
//-----------------------------------------------------------------------------


datablock AudioDescription(Shore01Looping3d)
{
  volume   = 1.0;
  isLooping= true;

  is3D     = true;
  ReferenceDistance = 20;
  maxDistance = 65;
  type     = $EnvAudioType;
};

datablock AudioDescription(TreeGrove01Looping3d)
{
  volume   = 1.0;
  isLooping= true;

  is3D     = true;
  ReferenceDistance = 20;
  maxDistance = 80;
  type     = $EnvAudioType;
};

datablock AudioDescription(Tree01Looping3d)
{
  volume   = 1.0;
  isLooping= true;

  is3D     = true;
  ReferenceDistance = 20;
  maxDistance = 60;
  type     = $EnvAudioType;
};

datablock AudioDescription(Fire01Looping3d)
{
  volume   = 1.0;
  isLooping= true;

  is3D     = true;
  ReferenceDistance = 3;
  maxDistance = 10;
  type     = $EnvAudioType;
};

//-----------------------------------------------------------------------------


datablock AudioProfile(Shore01Snd)
{
   fileName = "~/data/sound/Lakeshore_mono_01.ogg";
   description = "Shore01Looping3d";
	 preload = true;
};

datablock AudioProfile(TreeGrove01Snd)
{
   fileName = "~/data/sound/treegrove_mono_01.ogg";
   description = "TreeGrove01Looping3d";
	 preload = true;
};

datablock AudioProfile(Tree01Snd)
{
   fileName = "~/data/sound/tree_mono_01.ogg";
   description = "Tree01Looping3d";
	 preload = true;
};

datablock AudioProfile(Fire01Snd)
{
   fileName = "~/data/sound/Fire_Mono_01.ogg";
   description = "Fire01Looping3d";
	 preload = true;
};