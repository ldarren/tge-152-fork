//-----------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-----------------------------------------------------------------------------
//-------------------------------------------------------------------------
//						ShapeBaseImageData
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
//						|- ShapeBaseImageData
//
//-------------------------------------------------------------------------

datablock ShapeBaseImageData(BaseShapeBaseImageData)
{
   //-------------------------------------------------------------------------
   // GameBaseData Fields
   //-------------------------------------------------------------------------
   //category				= "TestShapes"; // Not placeable, so you don't need this
   //nametag				= "TestShapeName";

   //-------------------------------------------------------------------------
   // ShapeBaseImage Fields
   //-------------------------------------------------------------------------

   // shapeFile - The model file for this shape. (No collision box required)
   shapeFile			= "~/data/GPGTBase/shapes/markers/dummy.dts";

   // emap - Enable environmental mapping (uses texture specified in sky's DML file)
   emap				= false;

   // cloakable - Is this image able to cloak? If true, cloaks with mount shape.
   cloakable			= true;

   // mountpoint - Which numbered joint/node does this image mount to (0..30) (31 is reserved)
   mountpoint			= 0; 


   // offset - a Transform to modify the mounting translation of this image (to adjust for mounting issues, instead of editting model)
   // rotation - a Transform to modify the mounting rotation of this image (to adjust for mounting issues, instead of editting model)
   offset				= "0.0 0.0 0.0"; 
   rotation			= "0.0 0.0 0.0 0.0"; 


   // eyeRotation - Adjust where the weapon is rendered (translation) in first POV.
   // eyeRotation - Adjust where the weapon is rendered (rotation) in first POV.
   eyeOffset			= "0.0 0.0 0.0"; 
   eyeRotation			= "0.0 0.0 0.0 0.0";


   //correctMuzzleVector - If true, use the muzzlepoint for calulating image (weapon) pointing, else use eyepoint
   correctMuzzleVector	= true;

   // firstPerson - Render this image while in first POV?
   firstPerson			= true;

   //
   // General Physical Parameter
   //
   mass				= 10.0; // Kilos

   // usesEnergy - If true, and energy < minEnergy, will report having no ammo.
   usesEnergy			= true;	
   minEnergy			= 1.0;

   // accuFire - Not used
   accuFire			= false;

   // 
   // Lighting Parameters
   //
   lightType			= NoLight;  // NoLight, ConstantLight, PulsingLight
   lightColor			= "1.0 1.0 1.0 1.0"; // This light is pure white
   lightTime			= 1000; // Time between ON-OFF-ON is 1 second
   lightRadius			= 1.0;  // Radius of light is 1 meter


   // 
   // Shell casing parameters
   //
   //casing			= ""; // Debris datablock for shell casings 
   shellExitDir		= "-1.0 1.0 0.0"; // eject to the right and up
   shellExitVariance	= 1.0; // Degrees (about) eject vector by which to vary ejection (randomly)
   shellVelocity		= 2.0; // Velocity (m/s) at which shell eject


   //
   // State Machine  Parameters - Please see:
   //                             GPGT Volume I & II - Console Classes Appendix  and/or
   //                             GPGT Volume II     - Weapons
   // 
   //stateName						      string
   //stateTransitionOnLoaded		   string
   //stateTransistionOnNotLoaded	   string	
   //stateTransitionOnAmmo			   string
   //stateTransistionOnTarget		   string
   //stateTransistionOnNoTarget	   string	
   //stateTransitiononWet			   string
   //stateTransitiononNotWet		   string
   //stateTransitionOnTriggerUp	   string
   //stateTransitionOnTriggerDown	string
   //stateTransitionOnTimeout		   string
   //stateTimeoutValue				   float
   //stateWaitForTimeout			   bool
   //stateFire					       	bool
   //stateEjectShell				      string	
   //stateEnergyDrain				   float
   //stateAllowImageChange			   bool
   //stateDirection				      bool
   //stateLoadedFlag				      bool
   //stateSpinThread				      enum string
   //stateRecoil					      enum string
   //stateSequence					   string
   //stateSequenceRandomFlash		   bool
   //stateSound					      audio profile string
   //stateScript					      string
   //stateEmitter					      emitter datablock
   //stateEmitterTime				   float
   //stateEmitterNode				   integer
   //stateIgnoreLoadedForReady		bool

   // computerCRC - Check CRC of shape file for match with controlling server's version.  If mismatch, game load fails.
   computeCRC			= false;
};

