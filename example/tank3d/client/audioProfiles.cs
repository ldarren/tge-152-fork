//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Channel assignments (channel 0 is unused in-game).

$GuiAudioType     = 1;
$SimAudioType     = 2;
$MessageAudioType = 3;
$MusicAudioType	= 4;

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


new AudioDescription(GameFxNoLoopSet)
 {
   volume   = 1.0;
   //isStreaming = true;
   isLooping= false;
   is3D     = false;
   type     = $SimAudioType;
};


new AudioDescription(GameFxLoopSet)
 {
   volume   = 1.0;
   //isStreaming = true;
   isLooping= true;
   is3D     = false;
   type     = $SimAudioType;
};

new AudioDescription(AudioDefault3dCl)
{
   volume   = 1.0;
   isLooping= false;

   is3D     = true;
   ReferenceDistance= 20.0;
   MaxDistance= 100.0;
   type     = $SimAudioType;
};

//----------------------------------------------------------------------
//	'Parked' here since looping sound is faster produced on the client side
//----------------------------------------------------------------------

new AudioProfile(InGameMusic)
 {
   filename    = "~/data/sound/Leningrad_March.ogg";
   description = "AudioMusic";
   preload = false;
 };

new AudioProfile(InGameFxExplodeHit)
{
   //used in moogun.cs inside MoogunProjectile::onCollision
   filename    = "~/data/sound/projectile_hit.ogg";
   description = "GameFxNoLoopSet";
   preload = true;
};

new AudioProfile(InGameFxSpawn)
{
  //used in spawn.cs inside spawnPlayer, commented out due to the sound not really fiting in with the rest of the game theme
   filename    = "~/data/sound/spawn5.ogg";
   description = "GameFxNoLoopSet";
   preload = true;
};

new AudioProfile(InGameFxTurn)
{
   //used in inside rotate commented out due to overlaping problem, if used once the sound is too short, if inside rotate loop its overlapping.
   filename    = "~/data/sound/tankrotate5.ogg";
   description = "GameFxNoLoopSet";//"AudioDefault3dCl";
   preload = true;
};