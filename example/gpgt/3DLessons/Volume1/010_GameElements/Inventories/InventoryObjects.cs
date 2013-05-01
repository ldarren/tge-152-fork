//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-------------------------------------------------------------------------
//						Sample Inventory Items
//-------------------------------------------------------------------------
datablock ItemData( InventoryItem : BaseItem )
{
	//-------------------------------------------------------------------------
	// ItemData Fields
	//-------------------------------------------------------------------------
	category				= "InventoryLessonShapes";
	lightType				= "None";
	pickupName				= "Inventory Item";
	sticky					= true; 

	className				= InventoryItems;
	//-------------------------------------------------------------------------
	// From ShapeBase
	//-------------------------------------------------------------------------
	shapeFile				= "./data/Coin/coin.dts";

	// lightColor - This item will emit a pure white light
	lightColor				= "1.0 1.0 0.0 1.0";
	
	// lightOnlyStatic - This item will only light objects/terrain/etc. when this item
	// is marked as static (in the object definition)
	lightOnlyStatic			= false;

	// lightRadius - This object will emit a light with a radius of 2 meters
	lightRadius				= 2.0;

	// lightType - This light is pulsing on-off-on-...
	lightType				= PulsingLight; // NoLight, PulsingLight, ConstantLight

	// lightTime - This 'pulsing' light will go from ON to OFF to ON in 3 seconds.
	// Only meaningful for pulsing lights
	lightTime				= 1000;

	mass					= 1.0;

	// respawn - Dynamic variable used by GPGT scripts that decides whether to delete or respawn item
    respawn					= true;
};


datablock ItemData( HolyHandGrenadeItem : BaseItem )
{
	//-------------------------------------------------------------------------
	// ItemData Fields
	//-------------------------------------------------------------------------
	category				= "InventoryLessonShapes";
	lightType				= "None";
	pickupName				= "Holy Hand Grenade";
	sticky					= true; 

	className				= InventoryItems;
	//-------------------------------------------------------------------------
	// From ShapeBase
	//-------------------------------------------------------------------------
	shapeFile				= "./data/HolyHandgrenade/holyhandgrenade.dts";

	// lightColor - This item will emit a pure white light
	lightColor				= "0.0 0.8 1.0 1.0";
	
	// lightOnlyStatic - This item will only light objects/terrain/etc. when this item
	// is marked as static (in the object definition)
	lightOnlyStatic			= false;

	// lightRadius - This object will emit a light with a radius of 2 meters
	lightRadius				= 4.0;

	// lightType - This light is pulsing on-off-on-...
	lightType				= ConstantLight; // NoLight, PulsingLight, ConstantLight

	mass					= 5.0;

	// elasticity - How bouncy is this object.  A little elasticity is good, because
	// a value of zero may cause a dropped item to 'hesitate' before settling.
	// This is a calculation rounding issue I believe.
	elasticity				= 0.5;

	// friction - How much will this item 'slow' while sliding?
	// This item will arrest quickly.
	friction				= 0.3; 

	// sticky - This item will not stick to the terrain or interiors when it hits. Instead,
	// it will slide.
	sticky					= false; 

	// respawn - Dynamic variable used by GPGT scripts that decides whether to delete or respawn item
    respawn					= true;

};

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
function InventoryItems::onAdd( %DB , %Obj )
{
	Parent::onAdd( %DB , %Obj );
	%Obj.collideable	= true;
	%Obj.static			= false;
	%Obj.rotate			= true;
}



//-----------------------------------------------------------------------------
// Hand Grenade Explosion
datablock ParticleData(GrenadeExplosionParticle : baseSmokePD1)
{
   dragCoefficient      = 2;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.2;
   constantAcceleration = 0.0;
   lifetimeMS           = 750;
   lifetimeVarianceMS   = 200;
   colors[0]     = "0.6 0.0 1.0 1.0";
   colors[1]     = "0.6 0.5 1.0 0.0";
   sizes[0]      = 1.5;
   sizes[1]      = 2.0;
};
datablock ParticleEmitterData(GrenadeExplosionEmitter)
{
   ejectionPeriodMS = 7;
   periodVarianceMS = 0;
   ejectionVelocity = 1;
   velocityVariance = 1.0;
   ejectionOffset   = 0.0;
   thetaMin         = 0;
   thetaMax         = 60;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvances = false;
   particles = "GrenadeExplosionParticle";
};

datablock ExplosionData(GrenadeExplosion)
{
   lifeTimeMS = 2000;
   particleEmitter = GrenadeExplosionEmitter;
   particleDensity = 250;
   particleRadius = 8;
   faceViewer     = true;
   explosionScale = "1 1 1";
   shakeCamera = true;
   camShakeFreq = "10.0 11.0 10.0";
   camShakeAmp = "1.0 1.0 1.0";
   camShakeDuration = 0.5;
   camShakeRadius = 10.0;
   // Dynamic light
   lightStartRadius = 6;
   lightEndRadius = 0;
   lightStartColor = "0 0 0.6";
   lightEndColor = "0 0 0";
};



// ******************************************************************
//					HolyHandGrenade::onUse() 
// ******************************************************************
function HolyHandGrenadeItem::onUse( %useDB , %ownerObj )
{
    //
    // 1. Remove one of these items from the inventory.
    //    If none exists, just return 0.
    //
    if(0 == %ownerObj.myInventory.getInventoryCount( %useDB )) 
    {
        return 0;
    }
    %ownerObj.myInventory.removeObject( %useDB );

    // 2. Do something with it.  We're not doing anything so we will 
    //    return false. i.e. onUse() failed!
    //
    %newObject = new Item() 
    {
        position	= %ownerObj.getWorldBoxCenter();
        datablock	= %useDB;
        collideable	= true; 
        static		= false; // Can't throw a static.  
        rotate		= true;
    };

    %ownerObj.getDatablock().throwObject( %ownerObj , %newObject );

    %newObject.getDatablock().schedule( 2000, explodeGrenade, %newObject ); 

    return true;
}

function HolyHandGrenadeItem::explodeGrenade( %DB , %Obj ) {
   
    %explosion = new Explosion() {
        datablock = GrenadeExplosion;
        position  = %obj.getPosition();
    };

   echo("HolyHandGrenadeItem::explodeGrenade() -> ", %explosion );

    %obj.schedule(0, delete);
}

