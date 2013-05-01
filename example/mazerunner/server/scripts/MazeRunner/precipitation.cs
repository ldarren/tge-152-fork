//------------------------------------------------------
// Copyright 2000-2005, GarageGames.com, Inc.
// Written, modfied, or otherwise interpreted by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

//-----------------------------------------------------------------------------
// Audio Description - 2D Looping
//-----------------------------------------------------------------------------
datablock AudioDescription(MazeRunner2DLooping)
{
   volume      = 0.7;
   isLooping   = true;
   is3D        = false;
   type        = $SimAudioType;
};

//-----------------------------------------------------------------------------
// MazeRunner Rain
//-----------------------------------------------------------------------------

datablock AudioProfile(MazeRunnerRainSound)
{
   filename    = "~/data/precipitation/rain2.ogg";
   description = MazeRunner2DLooping;
};

datablock PrecipitationData(MazeRunnerRain) 
{
   soundProfile      = "MazeRunnerRainSound";

   dropTexture       = "~/data/precipitation/drops0";
   dropSize          = 0.5;
   //splashSize        = 1.0;
   useTrueBillboards = false;
   splashMS          = 500;
};

