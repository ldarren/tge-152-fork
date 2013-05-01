//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
////
// Static Item Datablock
////
datablock ItemData( StaticEgg )
{
	category				= "LessonShapes";
	shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
};

////
// Non-Static Item Datablock
////
datablock ItemData( NonStaticEgg )
{
	category				= "LessonShapes";
	shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
};


////
// Sticky Item Datablock
////
datablock ItemData( StickyEgg : NonStaticEgg)
{
	sticky					= true; 
	
	elasticity = 0.5;


	// Give this a light to make it easier to see
	lightColor				= "0.8 1 1 1";
	lightRadius				= 2.0;
	lightType				= "ConstantLight";
};

////
// Non-Sticky Item Datablock
////
datablock ItemData( NonStickyEgg : StickyEgg)
{
	sticky					= false; 
};

////
// Rotating Item Datablock
////
datablock ItemData( RotatingEgg : StaticEgg )
{
	// Give this a light to make it easier to see
	lightColor				= "1 0 1 1";
	lightRadius			= 2.0;
	lightType				 = "ConstantLight";
};


////
// Zero Friction Item Datablock
////
datablock ItemData( ZeroFrictionEgg )
{
   category				= "LessonShapes";
   shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";
   
   friction = 0.0;
   
   mass = 10;
   
   // Give this a light to make it easier to see
   lightColor				= "1 1 1 1";
   lightRadius			= 2.0;
   lightType				 = "ConstantLight";
};

datablock ItemData( MediumFrictionEgg : ZeroFrictionEgg)
{
	friction = 0.3;
};

datablock ItemData( HighFrictionEgg : ZeroFrictionEgg)
{
	friction = 2.0;
};

datablock ItemData( NegativeFrictionEgg : ZeroFrictionEgg)
{
	friction    = -2.0;
};

////
// Zero Gravity Egg Item Datablock
////
datablock ItemData( ZeroGravityEgg )
{
	category				= "LessonShapes";
	shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";

	gravityMod = 0.0;
	sticky     = true;
};

////
// Normal Gravity Egg Item Datablock
////
datablock ItemData( NormalGravityEgg : ZeroGravityEgg )
{
	gravityMod = 1.0;
};

////
// Double Gravity Egg Item Datablock
////
datablock ItemData( DoubleGravityEgg : ZeroGravityEgg )
{
	gravityMod = 2.0;
};

////
// Negative Gravity Egg Item Datablock
////
datablock ItemData( NegativeGravityEgg : ZeroGravityEgg )
{
	gravityMod = -0.1;
};


////
// No Bounce Egg Item Datablock
////
datablock ItemData( NoBounceEgg )
{
	category				= "LessonShapes";
	shapeFile				= "~/data/GPGTBase/shapes/markers/Eggs/egg.dts";

	elasticity = 0.0;
};


////
// Low Bounce Egg Item Datablock
////
datablock ItemData( LowBounceEgg : NoBounceEgg )
{
	elasticity = 0.1;
};


////
// Medium Bounce Egg Item Datablock
////
datablock ItemData( MediumBounceEgg : NoBounceEgg )
{
	elasticity = 0.5;
};

////
// Full Bounce Egg Item Datablock
////
datablock ItemData( FullBounceEgg : NoBounceEgg )
{
	elasticity = 1.0;
};

////
// Over Bounce Egg Item Datablock
////
datablock ItemData( OverBounceEgg : NoBounceEgg )
{
	elasticity = 2.0;
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


