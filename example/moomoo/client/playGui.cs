//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PlayGui is the main TSControl through which the game is viewed.
//-----------------------------------------------------------------------------

function PlayGui::onWake(%this)
{
   // Turn off an y shell sounds...
   // alxStop( ... );
   $enableDirectInput = "1";
   activateDirectInput();

   // Activate the game's action map
   moveMap.push();
	
	if(!alxIsPlaying($bgm_handle))
		$bgm_handle = alxPlay(InGameMusic);		// Play in game music
	if(!alxIsPlaying($amb_handle))
		$amb_handle = alxPlay(InGameSFX);			// Play Sound FX (ambience)	
}

function PlayGui::onSleep(%this)
{
   // Pop the keymap
   moveMap.pop();
}

function PlayGui::update(%this,%clientId,%name,%score)
{
   // Update or add the player to the control
   if (score.getRowNumById(%clientId) == -1)
      score.addRow(%clientId, %text);
   else
      score.setRowById(%clientId, %text);

   // Sorts by score
   score.sortNumerical(1,false);
   score.clearSelection();
}

function PlayGui::updateScore(%this,%clientId,%score)
{
   %text = score.getRowTextById(%clientId);
   %text = setField(%text,1,%score);
   score.setRowById(%clientId, %text);
   score.sortNumerical(1,false);
   score.clearSelection();
}
/*
function PlayGui::remove(%this,%clientId)
{
   score.removeRowById(%clientId);
}

function PlayGui::toggle(%this)
{
   if (%this.isAwake())
      Canvas.popDialog(%this);
   else
      Canvas.pushDialog(%this);
}
*/
function PlayGui::clear(%this)
{
   // Override to clear the list.
  score.clear();
}

function PlayGui::zeroScores(%this)
{
   for (%i = 0; %i <  score.rowCount(); %i++) {
      %text =  score.getRowText(%i);
      %text = setField(%text,1,"0");
       score.setRowById( score.getRowId(%i), %text);
   }
    score.clearSelection();
}