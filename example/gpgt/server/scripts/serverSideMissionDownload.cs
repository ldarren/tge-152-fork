//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Mission Loading
// The server portion of the client/server mission loading process
//-----------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Loading Phases:
// Phase 1: Transmit Datablocks
// Phase 2: Transmit Ghost Objects
// Phase 3: Scene Lighting

//
// The server invokes the client MissionStartPhase[1-3] function to request
// permission to start each phase.  When a client is ready for a phase,
// it responds with MissionStartPhase[1-3]Ack.

function GameConnection::loadMission( %clientConn )
{
   //echo("\c5 GameConnection::loadMission( " , %clientConn , " )" );
   // Send over the information that will display the server info
   // when we learn it got there, we'll send the data blocks
   %clientConn.currentPhase = 0;
   if (%clientConn.isAIControlled())
   {
      // Cut to the chase...
      %clientConn.onClientEnterGame();
   }
   else
   {
      commandToClient(%clientConn, 'MissionStartPhase1', $missionSequence,
         $Server::MissionFile, MissionGroup.musicTrack);
      echo("*** Sending mission load to client: " @ $Server::MissionFile);
   }
}

function serverCmdMissionStartPhase1Ack( %clientConn , %seq )
{
   //echo("\c5 serverCmdMissionStartPhase1Ack( " , %clientConn , " , " , %seq , " )" );
   // Make sure to ignore calls from a previous mission load
   if (%seq != $missionSequence || !$MissionRunning)
      return;
   if (%clientConn.currentPhase != 0)
      return;
   %clientConn.currentPhase = 1;

   // Start with the CRC
   %clientConn.setMissionCRC( $missionCRC );

   // Send over the datablocks...
   // OnDataBlocksDone will get called when have confirmation
   // that they've all been received.
   %clientConn.transmitDataBlocks($missionSequence);
}

function GameConnection::onDataBlocksDone( %clientConn , %seq )
{
   //echo("\c5 GameConnection::onDataBlocksDone( " , %clientConn , " , " , %seq , " )" );
   // Make sure to ignore calls from a previous mission load
   if (%seq != $missionSequence)
      return;
   if (%clientConn.currentPhase != 1)
      return;
   %clientConn.currentPhase = 1.5;

   // On to the next phase
   commandToClient(%clientConn, 'MissionStartPhase2', $missionSequence, $Server::MissionFile);
}

function serverCmdMissionStartPhase2Ack( %clientConn , %seq )
{
   //echo("\c5 serverCmdMissionStartPhase2Ack( " , %clientConn , " , " , %seq , " )" );
   // Make sure to ignore calls from a previous mission load
   if (%seq != $missionSequence || !$MissionRunning)
      return;
   if (%clientConn.currentPhase != 1.5)
      return;
   %clientConn.currentPhase = 2;

   // Update mod paths, this needs to get there before the objects.
   %clientConn.transmitPaths();

   // Start ghosting objects to the client
   %clientConn.activateGhosting();
   
}

function GameConnection::clientWantsGhostAlwaysRetry( %clientConn )
{
   //echo("\c5 GameConnection::clientWantsGhostAlwaysRetry( " , %clientConn , " )" );
   if($MissionRunning)
   {
      %clientConn.activateGhosting();
   }
}

function GameConnection::onGhostAlwaysFailed( %clientConn )
{
   //echo("\c5 GameConnection::onGhostAlwaysFailed( " , %clientConn , " )" );

}

function GameConnection::onGhostAlwaysObjectsReceived( %clientConn )
{
   //echo("\c5 GameConnection::onGhostAlwaysObjectsReceived( " , %clientConn , " )" );
   // Ready for next phase.
   commandToClient(%clientConn, 'MissionStartPhase3', $missionSequence, $Server::MissionFile);
}

function serverCmdMissionStartPhase3Ack( %clientConn , %seq )
{
   //echo("\c5 serverCmdMissionStartPhase3Ack( " , %clientConn , " , " , %seq , " )" );

   // Make sure to ignore calls from a previous mission load
   if(%seq != $missionSequence || !$MissionRunning)
      return;

   if(%clientConn.currentPhase != 2)
      return;

   %clientConn.currentPhase = 3;

   // Server is ready to drop into the game
   %clientConn.startMission();
   %clientConn.onClientEnterGame();

}

