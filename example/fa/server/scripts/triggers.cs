
datablock TriggerData( TempleOfEvilTrigger )
{
	// tickPeriodMS is a value is used to control how often the console
	// onTriggerTick callback is called while there are any objects
	// in the trigger. The default value is 100 MS.
	tickPeriodMS = 9000;
};

function TempleOfEvilTrigger::onEnterTrigger( %this, %trigger, %obj )
{
	echo( "The player has just entered the Temple of Evil! - Not the smartest move!");

	// This method is called whenever an object enters the %trigger
	// area, the object is passed as %obj.  The default onEnterTrigger
	// method (in the C++ code) invokes the ::onTrigger(%trigger,1) method on
	// every object (whatever it's type) in the same group as the trigger.
	Parent::onEnterTrigger( %this, %trigger, %obj );
}

function TempleOfEvilTrigger::onLeaveTrigger( %this, %trigger, %obj )
{
	echo( "The player has just left the Temple of Evil! - The player lives to fight another day!");

	// This method is called whenever an object leaves the %trigger
	// area, the object is passed as %obj.  The default onLeaveTrigger
	// method (in the C++ code) invokes the ::onTrigger(%trigger,0) method on
	// every object (whatever it's type) in the same group as the trigger.
	Parent::onLeaveTrigger( %this, %trigger, %obj );
}

function TempleOfEvilTrigger::onTickTrigger( %this, %trigger )
{
	echo( "The player is still in the Temple of Evil! - Calling all monsters!");

	// This method is called every tickPerioMS, as long as any
	// objects intersect the trigger. The default onTriggerTick
	// method (in the C++ code) invokes the ::onTriggerTick(%trigger) method on
	// every object (whatever it's type) in the same group as the trigger.

	// You can iterate through the objects in the list by using these
	// methods:
	//    %this.getNumObjects();
	//    %this.getObject(n);
	Parent::onTickTrigger( %this, %trigger );
}
