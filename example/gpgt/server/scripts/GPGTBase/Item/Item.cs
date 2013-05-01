//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-------------------------------------------------------------------------
//						ItemData
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
//						|- PathedInteriorData
//								|
//								|- ShapeBaseData
//										|
//										|- ItemData 
//
//-------------------------------------------------------------------------

datablock ItemData( BaseItem )
{
   //-------------------------------------------------------------------------
   // Parent Fields
   //-------------------------------------------------------------------------
   // From GameBaseData
   //-------------------------------------------------------------------------
   category				= "TestShapes";
   nametag				= "DefaultItem";

   //-------------------------------------------------------------------------
   // From ShapeBaseData
   //-------------------------------------------------------------------------
   //
   // We won't set all the values from ShapeBaseData.  Instead, we'll just 
   // set the few values that can be 'troublesome' if not set.
   // 
   shapeFile         = "~/data/GPGTBase/shapes/markers/dummy.dts";
   cloakTexture      = "~/data/GPGTBase/textures/testcloakskin.png";
   emap              = false;

   mass              = 10.0;
   drag              = 0.0;
   density           = 0.1;
   computeCRC        = false;

   //-------------------------------------------------------------------------
   // ItemData Fields
   //-------------------------------------------------------------------------
   // dynamicType - Used to return additional type bits for getType() calls
   dynamicType			= 0;    

   // elasticity - How bouncy is this object.  A little elasticity is good, because
   // a value of zero may cause a dropped item to 'hesitate' before settling.
   // This is a calculation rounding issue I believe.
   elasticity			= 0.05;

   // friction - How much will this item 'slow' while sliding?
   // This item will arrest quickly.
   friction				= 0.7; 

   // gravityMod - Gravity affects this item normally.
   gravityMod			= 1.0;

   // lightColor - This item will emit a pure white light
   lightColor			= "1.0 1.0 1.0 1.0";

   // lightOnlyStatic - This item will only light objects/terrain/etc. when this item
   // is marked as static (in the object definition)
   lightOnlyStatic	= true;

   // lightRadius - This object will emit a light with a radius of 2 meters
   lightRadius			= 2.0;

   // lightType - This light is pulsing on-off-on-...
   lightType			= "PulsingLight"; // NoLight, PulsingLight, ConstantLight

   // lightTime - This 'pulsing' light will go from ON to OFF to ON in 3 seconds.
   // Only meaningful for pulsing lights
   lightTime			= 3000;

   // maxVelocity - This shape is velocity 'limited' to 10 meters per second
   maxVelocity			= 10.0;

   // pickupName - This (dynamic) field provides a message name for this shape when it
   // is picked up.
   pickupName			= "Default Item";

   // sticky - This item will not stick to the terrain or interiors when it hits. Instead,
   // it will slide.
   sticky				= false; 
};

