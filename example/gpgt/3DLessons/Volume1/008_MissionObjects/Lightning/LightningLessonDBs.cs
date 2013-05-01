//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// Audio Description - 2D Looping
//-----------------------------------------------------------------------------
datablock AudioDescription(AD_2DNonLooping)
{
   volume = 1.0;
   isLooping = false;
   is3D = false;
   type = 2; //$SimAudioType
};

datablock AudioDescription(AD_LightningStrike)
{
   volume   = 1.0;
   isLooping= false;

   is3D     = true;
   ReferenceDistance= 10.0;
   MaxDistance= 400.0;
   type = 2; //$SimAudioType
};


 //-----------------------------------------------------------------------------

datablock AudioProfile(ThunderSound0)
{
   filename  = "~/data/GPGTBase/sound/thunder1.ogg";
   description = AD_LightningStrike;
//   description = AD_2DNonLooping;
};

datablock AudioProfile(ThunderSound1)
{
   filename  = "~/data/GPGTBase/sound/thunder2.ogg";
   description = AD_LightningStrike;
//   description = AD_2DNonLooping;
};

datablock AudioProfile(LightningStrikeSound)
{
   filename  = "~/data/Sound/lightningstrike.wav";
   description = AD_LightningStrike;
};

datablock LightningData(GeneratedLightningExample)
{
	  // strikeTextures are unused.
   
   // Play this sound when lightning strikes!
   strikeSound = LightningStrikeSound;

	  // Up to eight thunder sounds can be defined
   thunderSounds[0] = ThunderSound0;
   thunderSounds[1] = ThunderSound1;
   thunderSounds[2] = ThunderSound0;
   thunderSounds[3] = ThunderSound1;
};

datablock WeatherLightningData(TexturedLightningExample)
{
   // Up to eight Lightning textures can be defined
   strikeTextures[0] = "./data/lightningFrame1";
   strikeTextures[1] = "./data/lightningFrame2";
   strikeTextures[2] = "./data/lightningFrame3";

   // Up to eight lighting origin flashes can be defined   
   flashTextures[0]  = "./data/flash";
   
   fuzzyTextures[0] = "./data/lightningFuzzy1";
   fuzzyTextures[1] = "./data/lightningFuzzy2";
   fuzzyTextures[2] = "./data/lightningFuzzy3";
   

   // Play this sound when lightning strikes!
   strikeSound = LightningStrikeSound;

	  // Up to eight thunder sounds can be defined
   thunderSounds[0] = ThunderSound0;
   thunderSounds[1] = ThunderSound1;
};

