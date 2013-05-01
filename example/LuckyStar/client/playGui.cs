//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PlayGui is the main TSControl through which the game is viewed.
//-----------------------------------------------------------------------------

function PlayGui::onWake(%this)
{
	// Turn off any shell sounds...
	// alxStop( ... );
	$enableDirectInput = "1";
	activateDirectInput();
	
	// Activate the game's action map
	moveMap.push();

	// show control panel, if any
	if (!ControlPanelGui.isAwake()) Canvas.pushDialog(ControlPanelGui, 10);
	// show lobby, in multiplayer mode
	if ($Server::ServerType $= "MultiPlayer" && !LobbyGui.isAwake()) Canvas.pushDialog(LobbyGui, 10);
	
	// initialize LuckyStarClient
	LuckyStarClient.gameState = -1;
	LuckyStarClient.updateScore(0);
	LuckyStarClient.changeState(LuckyStarClient.LobbyState, AudioLobby);
}

function PlayGui::onSleep(%this)
{
	// Pop the keymap
	moveMap.pop();
	//LuckyStarClient.stopSong(); // handled in common
	LuckyStarClient.changeState(LuckyStarClient.LobbyState); // stop server side, purposely missed out audioprofile
	
	if (isObject(ControlPanelGui) && ControlPanelGui.isAwake()) Canvas.popDialog(ControlPanelGui);
	if (isObject(TestingToolGui) && TestingToolGui.isAwake()) Canvas.popDialog(TestingToolGui);
}
