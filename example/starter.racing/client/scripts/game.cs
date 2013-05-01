//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Game start / end events sent from the server
//----------------------------------------------------------------------------

function clientCmdSetCounter(%count)
{
	if(%count == 3)
		counter.setVisible(true);

	counter.setBitmap("starter.racing/client/ui/" @ %count @ ".png");

	alxPlay(AudioCountBeep);
}

function clientCmdGameStart(%seq)
{
	// Display the GO bitmap and play a sound
	counter.setBitmap("starter.racing/client/ui/go.png");
	alxPlay(AudioGoBeep);
	// Remove the GO after a second.
	counter.schedule(1000, setVisible, false);
}

function clientCmdGameEnd(%seq)
{
   // Stop local activity... the game will be destroyed on the server
   alxStopAll();

   // Copy the current scores from the player list into the
   // end game gui (bit of a hack for now).
   EndGameGuiList.clear();
   for (%i = 0; %i < PlayerListGuiList.rowCount(); %i++) {
      %text = PlayerListGuiList.getRowText(%i);
      %id = PlayerListGuiList.getRowId(%i);
      EndGameGuiList.addRow(%id,%text);
   }
   EndGameGuiList.sortNumerical(1,false);

   // Display the end-game screen
   Canvas.setContent(EndGameGui);
}
