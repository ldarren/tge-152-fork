//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Misc. server commands avialable to clients
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

function serverCmdSpawnPlayer(%client)
{
  %client.spawnPlayer(false);
}

function serverCmdToggleCamera(%client)
{
  if (!isObject(%client.player))
    return;
  
  %control = %client.getControlObject();
  if (%control == %client.player)
  {
    %control = %client.camera;
    %control.mode = "Fly";
    %client.camera.setFlyMode();
    snapAtlasGeometryMorph();
  }
  else
  {
    %control = %client.player;
    %client.camera.setCameraSubject(%client.player); 
    %client.camera.setThirdPersonMode();
    snapAtlasGeometryMorph();
  }
  %client.setControlObject(%control); 
}

function serverCmdDropPlayerAtCamera(%client)
{
  if (!isObject(%client.player))
    return;

  if ($Server::TestCheats || isObject(EditorGui))
  {
    %control = %client.getControlObject();
    if (%control == %client.player)
    {
      %control.mode = "Fly";
      %client.camera.setFlyMode();
      %client.player.setTransform(%client.camera.getTransform());
      %client.player.setVelocity("0 0 0");
      %client.camera.setThirdPersonMode();
      snapAtlasGeometryMorph();
    }
    else
    {
      %client.player.setTransform(%client.camera.getTransform());
      %client.player.setVelocity("0 0 0");
      snapAtlasGeometryMorph();
    }
  }
}

function serverCmdDropCameraAtPlayer(%client)
{
  if (!isObject(%client.player))
    return;
  
  %control = %client.getControlObject();
  if (%control == %client.player)
  {
    %control = %client.camera;
    %control.mode = "Fly";
    %client.camera.setFlyMode();
    %client.camera.setTransform(%client.player.getEyeTransform());
    %client.camera.setVelocity("0 0 0");
    %client.setControlObject(%client.camera);
    snapAtlasGeometryMorph();
  }
  else
  {
    %client.camera.setTransform(%client.player.getEyeTransform());
    %client.camera.setVelocity("0 0 0");
    snapAtlasGeometryMorph();
  }
}


//-----------------------------------------------------------------------------

function serverCmdSelfDestruct(%client)
{
   if (isObject(%client.player))
      %client.player.kill("SelfDestruction");
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

function serverCmdCastSpellbookSpell(%client, %book_slot, %target_ghost)
{
  if (%target_ghost != -1)
    %target = %client.ResolveGhost(%target_ghost);
  else
    %target = -1;

  afxPerformSpellbookCast(%client.player, %book_slot, %target, %client);
}

function serverCmdDoGreatBallCast(%client, %target_ghost)
{
  if (%target_ghost != -1)
    %target = %client.ResolveGhost(%target_ghost);
  else
    %target = -1;

  afxPerformSpellCast(%client.player, GreatBallSpell, %target, %client);
}

function serverCmdDoPhraseTesterPush(%client)
{
  performPhraseTesterPush(%client.player);
}

function serverCmdDoPhraseTesterHalt(%client)
{
  performPhraseTesterHalt(%client.player);
}

function serverCmdInterruptSpellcasting(%client)
{
  performSpellCastingInterrupt(%client);
}

function serverCmdInflictDamage(%client)
{
  if (isObject(%client.player))
    %client.player.damage(0, 0, 10, "User");
}

//function serverCmdGetSpellIcon(%client, %book_slot)
//{
//  return %client.spellbook.getSpellIcon(%book_slot);
//}

function dollyCamAwayFromPlayer(%client)
{   
  %offset = %client.camera.getThirdPersonOffset();

  %x_pos = firstWord(%offset);
  %y_pos = getWord(%offset,1);
  %z_pos = getWord(%offset,2);

  if ($firstPerson)
  {
    %y_pos = -1.0;
    // echo("LEAVE FIRST PERSON");
  }
  else
  {
    %y_pos -= 0.5;
  }

  %client.camera.setThirdPersonOffset(%x_pos @ " " @ %y_pos @ " " @ %z_pos);
}

function dollyCamTowardPlayer(%client)
{   
  if (!$firstPerson)
  {
    %offset = %client.camera.getThirdPersonOffset();

    %x_pos = firstWord(%offset);
    %y_pos = getWord(%offset,1);
    %z_pos = getWord(%offset,2);

    if (%y_pos < -1.0)
    {
      %y_pos += 0.5;
    }
    else
    {
      %y_pos = -0.5;
      //echo("GOTO FIRST PERSON");
    }

    %client.camera.setThirdPersonOffset(%x_pos @ " " @ %y_pos @ " " @ %z_pos);
  }
}

function serverCmdDollyThirdPersonCam(%client, %toward)
{
  if (%toward)
  {
    dollyCamTowardPlayer(%client);
  }
  else
  {
    dollyCamAwayFromPlayer(%client); 
  }
}

$save_third_person_y = -3;

// Note - the third-person camera auto-switches to and from
// first person mode based on the y value of the third-person
// offset. The threshold is -1.0. Values greater than the
// threshold force a switch to first-person-mode, values less
// than or equal to -1.0, force a switch to third-person mode.

function serverCmdToggleFirstPersonPOV(%client)
{
  %offset = %client.camera.getThirdPersonOffset();
  %x_pos = firstWord(%offset);
  %y_pos = getWord(%offset,1);
  %z_pos = getWord(%offset,2);

  // save third_person y-offset, force first-person on
  if (%y_pos < -0.5)
  {
    $save_third_person_y = %y_pos;
    %y_pos = -0.5;
  }
  // restore third_person y-offset, force first-person off
  else
  {
    %y_pos = $save_third_person_y;
  }

  %client.camera.setThirdPersonOffset(%x_pos SPC %y_pos SPC %z_pos);
  %client.camera.setThirdPersonSnap();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// DATABLOCK CACHE CODE <<

// This function is an alternate to the common serverCmdMissionStartPhase1Ack()
// found in common/server/missionDownload.cs. This version is called when
// we want to load datablocks from a cache rather than have them transmitted
// from the server.
// 
function serverCmdMissionStartPhase1Ack_UseCache(%client, %seq)
{
  echo("<<<< client will load datablocks from a cache >>>>");
  echo("    <<<< skipping datablock transmission >>>>");

  // Make sure to ignore calls from a previous mission load
  if (%seq != $missionSequence || !$MissionRunning)
    return;
  if (%client.currentPhase != 0)
    return;
  %client.currentPhase = 1;

  // Start with the CRC
  %client.setMissionCRC( $missionCRC );

  %client.onBeginDatablockCacheLoad($missionSequence);
}

function GameConnection::onBeginDatablockCacheLoad( %this, %missionSequence )
{
   // Make sure to ignore calls from a previous mission load
   if (%missionSequence != $missionSequence)
      return;
   if (%this.currentPhase != 1)
      return;
   %this.currentPhase = 1.5;
   commandToClient(%this, 'MissionStartPhase1_LoadCache', $missionSequence, $Server::MissionFile);
}

// This function overrides a function with the same name found in
// common/server/missionDownload.cs. When datablock caching is enabled
// on the server, it tells the server to save the cache as necessary,
// calculates the cache's CRC, and sends it to the client along with
// other mission info.
function GameConnection::loadMission(%this)
{
  %cache_crc = "";

  if ($Pref::Server::EnableDatablockCache)
  {
    if (!isDatablockCacheSaved())
    {
      echo("<<<< saving server datablock cache >>>>");
      %this.saveDatablockCache();
    }

    if (isFile($Pref::Server::DatablockCacheFilename))
    {
      %cache_crc = getDatablockCacheCRC();
      echo("    <<<< sending CRC to client:" SPC %cache_crc SPC ">>>>");
    }
  }

  // Send over the information that will display the server info
  // when we learn it got there, we'll send the data blocks
  %this.currentPhase = 0;
  if (%this.isAIControlled())
  {
    // Cut to the chase...
    %this.onClientEnterGame();
  }
  else
  {
    commandToClient(%this, 'MissionStartPhase1', $missionSequence,
      $Server::MissionFile, MissionGroup.musicTrack, %cache_crc);
    echo("*** Sending mission load to client: " @ $Server::MissionFile);
  }
}
// DATABLOCK CACHE CODE >>
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
