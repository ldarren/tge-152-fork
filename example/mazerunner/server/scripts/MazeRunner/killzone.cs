//------------------------------------------------------
// Copyright 2000-2005, GarageGames.com, Inc.
// Written, modfied, or otherwise interpreted by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading Killzone Datablock Definitions and Scripts ---------");

function rldkill() {
   exec("./killzone.cs");
}

//-------------------------------------------------------------------------
//             KillZoneTrigger Datablock Definition
//-------------------------------------------------------------------------
datablock TriggerData(KillZoneTrigger)
{
   tickPeriodMS = 100;
};

// ******************************************************************
//					onEnterTrigger() 
// ******************************************************************
//
// 1. Use the player class' loseALife() method to take a way one life.
//
function KillZoneTrigger::onEnterTrigger(%DB , %Trigger , %Obj)
{
   // 1   
   %Obj.loseALife();
}

// ******************************************************************
//		buildKillZone() 
// ******************************************************************
//
// 1. Create the KillZone trigger
//
function buildKillZone()
{
   new Trigger(KillZone) {
      position   = "-256 256 40";
      rotation   = "1 0 0 0";
      scale      = "512 512 25";
      dataBlock  = "KillZoneTrigger";
      polyhedron = "0.0000000 0.0000000 0.0000000 1.0000000 0.0000000 0.0000000 0.0000000 -1.0000000 0.0000000 0.0000000 0.0000000 1.0000000";
   };

   MissionGroup.add( KillZone );
}