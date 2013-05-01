//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


datablock AudioProfile(HeavyRainSound)
{
   filename    = "~/data/sound/amb.ogg";
   description = AudioLooping2d;
};

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
   soundProfile = "HeavyRainSound";

   dropTexture = "~/data/environment/rain";
   splashTexture = "~/data/environment/water_splash";
   dropSize = 0.35;
   splashSize = 0.1;
   useTrueBillboards = false;
   splashMS = 500;
};


//-=-=-=-=-=-=-=-

datablock AudioProfile(Sandstormsound)
{
   filename    = "~/data/sound/waste.ogg";
   description = AudioLooping2d;
   volume   = 1.0;
};

datablock PrecipitationData(Sandstorm)
{
   soundProfile = "Sandstormsound";

   dropTexture = "~/data/environment/sandstorm";
   splashTexture = "~/data/environment/sandstorm2";
   dropSize = 10;
   splashSize = 2;
   useTrueBillboards = false;
   splashMS = 250;
};

datablock AudioProfile(dustsound)
{
   filename    = "~/data/sound/dust.ogg";
   description = AudioLooping2d;
};

datablock PrecipitationData(dustspecks)
{
   soundProfile = "dustsound";

   dropTexture = "~/data/environment/dust";
   splashTexture = "~/data/environment/dust2";
   dropSize = 0.25;
   splashSize = 0.25;
   useTrueBillboards = false;
   splashMS = 250;
};

//-=-=-=-=-=-=-=-



datablock PrecipitationData(HeavySnow)
{
    dropTexture = "~/data/environment/snow";
   splashTexture = "~/data/environment/snow";
   dropSize = 0.27;
   splashSize = 0.27;
   useTrueBillboards = false;
   splashMS = 50;
};