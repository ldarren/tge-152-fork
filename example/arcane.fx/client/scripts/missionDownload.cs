//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Mission Loading & Mission Info
// The mission loading server handshaking is handled by the
// common/client/missingLoading.cs.  This portion handles the interface
// with the game GUI.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Loading Phases:
// Phase 1: Download Datablocks
// Phase 2: Download Ghost Objects
// Phase 3: Scene Lighting

//----------------------------------------------------------------------------
// Phase 1
//----------------------------------------------------------------------------

function onMissionDownloadPhase1(%missionName, %musicTrack, %alt_msg)
{
  // Close and clear the message hud (in case it's open)
  MessageHud.close();
  //cls();

  // Reset the loading progress controls:
  LoadingProgress.setValue(0);
  LoadingProgressTxt.setValue((%alt_msg $= "") ? "LOADING DATABLOCKS" : %alt_msg);
}

function onPhase1Progress(%progress)
{
   LoadingProgress.setValue(%progress);
   Canvas.repaint();
}

function onPhase1Complete()
{
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// DATABLOCK CACHE CODE <<

$loadFromDatablockCache = false;

//
// This function overrides a function with the same name found in
// common/client/missionDownload.cs. This replacement decides if the
// client can use a datablock cache instead of requiring transmission
// of datablocks form the server.
// 
function clientCmdMissionStartPhase1(%seq, %missionName, %musicTrack, %cache_crc)
{
  // These need to come after the cls.
  echo ("*** New Mission: " @ %missionName);
  echo ("*** Phase 1: Download Datablocks & Targets");

  $loadFromDatablockCache = false;
  if ($pref::Client::EnableDatablockCache)
  {
    %cache_filename = $pref::Client::DatablockCacheFilename;

    // if cache CRC is provided, check for validity
    if (%cache_crc !$= "")
    {
      // check for existence of cache file
      if (isFile(%cache_filename))
      { 
        // here we are not comparing the CRC of the cache itself, but the CRC of
        // the server cache (stored in the header) when these datablocks were
        // transmitted.
        %my_cache_crc = extractDatablockCacheCRC(%cache_filename);
        echo("<<<< client cache CRC:" SPC %my_cache_crc SPC ">>>>");
        echo("<<<< comparing CRC codes:" SPC "s:" @ %cache_crc SPC "c:" @ %my_cache_crc SPC ">>>>");
        if (%my_cache_crc == %cache_crc)
        {
          echo("<<<< cache CRC codes match, datablocks will be loaded from local cache. >>>>");
          $loadFromDatablockCache = true;
        }
        else
        {
          echo("<<<< cache CRC codes differ, datablocks will be transmitted and cached. >>>>" SPC %cache_crc);
          setDatablockCacheCRC(%cache_crc);
        }
      }
      else
      {
        echo("<<<< client datablock cache does not exist, datablocks will be transmitted and cached. >>>>");
        setDatablockCacheCRC(%cache_crc);
      }
    }
    else
    {
      echo("<<<< server datablock caching is disabled, datablocks will be transmitted. >>>>");
    }
    if ($loadFromDatablockCache)
    {
      // skip datablock transmission and initiate a cache load
      onMissionDownloadPhase1(%missionName, %musicTrack, "LOADING CACHED DATABLOCKS");
      commandToServer('MissionStartPhase1Ack_UseCache', %seq);
      return;
    }
  }
  else if (%cache_crc !$= "")
  {
    echo("<<<< client datablock caching is disabled, datablocks will be transmitted. >>>>");
  }

  // initiate a datablock transmission
  onMissionDownloadPhase1(%missionName, %musicTrack);
  commandToServer('MissionStartPhase1Ack', %seq);
}

function clientCmdMissionStartPhase1_LoadCache(%seq, %missionName)
{
  if ($pref::Client::EnableDatablockCache && $loadFromDatablockCache)
  {
    echo("<<<< Loading Datablocks From Cache >>>>");
    if (ServerConnection.loadDatablockCache_Begin())
    {
      schedule(10, 0, "updateLoadDatablockCacheProgress", %seq, %missionName);
    }
  }
}

function updateLoadDatablockCacheProgress(%seq, %missionName)
{
   if (ServerConnection.loadDatablockCache_Continue())
   {
      $loadDatablockCacheProgressThread = schedule(10, 0, "updateLoadDatablockCacheProgress", %seq, %missionName);
      return;
   }
 
   echo("<<<< Finished Loading Datablocks From Cache >>>>");
   clientCmdMissionStartPhase2(%seq,%missionName);
}
// DATABLOCK CACHE CODE >>
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

//----------------------------------------------------------------------------
// Phase 2
//----------------------------------------------------------------------------

function onMissionDownloadPhase2()
{
   // Reset the loading progress controls:
   LoadingProgress.setValue(0);
   LoadingProgressTxt.setValue("LOADING OBJECTS");
   Canvas.repaint();
}

function onPhase2Progress(%progress)
{
   LoadingProgress.setValue(%progress);
   Canvas.repaint();
}

function onPhase2Complete()
{
}   

function onFileChunkReceived(%fileName, %ofs, %size)
{
   LoadingProgress.setValue(%ofs / %size);
   LoadingProgressTxt.setValue("Downloading " @ %fileName @ "...");
}

//----------------------------------------------------------------------------
// Phase 3
//----------------------------------------------------------------------------

function onMissionDownloadPhase3()
{
   LoadingProgress.setValue(0);
   LoadingProgressTxt.setValue("LIGHTING MISSION");
   Canvas.repaint();
}

function onPhase3Progress(%progress)
{
   LoadingProgress.setValue(%progress);
}

function onPhase3Complete()
{
   LoadingProgress.setValue( 1 );
   $lightingMission = false;
}

//----------------------------------------------------------------------------
// Mission loading done!
//----------------------------------------------------------------------------

function onMissionDownloadComplete()
{
   // Client will shortly be dropped into the game, so this is
   // good place for any last minute gui cleanup.
   resetSelectronStyle();
}


//------------------------------------------------------------------------------
// Before downloading a mission, the server transmits the mission
// information through these messages.
//------------------------------------------------------------------------------

addMessageCallback( 'MsgLoadInfo', handleLoadInfoMessage );
addMessageCallback( 'MsgLoadDescripition', handleLoadDescriptionMessage );
addMessageCallback( 'MsgLoadInfoDone', handleLoadInfoDoneMessage );

//------------------------------------------------------------------------------

function handleLoadInfoMessage( %msgType, %msgString, %mapName ) {
	
	// Need to pop up the loading gui to display this stuff.
	Canvas.setContent("LoadingGui");

	// Clear all of the loading info lines:
	for( %line = 0; %line < LoadingGui.qLineCount; %line++ )
		LoadingGui.qLine[%line] = "";
	LoadingGui.qLineCount = 0;

   //
	LOAD_MapName.setText( %mapName );
}

//------------------------------------------------------------------------------

function handleLoadDescriptionMessage( %msgType, %msgString, %line )
{
	LoadingGui.qLine[LoadingGui.qLineCount] = %line;
	LoadingGui.qLineCount++;

   // Gather up all the previous lines, append the current one
   // and stuff it into the control
	%text = "<spush><font:Arial:16>";
	
	for( %line = 0; %line < LoadingGui.qLineCount - 1; %line++ )
		%text = %text @ LoadingGui.qLine[%line] @ " ";
   %text = %text @ LoadingGui.qLine[%line] @ "<spop>";

	LOAD_MapDescription.setText( %text );
}

//------------------------------------------------------------------------------

function handleLoadInfoDoneMessage( %msgType, %msgString )
{
   // This will get called after the last description line is sent.
}
