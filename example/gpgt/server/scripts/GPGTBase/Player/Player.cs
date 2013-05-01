//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-------------------------------------------------------------------------
//                        PlayerData
//-------------------------------------------------------------------------
// Datablock Hierarchy
//-------------------------------------------------------------------------
//
// SimObject 
//        |
//        |- SimDataBlock
//                |
//                |- GameBaseData
//                        |
//                        |- ShapeBaseData
//                                |
//                                |- PlayerData 
//
//-------------------------------------------------------------------------

// Load dts shapes and merge animations
exec("~/data/BlueGuy/player.cs");

//-------------------------------------------------------------------------
//                        BasePlayer 
//-------------------------------------------------------------------------
datablock PlayerData( BasePlayer )
{
   shapeFile            = "~/data/BlueGuy/player.dts";
   category             = "Bipeds";

   // boundingBox - Adjust this based on the size of our model.
   boundingBox          = "1.2 1.2 2.3"; 

   // pickupRadius - Used to make the pickup radius larger.  i.e. More prone to collide w/
   //                items.
   pickupRadius         = 0.75;

   // Basic Physical Parameters
   mass                 = 90;  // Kilos
   density              = 10;  // Relative density 

   // 
   // Forces & Factors
   //
   runForce             = 48 * 90; // Notice that this is a multiple of the players MASS
   runSurfaceAngle      = 30;      // Player can only walk if surface angle (degrees) is less than this

   jumpForce            = 22 * 90; // Again, this is a multiple of mass
   jumpDelay            = 0; 
   jumpSurfaceAngle     = 60;      // Maximum angle (degrees) beyond which character will not jump

   drag                 = 0;       // Air resistance (Values can be negative.)

   maxBackwardSpeed     = 10;      // Maximum backward velocity (on ground)
   maxForwardSpeed      = 14;      // Maximum forward velocity (on ground)
   maxSideSpeed         = 12;      // Maximum sideways velocity (on ground)

   maxUnderwaterBackwardSpeed   = 5;    // Maximum backward velocity (underwater)
   maxUnderwaterForwardSpeed    = 7;    // Maximum forward velocity (underwater)
   maxUnderwaterSideSpeed       = 6;    // Maximum sideways velocity (underwater)

   horizMaxSpeed                = 68;   // Maximum horizontal component of velocity (general)
   horizResistSpeed             = 33;   // Horizontal component of velocity (general) at which drag activates
   horizResistFactor            = 0.35; // Resistance multiplier

   upMaxSpeed                   = 100;  // Maximum vertical component of velocity (general)
   upResistFactor               = 10;   // Vertical component of velocity (general) at which drag activates
   upResistSpeed                = 1;    // Resistance multiplier

   minJumpSpeed                 = 0;     
   maxJumpSpeed                 = 0;     

   recoverDelay                 = 8;    // Delays motion for N ticks after a fall
   recoverRunForceScale         = 0.1;  // Multiplier modifying run force during recovery period

   maxStepHeight                = 0.75; // Max height player can step up (0.75m ~= 2.5ft)

   boxHeadBackPercentage        = 0;    // See GPGT Volume 2 - Weapons Chapter
   boxHeadFrontPercentage       = 1;
   boxHeadLeftPercentage        = 0;
   boxHeadRightPercentage       = 1;
   boxNormalHeadPercentage      = 0.83;
   boxNormalTorsoPercentage     = 0.49;

   //maxDamage                = 50.0; 
   //disabledLevel            = 35.0; 
   //destroyedLevel            = 49.95;
   //repairRate                = 0.001;
   isInvincible            = true;  // Default player is unkillable 


   enableAutoRecharge   = false;  // Dynamic field used to enable/disable auto-recharge
   rechargeRate         = 1/32;   // Dynamic field used by onAdd() for setRepairRate() (if enabled) 

   enableAutoRepair     = false; // Dynamic field used to enable/disable auto-repair
   repairRate				= 1/32;  // Rate (in points per tick) at which this shape self-repairs


   renderFirstPerson = false; // Render character geometry while in 1st POV

   cameraDefaultFov        = 120.0;
   cameraMinFov            = 45.0;
   cameraMaxFov            = 120.0;

   cameraMinDist            = 45.0;
   cameraMaxDist            = 60.0;

   observeThroughObject    = true; // Use the camera's FOV and distance settings 
                                    // while the camera is orbitting this shape.
                                    
   useEyePoint             = false; // Use the eye-point for camera location in first POV

   minLookAngle            = -1.57; // Straight-down
   maxLookAngle            = 1.57;  // Straight-up
   maxFreelookAngle        = 2.1;    // 2/3 (total) rotation in either direction

   // This player does not have debris.
   //    debrisShapeName            = "~/data/GPGTBase/shapes/markers/dummy.dts";
   //    debris                    = playerDebris;

   //-------------------------------------------------------------------------
   // FOOT PRINTS 
   //-------------------------------------------------------------------------
   //    decalData
   //    decalOffset    = 0.25;

   //-------------------------------------------------------------------------
   // FOOT PUFFS
   //-------------------------------------------------------------------------
   //    footPuffEmitter = LightPuffEmitter;
   //    footPuffNumParts = 10;
   //    footPuffRadius = 0.25;
   dustEmitter = "";


   //-------------------------------------------------------------------------
   //  IMPACT PARAMETERS
   //-------------------------------------------------------------------------
   minImpactSpeed              = 25;
   groundImpactMinSpeed        = 25;
   groundImpactShakeAmp        = "1.0 1.0 1.0";
   groundImpactShakeDuration   = 0.1;
   groundImpactShakeFalloff    = 10.0;
   groundImpactShakeFreq       = "4.0 4.0 4.0";

   //-------------------------------------------------------------------------
   // ANIMATION
   //-------------------------------------------------------------------------
   maxTimeScale    = 1.5;  // Maximum time scaling of action animations 

   //-------------------------------------------------------------------------
   // SPLASHES
   //-------------------------------------------------------------------------
   //    splash
   //    splashAngle
   //    splashEmitter
   //    splashFreqMod
   //    splashVelEpsilon
   //    splashVelocity

   //-------------------------------------------------------------------------
   // BUBBLES
   //-------------------------------------------------------------------------
   //    bubbleEmitTime

   //-------------------------------------------------------------------------
   // SOUNDS
   //-------------------------------------------------------------------------
   //    exitingWater
   //    exitSplashSoundVelocity
   //    FootBubblesSound
   //    FootHardSound
   //    FootMetalSound
   //    FootShallowSound
   //    FootSnowSound
   //    FootSoftSound
   //    footstepSplashHeight
   //    FootUnderwaterSound
   //    FootWadingSound
   //    hardSplashSoundVelocity
   //    impactHardSound
   //    impactMetalSound
   //    impactSnowSound
   //    impactSoftSound
   //    impactWaterEasy
   //    impactWaterHard
   //    impactWaterMedium
   //    mediumSplashSoundVelocity
   //    movingBubblesSound
   //    waterBreathSound

};



