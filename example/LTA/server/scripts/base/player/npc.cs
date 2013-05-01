// ============================================================
// Project            :  VR20
// File               :  .\LTA\server\scripts\npc.cs
// Copyright          :  
// Author             :  Darren
// Created on         :  Monday, July 09, 2007 3:36 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

function loadNPCPlayer(%source, %role, %index, %transform)
{
	%typeInfo = getxmldata(%source, %role, 0);	
	%indexInfo = getxmldata(%typeInfo, %index, 0);	
	%name = getxmldata(%indexInfo, "name", 0);
	//%name = getWord(%name, 1);
	%avatar = getxmldata(%indexInfo, "avatar", 0);
	//%avatar = getWord(%avatar, 1);
	%speech = getxmldata(%indexInfo, "speech", 0);
	//%speech = getWord(%speech, 1);
	%portrait = getxmldata(%indexInfo, "portrait", 0);
	//%speech = getWord(%portrait, 1);
	
	//echo("######################### "@%name@" ## "@%avatar@" ## "@%speech);
	
	%npc = 0;
	
	if (%avatar.category $= "NPC")
	{
		%npc = new AIPlayer(%name)
		{
			datablock = %avatar;
			//scale = "0.1 0.1 0.1";
			//receiveSunLight = "0";
		};
	}
	else
	{
		%npc = new StaticShape(%name) 
		{
			dataBlock = %avatar;
			canSaveDynamicFields = "1";
			rotation = "1 0 0 0";
			position = "0 0 0";
			scale = "0.1 0.1 0.1";
			receiveSunLight = "1";
			receiveLMLighting = "1";
			useAdaptiveSelfIllumination = "0";
			useCustomAmbientLighting = "0";
			customAmbientSelfIllumination = "0";
			customAmbientLighting = "0 0 0 1";
			useLightingOcclusion = "1";
		};
	}
	
	MissionCleanup.add( %npc );
	
	%npc.setTransform(%transform);
	%npc.setShapeName(%name);
	/*if (%avatar.category $= "AnimatedNPC")*/ %npc.playThread(0,"ambient");

	%npc.RPGDialogScript = %speech;
	%npc.RPGDialogPortrait = %portrait;
	%npc.RPGDialogStartQuestion = 1;
	%npc.RPGDialogBusy = false;
	%npc.RPGDialogBusyText = 'Sorry but I\'m busy talking to %1 right now.';
	%npc.RPGDialogTalkingTo = 0;
}

function getNPCSpawnGroup()
{
	%groupName = "MissionGroup/NPCDropPoints";
	return nameToID(%groupName);
}

function pickNPCSpawnPoint() 
{
   %group = getNPCSpawnGroup();

   if (%group != -1) {
      %count = %group.getCount();
      if (%count != 0) {
         %index = getRandom(%count-1);
         %spawn = %group.getObject(%index);
         return %spawn.getTransform();
      }
      else
         error("No spawn points found in " @ %groupName);
   }
   else
      error("Missing spawn points group " @ %groupName);

   // Could be no spawn points, in which case we'll stick the
   // player at the center of the world.
   return "0 0 300 1 0 0 0";
}

function spawnNPC(%config)
{
	%source = loadXMLtoString(%config);
	%group = getNPCSpawnGroup();
	//if (%group == -1) return;
	
	%count = %group.getCount();
	
	for (%i = 0; %i < %count; %i++)
	{
		%spawn = %group.getObject(%i);
		loadNPCPlayer(%source,%spawn.role,%spawn.index,%spawn.getTransform());
	}
}
