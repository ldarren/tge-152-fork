//-----------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-----------------------------------------------------------------------------

//-------------------------------------------------------------------------
//						TriggerData
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
//						|- TriggerData 
//
//-------------------------------------------------------------------------
datablock TriggerData( baseTrigger )
{
   // The period is value is used to control how often the console
   // onTriggerTick callback is called while there are any objects
   // in the trigger.  The default value is 100 MS.

   tickPeriodMS = 100;
};


