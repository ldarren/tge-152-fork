//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
////
// Constant Light Item Datablocks
////
datablock ItemData( ConstantLightEgg0 )
{
	category				= "LessonShapes";
	shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
	lightColor				= "1 0 0 1.0";
	lightRadius				= 3.0;
	lightType				= "ConstantLight";
};

datablock ItemData( ConstantLightEgg1 : ConstantLightEgg0 )
{
	lightColor				= "0 1 0 1.0";
};

datablock ItemData( ConstantLightEgg2 : ConstantLightEgg0 )
{
	lightColor				= "0 0 1 1.0";
};

////
// Pulsing Light Item Datablocks
////
datablock ItemData( PulsingLightEgg0 )
{
	category				 = "LessonShapes";
	shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
	lightColor			= "1 0 0 1.0";
	lightRadius		= 3.0;
	lightType				= "PulsingLight";
	lightTime				= 1500;
};

datablock ItemData( PulsingLightEgg1 : PulsingLightEgg0 )
{
	lightColor				= "0 1 0 1.0";
};

datablock ItemData( PulsingLightEgg2 : PulsingLightEgg0 )
{
	lightColor				= "0 0 1 1.0";
};


////
// Fade and Hide Constant Light Item Datablocks
////
datablock ItemData( FadeAndHideConstantLightEgg )
{
	category				= "LessonShapes";
	shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
	lightColor				= "1 1 0 1";
	lightRadius				= 2.0;
	lightType				= "ConstantLight";
};


////
// Cloaked Constant Light Item Datablocks
////
datablock ItemData( CloakedConstantLightEgg )
{
	category				= "LessonShapes";
	shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
	lightColor				= "1 1 0 1";
	lightRadius				= 2.0;
	lightType				= "ConstantLight";
};

////
// Cloaked Pulsing Light Item Datablocks
////
datablock ItemData( CloakedPulsingLightEgg )
{
	category				= "LessonShapes";
	shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
	lightColor				= "1 1 0 1";
	lightRadius				= 2.0;
	lightType				= "PulsingLight";
	lightTime				= 750;

};




//
return;
datablock ItemData( SampleItem : BaseItem )
{
	category				= "TestShapes";
	//
	// Item Fields
	//
	elasticity				= 1.0;	
	friction				= 1.0;
	gravityMod				= 1.0;
	lightColor				= "0.0 1.0 0.6 0.0";
	lightOnlyStatic			= false;
	lightRadius				= 2.0;
	lightTime				= 3000;
	lightType				= "ConstantLight";
	maxVelocity				= 10.0;
	pickupName				= "Sample Item";
	sticky					= true; 
	//-------------------------------------------------------------------------
	// From GameBaseData
	//-------------------------------------------------------------------------
	className				= "SampleItems"; // Associate with new namespace for onAdd, etc
};


