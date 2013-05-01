
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// PHRASE TESTER SPELL
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
$spell_reload = isObject(PhraseTesterSpell);
if ($spell_reload)
{
  // mark datablocks so we can detect which are reloaded this script
  markDataBlocks();
  // reset data path from previously saved value
  %mySpellDataPath = PhraseTesterSpell.spellDataPath;
  PhraseTesterSpell.reset();
}
else
{
  // set data path from default plus containing folder name
  %mySpellDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

datablock afxZodiacData(PHT_casting_Zode_CE)
{
  texture = %mySpellDataPath @ "/PHT/zodiacs/crop3_zode";
  radius = 2.0;
  startAngle = 0.0;
  rotationRate = 360.0;
  color = "1.0 0.0 0.0 0.5";
};
datablock afxEffectWrapperData(PHT_casting_Zode_EW)
{
  effect = PHT_casting_Zode_CE;
  constraint = caster;
  lifetime = 1.0;
  fadeOutTime = 0.5;
};

// this offset defines the radius of the portal-zodes
// angle and rotation rate of the portal-beams.
datablock afxXM_SpinData(PHT_Spin_XM)
{
  spinAxis = "0 0 1";
  spinAngle = -15; // 0-15
  spinRate = -80;
};
datablock afxXM_LocalOffsetData(PHT_Offset_XM)
{
  localOffset = "0 2.2 0";
};

datablock afxZodiacData(PHT_launch_Zode_CE : PHT_casting_Zode_CE)
{
  rotationRate = -100.0;
  color = "0.0 1.0 1.0 0.5";
  radius = 0.4;
};
datablock afxEffectWrapperData(PHT_launch_Zode_EW)
{
  effect = PHT_launch_Zode_CE;
  constraint = caster;
  lifetime = 10.0;
  fadeOutTime = 1.0;
  xfmModifiers[0] = "PHT_Spin_XM";
  xfmModifiers[1] = "PHT_Offset_XM";
};

datablock afxZodiacData(PHT_delivery_Zode_CE : PHT_casting_Zode_CE)
{
  rotationRate = -100.0;
  color = "1.0 1.0 0.0 0.5";
};
datablock afxEffectWrapperData(PHT_delivery_Zode_EW)
{
  effect = PHT_delivery_Zode_CE;
  constraint = caster;
  fadeOutTime = 1.0;
};

datablock afxZodiacData(PHT_impact_Zode_CE : PHT_casting_Zode_CE)
{
  rotationRate = 360.0;
  color = "1.0 0.5 0.0 0.5";
  radius = 1.0;
};
datablock afxEffectWrapperData(PHT_impact_Zode_EW)
{
  effect = PHT_impact_Zode_CE;
  constraint = caster;
  lifetime = 1.0;
  fadeOutTime = 1.0;
};

datablock afxZodiacData(PHT_linger_Zode_CE : PHT_casting_Zode_CE)
{
  rotationRate = 360.0;
  color = "0.0 1.0 0.0 0.5";
};
datablock afxEffectWrapperData(PHT_linger_Zode_EW)
{
  effect = PHT_linger_Zode_CE;
  constraint = caster;
  fadeOutTime = 1.0;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// missile


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// spell

datablock afxMagicSpellData(PhraseTesterSpell)
{
  castingDur = 2.0;
  numCastinLoops = $AFX::INFINITE_REPEATS;

  deliveryDur = $AFX::INFINITE_TIME;
  lingerDur = $AFX::INFINITE_TIME;

  addCastingEffect = PHT_casting_Zode_EW;
  addLaunchEffect = PHT_launch_Zode_EW;
  addDeliveryEffect = PHT_delivery_Zode_EW;
  addImpactEffect = PHT_impact_Zode_EW;
  addLingerEffect = PHT_linger_Zode_EW;
};
//
datablock afxRPGMagicSpellData(PhraseTesterSpell_RPG)
{
  name = "Phrase Tester";
  desc = "For testing unusual phrase timings, " @
         "especially those requiring an external event to push " @
         "the spell into the next stage.\n\n" @
         "    push to next stage:  [semicolon]\n" @
         "    halt effect:  [ctrl-semicolon]" @
         "\n\n[testing effect]";
  iconBitmap = %mySpellDataPath @ "/PHT/icons/pht";
  sourcePack = "Core Tech";
  target = "self";
  castingDur = PhraseTesterSpell.castingDur;
};

function PhraseTesterSpell::onActivate(%this, %spell, %caster, %target)
{
  if (isObject(%caster.phrase_tester))
  {
    %caster.phrase_tester.restarting = true;
    %caster.phrase_tester.interrupt();
  }

  Parent::onActivate(%this, %spell, %caster, %target);
  %caster.phrase_tester = %spell;
}

function PhraseTesterSpell::onDeactivate(%this, %spell)
{
  if (!%spell.restarting)
  {
    %caster = %spell.getCaster();
    if (isObject(%caster))
      %caster.phrase_tester = "";
  }
}

function performPhraseTesterPush(%caster)
{
  if (isObject(%caster.phrase_tester))
  {
    echo("Push PhraseTester to next stage.");
    %caster.phrase_tester.interruptStage();
  }
}

function performPhraseTesterHalt(%caster)
{
  if (isObject(%caster.phrase_tester))
  {
    echo("Halt PhraseTester.");
    %caster.phrase_tester.interrupt();
  }
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
  PhraseTesterSpell.scriptFile = $afxAutoloadScriptFile;
  PhraseTesterSpell.spellDataPath = %mySpellDataPath;
  if (isFunction(addDemoSpellbookSpell))
    addDemoSpellbookSpell(PhraseTesterSpell, PhraseTesterSpell_RPG);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
