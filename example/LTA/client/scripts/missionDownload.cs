// ============================================================
// Project            :  LTA: VR-20
// File               :  .\lta\client\scripts\missionDownload.cs
// Copyright          :  
// Author             :  ldarren
// Created on         :  Wednesday, June 20, 2007 1:33 AM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

// LOADING DATABLOCKS
function onMissionDownloadPhase1(%missionName, %musicTrack)
{
   // Close and clear the message hud (in case it's open)
   MessageHud.close();
   //cls();

   // Reset the loading progress controls:
   LoadingProgress.setValue(0);
}

function onPhase1Progress(%progress)
{
   LoadingProgress.setValue(%progress/3);
   Canvas.repaint();
}

function onPhase1Complete()
{
}

//----------------------------------------------------------------------------
// Phase 2
//----------------------------------------------------------------------------

// LOADING OBJECTS
function onMissionDownloadPhase2()
{
   // Reset the loading progress controls:
   LoadingProgress.setValue(0.333);
   Canvas.repaint();
}

function onPhase2Progress(%progress)
{
   LoadingProgress.setValue(0.333 + %progress/3);
   Canvas.repaint();
}

function onPhase2Complete()
{
}   

//  Downloading mission files
function onFileChunkReceived(%fileName, %ofs, %size)
{
   LoadingProgress.setValue(0.333 + (%ofs / %size)/3);
}

//----------------------------------------------------------------------------
// Phase 3
//----------------------------------------------------------------------------

//  LIGHTING MISSION
function onMissionDownloadPhase3()
{
   LoadingProgress.setValue(0.666);
   Canvas.repaint();
}

function onPhase3Progress(%progress)
{
   LoadingProgress.setValue(0.666 + %progress/3);
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
}


//------------------------------------------------------------------------------
// Before downloading a mission, the server transmits the mission
// information through these messages.
//------------------------------------------------------------------------------

addMessageCallback( 'MsgLoadInfo', handleLoadInfoMessage );
addMessageCallback( 'MsgLoadDescripition', handleLoadDescriptionMessage );
addMessageCallback( 'MsgLoadInfoDone', handleLoadInfoDoneMessage );

//------------------------------------------------------------------------------

function handleLoadInfoMessage( %msgType, %msgString, %mapName ) 
{
	// Need to pop up the loading gui to display this stuff.
   if (!LoadingGui.isAwake()) Canvas.setContent("LoadingGui");
}

//------------------------------------------------------------------------------

function handleLoadDescriptionMessage( %msgType, %msgString, %line )
{
}

//------------------------------------------------------------------------------

function handleLoadInfoDoneMessage( %msgType, %msgString )
{
   // This will get called after the last description line is sent.
}
