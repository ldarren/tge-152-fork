
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// CANTRIP AND FALL SPELL
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
$spell_reload = isObject(CantripAndFallSpell);
if ($spell_reload)
{
  // mark datablocks so we can detect which are reloaded this script
  markDataBlocks();
  // reset data path from previously saved value
  %mySpellDataPath = CantripAndFallSpell.spellDataPath;
  CantripAndFallSpell.reset();
}
else
{
  // set data path from default plus containing folder name
  %mySpellDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SPELLCASTER ANIMATION

//
// To initiate this spell, the caster kicks, as if tripping his
// target from afar.
//

datablock afxAnimClipData(CTaF_Casting_Clip_CE)
{
  clipName = "ctaf";
  ignoreCorpse = true;
  rate = 1.0;
};
//
datablock afxEffectWrapperData(CTaF_Casting_Clip_EW)
{
  effect = CTaF_Casting_Clip_CE;
  constraint = "caster";
  lifetime = 35/30;
  delay = 0.0;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CASTING DUST

//
// Concurrent with the spellcaster's kick animation, dust is kicked-
// up from the caster's right foot.  This is done using an emitter
// constrained to the foot.
//

// dark dust
datablock ParticleData(CTaF_Dust1_P)
{
  textureName          = %mySpellDataPath @ "/CTaF/particles/smoke";
  dragCoeffiecient     = 0.5;
  gravityCoefficient   = 0.2;
  inheritedVelFactor   = 0.00;
  lifetimeMS           = 700;
  lifetimeVarianceMS   = 300;
  useInvAlpha          = true;
  spinRandomMin        = -60.0;
  spinRandomMax        = 60.0;
  colors[0]            = "0.4 0.33 0.2 0.0";  
  colors[1]            = "0.4 0.33 0.2 0.15";
  colors[2]            = "0.4 0.33 0.2 0.05";
  colors[3]            = "0.4 0.33 0.2 0.0";
  sizes[0]             = 0.7; 
  sizes[1]             = 1.6;
  sizes[2]             = 1.0;
  sizes[3]             = 0.3;
  times[0]             = 0.0;
  times[1]             = 0.2;
  times[2]             = 0.7;
  times[3]             = 1.0;   
};
// light dust
datablock ParticleData(CTaF_Dust2_P : CTaF_Dust1_P)
{
  colors[0]            = "0.66 0.55 0.33 0.0";   
  colors[1]            = "0.66 0.55 0.33 0.15";
  colors[2]            = "0.66 0.55 0.33 0.05";
  colors[3]            = "0.66 0.55 0.33 0.0";
};

// foot dust emitter (standard Torque "sprinkler" emitter)
datablock ParticleEmitterData(CTaF_footDust_E)
{
  ejectionPeriodMS      = 30;
  periodVarianceMS      = 7;
  ejectionVelocity      = 0.3;//1.0;
  velocityVariance      = 0.1;//0.3;  
  particles             = "CTaF_Dust1_P CTaF_Dust2_P";

  // TGE emitterType = "sprinkler";
};
// 
datablock afxEffectWrapperData(CTaF_FootDust_EW)
{
  effect = CTaF_footDust_E;
  constraint = "caster.Bip01 R Foot";
  lifetime = 0.3;
  delay = 0.75;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// BANANA PEEL

//
// The target trips on a magic banana peel that is shoots through
// the air.  The orc's fall animation is in a backwards direction,
// so the peel shoots forward.  No attempt is made to line up the
// peel with a specific foot, as this would probably be impossible.
//

// banana peel offset, just a bit in front of the orc
datablock afxXM_LocalOffsetData(CTaF_Banana_offset_XM)
{
  localOffset = "0 1 0";
};

// banana peel
datablock afxModelData(CTaF_Banana_CE)
{
  shapeFile = %mySpellDataPath @ "/CTaF/models/CTaF_banana.dts";
  sequence = "slip";
};
//
datablock afxEffectWrapperData(CTaF_Banana_EW)
{
  effect = CTaF_Banana_CE;
  constraint = "impactedObject";
  delay = 0.2;
  fadeInTime  = 0.1;
  fadeOutTime = 0.1;
  lifetime = (40/30)-0.1;
  xfmModifiers[0] = CTaF_Banana_offset_XM;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// TARGET ANIMATION

//
//
//

datablock afxAnimClipData(CTaF_TripFall_CE)
{
  clipName = "ctaf_dn";
  ignoreCorpse = true;
  rate = 1.0;
};
//
datablock afxEffectWrapperData(CTaF_TripFall_EW)
{
  effect = CTaF_TripFall_CE;
  constraint = "impactedObject";
  lifetime = 2.3;
  lifeConstraint = "impactedObject";
  lifeConditions = $AFX::ALIVE;
};

datablock afxAnimClipData(CTaF_GetUp_CE)
{
  clipName = "ctaf_up";
  ignoreCorpse = true;
  rate = -1;
  transitionTime = 0.5;
};
//
datablock afxEffectWrapperData(CTaF_GetUp_EW)
{
  effect = CTaF_GetUp_CE;
  constraint = "impactedObject";
  lifetime = 2.2;
  delay = 2.2;
  lifeConstraint = "impactedObject";
  lifeConditions = $AFX::ALIVE;
};

datablock afxAnimLockData(CTaF_AnimLock_CE)
{
  priority = 0;
};
//
datablock afxEffectWrapperData(CTaF_AnimLock_EW)
{
  effect = CTaF_AnimLock_CE;
  constraint = "impactedObject";
  lifetime = 4.5;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// TARGET DUST

//
// When the target falls to the ground, dust is forced-up beneath
// him.  To approximate the area the falling orc strikes a disc
// emitter is used.  The particle types are reused from the casting
// dust.
//

datablock afxParticleEmitterDiscData(CTaF_fallDust_E) // TGEA
{
  ejectionPeriodMS      = 15;
  periodVarianceMS      = 4;
  ejectionVelocity      = 0.3;
  velocityVariance      = 0.1;
  particles             = "CTaF_Dust1_P CTaF_Dust2_P";

  // TGE emitterType = "disc";
  vector = "0 0 1";
  radiusMin = 1.0;
  radiusMax = 2.5;
};
// 
datablock afxEffectWrapperData(CTaF_FallDust_EW)
{
  effect = CTaF_fallDust_E;
  constraint = "impactedObject";
  lifetime = 0.6;
  delay = 0.7;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// sounds

datablock AudioProfile(CTaF_KickSnd_CE)
{
   fileName = %mySpellDataPath @ "/CTaF/sounds/CTAF_cast.wav";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(CTaF_KickSnd_EW)
{
  effect = CTaF_KickSnd_CE;
  constraint = "caster.Bip01 R Foot";
  delay = 0.75;
  lifetime = 0.4;
  scaleFactor = 0.2;
};

datablock AudioProfile(CTaF_GetupSnd_CE)
{
   fileName = %mySpellDataPath @ "/CTaF/sounds/CTAF_getup.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(CTaF_GetupSnd_EW)
{
  effect = CTaF_GetupSnd_CE;
  constraint = "impactedObject";
  delay = 3.6;
  lifetime = 1.154;
};

datablock AudioProfile(CTaF_RolloverSnd_CE)
{
   fileName = %mySpellDataPath @ "/CTaF/sounds/CTAF_rollover.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(CTaF_RolloverSnd_EW)
{
  effect = CTaF_RolloverSnd_CE;
  constraint = "impactedObject";
  delay = 2.25;
  lifetime = 0.841;
};

datablock AudioProfile(CTaF_TripSnd_CE)
{
   fileName = %mySpellDataPath @ "/CTaF/sounds/CTAF_trip.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(CTaF_TripSnd_EW)
{
  effect = CTaF_TripSnd_CE;
  constraint = "impactedObject";
  delay = 0.1;
  lifetime = 1.366;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CANTRIP AND FALL SPELL
//

datablock afxMagicSpellData(CantripAndFallSpell)
{
  castingDur = 0.85;
  directDamage = 1.0;

    // spellcaster animation //
  addCastingEffect = CTaF_Casting_Clip_EW;
    // casting dust //
  addCastingEffect = CTaF_FootDust_EW;
    // kick sound //
  addCastingEffect = CTaF_KickSnd_EW;

    // banana peel //
  addImpactEffect = CTaF_Banana_EW;
    // target animation //
  addImpactEffect = CTaF_TripFall_EW;
  addImpactEffect = CTaF_GetUp_EW;
  addImpactEffect = CTaF_AnimLock_EW;
    // target dust //
  addImpactEffect = CTaF_FallDust_EW;
    // sounds //
  addImpactEffect = CTaF_TripSnd_EW;
  addImpactEffect = CTaF_RolloverSnd_EW;
  addImpactEffect = CTaF_GetupSnd_EW;
};
//
datablock afxRPGMagicSpellData(CantripAndFallSpell_RPG)
{
  name = "Cantrip and Fall";
  desc = "An old mage school prank. Take a running orc, " @ 
         "add one teleported banana peel, and presto!" @
         "\n" @
         "\nspell design: Jeff Faust, Matthew Durante" @
         "\nsound effects: Dave Schroeder" @ 
         "\nspell concept: Jeff Faust";
  sourcePack = "Spell Pack 1";      
  iconBitmap = %mySpellDataPath @ "/CTaF/icons/ctaf";
  target = "enemy";
  range = 50;
  manaCost = 10;
  directDamage = 1.0;
  castingDur = CantripAndFallSpell.castingDur;
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
  CantripAndFallSpell.scriptFile = $afxAutoloadScriptFile;
  CantripAndFallSpell.spellDataPath = %mySpellDataPath;
  if (isFunction(addDemoSpellbookSpell))
    addDemoSpellbookSpell(CantripAndFallSpell, CantripAndFallSpell_RPG);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
