//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


//Env Pack: Heavy Rain Datablock

// Look to starter.fps for proper scripting blocks, these are demo specific
//datablock AudioProfile(HeavyRainSound)
//{
//   filename    = "~/data/sound/amb.ogg";
//   description = AudioLooping2d;
//};

datablock PrecipitationData(HeavyRain2)
{
   dropTexture = "~/data/environment/mist";
   splashTexture = "~/data/environment/mist2";
   dropSize = 10;
   splashSize = 0.1;
   useTrueBillboards = false;
   splashMS = 250;
};

datablock PrecipitationData(HeavyRain3)
{
   dropTexture = "~/data/environment/shine";
   splashTexture = "~/data/environment/mist2";
   dropSize = 20;
   splashSize = 0.1;
   useTrueBillboards = false;
   splashMS = 250;
};

datablock PrecipitationData(HeavyRain)
{
   //soundProfile = "HeavyRainSound";

   dropTexture = "~/data/environment/rain";
   splashTexture = "~/data/environment/water_splash";
   dropSize = 0.35;
   splashSize = 0.1;
   useTrueBillboards = false;
   splashMS = 500;
};

//-----------------------------------------------------------------------------

datablock AudioProfile(ThunderCrash1Sound)
{
   filename  = "~/data/sound/environment/ambient/thunder1.ogg";
   description = Audio2d;
};

datablock AudioProfile(ThunderCrash2Sound)
{
   filename  = "~/data/sound/environment/ambient/thunder2.ogg";
   description = Audio2d;
};

datablock AudioProfile(ThunderCrash3Sound)
{
   filename  = "~/data/sound/environment/ambient/thunder3.ogg";
   description = Audio2d;
};

datablock AudioProfile(ThunderCrash4Sound)
{
   filename  = "~/data/sound/environment/ambient/thunder4.ogg";
   description = Audio2d;
};

//datablock AudioProfile(LightningHitSound)
//{
//   filename  = "~/data/sound/crossbow_explosion.ogg";
//   description = AudioLightning3d;
//};

datablock LightningData(LightningStorm)
{
   strikeTextures[0]  = "demo/data/environment/lightning1frame1";
   strikeTextures[1]  = "demo/data/environment/lightning1frame2";
   strikeTextures[2]  = "demo/data/environment/lightning1frame3";
   
   //strikeSound = LightningHitSound;
   thunderSounds[0] = ThunderCrash1Sound;
   thunderSounds[1] = ThunderCrash2Sound;
   thunderSounds[2] = ThunderCrash3Sound;
   thunderSounds[3] = ThunderCrash4Sound;
};

datablock PrecipitationData(Leaves)
{
   //soundProfile = "Blackforest";

   dropTexture = "~/data/environment/leaves";
   //splashTexture = "~/data/environment/deathrain_splash";
   dropSize = 0.35;
   splashSize = 0.1;
   useTrueBillboards = false;
   splashMS = 500;
};

