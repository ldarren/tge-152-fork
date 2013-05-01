
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// MAPLELEAF FRAG SPELL
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
$spell_reload = isObject(MapleleafFragSpell);
if ($spell_reload)
{
  // mark datablocks so we can detect which are reloaded this script
  markDataBlocks();
  // reset data path from previously saved value
  %mySpellDataPath = MapleleafFragSpell.spellDataPath;
  MapleleafFragSpell.reset();
}
else
{
  // set data path from default plus containing folder name
  %mySpellDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

$isTGEA = (afxGetEngine() $= "TGEA");
$BrightLighting_mask = 0x800000; // BIT(23); 

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CASTING ZODIAC

//
// The main casting zodiac is formed by two zodiacs plus a white
// reveal glow seen when the casting first starts. Additive blends
// are used to suggest projected light.
//

// this is the white reveal glow
datablock afxZodiacData(MLF_ZodeReveal_CE)
{  
  texture = %mySpellDataPath @ "/MLF/zodiacs/MLF_caster_reveal";
  radius = 3.0;
  startAngle = 7.5; //0.0+7.5
  rotationRate = -30.0;  
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(MLF_ZodeReveal_EW)
{
  effect = MLF_ZodeReveal_CE;
  posConstraint = caster;
  delay = 0.0;
  lifetime = 0.75;
  fadeInTime = 0.5;
  fadeOutTime = 0.25;
};

%MLF_MainZodeRevealLight_intensity = 2.5;

datablock afxXM_LocalOffsetData(MLF_MainZodeRevealLight_offset_XM)
{
  localOffset = "0 2 -4";
};
datablock afxXM_SpinData(MLF_MainZodeRevealLight_spin1_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 0;
  spinRate = -30;
};
datablock afxXM_SpinData(MLF_MainZodeRevealLight_spin2_XM : MLF_MainZodeRevealLight_spin1_XM)
{
  spinAngle = 120;
};
datablock afxXM_SpinData(MLF_MainZodeRevealLight_spin3_XM : MLF_MainZodeRevealLight_spin1_XM)
{
  spinAngle = 240;
};

// main zode reveal light
datablock afxLightData(MLF_MainZodeRevealLight_CE) // AFX-SPOTLIGHT
{
  type = "Spot";
  radius = ($isTGEA) ? 10 : 5;
  direction = "0 -0.313 0.95";
  sgCastsShadows = false;

  color = 1.0*%MLF_MainZodeRevealLight_intensity SPC
          1.0*%MLF_MainZodeRevealLight_intensity SPC
          1.0*%MLF_MainZodeRevealLight_intensity;

  lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF; // TGEA (ignored by TGE)
  sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Near Linear";
};

datablock afxEffectWrapperData(MLF_MainZodeRevealLight_1_EW : MLF_ZodeReveal_EW)
{
  effect = MLF_MainZodeRevealLight_CE;
  xfmModifiers[0] = MLF_MainZodeRevealLight_spin1_XM;
  xfmModifiers[1] = MLF_MainZodeRevealLight_offset_XM;
};
datablock afxEffectWrapperData(MLF_MainZodeRevealLight_2_EW : MLF_ZodeReveal_EW)
{
  effect = MLF_MainZodeRevealLight_CE;
  xfmModifiers[0] = MLF_MainZodeRevealLight_spin2_XM;
  xfmModifiers[1] = MLF_MainZodeRevealLight_offset_XM;
};
datablock afxEffectWrapperData(MLF_MainZodeRevealLight_3_EW : MLF_ZodeReveal_EW)
{
  effect = MLF_MainZodeRevealLight_CE;
  xfmModifiers[0] = MLF_MainZodeRevealLight_spin3_XM;
  xfmModifiers[1] = MLF_MainZodeRevealLight_offset_XM;
};

// the main casting zodiac, a yellowish-green sun-like
// shape.
datablock afxZodiacData(MLF_Zode1_CE)
{  
  texture = %mySpellDataPath @ "/MLF/zodiacs/MLF_caster";
  radius = 3.0;
  startAngle = 0.0;
  rotationRate = -30.0;
  color = "1.0 1.0 1.0 0.9";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(MLF_ZodeEffect1_EW)
{
  effect = MLF_Zode1_CE;
  posConstraint = caster;

  delay = 0.25;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 5.2;
};

// Main Zode Underglow
//  Glowing zodiacs blended additively tend to be washed-out atop
//  light groundplanes, as is the case with TLK and the sand-colored
//  ground in the demo.  To make them visually "pop" the ground must
//  be darkened, and this is done here.  The "underglow" zodiac is
//  a copy of the glow zodiac that is blended normally.  Because the
//  glow zodiacs have halos extending beyond their opaque regions
//  that blend with black, the ground is subtly darkened.  As the
//  glow is layered atop it -- it pops!  Increasing the color value
//  increases the effect.
datablock afxZodiacData(MLF_Zode1_TLKunderglow_CE : MLF_Zode1_CE)
{
  color = "0.65 0.65 0.65 0.65";
  blend = normal;
};
//
datablock afxEffectWrapperData(MLF_ZodeEffect1_TLKunderglow_EW : MLF_ZodeEffect1_EW)
{
  effect = MLF_Zode1_TLKunderglow_CE;
  execConditions = $BrightLighting_mask;
};

// a green detail zodiac. inner ring of runes, outer ring
// of skulls
datablock afxZodiacData(MLF_Zode2_CE)
{  
  texture = %mySpellDataPath @ "/MLF/zodiacs/zode_text";
  radius = 3.0;
  startAngle = 0.0;
  rotationRate = 20.0; //60
  color = "0.0 1.0 0.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(MLF_ZodeEffect2_EW)
{
  effect = MLF_Zode2_CE;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 5.2;
};

// Runes & Skulls Zode Underglow
//  Here the zode is made black but only slightly opaque to subtly
//  darken the ground, making the additive glow zode appear more
//  saturated.
datablock afxZodiacData(MLF_Zode2_TLKunderglow_CE : MLF_Zode2_CE)
{
  color = "0 0 0 0.25";
  blend = normal;
};
//
datablock afxEffectWrapperData(MLF_ZodeEffect2_TLKunderglow_EW : MLF_ZodeEffect2_EW)
{
  effect = MLF_Zode2_TLKunderglow_CE;
  execConditions = $BrightLighting_mask;
};

if ($isTGEA)
{
  %MLF_CastingZodeLight_LMODELS_intensity = 25.0;
  %MLF_CastingZodeLight_LTERRAIN_intensity = 2.5;

  // this offset defines the lights distance beneath the terrain
  datablock afxXM_LocalOffsetData(MLF_CastingZodeLight_offset_XM)
  {
    localOffset = "0 0 -2";
  };

  datablock afxLightData(MLF_CastingZodeLight_LMODELS_CE)
  {
    type = "Point";
    radius = 3; //5;
    sgCastsShadows = false;
    sgDoubleSidedAmbient = true;

    sgLightingModelName = "Inverse Square Fast Falloff";

    color = 1.0*%MLF_CastingZodeLight_LMODELS_intensity SPC
      1.0*%MLF_CastingZodeLight_LMODELS_intensity SPC
      0.0*%MLF_CastingZodeLight_LMODELS_intensity;

    lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF;
  };
  datablock afxLightData(MLF_CastingZodeLight_LTERRAIN_CE : MLF_CastingZodeLight_LMODELS_CE)
  {
    radius = 2.25; //2.5;
    sgLightingModelName = "Inverse Square Fast Falloff";
    color = 1.0*%MLF_CastingZodeLight_LTERRAIN_intensity SPC
      0.5*%MLF_CastingZodeLight_LTERRAIN_intensity SPC
      0.0*%MLF_CastingZodeLight_LTERRAIN_intensity;

    lightIlluminationMask = $AFX::ILLUM_TERRAIN | $AFX::ILLUM_ATLAS;
  };

  datablock afxMultiLightData(MLF_CastingZodeLight_Multi_CE)
  {
    lights[0] = MLF_CastingZodeLight_LMODELS_CE;
    lights[1] = MLF_CastingZodeLight_LTERRAIN_CE;
  };
  datablock afxEffectWrapperData(MLF_CastingZodeLight_Multi_EW : MLF_ZodeEffect1_EW)
  {
    effect = MLF_CastingZodeLight_Multi_CE;
    xfmModifiers[0] = MLF_CastingZodeLight_offset_XM;
  };

  $MLF_CastingZodeLight = MLF_CastingZodeLight_Multi_EW;
}
else
{
  %MLF_CastingZodeLight_intensity = 2.5;

  // this offset defines the lights distance beneath the terrain
  datablock afxXM_LocalOffsetData(MLF_CastingZodeLight_offset_XM)
  {
    localOffset = "0 0 -4";
  };

  datablock afxLightData(MLF_CastingZodeLight_CE)
  {
    type = "Point";
    radius = 5;
    sgCastsShadows = false;
    sgDoubleSidedAmbient = true;

    sgLightingModelName = "Near Linear";
    color = 1.0*%MLF_CastingZodeLight_intensity SPC
      1.0*%MLF_CastingZodeLight_intensity SPC
      0.0*%MLF_CastingZodeLight_intensity;
  };
  //
  datablock afxEffectWrapperData(MLF_CastingZodeLight_EW : MLF_ZodeEffect1_EW)
  {
    effect = MLF_CastingZodeLight_CE;
    xfmModifiers[0] = "MLF_CastingZodeLight_offset_XM";
  };

  $MLF_CastingZodeLight = MLF_CastingZodeLight_EW;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SPELLCASTER ANIMATION

//
// This clip selects the main animation sequence for casting this
// spell. Like a Whirling Dervish, the spellcaster spins his body,
// impelling magically summoned leaves to swirl likewise into a
// raging tornado. As the tornado strengthens, the spellcaster loses
// control and is just along for the ride as the winds lift him aloft
// and then unceremoniously drop him back to earth.
//

datablock afxAnimClipData(MLF_Casting_Clip_CE)
{
  clipName = "mlf";
  ignoreCorpse = true;
  rate = 1.0;
};
//
datablock afxEffectWrapperData(MLF_Casting_Clip_EW)
{
  effect = MLF_Casting_Clip_CE;
  constraint = "caster";
  lifetime = 9.7;
  delay = 0.0;
};

datablock afxAnimLockData(MLF_AnimLock_CE)
{
  priority = 0;
};
//
datablock afxEffectWrapperData(MLF_AnimLock_EW)
{
  effect = MLF_AnimLock_CE;
  constraint = "caster";
  lifetime = 4.5;
  delay = 5.0;
};



//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// MAGIC HAND SPARKLERS

//
// While casting, the spellcaster's hands emit bright purple sparkles
// that progressivly increase in density with each of his spin moves.
// 
// Density changes come from ParticleData parameters. Aside from
// using different particles, the emitters are identical.
//

//
// bright purple sparkles LITE
//
datablock ParticleData(MLF_MagicA_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/MLF_magicA";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 250;
   lifetimeVarianceMS   = 75;
   useInvAlpha          = false;
   spinRandomMin        = -360.0;
   spinRandomMax        = 360.0;
   colors[0]            = "1.0 1.0 1.0 1.0";
   colors[1]            = "1.0 1.0 1.0 0.0";
   sizes[0]             = 0.35;
   sizes[1]             = 0.35;
   times[0]             = 0.9;
   times[1]             = 1.0;

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // magicA
   textureCoords[0]     = "0.5  0.75";
   textureCoords[1]     = "0.5  1.0";
   textureCoords[2]     = "0.75 1.0";
   textureCoords[3]     = "0.75 0.75";
};
//
datablock ParticleEmitterData(MLF_MagicA_E)
{
  ejectionOffset        = 0.02;
  ejectionPeriodMS      = 10;
  periodVarianceMS      = 0;
  ejectionVelocity      = 0.8;
  velocityVariance      = 0.2;
  thetaMin              = 0.0;
  thetaMax              = 180.0;
  particles             = MLF_MagicA_P;
};

datablock afxEffectWrapperData(MLF_MagicSprayA_lf_hand_EW)
{
  effect = MLF_MagicA_E;
  constraint = "caster.Bip01 L Hand";
  lifetime = 5.0;
};
datablock afxEffectWrapperData(MLF_MagicSprayA_rt_hand_EW : MLF_MagicSprayA_lf_hand_EW)
{
  constraint = "caster.Bip01 R Hand";
};

//
// bright purple sparkles MEDIUM
//
datablock ParticleData(MLF_MagicB_P : MLF_MagicA_P)
{
   // TGE textureName = %mySpellDataPath @ "/MLF/particles/MLF_magicB";
   sizes[0] = 0.6;
   sizes[1] = 0.6;

   textureCoords[0]     = "0.0 0.5";
   textureCoords[1]     = "0.0 1.0";
   textureCoords[2]     = "0.5 1.0";
   textureCoords[3]     = "0.5 0.5";
};
//
datablock ParticleEmitterData(MLF_MagicB_E : MLF_MagicA_E)
{
  particles = MLF_MagicB_P;
};

datablock afxEffectWrapperData(MLF_MagicSprayB_lf_hand_EW)
{
  effect = MLF_MagicB_E;
  constraint = "caster.Bip01 L Hand";
  lifetime = 0.5;
  delay = 1.0;
};
datablock afxEffectWrapperData(MLF_MagicSprayB_rt_hand_EW : MLF_MagicSprayB_lf_hand_EW)
{
  constraint = "caster.Bip01 R Hand";
};

//
// bright purple sparkles HEAVY
//
datablock ParticleData(MLF_MagicC_P : MLF_MagicB_P)
{
   sizes[0] = 0.75;
   sizes[1] = 0.75;
};
//
datablock ParticleEmitterData(MLF_MagicC_E : MLF_MagicA_E)
{
  particles = MLF_MagicC_P;
};

datablock afxEffectWrapperData(MLF_MagicSprayC_lf_hand_EW)
{
  effect = MLF_MagicC_E;
  constraint = "caster.Bip01 L Hand";
  lifetime = 0.67;
  delay = 2.5;
};
datablock afxEffectWrapperData(MLF_MagicSprayC_rt_hand_EW : MLF_MagicSprayC_lf_hand_EW)
{
  constraint = "caster.Bip01 R Hand";
};

//
// bright purple sparkles HEAVIEST
//
datablock ParticleData(MLF_MagicD_P : MLF_MagicB_P)
{
   sizes[0] = 1.0;
   sizes[1] = 1.0;
};
//
datablock ParticleEmitterData(MLF_MagicD_E : MLF_MagicA_E)
{
  particles = MLF_MagicD_P;
};

datablock afxEffectWrapperData(MLF_MagicSprayD_lf_hand_EW)
{
  effect = MLF_MagicD_E;
  constraint = "caster.Bip01 L Hand";
  lifetime = 0.66;
  delay = 4.33;
};
datablock afxEffectWrapperData(MLF_MagicSprayD_rt_hand_EW : MLF_MagicSprayD_lf_hand_EW)
{
  constraint = "caster.Bip01 R Hand";
};

//
// purple lights in each hand represent light cast 
// by the purple sparkles. these have a long fade-in
// time that gradually builds up the brightness.
//
%MLF_MagicLight_intensity = ($isTGEA) ? 3.0 : 1.0;
datablock afxLightData(MLF_MagicLight_CE)
{
  type = "Point";  
  radius = ($isTGEA) ? 2 : 4;
  color  = 0.25*%MLF_MagicLight_intensity SPC
           0.0 *%MLF_MagicLight_intensity SPC
           0.5 *%MLF_MagicLight_intensity;
  sgLightingModelName = ($isTGEA) ? "Inverse Square Fast Falloff" : "Near Linear";
};

datablock afxEffectWrapperData(MLF_MagicLight_lf_hand_EW)
{
  effect = MLF_MagicLight_CE;
  constraint = "caster.Bip01 L Hand"; 
  delay    = 0.0;
  lifetime = 5.0;
  fadeInTime  = 4.33;
  fadeOutTime = 0.3;
};
datablock afxEffectWrapperData(MLF_MagicLight_rt_hand_EW : MLF_MagicLight_lf_hand_EW)
{
  constraint = "caster.Bip01 R Hand"; 
  lifetime = 6.2;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// LEAF PILES
//

//
// When the spellcaster makes one of his leaping spin moves,
// a circular pile of leaves will appear when he lands. The
// overall leaf pile builds up in three stages then fades
// after the tornado forms.
//

//
// the 1st leaf pile stage
//

// reveal
datablock afxZodiacData(MLF_LeafZodeReveal_A_CE)
{  
  texture = %mySpellDataPath @ "/MLF/zodiacs/MLF_caster_leaves_reveal_A";
  radius = 4.3;
  startAngle = 0.0;
  rotationRate = 0.0;
  color = "0.7 0.35 0.7 0.7"; //"0.5 0.25 0.5 0.5";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(MLF_LeafZodeReveal_A_EW)
{
  effect = MLF_LeafZodeReveal_A_CE;
  posConstraint = caster;
  delay = 1.67;
  lifetime = 0.5;
  fadeInTime = 0.25;
  fadeOutTime = 0.25;
};

// static
datablock afxZodiacData(MLF_LeafZode_A_CE : MLF_LeafZodeReveal_A_CE)
{  
  texture = %mySpellDataPath @ "/MLF/zodiacs/MLF_caster_leaves_A";
  color = "1.0 1.0 1.0 0.6";
};
//
datablock afxEffectWrapperData(MLF_LeafZode_A_EW)
{
  effect = MLF_LeafZode_A_CE;
  posConstraint = caster;
  delay = 1.79; //1.67+.12
  lifetime = 5.36;
  fadeInTime = 0.25;
  fadeOutTime = 0.25;
};

// underglow
datablock afxZodiacData(MLF_LeafZode_A_TLKunderglow_CE : MLF_LeafZode_A_CE)
{
  color = "0.4 0.4 0.4 0.4";
  blend = normal;
};
//
datablock afxEffectWrapperData(MLF_LeafZode_A_TLKunderglow_EW : MLF_LeafZode_A_EW)
{
  effect = MLF_LeafZode_A_TLKunderglow_CE;
  execConditions = $BrightLighting_mask;
};

// disappear
datablock afxZodiacData(MLF_LeafZodeDisappear_A_CE : MLF_LeafZodeReveal_A_CE)
{  
  texture = %mySpellDataPath @ "/MLF/zodiacs/MLF_caster_leaves_reveal_A";
  color = "0.5 0.25 0.5 0.5";
};
//
datablock afxEffectWrapperData(MLF_LeafZodeDisappear_A_EW)
{
  effect = MLF_LeafZodeDisappear_A_CE;
  posConstraint = caster;
  delay = 6.9; //6.2+1.2-0.5
  lifetime = 0.5;
  fadeInTime = 0.25;
  fadeOutTime = 0.25;
};

//
// the 2nd leaf pile stage
//

// reveal -- this reveal zode shared with disappear effect
datablock afxZodiacData(MLF_LeafZodeReveal_B_CE)
{  
  texture = %mySpellDataPath @ "/MLF/zodiacs/MLF_caster_leaves_reveal_B";
  radius = 4.3;
  startAngle = 0.0;
  rotationRate = 0.0;
  color = "0.6 0.3 0.6 0.6";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(MLF_LeafZodeReveal_B_EW)
{
  effect = MLF_LeafZodeReveal_B_CE;
  posConstraint = caster;
  delay = 3.33;
  lifetime = 0.5;
  fadeInTime = 0.25;
  fadeOutTime = 0.25;
};

// static
datablock afxZodiacData(MLF_LeafZode_B_CE : MLF_LeafZodeReveal_B_CE)
{  
  texture = %mySpellDataPath @ "/MLF/zodiacs/MLF_caster_leaves_B";
  color = "1.0 1.0 1.0 0.7";
};
//
datablock afxEffectWrapperData(MLF_LeafZode_B_EW)
{
  effect = MLF_LeafZode_B_CE;
  posConstraint = caster;
  delay = 3.45; //3.33+.12
  fadeInTime = 0.25;
  fadeOutTime = 0.25;
  lifetime = 3.4;
};

// underglow
datablock afxZodiacData(MLF_LeafZode_B_TLKunderglow_CE : MLF_LeafZode_B_CE)
{
  color = "0.4 0.4 0.4 0.4";
  blend = normal;
};
//
datablock afxEffectWrapperData(MLF_LeafZode_B_TLKunderglow_EW : MLF_LeafZode_B_EW)
{
  effect = MLF_LeafZode_B_TLKunderglow_CE;
  execConditions = $BrightLighting_mask;
};

// disappear
datablock afxEffectWrapperData(MLF_LeafZodeDisappear_B_EW)
{
  effect = MLF_LeafZodeReveal_B_CE;
  posConstraint = caster;
  delay = 6.6; //6.2+0.9-0.5
  lifetime = 0.5;
  fadeInTime = 0.25;
  fadeOutTime = 0.25;
};

//
// the 3rd leaf pile stage
//

// reveal
datablock afxZodiacData(MLF_LeafZodeReveal_C_CE)
{  
  texture = %mySpellDataPath @ "/MLF/zodiacs/MLF_caster_leaves_reveal_C";
  radius = 4.3;
  startAngle = 0.0;
  rotationRate = 0.0;
  color = "0.5 0.25 0.5 0.5"; //"0.7 0.35 0.7 0.7";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(MLF_LeafZodeReveal_C_EW)
{
  effect = MLF_LeafZodeReveal_C_CE;
  posConstraint = caster;
  delay = 5.6;
  lifetime = 0.5;
  fadeInTime = 0.25;
  fadeOutTime = 0.25;
};

// static
datablock afxZodiacData(MLF_LeafZode_C_CE : MLF_LeafZodeReveal_C_CE)
{  
  texture = %mySpellDataPath @ "/MLF/zodiacs/MLF_caster_leaves_C";
  color = "1.0 1.0 1.0 0.85";
};
//
datablock afxEffectWrapperData(MLF_LeafZode_C_EW)
{
  effect = MLF_LeafZode_C_CE;
  posConstraint = caster;
  delay = 5.72; //5.6+.12
  fadeInTime = 0.25;
  fadeOutTime = 0.25;
  lifetime = 0.83;
};

// underglow
datablock afxZodiacData(MLF_LeafZode_C_TLKunderglow_CE : MLF_LeafZode_C_CE)
{
  color = "0.4 0.4 0.4 0.4";
  blend = normal;
};
//
datablock afxEffectWrapperData(MLF_LeafZode_C_TLKunderglow_EW : MLF_LeafZode_C_EW)
{
  effect = MLF_LeafZode_C_TLKunderglow_CE;
  execConditions = $BrightLighting_mask;
};

// disappear
datablock afxZodiacData(MLF_LeafZodeDisappear_C_CE : MLF_LeafZodeReveal_C_CE)
{  
  texture = %mySpellDataPath @ "/MLF/zodiacs/MLF_caster_leaves_reveal_C";
  color = "0.7 0.35 0.7 0.7";
};
//
datablock afxEffectWrapperData(MLF_LeafZodeDisappear_C_EW)
{
  effect = MLF_LeafZodeDisappear_C_CE;
  posConstraint = caster;
  delay = 6.3; //6.2+0.6-0.5
  lifetime = 0.5;
  fadeInTime = 0.25;
  fadeOutTime = 0.25;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SPARKLE LEAF PARTICLES
//

// 
// This mix of leaf and sparkle particles is shared by a number
// of effects.
//

datablock ParticleData(MLF_LeafA_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/MLF_leafA";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 800;
   lifetimeVarianceMS   = 200;
   useInvAlpha          = false;
   spinRandomMin        = -360.0;
   spinRandomMax        = 360.0;
   colors[0]            = "1.0 1.0 1.0 0.0";
   colors[1]            = "1.0 1.0 1.0 0.6";
   colors[2]            = "1.0 1.0 1.0 0.6";
   colors[3]            = "1.0 1.0 1.0 0.0";
   sizes[0]             = 0.3;
   sizes[1]             = 0.3;
   sizes[2]             = 0.3;
   sizes[3]             = 0.3;
   times[0]             = 0.0;
   times[1]             = 0.3;
   times[2]             = 0.7;
   times[3]             = 1.0;

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // leafA
   textureCoords[0]     = "0.0 0.0";
   textureCoords[1]     = "0.0 0.5";
   textureCoords[2]     = "0.5 0.5";
   textureCoords[3]     = "0.5 0.0";
};
datablock ParticleData(MLF_Sparkle_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/sparkle";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.0;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 600;
   lifetimeVarianceMS   = 200;
   useInvAlpha          = false;
   spinRandomMin        = 0.0;
   spinRandomMax        = 0.0;
   colors[0]            = "1.0 1.0 1.0 1.0";
   colors[1]            = "0.5 1.0 0.5 1.0";
   colors[2]            = "0.0 1.0 0.0 1.0";
   sizes[0]             = 0.3;
   sizes[1]             = 0.2;
   sizes[2]             = 0.05;
   times[0]             = 0.0;
   times[1]             = 0.3;
   times[2]             = 1.0;

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // sparkle
   textureCoords[0]     = "0.75 0.75";
   textureCoords[1]     = "0.75 1.0";
   textureCoords[2]     = "1.0  1.0";
   textureCoords[3]     = "1.0  0.75";
};

datablock ParticleEmitterData(MLF_LeafA_E)
{
  ejectionOffset        = 0.02;
  ejectionPeriodMS      = 10;
  periodVarianceMS      = 0;
  ejectionVelocity      = 0.8;
  velocityVariance      = 0.2;
  thetaMin              = 0.0;
  thetaMax              = 180.0;
  particles             = "MLF_LeafA_P MLF_Sparkle_P";
};

datablock ParticleEmitterData(MLF_LeafB_E : MLF_LeafA_E)
{
  ejectionPeriodMS      = 5;
  periodVarianceMS      = 0;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SMALL LEAF VORTEX
//

//
// The spellcaster's initial spin moves generate a small swirling
// leaf vortex -- a preliminary tornado.
//
// The A funnels begin during the first spin move and continue,
// overlapping the B funnels which begin during the second spin
// move. The overall effect is a build up of swirling leaves.
//
// Spin and offset parameters are carefully chosen so that the
// emitters trace out a nice shape. 
//

//
// the A vortex funnels
//
datablock afxXM_SpinData(MLF_LeafFunnelA_1_spin_XM)
{
  spinAxis = "0.2 0.0 1.0";
  spinRate = 360;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelA_1_offset_XM)
{
  localOffset = "1.8 0.0 1.2";
};
datablock afxEffectWrapperData(MLF_LeafFunnelA_1_EW)
{
  effect = MLF_LeafA_E;
  posConstraint = caster;
  lifetime = 3.5;//5.2;
  delay = 1.0;
  xfmModifiers[0] = "MLF_LeafFunnelA_1_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelA_1_offset_XM";
};

datablock afxXM_SpinData(MLF_LeafFunnelA_2_spin_XM)
{
  spinAxis = "-0.2 0.15 1.0";
  spinRate = -360;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelA_2_offset_XM)
{
  localOffset = "-1.5 0.0 1.6";
};
datablock afxEffectWrapperData(MLF_LeafFunnelA_2_EW)
{
  effect = MLF_LeafA_E;
  posConstraint = caster;
  lifetime = 3.4;//5.1;
  delay = 1.1;
  xfmModifiers[0] = "MLF_LeafFunnelA_2_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelA_2_offset_XM";
};

datablock afxXM_SpinData(MLF_LeafFunnelA_3_spin_XM)
{
  spinAxis = "-0.1 -0.3 1.0";
  spinRate = 510;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelA_3_offset_XM)
{
  localOffset = "1.6 0.0 2.0";
};
datablock afxEffectWrapperData(MLF_LeafFunnelA_3_EW)
{
  effect = MLF_LeafA_E;
  posConstraint = caster;
  lifetime = 3.3;//5.0;
  delay = 1.2;
  xfmModifiers[0] = "MLF_LeafFunnelA_3_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelA_3_offset_XM";
};

//
// the B vortex funnels
//
datablock afxXM_SpinData(MLF_LeafFunnelB_1_spin_XM)
{
  spinAxis = ".32 -0.3 1.0";
  spinRate = 400;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelB_1_offset_XM)
{
  localOffset = "-1.5 0.0 2.2";
};
datablock afxEffectWrapperData(MLF_LeafFunnelB_1_EW)
{
  effect = MLF_LeafA_E;
  posConstraint = caster;
  lifetime = 2.7;//3.7;
  delay = 2.5;
  xfmModifiers[0] = "MLF_LeafFunnelB_1_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelB_1_offset_XM";
};

datablock afxXM_SpinData(MLF_LeafFunnelB_2_spin_XM)
{
  spinAxis = "-.32 -0.15 1.0";
  spinRate = -560;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelB_2_offset_XM)
{
  localOffset = "-1.6 0.0 2.5";
};
datablock afxEffectWrapperData(MLF_LeafFunnelB_2_EW)
{
  effect = MLF_LeafA_E;
  posConstraint = caster;
  lifetime = 2.6;//3.6;
  delay = 2.6;
  xfmModifiers[0] = "MLF_LeafFunnelB_2_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelB_2_offset_XM";
};

datablock afxXM_SpinData(MLF_LeafFunnelB_3_spin_XM)
{
  spinAxis = "0.1 -0.22 1.0";
  spinRate = 310;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelB_3_offset_XM)
{
  localOffset = "1.4 0.0 3.0";
};
datablock afxEffectWrapperData(MLF_LeafFunnelB_3_EW)
{
  effect = MLF_LeafA_E;
  posConstraint = caster;
  lifetime = 2.5;//3.5;
  delay = 2.7;
  xfmModifiers[0] = "MLF_LeafFunnelB_3_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelB_3_offset_XM";
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// HAND CLAP FINISH

//
// The end of the casting time is punctuated with a dramatic hand
// clap that begins the formation of the tornado missile. 
//
// Hand clap effects include a spray of green sparkles and a 
// purple light.
//

datablock ParticleEmitterData(MLF_SparkleClap_E)
{
  ejectionOffset    = 0.02;
  ejectionPeriodMS  = 1;
  periodVarianceMS  = 0;
  ejectionVelocity  = 8.0;
  velocityVariance  = 2.5;
  thetaMin          = 0.0;
  thetaMax          = 180.0;
  particles         = MLF_Sparkle_P;
};
//
datablock afxXM_WorldOffsetData(MLF_SparkleClap_offset_XM)
{
  worldOffset = "0.0 0.0 5.0";
};
//
datablock afxEffectWrapperData(MLF_SparkleClap_EW)
{
  effect = MLF_SparkleClap_E;
  constraint = caster;
  lifetime = 0.25;
  delay = 4.95;
  xfmModifiers[0] = "MLF_SparkleClap_offset_XM";
};

%MLF_ClapLight_intensity = 0.75;
datablock sgLightObjectData(MLF_ClapLight_CE)
{
  CastsShadows = true;
  Radius = 9.0;
  Brightness = 2*%MLF_ClapLight_intensity;
  Colour = "0.9 0.4 1.0";

  FlareOn = true;
  LinkFlare = true;
  //LinkFlareSize = true;
  FlareTP = true;
  FlareBitmap = %mySpellDataPath @ "/MLF/lights/MLF_clapLightFlare";
  NearSize = 3.5; //6;
  FarSize  = 1.75; //3;
  NearDistance = 10;
  FarDistance  = 100;

  AnimBrightness = true;
  LerpBrightness = true;
  MinBrightness = 2*%MLF_ClapLight_intensity*0.6;
  MaxBrightness = 2*%MLF_ClapLight_intensity*1.3;
  BrightnessKeys = "AZA";
  BrightnessTime = 0.1;
};
//
datablock afxEffectWrapperData(MLF_ClapLight_EW)
{
  effect = MLF_ClapLight_CE;
  constraint = caster; 
  delay    = 4.9;
  lifetime = 0.3;
  fadeInTime  = 0.1;
  fadeOutTime = 0.6;
  xfmModifiers[0] = MLF_SparkleClap_offset_XM;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// TORNADO FORMATION
//
// When the tornado missile is launched it remains stationary
// for a short time before moving off toward the target. During
// this pause, a pair of sparkling spheres and a series of leaf
// emitters trace out the initial shape of the tornado.
//

//
// MISSILE MAGIC SPHERES
//
// The purple sparkle magic that originates in the spellcaster's
// hands, forms two spheres that separate, moving up and down to
// draw out the body of the tornado. 
//

// This particles are similar to what's used in the hand sparklers.

datablock ParticleData(MLF_MagicMissile_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/MLF_magicB";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 150;
   lifetimeVarianceMS   = 50;
   useInvAlpha          = false;
   spinRandomMin        = -360.0;
   spinRandomMax        = 360.0;
   colors[0]            = "1.0 1.0 1.0 1.0";
   colors[1]            = "1.0 1.0 1.0 0.0";
   sizes[0]             = 2.2;
   sizes[1]             = 1.0;
   times[0]             = 0.9;
   times[1]             = 1.0;

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // magicB
   textureCoords[0]     = "0.0 0.5";
   textureCoords[1]     = "0.0 1.0";
   textureCoords[2]     = "0.5 1.0";
   textureCoords[3]     = "0.5 0.5";
};
//
datablock ParticleEmitterData(MLF_MagicMissile_E)
{
  ejectionOffset        = 0.02;
  ejectionPeriodMS      = 5; //10;
  periodVarianceMS      = 1;
  ejectionVelocity      = 5.0;
  velocityVariance      = 1.0;
  thetaMin              = 0.0;
  thetaMax              = 180.0;
  particles             = MLF_MagicMissile_P;
};

//
// This magic sphere moves up, following a simple
// two point motion path.
//
datablock afxPathData(MLF_MagicMissileTOP_Path)
{
  points = "0 0 0	0 0 4.5"; //5.0";	
  ///lifetime = 2.0;
  ///delay = 0.5;
  lifetime = 1.0;
  delay = 0.25;
};
datablock afxXM_PathConformData(MLF_MagicMissileTOP_Path_XM)
{
  paths = "MLF_MagicMissileTOP_Path";
};
datablock afxEffectWrapperData(MLF_MagicMissileTOP_EW)
{
  effect = MLF_MagicMissile_E;
  posConstraint = missile;
  ///lifetime = 3.5;
  lifetime = 1.75;
  xfmModifiers[0] = MLF_MagicMissileTOP_Path_XM;
};

//
// This magic sphere moves down, following a simple
// two point motion path.
//
datablock afxPathData(MLF_MagicMissileBOT_Path)
{
  points = "0 0 0	0 0 -4.3"; //-3.5";
  ///lifetime = 2.0;
  ///delay = 0.5;
  lifetime = 1.0;
  delay = 0.25;
};
datablock afxXM_PathConformData(MLF_MagicMissileBOT_Path_XM)
{
  paths = "MLF_MagicMissileBOT_Path";
};
datablock afxEffectWrapperData(MLF_MagicMissileBOT_EW)
{
  effect = MLF_MagicMissile_E;
  posConstraint = missile;
  xfmModifiers[0] = MLF_MagicMissileBOT_Path_XM;
};


//
// FUNNEL TOP
//
// These emitters trace out successively larger particle rings
// to suggest the top end of the tornado funnel
//

datablock afxXM_SpinData(MLF_LeafFunnelMissileTOP1_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = 800;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelMissileTOP1_offset_XM)
{
  localOffset = "-1.5 0.0 1.0";
};
datablock afxEffectWrapperData(MLF_LeafFunnelMissileTOP1_EW)
{
  effect = MLF_LeafB_E;
  posConstraint = missile;
  ///lifetime = 2.0;
  ///delay = 0.25;
  lifetime = 1.0;
  delay = 0.125;
  xfmModifiers[0] = "MLF_LeafFunnelMissileTOP1_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelMissileTOP1_offset_XM";
};

datablock afxXM_SpinData(MLF_LeafFunnelMissileTOP2_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = -1233;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelMissileTOP2_offset_XM)
{
  localOffset = "2.0 0.0 2.0";
};
datablock afxEffectWrapperData(MLF_LeafFunnelMissileTOP2_EW)
{
  effect = MLF_LeafB_E;
  posConstraint = missile;
  ///lifetime = 2.0;
  ///delay = 0.75;
  lifetime = 1.0;
  delay = 0.375;
  xfmModifiers[0] = "MLF_LeafFunnelMissileTOP2_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelMissileTOP2_offset_XM";
};

datablock afxXM_SpinData(MLF_LeafFunnelMissileTOP3_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = 730;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelMissileTOP3_offset_XM)
{
  localOffset = "-2.5 0.0 3.0";
};
datablock afxEffectWrapperData(MLF_LeafFunnelMissileTOP3_EW)
{
  effect = MLF_LeafB_E;
  posConstraint = missile;
  ///lifetime = 2.0;
  ///delay = 1.25;
  lifetime = 1.0;
  delay = 0.625;
  xfmModifiers[0] = "MLF_LeafFunnelMissileTOP3_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelMissileTOP3_offset_XM";
};

datablock afxXM_SpinData(MLF_LeafFunnelMissileTOP4_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = -899;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelMissileTOP4_offset_XM)
{
  localOffset = "2.75 0.0 4.0";
};
datablock afxEffectWrapperData(MLF_LeafFunnelMissileTOP4_EW)
{
  effect = MLF_LeafB_E;
  posConstraint = missile;
  ///lifetime = 2.0;
  ///delay = 1.75;
  lifetime = 1.0;
  delay = 0.875;
  xfmModifiers[0] = "MLF_LeafFunnelMissileTOP4_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelMissileTOP4_offset_XM";
};

datablock afxXM_SpinData(MLF_LeafFunnelMissileTOP5_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = -899;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelMissileTOP5_offset_XM)
{
  localOffset = "-3.0 0.0 5.0";
};
datablock afxEffectWrapperData(MLF_LeafFunnelMissileTOP5_EW)
{
  effect = MLF_LeafB_E;
  posConstraint = missile;
  ///lifetime = 2.0;
  ///delay = 2.25;
  lifetime = 1.0;
  delay = 1.125;
  xfmModifiers[0] = "MLF_LeafFunnelMissileTOP5_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelMissileTOP5_offset_XM";
};

//
// FUNNEL BOTTOM
//
// These emitters trace out successively smalle particle rings 
// to suggest the bottom end of the tornado funnel
//

datablock afxXM_SpinData(MLF_LeafFunnelMissileBOT1_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = -1288;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelMissileBOT1_offset_XM)
{
  localOffset = "1.3 0.0 0.0";
};
datablock afxEffectWrapperData(MLF_LeafFunnelMissileBOT1_EW)
{
  effect = MLF_LeafB_E;
  posConstraint = missile;
  ///lifetime = 2.0;
  ///delay = 0.5;
  lifetime = 1.0;
  delay = 0.25;
  xfmModifiers[0] = "MLF_LeafFunnelMissileBOT1_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelMissileBOT1_offset_XM";
};

datablock afxXM_SpinData(MLF_LeafFunnelMissileBOT2_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = 788;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelMissileBOT2_offset_XM)
{
  localOffset = "-1.10 0.0 -1.0";
};
datablock afxEffectWrapperData(MLF_LeafFunnelMissileBOT2_EW)
{
  effect = MLF_LeafB_E;
  posConstraint = missile;
  ///lifetime = 2.0;
  ///delay = 1.0;
  lifetime = 1.0;
  delay = 0.5;
  xfmModifiers[0] = "MLF_LeafFunnelMissileBOT2_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelMissileBOT2_offset_XM";
};

datablock afxXM_SpinData(MLF_LeafFunnelMissileBOT3_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = -953;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelMissileBOT3_offset_XM)
{
  localOffset = "0.75 0.0 -2.0";
};
datablock afxEffectWrapperData(MLF_LeafFunnelMissileBOT3_EW)
{
  effect = MLF_LeafB_E;
  posConstraint = missile;
  ///lifetime = 2.0;
  ///delay = 1.5;
  lifetime = 1.0;
  delay = 0.75;
  xfmModifiers[0] = "MLF_LeafFunnelMissileBOT3_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelMissileBOT3_offset_XM";
};

datablock afxXM_SpinData(MLF_LeafFunnelMissileBOT4_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = 977;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelMissileBOT4_offset_XM)
{
  localOffset = "0.5 0.0 -3.0";
};
datablock afxEffectWrapperData(MLF_LeafFunnelMissileBOT4_EW)
{
  effect = MLF_LeafB_E;
  posConstraint = missile;
  ///lifetime = 2.0;
  ///delay = 2.0;
  lifetime = 1.0;
  delay = 1.0;
  xfmModifiers[0] = "MLF_LeafFunnelMissileBOT4_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelMissileBOT4_offset_XM";
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// RAGING LEAF TORNADO
//

// These are the main tornado funnel particles,
// leaves and sparkles

datablock ParticleData(MLF_LeafTornado_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/MLF_leafA";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.5;//0.00;
   lifetimeMS           = 300;
   lifetimeVarianceMS   = 75;
   useInvAlpha          = false;
   spinRandomMin        = -360.0;
   spinRandomMax        = 360.0;
   colors[0]            = "0.0 0.0 0.0 0.0";
   colors[1]            = "1.0 1.0 1.0 0.5";
   colors[2]            = "1.0 1.0 1.0 0.5";
   colors[3]            = "0.0 0.0 0.0 0.0";
   sizes[0]             = 0.5;
   sizes[1]             = 0.5;
   sizes[2]             = 0.5;
   sizes[3]             = 0.5;
   times[0]             = 0.0;
   times[1]             = 0.1;
   times[2]             = 0.7;
   times[3]             = 1.0;

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // leafA
   textureCoords[0]     = "0.0 0.0";
   textureCoords[1]     = "0.0 0.5";
   textureCoords[2]     = "0.5 0.5";
   textureCoords[3]     = "0.5 0.0";
};
datablock ParticleData(MLF_SparkleTornado_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/sparkle";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.0;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 200;
   lifetimeVarianceMS   = 50;
   useInvAlpha          = false;
   spinRandomMin        = 0.0;
   spinRandomMax        = 0.0;
   colors[0]            = "1.0 1.0 1.0 1.0";
   colors[1]            = "0.5 1.0 0.5 1.0";
   colors[2]            = "0.0 1.0 0.0 1.0";
   sizes[0]             = 0.5;
   sizes[1]             = 0.4;
   sizes[2]             = 0.08;
   times[0]             = 0.0;
   times[1]             = 0.3;
   times[2]             = 1.0;

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // sparkle
   textureCoords[0]     = "0.75 0.75";
   textureCoords[1]     = "0.75 1.0";
   textureCoords[2]     = "1.0  1.0";
   textureCoords[3]     = "1.0  0.75";
};

datablock ParticleEmitterData(MLF_LeafTornado_Heavy_E)
{
  ejectionOffset        = 0.5;
  ejectionPeriodMS      = 3; //5;
  periodVarianceMS      = 1; //2;
  ejectionVelocity      = 1.5;
  velocityVariance      = 0.5;
  thetaMin              = 0.0;
  thetaMax              = 180.0;
  particles             = "MLF_LeafTornado_P MLF_SparkleTornado_P MLF_SparkleTornado_P";
};
datablock ParticleEmitterData(MLF_LeafTornado_Medium_E : MLF_LeafTornado_Heavy_E)
{
  ejectionOffset        = 0.2;
  ejectionPeriodMS      = 6; //13;
  periodVarianceMS      = 2; //3;
};
datablock ParticleEmitterData(MLF_LeafTornado_Light_E : MLF_LeafTornado_Heavy_E)
{
  ejectionOffset        = 0.05;
  ejectionPeriodMS      = 13; //25;
  periodVarianceMS      = 2;  //5;
};

//
// MAIN FUNNEL LEAVES AND SPARKLES
//

// These are the main tornado funnel particles,
// leaves and sparkles

// this path is shared by a number of the tornado effects
datablock afxPathData(MLF_LeafTornado_Path)
{
  points = "-2  -1.464101  0 " @
           " 0   2         0 " @
           " 2  -1.464101  0 " @
           "-2  -1.464101  0";
  loop = cycle;
  ///lifetime = 3.0;
  lifetime = 1.5;
};

datablock afxXM_SpinData(MLF_LeafTornado_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = 2000;
};

datablock afxXM_PathConformData(MLF_LeafTornadoA_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.2;
  ////pathTimeOffset  = 3.15; // 2.40+0.75
  pathTimeOffset  = 1.575;
};
datablock afxXM_LocalOffsetData(MLF_LeafTornadoA_offset_XM)
{
  localOffset = "2.4 0.0 4.0";
};
datablock afxEffectWrapperData(MLF_LeafTornadoA_EW)
{
  effect = MLF_LeafTornado_Heavy_E;
  posConstraint = missile;
  ///delay = 2.40;
  delay = 1.20;
  xfmModifiers[0] = "MLF_LeafTornadoA_path_XM";
  xfmModifiers[1] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[2] = "MLF_LeafTornadoA_offset_XM";
};

datablock afxXM_PathConformData(MLF_LeafTornadoB_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.3;
  ////pathTimeOffset  = 3.15; // 2.25+0.90
  pathTimeOffset  = 1.575; 
};
datablock afxXM_LocalOffsetData(MLF_LeafTornadoB_offset_XM)
{
  localOffset = "2.19 0.0 3.5";
};
datablock afxEffectWrapperData(MLF_LeafTornadoB_EW)
{
  effect = MLF_LeafTornado_Heavy_E;
  posConstraint = missile;
  ///delay = 2.25;
  delay = 1.125;
  xfmModifiers[0] = "MLF_LeafTornadoB_path_XM";
  xfmModifiers[1] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[2] = "MLF_LeafTornadoB_offset_XM";
};

datablock afxXM_PathConformData(MLF_LeafTornadoC_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.4;
  ////pathTimeOffset  = 3.15; // 2.10+1.05
  pathTimeOffset  = 1.575;
};
datablock afxXM_LocalOffsetData(MLF_LeafTornadoC_offset_XM)
{
  localOffset = "1.91 0.0 3.0";
};
datablock afxEffectWrapperData(MLF_LeafTornadoC_EW)
{
  effect = MLF_LeafTornado_Heavy_E;
  posConstraint = missile;
  delay = 2.10;
  xfmModifiers[0] = "MLF_LeafTornadoC_path_XM";
  xfmModifiers[1] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[2] = "MLF_LeafTornadoC_offset_XM";
};

datablock afxXM_PathConformData(MLF_LeafTornadoD_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.5;
  ////pathTimeOffset  = 3.15; // 1.95+1.20
  pathTimeOffset  = 1.575;
};
datablock afxXM_LocalOffsetData(MLF_LeafTornadoD_offset_XM)
{
  localOffset = "1.45 0.0 2.5";
};
datablock afxEffectWrapperData(MLF_LeafTornadoD_EW)
{
  effect = MLF_LeafTornado_Medium_E;
  posConstraint = missile;
  ///delay = 1.95;
  delay = 0.975;
  xfmModifiers[0] = "MLF_LeafTornadoD_path_XM";
  xfmModifiers[1] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[2] = "MLF_LeafTornadoD_offset_XM";
};

datablock afxXM_PathConformData(MLF_LeafTornadoE_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.6;
  ////pathTimeOffset  = 3.15; // 1.80+1.35
  pathTimeOffset  = 1.575;
};
datablock afxXM_LocalOffsetData(MLF_LeafTornadoE_offset_XM)
{
  localOffset = "1.34 0.0 2.0";
};
datablock afxEffectWrapperData(MLF_LeafTornadoE_EW)
{
  effect = MLF_LeafTornado_Medium_E;
  posConstraint = missile;
  ///delay = 1.80;
  delay = 0.90;
  xfmModifiers[0] = "MLF_LeafTornadoE_path_XM";
  xfmModifiers[1] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[2] = "MLF_LeafTornadoE_offset_XM";
};

datablock afxXM_PathConformData(MLF_LeafTornadoF_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.7;
  ////pathTimeOffset  = 3.15; // 1.65+1.50
  pathTimeOffset  = 1.575;
};
datablock afxXM_LocalOffsetData(MLF_LeafTornadoF_offset_XM)
{
  localOffset = "1.15 0.0 1.5";
};
datablock afxEffectWrapperData(MLF_LeafTornadoF_EW)
{
  effect = MLF_LeafTornado_Medium_E;
  posConstraint = missile;
  ///delay = 1.65;
  delay = 0.825;
  xfmModifiers[0] = "MLF_LeafTornadoF_path_XM";
  xfmModifiers[1] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[2] = "MLF_LeafTornadoF_offset_XM";
};

datablock afxXM_PathConformData(MLF_LeafTornadoG_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.8;
  ////pathTimeOffset  = 3.15; // 1.50+1.65
  pathTimeOffset  = 1.575;
};
datablock afxXM_LocalOffsetData(MLF_LeafTornadoG_offset_XM)
{
  localOffset = "1.03 0.0 1.0";
};
datablock afxEffectWrapperData(MLF_LeafTornadoG_EW)
{
  effect = MLF_LeafTornado_Medium_E;
  posConstraint = missile;
  ///delay = 1.50;
  delay = 0.75;
  xfmModifiers[0] = "MLF_LeafTornadoG_path_XM";
  xfmModifiers[1] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[2] = "MLF_LeafTornadoG_offset_XM";
};

datablock afxXM_PathConformData(MLF_LeafTornadoH_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.7;
  ////pathTimeOffset  = 3.45; // 1.65+1.80
  pathTimeOffset  = 1.725;
};
datablock afxXM_LocalOffsetData(MLF_LeafTornadoH_offset_XM)
{
  localOffset = "0.694 0.0 0.5";
};
datablock afxEffectWrapperData(MLF_LeafTornadoH_EW)
{
  effect = MLF_LeafTornado_Medium_E;
  posConstraint = missile;
  ///delay = 1.65;
  delay = 0.825;
  xfmModifiers[0] = "MLF_LeafTornadoH_path_XM";
  xfmModifiers[1] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[2] = "MLF_LeafTornadoH_offset_XM";
};

datablock afxXM_PathConformData(MLF_LeafTornadoI_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.6;
  ////pathTimeOffset  = 3.75; // 1.80+1.95;
  pathTimeOffset  = 1.875;
};
datablock afxXM_LocalOffsetData(MLF_LeafTornadoI_offset_XM)
{
  localOffset = "0.533 0.0 0.0";
};
datablock afxEffectWrapperData(MLF_LeafTornadoI_EW)
{
  effect = MLF_LeafTornado_Medium_E;
  posConstraint = missile;
  ///delay = 1.80;
  delay = 0.90;
  xfmModifiers[0] = "MLF_LeafTornadoI_path_XM";
  xfmModifiers[1] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[2] = "MLF_LeafTornadoI_offset_XM";
};

datablock afxXM_PathConformData(MLF_LeafTornadoJ_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.5;
  ////pathTimeOffset  = 4.05; // 1.95+2.10
  pathTimeOffset  = 2.025;
};
datablock afxXM_LocalOffsetData(MLF_LeafTornadoJ_offset_XM)
{
  localOffset = "0.533 0.0 -0.5";
};
datablock afxEffectWrapperData(MLF_LeafTornadoJ_EW)
{
  effect = MLF_LeafTornado_Light_E;
  posConstraint = missile;
  ///delay = 1.95;
  delay = 0.975;
  xfmModifiers[0] = "MLF_LeafTornadoJ_path_XM";
  xfmModifiers[1] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[2] = "MLF_LeafTornadoJ_offset_XM";
};

datablock afxXM_PathConformData(MLF_LeafTornadoK_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.4;
  ////pathTimeOffset  = 4.35; // 2.10+2.25
  pathTimeOffset  = 2.175;
};
datablock afxXM_LocalOffsetData(MLF_LeafTornadoK_offset_XM)
{
  localOffset = "0.383 0.0 -1.0";
};
datablock afxEffectWrapperData(MLF_LeafTornadoK_EW)
{
  effect = MLF_LeafTornado_Light_E;
  posConstraint = missile;
  ///delay = 2.10;
  delay = 1.05;
  xfmModifiers[0] = "MLF_LeafTornadoK_path_XM";
  xfmModifiers[1] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[2] = "MLF_LeafTornadoK_offset_XM";
};

datablock afxXM_PathConformData(MLF_LeafTornadoL_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.3;
  ////pathTimeOffset  = 4.65; // 2.25+2.40
  pathTimeOffset  = 2.325;
};
datablock afxXM_LocalOffsetData(MLF_LeafTornadoL_offset_XM)
{
  localOffset = "0.383 0.0 -1.5";
};
datablock afxEffectWrapperData(MLF_LeafTornadoL_EW)
{
  effect = MLF_LeafTornado_Light_E;
  posConstraint = missile;
  ///delay = 2.25;
  delay = 1.125;
  xfmModifiers[0] = "MLF_LeafTornadoL_path_XM";
  xfmModifiers[1] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[2] = "MLF_LeafTornadoL_offset_XM";
};

datablock afxXM_PathConformData(MLF_LeafTornadoM_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.2;
  ////pathTimeOffset  = 4.95; // 2.40+2.55
  pathTimeOffset  = 2.475;
};
datablock afxXM_LocalOffsetData(MLF_LeafTornadoM_offset_XM)
{
  localOffset = "0.173 0.0 -2.0";
};
datablock afxEffectWrapperData(MLF_LeafTornadoM_EW)
{
  effect = MLF_LeafTornado_Light_E;
  posConstraint = missile;
  ///delay = 2.40;
  delay = 1.20;
  xfmModifiers[0] = "MLF_LeafTornadoM_path_XM";
  xfmModifiers[1] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[2] = "MLF_LeafTornadoM_offset_XM";
};

datablock afxXM_PathConformData(MLF_LeafTornadoN_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.1;
  ////pathTimeOffset  = 5.25; // 2.55+2.70
  pathTimeOffset  = 2.625;
};
datablock afxXM_LocalOffsetData(MLF_LeafTornadoN_offset_XM)
{
  localOffset = "0.12 0.0 -2.5";
};
datablock afxEffectWrapperData(MLF_LeafTornadoN_EW)
{
  effect = MLF_LeafTornado_Light_E;
  posConstraint = missile;
  ///delay = 2.55;
  delay = 1.275;
  xfmModifiers[0] = "MLF_LeafTornadoN_path_XM";
  xfmModifiers[1] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[2] = "MLF_LeafTornadoN_offset_XM";
};

datablock afxXM_LocalOffsetData(MLF_LeafTornadoO_offset_XM)
{
  localOffset = "0.10 0.0 -3.0";
};
datablock afxEffectWrapperData(MLF_LeafTornadoO_EW)
{
  effect = MLF_LeafTornado_Light_E;
  posConstraint = missile;
  ///delay = 2.70;
  delay = 1.35;
  xfmModifiers[0] = "MLF_LeafTornado_spin_XM";
  xfmModifiers[1] = "MLF_LeafTornadoO_offset_XM";
};

//
// MAIN FUNNEL DUST CORE
//

//
// This series of effects gives the tornado funnel
// some core density. Each effect has its own particle
// and emitter variation.
//

// the series of particles vary only in size
datablock ParticleData(MLF_MagicTornadoA_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/MLF_magicC";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 250;
   lifetimeVarianceMS   = 75;
   useInvAlpha          = true;
   spinRandomMin        = -360.0;
   spinRandomMax        = 360.0;
   colors[0]            = "1.0 1.0 1.0 0.0";
   colors[1]            = "1.0 1.0 1.0 0.25";
   colors[2]            = "1.0 1.0 1.0 0.25";
   colors[3]            = "1.0 1.0 1.0 0.0";
   sizes[0]             = 1.5;
   sizes[1]             = 3.0;
   sizes[2]             = 3.0;
   sizes[3]             = 1.5;
   times[0]             = 0.0;
   times[1]             = 0.3;
   times[2]             = 0.7;
   times[3]             = 1.0;

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // magicC
   textureCoords[0]     = "0.5 0.0";
   textureCoords[1]     = "0.5 0.5";
   textureCoords[2]     = "1.0 0.5";
   textureCoords[3]     = "1.0 0.0";
};
datablock ParticleData(MLF_MagicTornadoB_P : MLF_MagicTornadoA_P)
{
   sizes[0]             = 1.35;
   sizes[1]             = 2.7;
   sizes[2]             = 2.7;
   sizes[3]             = 1.35;
};
datablock ParticleData(MLF_MagicTornadoC_P : MLF_MagicTornadoA_P)
{
   sizes[0]             = 1.25;
   sizes[1]             = 2.5;
   sizes[2]             = 2.5;
   sizes[3]             = 1.25;
};
datablock ParticleData(MLF_MagicTornadoD_P : MLF_MagicTornadoA_P)
{
   sizes[0]             = 1.15;
   sizes[1]             = 2.3;
   sizes[2]             = 2.3;
   sizes[3]             = 1.15;
};
datablock ParticleData(MLF_MagicTornadoE_P : MLF_MagicTornadoA_P)
{
   sizes[0]             = 1.05;
   sizes[1]             = 2.1;
   sizes[2]             = 2.1;
   sizes[3]             = 1.05;
};
datablock ParticleData(MLF_MagicTornadoF_P : MLF_MagicTornadoA_P)
{
   sizes[0]             = 0.95;
   sizes[1]             = 1.9;
   sizes[2]             = 1.9;
   sizes[3]             = 0.95;
};
datablock ParticleData(MLF_MagicTornadoG_P : MLF_MagicTornadoA_P)
{
   sizes[0]             = 0.85;
   sizes[1]             = 1.7;
   sizes[2]             = 1.7;
   sizes[3]             = 0.85;
};
datablock ParticleData(MLF_MagicTornadoH_P : MLF_MagicTornadoA_P)
{
   sizes[0]             = 0.75;
   sizes[1]             = 1.5;
   sizes[2]             = 1.5;
   sizes[3]             = 0.75;
};
datablock ParticleData(MLF_MagicTornadoI_P : MLF_MagicTornadoA_P)
{
   sizes[0]             = 0.65;
   sizes[1]             = 1.3;
   sizes[2]             = 1.3;
   sizes[3]             = 0.65;
};
datablock ParticleData(MLF_MagicTornadoJ_P : MLF_MagicTornadoA_P)
{

   sizes[0]             = 0.55;
   sizes[1]             = 1.1;
   sizes[2]             = 1.1;
   sizes[3]             = 0.55;
};
datablock ParticleData(MLF_MagicTornadoK_P : MLF_MagicTornadoA_P)
{
   sizes[0]             = 0.45;
   sizes[1]             = 0.9;
   sizes[2]             = 0.9;
   sizes[3]             = 0.45;
};
datablock ParticleData(MLF_MagicTornadoL_P : MLF_MagicTornadoA_P)
{
   sizes[0]             = 0.35;
   sizes[1]             = 0.7;
   sizes[2]             = 0.7;
   sizes[3]             = 0.35;
};
datablock ParticleData(MLF_MagicTornadoM_P : MLF_MagicTornadoA_P)
{
   sizes[0]             = 0.25;
   sizes[1]             = 0.5;
   sizes[2]             = 0.5;
   sizes[3]             = 0.25;
};
datablock ParticleData(MLF_MagicTornadoN_P : MLF_MagicTornadoA_P)
{

   sizes[0]             = 0.15;
   sizes[1]             = 0.3;
   sizes[2]             = 0.3;
   sizes[3]             = 0.15;
};

// the series of emitters vary only in ejection offset and
// velocity, and point to varied particles.
datablock ParticleEmitterData(MLF_MagicTornadoA_E)
{
  ejectionOffset        = 0.500; //1.0;
  ejectionPeriodMS      = 10; //20;
  periodVarianceMS      = 2;  //4;
  ejectionVelocity      = 1.00; //10.0;
  velocityVariance      = 0.20; //2.0;
  thetaMin              = 0.0;
  thetaMax              = 180.0;
  particles             = MLF_MagicTornadoA_P;
};
datablock ParticleEmitterData(MLF_MagicTornadoB_E : MLF_MagicTornadoA_E)
{
  ejectionOffset        = 0.465; //0.93;
  ejectionVelocity      = 0.95; //9.5;
  velocityVariance      = 0.19; //1.9;
  particles             = MLF_MagicTornadoB_P;
};
datablock ParticleEmitterData(MLF_MagicTornadoC_E : MLF_MagicTornadoA_E)
{
  ejectionOffset        = 0.430; //0.86;
  ejectionVelocity      = 0.90; //9.0;
  velocityVariance      = 0.18; //1.8;
  particles             = MLF_MagicTornadoC_P;
};
datablock ParticleEmitterData(MLF_MagicTornadoD_E : MLF_MagicTornadoA_E)
{
  ejectionOffset        = 0.395; //0.79;
  ejectionVelocity      = 0.85; //8.5;
  velocityVariance      = 0.17; //1.7;
  particles             = MLF_MagicTornadoD_P;
};
datablock ParticleEmitterData(MLF_MagicTornadoE_E : MLF_MagicTornadoA_E)
{
  ejectionOffset        = 0.360; //0.72;
  ejectionVelocity      = 0.80; //8.0;
  velocityVariance      = 0.16; //1.6;
  particles             = MLF_MagicTornadoE_P;
};
datablock ParticleEmitterData(MLF_MagicTornadoF_E : MLF_MagicTornadoA_E)
{
  ejectionOffset        = 0.325; //0.65;
  ejectionVelocity      = 0.75; //7.5;
  velocityVariance      = 0.15; //1.5;
  particles             = MLF_MagicTornadoF_P;
};
datablock ParticleEmitterData(MLF_MagicTornadoG_E : MLF_MagicTornadoA_E)
{
  ejectionOffset        = 0.290; //0.58;
  ejectionVelocity      = 0.70; //7.0;
  velocityVariance      = 0.14; //1.4;
  particles             = MLF_MagicTornadoG_P;
};
datablock ParticleEmitterData(MLF_MagicTornadoH_E : MLF_MagicTornadoA_E)
{
  ejectionOffset        = 0.255; //0.51;
  ejectionVelocity      = 0.65; //6.5;
  velocityVariance      = 0.13; //1.3;
  particles             = MLF_MagicTornadoH_P;
};
datablock ParticleEmitterData(MLF_MagicTornadoI_E : MLF_MagicTornadoA_E)
{
  ejectionOffset        = 0.220; //0.44;
  ejectionVelocity      = 0.60; //6.0;
  velocityVariance      = 0.12; //1.2;
  particles             = MLF_MagicTornadoI_P;
};
datablock ParticleEmitterData(MLF_MagicTornadoJ_E : MLF_MagicTornadoA_E)
{
  ejectionOffset        = 0.185; //0.37;
  ejectionVelocity      = 0.55; //5.5;
  velocityVariance      = 0.11; //1.1;
  particles             = MLF_MagicTornadoJ_P;
};
datablock ParticleEmitterData(MLF_MagicTornadoK_E : MLF_MagicTornadoA_E)
{
  ejectionOffset        = 0.150; //0.30;
  ejectionVelocity      = 0.50; //5.0;
  velocityVariance      = 0.10; //1.0;
  particles             = MLF_MagicTornadoK_P;
};
datablock ParticleEmitterData(MLF_MagicTornadoL_E : MLF_MagicTornadoA_E)
{
  ejectionOffset        = 0.115; //0.23;
  ejectionVelocity      = 0.45; //4.5;
  velocityVariance      = 0.09; //0.9;
  particles             = MLF_MagicTornadoL_P;
};
datablock ParticleEmitterData(MLF_MagicTornadoM_E : MLF_MagicTornadoA_E)
{
  ejectionOffset        = 0.080; //0.16;
  ejectionVelocity      = 0.40; //4.0;
  velocityVariance      = 0.08; //0.8;
  particles             = MLF_MagicTornadoM_P;
};
datablock ParticleEmitterData(MLF_MagicTornadoN_E : MLF_MagicTornadoA_E)
{
  ejectionOffset        = 0.045; //0.09;
  ejectionVelocity      = 0.35; //3.5;
  velocityVariance      = 0.07; //0.7;
  particles             = MLF_MagicTornadoN_P;
};

//
// This series of effects reuses path modifiers from the
// MLF_LeafTornado* elements.
//
datablock afxXM_WorldOffsetData(MLF_MagicTornadoA_offset_XM)
{
  worldOffset = "0.0 0.0 4.0";
};
datablock afxEffectWrapperData(MLF_MagicTornadoA_EW)
{
  effect = MLF_MagicTornadoA_E;
  posConstraint = missile;
  ///delay = 5.40; // 2.40+3.0;
  delay = 2.70;
  xfmModifiers[0] = "MLF_MagicTornadoA_offset_XM";
  xfmModifiers[1] = "MLF_LeafTornadoA_path_XM";
};

datablock afxXM_WorldOffsetData(MLF_MagicTornadoB_offset_XM)
{
  worldOffset = "0.0 0.0 3.5";
};
datablock afxEffectWrapperData(MLF_MagicTornadoB_EW)
{
  effect = MLF_MagicTornadoB_E;
  posConstraint = missile;
  ///delay = 5.25; // 2.25+3.0
  delay = 2.625; 
  xfmModifiers[0] = "MLF_MagicTornadoB_offset_XM";
  xfmModifiers[1] = "MLF_LeafTornadoB_path_XM";
};

datablock afxXM_WorldOffsetData(MLF_MagicTornadoC_offset_XM)
{
  worldOffset = "0.0 0.0 3.0";
};
datablock afxEffectWrapperData(MLF_MagicTornadoC_EW)
{
  effect = MLF_MagicTornadoC_E;
  posConstraint = missile;
  ///delay = 5.10; // 2.10+3.0
  delay = 2.55; 
  xfmModifiers[0] = "MLF_MagicTornadoC_offset_XM";
  xfmModifiers[1] = "MLF_LeafTornadoC_path_XM";
};

datablock afxXM_WorldOffsetData(MLF_MagicTornadoD_offset_XM)
{
  worldOffset = "0.0 0.0 2.5";
};
datablock afxEffectWrapperData(MLF_MagicTornadoD_EW)
{
  effect = MLF_MagicTornadoD_E;
  posConstraint = missile;
  ///delay = 4.95; // 1.95+3.0
  delay = 2.475; 
  xfmModifiers[0] = "MLF_MagicTornadoD_offset_XM";
  xfmModifiers[1] = "MLF_LeafTornadoD_path_XM";
};

datablock afxXM_WorldOffsetData(MLF_MagicTornadoE_offset_XM)
{
  worldOffset = "0.0 0.0 2.0";
};
datablock afxEffectWrapperData(MLF_MagicTornadoE_EW)
{
  effect = MLF_MagicTornadoE_E;
  posConstraint = missile;
  ///delay = 4.80; // 1.80+3.0
  delay = 2.40;
  xfmModifiers[0] = "MLF_MagicTornadoE_offset_XM";
  xfmModifiers[1] = "MLF_LeafTornadoE_path_XM";
};

datablock afxXM_WorldOffsetData(MLF_MagicTornadoF_offset_XM)
{
  worldOffset = "0.0 0.0 1.5";
};
datablock afxEffectWrapperData(MLF_MagicTornadoF_EW)
{
  effect = MLF_MagicTornadoF_E;
  posConstraint = missile;
  ///delay = 4.65; // 1.65+3.0
  delay = 2.325; 
  xfmModifiers[0] = "MLF_MagicTornadoF_offset_XM";
  xfmModifiers[1] = "MLF_LeafTornadoF_path_XM";
};

datablock afxXM_WorldOffsetData(MLF_MagicTornadoG_offset_XM)
{
  worldOffset = "0.0 0.0 1.0";
};
datablock afxEffectWrapperData(MLF_MagicTornadoG_EW)
{
  effect = MLF_MagicTornadoG_E;
  posConstraint = missile;
  ///delay = 4.50; // 1.50+3.0
  delay = 2.25;
  xfmModifiers[0] = "MLF_MagicTornadoG_offset_XM";
  xfmModifiers[1] = "MLF_LeafTornadoG_path_XM";
};

datablock afxXM_WorldOffsetData(MLF_MagicTornadoH_offset_XM)
{
  worldOffset = "0.0 0.0 0.5";
};
datablock afxEffectWrapperData(MLF_MagicTornadoH_EW)
{
  effect = MLF_MagicTornadoH_E;
  posConstraint = missile;
  ///delay = 4.65; // 1.65+3.0
  delay = 2.325;
  xfmModifiers[0] = "MLF_MagicTornadoH_offset_XM";
  xfmModifiers[1] = "MLF_LeafTornadoH_path_XM";
};

datablock afxXM_WorldOffsetData(MLF_MagicTornadoI_offset_XM)
{
  worldOffset = "0.0 0.0 0.0";
};
datablock afxEffectWrapperData(MLF_MagicTornadoI_EW)
{
  effect = MLF_MagicTornadoI_E;
  posConstraint = missile;
  ///delay = 4.80; // 1.80+3.0
  delay = 2.40;
  xfmModifiers[0] = "MLF_MagicTornadoI_offset_XM";
  xfmModifiers[1] = "MLF_LeafTornadoI_path_XM";
};

datablock afxXM_WorldOffsetData(MLF_MagicTornadoJ_offset_XM)
{
  worldOffset = "0.0 0.0 -0.5";
};
datablock afxEffectWrapperData(MLF_MagicTornadoJ_EW)
{
  effect = MLF_MagicTornadoJ_E;
  posConstraint = missile;
  ///delay = 4.95; // 1.95+3.0
  delay = 2.475;
  xfmModifiers[0] = "MLF_MagicTornadoJ_offset_XM";
  xfmModifiers[1] = "MLF_LeafTornadoJ_path_XM";
};

datablock afxXM_WorldOffsetData(MLF_MagicTornadoK_offset_XM)
{
  worldOffset = "0.0 0.0 -1.0";
};
datablock afxEffectWrapperData(MLF_MagicTornadoK_EW)
{
  effect = MLF_MagicTornadoK_E;
  posConstraint = missile;
  ///delay = 5.10; // 2.10+3.0
  delay = 2.55; 
  xfmModifiers[0] = "MLF_MagicTornadoK_offset_XM";
  xfmModifiers[1] = "MLF_LeafTornadoK_path_XM";
};

datablock afxXM_WorldOffsetData(MLF_MagicTornadoL_offset_XM)
{
  worldOffset = "0.0 0.0 -1.5";
};
datablock afxEffectWrapperData(MLF_MagicTornadoL_EW)
{
  effect = MLF_MagicTornadoL_E;
  posConstraint = missile;
  ///delay = 5.25; // 2.25+3.0
  delay = 2.625;
  xfmModifiers[0] = "MLF_MagicTornadoL_offset_XM";
  xfmModifiers[1] = "MLF_LeafTornadoL_path_XM";
};

datablock afxXM_WorldOffsetData(MLF_MagicTornadoM_offset_XM)
{
  worldOffset = "0.0 0.0 -2.0";
};
datablock afxEffectWrapperData(MLF_MagicTornadoM_EW)
{
  effect = MLF_MagicTornadoM_E;
  posConstraint = missile;
  ///delay = 5.40; // 2.40+3.0
  delay = 2.70;
  xfmModifiers[0] = "MLF_MagicTornadoM_offset_XM";
  xfmModifiers[1] = "MLF_LeafTornadoM_path_XM";
};

datablock afxXM_WorldOffsetData(MLF_MagicTornadoN_offset_XM)
{
  worldOffset = "0.0 0.0 -2.5";
};
datablock afxEffectWrapperData(MLF_MagicTornadoN_EW)
{
  effect = MLF_MagicTornadoN_E;
  posConstraint = missile;
  ///delay = 5.55; // 2.55+3.0
  delay = 2.775;
  xfmModifiers[0] = "MLF_MagicTornadoN_offset_XM";
  xfmModifiers[1] = "MLF_LeafTornadoN_path_XM";
};

//
// SCATTERD LEAVES
//

datablock ParticleData(MLF_LeafMissileCloud_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/MLF_leafA";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 1.0;//0.00;
   lifetimeMS           = 1000;
   lifetimeVarianceMS   = 250;
   useInvAlpha          = false;
   spinRandomMin        = -360.0;
   spinRandomMax        = 360.0;
   colors[0]            = "0.0 0.0 0.0 0.0";
   colors[1]            = "1.0 1.0 1.0 0.6";
   colors[2]            = "1.0 1.0 1.0 0.6";
   colors[3]            = "0.0 0.0 0.0 0.0";
   sizes[0]             = 0.5;
   sizes[1]             = 0.5;
   sizes[2]             = 0.5;
   sizes[3]             = 0.5;
   times[0]             = 0.0;
   times[1]             = 0.3;
   times[2]             = 0.7;
   times[3]             = 1.0;

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // leafA
   textureCoords[0]     = "0.0 0.0";
   textureCoords[1]     = "0.0 0.5";
   textureCoords[2]     = "0.5 0.5";
   textureCoords[3]     = "0.5 0.0";
};
datablock ParticleEmitterData(MLF_LeafMissileCloud_E)
{
  ejectionOffset        = 1.0;
  ejectionPeriodMS      = 13; //25;
  periodVarianceMS      = 3;  //7;
  ejectionVelocity      = 7.0;
  velocityVariance      = 2.5;
  thetaMin              = 0.0;
  thetaMax              = 180.0;
  particles             = "MLF_LeafMissileCloud_P";
};

datablock afxPathData(MLF_LeafMissileCloud_Path)
{
  points = "0  0  0	" @ 
           "0  0  4 " @ 
           "0  0  0 " @ 
           "0  0 -1 " @ 
           "0  0  0";
  loop = cycle;
  lifetime = 1.5;
};

datablock afxXM_PathConformData(MLF_LeafMissileCloud_path_XM)
{
  paths = "MLF_LeafMissileCloud_Path";
};

datablock afxXM_SpinData(MLF_LeafMissileCloud1_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = 3325;
};
datablock afxXM_LocalOffsetData(MLF_LeafMissileCloud1_offset_XM)
{
  localOffset = "7.0 0.0 0.0";
};
//
datablock afxEffectWrapperData(MLF_LeafMissileCloud1_EW)
{
  effect = MLF_LeafMissileCloud_E;
  constraint = missile;
  ///delay = 3.0;
  delay = 1.5;
  xfmModifiers[0] = "MLF_LeafMissileCloud1_spin_XM";
  xfmModifiers[1] = "MLF_LeafMissileCloud1_offset_XM";
  xfmModifiers[2] = "MLF_LeafMissileCloud_path_XM";
};

datablock afxXM_SpinData(MLF_LeafMissileCloud2_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = 2556;
};
datablock afxXM_LocalOffsetData(MLF_LeafMissileCloud2_offset_XM)
{
  localOffset = "10.0 0.0 0.0";
};
//
datablock afxEffectWrapperData(MLF_LeafMissileCloud2_EW)
{
  effect = MLF_LeafMissileCloud_E;
  constraint = missile;
  ///delay = 4.5;
  delay = 2.25;
  xfmModifiers[0] = "MLF_LeafMissileCloud2_spin_XM";
  xfmModifiers[1] = "MLF_LeafMissileCloud2_offset_XM";
  xfmModifiers[2] = "MLF_LeafMissileCloud_path_XM";
};

datablock afxXM_SpinData(MLF_LeafMissileCloud3_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = 4111;
};
datablock afxXM_LocalOffsetData(MLF_LeafMissileCloud3_offset_XM)
{
  localOffset = "15.0 0.0 0.0";
};
//
datablock afxEffectWrapperData(MLF_LeafMissileCloud3_EW)
{
  effect = MLF_LeafMissileCloud_E;
  constraint = missile;
  ///delay = 6.0;
  delay = 3.0;
  xfmModifiers[0] = "MLF_LeafMissileCloud3_spin_XM";
  xfmModifiers[1] = "MLF_LeafMissileCloud3_offset_XM";
  xfmModifiers[2] = "MLF_LeafMissileCloud_path_XM";
};

//
// GROUND VORTEX AND SWIRLS
//

//
// A circle of dust that rings the base of the tornado funnel.
//

datablock ParticleData(MLF_TornadoDust_GroundVortex1_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/smoke";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 500;
   lifetimeVarianceMS   = 100;
   useInvAlpha          = true;
   spinRandomMin        = -60.0;
   spinRandomMax        = 60.0;
   colors[0]            = "0.4 0.33 0.2 0.0";  
   colors[1]            = "0.4 0.33 0.2 0.15";
   colors[2]            = "0.4 0.33 0.2 0.05";
   colors[3]            = "0.4 0.33 0.2 0.0";
   sizes[0]             = 0.7; //0.1;
   sizes[1]             = 1.6; //0.3;
   sizes[2]             = 1.0; //1.0;
   sizes[3]             = 0.3; //4.0;
   times[0]             = 0.0;
   times[1]             = 0.2;
   times[2]             = 0.7;
   times[3]             = 1.0;   

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // smoke
   textureCoords[0]     = "0.875 0.625";
   textureCoords[1]     = "0.875 0.75";
   textureCoords[2]     = "1.0   0.75";
   textureCoords[3]     = "1.0   0.625";
};
datablock ParticleData(MLF_TornadoDust_GroundVortex2_P : MLF_TornadoDust_GroundVortex1_P)
{
   colors[0]            = "0.66 0.55 0.33 0.0";   
   colors[1]            = "0.66 0.55 0.33 0.15";
   colors[2]            = "0.66 0.55 0.33 0.05";
   colors[3]            = "0.66 0.55 0.33 0.0";
};

// a cone shaped emitter
datablock afxParticleEmitterConeData(MLF_TornadoDust_GroundVortex_E) // TGEA
{
  ejectionOffset        = 6.0;
  ejectionPeriodMS      = 4; //8;
  periodVarianceMS      = 1; //2;
  ejectionVelocity      = 8.0;
  velocityVariance      = 1.5;  
  particles             = "MLF_TornadoDust_GroundVortex1_P MLF_TornadoDust_GroundVortex2_P";

  // TGE emitterType = "cone";
  vector = "0.0 0.0 1.0";
  spreadMin = 179.0;
  spreadMax = 179.0;
  ejectionInvert = true;
};

datablock afxXM_WorldOffsetData(MLF_TornadoDust_GroundVortex_offset_XM)
{
  worldOffset = "0.0 0.0 -4.15";
};
datablock afxEffectWrapperData(MLF_TornadoDust_GroundVortex_EW)
{
  effect = MLF_TornadoDust_GroundVortex_E;
  posConstraint = missile;
  ///delay = 3.0;
  delay = 1.5;
  xfmModifiers[0] = MLF_TornadoDust_GroundVortex_offset_XM;
};

//
// Swirls of dust that surround the base of the tornado funnel.
//

datablock ParticleData(MLF_TornadoDust_GroundSwirl1_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/smoke";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 500;
   useInvAlpha          = true;
   spinRandomMin        = -60.0;
   spinRandomMax        = 60.0;
   colors[0]            = "0.4 0.33 0.2 0.0";  
   colors[1]            = "0.4 0.33 0.2 0.15";
   colors[2]            = "0.4 0.33 0.2 0.05";
   colors[3]            = "0.4 0.33 0.2 0.0";
   sizes[0]             = 0.8; //0.1;
   sizes[1]             = 2.0; //0.3;
   sizes[2]             = 1.0; //1.0;
   sizes[3]             = 0.3; //4.0;
   times[0]             = 0.0;
   times[1]             = 0.2;
   times[2]             = 0.7;
   times[3]             = 1.0;   

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // smoke
   textureCoords[0]     = "0.875 0.625";
   textureCoords[1]     = "0.875 0.75";
   textureCoords[2]     = "1.0   0.75";
   textureCoords[3]     = "1.0   0.625";
};
datablock ParticleData(MLF_TornadoDust_GroundSwirl2_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/smoke";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 500;
   useInvAlpha          = true;
   spinRandomMin        = -60.0;
   spinRandomMax        = 60.0;
   colors[0]            = "0.66 0.55 0.33 0.0";   
   colors[1]            = "0.66 0.55 0.33 0.15";
   colors[2]            = "0.66 0.55 0.33 0.05";
   colors[3]            = "0.66 0.55 0.33 0.0";
   sizes[0]             = 0.8; //0.1;
   sizes[1]             = 2.0; //0.3;
   sizes[2]             = 1.0; //1.0;
   sizes[3]             = 0.3; //4.0;
   times[0]             = 0.0;
   times[1]             = 0.2;
   times[2]             = 0.7;
   times[3]             = 1.0;   

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // smoke
   textureCoords[0]     = "0.875 0.625";
   textureCoords[1]     = "0.875 0.75";
   textureCoords[2]     = "1.0   0.75";
   textureCoords[3]     = "1.0   0.625";
};
datablock afxParticleEmitterConeData(MLF_TornadoDust_GroundSwirl_E) // TGEA
{
  ejectionOffset        = 0.0;
  ejectionPeriodMS      = 2; //5;
  periodVarianceMS      = 1; //2;
  ejectionVelocity      = 3.0;
  velocityVariance      = 0.5;  
  particles             = "MLF_TornadoDust_GroundSwirl1_P MLF_TornadoDust_GroundSwirl2_P";

  // TGE emitterType = "cone";
  vector = "0.0 1.0 0.0";
  spreadMin = 0.0;
  spreadMax = 30.0;
};

datablock afxXM_SpinData(MLF_TornadoDust_GroundSwirl1_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = 2886;
};
datablock afxXM_LocalOffsetData(MLF_TornadoDust_GroundSwirl1_offset_XM)
{
  localOffset = "8.0 0.0 0.0";
};
datablock afxEffectWrapperData(MLF_TornadoDust_GroundSwirl1_EW)
{
  effect = MLF_TornadoDust_GroundSwirl_E;
  posConstraint = missile;
  ///delay = 4.0;
  delay = 2.0;
  xfmModifiers[0] = MLF_TornadoDust_GroundSwirl1_spin_XM;
  xfmModifiers[1] = MLF_TornadoDust_GroundSwirl1_offset_XM;
  xfmModifiers[2] = MLF_TornadoDust_GroundVortex_offset_XM;

  //orbitAxis = "0.0 0.0 1.0";
  //orbitRate = 2886;
  //orbitOffset = "8.0 0.0 0.0";
  //offset = "0.0 0.0 -4.15"; //-3.4";
};

datablock afxXM_SpinData(MLF_TornadoDust_GroundSwirl2_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = 1746;
};
datablock afxXM_LocalOffsetData(MLF_TornadoDust_GroundSwirl2_offset_XM)
{
  localOffset = "13.0 0.0 0.0";
};
datablock afxEffectWrapperData(MLF_TornadoDust_GroundSwirl2_EW)
{
  effect = MLF_TornadoDust_GroundSwirl_E;
  posConstraint = missile;
  ///delay = 5.0;
  delay = 2.5;
  xfmModifiers[0] = MLF_TornadoDust_GroundSwirl2_spin_XM;
  xfmModifiers[1] = MLF_TornadoDust_GroundSwirl2_offset_XM;
  xfmModifiers[2] = MLF_TornadoDust_GroundVortex_offset_XM;

  //orbitAxis = "0.0 0.0 1.0";
  //orbitRate = 1746;
  //orbitOffset = "13.0 0.0 0.0";
  //offset = "0.0 0.0 -4.15"; //-3.4";
};

datablock afxXM_SpinData(MLF_TornadoDust_GroundSwirl3_spin_XM)
{
  spinAxis = "0.0 0.0 1.0";
  spinRate = 1344;
};
datablock afxXM_LocalOffsetData(MLF_TornadoDust_GroundSwirl3_offset_XM)
{
  localOffset = "20.0 0.0 0.0";
};
datablock afxEffectWrapperData(MLF_TornadoDust_GroundSwirl3_EW)
{
  effect = MLF_TornadoDust_GroundSwirl_E;
  posConstraint = missile;
  ///delay = 6.0;
  delay = 3.0;
  xfmModifiers[0] = MLF_TornadoDust_GroundSwirl3_spin_XM;
  xfmModifiers[1] = MLF_TornadoDust_GroundSwirl3_offset_XM;
  xfmModifiers[2] = MLF_TornadoDust_GroundVortex_offset_XM;

  //orbitAxis = "0.0 0.0 1.0";
  //orbitRate = 1344;
  //orbitOffset = "20.0 0.0 0.0";
  //offset = "0.0 0.0 -4.15"; //-3.4";
};




//
// FUNNEL TOP SPRAY
//

// dirt and dust 
datablock ParticleData(MLF_TornadoDustAir1_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/smoke";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.1;  //0.1
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 1500;
   lifetimeVarianceMS   = 500;
   useInvAlpha          = true;
   spinRandomMin        = -60.0;
   spinRandomMax        = 60.0;
   colors[0]            = "0.4 0.33 0.2 0.0";
   colors[1]            = "0.4 0.33 0.2 0.05";
   colors[2]            = "0.4 0.33 0.2 0.25";
   colors[3]            = "0.4 0.33 0.2 0.0";
   sizes[0]             = 0.5;
   sizes[1]             = 1.0;
   sizes[2]             = 2.0;
   sizes[3]             = 4.0; //3.0
   times[0]             = 0.0;
   times[1]             = 0.3;
   times[2]             = 0.7;
   times[3]             = 1.0;

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // smoke
   textureCoords[0]     = "0.875 0.625";
   textureCoords[1]     = "0.875 0.75";
   textureCoords[2]     = "1.0   0.75";
   textureCoords[3]     = "1.0   0.625";
};
datablock ParticleData(MLF_TornadoDustAir2_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/smoke";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.1;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 1500;
   lifetimeVarianceMS   = 500;
   useInvAlpha          = true;
   spinRandomMin        = -60.0;
   spinRandomMax        = 60.0;
   colors[0]            = "0.66 0.55 0.33 0.0";
   colors[1]            = "0.66 0.55 0.33 0.05";
   colors[2]            = "0.66 0.55 0.33 0.25";
   colors[3]            = "0.66 0.55 0.33 0.0";
   sizes[0]             = 0.5;
   sizes[1]             = 1.0;
   sizes[2]             = 2.0;
   sizes[3]             = 4.0; //3.0
   times[0]             = 0.0;
   times[1]             = 0.3;
   times[2]             = 0.7;
   times[3]             = 1.0;

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // smoke
   textureCoords[0]     = "0.875 0.625";
   textureCoords[1]     = "0.875 0.75";
   textureCoords[2]     = "1.0   0.75";
   textureCoords[3]     = "1.0   0.625";
};
datablock ParticleData(MLF_TornadoMagicDust_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/MLF_magicC";
   dragCoeffiecient     = 0.5; //0.5;
   gravityCoefficient   = 0.1;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 1500;
   lifetimeVarianceMS   = 500;
   useInvAlpha          = true;
   spinRandomMin        = -180.0;
   spinRandomMax        = 180.0;
   colors[0]            = "1.0 1.0 1.0 0.0";
   colors[1]            = "1.0 1.0 1.0 0.25";
   colors[2]            = "1.0 1.0 1.0 0.25";
   colors[3]            = "1.0 1.0 1.0 0.0";
   sizes[0]             = 1.5;
   sizes[1]             = 2.0;
   sizes[2]             = 4.0; //3.0;
   sizes[3]             = 5.0; //1.5;
   times[0]             = 0.0;
   times[1]             = 0.3;
   times[2]             = 0.7;
   times[3]             = 1.0;

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // magicC
   textureCoords[0]     = "0.5 0.0";
   textureCoords[1]     = "0.5 0.5";
   textureCoords[2]     = "1.0 0.5";
   textureCoords[3]     = "1.0 0.0";
};
datablock afxParticleEmitterConeData(MLF_TornadoDust_AirSpout_E) // TGEA
{
  ejectionOffset        = 1.0;
  ejectionPeriodMS      = 2; //3;
  periodVarianceMS      = 0; //1;
  ejectionVelocity      = 4.0;
  velocityVariance      = 1.0; 
  particles             = "MLF_TornadoDustAir1_P MLF_TornadoDustAir2_P MLF_TornadoMagicDust_P";
  // TGE emitterType = "cone";
  vector = "0.0 0.0 1.0";
  spreadMin = 0.0; 
  spreadMax = 130.0; //20.0; 
};

// green sparkles 
datablock ParticleData(MLF_TornadoSparkleDust_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/sparkle";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.35;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 500;
   useInvAlpha          = false;
   spinRandomMin        = 0.0;
   spinRandomMax        = 0.0;
   colors[0]            = "0.0 0.0 0.0 0.0";
   colors[1]            = "0.5 1.0 0.5 1.0";
   colors[2]            = "0.0 1.0 0.0 1.0";
   sizes[0]             = 0.5;
   sizes[1]             = 0.4;
   sizes[2]             = 0.08;
   times[0]             = 0.0;
   times[1]             = 0.3;
   times[2]             = 1.0;

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_parts"; // sparkle
   textureCoords[0]     = "0.75 0.75";
   textureCoords[1]     = "0.75 1.0";
   textureCoords[2]     = "1.0  1.0";
   textureCoords[3]     = "1.0  0.75";
};
datablock afxParticleEmitterConeData(MLF_TornadoSparkle_Air_E) // TGEA
{
  ejectionOffset        = 1.0;
  ejectionPeriodMS      = 4; //8;
  periodVarianceMS      = 1; //2;
  ejectionVelocity      = 8.0;
  velocityVariance      = 6.0;
  particles             = "MLF_TornadoSparkleDust_P";

  // TGE emitterType = "cone";
  vector = "0.0 0.0 1.0";
  spreadMin = 100.0; //175.0;
  spreadMax = 180.0;
};

datablock afxXM_PathConformData(MLF_TornadoDust_AirSpout_path_XM)
{
  paths           = "MLF_LeafTornado_Path";
  pathMult        = 0.2;
  ////pathTimeOffset  = 3.15; // 2.40+0.75
  pathTimeOffset  = 1.575;
};

//
// Dirt and dust that sprays from top of funnel.
//
datablock afxXM_WorldOffsetData(MLF_TornadoDust_AirSpout_offset_XM)
{
  worldOffset = "0.0 0.0 4.0"; //4.3
};
//
datablock afxEffectWrapperData(MLF_TornadoDust_AirSpout_EW)
{
  effect = MLF_TornadoDust_AirSpout_E;
  posConstraint = missile;
  ///delay = 5.40; // 2.40+3.0;
  delay = 2.70;
  xfmModifiers[0] = "MLF_TornadoDust_AirSpout_offset_XM";
  xfmModifiers[1] = "MLF_TornadoDust_AirSpout_path_XM";
};


//
// Green sparkles that spray from top of funnel.
//
datablock afxXM_WorldOffsetData(MLF_TornadoSparkle_Air_offset_XM)
{
  worldOffset = "0.0 0.0 4.2"; //4.3
};
//
datablock afxEffectWrapperData(MLF_TornadoSparkle_Air_EW)
{
  effect = MLF_TornadoSparkle_Air_E;
  posConstraint = missile;
  ///delay = 5.40; // 2.40+3.0;
  delay = 2.70;
  xfmModifiers[0] = "MLF_TornadoSparkle_Air_offset_XM";
  xfmModifiers[1] = "MLF_TornadoDust_AirSpout_path_XM";
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// BLOOD EFFECTS
//

// GUSH particles are each a single blood drop
datablock ParticleData(MLF_BloodGush_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/MLF_bloodsquirt";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 1.0; //0.2;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 800;
   lifetimeVarianceMS   = 200;
   useInvAlpha          = true;
   //spinRandomMin        = -360.0;
   //spinRandomMax        = 360.0;
   colors[0]            = "1.0 1.0 1.0 1.0";   
   colors[1]            = "1.0 1.0 1.0 0.4";
   sizes[0]             = 0.05;   
   sizes[1]             = 0.15;
   times[0]             = 0.0;   
   times[1]             = 1.0;

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_blood"; // bloodsquirt
   textureCoords[0]     = "0.75 0.75";
   textureCoords[1]     = "0.75 1.0";
   textureCoords[2]     = "1.0  1.0";
   textureCoords[3]     = "1.0  0.75";
};

// SPURT particles are a splatter of blood drops
datablock ParticleData(MLF_BloodSpurt_P)
{
   // TGE textureName          = %mySpellDataPath @ "/MLF/particles/MLF_bloodspurt";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 1.0; //0.2;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 800;
   lifetimeVarianceMS   = 200;
   useInvAlpha          = true;
   spinRandomMin        = -360.0;
   spinRandomMax        = 360.0;
   colors[0]            = "1.0 1.0 1.0 1.0";   
   colors[1]            = "1.0 1.0 1.0 0.4";
   sizes[0]             = 0.9;   
   sizes[1]             = 0.95;
   times[0]             = 0.0;   
   times[1]             = 1.0;

   textureName          = %mySpellDataPath @ "/MLF/particles/mlf_tiled_blood"; // bloodspurt
   textureCoords[0]     = "0.0 0.0";
   textureCoords[1]     = "0.0 0.5";
   textureCoords[2]     = "0.5 0.5";
   textureCoords[3]     = "0.5 0.0";
};

//
// SPURTING blood -- short bursts of blood. These will
// occur on a corpse impact.
//

datablock ParticleEmitterData(MLF_BloodSpurt_E)
{
  // TGE emitterType = "sprinkler";

  ejectionOffset        = 0.02;
  ejectionPeriodMS      = 10;
  periodVarianceMS      = 3;
  ejectionVelocity      = 5.0;
  velocityVariance      = 1.5;  
  particles             = "MLF_BloodSpurt_P MLF_BloodGush_P";  
};

// blood spurts from body, then arm, then leg

datablock afxEffectWrapperData(MLF_BloodSpurt_Body_EW)
{
  effect = MLF_BloodSpurt_E;
  constraint = "impactedObject.Bip01 Spine1";
  delay    = 0.0;
  lifetime = 0.3; 
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};

datablock afxEffectWrapperData(MLF_BloodSpurt_LFArm_EW)
{
  effect = MLF_BloodSpurt_E;
  constraint = "impactedObject.Bip01 L Clavicle";
  delay    = 0.5;
  lifetime = 0.3; 
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};

datablock afxEffectWrapperData(MLF_BloodSpurt_RTLeg_EW)
{
  effect = MLF_BloodSpurt_E;
  constraint = "impactedObject.Bip01 R Calf";
  delay    = 1.0;
  lifetime = 0.3; 
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};


//
// GUSHING blood -- shooting streams of blood. These will 
// only occur if target is dying. Not on a corpse and not
// on a target that survives the impact.
//

// three identicle vector emitters with different vectors only
datablock afxParticleEmitterVectorData(MLF_BloodGush1_E) // TGEA
{
  // TGE emitterType           = "vector";
  ejectionOffset        = 0.02;
  ejectionPeriodMS      = 2;
  periodVarianceMS      = 1;
  ejectionVelocity      = 3.0;
  velocityVariance      = 0.0;  
  particles             = "MLF_BloodGush_P";
  vector                = "1.0 0.0 0.5";
};
//
datablock afxParticleEmitterVectorData(MLF_BloodGush2_E : MLF_BloodGush1_E) // TGEA
{
  vector = "1.0 -0.9 0.1";
};
//
datablock afxParticleEmitterVectorData(MLF_BloodGush3_E : MLF_BloodGush1_E) // TGEA
{
  vector = "0.5 0.0 -1.0";
};

// body bleeding
datablock afxEffectWrapperData(MLF_BloodGush_Body1_EW)
{
  effect = MLF_BloodGush1_E;
  constraint = "impactedObject.Bip01 Spine1";
  lifetime = 4.0;
  delay = 0.5;   //0.0+0.5
  lifeConstraint = "impactedObject";
  lifeConditions = $AFX::DYING;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};
datablock afxEffectWrapperData(MLF_BloodGush_Body2_EW : MLF_BloodGush_Body1_EW)
{
  effect = MLF_BloodGush2_E;
};
datablock afxEffectWrapperData(MLF_BloodGush_Body3_EW : MLF_BloodGush_Body1_EW)
{
  effect = MLF_BloodGush3_E;
};

// shoulder bleeding
datablock afxEffectWrapperData(MLF_BloodGush_LFArm1_EW)
{
  effect = MLF_BloodGush1_E;
  constraint = "impactedObject.Bip01 L Clavicle";
  lifetime = 4.0;
  delay = 1.0;   //0.5+0.5
  lifeConstraint = "impactedObject";
  lifeConditions = $AFX::DYING;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};
datablock afxEffectWrapperData(MLF_BloodGush_LFArm2_EW : MLF_BloodGush_LFArm1_EW)
{
  effect = MLF_BloodGush2_E;
};

// leg bleeding
datablock afxEffectWrapperData(MLF_BloodGush_RTLeg1_EW)
{
  effect = MLF_BloodGush1_E;
  constraint = "impactedObject.Bip01 R Calf";  
  lifetime = 4.0;
  delay = 1.5;  //1.0+0.5
  lifeConstraint = "impactedObject";
  lifeConditions = $AFX::DYING;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};
datablock afxEffectWrapperData(MLF_BloodGush_RTLeg2_EW : MLF_BloodGush_RTLeg1_EW)
{
  effect = MLF_BloodGush2_E;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// IMPACT LEAF VORTEX
//

//
// Currently the tornado missile goes away upon impact, so these
// vertex effects suggest its continuation and break up.
//
// These are very similar to the vortex effects used on the
// spellcaster earlier in the casting phase of the spell and
// make use of the same particle emitter.
//

datablock afxXM_SpinData(MLF_LeafFunnelImpact_1_spin_XM)
{
  spinAxis = "-0.2 0.3 1.0";
  spinRate = 760;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelImpact_1_offset_XM)
{
  localOffset = "-1.1 0.0 1.0";
};
//
datablock afxEffectWrapperData(MLF_LeafFunnelImpact_1_EW)
{
  effect = MLF_LeafA_E;
  posConstraint = "impactPoint";
  lifetime = 1.0;
  delay = 0.0;
  xfmModifiers[0] = "MLF_LeafFunnelImpact_1_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelImpact_1_offset_XM";
  execConditions[0] = $AFX::IMPACTED_SOMETHING;
};

datablock afxXM_SpinData(MLF_LeafFunnelImpact_2_spin_XM)
{
  spinAxis = "0.2 0.15 1.0";
  spinRate = -660;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelImpact_2_offset_XM)
{
  localOffset = "1.2 0.0 2.0";
};
//
datablock afxEffectWrapperData(MLF_LeafFunnelImpact_2_EW)
{
  effect = MLF_LeafA_E;
  posConstraint = "impactedObject";
  lifetime = 1.0;
  delay = 0.175;
  xfmModifiers[0] = "MLF_LeafFunnelImpact_2_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelImpact_2_offset_XM";
  execConditions[0] = $AFX::IMPACTED_SOMETHING;
};

datablock afxXM_SpinData(MLF_LeafFunnelImpact_3_spin_XM)
{
  spinAxis = "-0.31 0.2 1.0";
  spinRate = 810;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelImpact_3_offset_XM)
{
  localOffset = "-1.35 0.0 3.0";
};
//
datablock afxEffectWrapperData(MLF_LeafFunnelImpact_3_EW)
{
  effect = MLF_LeafA_E;
  posConstraint = "impactedObject";
  lifetime = 1.0;
  delay = 0.35;
  xfmModifiers[0] = "MLF_LeafFunnelImpact_3_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelImpact_3_offset_XM";
  execConditions[0] = $AFX::IMPACTED_SOMETHING;
};

datablock afxXM_SpinData(MLF_LeafFunnelImpact_4_spin_XM)
{
  spinAxis = "0.21 -0.16 1.0";
  spinRate = -940;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelImpact_4_offset_XM)
{
  localOffset = "1.5 0.0 4.0";
};
//
datablock afxEffectWrapperData(MLF_LeafFunnelImpact_4_EW)
{
  effect = MLF_LeafA_E;
  posConstraint = "impactedObject";
  lifetime = 1.0;
  delay = 0.525;
  xfmModifiers[0] = "MLF_LeafFunnelImpact_4_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelImpact_4_offset_XM";
  execConditions[0] = $AFX::IMPACTED_SOMETHING;
};

datablock afxXM_SpinData(MLF_LeafFunnelImpact_5_spin_XM)
{
  spinAxis = "0.1 -0.2 1.0";
  spinRate = 1021;
};
datablock afxXM_LocalOffsetData(MLF_LeafFunnelImpact_5_offset_XM)
{
  localOffset = "-1.8 0.0 5.0";
};
//
datablock afxEffectWrapperData(MLF_LeafFunnelImpact_5_EW)
{
  effect = MLF_LeafA_E;
  posConstraint = "impactedObject";
  lifetime = 1.0;
  delay = 0.70;
  xfmModifiers[0] = "MLF_LeafFunnelImpact_5_spin_XM";
  xfmModifiers[1] = "MLF_LeafFunnelImpact_5_offset_XM";
  execConditions[0] = $AFX::IMPACTED_SOMETHING;
};

// this offset defines the lights distance beneath the terrain
//datablock afxXM_LocalOffsetData(MLF_TornadoLight_offset_XM)
//{
//  localOffset = "0 0 4.5";
//};
%MLF_FunnelImpactLight_intensity = 0.5;
datablock afxLightData(MLF_FunnelImpactLight_CE)
{
  type = "Point";
  radius = ($isTGEA) ? 4 : 7;
  sgCastsShadows = false;

  sgLightingModelName = ($isTGEA) ? "Original Stock" : "Original Advanced";
  color = 1.0*%MLF_FunnelImpactLight_intensity SPC
          1.0*%MLF_FunnelImpactLight_intensity SPC
          0.7*%MLF_FunnelImpactLight_intensity;
};
//
datablock afxEffectWrapperData(MLF_FunnelImpactLight_EW)
{
  effect = MLF_FunnelImpactLight_CE;
  posConstraint = "impactedObject";
  lifetime = ($isTGEA) ? 0.5 : 1.70;
  delay = 0;
  fadeInTime = 0.5;
  fadeOutTime = ($isTGEA) ? 1.5 : 0.5;
  xfmModifiers[0] = MLF_LeafFunnelImpact_3_offset_XM;
  execConditions[0] = $AFX::IMPACTED_SOMETHING;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// TARGET IMPACT ANIMATIONS

//
// These are animation clips used on a target when hit by the
// tornado missile.
//

//
// This is a sequence that plays when the target is hit by
// the tornado missile. The target is lifted from the ground
// spinning and then dropped.
//
datablock afxAnimClipData(MLF_Impact_Clip_CE)
{
  clipName = "mlf_hit";
  rate = 1.0;
};
datablock afxEffectWrapperData(MLF_Impact_Clip_EW)
{
  effect = MLF_Impact_Clip_CE;
  constraint = "impactedObject";
  lockAnimation = true;
  lifetime = 2.0;
  delay = 0.0;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};

//
// This is a dying sequence that kicks in after the impact
// animation IF the target died from damage done. 
//   Note the use of "lifeConditions" by this effect.
//
datablock afxAnimClipData(MLF_Death_Clip_CE)
{
  clipName = "death9";
  treatAsDeathAnim = true;
  rate = 1.0;
};
datablock afxEffectWrapperData(MLF_Death_Clip_EW)
{
  effect = MLF_Death_Clip_CE;
  constraint = "impactedObject";
  lifetime = 3.633;
  delay = 1.8;
  lifeConstraint = "impactedObject";
  lifeConditions = $AFX::DYING;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// Sounds

datablock AudioProfile(MLF_ZodiacSnd_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_zodiac.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(MLF_ZodiacSnd_EW)
{
  effect = MLF_ZodiacSnd_CE;
  constraint = "caster";
  delay = 0.0;
  lifetime = 1.129;
};

datablock AudioProfile(MLF_WindbedSnd_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_windbed.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(MLF_WindbedSnd_EW)
{
  effect = MLF_WindbedSnd_CE;
  constraint = "caster";
  delay = 0.0;
  lifetime = 7.029;
};

datablock AudioProfile(MLF_Jump1aSnd_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_JUMP1a.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(MLF_Jump1aSnd_EW)
{
  effect = MLF_Jump1aSnd_CE;
  constraint = "caster";
  delay = 0.9;
  lifetime = 1.229;
};

datablock AudioProfile(MLF_Jump2aSnd_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_JUMP2a.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(MLF_Jump2aSnd_EW)
{
  effect = MLF_Jump2aSnd_CE;
  constraint = "caster";
  delay = 2.5;
  lifetime = 1.509;
};

datablock AudioProfile(MLF_Jump3aSnd_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_JUMP3a.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(MLF_Jump3aSnd_EW)
{
  effect = MLF_Jump3aSnd_CE;
  constraint = "caster";
  delay = 4.2;
  lifetime = 5.64;
};

datablock AudioProfile(MLF_Land1Snd_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_Land1.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(MLF_Land1Snd_EW)
{
  effect = MLF_Land1Snd_CE;
  constraint = "caster";
  delay = 1.4;
  lifetime = 1.663;
};

datablock AudioProfile(MLF_Land2Snd_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_Land2.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(MLF_Land2Snd_EW)
{
  effect = MLF_Land2Snd_CE;
  constraint = "caster";
  delay = 3.4;
  lifetime = 1.751;
};

datablock AudioProfile(MLF_HandclapSnd_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_Handclap.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(MLF_HandclapSnd_EW)
{
  effect = MLF_HandclapSnd_CE;
  constraint = "caster";
  delay = 4.9;
  lifetime = 2.849;
};

datablock AudioProfile(MLF_Castland1aSnd_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_castland1a.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(MLF_Castland1aSnd_EW)
{
  effect = MLF_Castland1aSnd_CE;
  constraint = "caster";
  delay = 8.2;
  lifetime = 0.754;
};

datablock AudioProfile(MLF_Castland2Snd_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_castland2.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(MLF_Castland2Snd_EW)
{
  effect = MLF_Castland2Snd_CE;
  constraint = "caster";
  delay = 9.2;
  lifetime = 0.363;
};


datablock AudioProfile(MLF_TornadoLoop_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_loop.ogg";
   description = SpellAudioMissileLoop_AD;
   preload = false;
};

datablock AudioProfile(MLF_TargetlandSnd_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_targetland.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(MLF_TargetlandSnd_EW)
{
  effect = MLF_TargetlandSnd_CE;
  constraint = "impactedObject";
  delay = 1.6;
  lifetime = 0.467;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};

datablock AudioProfile(MLF_LastBloodSnd_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_lastblood.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
//
datablock afxEffectWrapperData(MLF_LastBlood1Snd_EW)
{
  effect = MLF_LastBloodSnd_CE;
  constraint = "impactedObject";
  delay = 0.2;
  lifetime = 0.788;
  lifeConstraint = "impactedObject";
  lifeConditions = $AFX::ALIVE;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};
datablock afxEffectWrapperData(MLF_LastBlood2Snd_EW : MLF_LastBlood1Snd_EW)
{
  delay = 0.4;
};
datablock afxEffectWrapperData(MLF_LastBlood3Snd_EW : MLF_LastBlood1Snd_EW)
{
  delay = 1.6;
};
datablock afxEffectWrapperData(MLF_LastBlood4Snd_EW : MLF_LastBlood1Snd_EW)
{
  delay = 2.0;
};
datablock afxEffectWrapperData(MLF_LastBlood5Snd_EW : MLF_LastBlood1Snd_EW)
{
  delay = 2.2;
};

datablock AudioProfile(MLF_Kill_Snd_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_kill_big.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(MLF_Kill_Snd_EW)
{
  effect = MLF_Kill_Snd_CE;
  constraint = "impactedObject";
  delay = 0.2;
  lifetime = 6.126;
  lifeConstraint = "impactedObject";
  lifeConditions = $AFX::DYING;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};

datablock AudioProfile(MLF_ImpactSnd_CE)
{
   fileName = %mySpellDataPath @ "/MLF/sounds/MLF_impact.ogg";
   description = SpellAudioImpact_AD;
   preload = false;
};
datablock afxEffectWrapperData(MLF_ImpactSnd_EW)
{
  effect = MLF_ImpactSnd_CE;
  constraint = "impactPoint";
  delay = 0;
  lifetime = 2.327;
  execConditions[0] = $AFX::IMPACTED_SOMETHING;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// RAGING LEAF TORNADO MISSILE

datablock afxMagicMissileData(MLF_LeafMissile_Tornado)
{
  //particleEmitter       = MLF_MagicMissile_E;
  muzzleVelocity        = 0.1; //15;
  velInheritFactor      = 0;
  lifetime              = 50000; //30000;
  isBallistic           = true;
  ballisticCoefficient  = 0.95;
  gravityMod            = 0;//0.05;
  isGuided              = true;
  precision             = 30;
  trackDelay            = 7;

  sound = MLF_TornadoLoop_CE;

  //hasLight    = true;
  //lightRadius = 4;
  //lightColor  = "0.5 0.0 1.0";

  acceleration = 10.0;
  accelLifetime = 1750; //1500;
  accelDelay =  2000; //750;

  followTerrain = true;
  followTerrainHeight = 4.5; //3.5;
  followTerrainAdjustRate = 5;
  followTerrainAdjustDelay = 2000;

  wiggleAxis       = "0 0 1" SPC "0 0 1";
  //wiggleMagnitudes = "0.15"  SPC "0.25";
  //wiggleSpeeds     = "3.0"   SPC "2.0";
  wiggleMagnitudes = "0.08"  SPC "0.12";
  wiggleSpeeds     = "2.5"   SPC "1.5";

  // NOTE - The starting point of the missile effects the initial shape
  // of the tornado. With the given animation, it should start at the 
  // location of the character's hand clap. When using animation without
  // a matching hand clap, leave launchNode undefined and use the launchOffset
  // below to get a good initial tornado shape.
  launchNode = "Bip01 R Hand";
  launchOffsetServer = "0.0 0.0 4.7";
  echoLaunchOffset = false;
  //launchNode = "";
  //launchOffset = "0.0 0.0 4.7";
};

%MLF_MagicMissileLight1_intensity = 0.75;
datablock afxLightData(MLF_MagicMissileLight1_CE)
{
  type = "Point";
  radius = 5;
  sgCastsShadows = false;

  sgLightingModelName = ($isTGEA) ? "Inverse Square Fast Falloff" : "Near Linear";
  color = 0.5*%MLF_MagicMissileLight1_intensity SPC
    0.0*%MLF_MagicMissileLight1_intensity SPC
    1.0*%MLF_MagicMissileLight1_intensity;
};
//
datablock afxEffectWrapperData(MLF_MagicMissileLight1_EW : MLF_MagicMissileBOT_EW)
{
  effect = MLF_MagicMissileLight1_CE;
};

%MLF_MagicMissileLight2_intensity = 0.5;
datablock afxLightData(MLF_MagicMissileLight2_CE)
{
  type = "Point";
  radius = 3;
  sgCastsShadows = false;

  sgLightingModelName = "Inverse Square Fast Falloff";
  color = 1.0*%MLF_MagicMissileLight2_intensity SPC
    1.0*%MLF_MagicMissileLight2_intensity SPC
    1.0*%MLF_MagicMissileLight2_intensity;
};
//
datablock afxEffectWrapperData(MLF_MagicMissileLight2_EW : MLF_MagicMissileBOT_EW)
{
  effect = MLF_MagicMissileLight2_CE;
};

// this offset defines the lights distance beneath the terrain
//datablock afxXM_LocalOffsetData(MLF_TornadoLight_offset_XM)
//{
//  localOffset = "0 0 4.5";
//};
%MLF_TornadoLight_intensity = 0.2;
datablock afxLightData(MLF_TornadoLight_CE)
{
  type = "Point";
  radius = 24;
  sgCastsShadows = false;

  sgLightingModelName = "Original Advanced";
  color = 1.0*%MLF_TornadoLight_intensity SPC
    1.0*%MLF_TornadoLight_intensity SPC
    0.7*%MLF_TornadoLight_intensity;

  // TGEA
  //lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF;
};
//
datablock afxEffectWrapperData(MLF_TornadoLight_EW)
{
  effect = MLF_TornadoLight_CE;
  constraint = missile;
  delay = 0.75;
  fadeInTime = 1.5;
  fadeOutTime = 0.25;
  //xfmModifiers[0] = MLF_TornadoLight_offset_XM;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// MAPLELEAF FRAG SPELL
//

datablock afxMagicSpellData(MapleleafFragSpell)
{
    // warmup //
  castingDur = 5.0; //5.5; //6.2;

  missile = MLF_LeafMissile_Tornado;

    // casting zodiac //
  addCastingEffect = MLF_ZodeReveal_EW;
  addCastingEffect = MLF_MainZodeRevealLight_1_EW;
  addCastingEffect = MLF_MainZodeRevealLight_2_EW;
  addCastingEffect = MLF_MainZodeRevealLight_3_EW;
  addCastingEffect = MLF_ZodeEffect1_TLKunderglow_EW;
  addCastingEffect = MLF_ZodeEffect1_EW;
  addCastingEffect = MLF_ZodeEffect2_TLKunderglow_EW;
  addCastingEffect = MLF_ZodeEffect2_EW;
  addCastingEffect = $MLF_CastingZodeLight;
    // spellcaster animation //
  addCastingEffect = MLF_Casting_Clip_EW;
  addCastingEffect = MLF_AnimLock_EW;
    // hand sparklers //
  addCastingEffect = MLF_MagicSprayA_lf_hand_EW;
  addCastingEffect = MLF_MagicSprayA_rt_hand_EW;
  addCastingEffect = MLF_MagicSprayB_lf_hand_EW;
  addCastingEffect = MLF_MagicSprayB_rt_hand_EW;
  addCastingEffect = MLF_MagicSprayC_lf_hand_EW;
  addCastingEffect = MLF_MagicSprayC_rt_hand_EW;
  addCastingEffect = MLF_MagicSprayD_lf_hand_EW;
  addCastingEffect = MLF_MagicSprayD_rt_hand_EW;
  addCastingEffect = MLF_MagicLight_lf_hand_EW;
  addCastingEffect = MLF_MagicLight_rt_hand_EW;
   // leaf piles //
  addCastingEffect = MLF_LeafZodeReveal_A_EW;
  addCastingEffect = MLF_LeafZode_A_TLKunderglow_EW;
  addCastingEffect = MLF_LeafZode_A_EW;
  addCastingEffect = MLF_LeafZodeDisappear_A_EW;
  addCastingEffect = MLF_LeafZodeReveal_B_EW;
  addCastingEffect = MLF_LeafZode_B_TLKunderglow_EW;
  addCastingEffect = MLF_LeafZode_B_EW;
  addCastingEffect = MLF_LeafZodeDisappear_B_EW;
  addCastingEffect = MLF_LeafZodeReveal_C_EW;
  addCastingEffect = MLF_LeafZode_C_TLKunderglow_EW;
  addCastingEffect = MLF_LeafZode_C_EW;
  addCastingEffect = MLF_LeafZodeDisappear_C_EW;
    // small leaf vortex //
  addCastingEffect = MLF_LeafFunnelA_1_EW;
  addCastingEffect = MLF_LeafFunnelA_2_EW;
  addCastingEffect = MLF_LeafFunnelA_3_EW;
  addCastingEffect = MLF_LeafFunnelB_1_EW;
  addCastingEffect = MLF_LeafFunnelB_2_EW;
  addCastingEffect = MLF_LeafFunnelB_3_EW;
    // hand clap finish //
  addCastingEffect = MLF_ClapLight_EW; 
  addCastingEffect = MLF_SparkleClap_EW;
  addCastingEffect = MLF_SparkleClap_EW;
    // sound //
  addCastingEffect = MLF_ZodiacSnd_EW;
  addCastingEffect = MLF_WindbedSnd_EW;
  addCastingEffect = MLF_Jump1aSnd_EW;
  addCastingEffect = MLF_Jump2aSnd_EW;
  addCastingEffect = MLF_Jump3aSnd_EW;
  addCastingEffect = MLF_Land1Snd_EW;
  addCastingEffect = MLF_Land2Snd_EW;
  addCastingEffect = MLF_HandclapSnd_EW;
  addCastingEffect = MLF_Castland1aSnd_EW;
  addCastingEffect = MLF_Castland2Snd_EW;
    // formation: magic spheres //
  addDeliveryEffect = MLF_MagicMissileTOP_EW;
  addDeliveryEffect = MLF_MagicMissileBOT_EW;
  addDeliveryEffect = MLF_MagicMissileLight1_EW;
  addDeliveryEffect = MLF_MagicMissileLight2_EW;
  addDeliveryEffect = MLF_TornadoLight_EW;
    // formation: funnel top // 
  addDeliveryEffect = MLF_LeafFunnelMissileTOP1_EW;
  addDeliveryEffect = MLF_LeafFunnelMissileTOP2_EW;
  addDeliveryEffect = MLF_LeafFunnelMissileTOP3_EW;
  addDeliveryEffect = MLF_LeafFunnelMissileTOP4_EW;
  addDeliveryEffect = MLF_LeafFunnelMissileTOP5_EW;
    // formation: funnel bottom //
  addDeliveryEffect = MLF_LeafFunnelMissileBOT1_EW;
  addDeliveryEffect = MLF_LeafFunnelMissileBOT2_EW;
  addDeliveryEffect = MLF_LeafFunnelMissileBOT3_EW;
  addDeliveryEffect = MLF_LeafFunnelMissileBOT4_EW;
    // main tornado leaf funnel //
  addDeliveryEffect = MLF_LeafTornadoA_EW;
  addDeliveryEffect = MLF_LeafTornadoB_EW;
  addDeliveryEffect = MLF_LeafTornadoC_EW;
  addDeliveryEffect = MLF_LeafTornadoD_EW;
  addDeliveryEffect = MLF_LeafTornadoE_EW;
  addDeliveryEffect = MLF_LeafTornadoF_EW;
  addDeliveryEffect = MLF_LeafTornadoG_EW;
  addDeliveryEffect = MLF_LeafTornadoH_EW;
  addDeliveryEffect = MLF_LeafTornadoI_EW;
  addDeliveryEffect = MLF_LeafTornadoJ_EW;
  addDeliveryEffect = MLF_LeafTornadoK_EW;
  addDeliveryEffect = MLF_LeafTornadoL_EW;
  addDeliveryEffect = MLF_LeafTornadoM_EW;
  addDeliveryEffect = MLF_LeafTornadoN_EW;
  addDeliveryEffect = MLF_LeafTornadoO_EW;
    // main tornado dust funnel //
  addDeliveryEffect = MLF_MagicTornadoA_EW;
  addDeliveryEffect = MLF_MagicTornadoB_EW;
  addDeliveryEffect = MLF_MagicTornadoC_EW;
  addDeliveryEffect = MLF_MagicTornadoD_EW;
  addDeliveryEffect = MLF_MagicTornadoE_EW;
  addDeliveryEffect = MLF_MagicTornadoF_EW;
  addDeliveryEffect = MLF_MagicTornadoG_EW;
  addDeliveryEffect = MLF_MagicTornadoH_EW;
  addDeliveryEffect = MLF_MagicTornadoI_EW;
  addDeliveryEffect = MLF_MagicTornadoJ_EW;
  addDeliveryEffect = MLF_MagicTornadoK_EW;
  addDeliveryEffect = MLF_MagicTornadoL_EW;
  addDeliveryEffect = MLF_MagicTornadoM_EW;
  addDeliveryEffect = MLF_MagicTornadoN_EW;
    // funnel embellishments //
  addDeliveryEffect = MLF_TornadoDust_GroundVortex_EW;
  addDeliveryEffect = MLF_TornadoDust_GroundSwirl1_EW;
  addDeliveryEffect = MLF_TornadoDust_GroundSwirl2_EW;
  addDeliveryEffect = MLF_TornadoDust_GroundSwirl3_EW;
  addDeliveryEffect = MLF_TornadoSparkle_Air_EW;
  addDeliveryEffect = MLF_TornadoDust_AirSpout_EW;
  addDeliveryEffect = MLF_LeafMissileCloud1_EW;
  addDeliveryEffect = MLF_LeafMissileCloud2_EW;
  addDeliveryEffect = MLF_LeafMissileCloud3_EW;  

    // target animation //
  addImpactEffect = MLF_Impact_Clip_EW;
  addImpactEffect = MLF_Death_Clip_EW;
    // impact vortex //
  addImpactEffect = MLF_LeafFunnelImpact_1_EW;
  addImpactEffect = MLF_LeafFunnelImpact_2_EW;
  addImpactEffect = MLF_LeafFunnelImpact_3_EW;
  addImpactEffect = MLF_LeafFunnelImpact_4_EW;
  addImpactEffect = MLF_LeafFunnelImpact_5_EW;
  addImpactEffect = MLF_FunnelImpactLight_EW;
    // blood spurting //
  addImpactEffect = MLF_BloodSpurt_Body_EW;
  addImpactEffect = MLF_BloodSpurt_LFArm_EW;
  addImpactEffect = MLF_BloodSpurt_RTLeg_EW;
  addImpactEffect = MLF_BloodGush_Body1_EW;
  addImpactEffect = MLF_BloodGush_Body2_EW;
  addImpactEffect = MLF_BloodGush_Body3_EW;
  addImpactEffect = MLF_BloodGush_LFArm1_EW;
  addImpactEffect = MLF_BloodGush_LFArm2_EW;
  addImpactEffect = MLF_BloodGush_RTLeg1_EW;
  addImpactEffect = MLF_BloodGush_RTLeg2_EW;
    // sound //
  addImpactEffect = MLF_ImpactSnd_EW;
  addImpactEffect = MLF_TargetlandSnd_EW;
  addImpactEffect = MLF_LastBlood1Snd_EW;
  addImpactEffect = MLF_LastBlood2Snd_EW;
  addImpactEffect = MLF_LastBlood3Snd_EW;
  addImpactEffect = MLF_LastBlood4Snd_EW;
  addImpactEffect = MLF_LastBlood5Snd_EW;
  addImpactEffect = MLF_Kill_Snd_EW;
};
//
datablock afxRPGMagicSpellData(MapleleafFragSpell_RPG)
{
  name = "Mapleleaf Frag";
  desc = "Aside from the raging TORNADO, razor edged LEAVES, " @
         "gushing BLOOD, and 100 DAMAGE, it's like a fine Autumn afternoon, " @ 
         "playing in the leaves. Grab a rake." @
         "\n" @
         "\nspell design: Matthew Durante" @
         "\nsound effects: Dave Schroeder" @ 
         "\nspell concept: Jeff Faust";
  sourcePack = "Spell Pack 1";

  iconBitmap = %mySpellDataPath @ "/MLF/icons/mlf";
  target = "enemy";
  range = 75;
  manaCost = 10;
  directDamage = 100.0;
  castingDur = MapleleafFragSpell.castingDur;
};

// set a level of detail
function MapleleafFragSpell::onActivate(%this, %spell, %caster, %target)
{
  Parent::onActivate(%this, %spell, %caster, %target);
  if (MissionInfo.hasBrightLighting)
    %spell.setExecConditions($BrightLighting_mask);
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
  MapleleafFragSpell.scriptFile = $afxAutoloadScriptFile;
  MapleleafFragSpell.spellDataPath = %mySpellDataPath;
  if (isFunction(addDemoSpellbookSpell))
    addDemoSpellbookSpell(MapleleafFragSpell, MapleleafFragSpell_RPG);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//