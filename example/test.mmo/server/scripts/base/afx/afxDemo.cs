
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// Functions for AFX Demo
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// autoloading of effects

function autoloadAFXModules()
{
  %pattern = "~/server/scripts/base/effects/*/*.cs";
  %i = 0;
  for (%file = findFirstFile(%pattern); %file !$= ""; %file = findNextFile(%pattern)) 
  {
    %path = filePath(%file);
    %tokens = %path;
    %folder = "";
    while (%tokens !$= "")
    {
      %tokens = nextToken(%tokens, "tokfolder", "/");
      if (%tokfolder !$= "")
        %folder = %tokfolder;
    }


    $afxAutoloadScriptFile = %file;
    $afxAutoloadScriptFolder = %folder;
    exec(%file);
    $afxAutoloadScriptFile = "";
    $afxAutoloadScriptFolder = "";

    %i++;
  }

  if (%i == 0)
    error("No AFX Modules Found.");
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// demo spellbook management

$Book_Max_Spells = 12*12;

function loadDemoSpellbook(%book)
{
  for (%i = 0; %i < $Book_Max_Spells; %i++)
  {
    if ($Book_Rpg[%i] !$= "")
    {
      %book.rpgSpells[%i] = $Book_Rpg[%i];
      %book.spells[%i] = $Book_Spells[%i];
    }
  }

  deleteVariables("$Book_Spells*");
  deleteVariables("$Book_Rpg*");
}

function addDemoSpellbookPlaceholder(%placeholder, %page, %slot)
{
  if (%placeholder.getClassName() !$= "afxRPGMagicSpellData")
    return;

  %placeholder.isPlaceholder = true;

  %idx = %page*12 + %slot;
  if (%idx >= 0 && %idx < $Book_Max_Spells)
  {
    $Book_Rpg[%idx] = %placeholder;
  }
}

function addDemoSpellbookSpell(%spell_db, %rpg_db)
{
  if (!isObject(%spell_db) || !isObject(%rpg_db))
    return;

  %spell_name = %rpg_db.name;
  if (%spell_name $= "")
    return;

  // find a placeholder with a matching name
  %idx = -1;
  %cap = 12*12;
  for (%i = 0; %i  < $Book_Max_Spells; %i++)
  {
    if ($Book_Rpg[%i] !$= "" && $Book_Rpg[%i].name $= %spell_name && $Book_Rpg[%i].isPlaceholder)
    {
      %idx = %i;
      break;
    }
  }

  // if placeholder was found, replace it
  if (%idx >= 0)
  {
    $Book_Spells[%idx] = %spell_db;
    $Book_Rpg[%idx] = %rpg_db;
    %spell_db.rpgSpellData = %rpg_db;
    %rpg_db.spellFXData = %spell_db;
    return;
  }

  // if no placeholder was found, use first empty slot
  // beginning with 4th spellbank.
  %idx = -1;
  for (%i = 3*12; %i  < $Book_Max_Spells; %i++)
  {
    if ($Book_Rpg[%i] $= "")
    {
      %idx = %i;
      break;
    }
  }

  // if empty slot was found, insert spell there
  if (%idx >= 0)
  {
    $Book_Spells[%idx] = %spell_db;
    $Book_Rpg[%idx] = %rpg_db;
    %spell_db.rpgSpellData = %rpg_db;
    %rpg_db.spellFXData = %spell_db;
    return;
  }

  error("Demo Spellbook is full. Cannot add spell, \"" @ %spell_name @ "\".");
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// demo selectron management

$Total_Sele_Styles = 0;
$Sele_Style_Names[0] = "";
$Sele_Style_Ids[0] = "";

function resetDemoSelectronStyles()
{
  $Total_Sele_Styles = 0;
}

function addDemoSelectronStyle(%sele_name, %sele_id)
{
  $Sele_Style_Names[$Total_Sele_Styles] = %sele_name;
  $Sele_Style_Ids[$Total_Sele_Styles] = %sele_id;
  $Total_Sele_Styles++;
}

function serverCmdNextSelectronStyle(%client, %current_style, %do_prev, %display_msg)
{
  %idx = -1;
  for (%i = 0; %i < $Total_Sele_Styles; %i++)
  {
    if (%current_style == $Sele_Style_Ids[%i])
    {
      %idx = %i;
      break;
    }
  }

  if (%idx == -1)
    return;

  if (%do_prev)
  {
    if (%idx == 0)
      %idx = $Total_Sele_Styles;
    %idx--;
  }
  else
  {
    %idx++;
    if (%idx >= $Total_Sele_Styles)
      %idx = 0;
  }

  commandToClient(%client, 'UpdateSelectronStyle', $Sele_Style_Names[%idx], $Sele_Style_Ids[%idx], %display_msg);
}

function serverCmdSetSelectronStyle(%client, %style_name, %display_msg)
{
  %idx = -1;
  for (%i = 0; %i < $Total_Sele_Styles; %i++)
  {
    if (%style_name $= $Sele_Style_Names[%i])
    {
      %idx = %i;
      break;
    }
  }

  if (%idx == -1)
  {
    if ($Total_Sele_Styles == 0)
      return;
    %idx = 0;
  }

  commandToClient(%client, 'UpdateSelectronStyle', $Sele_Style_Names[%idx], $Sele_Style_Ids[%idx], %display_msg);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

// demo sci-fi mode

function serverCmdEnableSciFiMode(%client)
{
  if (%client.player != 0)
  {
    echo("Enable Sci-Fi Mode");
    if (isFunction(equipSciFiOrc))
      equipSciFiOrc(%client.player);
    serverCmdSetSelectronStyle(%client, "SCI-FI", false);
  }
}

function serverCmdDisableSciFiMode(%client, %orc)
{
  if (%client.player != 0)
  {
    echo("Disable Sci-Fi Mode");
    if (isFunction(unequipSciFiOrc))
      unequipSciFiOrc(%client.player);
    serverCmdSetSelectronStyle(%client, "AFX Default", false);
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

function afxPickTeleportDest(%current_pos)
{
  %dist = VectorDist(%current_pos, "360 310 218");

  if (%dist < 100)
    return pickSpawnPoint("MissionGroup/FarTeleports");
  else
    return pickSpawnPoint("MissionGroup/NearTeleports");
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
