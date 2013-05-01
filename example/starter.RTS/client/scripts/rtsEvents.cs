
// Hook all the event callbacks...
addMessageCallback('MsgUnitAttacked',     handleUnitAttacked);
addMessageCallback('MsgBaseAttacked',     handleBaseAttacked);
addMessageCallback('MsgAllyAttacked',     handleAllyAttacked);
addMessageCallback('MsgResearchComplete', handleResearchComplete);
addMessageCallback('MsgUnitComplete',     handleUnitComplete);
addMessageCallback('MsgBuildingComplete', handleBuildingComplete);
addMessageCallback('MsgMapLocPing',       handleMapLocPing);
addMessageCallback('MsgPurchaseDenied',   handlePurchaseDenied);

// Attack Events
function handleUnitAttacked(%msgType, %msgString, %location)
{
   if(!$RTSCamera.isInCameraView(%location))
   {
      clientCmdBottomPrint("Your unit is under attack!", 2, 2);
      MapHud.createPingEvent(%location, "1 0 0");
      $LastEventPos = %location;
   }
}

function handleBaseAttacked(%msgType, %msgString, %location)
{
	 if(!$RTSCamera.isInCameraView(%location))
	 {
      clientCmdBottomPrint("Your base is under attack!", 2, 2);
      $LastEventPos = %location;
   }
}

function handleAllyAttacked(%msgType, %msgString, %allyName, %location)
{
	 if(!$RTSCamera.isInCameraView(%location))
	 {
      clientCmdBottomPrint(%allyName SPC "is under attack!", 2, 2);
      $LastEventPos = %location;
   }
}

// Completed Events
function handleResearchComplete(%msgType, %msgString, %location)
{
   clientCmdBottomPrint(%msgString SPC "research is completed.", 10, 2);
}

function handleUnitComplete(%msgType, %msgString, %location)
{
   clientCmdBottomPrint(%msgString SPC "training is completed.", 10, 2);
      MapHud.createPingEvent(%location, "1 0 0");
      $LastEventPos = %location;
}

function handleBuildingComplete(%msgType, %msgString, %location)
{
   clientCmdBottomPrint(%msgString SPC "construction is completed.", 10, 2);
}

// Misc. Events
function handleMapLocPing(%msgType, %msgString, %issueingClient, %location)
{
	echo("Do something here");
}

function handlePurchaseDenied(%msgType, %msgString, %reason)
{
  clientCmdBottomPrint(%msgString SPC %reason, 10, 2);
}
