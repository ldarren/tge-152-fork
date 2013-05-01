//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PlayGui is the main TSControl through which the game is viewed.
// The PlayGui also contains the hud controls.
//-----------------------------------------------------------------------------

function PlayGui::onWake(%this)
{
   // Turn off any shell sounds...
   // alxStop( ... );

   $enableDirectInput = "1";
   activateDirectInput();

   // Message hud dialog
   Canvas.pushDialog( MainChatHud );
   chatHud.attach(HudMessageVector);

   // just update the action map here
   moveMap.push();
   racingMap.push();
   
   // hack city - these controls are floating around and need to be clamped
   schedule(0, 0, "refreshCenterTextCtrl");
   schedule(0, 0, "refreshBottomTextCtrl");
}

function PlayGui::onSleep(%this)
{
   Canvas.popDialog( MainChatHud  );
   
   // pop the keymaps
   moveMap.pop();
   racingMap.pop();
}

//-----------------------------------------------------------------------------
function PlayGui::updateLapCounter(%this)
{
	LapCounter.setText("Lap" SPC %this.lap SPC "/" SPC %this.maxLaps);
}

function clientCmdSetMaxLaps(%laps)
{
	// Reset the current lap to 1 and set the max laps.
	PlayGui.lap = 1;
	PlayGui.maxLaps = %laps;
	PlayGui.updateLapCounter();
}

function clientCmdIncreaseLapCounter()
{
	// Increase the lap.
	PlayGui.lap++;
	PlayGui.updateLapCounter();
}

//-----------------------------------------------------------------------------

function refreshBottomTextCtrl()
{
   BottomPrintText.position = "0 0";
}

function refreshCenterTextCtrl()
{
   CenterPrintText.position = "0 0";
}


