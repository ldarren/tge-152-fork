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
	
	if (alxIsPlaying($musicHandle))
	{
		alxStop($musicHandle);
	}
	if (!alxIsPlaying($GameMusicHandle))
	{
		$GameMusicHandle = alxPlay(AudioPlayGui);
	}
   
   // hack city - these controls are floating around and need to be clamped
   schedule(0, 0, "refreshCenterTextCtrl");
   schedule(0, 0, "refreshBottomTextCtrl");
}

function PlayGui::onSleep(%this)
{
   Canvas.popDialog( MainChatHud  );
   
   // pop the keymaps
   moveMap.pop();
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

function clientCmdSetWeaponGUI(%data)
{
    if(%data $= "")
      return;
      
    for(%i = 0; %i < $arrWeapGUI.count; %i++)
    {
        $arrWeapGUI.id[%i].visible = 0;
    }
      
    %guiname = "gui"@%data;
	%guiname.visible = 1;
}
