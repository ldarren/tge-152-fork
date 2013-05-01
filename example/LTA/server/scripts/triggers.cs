
// tickPeriodMS is a value is used to control how often the console
// onTriggerTick callback is called while there are any objects
// in the trigger. The default value is 100 MS.
datablock TriggerData(CheckpointTrigger)
{   
	tickPeriodMS = 1000;
};

// This method is called whenever an object enters the %trigger
// area, the object is passed as %obj.  The default onEnterTrigger
// method (in the C++ code) invokes the ::onTrigger(%trigger,1) method on
// every object (whatever it's type) in the same group as the trigger.
function CheckpointTrigger::onEnterTrigger(%this,%trigger,%obj)
{
	if (%obj.client == 0) return;
	commandtoclient(%obj.client, 'SetCheckPoint',%trigger.getname());   
	Parent::onEnterTrigger(%this,%trigger,%obj);
}

// This method is called whenever an object leaves the %trigger
// area, the object is passed as %obj.  The default onLeaveTrigger
// method (in the C++ code) invokes the ::onTrigger(%trigger,0) method on
// every object (whatever it's type) in the same group as the trigger.
function CheckpointTrigger::onLeaveTrigger(%this,%trigger,%obj)
{   
	Parent::onLeaveTrigger(%this,%trigger,%obj);
}

// This method is called every tickPerioMS, as long as any
// objects intersect the trigger. The default onTriggerTick
// method (in the C++ code) invokes the ::onTriggerTick(%trigger) method on
// every object (whatever it's type) in the same group as the trigger.

// You can iterate through the objects in the list by using these
// methods:
//    %this.getNumObjects();
//    %this.getObject(n);
function CheckpointTrigger::onTickTrigger(%this,%trigger)
{   
	Parent::onTickTrigger(%this,%trigger);
}

datablock TriggerData(QuestTrigger)
{
	tickPeriodMS = 1000;
};

function QuestTrigger::onEnterTrigger(%this,%trigger,%player)
{
	eval(%trigger.command@"("@%player@",true);");
}

function QuestTrigger::onLeaveTrigger(%this,%trigger,%player)
{
	eval(%trigger.command@"("@%player@",false);");
}

function QuestTrigger::onTickTrigger(%this,%trigger)
{
	Parent::onTickTrigger(%this,%trigger);
}
