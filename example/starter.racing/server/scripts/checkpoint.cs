//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

datablock TriggerData(CheckPointTrigger)
{
   // The period is value is used to control how often the console
   // onTriggerTick callback is called while there are any objects
   // in the trigger.  The default value is 100 MS.
   tickPeriodMS = 100;
};

//-----------------------------------------------------------------------------

function CheckPointTrigger::onEnterTrigger(%this,%trigger,%obj)
{
	Parent::onEnterTrigger(%this,%trigger,%obj);

	if(%obj.client.nextCheck == %trigger.checkpoint)
	{
		if(%trigger.isLast)
		{
			// Player has completed a lap.
			%obj.client.lap++;

			if(%obj.client.lap >= $Game::Laps)
			{
				// Increase his score by 1.
				%obj.client.incScore(1);
            	// End the game
             	cycleGame();			
			}
			else {
				%obj.client.nextCheck = 1;
				commandToClient(%obj.client, 'IncreaseLapCounter');
			}
		}
		else {
			// Continue to the next one.
			%obj.client.nextCheck++;
		}
	}
	else
		centerPrint(%obj.client, "Wrong Checkpoint!", 1, 1);
}
