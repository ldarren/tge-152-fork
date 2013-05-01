
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// TRY FADE SPELL
//
//// Copyright (C) Faust Logic, Inc.
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
$spell_reload = isObject(TryAlphaFadeSpell);
if ($spell_reload)
{
  // mark datablocks so we can detect which are reloaded this script
  markDataBlocks();
  // reset data path from previously saved value
  %mySpellDataPath = TryAlphaFadeSpell.spellDataPath;
  TryAlphaFadeSpell.reset();
}
else
{
  // set data path from default plus containing folder name
  %mySpellDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

datablock afxScriptEventData(TAF_FadeOutScript_CE)
{
  methodName = "FadeOutCaster";   // name of method in TryAlphaFadeSpell subclass
};
datablock afxEffectWrapperData(TAF_FadeOutScript_EW)
{
  effect = TAF_FadeOutScript_CE;
  constraint = "impactedObject";
  delay = 0;
};

datablock afxScriptEventData(TAF_FadeInScript_CE)
{
  methodName = "FadeInCaster";
};
datablock afxEffectWrapperData(TAF_FadeInScript_EW)
{
  effect = TAF_FadeInScript_CE;
  constraint = "impactedObject";
  delay = 1;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// TRY ALPHA FADE SPELL
//

datablock afxMagicSpellData(TryAlphaFadeSpell)
{
  castingDur = 0;
  lingerDur = 2;
  numLingerLoops = 5;

  addLingerEffect = TAF_FadeOutScript_EW;
  addLingerEffect = TAF_FadeInScript_EW;
};
//
datablock afxRPGMagicSpellData(TryAlphaFadeSpell_RPG)
{
  name = "Try Alpha Fade";
  desc = "The key to a STEALTHY lifestyle." @
         "For testing player fading in and out.\n\n"; 
  sourcePack = "Core Tech 1.1";
  //iconBitmap = %mySpellDataPath @ "/AP/icons/ap";
  target = "self";
  manaCost = 10;
  reagentCost = 0;
  castingDur = TryAlphaFadeSpell.castingDur;
};

// script methods


function TryAlphaFadeSpell::FadeOutCaster(%this, %spell, %target, %constraint, %xfm, %data)
{
  if (isObject(%target))
    %target.startFade(1000, 0, true);
}

function TryAlphaFadeSpell::FadeInCaster(%this, %spell, %target, %constraint, %xfm, %data)
{
  if (isObject(%target))
    %target.startFade(1000, 0, false);
}

function TryAlphaFadeSpell::onActivate(%this, %spell, %caster)
{
  Parent::onActivate(%this, %spell, %caster);
  if (isObject(%caster))
    %spell.caster = %caster;
}

function TryAlphaFadeSpell::onInterrupt(%this, %spell, %caster)
{
  Parent::onInterrupt(%this, %spell, %caster);
  if (isObject(%caster))
    %caster.startFade(500, 0, false);
  else if (isObject(%spell.caster))
    %spell.caster.startFade(500, 0, false);
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
  TryAlphaFadeSpell.scriptFile = $afxAutoloadScriptFile;
  TryAlphaFadeSpell.spellDataPath = %mySpellDataPath;
  if (isFunction(addDemoSpellbookSpell))
  {
    addDemoSpellbookSpell(TryAlphaFadeSpell, TryAlphaFadeSpell_RPG);
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//