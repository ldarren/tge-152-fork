//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// This script function is called before a client connection
// is accepted.  Returning "" will accept the connection,
// anything else will be sent back as an error to the client.
// All the connect args are passed also to onConnectRequest
//
function GameConnection::onConnectRequest( %clientConn, %netAddress, %name )
{
   //echo("\c5 GameConnection::onConnectRequest( " , %clientConn , " , " , %netAddress , " , " , %name ,  " )" );
   if($Server::PlayerCount >= $pref::Server::MaxPlayers)
   {
      return "CR_SERVERFULL";
   }
   return "";
}

//-----------------------------------------------------------------------------
// This script function is the first called on a client accept
//
function GameConnection::onConnect( %clientConn, %name )
{
   //echo("\c5 GameConnection::onConnect( " , %clientConn , " , " , %name ,  " )" );

   // Simulated client lag for testing...
   // %clientConn.setSimulatedNetParams(0.1, 30);

   // Get the client's unique id:
   // %authInfo = %clientConn.getAuthInfo();
   // %clientConn.guid = getField( %authInfo, 3 );
   %clientConn.guid = 0;
   addToServerGuidList( %clientConn.guid );
   
   // Set admin status
   if (%clientConn.getAddress() $= "local") {
      %clientConn.isAdmin = true;
      %clientConn.isSuperAdmin = true;
   }
   else {
      %clientConn.isAdmin = false;
      %clientConn.isSuperAdmin = false;
   }

   // Save client preferences on the connection object for later use.
   %clientConn.gender = "Male";
   %clientConn.armor = "Light";
   %clientConn.race = "Human";
   %clientConn.skin = addTaggedString( "base" );
   %clientConn.setPlayerName(%name);
   %clientConn.score = 0;

   // 
   $instantGroup = ServerGroup;
   $instantGroup = MissionCleanup;
   //echo("\c5 CADD: " @ %clientConn @ " " @ %clientConn.getAddress());

   // If the mission is running, go ahead download it to the client
   if ($missionRunning)
      %clientConn.loadMission();
   $Server::PlayerCount++;

ServerConnection.setFirstPerson(1);
}

//-----------------------------------------------------------------------------
// A player's name could be obtained from the auth server, but for
// now we use the one passed from the client.
// %realName = getField( %authInfo, 0 );
//
function GameConnection::setPlayerName( %clientConn ,%name )
{
   //echo("\c5 GameConnection::setPlayerName( " , %clientConn , " , " , %name ,  " )" );

   %clientConn.sendGuid = 0;

   // Minimum length requirements
   %name = stripTrailingSpaces( strToPlayerName( %name ) );
   if ( strlen( %name ) < 3 )
      %name = "Poser";

   // Make sure the alias is unique, we'll hit something eventually
   if (!isNameUnique(%name))
   {
      %isUnique = false;
      for (%suffix = 1; !%isUnique; %suffix++)  {
         %nameTry = %name @ "." @ %suffix;
         %isUnique = isNameUnique(%nameTry);
      }
      %name = %nameTry;
   }

   // Tag the name with the "smurf" color:
   %clientConn.nameBase = %name;
   %clientConn.name = addTaggedString("\cp\c8" @ %name @ "\co");
}

function isNameUnique(%name)
{
   %count = ClientGroup.getCount();
   for ( %i = 0; %i < %count; %i++ )
   {
      %test = ClientGroup.getObject( %i );
      %rawName = stripChars( detag( getTaggedString( %test.name ) ), "\cp\co\c6\c7\c8\c9" );
      if ( strcmp( %name, %rawName ) == 0 )
         return false;
   }
   return true;
}

//-----------------------------------------------------------------------------
// This function is called when a client drops for any reason
//
function GameConnection::onDrop( %clientConn , %reason )
{
   //echo("\c5 GameConnection::onDrop( " , %clientConn , " , " , %reason ,  " )" );
   %clientConn.onClientLeaveGame();
   
   removeFromServerGuidList( %clientConn.guid );

   removeTaggedString(%clientConn.name);
   //echo("\c5 CDROP: " @ %clientConn @ " " @ %clientConn.getAddress());
   $Server::PlayerCount--;
   
   // Reset the server if everyone has left the game
   if( $Server::PlayerCount == 0 && $Server::Dedicated)
      schedule(0, 0, "resetServerDefaults");
}


//-----------------------------------------------------------------------------
function GameConnection::startMission( %clientConn )
{
   //echo("\c5 GameConnection::startMission( " , %clientConn , " )" );
   // Inform the client the mission starting
   commandToClient(%clientConn, 'MissionStart', $missionSequence);
}


function GameConnection::endMission( %clientConn )
{
   //echo("\c5 GameConnection::endMission( " , %clientConn , " )" );
   // Inform the client the mission is done
   commandToClient(%clientConn, 'MissionEnd', $missionSequence);
}

//--------------------------------------------------------------------------
// Update all the clients with the new score

function GameConnection::incScore( %clientConn , %delta )
{
   //echo("\c5 GameConnection::incScore( " , %clientConn , " , " , %delta , " )" );
   %clientConn.score += %delta;
}
