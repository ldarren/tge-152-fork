
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// TELEPORT TO FAUSTLOGIC.COM SPELL
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

$AFX_VERSION = (isFunction(afxGetVersion)) ? afxGetVersion() : 1.02;
$MIN_REQUIRED_VERSION = 1.1;

// Test version requirements for this script
if ($AFX_VERSION < $MIN_REQUIRED_VERSION)
{
  error("AFX script " @ fileName($afxAutoloadScriptFile) @ " is not compatible with AFX versions older than " @ $MIN_REQUIRED_VERSION @ ".");
  return;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

//
// Here we test if the script is being reloaded or if this is the
// first time the script has executed this mission.
//
$spell_reload = isObject(TeleportFLSpell);
if ($spell_reload)
{
  // mark datablocks so we can detect which are reloaded this script
  markDataBlocks();
  // reset data path from previously saved value
  %mySpellDataPath = TeleportFLSpell.spellDataPath;
  TeleportFLSpell.reset();
}
else
{
  // set data path from default plus containing folder name
  %mySpellDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

datablock afxAnimClipData(TFL_SummonClip_CE)
{
  clipName = "summon";
  rate = 2.0;
};
//
datablock afxEffectWrapperData(TFL_SummonClip_EW)
{
  effect = TFL_SummonClip_CE;
  constraint = "caster";
};

datablock afxZodiacData(TFL_Zode_CE)
{
  texture = %mySpellDataPath @ "/TFL/zodiacs/fl_logo_zode";
  radius = 2.5;
  startAngle = 0;
  rotationRate = 0.0;
  color = "1.0 0.3 0.0 0.8";
};
datablock afxEffectWrapperData(TFL_Zode_EW)
{
  effect = TFL_Zode_CE;
  constraint = caster;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// spell

datablock afxMagicSpellData(TeleportFLSpell)
{
  allowMovementInterrupts = false;
  castingDur = 1.0;

  addCastingEffect = TFL_SummonClip_EW;
  addCastingEffect = TFL_Zode_EW;
};
//
datablock afxRPGMagicSpellData(TeleportFLSpell_RPG)
{
  name = "Teleport to FaustLogic.com";
  desc = "Open browser to the Faust Logic homepage.\n\n[novelty spell]"; 
  sourcePack = "Spell Pack 1";
  iconBitmap = %mySpellDataPath @ "/TFL/icons/tfl";
  target = "nothing";
  manaCost = 0;
  castingDur = TeleportFLSpell.castingDur;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// scripting

function TeleportFLSpell::onImpact(%this, %spell, %caster, %impObj, %impPos, %impNorm)
{
  if (%caster.client)
    commandToClient(%caster.client, 'OpenWebPage', "Teleport to FaustLogic.com", 
                    "Visit Faust Logic website?", "http://www.faustlogic.com");
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

if ($spell_reload)
{
  // Removes then adds all reloaded datablocks
  touchDataBlocks();
}
else
{
  // save script filename and data path for reloads
  TeleportFLSpell.scriptFile = $afxAutoloadScriptFile;
  TeleportFLSpell.spellDataPath = %mySpellDataPath;
  if (isFunction(addDemoSpellbookSpell))
    addDemoSpellbookSpell(TeleportFLSpell, TeleportFLSpell_RPG);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//


