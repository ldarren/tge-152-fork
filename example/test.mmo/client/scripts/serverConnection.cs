//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Functions dealing with connecting to a server


//-----------------------------------------------------------------------------
// Server connection error
//-----------------------------------------------------------------------------

addMessageCallback( 'MsgConnectionError', handleConnectionErrorMessage );

function handleConnectionErrorMessage(%msgType, %msgString, %msgError)
{
   // On connect the server transmits a message to display if there
   // are any problems with the connection.  Most connection errors
   // are game version differences, so hopefully the server message
   // will tell us where to get the latest version of the game.
   $ServerConnectionErrorMessage = %msgError;
}


//----------------------------------------------------------------------------
// GameConnection client callbacks
//----------------------------------------------------------------------------

function GameConnection::initialControlSet(%this)
{
	echo ("*** Initial Control Object");
	
	// The first control object has been set by the server
	// and we are now ready to go.
	
	// first check if the editor is active
	if (!Editor::checkActiveLoadDone())
	{
		// Activate if not already set...
		//if (Canvas.getContent() != PlayerSelection.getId())
			//Canvas.setContent(PlayerSelection);
		
		// Activate if not already set...
		if (Canvas.getContent() != $Client::GameGUI.getId())
			Canvas.setContent($Client::GameGUI);
	}
}

function GameConnection::setLagIcon(%this, %state)
{
   if (%this.getAddress() $= "local")
      return;
   LagIcon.setVisible(%state $= "true");
}

function GameConnection::onConnectionAccepted(%this)
{
   // Called on the new connection object after connect() succeeds.
   LagIcon.setVisible(false);
}

function GameConnection::onConnectionTimedOut(%this)
{
   // Called when an established connection times out
   disconnectedCleanup();
   MessageBoxOK( "TIMED OUT", "The server connection has timed out.");
}

function GameConnection::onConnectionDropped(%this, %msg)
{
   // Established connection was dropped by the server
   disconnectedCleanup();
   MessageBoxOK( "DISCONNECT", "The server has dropped the connection: " @ %msg);
}

function GameConnection::onConnectionError(%this, %msg)
{
   // General connection error, usually raised by ghosted objects
   // initialization problems, such as missing files.  We'll display
   // the server's connection error message.
   disconnectedCleanup();
   MessageBoxOK( "DISCONNECT", $ServerConnectionErrorMessage @ " (" @ %msg @ ")" );
}


//----------------------------------------------------------------------------
// Connection Failed Events
//----------------------------------------------------------------------------

function GameConnection::onConnectRequestRejected( %this, %msg )
{
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
         // XXX Should put up a password-entry dialog.
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

function GameConnection::onConnectRequestTimedOut(%this)
{
   disconnectedCleanup();
   MessageBoxOK( "TIMED OUT", "Your connection to the server timed out." );
}


//-----------------------------------------------------------------------------
// Disconnect
//-----------------------------------------------------------------------------

function disconnect()
{
   // Delete the connection if it's still there.
   if (isObject(ServerConnection))
      ServerConnection.delete();
   disconnectedCleanup();

   // Call destroyServer in case we're hosting
   destroyServer();
}

function disconnectedCleanup()
{
   // Clear misc script stuff
   //HudMessageVector.clear();

   // Terminate all playing sounds
   alxStopAll();
   if (isObject(MusicPlayer))
      MusicPlayer.stop();

   //
   LagIcon.setVisible(false);
   //PlayerListGui.clear();
   
   // Clear all print messages
   //clientCmdclearBottomPrint();
   //clientCmdClearCenterPrint();

   // Back to the launch screen
   if (Canvas.getContent() != MainMenuGui.getId())
      Canvas.setContent(MainMenuGui);

   // Dump anything we're not using
   clearTextureHolds();
   purgeResources();
}

//-----------------------------------------------------------------------------
// AFX - Object selection
//-----------------------------------------------------------------------------

// AFX OBJECT SELECTION CODE

$Current_Selectron_Style = 0;

function gotoNextSelectronStyle()
{
  commandToServer('NextSelectronStyle', $Current_Selectron_Style, false);
}

function gotoPreviousSelectronStyle()
{
  commandToServer('NextSelectronStyle', $Current_Selectron_Style, true);
}

function clientCmdUpdateSelectronStyle(%style_id)
{
  $Current_Selectron_Style = %style_id;

  ServerConnection.resetSelection();
}

function GameConnection::onObjectSelected(%this, %obj)
{
  if (%obj.selectron == 0 && %obj != %this.getControlObject())
  {
    %selectron = startSelectron(%obj, $Current_Selectron_Style);
    if (%selectron != 0)
      %selectron.addConstraint(%obj, "selected");
    %obj.selectron = %selectron;
  }  
}

function GameConnection::onObjectDeselected(%this, %obj)
{
  if (%obj.selectron != 0 &&isObject(%obj.selectron))
  {
    %obj.selectron.stopSelectron();
    %obj.selectron = 0;
  }
}

function GameConnection::resetSelection(%this)
{
  %sel_obj = %this.getSelectedObj();
  if (%sel_obj != -1)
     %this.setSelectedObj(%sel_obj);
}

function GameConnection::onObjectRollover(%this)
{
}

$mouseMarker = new ClientSideTSStatic()            
{               
	shapeName = "~/data/shapes/markers/octahedron.dts";
	position = "0 0 0";               
	rotate = "0 0 1 0";               
	scale = "1 1 1";            
	selectron = 0;
	hidden = true;
};
	
function GameConnection::onDestSet(%this, %dest)
{
	if ($mouseMarker.selectron != 0 &&isObject($mouseMarker.selectron))
	{
		$mouseMarker.selectron.stopSelectron();
		$mouseMarker.selectron = 0;
	}
	$pos = getWords(%dest,1,3);
	
	$mouseMarker.setTransform($pos);
	%selectron = startSelectron($mouseMarker, 20); // hardcoded in server/script/base/effects/user/selectron.cs
	if (%selectron != 0)
	%selectron.addConstraint($mouseMarker, "selected");
	$mouseMarker.selectron = %selectron;
}

// AFX