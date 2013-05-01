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
}

function PlayGui::onSleep(%this)
{
   // Pop the keymap
   moveMap.pop();
}
