//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//    Changes:
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

function DisplayScreenMessage(%client, %message)
{
  if (isObject(%client))
    commandToClient(%client, 'DisplayScreenMessage', %message, %time);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

function BroadcastScreenMessage(%message)
{
  %count = ClientGroup.getCount();
  for (%i = 0; %i < %count; %i++)
  {
    %cl = ClientGroup.getObject(%i);
    if( !%cl.isAIControlled() )
      commandToClient(%cl, 'DisplayScreenMessage', %message);
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// These are called from loadMission() in example/common/server/missionLoad.cs. 
// Declaring these empty functions here prevents an error message without editing
// any files in the common area.

function clearCenterPrintAll()
{
}

function clearBottomPrintAll()
{
}