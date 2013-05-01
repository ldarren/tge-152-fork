//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

//----------------------------------------------------------------------------
// Game start / end events sent from the server
//----------------------------------------------------------------------------

function clientCmdGameStart( )
{
   // echo("\c4 clientCmdGameStart()");

   playGUI.initialize();

}

function clientCmdGameEnd( )
{
   // echo("\c4 clientCmdGameEnd()");

      // 1
   Canvas.setContent( EndGameGui );

   alxStopAll();
}

