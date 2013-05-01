//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-------------------------------------------------------------------------
//						Sample Items
//-------------------------------------------------------------------------

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
	lightRadius				= 4.0;
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

datablock ItemData( SlideNoBounceItem : SampleItem )
{
	friction				= 0.2;		// Give this a low friction so it will slide
	elasticity				= 0.0;		// Don't BounceItem at all.
	sticky					= false;	// Don't stick on hitting
	lightRadius				= 5.0;		// Give this a bigger radius
	lightColor				= "0.8 1.0 0.0 0.0";

};

datablock ItemData( NoSlideItem : SampleItem )
{
	friction				= 1.0;		// Friction is so high, it won't slide
	elasticity				= 0.0;		// Don't BounceItem at all.
	sticky					= false;	// Don't stick on hitting
	lightRadius				= 2.0;		// Give this a bigger radius
	lightColor				= "0.2 1.0 0.0 0.0";
	//-------------------------------------------------------------------------
	// From ShapeBase
	//-------------------------------------------------------------------------
	maxVelocity				= 40.0; // Let this thing get a move on
};


datablock ItemData( BounceItem : SampleItem )
{
	elasticity				= 0.8;		// Don't BounceItem at all.
	sticky					= false;	// This object will stick where it hits
	lightRadius				= 20.0;		// Give this a huge radius
	lightColor				= "1.0 0.0 0.2 0.0";
	//-------------------------------------------------------------------------
	// From ShapeBase
	//-------------------------------------------------------------------------
	maxVelocity				= 40.0; // Let this thing get a move on
};



datablock ItemData( SuperBallItem : SampleItem )
{
	elasticity				= 1.0;	// 100% of velocity goes into resultant
									// impact vector
	sticky					= false; // This object will stick where it hits
	friction				= 0.0; // Don't want any loss due to friction w/ surfaces we impact
	lightColor				= "0.8 1.0 1.0 0.0";
	//-------------------------------------------------------------------------
	// From ShapeBase
	//-------------------------------------------------------------------------
	drag					= 0.0; // Don't want any loss due to air resistance
	maxVelocity				= 40.0; // Let this thing get a move on

};
function SuperBallItem::onAdd( %ItemDB , %Item ) 
{
//	echo("SuperBallItem::onAdd()");
	Parent::onAdd( %ItemDB , %Item );
	%Item.rotate		= false;
}




datablock ItemData( StickyBallItem : SuperBallItem )
{
	elasticity				= 1.0;	// 100% of velocity goes into resultant
									// impact vector
	sticky					= true; // This object will stick where it hits
	friction				= 0.0; // Don't want any loss due to friction w/ surfaces we impact
	lightTime				= 500;
	lightColor				= "1.0 1.0 0.0 0.0";
	//-------------------------------------------------------------------------
	// From ShapeBase
	//-------------------------------------------------------------------------
	drag					= 0.0; // Don't want any loss due to air resistance
	maxVelocity				= 40.0; // Let this thing get a move on

};

datablock ItemData( OverBouncyItem : SampleItem )
{
	elasticity				= 1.5;	// 100% of velocity goes into resultant
									// impact vector
	sticky					= false; // This object will NOT stick where it hits
	friction				= 0.0; // Don't want any loss due to friction w/ surfaces we impact
	lightColor				= "1.0 0.4 0.4 0.0";
	//-------------------------------------------------------------------------
	// From ShapeBase
	//-------------------------------------------------------------------------
	drag					= 0.0; // Don't want any loss due to air resistance
	maxVelocity				= 40.0; // Let this thing get a move on

};
		

datablock ItemData( AnimatedGearItem : SampleItem )
{
	lightRadius				= 10.0;		// Give this a big radius
	lightColor				= "1.0 0.2 0.0 0.0";
	lightType				= "PulsingLight";
	lightTime				= 1000;
	//-------------------------------------------------------------------------
	// From ShapeBase
	//-------------------------------------------------------------------------
	shapeFile				= "~/data/Shapes/TestShapes/SampleGears.dts";
};


function SampleItems::onAdd( %ItemDB , %Item ) 
{
//	echo("SampleItems::onAdd()");
	Parent::onAdd( %ItemDB , %Item );
	%Item.collideable	= false;
	%Item.static		= false;
	%Item.rotate		= true;
}



