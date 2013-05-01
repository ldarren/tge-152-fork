//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

//----------------------------------------------------------------------------
// Game start / end events sent from the server
//----------------------------------------------------------------------------

function clientCmdGameStart(%seq)
{
}

function clientCmdGameEnd(%seq)
{
   // Stop local activity... the game will be destroyed on the server
   alxStopAll();
}
