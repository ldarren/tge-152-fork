//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
// Audio Description - 2D Looping
//-----------------------------------------------------------------------------
datablock AudioDescription(AD2DLooping)
{
   volume      = 1.0;
   isLooping   = true;
   is3D        = false;
   type        = $SimAudioType;
};


//-----------------------------------------------------------------------------
// Raining Cats n' Dogs
//-----------------------------------------------------------------------------
datablock AudioProfile(WoofMeowRainAP)
{
   filename    = "./data/catndogs.ogg";
   description = AD2DLooping;
   minLoopGap  = 1000;
   maxLoopGap  = 5000;
};

datablock PrecipitationData(RaininCatsNDogs)
{
   soundProfile = "WoofMeowRainAP";

   dropTexture = "./data/catsndogs";
   dropSize = 1.0;
   splashSize = 1.0;
   useTrueBillboards = false;
   splashMS = 250;
};


//-----------------------------------------------------------------------------
// Snowing
//-----------------------------------------------------------------------------

datablock PrecipitationData(Snowfall)
{
   dropTexture = "./data/snowflakes";
   dropSize = 0.33;
   useTrueBillboards = false;
};


//-----------------------------------------------------------------------------
// Cartoon Rain
//-----------------------------------------------------------------------------

datablock AudioProfile(RainFallAP)
{
   filename    = "./data/rain.ogg";
   description = AD2DLooping;
};

datablock PrecipitationData(CartoonRain) 
{
   soundProfile = "RainFallAP";

   dropTexture = "./data/drops0";
   dropSize = 0.5;
   splashSize = 0.5;
   useTrueBillboards = false;
   splashMS = 250;
};

