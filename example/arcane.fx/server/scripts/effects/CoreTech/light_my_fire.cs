
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// LIGHT MY FIRE SPELL
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
$spell_reload = isObject(LightMyFireSpell);
if ($spell_reload)
{
  // mark datablocks so we can detect which are reloaded this script
  markDataBlocks();
  // reset data path from previously saved value
  %mySpellDataPath = LightMyFireSpell.spellDataPath;
  LightMyFireSpell.reset();
}
else
{
  // set data path from default plus containing folder name
  %mySpellDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

$isTGEA = (afxGetEngine() $= "TGEA");

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SPELLCASTER ANIMATION

//
// The spellcaster quickly raises his hands to cast this simple
// spell.
//

datablock afxAnimClipData(LMF_SummonClip_CE)
{
  clipName = "summon";
  rate = 2.0;
};
//
datablock afxEffectWrapperData(LMF_SummonClip_EW)
{
  effect = LMF_SummonClip_CE;
  constraint = "caster";
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CASTING HAND FIRE PARTICLES

//
// Fire shoots from the spellcaster's raised hands as he casts this
// spell.  The fire particle texture-map here is used throughout the
// effect for all fire particles.
//

// hand fire particle
datablock ParticleData(LMF_fireHand_P)
{
   // TGE textureName          = %mySpellDataPath @ "/LMF/particles/LMF_fireA";

   dragCoeffiecient     = 0.5;
   gravityCoefficient   = -0.4;

   lifetimeMS           = 250;
   lifetimeVarianceMS   = 50;
   useInvAlpha          = false;
   spinRandomMin        = -360.0;
   spinRandomMax        = 360.0;

   colors[0]            = "1.0 1.0 1.0 1.0";
   colors[1]            = "1.0 0.9 0.3 1.0";
   colors[2]            = "0.7 0.0 0.0 0.0";

   sizes[0]             = 0.5;
   sizes[1]             = 0.7;
   sizes[2]             = 1.0;

   times[0]             = 0.0;
   times[1]             = 0.3;
   times[2]             = 1.0;

   textureName          = %mySpellDataPath @ "/LMF/particles/lmf_tiled_parts"; // fireA
   textureCoords[0]     = "0.0 0.0";
   textureCoords[1]     = "0.0 0.5";
   textureCoords[2]     = "0.5 0.5";
   textureCoords[3]     = "0.5 0.0";
};

// hand fire emitter (standard Torque "sprinkler" emitter)
datablock ParticleEmitterData(LMF_fireHand_E)
{
  ejectionPeriodMS      = 30;
  periodVarianceMS      = 7;
  ejectionVelocity      = 1.0;
  velocityVariance      = 0.3;  
  particles             = "LMF_fireHand_P";

  // TGE emitterType = "sprinkler";
};

// hand fire, left hand
datablock afxEffectWrapperData(LMF_CastingFire_lf_hand_EW)
{
  effect = LMF_fireHand_E;
  constraint = "caster.Bip01 L Hand";
  lifetime = 0.4;
  delay = 0.1;
};
// hand fire, right hand
datablock afxEffectWrapperData(LMF_CastingFire_rt_hand_EW : LMF_CastingFire_lf_hand_EW)
{
  constraint = "caster.Bip01 R Hand";
  delay = 0.15;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CASTING HAND FIRE LIGHTS

//
// Lights constrained to the caster's hands flash on and off in 
// conjunction with the fire hand particles above.  Multiple lights
// are used, fading in and out using the fade parameters of 
// afxEffectWrapperData, to create this effect.
//

datablock afxXM_LocalOffsetData(LMF_CastingFireLight_offset1_XM)
{
  localOffset = "0.1 -0.3 0.0";
};
datablock afxXM_LocalOffsetData(LMF_CastingFireLight_offset2_XM)
{
  localOffset = "-0.2 0.1 0.2";
};
datablock afxXM_LocalOffsetData(LMF_CastingFireLight_offset3_XM)
{
  localOffset = "0.2 -0.1 -0.3";
};

if ($isTGEA)
{
  %LMF_CastingFireLightA_LMODELS_intensity = 3;
  %LMF_CastingFireLightA_LTERRAIN_intensity = 0.75;
  datablock afxLightData(LMF_FireShadowLight_A_LMODELS_CE)
  {
    type = "Point";  
    radius = 3;
    sgCastsShadows = true;
    sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Original Stock";
    color = 1.0*%LMF_CastingFireLightA_LMODELS_intensity SPC
            0.9*%LMF_CastingFireLightA_LMODELS_intensity SPC
            0.3*%LMF_CastingFireLightA_LMODELS_intensity;
    lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF; // TGEA (ignored by TGE)
  };
  datablock afxLightData(LMF_FireShadowLight_A_LTERRAIN_CE : LMF_FireShadowLight_A_LMODELS_CE)
  {
    radius = 4;
    color = 1.0*%LMF_CastingFireLightA_LTERRAIN_intensity SPC
            0.9*%LMF_CastingFireLightA_LTERRAIN_intensity SPC
            0.3*%LMF_CastingFireLightA_LTERRAIN_intensity;
    lightIlluminationMask = $AFX::ILLUM_TERRAIN | $AFX::ILLUM_ATLAS; // TGEA (ignored by TGE)
  };
  
  %LMF_CastingFireLightB_LMODELS_intensity = 3;
  %LMF_CastingFireLightB_TERRAIN_intensity = 0.75;
  datablock afxLightData(LMF_FireShadowLight_B_LMODELS_CE)
  {
    type = "Point";  
    radius = 2;
    sgCastsShadows = true;
    sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Original Stock";
    color = 1.0*%LMF_CastingFireLightB_LMODELS_intensity SPC
            0.5*%LMF_CastingFireLightB_LMODELS_intensity SPC
            0.1*%LMF_CastingFireLightB_LMODELS_intensity;
    lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF; // TGEA (ignored by TGE)
  };
  datablock afxLightData(LMF_FireShadowLight_B_LTERRAIN_CE : LMF_FireShadowLight_B_LMODELS_CE)
  {
    radius = 3;
    color = 1.0*%LMF_CastingFireLightB_TERRAIN_intensity SPC
            0.5*%LMF_CastingFireLightB_TERRAIN_intensity SPC
            0.1*%LMF_CastingFireLightB_TERRAIN_intensity;
    lightIlluminationMask = $AFX::ILLUM_TERRAIN | $AFX::ILLUM_ATLAS; // TGEA (ignored by TGE)
  };
  
  //////

  datablock afxMultiLightData(LMF_FireShadowLight_A_Multi_CE)
  {
    lights[0] = LMF_FireShadowLight_A_LMODELS_CE;
    lights[1] = LMF_FireShadowLight_A_LTERRAIN_CE;
  };
  datablock afxMultiLightData(LMF_FireShadowLight_B_Multi_CE)
  {
    lights[0] = LMF_FireShadowLight_B_LMODELS_CE;
    lights[1] = LMF_FireShadowLight_B_LTERRAIN_CE;
  }; 
  
  datablock afxEffectWrapperData(LMF_CastingFireLight_lf_hand_1_Multi_EW)
  {
    effect = LMF_FireShadowLight_A_Multi_CE;
    constraint = "caster.Bip01 L Hand";
    lifetime = 0.2;
    delay = 0.1;
    fadeInTime = 0.05;
    fadeOutTime = 0.05;
  };
  datablock afxEffectWrapperData(LMF_CastingFireLight_lf_hand_2_Multi_EW : LMF_CastingFireLight_lf_hand_1_Multi_EW)
  {
    effect = LMF_FireShadowLight_B_Multi_CE;
    delay = 0.15;
    xfmModifiers[0] = "LMF_CastingFireLight_offset2_XM";
  };
  datablock afxEffectWrapperData(LMF_CastingFireLight_lf_hand_3_Multi_EW : LMF_CastingFireLight_lf_hand_1_Multi_EW)
  {
    delay = 0.35;
    xfmModifiers[0] = "LMF_CastingFireLight_offset1_XM";
  };

  datablock afxEffectWrapperData(LMF_CastingFireLight_rt_hand_1_Multi_EW)
  {
    effect = LMF_FireShadowLight_B_Multi_CE;
    constraint = "caster.Bip01 R Hand";
    lifetime = 0.2;
    delay = 0.15;
    fadeInTime = 0.05;
    fadeOutTime = 0.05;
    xfmModifiers[0] = "LMF_CastingFireLight_offset1_XM";
  };
  datablock afxEffectWrapperData(LMF_CastingFireLight_rt_hand_2_Multi_EW : LMF_CastingFireLight_rt_hand_1_Multi_EW)
  {
    effect = LMF_FireShadowLight_A_Multi_CE;
    delay = 0.25;
  };
  datablock afxEffectWrapperData(LMF_CastingFireLight_rt_hand_3_Multi_EW : LMF_CastingFireLight_rt_hand_1_Multi_EW)
  {
    delay = 0.4;
    xfmModifiers[0] = "LMF_CastingFireLight_offset2_XM";
  };
  
  $LMF_CastingFireLight_lf_hand_1 = LMF_CastingFireLight_lf_hand_1_Multi_EW;
  $LMF_CastingFireLight_lf_hand_2 = LMF_CastingFireLight_lf_hand_2_Multi_EW;
  $LMF_CastingFireLight_lf_hand_3 = LMF_CastingFireLight_lf_hand_3_Multi_EW;
  $LMF_CastingFireLight_rt_hand_1 = LMF_CastingFireLight_rt_hand_1_Multi_EW;
  $LMF_CastingFireLight_rt_hand_1 = LMF_CastingFireLight_rt_hand_2_Multi_EW;
  $LMF_CastingFireLight_rt_hand_3 = LMF_CastingFireLight_rt_hand_3_Multi_EW;
}
else
{
  %LMF_CastingFireLightA_intensity = ($isTGEA) ? 0.75 : 1.0;
  datablock afxLightData(LMF_FireShadowLight_A_CE)
  {
    type = "Point";  
    radius = 4;
    sgCastsShadows = true;
    sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Original Stock";
    color = 1.0*%LMF_CastingFireLightA_intensity SPC
            0.9*%LMF_CastingFireLightA_intensity SPC
            0.3*%LMF_CastingFireLightA_intensity;
  };

  %LMF_CastingFireLightB_intensity = ($isTGEA) ? 0.75 : 1.0;
  datablock afxLightData(LMF_FireShadowLight_B_CE)
  {
    type = "Point";  
    radius = 3;
    sgCastsShadows = true;
    sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Original Stock";
    color = 1.0*%LMF_CastingFireLightB_intensity SPC
            0.5*%LMF_CastingFireLightB_intensity SPC
            0.1*%LMF_CastingFireLightB_intensity;
  };

  datablock afxEffectWrapperData(LMF_CastingFireLight_lf_hand_1_EW)
  {
    effect = LMF_FireShadowLight_A_CE;
    constraint = "caster.Bip01 L Hand";
    lifetime = 0.2;
    delay = 0.1;
    fadeInTime = 0.05;
    fadeOutTime = 0.05;
  };
  datablock afxEffectWrapperData(LMF_CastingFireLight_lf_hand_2_EW : LMF_CastingFireLight_lf_hand_1_EW)
  {
    effect = LMF_FireShadowLight_B_CE;
    delay = 0.15;
    xfmModifiers[0] = "LMF_CastingFireLight_offset2_XM";
  };
  datablock afxEffectWrapperData(LMF_CastingFireLight_lf_hand_3_EW : LMF_CastingFireLight_lf_hand_1_EW)
  {
    delay = 0.35;
    xfmModifiers[0] = "LMF_CastingFireLight_offset1_XM";
  };

  datablock afxEffectWrapperData(LMF_CastingFireLight_rt_hand_1_EW)
  {
    effect = LMF_FireShadowLight_B_CE;
    constraint = "caster.Bip01 R Hand";
    lifetime = 0.2;
    delay = 0.15;
    fadeInTime = 0.05;
    fadeOutTime = 0.05;
    xfmModifiers[0] = "LMF_CastingFireLight_offset1_XM";
  };
  datablock afxEffectWrapperData(LMF_CastingFireLight_rt_hand_2_EW : LMF_CastingFireLight_rt_hand_1_EW)
  {
    effect = LMF_FireShadowLight_A_CE;
    delay = 0.25;
  };
  datablock afxEffectWrapperData(LMF_CastingFireLight_rt_hand_3_EW : LMF_CastingFireLight_rt_hand_1_EW)
  {
    delay = 0.4;
    xfmModifiers[0] = "LMF_CastingFireLight_offset2_XM";
  };
  
  $LMF_CastingFireLight_lf_hand_1 = LMF_CastingFireLight_lf_hand_1_EW;
  $LMF_CastingFireLight_lf_hand_2 = LMF_CastingFireLight_lf_hand_2_EW;
  $LMF_CastingFireLight_lf_hand_3 = LMF_CastingFireLight_lf_hand_3_EW;
  $LMF_CastingFireLight_rt_hand_1 = LMF_CastingFireLight_rt_hand_1_EW;
  $LMF_CastingFireLight_rt_hand_1 = LMF_CastingFireLight_rt_hand_2_EW;
  $LMF_CastingFireLight_rt_hand_3 = LMF_CastingFireLight_rt_hand_3_EW;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CAMPFIRE PARTICLES

//
// The essence of the campfire effect is the particles emitted 
// here.  There are 10 emitters, emerging in sequence.  Each spins
// about the center of the campfire, its radius of spin randomized
// using paths.  Some emit particles that form thin tendrils of fire,
// while other emit fire that becomes thick smoke billowing up.
//

// fire particle
datablock ParticleData(LMF_fire_P)
{
  // TGE textureName          = %mySpellDataPath @ "/LMF/particles/LMF_fireA";

  dragCoeffiecient     = 0.5;
  gravityCoefficient   = -0.4;

  inheritedVelFactor   = 0.00;
  lifetimeMS           = 1200;
  lifetimeVarianceMS   = 300;
  useInvAlpha          = false;
  spinRandomMin        = -360.0;
  spinRandomMax        = 360.0;

  colors[0]            = "1.0 1.0 1.0 1.0";
  colors[1]            = "1.0 0.9 0.3 1.0";
  colors[2]            = "1.0 0.4 0.1 1.0";
  colors[3]            = "1.0 0.0 0.0 0.5";

  sizes[0]             = 1.0*0.5;
  sizes[1]             = 2.0*0.5;
  sizes[2]             = 1.0*0.5;
  sizes[3]             = 0;

  times[0]             = 0.0;
  times[1]             = 0.2;
  times[2]             = 0.5;
  times[3]             = 1.0;

  // TGEA
  textureName          = %mySpellDataPath @ "/LMF/particles/lmf_tiled_parts"; // fireA
  textureCoords[0]     = "0.0 0.0";
  textureCoords[1]     = "0.0 0.5";
  textureCoords[2]     = "0.5 0.5";
  textureCoords[3]     = "0.5 0.0";
};
// fire particle, grows large at end of lifetime to transition to smoke
datablock ParticleData(LMF_fireBig_P : LMF_fire_P)
{
  lifetimeMS           = 1200*1.4;
  lifetimeVarianceMS   = 300*1.4;

  colors[0]            = "1.0   1.0  1.0   0.0";
  colors[1]            = "1.0   0.9  0.3   0.0";
  colors[2]            = "1.0   0.4  0.1   0.0";
  colors[3]            = "0.425 0.1  0.025 0.0";
  colors[4]            = "0.0   0.0  0.0   0.0";

  sizes[3]             = 2.0;
  sizes[4]             = 3.5;

  times[0]             = 0.0;
  times[1]             = 0.14;
  times[2]             = 0.4;
  times[3]             = 0.7;
  times[4]             = 1.0;
};

// smoke particle
//  (Note: setting "useInvAlpha" changes the openGL blend mode used
//         to render the particles, and is necessary to get a non-
//         glowy particle.  Hopefully in the future the blend mode
//         can be specified explicity.)
datablock ParticleData(LMF_smoke_P)
{
  // TGE textureName          = %mySpellDataPath @ "/LMF/particles/LMF_smokeA";

  dragCoeffiecient     = 0.5;
  gravityCoefficient   = -0.4;

  inheritedVelFactor   = 0.00;
  lifetimeMS           = 3500;
  lifetimeVarianceMS   = 500;
  spinRandomMin        = -360.0;
  spinRandomMax        = -300.0;

  colors[0]            = "0.0 0.0 0.0 0.0";
  colors[1]            = "0.0 0.0 0.0 0.0";
  colors[2]            = "0.3 0.3 0.3 0.5";
  colors[3]            = "0.0 0.0 0.0 0.0";

  sizes[0]             = 2.5;
  sizes[1]             = 3.0;
  sizes[2]             = 4.5;
  sizes[3]             = 7.0;

  times[0]             = 0.0;
  times[1]             = 0.3;
  times[2]             = 0.6;
  times[3]             = 1.0;

  // TGEA
  textureName          = %mySpellDataPath @ "/LMF/particles/lmf_tiled_parts"; // smokeA
  textureCoords[0]     = "0.5 0.0";
  textureCoords[1]     = "0.5 0.5";
  textureCoords[2]     = "1.0 0.5";
  textureCoords[3]     = "1.0 0.0";
};
// smoke particle, large
datablock ParticleData(LMF_smokeBig_P : LMF_smoke_P)
{
  spinRandomMin        = -360.0*0.5;
  spinRandomMax        = -300.0*0.5;
   
  colors[2]            = "0.15 0.15 0.15 0.25"; 

  sizes[0]             = 2.5*2.0;
  sizes[1]             = 3.0*2.0;
  sizes[2]             = 4.5*2.0;
  sizes[3]             = 7.0*2.0;
};

// fire emission variables:
//  three levels of emission velocity
%LMF_fireVelocity_A = 1.0;
%LMF_fireVelocity_B = 0.5;
%LMF_fireVelocity_C = 0.2;
//  five different emission vectors
%LMF_fireVector_A = "1.0 0.0 0.3";
%LMF_fireVector_B = "0.0 0.9 0.1";
%LMF_fireVector_C = "0.5 0.5 0.5";
%LMF_fireVector_D = "0.0 0.0 1.0";
%LMF_fireVector_E = "0.0 1.0 0.5";

// fire emitter 1 -- fire tendril
datablock afxParticleEmitterVectorData(LMF_fire1_E) // TGEA
{
  ejectionOffset        = 0.0;
  ejectionPeriodMS      = 10;
  periodVarianceMS      = 3;
  ejectionVelocity      = %LMF_fireVelocity_A;
  velocityVariance      = %LMF_fireVelocity_A*0.1;
  particles             = LMF_fire_P;

  fadeColor = true;
  fadeSize = true;

  // TGE emitterType = "vector";
  vector = %LMF_fireVector_A;
};
// fire emitter 2 -- fire tendril
datablock afxParticleEmitterVectorData(LMF_fire2_E : LMF_fire1_E) // TGEA
{
  ejectionVelocity = %LMF_fireVelocity_B;
  velocityVariance = %LMF_fireVelocity_B*0.1;
  vector           = %LMF_fireVector_E;
};
// fire emitter 3 -- smoky fire
datablock afxParticleEmitterVectorData(LMF_fire3_E : LMF_fire1_E) // TGEA
{
  particles        = "LMF_fireBig_P LMF_smoke_P LMF_smokeBig_P";
  fadeSize = false;
  fadeColor = false;

  ejectionVelocity = %LMF_fireVelocity_C;
  velocityVariance = %LMF_fireVelocity_C*0.1;
  vector           = %LMF_fireVector_B;

  // TGEA
  blendStyle       = "USER";
  srcBlendFactor   = "ONE";
  dstBlendFactor   = "ONE_MINUS_SRC_ALPHA";
  //sortParticles = true;
};
// fire emitter 4 -- fire tendril
datablock afxParticleEmitterVectorData(LMF_fire4_E : LMF_fire1_E) // TGEA
{
  ejectionVelocity = %LMF_fireVelocity_B;
  velocityVariance = %LMF_fireVelocity_B*0.1;
  vector           = %LMF_fireVector_D;
};
// fire emitter 5 -- fire tendril
datablock afxParticleEmitterVectorData(LMF_fire5_E : LMF_fire1_E) // TGEA
{
  ejectionVelocity = %LMF_fireVelocity_A;
  velocityVariance = %LMF_fireVelocity_A*0.1;
  vector           = %LMF_fireVector_C;
};
// fire emitter 6 -- smoky fire
datablock afxParticleEmitterVectorData(LMF_fire6_E : LMF_fire1_E) // TGEA
{
  particles        = "LMF_fireBig_P LMF_smoke_P LMF_smokeBig_P";
  fadeSize = false;
  fadeColor = false;

  ejectionVelocity = %LMF_fireVelocity_B;
  velocityVariance = %LMF_fireVelocity_B*0.1;
  vector           = %LMF_fireVector_B;

  // TGEA
  blendStyle       = "USER";
  srcBlendFactor   = "ONE";
  dstBlendFactor   = "ONE_MINUS_SRC_ALPHA";
  //sortParticles = true;
};
// fire emitter 7 -- fire tendril
datablock afxParticleEmitterVectorData(LMF_fire7_E : LMF_fire1_E) // TGEA
{
  ejectionVelocity = %LMF_fireVelocity_B;
  velocityVariance = %LMF_fireVelocity_B*0.1;
  vector           = %LMF_fireVector_A;
};
// fire emitter 8 -- fire tendril
datablock afxParticleEmitterVectorData(LMF_fire8_E : LMF_fire1_E) // TGEA
{
  ejectionVelocity = %LMF_fireVelocity_C;
  velocityVariance = %LMF_fireVelocity_C*0.1;
  vector           = %LMF_fireVector_C;
};
// fire emitter 9 -- smoky fire
datablock afxParticleEmitterVectorData(LMF_fire9_E : LMF_fire1_E) // TGEA
{
  particles        = "LMF_fireBig_P LMF_smoke_P LMF_smokeBig_P";
  fadeSize = false;
  fadeColor = false;

  ejectionVelocity = %LMF_fireVelocity_A;
  velocityVariance = %LMF_fireVelocity_A*0.1;
  vector           = %LMF_fireVector_D;

  // TGEA
  blendStyle       = "USER";
  srcBlendFactor   = "ONE";
  dstBlendFactor   = "ONE_MINUS_SRC_ALPHA";
  //sortParticles = true;
};
// fire emitter 10 -- fire tendril
datablock afxParticleEmitterVectorData(LMF_fire10_E : LMF_fire1_E) // TGEA
{
  ejectionVelocity = %LMF_fireVelocity_B;
  velocityVariance = %LMF_fireVelocity_B*0.1;
  vector           = %LMF_fireVector_E;
};

// local offset to place emitters in center of campfire
datablock afxXM_LocalOffsetData(LMF_Fire_Offset_Center_XM)
{
  localOffset = "0 0.6 0";
};

// spin modifiers, unique to each of the 10 emitters, that rotate
//  about the campfire's center:
%LMF_fireSpinMult = 4.0;
datablock afxXM_SpinData(LMF_Fire_Spin1_XM)
{
  spinAxis  = "0 0 1";
  spinAngle = 0;
  spinRate  = 120*%LMF_fireSpinMult;
};
datablock afxXM_SpinData(LMF_Fire_Spin2_XM)
{
  spinAxis  = "0 0 1";
  spinAngle = 45;
  spinRate  = -150*%LMF_fireSpinMult;
};
datablock afxXM_SpinData(LMF_Fire_Spin3_XM)
{
  spinAxis  = "0 0 1";
  spinAngle = -98;
  spinRate  = -190*%LMF_fireSpinMult;
};
datablock afxXM_SpinData(LMF_Fire_Spin4_XM)
{
  spinAxis  = "0 0 1";
  spinAngle = 0;
  spinRate  = 150*%LMF_fireSpinMult;
};
datablock afxXM_SpinData(LMF_Fire_Spin5_XM)
{
  spinAxis  = "0 0 1";
  spinAngle = 4;
  spinRate  = 133*%LMF_fireSpinMult;
};
datablock afxXM_SpinData(LMF_Fire_Spin6_XM)
{
  spinAxis  = "0 0 1";
  spinAngle = 156;
  spinRate  = -167*%LMF_fireSpinMult;
};
datablock afxXM_SpinData(LMF_Fire_Spin7_XM)
{
  spinAxis  = "0 0 1";
  spinAngle = 222;
  spinRate  = 80*%LMF_fireSpinMult;
};
datablock afxXM_SpinData(LMF_Fire_Spin8_XM)
{
  spinAxis  = "0 0 1";
  spinAngle = -67;
  spinRate  = -145*%LMF_fireSpinMult;
};
datablock afxXM_SpinData(LMF_Fire_Spin9_XM)
{
  spinAxis  = "0 0 1";
  spinAngle = 0;
  spinRate  = 100*%LMF_fireSpinMult;
};
datablock afxXM_SpinData(LMF_Fire_Spin10_XM)
{
  spinAxis  = "0 0 1";
  spinAngle = 78;
  spinRate  = -156*%LMF_fireSpinMult;
};

// four unique looping paths to randomize the radius of
//  rotation:
datablock afxPathData(LMF_Fire_PathA)
{
  points = "0  0.5 0 " @
           "0  1.0 0 " @
           "0  0.1 0 " @
           "0  0.5 0";
  loop = cycle;
  lifetime = 2.0;
};
datablock afxPathData(LMF_Fire_PathB)
{
  points = "0  0.1 0 " @
           "0 -0.3 0 " @
           "0  0.6 0 " @
           "0  0.1 0";
  loop = cycle;
  lifetime = 1.5;
};
datablock afxPathData(LMF_Fire_PathC)
{
  points = "0  0.7 0 " @
           "0  0.2 0 " @
           "0  0.4 0 " @
           "0  0.7 0";
  loop = cycle;
  lifetime = 2.5;
};
datablock afxPathData(LMF_Fire_PathD)
{
  points = "0 -0.3 0 " @
           "0  0.0 0 " @
           "0  1.0 0 " @
           "0 -0.3 0";
  loop = cycle;
  lifetime = 3.0;
};
%LMF_firePathMult = 0.5;
datablock afxXM_PathConformData(LMF_Fire_pathA_XM)
{
  paths = "LMF_Fire_PathA";
  pathMult = %LMF_firePathMult;
};
datablock afxXM_PathConformData(LMF_Fire_pathB_XM)
{
  paths = "LMF_Fire_PathB";
  pathMult = %LMF_firePathMult;
};
datablock afxXM_PathConformData(LMF_Fire_pathC_XM)
{
  paths = "LMF_Fire_PathC";
  pathMult = %LMF_firePathMult;
};
datablock afxXM_PathConformData(LMF_Fire_pathD_XM)
{
  paths = "LMF_Fire_PathD";
  pathMult = %LMF_firePathMult;
};

// delay variables that reveal the emitters in a linear sequence:
%LMF_fireDelay    = 1.5;
%LMF_fireDelay_1  = %LMF_fireDelay*0;
%LMF_fireDelay_2  = %LMF_fireDelay*1;
%LMF_fireDelay_3  = %LMF_fireDelay*2;
%LMF_fireDelay_4  = %LMF_fireDelay*3;
%LMF_fireDelay_5  = %LMF_fireDelay*4;
%LMF_fireDelay_6  = %LMF_fireDelay*5;
%LMF_fireDelay_7  = %LMF_fireDelay*6;
%LMF_fireDelay_8  = %LMF_fireDelay*7;
%LMF_fireDelay_9  = %LMF_fireDelay*8;
%LMF_fireDelay_10 = %LMF_fireDelay*9;

// fire 1
datablock afxEffectWrapperData(LMF_fire1_EW)
{
  effect = LMF_fire1_E;
  posConstraint = "#scene.CampFire";
  lifetime = 5.0;
  delay    = %LMF_fireDelay_1;
  fadeInTime  = 1.5;
  fadeOutTime = 1.5;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin1_XM";
  xfmModifiers[2] = "LMF_Fire_pathA_XM";

  forcedBBox = "-2.0 -2.0 0.0 2.0 2.0 40.0";
};
// fire 2
datablock afxEffectWrapperData(LMF_fire2_EW : LMF_fire1_EW)
{
  effect = LMF_fire2_E;
  delay  = %LMF_fireDelay_2;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin2_XM";
  xfmModifiers[2] = "LMF_Fire_pathB_XM";
};
// fire 3
datablock afxEffectWrapperData(LMF_fire3_EW : LMF_fire1_EW)
{
  effect = LMF_fire3_E;
  delay  = %LMF_fireDelay_3;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin3_XM";
  xfmModifiers[2] = "LMF_Fire_pathC_XM";
};
// fire 4
datablock afxEffectWrapperData(LMF_fire4_EW : LMF_fire1_EW)
{
  effect = LMF_fire4_E;
  delay  = %LMF_fireDelay_4;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin4_XM";
  xfmModifiers[2] = "LMF_Fire_pathD_XM";
};
// fire 5
datablock afxEffectWrapperData(LMF_fire5_EW : LMF_fire1_EW)
{
  effect = LMF_fire5_E;
  delay  = %LMF_fireDelay_5;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin5_XM";
  xfmModifiers[2] = "LMF_Fire_pathB_XM";
};
// fire 6
datablock afxEffectWrapperData(LMF_fire6_EW : LMF_fire1_EW)
{
  effect = LMF_fire6_E;
  delay  = %LMF_fireDelay_6;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin6_XM";
  xfmModifiers[2] = "LMF_Fire_pathA_XM";
};
// fire 7
datablock afxEffectWrapperData(LMF_fire7_EW : LMF_fire1_EW)
{
  effect = LMF_fire7_E;
  delay  = %LMF_fireDelay_7;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin7_XM";
  xfmModifiers[2] = "LMF_Fire_pathC_XM";
};
// fire 8
datablock afxEffectWrapperData(LMF_fire8_EW : LMF_fire1_EW)
{
  effect = LMF_fire8_E;
  delay  = %LMF_fireDelay_8;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin8_XM";
  xfmModifiers[2] = "LMF_Fire_pathA_XM";
};
// fire 9
datablock afxEffectWrapperData(LMF_fire9_EW : LMF_fire1_EW)
{
  effect = LMF_fire9_E;
  delay  = %LMF_fireDelay_9;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin9_XM";
  xfmModifiers[2] = "LMF_Fire_pathB_XM";
};
// fire 10
datablock afxEffectWrapperData(LMF_fire10_EW : LMF_fire1_EW)
{
  effect = LMF_fire10_E;
  delay  = %LMF_fireDelay_10;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin10_XM";
  xfmModifiers[2] = "LMF_Fire_pathD_XM";
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CAMPFIRE LIGHTS

//
// For each of the ten fire emitters defined above, there is a
// corresponding light.  Their timing and placement with transform
// modifiers is identical to the emitters.  In addition, new paths
// are used to jiggle the lights up and down which randomizes the
// size of the circle of light cast on the ground for a nice effect.
//

// three campfire lights of varying sizes and colors
%LMF_FireLight_Radius_Mult = ($isTGEA) ? 4.0 : 2.0;
%LMF_FireLight_Intensity_Mult = ($isTGEA) ? 0.2 : 1.0;
datablock afxLightData(LFM_FireLight_A_CE)
{
  type = "Point";  
  radius = 1.2*%LMF_FireLight_Radius_Mult;
  color  = 0.50*%LMF_FireLight_Intensity_Mult SPC
           0.45*%LMF_FireLight_Intensity_Mult SPC
           0.15*%LMF_FireLight_Intensity_Mult;

  sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Near Linear";
};
datablock afxLightData(LFM_FireLight_B_CE : LFM_FireLight_A_CE)
{
  radius = 1.8*%LMF_FireLight_Radius_Mult;
  color  = 0.50*%LMF_FireLight_Intensity_Mult SPC
           0.30*%LMF_FireLight_Intensity_Mult SPC
           0.05*%LMF_FireLight_Intensity_Mult;
};
datablock afxLightData(LFM_FireLight_C_CE : LFM_FireLight_A_CE)
{
  radius = 0.7*%LMF_FireLight_Radius_Mult;
  color  = 0.50*%LMF_FireLight_Intensity_Mult SPC
           0.45*%LMF_FireLight_Intensity_Mult SPC
           0.40*%LMF_FireLight_Intensity_Mult;
};

// local offset to place lights elevated in the campfires center
datablock afxXM_LocalOffsetData(LMF_FireLight_Offset_Center_XM)
{
  localOffset = ($isTGEA) ? "0 0.6 2.0" : "0 0.6 1.0";
};

// paths to jiggle the lights up and down
datablock afxPathData(LMF_FireLight_PathA)
{
  points = "0 0  1.0 " @
           "0 0 -0.3 " @
           "0 0  0.2 " @
           "0 0 -0.2 " @
           "0 0  1.0";
  loop = cycle;
  lifetime = 0.5;
};
datablock afxPathData(LMF_FireLight_PathB)
{
  points = "0 0  0.1 " @
           "0 0  0.7 " @
           "0 0 -0.5 " @
           "0 0  0.1";
  loop = cycle;
  lifetime = 0.4;
};
datablock afxPathData(LMF_FireLight_PathC)
{
  points = "0 0 -0.5 " @
           "0 0  0.3 " @
           "0 0  0.0 " @
           "0 0  0.1 " @
           "0 0 -0.5";
  loop = cycle;
  lifetime = 0.6;
};
datablock afxXM_PathConformData(LMF_FireLight_pathA_XM)
{
  paths = "LMF_FireLight_PathA";
};
datablock afxXM_PathConformData(LMF_FireLight_pathB_XM)
{
  paths = "LMF_FireLight_PathB";
};
datablock afxXM_PathConformData(LMF_FireLight_pathC_XM)
{
  paths = "LMF_FireLight_PathC";
};

// campfire light 1
datablock afxEffectWrapperData(LMF_fireLight1_EW)
{
  effect = LFM_FireLight_A_CE;
  posConstraint = "#scene.CampFire";
  lifetime = 5.0;
  delay    = %LMF_fireDelay_1;
  fadeInTime  = 1.5;
  fadeOutTime = 1.5;
  xfmModifiers[0] = "LMF_FireLight_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin1_XM";
  xfmModifiers[2] = "LMF_Fire_pathA_XM";
  xfmModifiers[3] = "LMF_FireLight_pathA_XM";
};
// campfire light 2
datablock afxEffectWrapperData(LMF_fireLight2_EW : LMF_fireLight1_EW)
{
  effect = LFM_FireLight_B_CE;
  delay    = %LMF_fireDelay_2;
  xfmModifiers[0] = "LMF_FireLight_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin2_XM";
  xfmModifiers[2] = "LMF_Fire_pathB_XM";
  xfmModifiers[3] = "LMF_FireLight_pathB_XM";
};
// campfire light 3
datablock afxEffectWrapperData(LMF_fireLight3_EW : LMF_fireLight1_EW)
{
  effect = LFM_FireLight_C_CE;
  delay    = %LMF_fireDelay_3;
  xfmModifiers[0] = "LMF_FireLight_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin3_XM";
  xfmModifiers[2] = "LMF_Fire_pathC_XM";
  xfmModifiers[3] = "LMF_FireLight_pathC_XM";
};
// campfire light 4
datablock afxEffectWrapperData(LMF_fireLight4_EW : LMF_fireLight1_EW)
{
  effect = LFM_FireLight_B_CE;
  delay    = %LMF_fireDelay_4;
  xfmModifiers[0] = "LMF_FireLight_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin4_XM";
  xfmModifiers[2] = "LMF_Fire_pathD_XM";
  xfmModifiers[3] = "LMF_FireLight_pathA_XM";
};
// campfire light 5
datablock afxEffectWrapperData(LMF_fireLight5_EW : LMF_fireLight1_EW)
{
  effect = LFM_FireLight_A_CE;
  delay    = %LMF_fireDelay_5;
  xfmModifiers[0] = "LMF_FireLight_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin5_XM";
  xfmModifiers[2] = "LMF_Fire_pathB_XM";
  xfmModifiers[3] = "LMF_FireLight_pathB_XM";
};
// campfire light 6
datablock afxEffectWrapperData(LMF_fireLight6_EW : LMF_fireLight1_EW)
{
  effect = LFM_FireLight_B_CE;
  delay    = %LMF_fireDelay_6;
  xfmModifiers[0] = "LMF_FireLight_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin6_XM";
  xfmModifiers[2] = "LMF_Fire_pathA_XM";
  xfmModifiers[3] = "LMF_FireLight_pathB_XM";
};
// campfire light 7
datablock afxEffectWrapperData(LMF_fireLight7_EW : LMF_fireLight1_EW)
{
  effect = LFM_FireLight_A_CE;
  delay    = %LMF_fireDelay_7;
  xfmModifiers[0] = "LMF_FireLight_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin7_XM";
  xfmModifiers[2] = "LMF_Fire_pathC_XM";
  xfmModifiers[3] = "LMF_FireLight_pathA_XM";
};
// campfire light 8
datablock afxEffectWrapperData(LMF_fireLight8_EW : LMF_fireLight1_EW)
{
  effect = LFM_FireLight_C_CE;
  delay    = %LMF_fireDelay_8;
  xfmModifiers[0] = "LMF_FireLight_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin8_XM";
  xfmModifiers[2] = "LMF_Fire_pathA_XM";
  xfmModifiers[3] = "LMF_FireLight_pathC_XM";
};
// campfire light 9
datablock afxEffectWrapperData(LMF_fireLight9_EW : LMF_fireLight1_EW)
{
  effect = LFM_FireLight_A_CE;
  delay    = %LMF_fireDelay_9;
  xfmModifiers[0] = "LMF_FireLight_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin9_XM";
  xfmModifiers[2] = "LMF_Fire_pathB_XM";
  xfmModifiers[3] = "LMF_FireLight_pathA_XM";
};
// campfire light 10
datablock afxEffectWrapperData(LMF_fireLight10_EW : LMF_fireLight1_EW)
{
  effect = LFM_FireLight_B_CE;
  delay    = %LMF_fireDelay_10;
  xfmModifiers[0] = "LMF_FireLight_Offset_Center_XM";
  xfmModifiers[1] = "LMF_Fire_Spin10_XM";
  xfmModifiers[2] = "LMF_Fire_pathD_XM";
  xfmModifiers[3] = "LMF_FireLight_pathB_XM";
};

datablock afxPathData(LMF_FireShadowLight_1_Path)
{
  points = " 0    0    0"   SPC
     " 0.1 -0.3  0.8" SPC
     "-0.3  0.2 -0.6" SPC
     " 0.0 -0.6  0.4" SPC
     " -0.7 0.4 -0.8" SPC
     " 0    0    0";
  lifetime = 0.25*2.0;
  loop = cycle;
  mult = 0.30*0.25;
};
//
datablock afxXM_PathConformData(LMF_FireShadowLight_1_path_XM)
{
  paths = "LMF_FireShadowLight_1_Path";
};

datablock afxPathData(LMF_FireShadowLight_2_Path)
{
  points = " 0    0    0"   SPC
     " 0.4  0.7 -0.3" SPC
     "-0.3  0.0  0.4" SPC
     " 0.2  0.4 -0.8" SPC
     "-0.4 -0.8  0.5" SPC
     " 0    0    0";
  lifetime = 0.20*2.0;
  loop = cycle;
  mult = 0.25*0.25;
};
//
datablock afxXM_PathConformData(LMF_FireShadowLight_2_path_XM)
{
  paths = "LMF_FireShadowLight_2_Path";
};

if ($isTGEA)
{
  %LMF_FireShadowLight_LMODELS_intensity = 1;
  %LMF_FireShadowLight_LTERRAIN_intensity = 0.25;
  
  datablock sgLightObjectData(LMF_FireShadowLight_LMODELS_CE) 
  {
    Radius = 8;
    Brightness = ($isTGEA) ? %LMF_FireShadowLight_intensity : (2*%LMF_FireShadowLight_intensity);
    Colour = "1.0 0.7 0.2";
    CastsShadows = true;

    LightingModelName = ($isTGEA) ? "Original Advanced" : "Original Stock";

    AnimBrightness = true;
    LerpBrightness = true;
    MinBrightness = 2*%LMF_FireShadowLight_LMODELS_intensity*0.6;
    MaxBrightness = 2*%LMF_FireShadowLight_LMODELS_intensity;  
    BrightnessKeys = "AZA";
    BrightnessTime = 0.25;

    AnimRadius = true;
    LerpRadius = true;
    MinRadius = ($isTGEA) ? ((8-0.15)*0.5) : (8-0.15);
    MaxRadius = ($isTGEA) ? ((8+0.15)*0.5) : (8+0.15);
    RadiusKeys = "AZA";
    RadiusTime = 0.35;
    
    lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF; // TGEA (ignored by TGE)
  };
  datablock sgLightObjectData(LMF_FireShadowLight_LTERRAIN_CE : LMF_FireShadowLight_LMODELS_CE) 
  {
    MinBrightness = 2*%LMF_FireShadowLight_LTERRAIN_intensity*0.6;
    MaxBrightness = 2*%LMF_FireShadowLight_LTERRAIN_intensity;  
    lightIlluminationMask = $AFX::ILLUM_TERRAIN | $AFX::ILLUM_ATLAS; // TGEA (ignored by TGE) 
  };
  
  datablock sgLightObjectData(LMF_FireShadowLight2_LMODELS_CE : LMF_FireShadowLight_LMODELS_CE)
  {
    BrightnessTime = 0.35;
    RadiusTime = 0.5;
  };
  datablock sgLightObjectData(LMF_FireShadowLight2_LTERRAIN_CE : LMF_FireShadowLight_LTERRAIN_CE)
  {
    BrightnessTime = 0.35;
    RadiusTime = 0.5;
  };

  datablock afxMultiLightData(LMF_FireShadowLight_Multi_CE)
  {
    lights[0] = LMF_FireShadowLight_LMODELS_CE;
    lights[1] = LMF_FireShadowLight_LTERRAIN_CE;
  };
  datablock afxMultiLightData(LMF_FireShadowLight2_Multi_CE)
  {
    lights[0] = LMF_FireShadowLight2_LMODELS_CE;
    lights[1] = LMF_FireShadowLight2_LTERRAIN_CE;
  };

  datablock afxEffectWrapperData(LMF_FireShadowLight_1_Multi_EW)
  {
    effect = LMF_FireShadowLight_Multi_CE;
    posConstraint = "#scene.CampFire";
    lifetime = 9.0;
    delay    = %LMF_fireDelay_1;
    fadeInTime  = 0.5;
    fadeOutTime = 0.5;
    xfmModifiers[0] = "LMF_FireLight_Offset_Center_XM";
    xfmModifiers[1] = "LMF_FireShadowLight_1_path_XM";
  };

  datablock afxEffectWrapperData(LMF_FireShadowLight_2_Multi_EW : LMF_FireShadowLight_1_Multi_EW)
  {
    effect = LMF_FireShadowLight2_Multi_CE;
    xfmModifiers[1] = "LMF_FireShadowLight_2_path_XM";
  };

  $LMF_FireShadowLight_1 = LMF_FireShadowLight_1_Multi_EW;
  $LMF_FireShadowLight_2 = LMF_FireShadowLight_2_Multi_EW;
}
else
{
  %LMF_FireShadowLight_intensity = 0.25;
  datablock sgLightObjectData(LMF_FireShadowLight_CE) 
  {
    Radius = 8;
    Brightness = ($isTGEA) ? %LMF_FireShadowLight_intensity : (2*%LMF_FireShadowLight_intensity);
    Colour = "1.0 0.7 0.2";
    CastsShadows = true;

    LightingModelName = ($isTGEA) ? "Original Advanced" : "Original Stock";

    AnimBrightness = true;
    LerpBrightness = true;
    MinBrightness = 2*%LMF_FireShadowLight_intensity*0.6;
    MaxBrightness = 2*%LMF_FireShadowLight_intensity;  
    BrightnessKeys = "AZA";
    BrightnessTime = 0.25;

    AnimRadius = true;
    LerpRadius = true;
    MinRadius = ($isTGEA) ? ((8-0.15)*0.5) : (8-0.15);
    MaxRadius = ($isTGEA) ? ((8+0.15)*0.5) : (8+0.15);
    RadiusKeys = "AZA";
    RadiusTime = 0.35;
  };

  datablock sgLightObjectData(LMF_FireShadowLight2_CE : LMF_FireShadowLight_CE)
  {
    BrightnessTime = 0.35;
    RadiusTime = 0.5;
  };

  datablock afxEffectWrapperData(LMF_FireShadowLight_1_EW)
  {
    effect = LMF_FireShadowLight_CE;
    posConstraint = "#scene.CampFire";
    lifetime = 9.0;
    delay    = %LMF_fireDelay_1;
    fadeInTime  = 0.5;
    fadeOutTime = 0.5;
    xfmModifiers[0] = "LMF_FireLight_Offset_Center_XM";
    xfmModifiers[1] = "LMF_FireShadowLight_1_path_XM";
  };
  datablock afxEffectWrapperData(LMF_FireShadowLight_2_EW : LMF_FireShadowLight_1_EW)
  {
    effect = LMF_FireShadowLight2_CE;
    xfmModifiers[1] = "LMF_FireShadowLight_2_path_XM";
  };
  
  $LMF_FireShadowLight_1 = LMF_FireShadowLight_1_EW;
  $LMF_FireShadowLight_2 = LMF_FireShadowLight_2_EW;
}

datablock afxVolumeLightData(LMF_LightBeam_CE)
{
  LightOn = true;
  Radius = 7.0;
  Brightness = 1.0;
  Colour = "1.0 0.95 0.75";

  Texture = "common/lighting/corona.png"; //lightFalloffMono.png";
  lpDistance = 100;
  ShootDistance = 100;
  Xextent = 3.0;
  Yextent = 3.0;
  SubdivideU = 4;
  SubdivideV = 4;
  FootColour = "1.0 0.95 0.75 0.2";
  TailColour = "0 0 0 0";

  AnimRotation = true;
  LerpRotation = true;
  MinRotation = 0;
  MaxRotation = 359;
  RotationKeys = "AZA";
  RotationTime = 1.5*0.2;

  AnimBrightness = true;
  LerpBrightness = true;
  MinBrightness = 0.3;
  MaxBrightness = 0.9;
  BrightnessKeys = "AZA";
  BrightnessTime = 0.1;
};
//
datablock afxXM_SpinData(LMF_LightBeam_spin1_XM)
{
  spinAxis  = "0 0 1";
  spinAngle = 0;
  spinRate  = 60;
};

// this is necessary to push the light beam down into the campfire;
//  otherwise, the bottom end is visible...
datablock afxXM_LocalOffsetData(LMF_LightBeam_offset_XM)
{
  localOffset = ($isTGEA) ? "0.0 0.0 -2.0" : "0.0 0.0 -1.0";
};

datablock afxEffectWrapperData(LMF_LightBeam_EW)
{
  effect = LMF_LightBeam_CE;
  posConstraint = "#scene.CampFire";
  lifetime = 3.0;
  delay    = 0;
  fadeInTime  = 0.5;
  fadeOutTime = 0.5;
  xfmModifiers[0] = LMF_FireLight_Offset_Center_XM;
  xfmModifiers[1] = LMF_LightBeam_offset_XM;
  xfmModifiers[2] = LMF_LightBeam_spin1_XM;
  xfmModifiers[3] = LMF_FireShadowLight_1_path_XM;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CAMPFIRE GLOW ZODIACS

//
// To add interest (realism?) to the pattern of light cast by the 
// campfire on the ground, two subtle zodiacs are used to help create
// the illusion of a flickering pattern.
//

// fire glow zodiacs, rotating in opposite directions
datablock afxZodiacData(LMF_FireGlowZodeA_CE)
{
  texture = %mySpellDataPath @ "/LMF/zodiacs/LMF_groundGlow";
  radius = 3.0;
  startAngle = 0.0;
  rotationRate = 500.0*0.6;
  color = "1.0 1.0 1.0 0.1";
  blend = additive;
};
datablock afxZodiacData(LMF_FireGlowZodeB_CE : LMF_FireGlowZodeA_CE)
{
  radius = 3.3;
  rotationRate = -620.0*0.6;
};

datablock afxEffectWrapperData(LMF_FireGlowZode1_EW)
{
  effect = LMF_FireGlowZodeA_CE;
  posConstraint = "#scene.CampFire";
  delay = 3.0;
  fadeInTime = 2.0;
  fadeOutTime = 1.0;
  lifetime = 9.0;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
};
datablock afxEffectWrapperData(LMF_FireGlowZode2_EW : LMF_FireGlowZode1_EW)
{
  effect = LMF_FireGlowZodeB_CE;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CAMPFIRE EMBERS

//
// Embers are emitted throughout the lifetime of the campfire.  Some
// are emitted up through the smoke plume, while others spurt out
// randomly from the flames.  These embers are initially red/orange
// but turn black as they burn out and fall.  There are three 
// different ember texture maps, a bit decadent. 
//

// ember particle A
datablock ParticleData(LMF_emberA_P)
{
  // TGE textureName          = %mySpellDataPath @ "/LMF/particles/LMF_emberA";

  dragCoeffiecient     = 0.5;
  gravityCoefficient   = 1.0;

  inheritedVelFactor   = 0.00;
  lifetimeMS           = 1200;
  lifetimeVarianceMS   = 300;
  useInvAlpha          = true;
  spinRandomMin        = -720.0;
  spinRandomMax        = 720.0;

  colors[0]            = "1.0 1.0 1.0 1.0";
  colors[1]            = "1.0 1.0 1.0 1.0";
  colors[2]            = "0.0 0.0 0.0 1.0";
  colors[3]            = "0.0 0.0 0.0 0.0";

  sizes[0]             = 0.25;
  sizes[1]             = 0.25;
  sizes[2]             = 0.25;
  sizes[3]             = 0.25;
   
  times[0]             = 0.0;
  times[1]             = 0.4;
  times[2]             = 0.6;
  times[3]             = 1.0;

  // TGEA
  textureName          = %mySpellDataPath @ "/LMF/particles/lmf_tiled_parts"; // emberA
  textureCoords[0]     = "0.0  0.5";
  textureCoords[1]     = "0.0  0.75";
  textureCoords[2]     = "0.25 0.75";
  textureCoords[3]     = "0.25 0.5";
};
// ember particle B
datablock ParticleData(LMF_emberB_P : LMF_emberA_P)
{
  // TGE textureName          = %mySpellDataPath @ "/LMF/particles/LMF_emberB";

  // TGEA -- emberB
  textureCoords[0] = "0.25 0.5"; 
  textureCoords[1] = "0.25 0.75";
  textureCoords[2] = "0.5  0.75";
  textureCoords[3] = "0.5  0.5";
};
// ember particle C
datablock ParticleData(LMF_emberC_P : LMF_emberA_P)
{
  // TGE textureName          = %mySpellDataPath @ "/LMF/particles/LMF_emberC";

  // TGEA -- emberC
  textureCoords[0] = "0.5  0.5";
  textureCoords[1] = "0.5  0.75";
  textureCoords[2] = "0.75 0.75";
  textureCoords[3] = "0.75 0.5";
};

// upward ember emitter, using a cone
datablock afxParticleEmitterConeData(LMF_embersUp_E) // TGEA
{
  ejectionPeriodMS      = 600;
  periodVarianceMS      = 400;
  ejectionVelocity      = 15.0;
  velocityVariance      = 3.5;  
  particles             = "LMF_emberA_P LMF_emberB_P LMF_emberC_P";

  // TGE emitterType = "cone";
  vector = "0 0 1";
  spreadMin = 0.0;
  spreadMax = 40.0;
};
//
datablock afxEffectWrapperData(LMF_embersUp_EW)
{
  effect = LMF_embersUp_E;
  posConstraint = "#scene.CampFire";
  lifetime = 15.0;
  delay    = 3.5;
  fadeInTime  = 0;
  fadeOutTime = 0;
};

// spurting ember emitter, using standard Torque "sprinkler"
datablock ParticleEmitterData(LMF_embersSpurt_E)
{
  ejectionPeriodMS      = 600;
  periodVarianceMS      = 400;
  ejectionVelocity      = 10.0;
  velocityVariance      = 3.5;  
  particles             = "LMF_emberA_P LMF_emberB_P LMF_emberC_P";

  // TGE emitterType = "sprinkler";
};
//
datablock afxEffectWrapperData(LMF_embersSpurt_EW)
{
  effect = LMF_embersSpurt_E;
  posConstraint = "#scene.CampFire";
  lifetime = 15.0;
  delay    = 3.5;
  fadeInTime  = 0;
  fadeOutTime = 0;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CAMPFIRE BURST PARTICLES

//
// To add interest to the campfire, five short bursts of fire are
// added near the fire's base.  From these bursts come flying embers
// and sparks, defined subsequently.
// 

// fire-burst particle
datablock ParticleData(LMF_fireBurst_P)
{
  // TGE textureName          = %mySpellDataPath @ "/LMF/particles/LMF_fireA";

  dragCoeffiecient     = 0.5;
  gravityCoefficient   = -0.4;

  inheritedVelFactor   = 0.00;
  lifetimeMS           = 800;
  lifetimeVarianceMS   = 200;
  useInvAlpha          = false;
  spinRandomMin        = -360.0;
  spinRandomMax        = 360.0;

  colors[0]            = "0.7 0.0 0.0 0.0";
  colors[1]            = "1.0 1.0 1.0 1.0";
  colors[2]            = "1.0 0.4 0.1 1.0";
  colors[3]            = "0.7 0.0 0.0 0.0";

  sizes[0]             = 0.1;
  sizes[1]             = 2.0;
  sizes[2]             = 0.5;
  sizes[3]             = 0.1;

  times[0]             = 0.0;
  times[1]             = 0.25;
  times[2]             = 0.5;
  times[3]             = 1.0;

  // TGEA
  textureName          = %mySpellDataPath @ "/LMF/particles/lmf_tiled_parts"; // fireA
  textureCoords[0]     = "0.0 0.0";
  textureCoords[1]     = "0.0 0.5";
  textureCoords[2]     = "0.5 0.5";
  textureCoords[3]     = "0.5 0.0";
};

// fire burst emitter
datablock ParticleEmitterData(LMF_fireBurst_E)
{
  ejectionPeriodMS      = 10;
  periodVarianceMS      = 3;
  ejectionVelocity      = 1.0;
  velocityVariance      = 0.1;
  particles             = LMF_fireBurst_P;

  // TGE emitterType = "sprinkler";
};

// five local offsets to place the bursts in "random" positions:
datablock afxXM_LocalOffsetData(LMF_FireBurst_Offset1_XM)
{
  localOffset = "0.0 0.7 0.25";
};
datablock afxXM_LocalOffsetData(LMF_FireBurst_Offset2_XM)
{
  localOffset = "-0.3 0.2 0.35";
};
datablock afxXM_LocalOffsetData(LMF_FireBurst_Offset3_XM)
{
  localOffset = "0.5 -0.1 0.15";
};
datablock afxXM_LocalOffsetData(LMF_FireBurst_Offset4_XM)
{
  localOffset = "-0.5 0.5 0.25";
};
datablock afxXM_LocalOffsetData(LMF_FireBurst_Offset5_XM)
{
  localOffset = "0.2 -0.7 0.1";
};

// fire burst 1
datablock afxEffectWrapperData(LMF_fireBurst1_EW)
{
  effect = LMF_fireBurst_E;
  posConstraint = "#scene.CampFire";
  lifetime = 0.35*0.6;
  delay    = 5.0;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset1_XM";
};
// fire burst 2
datablock afxEffectWrapperData(LMF_fireBurst2_EW)
{
  effect = LMF_fireBurst_E;
  posConstraint = "#scene.CampFire";
  lifetime = 0.45*0.6;
  delay    = 7.0;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset2_XM";
};
// fire burst 3
datablock afxEffectWrapperData(LMF_fireBurst3_EW)
{
  effect = LMF_fireBurst_E;
  posConstraint = "#scene.CampFire";
  lifetime = 0.4*0.6;
  delay    = 7.5;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset3_XM";
};
// fire burst 4
datablock afxEffectWrapperData(LMF_fireBurst4_EW)
{
  effect = LMF_fireBurst_E;
  posConstraint = "#scene.CampFire";
  lifetime = 0.50*0.6;
  delay    = 9.1;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset4_XM";
};
// fire burst 5
datablock afxEffectWrapperData(LMF_fireBurst5_EW)
{
  effect = LMF_fireBurst_E;
  posConstraint = "#scene.CampFire";
  lifetime = 0.35*0.6;
  delay    = 10.9;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset5_XM";
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CAMPFIRE BURST LIGHTS

//
// Coordinated with the fire-bursts above are these lights that flash
// on and off quickly.
// 

// fire burst light
datablock afxLightData(LFM_FireBurstLight_CE)
{
  type = "Point";  
  radius = ($isTGEA) ? 5.0 : 4.0;
  color = "0.35 0.35 0.1";
  sgLightingModelName = ($isTGEA) ? "Original Stock" : "Near Linear";
};

// fire burst light 1
datablock afxEffectWrapperData(LMF_fireBurstLight1_EW : LMF_fireBurst1_EW)
{
  effect = LFM_FireBurstLight_CE;
  fadeInTime  = 0.1;
  fadeOutTime = 0.1;
};
// fire burst light 2
datablock afxEffectWrapperData(LMF_fireBurstLight2_EW : LMF_fireBurst2_EW)
{
  effect = LFM_FireBurstLight_CE;
  fadeInTime  = 0.1;
  fadeOutTime = 0.1;
};
// fire burst light 3
datablock afxEffectWrapperData(LMF_fireBurstLight3_EW : LMF_fireBurst3_EW)
{
  effect = LFM_FireBurstLight_CE;
  fadeInTime  = 0.1;
  fadeOutTime = 0.1;
};
// fire burst light 4
datablock afxEffectWrapperData(LMF_fireBurstLight4_EW : LMF_fireBurst4_EW)
{
  effect = LFM_FireBurstLight_CE;
  fadeInTime  = 0.1;
  fadeOutTime = 0.1;
};
// fire burst light 5
datablock afxEffectWrapperData(LMF_fireBurstLight5_EW : LMF_fireBurst5_EW)
{
  effect = LFM_FireBurstLight_CE;
  fadeInTime  = 0.1;
  fadeOutTime = 0.1;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CAMPFIRE BURST EMBERS

//
// Coordinated with the fire-bursts above are the emission of bursts
// of ember particles.
// 

// fire burst ember emitter
datablock ParticleEmitterData(LMF_embersBurst_E)
{
  ejectionPeriodMS      = 30;
  periodVarianceMS      = 10;
  ejectionVelocity      = 10.0;
  velocityVariance      = 3.5;  
  particles             = "LMF_emberA_P LMF_emberB_P LMF_emberC_P";

  // TGE emitterType = "sprinkler";
};

// fire burst embers 1
datablock afxEffectWrapperData(LMF_embersBurst1_EW : LMF_fireBurst1_EW)
{
  effect = LMF_embersBurst_E;
};
// fire burst embers 2
datablock afxEffectWrapperData(LMF_embersBurst2_EW : LMF_fireBurst2_EW)
{
  effect = LMF_embersBurst_E;
};
// fire burst embers 3
datablock afxEffectWrapperData(LMF_embersBurst3_EW : LMF_fireBurst3_EW)
{
  effect = LMF_embersBurst_E;
};
// fire burst embers 4
datablock afxEffectWrapperData(LMF_embersBurst4_EW : LMF_fireBurst4_EW)
{
  effect = LMF_embersBurst_E;
};
// fire burst embers 5
datablock afxEffectWrapperData(LMF_embersBurst5_EW : LMF_fireBurst5_EW)
{
  effect = LMF_embersBurst_E;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CAMPFIRE BURST SPARKS

//
// Coordinated with the fire-bursts above are the emission of sparks.
// Each spark consists of two kinds of particles: one tiny that, when
// emitted often and along a vector, creates a line; the other larger
// to appear as the "head" of the spark.  Between 2 and 5 sparks are
// emitted for each of the five fire bursts.  The numbers in the
// names of the spark datablocks corresponds to a specific burst, 1-5.
// 

// spark particle, tiny
datablock ParticleData(LMF_sparkBurst_P)
{
  // TGE textureName          = %mySpellDataPath @ "/LMF/particles/LMF_spark";

  dragCoeffiecient     = 0.5;
  gravityCoefficient   = 1.0;

  inheritedVelFactor   = 0.00;
  lifetimeMS           = 700;
  lifetimeVarianceMS   = 50;
  useInvAlpha          = false;

  colors[0]            = "1.0 1.0 1.0 0.4";
  colors[1]            = "1.0 0.9 0.5 0.4";
  colors[2]            = "1.0 0.0 0.0 0.0";

  sizes[0]             = 0.3*0.5;
  sizes[1]             = 0.1*0.5;
  sizes[2]             = 0.0;

  times[0]             = 0.0;
  times[1]             = 0.5;
  times[2]             = 1.0;

  // TGEA
  textureName          = %mySpellDataPath @ "/LMF/particles/lmf_tiled_parts"; // spark
  textureCoords[0]     = "0.75 0.5";
  textureCoords[1]     = "0.75 0.75";
  textureCoords[2]     = "1.0  0.75";
  textureCoords[3]     = "1.0  0.5";
};
// spark particle, larger
datablock ParticleData(LMF_sparkHeadBurst_P : LMF_sparkBurst_P)
{
   colors[0]            = "1.0 1.0 1.0 1.0";
   colors[1]            = "1.0 0.9 0.5 1.0";
   colors[2]            = "1.0 0.0 0.0 0.0";

   sizes[0]             = 0.3*1.0;
   sizes[1]             = 0.1*1.0;
   sizes[2]             = 0.0;
};

// spark emission vectors:
%LMF_sparkVector_A = " 1.0  0.0 1.0";
%LMF_sparkVector_B = "-0.1  0.3 1.0";
%LMF_sparkVector_C = " 0.4 -0.9 1.0";
%LMF_sparkVector_D = " 0.0  0.9 1.0";
%LMF_sparkVector_E = "-0.3 -0.4 1.0";
%LMF_sparkVector_F = " 0.2  0.2 1.0";
%LMF_sparkVector_G = " 0.5  0.0 1.0";

// spark 1A emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst1A_E) // TGEA
{
  ejectionPeriodMS      = 5;
  periodVarianceMS      = 0;
  ejectionVelocity      = 18.0;
  velocityVariance      = 0.0;
  particles             = LMF_sparkHeadBurst_P;
  overrideAdvance       = false;

  //TGE emitterType = "vector";
  vector = %LMF_sparkVector_A;
};
// spark 1A emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst1A_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  ejectionPeriodMS      = 2;
  periodVarianceMS      = 0;
  particles             = LMF_sparkBurst_P;
};

// spark 1B emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst1B_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  vector = %LMF_sparkVector_B;
};
// spark 1B emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst1B_E : LMF_sparkBurst1A_E) // TGEA
{
  vector = %LMF_sparkVector_B;
};

// spark 1C emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst1C_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  ejectionVelocity = 13.0;
  vector = %LMF_sparkVector_C;
};
// spark 1C emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst1C_E : LMF_sparkBurst1A_E) // TGEA
{
  ejectionVelocity = 13.0;
  vector = %LMF_sparkVector_C;
};

// spark 2A emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst2A_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  vector = %LMF_sparkVector_D;
};
// spark 2A emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst2A_E : LMF_sparkBurst1A_E) // TGEA
{
  vector = %LMF_sparkVector_D;
};

// spark 2B emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst2B_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  ejectionVelocity = 15.0;
  vector = %LMF_sparkVector_E;
};
// spark 2B emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst2B_E : LMF_sparkBurst1A_E) // TGEA
{
  ejectionVelocity = 15.0;
  vector = %LMF_sparkVector_E;
};

// spark 3A emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst3A_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  vector = %LMF_sparkVector_F;
};
// spark 3A emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst3A_E : LMF_sparkBurst1A_E) // TGEA
{
  vector = %LMF_sparkVector_F;
};

// spark 3B emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst3B_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  ejectionVelocity = 20.0;
  vector = %LMF_sparkVector_B;
};
// spark 3B emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst3B_E : LMF_sparkBurst1A_E) // TGEA
{
  ejectionVelocity = 20.0;
  vector = %LMF_sparkVector_B;
};

// spark 3C emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst3C_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  ejectionVelocity = 11.0;
  vector = %LMF_sparkVector_G;
};
// spark 3C emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst3C_E : LMF_sparkBurst1A_E) // TGEA
{
  ejectionVelocity = 11.0;
  vector = %LMF_sparkVector_G;
};

// spark 4A emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst4A_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  vector = %LMF_sparkVector_A;
};
// spark 4A emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst4A_E : LMF_sparkBurst1A_E) // TGEA
{
  vector = %LMF_sparkVector_A;
};

// spark 4B emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst4B_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  ejectionVelocity = 15.0;
  vector = %LMF_sparkVector_E;
};
// spark 4B emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst4B_E : LMF_sparkBurst1A_E) // TGEA
{
  ejectionVelocity = 15.0;
  vector = %LMF_sparkVector_E;
};

// spark 5A emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst5A_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  vector = %LMF_sparkVector_F;
};
// spark 5A emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst5A_E : LMF_sparkBurst1A_E) // TGEA
{
  vector = %LMF_sparkVector_F;
};

// spark 5B emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst5B_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  ejectionVelocity = 15.0;
  vector = %LMF_sparkVector_D;
};
// spark 5B emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst5B_E : LMF_sparkBurst1A_E) // TGEA
{
  ejectionVelocity = 15.0;
  vector = %LMF_sparkVector_D;
};

// spark 5C emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst5C_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  ejectionVelocity = 23.0;
  vector = %LMF_sparkVector_C;
};
// spark 5C emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst5C_E : LMF_sparkBurst1A_E) // TGEA
{
  ejectionVelocity = 23.0;
  vector = %LMF_sparkVector_C;
};

// spark 5D emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst5D_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  ejectionVelocity = 9.0;
  vector = %LMF_sparkVector_G;
};
// spark 5D emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst5D_E : LMF_sparkBurst1A_E) // TGEA
{
  ejectionVelocity = 9.0;
  vector = %LMF_sparkVector_G;
};

// spark 5E emitter, head
datablock afxParticleEmitterVectorData(LMF_sparkHeadBurst5E_E : LMF_sparkHeadBurst1A_E) // TGEA
{
  ejectionVelocity = 17.0;
  vector = %LMF_sparkVector_A;
};
// spark 5E emitter
datablock afxParticleEmitterVectorData(LMF_sparkBurst5E_E : LMF_sparkBurst1A_E) // TGEA
{
  ejectionVelocity = 17.0;
  vector = %LMF_sparkVector_A;
};

// variables used to randomize the lifetime of the sparks, and hence
//  their lengths:
%LFM_sparkLifetimeMult_A = 0.07;
%LFM_sparkLifetimeMult_B = 0.12;
%LFM_sparkLifetimeMult_C = 0.20;

// spark 1A
datablock afxEffectWrapperData(LMF_sparkBurst1A_EW)
{
  effect = LMF_sparkBurst1A_E;
  posConstraint = "#scene.CampFire";
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_A;
  delay    = 5.0+0.10;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset1_XM";
};
// spark 1A head
datablock afxEffectWrapperData(LMF_sparkHeadBurst1A_EW : LMF_sparkBurst1A_EW)
{
  effect = LMF_sparkHeadBurst1A_E;
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_A*0.05;
};

// spark 1B
datablock afxEffectWrapperData(LMF_sparkBurst1B_EW : LMF_sparkBurst1A_EW)
{
  effect = LMF_sparkBurst1B_E;
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_C;
  delay  = 5.0+0.15;
};
// spark 1B head
datablock afxEffectWrapperData(LMF_sparkHeadBurst1B_EW : LMF_sparkBurst1B_EW)
{
  effect = LMF_sparkHeadBurst1B_E;
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_C*0.05;
};

// spark 1C
datablock afxEffectWrapperData(LMF_sparkBurst1C_EW : LMF_sparkBurst1A_EW)
{
  effect = LMF_sparkBurst1C_E;
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_A;
  delay  = 5.0+0.19;
};
// spark 1C head
datablock afxEffectWrapperData(LMF_sparkHeadBurst1C_EW : LMF_sparkBurst1C_EW)
{
  effect = LMF_sparkHeadBurst1C_E;
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_A*0.05;
};

// spark 2A
datablock afxEffectWrapperData(LMF_sparkBurst2A_EW)
{
  effect = LMF_sparkBurst2A_E;
  posConstraint = "#scene.CampFire";
  lifetime = 0.45*0.6*%LFM_sparkLifetimeMult_B;
  delay    = 7.0+0.10;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset2_XM";
};
// spark 2A head
datablock afxEffectWrapperData(LMF_sparkHeadBurst2A_EW : LMF_sparkBurst2A_EW)
{
  effect = LMF_sparkHeadBurst2A_E;
  lifetime = 0.45*0.6*%LFM_sparkLifetimeMult_B*0.05;
};

// spark 2B
datablock afxEffectWrapperData(LMF_sparkBurst2B_EW : LMF_sparkBurst2A_EW)
{
  effect = LMF_sparkBurst2B_E;
  lifetime = 0.45*0.6*%LFM_sparkLifetimeMult_A;
  delay  = 7.0+0.11;
};
// spark 2B head
datablock afxEffectWrapperData(LMF_sparkHeadBurst2B_EW : LMF_sparkBurst2B_EW)
{
  effect = LMF_sparkHeadBurst2B_E;
  lifetime = 0.45*0.6*%LFM_sparkLifetimeMult_A*0.05;
};

// spark 3A
datablock afxEffectWrapperData(LMF_sparkBurst3A_EW)
{
  effect = LMF_sparkBurst3A_E;
  posConstraint = "#scene.CampFire";
  lifetime = 0.4*0.6*%LFM_sparkLifetimeMult_C;
  delay    = 7.5+0.06;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset3_XM";
};
// spark 3A head
datablock afxEffectWrapperData(LMF_sparkHeadBurst3A_EW : LMF_sparkBurst3A_EW)
{
  effect = LMF_sparkHeadBurst3A_E;
  lifetime = 0.4*0.6*%LFM_sparkLifetimeMult_C*0.05;
};

// spark 3B
datablock afxEffectWrapperData(LMF_sparkBurst3B_EW : LMF_sparkBurst3A_EW)
{
  effect = LMF_sparkBurst3B_E;
  lifetime = 0.4*0.6*%LFM_sparkLifetimeMult_A;
  delay  = 7.5+0.15;
};
// spark 3B head
datablock afxEffectWrapperData(LMF_sparkHeadBurst3B_EW : LMF_sparkBurst3B_EW)
{
  effect = LMF_sparkHeadBurst3B_E;
  lifetime = 0.4*0.6*%LFM_sparkLifetimeMult_A*0.05;
};

// spark 3C
datablock afxEffectWrapperData(LMF_sparkBurst3C_EW : LMF_sparkBurst3A_EW)
{
  effect = LMF_sparkBurst3C_E;
  lifetime = 0.4*0.6*%LFM_sparkLifetimeMult_B;
  delay  = 7.5+0.25;
};
// spark 3C head
datablock afxEffectWrapperData(LMF_sparkHeadBurst3C_EW : LMF_sparkBurst3C_EW)
{
  effect = LMF_sparkHeadBurst3C_E;
  lifetime = 0.4*0.6*%LFM_sparkLifetimeMult_B*0.05;
};

// spark 4A
datablock afxEffectWrapperData(LMF_sparkBurst4A_EW)
{
  effect = LMF_sparkBurst4A_E;
  posConstraint = "#scene.CampFire";
  lifetime = 0.50*0.6*%LFM_sparkLifetimeMult_A;
  delay    = 9.1+0.15;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset4_XM";
};
// spark 4A head
datablock afxEffectWrapperData(LMF_sparkHeadBurst4A_EW : LMF_sparkBurst4A_EW)
{
  effect = LMF_sparkHeadBurst4A_E;
  lifetime = 0.50*0.6*%LFM_sparkLifetimeMult_A*0.05;
};

// spark 4B
datablock afxEffectWrapperData(LMF_sparkBurst4B_EW : LMF_sparkBurst4A_EW)
{
  effect = LMF_sparkBurst4B_E;
  lifetime = 0.50*0.6*%LFM_sparkLifetimeMult_C;
  delay  = 9.1+0.18;
};
// spark 4B head
datablock afxEffectWrapperData(LMF_sparkHeadBurst4B_EW : LMF_sparkBurst4B_EW)
{
  effect = LMF_sparkHeadBurst4B_E;
  lifetime = 0.50*0.6*%LFM_sparkLifetimeMult_C*0.05;
};

// spark 5A
datablock afxEffectWrapperData(LMF_sparkBurst5A_EW)
{
  effect = LMF_sparkBurst5A_E;
  posConstraint = "#scene.CampFire";
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_A;
  delay    = 10.9+0.09;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset5_XM";
};
// spark 5A head
datablock afxEffectWrapperData(LMF_sparkHeadBurst5A_EW : LMF_sparkBurst5A_EW)
{
  effect = LMF_sparkHeadBurst5A_E;
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_A*0.05;
};

// spark 5B
datablock afxEffectWrapperData(LMF_sparkBurst5B_EW : LMF_sparkBurst5A_EW)
{
  effect = LMF_sparkBurst5B_E;
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_C;
  delay  = 10.9+0.10;
};
// spark 5B head
datablock afxEffectWrapperData(LMF_sparkHeadBurst5B_EW : LMF_sparkBurst5B_EW)
{
  effect = LMF_sparkHeadBurst5B_E;
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_C*0.05;
};

// spark 5C
datablock afxEffectWrapperData(LMF_sparkBurst5C_EW : LMF_sparkBurst5A_EW)
{
  effect = LMF_sparkBurst5C_E;
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_B;
  delay  = 10.9+0.15;
};
// spark 5C head
datablock afxEffectWrapperData(LMF_sparkHeadBurst5C_EW : LMF_sparkBurst5C_EW)
{
  effect = LMF_sparkHeadBurst5C_E;
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_B*0.05;
};

// spark 5D
datablock afxEffectWrapperData(LMF_sparkBurst5D_EW : LMF_sparkBurst5A_EW)
{
  effect = LMF_sparkBurst5D_E;
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_A;
  delay  = 10.9+0.17;
};
// spark 5D head
datablock afxEffectWrapperData(LMF_sparkHeadBurst5D_EW : LMF_sparkBurst5D_EW)
{
  effect = LMF_sparkHeadBurst5D_E;
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_A*0.05;
};

// spark 5E
datablock afxEffectWrapperData(LMF_sparkBurst5E_EW : LMF_sparkBurst5A_EW)
{
  effect = LMF_sparkBurst5E_E;
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_A;
  delay  = 10.9+0.25;
};
// spark 5E head
datablock afxEffectWrapperData(LMF_sparkHeadBurst5E_EW : LMF_sparkBurst5E_EW)
{
  effect = LMF_sparkHeadBurst5E_E;
  lifetime = 0.35*0.6*%LFM_sparkLifetimeMult_A*0.05;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SOUNDS

// hands fire //

datablock AudioProfile(LMF_HandsFire_Snd_CE)
{
   fileName = %mySpellDataPath @ "/LMF/sounds/LMF_hands_fire.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(LMF_HandsFire_LF_Snd_EW)
{
  effect = LMF_HandsFire_Snd_CE;
  constraint = "caster.Bip01 L Hand";
  lifetime = 3.583;
  delay = 0.4;
};
datablock afxEffectWrapperData(LMF_HandsFire_RT_Snd_EW)
{
  effect = LMF_HandsFire_Snd_CE;
  constraint = "caster.Bip01 R Hand";
  lifetime = 3.583;
  delay = 0.15;
};

// main fire // 

datablock AudioDescription(LMF_FireLoop_Snd_AD : SpellAudioLoop_AD)
{
  ReferenceDistance  = 10.0;
};
datablock AudioProfile(LMF_FireLoop_Snd_CE)
{
   fileName = %mySpellDataPath @ "/LMF/sounds/LMF_fire_loop.ogg";
   description = LMF_FireLoop_Snd_AD;
   preload = false;
};
datablock afxEffectWrapperData(LMF_FireLoop_Snd_EW)
{
  effect = LMF_FireLoop_Snd_CE;
  posConstraint = "#scene.CampFire";
  lifetime = 11.1;
  delay = 0.5;
  fadeInTime  = 2.0;
  fadeOutTime = 1.0;
};

// sparks //

datablock AudioProfile(LMF_Accent1_Snd_CE)
{
   fileName = %mySpellDataPath @ "/LMF/sounds/LMF_accent_1.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};

datablock AudioProfile(LMF_Accent2_Snd_CE)
{
   fileName = %mySpellDataPath @ "/LMF/sounds/LMF_accent_2.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};

datablock afxEffectWrapperData(LMF_Accent1_A_Snd_EW)
{
  effect = LMF_Accent1_Snd_CE;
  posConstraint = "#scene.CampFire";
  lifetime = 1.435;
  delay    = 5.0+0.10;
  scaleFactor = 0.8;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset1_XM";
};

datablock afxEffectWrapperData(LMF_Accent1_B_Snd_EW)
{
  effect = LMF_Accent1_Snd_CE;
  posConstraint = "#scene.CampFire";
  lifetime = 1.435;
  delay    = 7.0+0.10;
  scaleFactor = 0.6;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset2_XM";
};
datablock afxEffectWrapperData(LMF_Accent1_B2_Snd_EW : LMF_Accent1_B_Snd_EW)
{
  effect = LMF_Accent2_Snd_CE;
  posConstraint = "#scene.CampFire";
  lifetime = 1.584;
};

datablock afxEffectWrapperData(LMF_Accent1_C_Snd_EW)
{
  effect = LMF_Accent1_Snd_CE;
  posConstraint = "#scene.CampFire";
  lifetime = 1.435;
  delay    = 7.5+0.06;
  scaleFactor = 0.7;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset3_XM";
};

datablock afxEffectWrapperData(LMF_Accent1_D_Snd_EW)
{
  effect = LMF_Accent2_Snd_CE;
  posConstraint = "#scene.CampFire";
  lifetime = 1.584;
  delay    = 9.1+0.15;
  scaleFactor = 0.6;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset4_XM";
};

datablock afxEffectWrapperData(LMF_Accent1_E_Snd_EW)
{
  effect = LMF_Accent1_Snd_CE;
  posConstraint = "#scene.CampFire";
  lifetime = 1.435;
  delay    = 10.9+0.09;
  scaleFactor = 0.7;
  xfmModifiers[0] = "LMF_Fire_Offset_Center_XM";
  xfmModifiers[1] = "LMF_FireBurst_Offset5_XM";
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// LIGHT MY FIRE SPELL
//

datablock afxMagicSpellData(LightMyFireSpell)
{
  castingDur = 0.7;
  lingerDur = 9.0;
  numLingerLoops = $AFX::INFINITE_REPEATS;
  execOnNewClients = true;

    // spellcaster animation //
  addCastingEffect = LMF_SummonClip_EW;
    // casting hand fire particles //
  addCastingEffect = LMF_CastingFire_lf_hand_EW;
  addCastingEffect = LMF_CastingFire_rt_hand_EW;
    // casting hand fire lights //
  addCastingEffect = $LMF_CastingFireLight_lf_hand_1;
  addCastingEffect = $LMF_CastingFireLight_lf_hand_2;
  addCastingEffect = $LMF_CastingFireLight_lf_hand_3;
  addCastingEffect = $LMF_CastingFireLight_rt_hand_1;
  addCastingEffect = $LMF_CastingFireLight_rt_hand_2;
  addCastingEffect = $LMF_CastingFireLight_rt_hand_3;
    // casting hand fire sounds //
  addCastingEffect = LMF_HandsFire_LF_Snd_EW;
  addCastingEffect = LMF_HandsFire_RT_Snd_EW;

  addCastingEffect = LMF_LightBeam_EW;

    // campfire particles //
  addLingerEffect = LMF_fire1_EW;
  addLingerEffect = LMF_fire2_EW;
  addLingerEffect = LMF_fire3_EW;
  addLingerEffect = LMF_fire4_EW;
  addLingerEffect = LMF_fire5_EW;
  addLingerEffect = LMF_fire6_EW;
  addLingerEffect = LMF_fire7_EW;
  addLingerEffect = LMF_fire8_EW;
  addLingerEffect = LMF_fire9_EW;
  addLingerEffect = LMF_fire10_EW;
    // fire sound //
  addLingerEffect = LMF_FireLoop_Snd_EW;
    // campfire lights //
  addLingerEffect = LMF_fireLight1_EW;
  addLingerEffect = LMF_fireLight2_EW;
  addLingerEffect = LMF_fireLight3_EW;
  addLingerEffect = LMF_fireLight4_EW;
  addLingerEffect = LMF_fireLight5_EW;
  addLingerEffect = LMF_fireLight6_EW;
  addLingerEffect = LMF_fireLight7_EW;
  addLingerEffect = LMF_fireLight8_EW;
  addLingerEffect = LMF_fireLight9_EW;
  addLingerEffect = LMF_fireLight10_EW;
  addLingerEffect = $LMF_FireShadowLight_1;
  addLingerEffect = $LMF_FireShadowLight_2;
    // campfire glow zodiacs //
  addLingerEffect = LMF_FireGlowZode1_EW;
  addLingerEffect = LMF_FireGlowZode2_EW;
    // campfire embers //
  addLingerEffect = LMF_embersUp_EW;
  addLingerEffect = LMF_embersSpurt_EW;
    // campfire burst particles //
  addLingerEffect = LMF_fireBurst1_EW;
  addLingerEffect = LMF_fireBurst2_EW;
  addLingerEffect = LMF_fireBurst3_EW;
  addLingerEffect = LMF_fireBurst4_EW;
  addLingerEffect = LMF_fireBurst5_EW;
    // campfire burst lights //
  addLingerEffect = LMF_fireBurstLight1_EW;
  addLingerEffect = LMF_fireBurstLight2_EW;
  addLingerEffect = LMF_fireBurstLight3_EW;
  addLingerEffect = LMF_fireBurstLight4_EW;
  addLingerEffect = LMF_fireBurstLight5_EW;
    // campfire burst embers //
  addLingerEffect = LMF_embersBurst1_EW;
  addLingerEffect = LMF_embersBurst2_EW;
  addLingerEffect = LMF_embersBurst3_EW;
  addLingerEffect = LMF_embersBurst4_EW;
  addLingerEffect = LMF_embersBurst5_EW;
    // campfire burst sparks //
  addLingerEffect = LMF_sparkBurst1A_EW;
  addLingerEffect = LMF_sparkHeadBurst1A_EW;
  addLingerEffect = LMF_sparkBurst1B_EW;
  addLingerEffect = LMF_sparkHeadBurst1B_EW;
  addLingerEffect = LMF_sparkBurst1C_EW;
  addLingerEffect = LMF_sparkHeadBurst1C_EW;
  addLingerEffect = LMF_sparkBurst2A_EW;
  addLingerEffect = LMF_sparkHeadBurst2A_EW;
  addLingerEffect = LMF_sparkBurst2B_EW;
  addLingerEffect = LMF_sparkHeadBurst2B_EW;
  addLingerEffect = LMF_sparkBurst3A_EW;
  addLingerEffect = LMF_sparkHeadBurst3A_EW;
  addLingerEffect = LMF_sparkBurst3B_EW;
  addLingerEffect = LMF_sparkHeadBurst3B_EW;
  addLingerEffect = LMF_sparkBurst3C_EW;
  addLingerEffect = LMF_sparkHeadBurst3C_EW;
  addLingerEffect = LMF_sparkBurst4A_EW;
  addLingerEffect = LMF_sparkHeadBurst4A_EW;
  addLingerEffect = LMF_sparkBurst4B_EW;
  addLingerEffect = LMF_sparkHeadBurst4B_EW;
  addLingerEffect = LMF_sparkBurst5A_EW;
  addLingerEffect = LMF_sparkHeadBurst5A_EW;
  addLingerEffect = LMF_sparkBurst5B_EW;
  addLingerEffect = LMF_sparkHeadBurst5B_EW;
  addLingerEffect = LMF_sparkBurst5C_EW;
  addLingerEffect = LMF_sparkHeadBurst5C_EW;
  addLingerEffect = LMF_sparkBurst5D_EW;
  addLingerEffect = LMF_sparkHeadBurst5D_EW;
  addLingerEffect = LMF_sparkBurst5E_EW;
  addLingerEffect = LMF_sparkHeadBurst5E_EW;

  addLingerEffect = LMF_Accent1_A_Snd_EW;
  addLingerEffect = LMF_Accent1_B_Snd_EW;
  addLingerEffect = LMF_Accent1_B2_Snd_EW;
  addLingerEffect = LMF_Accent1_C_Snd_EW;
  addLingerEffect = LMF_Accent1_D_Snd_EW;
  addLingerEffect = LMF_Accent1_E_Snd_EW;
};
datablock afxRPGMagicSpellData(LightMyFireSpell_RPG)
{
  name = "Light My Fire";
  desc = "Try to set the NIGHT on fire. Ignite a roaring bonfire." @
         "\n(Cast again to douse.)" @
         "\n" @
         "\nspell design: Matthew Durante" @
         "\nsound effects: Dave Schroeder" @ 
         "\nspell concept: Jeff Faust";
  sourcePack = "Core Tech";
  iconBitmap = %mySpellDataPath @ "/LMF/icons/lmf";
  target = "nothing";
  manaCost = 10;
  castingDur = LightMyFireSpell.castingDur;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// scripting

// if spell is active, kill it instead of casting it
function LightMyFireSpell::readyToCast(%this, %caster, %target)
{
  if (!Parent::readyToCast(%this, %caster, %target))
    return false;

  if (!isObject(CampFire))
    return false;

  // if flames are active they are attached to the CampFire
  // object via the flames field.
  if (isObject(CampFire.flames))
  {
    CampFire.flames.interrupt();
    return false;
  }

  return true;
}

// stash the new spell instance in a field of the CampFire object
function LightMyFireSpell::onActivate(%this, %spell, %caster, %target)
{
  Parent::onActivate(%this, %spell, %caster, %target);
  CampFire.flames = %spell;
}

// clear the spell instance reference 
function LightMyFireSpell::onDeactivate(%this, %spell, %caster)
{
  Parent::onDeactivate(%this, %spell, %caster);
  CampFire.flames = "";
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
  LightMyFireSpell.scriptFile = $afxAutoloadScriptFile;
  LightMyFireSpell.spellDataPath = %mySpellDataPath;
  if (isFunction(addDemoSpellbookSpell))
    addDemoSpellbookSpell(LightMyFireSpell, LightMyFireSpell_RPG);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

