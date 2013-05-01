//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

// Functions dealing with connecting to a server

//----------------------------------------------------------------------------
// GameConnection client callbacks
//----------------------------------------------------------------------------

function GameConnection::initialControlSet( %serverConn )
{
   // echo("\c4 GameConnection::initialControlSet( " , %serverConn , " )" );

   // first check if the editor is active
   if (!Editor::checkActiveLoadDone())
   {
      if (Canvas.getContent() != PlayGui.getId())
      {
         Canvas.setContent(PlayGui);
      }      
   }
}

function GameConnection::setLagIcon( %serverConn , %state )
{
   // echo("\c4 GameConnection::setLagIcon( " , %serverConn , " , " , %state ,  " )" );

   if (%serverConn.getAddress() $= "local") return;

   LagIcon.setVisible(%state $= "true");
}

function GameConnection::onConnectionAccepted( %serverConn )
{
   // echo("\c4 GameConnection::onConnectionAccepted( " , %serverConn , " )" );
   LagIcon.setVisible(false);
}

function GameConnection::onConnectionTimedOut( %serverConn )
{
   // echo("\c4 GameConnection::onConnectionTimedOut( " , %serverConn , " )" );

   disconnectedCleanup();
   MessageBoxOK( "TIMED OUT", "The server connection has timed out.");
}

function GameConnection::onConnectionDropped( %serverConn , %msg )
{
   // echo("\c4 GameConnection::onConnectionDropped( " , %serverConn , " , " , %msg ,  " )" );
   // Established connection was dropped by the server
   disconnectedCleanup();
   MessageBoxOK( "DISCONNECT", "The server has dropped the connection: " @ %msg);
}

function GameConnection::onConnectionError(%serverConn, %msg)
{
   // echo("\c4 GameConnection::onConnectionError( " , %serverConn , " , " , %msg ,  " )" );

   // General connection error, usually raised by ghosted objects
   // initialization problems, such as missing files.  We'll display
   // the server's connection error message.
   disconnectedCleanup();
   MessageBoxOK( "DISCONNECT", $ServerConnectionErrorMessage @ " (" @ %msg @ ")" );
}


//----------------------------------------------------------------------------
// Connection Failed Events
//----------------------------------------------------------------------------
function GameConnection::onConnectRequestRejected( %serverConn, %msg )
{
   // echo("\c4 GameConnection::onConnectRequestRejected( " , %serverConn , " , " , %msg ,  " )" );
   switch$(%msg)
   {
      case "CR_INVALID_PROTOCOL_VERSION":
         %error = "Incompatible protocol version: Your game version is not compatible with this server.";
      case "CR_INVALID_CONNECT_PACKET":
         %error = "Internal Error: badly formed network packet";
      case "CR_YOUAREBANNED":
         %error = "You are not allowed to play on this server.";
      case "CR_SERVERFULL":
         %error = "This server is full.";
      case "CHR_PASSWORD":
         if ($Client::Password $= "")
            MessageBoxOK( "REJECTED", "That server requires a password.");
         else {
            $Client::Password = "";
            MessageBoxOK( "REJECTED", "That password is incorrect.");
         }
         return;
      case "CHR_PROTOCOL":
         %error = "Incompatible protocol version: Your game version is not compatible with this server.";
      case "CHR_CLASSCRC":
         %error = "Incompatible game classes: Your game version is not compatible with this server.";
      case "CHR_INVALID_CHALLENGE_PACKET":
         %error = "Internal Error: Invalid server response packet";
      default:
         %error = "Connection error.  Please try another server.  Error code: (" @ %msg @ ")";
   }
   disconnectedCleanup();
   MessageBoxOK( "REJECTED", %error);
}

function GameConnection::onConnectRequestTimedOut( %serverConn )
{
   // echo("\c4 GameConnection::onConnectRequestTimedOut( " , %serverConn , " )" );
   disconnectedCleanup();
   MessageBoxOK( "TIMED OUT", "Your connection to the server timed out." );
}


//-----------------------------------------------------------------------------
// Disconnect
//-----------------------------------------------------------------------------
function disconnect()
{
   // echo("\c4 disconnect( )" );
   // Delete the connection if it's still there.
   if (isObject(ServerConnection))
   {
      ServerConnection.delete();
   }

   disconnectedCleanup();

   destroyServer();
}

function disconnectedCleanup()
{
   // echo("\c4 disconnectedCleanup( )" );
   alxStopAll();

   LagIcon.setVisible(false);

   //PlayerListGui.clear();
   
   Canvas.setContent( MainMenuGui );

   clearTextureHolds();

   purgeResources();
}

