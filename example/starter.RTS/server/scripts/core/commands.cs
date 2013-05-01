//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Misc. server commands available to clients
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function serverCmdToggleCamera(%client)
{
   %control = %client.getControlObject();
   if (%control == %client.rtsCam)
   {
      %control = %client.observerCam;
      %control.mode = toggleCameraFly;
   }
   else
   {
      %control = %client.rtsCam;
      %control.mode = observerFly;
   }
   %client.setControlObject(%control);
}
	
function serverCmdDropPlayerAtCamera(%client)
{
   if ($Server::TestCheats || isObject(EditorGui))
   {
      %client.rtsCam.setTransform(%client.camera.getTransform());
      %client.rtsCam.setVelocity("0 0 0");
      %client.setControlObject(%client.rtsCam);
   }
}

function serverCmdDropCameraAtPlayer(%client)
{
   %client.observerCam.setTransform(%client.rtsCam.getEyeTransform());
   %client.observerCam.setVelocity("0 0 0");
   %client.setControlObject(%client.observerCam);
}


//-----------------------------------------------------------------------------

function serverCmdSuicide(%client)
{
   if (isObject(%client.player))
      %client.player.kill("Suicide");
}   

function serverCmdPlayCel(%client,%anim)
{
   if (isObject(%client.player))
      %client.player.playCelAnimation(%anim);
}

function serverCmdPlayDeath(%client)
{
   if (isObject(%client.player))
      %client.player.playDeathAnimation();
}


//-----------------------------------------------------------------------------
// Server interface to client selection.
//
// As the client selects and deselects units, it sends commands to the server
// so that the server is aware of its selection. This selection is kept in
// %client.selection. Since commands are guaranteed to arrive in order, we
// implicitly apply any issues (see next section) to the current selection.
//-----------------------------------------------------------------------------
function serverCmdAddToSelection(%client, %objID)
{
   %obj = %client.resolveObjectFromGhostIndex(%objID);
   %client.selection.add(%obj);
   
   sendAllStatsToClient(%client, %obj);
}

function serverCmdRemoveFromSelection(%client, %objID)
{
   %obj = %client.resolveObjectFromGhostIndex(%objID);
   %client.selection.remove(%obj);
}

function serverCmdClearSelection(%client)
{
   %client.selection.clear();
}

//-----------------------------------------------------------------------------
//
// Server interface for commands.
//
// These commands are applied to the current selection (see above for
// explanation). These are referred to as issues, since they all start with
// "issue."
//
// The relevant commands are:
//   issueAttack(%targetID)   -   Orders selected units to attack specified target.
//   issueMove(%x, %y, %z)    -   Orders selected units to move to specified location.
//   issueStop()              -   Orders selected units to stop attack/moving.
//
//   applyModifier(%modifier, %targetID, %duration) - Applies an RTSModifier to specified target.
//-----------------------------------------------------------------------------
function serverCmdIssueAttack(%client, %targetID)
{
   %target = %client.resolveObjectFromGhostIndex(%targetID);

   if (!isObject(%target))
   {
   	error("serverCmdSetTarget - Invalid target id" SPC %targetID SPC " cannot be resolved to object.");
   	return;
   }

   for (%i = 0; %i < ClientGroup.getCount(); %i++)
   {
      %cl = ClientGroup.getObject(%i);
      %cl.sendAttackEvent(%client.selection, %target);
   }
   
   for(%i=0; %i<%client.selection.getCount(); %i++)
   {
      %client.selection.getObject(%i).setAimObject(%target);
   }
}

function serverCmdIssueMove(%client, %x, %y, %z)
{
   if (%client.selection.getObject(0).getTeam() != %client.getTeam())
      return;

   for (%i = 0; %i < ClientGroup.getCount(); %i++)
   {
      %cl = ClientGroup.getObject(%i);
      %cl.sendMoveEvent(%client.selection, %x SPC %y);
   }
   
   %center = "0 0 0";
   for (%i = 0; %i < %client.selection.getCount(); %i++)
   {
      %center = VectorAdd(%center, %client.selection.getObject(%i).getPosition());
   }
   %center = VectorScale(%center, 1.0 / %client.selection.getCount());
   
   for(%i = 0; %i < %client.selection.getCount(); %i++)
   {
      %obj = %client.selection.getObject(%i);
      
      %dest = %x SPC %y SPC %z;
      %offset = VectorSub(%obj.getPosition(), %center);
      %dest = VectorAdd(%dest, %offset);
      %obj.clearAim();
      %obj.resourceType = "";
      %obj.TCLoc = "";
      %obj.resourcePos = "";
      %obj.status = "Idle";
      %obj.curGoal = "";
      if(isEventPending(%obj.resourceID) )
      {
        cancel(%obj.resourceID);
      }

      %obj.setMoveGoal(%dest);
   }
}

function serverCmdIssueResourceMove(%client, %x, %y, %z, %type)
{
echo("serverCmdIssueResourceMove--looking for resource object (" @ %type @
      ") at" SPC %x SPC %y SPC %z );
 
   if (%client.selection.getObject(0).getTeam() != %client.getTeam())
      return;
   %destinationResource = %client.resolveObjectFromGhostIndex(%type);
echo("Resolved (" @ %type @ ") to server object (" @ %destinationResource @ ")");
      
   for (%i = 0; %i < ClientGroup.getCount(); %i++)
   {
      %cl = ClientGroup.getObject(%i);
      %cl.sendMoveEvent(%client.selection, %x SPC %y);
   }

   %center = "0 0 0";
   for (%i = 0; %i < %client.selection.getCount(); %i++)
   {
      %center = VectorAdd(%center, %client.selection.getObject(%i).getPosition());
   }
   %center = VectorScale(%center, 1.0 / %client.selection.getCount());

   for(%i = 0; %i < %client.selection.getCount(); %i++)
   {
      %obj = %client.selection.getObject(%i);
      //echo(%obj.getDataBlock().getName());

      if(%obj.getDataBlock().getName() !$= "villagerBlock") continue;
      if (!isObject(%destinationResource) )
      {
        echo("serverCmdIssueResourceMove--Failed resolving resource object, client thought (" @ %type @
             ") server thought it resolved to (" @ %destinationResource @ ")");
        return;
      }      
      %obj.resourceType = %destinationResource.resourceType;
      %obj.resourcePos = %destinationResource.getPosition();
      %obj.status = "Collecting";
      %obj.currentAmt = 0;
      
echo("serverCmdIssueResourceMove-- (" @ %obj @ ") is moving to get resource (" @ 
     %obj.resourceType @ ") at (" @ %obj.resourcePos @ ")");
      %dest = %x SPC %y SPC %z;
      %offset = VectorSub(%obj.getPosition(), %center);
      %dest = VectorAdd(%dest, %offset);
      %obj.clearAim();

      %obj.setMoveGoal(%dest);
   }
}

function serverCmdIssueStop(%client)
{
   for(%i=0; %i<%client.selection.getCount(); %i++)
   {
      %obj = %client.selection.getObject(%i);
      if(%client.team == %obj.team)
      {
         %obj.stopAll();
      }
   }
}

function serverCmdApplyModifier(%client, %modifier, %targetID, %duration)
{
   %target = %client.resolveObjectFromGhostIndex(%targetID);
   %target.addModifier(%modifier.getID());
   %target.schedule(%duration, "removeModifier", %modifier.getID());
}
