// NOTE: All sound datablocks here are not used since we use a general looping
// ambience sound. Left here not to bloat but in case in the future need to use
// the proper methods
//

//-------------- Sound for Rain -------------------------------------
datablock AudioProfile(HeavyRainSound)
{
   filename    = "~/data/sound/env/rain.ogg";
   description = "AudioDefaultLooping3d";
   preload = false;
};


datablock AudioProfile(takeme)
{
   filename = "~/data/sound/takeme.wav";
   description = "AudioDefaultLooping3d";
	preload = false;
};


//-------------------- RAIN (NOT WORKING CORRECTLY)-----------------------------------------
datablock PrecipitationData(Rain) 
{ 
   type = 1; 
   materialList = "~/data/env/mmrain.dml"; 
   soundProfile = "HeavyRainSound";
   sizeX = 0.10; 
   sizeY = 0.10; 

   movingBoxPer = 0.35; 
   divHeightVal = 1.5; 
   sizeBigBox = 1; 
   topBoxSpeed = 20; 
   frontBoxSpeed = 30; 
   topBoxDrawPer = 0.5; 
   bottomDrawHeight = 40; 
   skipIfPer = -0.3; 
   bottomSpeedPer = 1.0; 
   frontSpeedPer = 1.5; 
   frontRadiusPer = 0.5; 
};

//-------------------- HEAVYRAIN -----------------------------------------
datablock PrecipitationData(HeavyRain)
{
  // soundProfile = "HeavyRainSound";

   dropTexture = "~/data/env/mmrain";
   splashTexture = "~/data/env/mmsplash";
   dropSize = 0.75;
   splashSize = 0.2;
   useTrueBillboards = false;
   splashMS = 250;
};

//-------------------- HEAVYRAIN2 -----------------------------------------
datablock PrecipitationData(HeavyRain2)
{
   dropTexture =  "~/data/env/mmmist";
   splashTexture = "~/data/env/mmmist2";
   dropSize = 10;
   splashSize = 0.1;
   useTrueBillboards = false;
   splashMS = 250;
};


 //-------------------------THUNDER------------------------------------------
/*
datablock AudioProfile(ThunderCrash1Sound)
{
   filename  = "~/data/sound/env/thunder1.ogg";
   description = Audio2d;
};

datablock AudioProfile(ThunderCrash2Sound)
{
   filename  = "~/data/sound/env/thunder2.ogg";
   description = Audio2d;
};

datablock AudioProfile(ThunderCrash3Sound)
{
   filename  = "~/data/sound/env/thunder3.ogg";
   description = Audio2d;
};

datablock AudioProfile(ThunderCrash4Sound)
{
   filename  = "~/data/sound/env/thunder4.ogg";
   description = Audio2d;
};

datablock AudioProfile(LightningHitSound)
{
   filename  = "~/data/sound/crossbow_explosion.ogg";
   description = AudioThunder3d;//AudioLightning3d;
};
*/
datablock LightningData(LightningStorm)
{
   strikeTextures[0]  = "~/data/env/mmlight1";
   strikeTextures[1]  = "~/data/env/mmlight2";
   strikeTextures[2]  = "~/data/env/mmlight3";
   
   //strikeSound = LightningHitSound;
   //thunderSounds[0] = ThunderCrash1Sound;
   //thunderSounds[1] = ThunderCrash2Sound;
   //thunderSounds[2] = ThunderCrash3Sound;
   //thunderSounds[3] = ThunderCrash4Sound;
};

