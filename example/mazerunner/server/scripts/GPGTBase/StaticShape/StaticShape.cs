//-----------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-----------------------------------------------------------------------------
//-------------------------------------------------------------------------
//						StaticShapeData
//-------------------------------------------------------------------------
// Datablock Hierarchy
//-------------------------------------------------------------------------
//
// SimObject 
//		|
//		|- SimDataBlock
//				|
//				|- GameBaseData
//						|
//						|- ShapeBaseData
//								|
//								|- StaticShapeData 
//
//-------------------------------------------------------------------------

datablock StaticShapeData( BaseStaticShape )
{
   //-------------------------------------------------------------------------
   // Parent Fields
   //-------------------------------------------------------------------------
   // From GameBaseData
   //-------------------------------------------------------------------------
   category				= "TestShapes";
   nametag					= "TestShapeName";

   //-------------------------------------------------------------------------
   // From ShapeBase
   //-------------------------------------------------------------------------
   shapeFile				= "~/data/GPGTBase/shapes/markers/dummy.dts";
   cloakTexture			= "~/data/GPGTBase/textures/testcloakskin.png";
   emap					   = false;
   explosion				= "";
   underwaterExplosion	= "";
   debris					= "";
   renderWhenDestroyed	= true;
   debrisShapeName		= "~/data/GPGTBase/shapes/markers/dummy.dts";
   mass					   = 100.0;
   drag					   = 0.5;
   density					= 0.2;
   maxEnergy				= 150.0;
   maxDamage				= 50.0;
   disabledLevel			= 35.0;
   destroyedLevel			= 50.0;
   repairRate				= 0.001;
   inheritEnergyFromMount	= true;
   isInvincible			= false;
   cameraMaxDistance		= 20.0;
   cameraMinDistance		= 5.0;
   cameraDefaultFov		= 90.0;
   cameraMinFov			= 20.0;
   firstPersonOnly		= false;
   useEyePoint				= true;
   observeThroughObject	= true;
   aiAvoidThis				= true;
   computeCRC				= false;

   //-------------------------------------------------------------------------
   // StaticShapeData Fields
   //-------------------------------------------------------------------------
   // dynamicType - Used to return additional type bits for getType() calls
   dynamicType				= 0; // Advanced

   // noIndividualDamage - (info only) Used by scripts for whatever you please
   noIndividualDamage		= false;
};

