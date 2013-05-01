%quest01 = new ScriptObject(Quest01)
{
	startLoc = "-85 100.2 66.4"; // teoh actor waiting place
	stopLoc = "-80 112 65.4"; // lead actor stopping loc
	spawnLoc = "-81.2 111.8 65.4"; // lead actor's brother spawning loc
	greetLoc = "-79.2 109.8 65.4"; // teoh actor greet lead actor loc
	catLoc1 = "-103 109 65.4";
	catLoc2 = "-105 111 65.4";
	catLoc3 = "-105 111 65.4";
	dogLoc1 = "-146.5 108 65.4";
	dogLoc2 = "-148.5 110 65.4";
	dogLoc3 = "-148.5 110 65.4";
	
	scheduleHandle = 0;
	
	actor_teoh = 0;
	actor_lead = 0;
	actor_br01 = 0;
	lead_old = 0;
	
	moveTolerance = 0.2;
	orgMoveTolerance = 0.25;
};

// reinitliatize when game start
function Quest01::init(%this)
{
	%this.actor_teoh = Teoh.getID();
	%this.actor_teoh.setMoveSpeed(0.8);
	%this.actor_teoh.setMoveTolerance(%this.moveTolerance);
}

// clean up when game end
function Quest01::remove(%this)
{
}

function Quest01::step01(%this, %player, %entering)
{
	if (!(%player.getType() & $TypeMasks::PlayerObjectType)) return;
	if (!%entering) return; // %player leaving trigger
	if (%player.quest!=0 || %this.actor_lead != 0) return; // quest occupied, TODO: display come-again message
	if (%player == %this.actor_teoh)
		return;
		
	echo("entering QuestPointTrigger");
	%this.actor_lead = %player;
	%this.actor_lead.quest = %this.getID();
	%this.actor_lead.stop();
	%this.actor_lead.setMoveTolerance(%this.moveTolerance);
	%this.actor_lead.setMoveDestination(%this.stopLoc);

	%this.actor_teoh.setAimLocation(%this.stopLoc);
	%this.actor_teoh.setMoveDestination(%this.greetLoc);
	%this.scheduleHandle= schedule(3000,0,"Quest01CheckStatus", %this);
}

function Quest01CheckStatus(%this)
{
	%this.actor_lead.stop();
	%this.actor_teoh.stop();
	%this.actor_lead.setAimObject(%this.actor_teoh);
	
	%this.actor_teoh.RPGDialogStartQuestion = 3;
	CreateRPGDialogBox(%this.actor_lead.client, %this.actor_teoh);
}

// spawn little brother and "morph" vr10 to green monster
function Quest01::dlgCmdStep01Opt01(%this,%client,%sender,%npcFile)
{
	if (%client.player != %this.actor_lead) return;

	%this.lead_old = %this.actor_lead.getdatablock();
	%this.actor_lead.setDataBlock(n004b);
	
	%this.actor_br01 = new AIPlayer(BR01)
	{
		datablock = n004b;
		//scale = "0.1 0.1 0.1";
		//receiveSunLight = "0";
	};
	
	%this.actor_br01.setTransform(%this.spawnLoc);
	%this.actor_br01.setShapeName(BR01);
	%this.actor_br01.setAimObject(%this.actor_teoh);
	MissionCleanup.add( %this.actor_br01 );
	
}

// lead actor go step 2 with actor teoh
function Quest01::dlgCmdStep01Opt02(%this,%client,%sender,%npcFile)
{
	if (%client.player != %this.actor_lead) return;
	%this.actor_lead.setAimLocation(%this.catLoc1);
	%this.actor_lead.setMoveDestination(%this.catLoc1);
	%this.actor_teoh.setAimLocation(%this.catLoc2);
	%this.actor_teoh.setMoveDestination(%this.catLoc2);
}

// lead actor go step 2 with brother and actor teoh
function Quest01::dlgCmdStep01Opt03(%this,%client,%sender,%npcFile)
{
	if (%client.player != %this.actor_lead) return;
	%this.actor_lead.setMoveTolerance(%this.orgMoveTolerance);
	%this.actor_lead.setAimLocation(%this.catLoc1);
	%this.actor_lead.setMoveDestination(%this.catLoc1);
	%this.actor_teoh.setAimLocation(%this.catLoc2);
	%this.actor_teoh.setMoveDestination(%this.catLoc2);
	%this.actor_br01.setAimLocation(%this.catLoc3);
	%this.actor_br01.setMoveDestination(%this.catLoc3);
}

function Quest01::step02(%this, %player, %entering)
{
	if (!(%player.getType() & $TypeMasks::PlayerObjectType)) return;
	if (!%entering) return; // %player leaving trigger
	if (%this.actor_lead != %player) return;
	%this.scheduleHandle= schedule(3000,0,"Quest02CheckStatus", %this);
}

function Quest02CheckStatus(%this)
{
	%this.actor_lead.stop();
	%this.actor_teoh.stop();
	%this.actor_lead.setAimObject(%this.actor_teoh);
	if (%this.actor_br01 != 0) 
	{
		%this.actor_br01.stop();
		%this.actor_br01.setAimObject(%this.actor_teoh);
	}
	
	%this.actor_teoh.RPGDialogStartQuestion = 7;
	CreateRPGDialogBox(%this.actor_lead.client, %this.actor_teoh);
}

// lead actor go step 2 with brother and actor teoh
function Quest01::dlgCmdStep02Opt01(%this,%client,%sender,%npcFile)
{
	if (%client.player != %this.actor_lead) return;
	%this.actor_lead.setMoveTolerance(%this.orgMoveTolerance);
	%this.actor_lead.setAimLocation(%this.dogLoc1);
	%this.actor_lead.setMoveDestination(%this.dogLoc1);
	%this.actor_teoh.setAimLocation(%this.dogLoc2);
	%this.actor_teoh.setMoveDestination(%this.dogLoc2);
	if (%this.actor_br01 != 0)
	{
		%this.actor_br01.setAimLocation(%this.dogLoc3);
		%this.actor_br01.setMoveDestination(%this.dogLoc3);
	}
}

function Quest01::step03(%this, %player, %entering)
{
	if (!(%player.getType() & $TypeMasks::PlayerObjectType)) return;
	if (!%entering) return; // %player leaving trigger
	if (%this.actor_lead != %player) return;
	%this.scheduleHandle= schedule(3000,0,"Quest03CheckStatus", %this);
}

function Quest03CheckStatus(%this)
{
	%this.actor_lead.stop();
	%this.actor_teoh.stop();
	%this.actor_lead.setAimObject(%this.actor_teoh);
	if (%this.actor_br01 != 0) 
	{
		%this.actor_br01.stop();
		%this.actor_br01.setAimObject(%this.actor_teoh);
	}
	
	%this.actor_teoh.RPGDialogStartQuestion = 8;
	CreateRPGDialogBox(%this.actor_lead.client, %this.actor_teoh);
}

function Quest01::dlgCmdEnd(%this,%client,%sender,%npcFile)
{
	CloseDialog(%client,%sender,%npcFile);
	if (%client.player != %this.actor_lead) return;
	%this.stop();
}

function Quest01::stop(%this)
{
	%this.actor_lead.clearAim();
	%this.actor_lead.quest = 0;
	if (%this.lead_old !=0) 
		%this.actor_lead.setDataBlock(%this.lead_old);
	%this.actor_lead = 0;

	%this.actor_teoh.RPGDialogStartQuestion = 1;
	%this.actor_teoh.clearAim();
	%this.actor_teoh.setAimLocation(%this.startLoc);
	%this.actor_teoh.setMoveDestination(%this.startLoc);
	
	%this.actor_br01.delete();
	%this.actor_br01 = 0;
}
