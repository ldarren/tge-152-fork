//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

//-----------------------------------------------------------------------------
// Base NonLooping 2D Audio Description Datablock
//-----------------------------------------------------------------------------
datablock AudioDescription(BaseNonLooping2DADDB)
{
   volume            = 1.0;
   isLooping         = false;

   is3D              = false;
   
   type              = $SimAudioType; 
};

//-----------------------------------------------------------------------------
// Base Looping 2D Audio Description Datablock
//-----------------------------------------------------------------------------
datablock AudioDescription(BaseLooping2DADDB)
{
   volume            = 1.0;
   isLooping         = true;
   
   is3D              = false;
   
   type              = $SimAudioType; 
};

//-----------------------------------------------------------------------------
// Base NonLooping 3D Audio Description Datablock
//-----------------------------------------------------------------------------
datablock AudioDescription(BaseNonLooping3DADDB)
{
   volume            = 1.0;
   isLooping         = false;

   is3D              = true;
   ReferenceDistance = 10.0;
   MaxDistance       = 400.0;
   
   type              = $SimAudioType; 
};

//-----------------------------------------------------------------------------
// Base Looping 3D Audio Description Datablock
//-----------------------------------------------------------------------------
datablock AudioDescription(BaseLooping3DADDB)
{
   volume            = 1.0;
   isLooping         = true;
   
   is3D              = true;
   ReferenceDistance = 10.0;
   MaxDistance       = 400.0;
   
   type              = $SimAudioType; 
};

