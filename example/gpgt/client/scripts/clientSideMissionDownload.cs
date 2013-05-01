//----------------------------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Loading Phases:
// Phase 1: Download Datablocks
// Phase 2: Download Ghost Objects
// Phase 3: Scene Lighting
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Phase 1
//----------------------------------------------------------------------------
function clientCmdMissionStartPhase1( %seq , %missionName , %musicTrack )
{
   // echo("\c4 clientCmdMissionStartPhase1( " , %seq , " , " , %missionName , " , " , %musicTrack , " )" );
   
   onMissionDownloadPhase1(%missionName, %musicTrack);

   Canvas.setContent( LoadingGui );

   commandToServer('MissionStartPhase1Ack', %seq);
}

function onDataBlockObjectReceived( %index , %total )
{
   //echo("\c4 onDataBlockObjectReceived( " , %index , " , " , %total ,  " )" );
   onPhase1Progress( %index , %total );
}


function onMissionDownloadPhase1( %missionName , %musicTrack )
{
   // echo("\c4 onMissionDownloadPhase1( " , %missionName , " , " , %musicTrack , " )" );

   %message = "LOADING DATABLOCKS<br>";

   loadingMLText.setValue("");
   loadingMLText.addText( "<just:center><font:Lucida Console:26>" , true );
   loadingMLText.addText( "<color:b10028><shadowcolor:001a69><shadow:1:1>" , true );
   loadingMLText.addText( %message , true );
   Canvas.repaint();
}

function onPhase1Progress( %index , %total )
{  
   //echo("\c4 onPhase1Progress( " , %index , " , " , %total , " )" );

   %message = "LOADING DATABLOCK<br>" @ %index + 1 @ " of " @ %total;

   loadingMLText.setValue("");
   loadingMLText.addText( "<just:center><font:Lucida Console:26>" , true );
   loadingMLText.addText( "<color:b10028><shadowcolor:001a69><shadow:1:1>" , true );
   loadingMLText.addText( %message , true );

   Canvas.repaint();
}

function onPhase1Complete()
{
   // echo("\c4 onPhase1Complete( )" );
}

//----------------------------------------------------------------------------
// Phase 2
//----------------------------------------------------------------------------
function clientCmdMissionStartPhase2( %seq , %missionName )
{
   // echo("\c4 clientCmdMissionStartPhase2( " , %seq , " , " , %missionName , " )" );
   // echo ("\c4 *** Phase 2: Download Ghost Objects");

   onPhase1Complete();

   purgeResources();

   onMissionDownloadPhase2(%missionName);

   commandToServer( 'MissionStartPhase2Ack' , %seq );
}

function onGhostAlwaysStarted( %ghostCount )
{
   // echo("\c4 onGhostAlwaysStarted( " , %ghostCount , " )" );
   $ghostCount = %ghostCount;
   $ghostsRecvd = 0;
}

function onGhostAlwaysObjectReceived( )
{
   $ghostsRecvd++;

   // echo("\c4 onGhostAlwaysObjectReceived( ) => " , $ghostsRecvd );

   onPhase2Progress( $ghostsRecvd , $ghostCount );
}


function onMissionDownloadPhase2( )
{
   // echo("\c4 onMissionDownloadPhase2( )" );
   %message = "LOADING OBJECTS<br>";

   loadingMLText.setValue("");
   loadingMLText.addText( "<just:center><font:Lucida Console:26>" , true );
   loadingMLText.addText( "<color:b10028><shadowcolor:001a69><shadow:1:1>" , true );
   loadingMLText.addText( %message , true );
   Canvas.repaint();
}

function onPhase2Progress( %index , %total )
{  
   //echo("\c4 onPhase2Progress( " , %index , " , " , %total , " )" );

   %message = "LOADING GHOST<br>" @ %index @ " of " @ %total;

   loadingMLText.setValue("");
   loadingMLText.addText( "<just:center><font:Lucida Console:26>" , true );
   loadingMLText.addText( "<color:b10028><shadowcolor:001a69><shadow:1:1>" , true );
   loadingMLText.addText( %message , true );

   Canvas.repaint();
}

function onPhase2Complete( )
{
   // echo("\c4 onPhase2Complete( )" );
}   

function onFileChunkReceived( %fileName , %bufferOffset , %size )
{
   //echo("\c4 onFileChunkReceived( " , %fileName , " , " ,
   //                              %bufferOffset , " , " , 
   //                              %size , " )" );

   loadingMLText.setValue("");
   loadingMLText.addText( "<just:center><font:Lucida Console:26>" , true );
   loadingMLText.addText( "<color:b10028><shadowcolor:001a69><shadow:1:1>" , true );
   loadingMLText.addText("Downloading " @ %fileName @ "...", true);
}

//----------------------------------------------------------------------------
// Phase 3
//----------------------------------------------------------------------------
function clientCmdMissionStartPhase3( %seq ,%missionName )
{
   // echo("\c4 clientCmdMissionStartPhase3( " , %seq , " , " , %missionName , " )" );
   // echo ("\c4 *** Phase 3: Mission Lighting");

   onPhase2Complete();

   StartClientReplication();  // starts fxShape replication
   StartFoliageReplication(); // starts fxFoliage replication

   $MSeq = %seq;
   $Client::MissionFile = %missionName;

   // Need to light the mission before we are ready.
   // The sceneLightingComplete function will complete the handshake 
   // once the scene lighting is done.
   if (lightScene("sceneLightingComplete", ""))
   {
      error("Lighting mission....");
      schedule(1, 0, "updateLightingProgress");
      onMissionDownloadPhase3(%missionName);
      $lightingMission = true;
   }
}

function updateLightingProgress( )
{
   // echo("\c4 updateLightingProgress( )" );
   onPhase3Progress( $SceneLighting::lightingProgress );
   if ($lightingMission) 
   {
      $lightingProgressThread = schedule(1, 0, "updateLightingProgress");
   }
}

function sceneLightingComplete( )
{
   // echo("\c4 sceneLightingComplete( )" );
   // echo("\c4 Mission lighting done");
   onPhase3Complete();
   
   // The is also the end of the mission load cycle.
   onMissionDownloadComplete();
   commandToServer('MissionStartPhase3Ack', $MSeq);
}


function onMissionDownloadPhase3( )
{
   // echo("\c4 onMissionDownloadPhase3(  )" );

   %message = "LIGHTING MISSION: 0% Done";
   loadingMLText.setValue("");
   loadingMLText.addText( "<just:center><font:Lucida Console:26>" , true );
   loadingMLText.addText( "<color:b10028><shadowcolor:001a69><shadow:1:1>" , true );
   loadingMLText.addText( %message , true );
   Canvas.repaint();
}

function onPhase3Progress( %progress )
{
   //echo("\c4 onPhase3Progress( " , %progress , " )" );

   %message = "LIGHTING MISSION: " @ %progress @ "% Done";

   loadingMLText.setValue("");
   loadingMLText.addText( "<just:center><font:Lucida Console:26>" , true );
   loadingMLText.addText( "<color:b10028><shadowcolor:001a69><shadow:1:1>" , true );
   loadingMLText.addText( %message , true );
}

function onPhase3Complete( )
{
   // echo("\c4 onPhase3Complete( )" );

   $lightingMission = false;
}


//----------------------------------------------------------------------------
// Mission loading done!
//----------------------------------------------------------------------------

function onMissionDownloadComplete( )
{
   // echo("\c4 onMissionDownloadComplete( )" );

   // Client will shortly be dropped into the game, so this is
   // good place for any last minute gui cleanup.
}



//----------------------------------------------------------------------------
// Helper functions
//----------------------------------------------------------------------------

function connect( %serverAddress )
{
   echo("\c4 connect( " , %serverAddress , " )");
   %conn = new GameConnection();
   %conn.connect( %serverAddress );
}