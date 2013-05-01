// Green Field
datablock PrecipitationData(LeavesData)
{
   //soundProfile = "Blackforest";

   dropTexture = "~/data/environment/leaves02";
   //splashTexture = "~/data/environment/deathrain_splash";
   dropSize = 0.35;
   splashSize = 0.1;
   useTrueBillboards = false;
   splashMS = 500;
};

// Arctic Zone
datablock PrecipitationData(HeavyRain2)
{
   dropTexture = "~/data/environment/mist";
   splashTexture = "~/data/environment/mist2";
   dropSize = 10;
   splashSize = 0.1;
   useTrueBillboards = false;
   splashMS = 250;
};

datablock PrecipitationData(HeavySnow)
{
    dropTexture = "~/data/environment/snow";
   splashTexture = "~/data/environment/snow";
   dropSize = 0.27;
   splashSize = 0.27;
   useTrueBillboards = false;
   splashMS = 50;
};


//-=-=-=-=-=-=-=- // Desert Storm

datablock AudioProfile(Sandstormsound)
{
   filename    = "~/data/sounds/waste.ogg";
   description = AudioLooping2d;
   volume   = 0.5;
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
   filename    = "~/data/sounds/dust.ogg";
   description = AudioLooping2d;
   volume   = 0.5;
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
