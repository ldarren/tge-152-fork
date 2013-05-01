//-----------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//-----------------------------------------------------------------------------

//-------------------------------------------------------------------------
//				TriggerData::onEnterTrigger()
//-------------------------------------------------------------------------
//
// This method is called whenever an object enters the %triggerObj
// area, the object is passed as %objectInTrigger.  The default onEnterTrigger
// method (in the C++ code) invokes the ::onTrigger(%triggerObj,1) method on
// every object (whatever it's type) in the same group as the trigger.
//
//-------------------------------------------------------------------------
function TriggerData::onEnterTrigger( %triggerDB , %triggerObj , %objectInTrigger )
{
   Parent::onEnterTrigger( %triggerDB , %triggerObj , %objectInTrigger );
}

//-------------------------------------------------------------------------
//				TriggerData::onLeaveTrigger()
//-------------------------------------------------------------------------
//
// This method is called whenever an object leaves the %triggerObj
// area, the object is passed as %objectInTrigger.  The default onLeaveTrigger
// method (in the C++ code) invokes the ::onTrigger(%triggerObj,0) method on
// every object (whatever it's type) in the same group as the trigger.
//
//-------------------------------------------------------------------------
function TriggerData::onLeaveTrigger(%triggerDB,%triggerObj,%objectInTrigger)
{
   Parent::onLeaveTrigger( %triggerDB , %triggerObj , %objectInTrigger );
}

//-------------------------------------------------------------------------
//				TriggerData::onTickTrigger()
//-------------------------------------------------------------------------
//
// This method is called every tickPerioMS, as long as any
// objects intersect the trigger. The default onTriggerTick
// method (in the C++ code) invokes the ::onTriggerTick(%triggerObj) method on
// every object (whatever it's type) in the same group as the trigger.
//
// You can iterate through the objects in the list by using these
// methods:
//    %triggerDB.getNumObjects();
//    %triggerDB.getObject(n);
//
//-------------------------------------------------------------------------
function TriggerData::onTickTrigger(%triggerDB,%triggerObj)
{
   Parent::onTickTrigger( %triggerDB , %triggerObj );
}


//-------------------------------------------------------------------------
//				TriggerData::onTrigger() - Group Triggers
//-------------------------------------------------------------------------
//
// This callback is called for every object a SimGroup/Set if the trigger(s)
// in that group are tripped.
//
//-------------------------------------------------------------------------
function ShapeBaseData::onTrigger( %DB , %Obj , %TriggerState )
{
    // Do something 
};


//-------------------------------------------------------------------------
//				TriggerData::onTriggerTick() - Group Triggers
//-------------------------------------------------------------------------
//
// This callback is called for every object a SimGroup/Set if the trigger(s)
// in that group tick.
//
//-------------------------------------------------------------------------
function ShapeBaseData::onTriggerTick( %DB , %Obj )
{
    // Do something 
};
