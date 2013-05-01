//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

function onServerInfoQuery()
{
   // When the server is queried for information, the value
   // of this function is returned as the status field of
   // the query packet.  This information is accessible as
   // the ServerInfo::State variable.
   return "Rocking!";
}

