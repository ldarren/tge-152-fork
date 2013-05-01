
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// MARK OF GG SPELL
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
$spell_reload = isObject(MarkOfGGSpell);
if ($spell_reload)
{
  // mark datablocks so we can detect which are reloaded this script
  markDataBlocks();
  // reset data path from previously saved value
  %mySpellDataPath = MarkOfGGSpell.spellDataPath;
  MarkOfGGSpell.reset();
}
else
{
  // set data path from default plus containing folder name
  %mySpellDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

datablock afxAnimClipData(MGG_SummonClip_CE)
{
  clipName = "summon";
  rate = 3.0;
};
//
datablock afxEffectWrapperData(MGG_SummonClip_EW)
{
  effect = MGG_SummonClip_CE;
  constraint = "caster";
  lifetime = 1.0;
};

datablock afxZodiacData(MGG_LogoZodiac_CE)
{
  texture = %mySpellDataPath @ "/MGG//zodiacs/mgg_zode";
  radius = 2.5;
  startAngle = 0;
  rotationRate = 0.0;
  color = "1.0 1.0 1.0 0.6";
};

datablock afxEffectWrapperData(MGG_LogoZodiac_EW)
{
  effect = MGG_LogoZodiac_CE;
  constraint = caster;
  lifetime = 0;
  residueLifetime = 10;
  fadeOutTime = 5;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// spell

datablock afxMagicSpellData(MarkOfGGSpell)
{
  castingDur = 0.25;
  allowMovementInterrupts = false;
  //
  addCastingEffect = MGG_LogoZodiac_EW;
  addCastingEffect = MGG_SummonClip_EW;
};
//
datablock afxRPGMagicSpellData(MarkOfGGSpell_RPG)
{
  name = "Mark of GG";
  desc = "Tag the ground with the GarageGames logo." @
          "\n\n[novelty spell]";
  sourcePack = "Core Tech";
  iconBitmap = %mySpellDataPath @ "/MGG/icons/mgg";
  target = "nothing";
  manaCost = 0;
  castingDur = MarkOfGGSpell.castingDur;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

if ($spell_reload)
{
  // Removes then adds all reloaded datablocks
  touchDataBlocks();
}
else
{
  // save script filename and data path for reloads
  MarkOfGGSpell.scriptFile = $afxAutoloadScriptFile;
  MarkOfGGSpell.spellDataPath = %mySpellDataPath;
  if (isFunction(addDemoSpellbookSpell))
    addDemoSpellbookSpell(MarkOfGGSpell, MarkOfGGSpell_RPG);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

