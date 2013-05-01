//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
// Put sound definitions here.

datablock AudioDescription( Looping3DAD )
{
   volume            = 1.0;
   isLooping         = true;
   is3D              = true;
   ReferenceDistance = 10.0;
   MaxDistance       = 80.0;
   type              = 2; 
};

datablock AudioProfile( MonoTest )
{
   filename = "./data/drip_mono16.ogg";
   description = Looping3DAD;
   preload = true;
};

datablock AudioProfile( StereoTest )
{
   filename = "./data/rain_stereo16.ogg";
   description = Looping3DAD;
   preload = true;
};

datablock StaticShapeData( SoundEgg )
{
	category				= "LessonShapes";
	shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
};
