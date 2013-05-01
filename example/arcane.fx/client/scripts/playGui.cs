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
  //FL// Canvas.pushDialog( MainChatHud );
  chatHud.attach(HudMessageVector);
  
  // just update the action map here
  moveMap.push();
    
  // hack city - these controls are floating around and need to be clamped
  schedule(0, 0, "refreshCenterTextCtrl");
  schedule(0, 0, "cursorOn");
}

function PlayGui::onSleep(%this)
{
   //FL//Canvas.popDialog( MainChatHud  );
   
   // pop the keymaps
   moveMap.pop();
}

function PlayGui::onMouseWheelDown(%this)
{   
  commandToServer('dollyThirdPersonCam', false);
}

function PlayGui::onMouseWheelUp(%this)
{   
  commandToServer('dollyThirdPersonCam', true);
}

function PlayGui::selectFromPreSelected()
{
  ServerConnection.setSelectedObjFromPreSelected();
}

function PlayGui::onSpellbookChange(%this, %spellbook)
{
  SpellBank.onSpellbookChange(%spellbook);
}

function toggleFirstPerson(%val)
{
   if (%val)
   {
      $firstPerson = !$firstPerson;
   }
}

//-----------------------------------------------------------------------------

function refreshCenterTextCtrl()
{
   CenteredMessageText.position = "0 0";
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// InfoBox

function InfoBox::onWake(%this)
{
  ///echo("**** InfoBox WAKE **** " @ %this.getExtent());
}

function InfoBox::onResize(%this, %width, %height)
{
  %txt_margin = 8;

  %this_pos = %this.getPosition(); 
  %pos_x = getWord(%this_pos,0); 
  %pos_y = getWord(%this_pos,1);

  %win_size = Canvas.getExtent();
  %win_wd = getWord(%win_size,0);
  %win_ht = getWord(%win_size,1);

  %box_wd = %width + 2*%txt_margin;
  %box_ht = %height + 2*%txt_margin;
  %box_x = %win_wd - (%box_wd + 8);
  %box_y = %win_ht - (%box_ht + 60 + 8);

  InfoBoxBackdrop.resize(%box_x, %box_y, %box_wd, %box_ht);
  %this.resize(%box_x + %txt_margin, %box_y + %txt_margin, %width, %height);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxSpellButton

function afxSpellButton::onMouseEnter(%this)
{
  InfoBoxBackdrop.visible = true;
  InfoBox.visible = true;
  InfoBox.setText(%this.getSpellDescription());
}

function afxSpellButton::onMouseLeave(%this)
{
  InfoBoxBackdrop.visible = false;
  InfoBox.visible = false;
}

function afxSpellButton::onRightClick(%this)
{
  if (!%this.isActive())
    return;

  if (ServerConnection.getAddress() !$= "local")
  {
    clientCmdDisplayScreenMessage("Script reloading only works on a local client.");
    return;
  }

  %spell_data = %this.getSpellDataBlock();

  if (%spell_data == -1)
  {
    clientCmdDisplayScreenMessage("Reload error: failed to find spell data.");
    return;
  }

  if (%spell_data.scriptFile $= "")
  {
    clientCmdDisplayScreenMessage("Reload error: no script file specified.");
    return;
  }

  if (!isFile(%spell_data.scriptFile))
  {
    clientCmdDisplayScreenMessage("Reload error: file" SPC %spell_data.scriptFile SPC "does not exist.");
    return;
  }

  %retval = exec(%spell_data.scriptFile);
  if (!%retval)
  {
    clientCmdDisplayScreenMessage("Reload error: exec() failed.");
    return;
  }

  if (wasSyntaxError())
  {
    clientCmdDisplayScreenMessage("Reload error: exec() syntax error.");
    return;
  }

  clientCmdDisplayScreenMessage("Reloaded:" SPC fileName(%spell_data.scriptFile));
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// SpellBank

function SpellBank::changeBanks(%this, %old_bank, %new_bank)
{
  switch (%old_bank)
  {
  case 2:
    commandToServer('DisableSciFiMode');
  }

  switch (%new_bank)
  {
  case 0:
    clientCmdDisplayScreenMessage("");
    clientCmdDisplayScreenMessage("");
    clientCmdDisplayScreenMessage("Primary Spellbank");
  case 1:
    clientCmdDisplayScreenMessage("");
    clientCmdDisplayScreenMessage("");
    clientCmdDisplayScreenMessage("Experimental Spellbank");
  case 2:
    clientCmdDisplayScreenMessage("");
    clientCmdDisplayScreenMessage("");
    clientCmdDisplayScreenMessage("SCI-FI Mode");
    commandToServer('EnableSciFiMode');
  case 3:
    clientCmdDisplayScreenMessage("");
    clientCmdDisplayScreenMessage("");
    clientCmdDisplayScreenMessage("Preview Spellbank");
  }
}

function SpellBank::onSpellbookChange(%this, %spellbook)
{
  %this.page = 0;
  %n_buttons = %this.getCount();
  for (%i = 2; %i < %n_buttons; %i++)
  {
    %button = %this.getObject(%i);
    if (isObject(%button))
      %button.onSpellbookChange(%spellbook, %this.page);
  }
}

function SpellBank::nextPage(%this)
{
  %was_page = %this.page;
  
  %loop = true;
  while (%loop)
  {
    %this.page = (%this.page + 1) % %this.num_pages;

    %n_buttons = %this.getCount();
    for (%i = 2; %i < %n_buttons; %i++)
    {
      %button = %this.getObject(%i);
      if (isObject(%button))
      {
        %button.onTurnPage(%this.page);
      }
    }

    if (%was_page == %this.page)
      break;

    for (%i = 2; %i < %n_buttons; %i++)
    {
      %button = %this.getObject(%i);
      if (isObject(%button) && %button.isVisible())
      {
        %loop = false;
        break;
      }
    }
  }

  if (%was_page != %this.page)
    %this.changeBanks(%was_page, %this.page);
}

function SpellBank::prevPage(%this)
{
  %was_page = %this.page;
  
  %loop = true;
  while (%loop)
  {
    %this.page = (%this.page == 0) ? %this.num_pages-1 : %this.page-1;

    %n_buttons = %this.getCount();
    for (%i = 2; %i < %n_buttons; %i++)
    {
      %button = %this.getObject(%i);
      if (isObject(%button))
      {
        %button.onTurnPage(%this.page);
      }
    }

    if (%was_page == %this.page)
      break;

    for (%i = 2; %i < %n_buttons; %i++)
    {
      %button = %this.getObject(%i);
      if (isObject(%button) && %button.isVisible())
      {
        %loop = false;
        break;
      }
    }
  }

  if (%was_page != %this.page)
    %this.changeBanks(%was_page, %this.page);
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// PlayerStatusbar

// clicking on the player's status bar selects the player
function PlayerStatusbar::onMouseDown(%this)
{
  //echo("SELECT SELF " @ ServerConnection.player);
  if (isObject(ServerConnection.player))
    ServerConnection.setSelectedObj(ServerConnection.player);
}

function TargetStatusbarLabel::setName(%this, %name)
{
  %formatting = "<color:FFD200><shadow:1:1><shadowcolor:000000><just:center><font:Arial:15>";
  %this.setText(%formatting @ %name);
}

function PlayerStatusbarLabel::setName(%this, %name)
{
  %formatting = "<color:FFD200><shadow:1:1><shadowcolor:000000><just:center><font:Arial:15>";
  %this.setText(%formatting @ %name);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

function spellButtonCastSpell(%book_slot)
{
  %sel_obj = ServerConnection.getSelectedObj();
  if (%sel_obj != -1)
    %sel_obj_ghost = ServerConnection.GetGhostIndex(%sel_obj);
  else
    %sel_obj_ghost = -1;
  commandToServer('CastSpellbookSpell', %book_slot, %sel_obj_ghost);
}

function clientCmdSetTargetStatusbarLabel(%label_name)
{
   TargetStatusbarLabel.setName(%label_name);
}

function clientCmdSetPlayerSpellBook(%spellbook_ghost)
{
  %spellbook = ServerConnection.ResolveGhost(%spellbook_ghost);
  ServerConnection.spellbook = %spellbook;
  PlayGui.setSpellBook(%spellbook);
  //echo("SPELLBOOK ON CLIENT " @ %spellbook_ghost @ " " @ %spellbook);
}

function clientCmdSetClientPlayer(%player_ghost)
{
  %player = ServerConnection.ResolveGhost(%player_ghost);
  ServerConnection.player = %player;
  PlayerStatusbarLabel.setName(%player.getShapeName());
  PlayerHealthStatusBar.setShape(%player);
  PlayerEnergyStatusBar.setShape(%player);
  //echo("PLAYER ON CLIENT " @ %player_ghost @ " " @ %player);
}

function clientCmdOpenWebPage(%title, %message, %page)
{
  MessageBoxYesNo(%title, %message, "gotoWebPage(\"" @ %page @ "\");", "");
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//