
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// TRY DEBRIS BURST SPELL
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
$spell_reload = isObject(TryDebrisBurstSpell);
if ($spell_reload)
{
  // mark datablocks so we can detect which are reloaded this script
  markDataBlocks();
  // reset data path from previously saved value
  %mySpellDataPath = TryDebrisBurstSpell.spellDataPath;
  TryDebrisBurstSpell.reset();
}
else
{
  // set data path from default plus containing folder name
  %mySpellDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

// Debris object
datablock DebrisData(TBD_Debris_CE)
{
   shapeFile = "~/data/shapes/crossbow/debris.dts";
   emitters = "CrossbowDebrisTrailEmitter";
   explosion = CrossbowDebrisExplosion;
   
   elasticity = 0.6;
   friction = 0.5;
   numBounces = 2;
   bounceVariance = 2;
   explodeOnMaxBounce = true;
   staticOnMaxBounce = false;
   snapOnMaxBounce = false;
   minSpinSpeed = 0;
   maxSpinSpeed = 700;
   render2D = false;
   lifetime = 4;
   lifetimeVariance = 0.4;
   velocity = 4;
   velocityVariance = 0.5;
   fade = false;
   useRadiusMass = true;
   baseRadius = 0.3;
   gravModifier = 0.8;
   terminalVelocity = 6;
   ignoreWater = true;
};

datablock afxXM_RandomRotData(TBD_random_rot)
{
  axis = "0 0 1";
  thetaMin = 45;
  thetaMax = 45;
};

datablock afxEffectWrapperData(TDB_DebrisBurst_EW)
{
  effect = TBD_Debris_CE;
  posConstraint = "caster.Bip01 Head";
  xfmModifiers[0] = TBD_random_rot;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// spell

datablock afxMagicSpellData(TryDebrisBurstSpell)
{
  allowMovementInterrupts = false;
  castingDur = 0.0;

  addCastingEffect = TDB_DebrisBurst_EW;
  addCastingEffect = TDB_DebrisBurst_EW;
  addCastingEffect = TDB_DebrisBurst_EW;
  addCastingEffect = TDB_DebrisBurst_EW;
  addCastingEffect = TDB_DebrisBurst_EW;
  addCastingEffect = TDB_DebrisBurst_EW;
  addCastingEffect = TDB_DebrisBurst_EW;
  addCastingEffect = TDB_DebrisBurst_EW;
  addCastingEffect = TDB_DebrisBurst_EW;
  addCastingEffect = TDB_DebrisBurst_EW;
};
//
datablock afxRPGMagicSpellData(TryDebrisBurstSpell_RPG)
{
  name = "Try Debris Burst";
  desc = "Explode debris in random directions from spellcaster's head, " @ 
         "(works best if caster is stationary " @
         "since debris does not take on the caster's velocity)." @
         "\n\n[experimental spell]"; 
  sourcePack = "Spell Pack 1";
  target = "nothing";
  manaCost = 0;
  castingDur = TryDebrisBurstSpell.castingDur;
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
  TryDebrisBurstSpell.scriptFile = $afxAutoloadScriptFile;
  TryDebrisBurstSpell.spellDataPath = %mySpellDataPath;
  if (isFunction(addDemoSpellbookSpell))
    addDemoSpellbookSpell(TryDebrisBurstSpell, TryDebrisBurstSpell_RPG);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//


