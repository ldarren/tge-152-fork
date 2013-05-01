
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// Try Physical Zone
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
$spell_reload = isObject(TryPhysicalZoneSpell);
if ($spell_reload)
{
  // mark datablocks so we can detect which are reloaded this script
  markDataBlocks();
  // reset data path from previously saved value
  %mySpellDataPath = TryPhysicalZoneSpell.spellDataPath;
  TryPhysicalZoneSpell.reset();
}
else
{
  // set data path from default plus containing folder name
  %mySpellDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

datablock afxXM_WorldOffsetData(PHZ_offset3_XM)
{
  worldOffset = "0.0 0.0 -4.0";
};

datablock afxPhysicalZoneData(PHYZ_PhysZone2_CE)
{
  velocityMod = 1;
  gravityMod = 1;
  appliedForce = "0 40000 0";
  forceType = "spherical";
  polyhedron = "-8 8 0 16 0 0 0 -16 0 0 0 12";
};
//
datablock afxEffectWrapperData(PHYZ_PhysZone2_EW)
{
  effect = PHYZ_PhysZone2_CE;
  posConstraint = "#scene.CampFire";
  xfmModifiers[0] = PHZ_offset3_XM;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// spell

datablock afxMagicSpellData(TryPhysicalZoneSpell)
{
  lingerDur = 1;
  allowMovementInterrupts = false;

  addLingerEffect = PHYZ_PhysZone2_EW;
};
//
datablock afxRPGMagicSpellData(TryPhysicalZoneSpell_RPG)
{
  name = "Try Physical Zone";
  desc = "This experimental spell briefly creates a POWERFUL radial force centered on the campfire. All orcs " @
         "in the vicinity will be TOSSED.\n\n" @
         "[experimental effect]"; 
  sourcePack = "Core Tech 1.1";
  target = "nothing";
  manaCost = 10;
  castingDur = TryPhysicalZoneSpell.castingDur;
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
  TryPhysicalZoneSpell.scriptFile = $afxAutoloadScriptFile;
  TryPhysicalZoneSpell.spellDataPath = %mySpellDataPath;
  if (isFunction(addDemoSpellbookSpell))
    addDemoSpellbookSpell(TryPhysicalZoneSpell, TryPhysicalZoneSpell_RPG);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

