
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// TRY MACHINEGUN SPELL
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
$spell_reload = isObject(TryMachineGunSpell);
if ($spell_reload)
{
  // mark datablocks so we can detect which are reloaded this script
  markDataBlocks();
  // reset data path from previously saved value
  %mySpellDataPath = TryMachineGunSpell.spellDataPath;
  TryMachineGunSpell.reset();
}
else
{
  // set data path from default plus containing folder name
  %mySpellDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

datablock afxMachineGunData(TMG_MachineGun_CE)
{
  projectile = CrossbowProjectile;
  roundsPerMinute = 250;
};
datablock afxEffectWrapperData(TMG_MachineGun_EW)
{
  effect = TMG_MachineGun_CE;
  posConstraint = "caster.Bip01 Head";
  posConstraint2 = "target.#center";
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// spell

datablock afxMagicSpellData(TryMachineGunSpell)
{
  allowMovementInterrupts = false;
  castingDur = 3.0;

  addCastingEffect = TMG_MachineGun_EW;
};
//
datablock afxRPGMagicSpellData(TryMachineGunSpell_RPG)
{
  name = "Try MachineGun";
  desc = "Shoot a machinegun at a target, " @ 
         "(lousy aim since it does not lead the target)." @
         "\n\n[experimental effect]"; 
  sourcePack = "Core Tech";
  target = "enemy";
  manaCost = 0;
  castingDur = TryMachineGunSpell.castingDur;
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
  TryMachineGunSpell.scriptFile = $afxAutoloadScriptFile;
  TryMachineGunSpell.spellDataPath = %mySpellDataPath;
  if (isFunction(addDemoSpellbookSpell))
    addDemoSpellbookSpell(TryMachineGunSpell, TryMachineGunSpell_RPG);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//


