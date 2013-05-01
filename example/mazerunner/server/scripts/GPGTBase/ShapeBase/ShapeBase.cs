//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-------------------------------------------------------------------------
//						ShapeBaseData
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
//
//-------------------------------------------------------------------------
// Note: This test shape will NOT render.  This is merely an example of
//       ShapeBaseData, showing it's hierarchy, the standard fields it has,
//		 and it's console methods.  If you need to render a stationary object
//		 use either TSStatic(), or if you need the functionaliy in ShapeBase
//		 use a StaticShape/Data.
//-------------------------------------------------------------------------

datablock ShapeBaseData( BaseShapeBase )
{
   //-------------------------------------------------------------------------
   // GameBaseData Fields
   //-------------------------------------------------------------------------
   category				= "TestShapes";
   nametag					= "TestShapeName";

   //-------------------------------------------------------------------------
   // ShapeBaseData Fields
   //-------------------------------------------------------------------------
   // shapeFile - The model file for this shape.
   shapeFile				= "~/data/GPGTBase/shapes/markers/dummy.dts";

   // cloakTexture - The texture to render when this shape is cloaked.
   cloakTexture			= "~/data/GPGTBase/textures/testcloakskin.png";

   // emap - Enable environmental mapping (uses texture specified in sky's DML file)
   emap					= false;

   // explosion - Datablock name for explosion to use when this object is destroyed
   explosion				= "";

   // underwaterExplosion - Datablock name for explosion to use when this object is destroyed underwater
   underwaterExplosion		= "";

   // debris - Datablock name for debris to use when this object is destroyed
   debris					= "";

   // renderWhenDestroyed - Continue to render this shape when it is destroyed?
   renderWhenDestroyed		= true;

   // debrisShapeName - Shape to render for debris
   debrisShapeName			= "~/data/GPGTBase/shapes/markers/dummy.dts";

   //
   // Basic Physical parameters
   //
   mass					= 100.0; // Kilos
   drag					= 0.5;   // 'Air-resistance'
   density					= 0.2;   // Relative density

   // 
   // Energy Parameters
   //
   maxEnergy				= 150.0;  // Maximum energy this shape can have
   inheritEnergyFromMount	= true;   // If true, and mounted, energy comes from mount shape

   enableAutoRecharge   = false;  // Dynamic field used to enable/disable auto-recharge
   rechargeRate         = 1/32;   // Dynamic field used by onAdd() for setRepairRate() (if enabled) 
                                  // This recharge rate == 1 point per second

   // 
   // Damage Parameters - (info only) parameters are used by scripts, not the engine.
   //
   maxDamage				= 50.0;  // Max damage this shape can sustain
   disabledLevel			= 35.0;  // Damage level at which this shape is considered disabled(info only)
   destroyedLevel			= 49.95; // Damage level at which this shape is considered destroyed (info only) 

   isInvincible			= false; // Takes no damage

   enableAutoRepair     = false; // Dynamic field used to enable/disable auto-repair
   repairRate				= 1/32;  // Rate (in points per tick) at which this shape self-repairs

   //
   // Camera Params
   //
   cameraMinDistance		= 5.0;  // Camera cannot be nearer than this from cam node
   cameraMaxDistance		= 20.0; // Camera cannot be further than this from cam node
   cameraDefaultFov		= 90.0; // Field-Of-View (normal for video games is 90)
   cameraMinFov			= 20.0; // Minimum allowed field of view.

   firstPersonOnly		= false; // Only allow first first-person mode (disallows switch to 3rd POV)
   useEyePoint				= true;  // In 1st POV, true == camera at eye node, else eye at centroid
   observeThroughObject	= true;  // If true, camera will use this shape's camera parameters, else uses camera's own settings.

   // aiAvoidThis - (info only) Used by scripts to adjust AI behavior
   aiAvoidThis				= true;

   // computerCRC - Check CRC of shape file for match with controlling server's version.  If mismatch, game load fails.
   computeCRC				= false;
};

