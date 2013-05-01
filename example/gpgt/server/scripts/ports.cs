//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// resetMission()
//-----------------------------------------------------------------------------
//
// 1. Attempt to open the specied port number.  Failing that, try up to 10
//    incremental port numbers.
// 2. Return either 0 (failure), or the port number used.
//
function portInit( %port )
{
   // 1
   %failCount = 0;

   while( %failCount < 10 && !setNetPort( %port ) ) 
   {
      warn("Port init failed on port " @ %port @ " trying next port.");
      %port++; 
      %failCount++;
   }

   // 2
   return ( %failCount ? 0 : %port );
}
 
