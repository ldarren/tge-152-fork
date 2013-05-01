//-----------------------------------------------------------------------------
// clientCmdSetGhostID() 
//-----------------------------------------------------------------------------
//
// 1. Given a ghost INDEX, get the local object ID that corresponds to it.
// 2. Failing to find a ghost, abort.
// 3. Set the PlayGUI player ghost tracking variable to this ID.
//
function clientCmdSetGhostID( %index ) 
{
   // 1
   %ID = serverConnection.resolveGhostID( %index );

   // 2
   if( !isObject( %ID ) ) return;

   // 3
   playGUI.playerGhost = %ID;
}
