//------------------------------------------------------
// Copyright 2000-2005, GarageGames.com, Inc.
// Written, modfied, or otherwise interpreted by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading Teleporter Datablock Definitions and Scripts ---------");

function rldtp() {
   exec("./teleporters.cs");
}

//-------------------------------------------------------------------------
//             TeleportStation_PD0 Datablock Definition
//-------------------------------------------------------------------------
datablock ParticleData(TeleportStation_PD0)
{
   dragCoefficient      = 0.0;
   gravityCoefficient   = -0.50;
   inheritedVelFactor   = 0.0;
   constantAcceleration = 0.0;
   lifetimeMS           = 1500;
   lifetimeVarianceMS   = 100;
   useInvAlpha          = false;
   spinRandomMin = -140.0;
   spinRandomMax =  140.0;
   
   textureName          = "~/data/particles/star";
   colors[0]     = "0.7 0.1 0.1 0.8";
   colors[1]     = "0.7 0.1 0.1 0.4";
   colors[2]     = "0.7 0.1 0.1 0.0";
   sizes[0]      = 0.8;
   sizes[1]      = 0.9;
   sizes[2]      = 1;
   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

//-------------------------------------------------------------------------
//             TeleportStation_PED0 Datablock Definition
//-------------------------------------------------------------------------
datablock ParticleEmitterData(TeleportStation_PED0)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0.3;
   ejectionVelocity = 0.1;
   ejectionOffset   = 1;
   velocityVariance = 0.5;
   thetaMin         = 85;
   thetaMax         = 90;
   phiReferenceVel  = 0;
   phiVariance      = 360;
   overrideAdvance  = false;
   particles        = "TeleportStation_PD0";
};

//-------------------------------------------------------------------------
//             TeleportStation_PD1 Datablock Definition
//-------------------------------------------------------------------------
datablock ParticleData(TeleportStation_PD1 : TeleportStation_PD0)
{
   colors[0]     = "0.1 0.7 0.1 0.8";
   colors[1]     = "0.1 0.7 0.1 0.4";
   colors[2]     = "0.1 0.7 0.1 0.0";
};

//-------------------------------------------------------------------------
//             TeleportStation_PED1 Datablock Definition
//-------------------------------------------------------------------------
datablock ParticleEmitterData(TeleportStation_PED1 : TeleportStation_PED0 )
{
   particles        = "TeleportStation_PD1";
};

//-------------------------------------------------------------------------
//             TeleportStation_PD2 Datablock Definition
//-------------------------------------------------------------------------
datablock ParticleData(TeleportStation_PD2 : TeleportStation_PD0)
{
   colors[0]     = "0.1 0.1 0.7 0.8";
   colors[1]     = "0.1 0.1 0.7 0.4";
   colors[2]     = "0.1 0.1 0.7 0.0";
};

//-------------------------------------------------------------------------
//             TeleportStation_PED2 Datablock Definition
//-------------------------------------------------------------------------
datablock ParticleEmitterData(TeleportStation_PED2 : TeleportStation_PED0 )
{
   particles        = "TeleportStation_PD2";
};


//-------------------------------------------------------------------------
//             TeleportTrigger Datablock Definition
//-------------------------------------------------------------------------
datablock TriggerData(TeleportTrigger)
{
   category     = "TeleportTriggers";
   tickPeriodMS = 100;
};

// ******************************************************************
//					Trigger::AttachEffect() 
// ******************************************************************
//
// 1. Define array containing PED names
// 2. Create and store reference to particle emitter node effect.
// 3. Create and store reference to physical zone effect.
//

// 1
$TeleEmitter[Red] = "TeleportStation_PED0";
$TeleEmitter[Green] = "TeleportStation_PED1";
$TeleEmitter[Blue] = "TeleportStation_PED2";

function Trigger::AttachEffect( %Obj )
{
   // 2
   %effect = new ParticleEmitterNode() {
      position  = vectorAdd(%Obj.getWorldBoxCenter(), "0 0 0");
      rotation  = "1 0 0 0";
      scale     = "1 1 1";
      dataBlock = "basePEND";
      emitter   = $TeleEmitter[%Obj.type];
      velocity  = "1";
   };
   TeleportStationEffectsGroup.add(%effect);


   // 3
   %pzone = new PhysicalZone() {
      position     = vectorAdd( %Obj.getPosition() , "0 0 0" );
      rotation     = "1 0 0 0";
      scale        = "1 1 1";
      velocityMod  = "0";
      gravityMod   = "1";
      appliedForce = "0 0 0";
      polyhedron   = "0.0000000 0.0000000 0.0000000 1.0000000 0.0000000 0.0000000 0.0000000 -1.0000000 0.0000000 0.0000000 0.0000000 1.0000000";
   };
   TeleportStationEffectsGroup.add(%pzone);
   %Obj.myPZone=%pzone;
   //echo("\c2 ", %Obj.myPZone );

}


// ******************************************************************
//					onEnterTrigger() 
// ******************************************************************
//
// 1. Activate the PZone. (Stopping the player.)
// 2. If the trigger is enabled...
// 2a. Check for target stations.
// 2b. Randomly select a target station.
// 2c. Disable the target station and schedule for reactivation in 2000 ms
// 2d. Disable the target station's PZone.
// 2e. Calculate the player's new transform.
// 2f. Start the player fading out, and schedule reverse process.
// 2g. Schedule a move to occur 'after' the fade is over.

//
function TeleportTrigger::onEnterTrigger(%DB , %Trigger , %Obj)
{
   //echo("\c2    Entering Trigger: ", %Trigger );
   //echo("\c2 Trigger.enabled == ", %Trigger.enabled);
   
   // 1   
   %Trigger.myPZone.activate();

   // 2
   if( %Trigger.enabled )
   {
      //echo("\c4 $Game::Player.lastTrigger == ", $Game::Player.lastTrigger );
      $Game::Player.lastTrigger = %Trigger;
      
      //echo("\c2 Trigger: ", %Trigger , " is enabled.");
      //echo("%Trigger.getGroup() ", %Trigger.getGroup());
      // 2a
      // Catch case where no group/target stations exist
      if( !isObject( %Trigger.getGroup() ) || (%Trigger.getGroup().getCount() < 2 ) )
      {
         error("No group or no stations??");
         return;
      }

      // 2b
      while( %Trigger == ( %targetStation = %Trigger.getGroup().getRandomObject() ) )
      {
      }

      // 2c
      %targetStation.enabled = false;      
      %targetStation.schedule( 2000 , reEnable );

      // 2d
      %targetStation.myPZone.activate();

      // 2e
      %oldTransform = %Obj.getTransform();

      %newPos       = %targetStation.getWorldBoxCenter();
      %newPos       = vectorAdd( "0 0 -0.25", %newPos );

      %newTransform = %newPos SPC getWords( %oldTransform, 3 , 6);

      // 2f
      %Obj.startFade( 750 , 0 , true );            
      %Obj.schedule( 750 , startFade , 750 , 0 , false );

      // 2g
      %Obj.schedule( 750 , setTransform , %newTransform );
      //echo("Sending....");
      //echo("\c2 Target Trigger: ", %targetStation , " is enabled? ", %targetStation.enabled);

   }   
}

// ******************************************************************
//					onLeaveTrigger() 
// ******************************************************************
//
// 1. Enable this trigger's PZone.
//
function TeleportTrigger::onLeaveTrigger(%DB , %Trigger , %Obj)
{
   // 1
   %Trigger.myPZone.deactivate();
}

// ******************************************************************
//					Trigger::reEnable() 
// ******************************************************************
//
// 1. Re-enable the destination trigger.
//
function Trigger::reEnable(%Trigger)
{
   // 1
   %Trigger.enabled = true;
}


