//-----------------------------------------------------------------------------
// Torque Engine
// 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

function DemoEditorAlert::onWake(%this)
{

   
   %text = "Welcome to the mission editor!\n\n";
   %text = %text @ "You can move objects by clicking on them and dragging with the mouse.\n\n";
   %text = %text @ "Right click and drag anywhere on the scene to change your view angle.\n\n";
   %text = %text @ "Use the w,s,a,d keys to fly around!\n\n";
   %text = %text @ "Hit F11 again to return to the demo.\n\n Have fun!";
   EditorAlertText.setText(%text);
   DemoEditorAlert.helpTag = DemoEditorAlert.helpTag + 1;
}