// ============================================================
// Project            :  VR20
// File               :  .\LTA\server\scripts\LTA\quests\quest02.cs
// Copyright          :  
// Author             :  Darren
// Created on         :  Wednesday, September 26, 2007 3:38 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

new ScriptObject(Quest02)
{
	buggy = 0;
	psionic = 0;
};

MissionCleanup.add(Quest02);

// reinitliatize when game start
function Quest02::init(%this)
{
	%buggy = new AIRoadVehicle() 
	{
		datablock = DefaultCar;
		path = %this.buggy_route;
	};
	MissionCleanup.add( %this.buggy );
	
	%this.buggy = %buggy;
	%buggy.currStop = 0;
	%buggy.route = "MissionGroup/Route01";
	%route = %buggy.route;
	%buggy.lastStop = %route.getCount() -1;
	%node = %route.getObject(%buggy.currStop);  
	%buggy.setTransform(%node.getTransform());

	%buggy.setShapeName("buggy");
	
	%buggy.setMoveSpeed(0.5);
	%buggy.setMoveTolerance(10);

	%buggy.RPGDialogScript = "q004";
	%buggy.RPGDialogPortrait = "buggy.jpg";
	%buggy.RPGDialogStartQuestion = 1;
	%buggy.RPGDialogBusy = false;
	%buggy.RPGDialogBusyText = 'Sorry but I\'m busy talking to %1 right now.';
	%buggy.RPGDialogTalkingTo = 0;
	
	%buggy.toStation();
/*	
	%this.psionic = new AIWheeledVehicle() 
	{
		datablock = PsionicJeep0;
		position = %this.psionic_startLoc;
		disableMove = 0;
	};

	%this.psionic.setTransform(%this.psionic_startLoc);
	%this.buggy.setShapeName("psionic");
	%this.buggy.RPGDialogScript = "q004";
	%this.buggy.RPGDialogPortrait = "psionic.jpg";
	%this.buggy.RPGDialogStartQuestion = 1;
	%this.buggy.RPGDialogBusy = false;
	%this.buggy.RPGDialogBusyText = 'Sorry but I\'m busy talking to %1 right now.';
	%this.buggy.RPGDialogTalkingTo = 0;*/
}

function AIWheeledVehicle::toNextStation(%this)
{
	%this.currStop += 1;
	if (%this.currStop > %this.lastStop) %this.currStop = 0;
	%this.toStation();
}

function AIWheeledVehicle::toStation( %this )
{   
   // Move to the given path node index
   %node = %this.route.getObject(%this.currStop);
   %this.setMoveTransform( %node.getTransform(), false );
error(">>>>>>>>>>>> goto "@%this.currStop@" transform "@%node.getTransform());
}

// clean up when game end
function Quest02::remove(%this)
{
}

function Quest02::stop01(%this, %player, %entering)
{
}

function Quest02::stop02(%this, %player, %entering)
{
}

function Quest02::stop03(%this, %player, %entering)
{
}

function Quest02::stop04(%this, %player, %entering)
{
}

///
/// Default WheeledVehicle datablock callbacks!
///
function DefaultCar::onReachDestination( %this, %obj )
{
	// Moves to the next node on the path.
error(">>>>>>>>>>>> Reached "@%this.currStop);
	%obj.toNextStation();
}

function PsionicJeep0::onReachDestination( %this, %obj )
{
}
