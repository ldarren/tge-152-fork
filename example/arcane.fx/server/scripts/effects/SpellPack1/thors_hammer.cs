
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// THOR'S HAMMER SPELL
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
$spell_reload = isObject(ThorsHammerSpell);
if ($spell_reload)
{
  // mark datablocks so we can detect which are reloaded this script
  markDataBlocks();
  // reset data path from previously saved value
  %mySpellDataPath = ThorsHammerSpell.spellDataPath;
  ThorsHammerSpell.reset();
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
// SPELLCASTER ANIMATION

//
// The spellcaster begins with a quick bow -- let's call it a
// prayer -- then catches Thor's mighty hammer fallen from on high.
// He squats under its weight, lifts it, tries to keep it up, tries
// to swing it and swing it he does or is that falling forward?  It
// doesn't matter: the hammer's mighty strike ripples through the 
// earth and poor orcy is sent flying.  This animation was done in 
// Maya in concert with the hammer model.
//

datablock afxAnimClipData(TH_Swing_Clip_CE)
{
  clipName = "th";
  ignoreCorpse = true;
  rate = 1.0;
};
datablock afxEffectWrapperData(TH_Swing_Clip_EW)
{
  effect = TH_Swing_Clip_CE;
  constraint = "caster";
  lifetime = 10.0;
  delay = 0.0;
  propagateTimeFactor = true;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CASTING ZODIAC

//
// The main casting zodiac is formed by two zodiacs plus a white
// reveal glow when the casting first starts. Twenty lightning 
// zodiacs flash-on as described below.
//

// this is the white reveal glow
datablock afxZodiacData(TH_ZodeReveal_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_caster_reveal";
  radius = 3.0;
  startAngle = 7.5; //0.0+7.5
  rotationRate = -30.0;  
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(TH_ZodeEffect_Reveal_EW)
{
  effect = TH_ZodeReveal_CE;
  posConstraint = caster;
  delay = 0.0; //0.01;
  lifetime = 0.75;
  fadeInTime = 0.5;
  fadeOutTime = 0.25;
};

%TH_MainZodeRevealLight_intensity = 2.5;

datablock afxXM_LocalOffsetData(TH_MainZodeRevealLight_offset_XM)
{
  localOffset = "0 2 -4";
};
datablock afxXM_SpinData(TH_MainZodeRevealLight_spin1_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 0;
  spinRate = -30;
};
datablock afxXM_SpinData(TH_MainZodeRevealLight_spin2_XM : TH_MainZodeRevealLight_spin1_XM)
{
  spinAngle = 120;
};
datablock afxXM_SpinData(TH_MainZodeRevealLight_spin3_XM : TH_MainZodeRevealLight_spin1_XM)
{
  spinAngle = 240;
};

// main zode reveal light
datablock afxLightData(TH_MainZodeRevealLight_CE) // AFX-SPOTLIGHT
{
  type = "Spot";
  radius = ($isTGEA) ? 10 : 5;
  direction = "0 -0.313 0.95";
  sgCastsShadows = false;

  color = 1.0*%TH_MainZodeRevealLight_intensity SPC
          1.0*%TH_MainZodeRevealLight_intensity SPC
          1.0*%TH_MainZodeRevealLight_intensity;

  lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF; // TGEA (ignored by TGE)
  sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Near Linear";
};

datablock afxEffectWrapperData(TH_MainZodeRevealLight_1_EW : TH_ZodeEffect_Reveal_EW)
{
  effect = TH_MainZodeRevealLight_CE;
  xfmModifiers[0] = TH_MainZodeRevealLight_spin1_XM;
  xfmModifiers[1] = TH_MainZodeRevealLight_offset_XM;
};
datablock afxEffectWrapperData(TH_MainZodeRevealLight_2_EW : TH_ZodeEffect_Reveal_EW)
{
  effect = TH_MainZodeRevealLight_CE;
  xfmModifiers[0] = TH_MainZodeRevealLight_spin2_XM;
  xfmModifiers[1] = TH_MainZodeRevealLight_offset_XM;
};
datablock afxEffectWrapperData(TH_MainZodeRevealLight_3_EW : TH_ZodeEffect_Reveal_EW)
{
  effect = TH_MainZodeRevealLight_CE;
  xfmModifiers[0] = TH_MainZodeRevealLight_spin3_XM;
  xfmModifiers[1] = TH_MainZodeRevealLight_offset_XM;
};

// this is the main pattern of the casting zode
datablock afxZodiacData(TH_Zode1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_caster";
  radius = 3.0;
  startAngle = 0.0;
  rotationRate = -30.0;
  color = "1.0 1.0 1.0 0.9";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(TH_ZodeEffect1_EW)
{
  effect = TH_Zode1_CE;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 6.0;
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
datablock afxZodiacData(TH_Zode1_TLKunderglow_CE : TH_Zode1_CE)
{
  color = "0.6 0.6 0.6 0.6";
  blend = normal;
};
//
datablock afxEffectWrapperData(TH_Zode1_TLKunderglow_EW : TH_ZodeEffect1_EW)
{
  effect = TH_Zode1_TLKunderglow_CE;
  execConditions = $BrightLighting_mask;
};

// this is the rune ring portion of the casting zode
datablock afxZodiacData(TH_Zode2_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/zode_text";
  radius = 3.0;
  startAngle = 0.0;
  rotationRate = 20.0;
  //color = "0.67 0.09 1.0 1.0";
  color = "0.0 0.78 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(TH_ZodeEffect2_EW)
{
  effect = TH_Zode2_CE;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 5.0;
};

if ($isTGEA)
{
  %TH_CastingZodeLight_LMODELS_intensity = 25.0;
  %TH_CastingZodeLight_LTERRAIN_intensity = 2.5;

  // this offset defines the lights distance beneath the terrain
  datablock afxXM_LocalOffsetData(TH_CastingZodeLight_offset_XM)
  {
    localOffset = "0 0 -2";
  };

  datablock afxLightData(TH_CastingZodeLight_LMODELS_CE)
  {
    type = "Point";
    radius = 3;
    sgCastsShadows = false;
    sgDoubleSidedAmbient = true;

    sgLightingModelName = "Inverse Square Fast Falloff";
    color =     0.620*%TH_CastingZodeLight_LMODELS_intensity SPC
      0.502*0.8*%TH_CastingZodeLight_LMODELS_intensity SPC
      1.000*%TH_CastingZodeLight_LMODELS_intensity;

    lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF;
  };
  datablock afxLightData(TH_CastingZodeLight_LTERRAIN_CE : TH_CastingZodeLight_LMODELS_CE)
  {
    radius = 2.25;
    sgLightingModelName = "Inverse Square Fast Falloff";
    color = 0.451    *%TH_CastingZodeLight_LTERRAIN_intensity SPC
            0.988*0.8*%TH_CastingZodeLight_LTERRAIN_intensity SPC
            1.000    *%TH_CastingZodeLight_LTERRAIN_intensity;

    lightIlluminationMask = $AFX::ILLUM_TERRAIN | $AFX::ILLUM_ATLAS;
  };

  datablock afxMultiLightData(TH_CastingZodeLight_Multi_CE)
  {
    lights[0] = TH_CastingZodeLight_LMODELS_CE;
    lights[1] = TH_CastingZodeLight_LTERRAIN_CE;
  };
  datablock afxEffectWrapperData(TH_CastingZodeLight_Multi_EW : TH_ZodeEffect1_EW)
  {
    effect = TH_CastingZodeLight_Multi_CE;
    xfmModifiers[0] = TH_CastingZodeLight_offset_XM;
  };

  $TH_CastingZodeLight = TH_CastingZodeLight_Multi_EW;
}
else
{
  %TH_CastingZodeLight_intensity = 2.5;

  // this offset defines the lights distance beneath the terrain
  datablock afxXM_LocalOffsetData(TH_CastingZodeLight_offset_XM)
  {
    localOffset = "0 0 -4";
  };

  datablock afxLightData(TH_CastingZodeLight_CE)
  {
    type = "Point";
    radius = 5;
    sgCastsShadows = false;
    sgDoubleSidedAmbient = true;

    sgLightingModelName = "SG - Near Linear (Lighting Pack)";
    color = 0.620    *%TH_CastingZodeLight_intensity SPC
            0.502*0.8*%TH_CastingZodeLight_intensity SPC
            1.000    *%TH_CastingZodeLight_intensity;
  };
  //
  datablock afxEffectWrapperData(TH_CastingZodeLight_EW : TH_ZodeEffect1_EW)
  {
    effect = TH_CastingZodeLight_CE;
    xfmModifiers[0] = TH_CastingZodeLight_offset_XM;
  };

  $TH_CastingZodeLight = TH_CastingZodeLight_EW;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CASTING ZODIAC LIGHTNING

//
// Zodiac lightning flashes appear throughout the first seven seconds
// or so of the spell-casting.  There are twenty individual flashes.
// Three texture maps are re-used over and over, but independent 
// afxZodiacData datablocks are required because the "startAngle" is
// continually modified to add randomness.  The lifetimes of the 
// flashes are also varied.
//

// lightning-glow zodiac 1
datablock afxZodiacData(TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowA";
  radius = 3.0;
  startAngle = 0.0;
  rotationRate = 0.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect1_EW)
{
  effect = TH_ZodeLightning1_CE;
  posConstraint = caster;
  delay = 1.0;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.15;
};

// lightning-glow zodiac 2
datablock afxZodiacData(TH_ZodeLightning2_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowC";
  startAngle = 70.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect2_EW)
{
  effect = TH_ZodeLightning2_CE;
  posConstraint = caster;
  delay = 1.75;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.05;
};

// lightning-glow zodiac 3
datablock afxZodiacData(TH_ZodeLightning3_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowA";
  startAngle = -46.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect3_EW)
{
  effect = TH_ZodeLightning3_CE;
  posConstraint = caster;
  delay = 2.75;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.1;
};

// lightning-glow zodiac 4
datablock afxZodiacData(TH_ZodeLightning4_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowB";
  startAngle = 145.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect4_EW)
{
  effect = TH_ZodeLightning4_CE;
  posConstraint = caster;
  delay = 2.85;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.1;
};

// lightning-glow zodiac 5
datablock afxZodiacData(TH_ZodeLightning5_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowC";
  startAngle = -210.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect5_EW)
{
  effect = TH_ZodeLightning5_CE;
  posConstraint = caster;
  delay = 3.10;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.15;
};

// lightning-glow zodiac 6
datablock afxZodiacData(TH_ZodeLightning6_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowA";
  startAngle = 89.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect6_EW)
{
  effect = TH_ZodeLightning6_CE;
  posConstraint = caster;
  delay = 3.25;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.05;
};

// lightning-glow zodiac 7
datablock afxZodiacData(TH_ZodeLightning7_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowC";
  startAngle = -18.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect7_EW)
{
  effect = TH_ZodeLightning7_CE;
  posConstraint = caster;
  delay = 3.75;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.05;
};

// lightning-glow zodiac 8
datablock afxZodiacData(TH_ZodeLightning8_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowB";
  startAngle = -123.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect8_EW)
{
  effect = TH_ZodeLightning8_CE;
  posConstraint = caster;
  delay = 3.95;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.1;
};

// lightning-glow zodiac 9
datablock afxZodiacData(TH_ZodeLightning9_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowA";
  startAngle = 67.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect9_EW)
{
  effect = TH_ZodeLightning9_CE;
  posConstraint = caster;
  delay = 4.35;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.15;
};

// lightning-glow zodiac 10
datablock afxZodiacData(TH_ZodeLightning10_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowC";
  startAngle = 0.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect10_EW)
{
  effect = TH_ZodeLightning10_CE;
  posConstraint = caster;
  delay = 4.75;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.05;
};

// lightning-glow zodiac 11
datablock afxZodiacData(TH_ZodeLightning11_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowA";
  startAngle = -45.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect11_EW)
{
  effect = TH_ZodeLightning11_CE;
  posConstraint = caster;
  delay = 5.75;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.1;
};

// lightning-glow zodiac 12
datablock afxZodiacData(TH_ZodeLightning12_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowC";
  startAngle = 98.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect12_EW)
{
  effect = TH_ZodeLightning12_CE;
  posConstraint = caster;
  delay = 6.0;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.15;
};

// lightning-glow zodiac 13
datablock afxZodiacData(TH_ZodeLightning13_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowB";
  startAngle = 0.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect13_EW)
{
  effect = TH_ZodeLightning13_CE;
  posConstraint = caster;
  delay = 6.1;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.15;
};

// lightning-glow zodiac 14
datablock afxZodiacData(TH_ZodeLightning14_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowA";
  startAngle = 125.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect14_EW)
{
  effect = TH_ZodeLightning14_CE;
  posConstraint = caster;
  delay = 6.25;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.05;
};

// lightning-glow zodiac 15
datablock afxZodiacData(TH_ZodeLightning15_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowC";
  startAngle = -56.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect15_EW)
{
  effect = TH_ZodeLightning15_CE;
  posConstraint = caster;
  delay = 6.3;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.1;
};

// lightning-glow zodiac 16
datablock afxZodiacData(TH_ZodeLightning16_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowC";
  startAngle = 179.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect16_EW)
{
  effect = TH_ZodeLightning16_CE;
  posConstraint = caster;
  delay = 6.45;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.15;
};

// lightning-glow zodiac 17
datablock afxZodiacData(TH_ZodeLightning17_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowB";
  startAngle = -122.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect17_EW)
{
  effect = TH_ZodeLightning17_CE;
  posConstraint = caster;
  delay = 6.5;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.05;
};

// lightning-glow zodiac 18
datablock afxZodiacData(TH_ZodeLightning18_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowA";
  startAngle = 67.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect18_EW)
{
  effect = TH_ZodeLightning18_CE;
  posConstraint = caster;
  delay = 6.65;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.15;
};

// lightning-glow zodiac 19
datablock afxZodiacData(TH_ZodeLightning19_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowC";
  startAngle = -13.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect19_EW)
{
  effect = TH_ZodeLightning19_CE;
  posConstraint = caster;
  delay = 6.70;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.1;
};

// lightning-glow zodiac 20
datablock afxZodiacData(TH_ZodeLightning20_CE : TH_ZodeLightning1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_casterGlowB";
  startAngle = 84.0;
};
//
datablock afxEffectWrapperData(TH_ZodeLightningEffect20_EW)
{
  effect = TH_ZodeLightning20_CE;
  posConstraint = caster;
  delay = 6.75;
  fadeInTime = 0.05;
  fadeOutTime = 0.05;
  lifetime = 0.05;
};

if ($isTGEA)
{
  %TH_ZodeLightningFlash_LTERRAIN_intensity = 0.75; //TGE: 1.0*0.375;
  %TH_ZodeLightningFlash_LMODELS_intensity = 7.5;
  datablock afxLightData(TH_ZodeLightningFlashLight_LTERRAIN_CE)
  {
    type = "Point";  
    radius = 5.5;
    sgCastsShadows = false;

    sgLightingModelName = "Inverse Square Fast Falloff";
    color = 0.00*%TH_ZodeLightningFlash_LTERRAIN_intensity SPC
            0.53*%TH_ZodeLightningFlash_LTERRAIN_intensity SPC
            1.00*%TH_ZodeLightningFlash_LTERRAIN_intensity;

    lightIlluminationMask = $AFX::ILLUM_TERRAIN | $AFX::ILLUM_ATLAS;
  };
  datablock afxLightData(TH_ZodeLightningFlashLight_LMODELS_CE)
  {
    type = "Point";  
    radius = 5.5;
    sgCastsShadows = false;

    sgLightingModelName = "Inverse Square Fast Falloff";
    color = 0.00*%TH_ZodeLightningFlash_LMODELS_intensity SPC
            0.53*%TH_ZodeLightningFlash_LMODELS_intensity SPC
            1.00*%TH_ZodeLightningFlash_LMODELS_intensity;

    lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF;
  };

  datablock afxMultiLightData(TH_ZodeLightningFlashLight_Multi_CE)
  {
    lights[0] = TH_ZodeLightningFlashLight_LMODELS_CE;
    lights[1] = TH_ZodeLightningFlashLight_LTERRAIN_CE;
  };



  datablock afxEffectWrapperData(TH_ZodeLightningFlash1_Multi_EW : TH_ZodeLightningEffect1_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.15;
    fadeOutTime = 0.15;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash2_Multi_EW : TH_ZodeLightningEffect2_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.05;
    fadeOutTime = 0.05;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash3_Multi_EW : TH_ZodeLightningEffect3_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.10;
    fadeOutTime = 0.10;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash4_Multi_EW : TH_ZodeLightningEffect4_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.10;
    fadeOutTime = 0.10;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash5_Multi_EW : TH_ZodeLightningEffect5_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.15;
    fadeOutTime = 0.15;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash6_Multi_EW : TH_ZodeLightningEffect6_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.05;
    fadeOutTime = 0.05;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash7_Multi_EW : TH_ZodeLightningEffect7_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.05;
    fadeOutTime = 0.05;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash8_Multi_EW : TH_ZodeLightningEffect8_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.10;
    fadeOutTime = 0.10;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash9_Multi_EW : TH_ZodeLightningEffect9_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.15;
    fadeOutTime = 0.15;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash10_Multi_EW : TH_ZodeLightningEffect10_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.05;
    fadeOutTime = 0.05;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash11_Multi_EW : TH_ZodeLightningEffect11_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.10;
    fadeOutTime = 0.10;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash12_Multi_EW : TH_ZodeLightningEffect12_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.15;
    fadeOutTime = 0.15;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash13_Multi_EW : TH_ZodeLightningEffect13_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.15;
    fadeOutTime = 0.15;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash14_Multi_EW : TH_ZodeLightningEffect14_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.05;
    fadeOutTime = 0.05;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash15_Multi_EW : TH_ZodeLightningEffect15_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.10;
    fadeOutTime = 0.10;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash16_Multi_EW : TH_ZodeLightningEffect16_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.15;
    fadeOutTime = 0.15;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash17_Multi_EW : TH_ZodeLightningEffect17_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.05;
    fadeOutTime = 0.05;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash18_Multi_EW : TH_ZodeLightningEffect18_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.15;
    fadeOutTime = 0.15;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash19_Multi_EW : TH_ZodeLightningEffect19_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.10;
    fadeOutTime = 0.10;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash20_Multi_EW : TH_ZodeLightningEffect20_EW)
  {
    effect = TH_ZodeLightningFlashLight_Multi_CE;
    fadeInTime  = 0.05;
    fadeOutTime = 0.05;
  };

  $TH_ZodeLightningFlash1 = TH_ZodeLightningFlash1_Multi_EW;
  $TH_ZodeLightningFlash2 = TH_ZodeLightningFlash2_Multi_EW;
  $TH_ZodeLightningFlash3 = TH_ZodeLightningFlash3_Multi_EW;
  $TH_ZodeLightningFlash4 = TH_ZodeLightningFlash4_Multi_EW;
  $TH_ZodeLightningFlash5 = TH_ZodeLightningFlash5_Multi_EW;
  $TH_ZodeLightningFlash6 = TH_ZodeLightningFlash6_Multi_EW;
  $TH_ZodeLightningFlash7 = TH_ZodeLightningFlash7_Multi_EW;
  $TH_ZodeLightningFlash8 = TH_ZodeLightningFlash8_Multi_EW;
  $TH_ZodeLightningFlash9 = TH_ZodeLightningFlash9_Multi_EW;
  $TH_ZodeLightningFlash10 = TH_ZodeLightningFlash10_Multi_EW;
  $TH_ZodeLightningFlash11 = TH_ZodeLightningFlash11_Multi_EW;
  $TH_ZodeLightningFlash12 = TH_ZodeLightningFlash12_Multi_EW;
  $TH_ZodeLightningFlash13 = TH_ZodeLightningFlash13_Multi_EW;
  $TH_ZodeLightningFlash14 = TH_ZodeLightningFlash14_Multi_EW;
  $TH_ZodeLightningFlash15 = TH_ZodeLightningFlash15_Multi_EW;
  $TH_ZodeLightningFlash16 = TH_ZodeLightningFlash16_Multi_EW;
  $TH_ZodeLightningFlash17 = TH_ZodeLightningFlash17_Multi_EW;
  $TH_ZodeLightningFlash18 = TH_ZodeLightningFlash18_Multi_EW;
  $TH_ZodeLightningFlash19 = TH_ZodeLightningFlash19_Multi_EW;
  $TH_ZodeLightningFlash20_A = TH_ZodeLightningFlash20_Multi_EW;
}
else
{
  %TH_ZodeLightningFlash_intensity = 1.0*0.375;
  datablock afxLightData(TH_ZodeLightningFlashLight_CE)
  {
    type = "Point";  
    radius = 5.5;
    sgCastsShadows = false;

    sgLightingModelName = "SG - Inverse Square Fast Falloff (Lighting Pack)";
    color = 0.00*%TH_ZodeLightningFlash_intensity SPC
      0.53*%TH_ZodeLightningFlash_intensity SPC
      1.00*%TH_ZodeLightningFlash_intensity;
  };

  datablock afxEffectWrapperData(TH_ZodeLightningFlash1_EW : TH_ZodeLightningEffect1_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.15;
    fadeOutTime = 0.15;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash2_EW : TH_ZodeLightningEffect2_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.05;
    fadeOutTime = 0.05;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash3_EW : TH_ZodeLightningEffect3_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.10;
    fadeOutTime = 0.10;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash4_EW : TH_ZodeLightningEffect4_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.10;
    fadeOutTime = 0.10;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash5_EW : TH_ZodeLightningEffect5_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.15;
    fadeOutTime = 0.15;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash6_EW : TH_ZodeLightningEffect6_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.05;
    fadeOutTime = 0.05;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash7_EW : TH_ZodeLightningEffect7_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.05;
    fadeOutTime = 0.05;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash8_EW : TH_ZodeLightningEffect8_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.10;
    fadeOutTime = 0.10;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash9_EW : TH_ZodeLightningEffect9_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.15;
    fadeOutTime = 0.15;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash10_EW : TH_ZodeLightningEffect10_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.05;
    fadeOutTime = 0.05;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash11_EW : TH_ZodeLightningEffect11_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.10;
    fadeOutTime = 0.10;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash12_EW : TH_ZodeLightningEffect12_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.15;
    fadeOutTime = 0.15;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash13_EW : TH_ZodeLightningEffect13_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.15;
    fadeOutTime = 0.15;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash14_EW : TH_ZodeLightningEffect14_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.05;
    fadeOutTime = 0.05;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash15_EW : TH_ZodeLightningEffect15_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.10;
    fadeOutTime = 0.10;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash16_EW : TH_ZodeLightningEffect16_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.15;
    fadeOutTime = 0.15;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash17_EW : TH_ZodeLightningEffect17_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.05;
    fadeOutTime = 0.05;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash18_EW : TH_ZodeLightningEffect18_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.15;
    fadeOutTime = 0.15;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash19_EW : TH_ZodeLightningEffect19_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.10;
    fadeOutTime = 0.10;
  };
  datablock afxEffectWrapperData(TH_ZodeLightningFlash20_EW : TH_ZodeLightningEffect20_EW)
  {
    effect = TH_ZodeLightningFlashLight_CE;
    fadeInTime  = 0.05;
    fadeOutTime = 0.05;
  };

  $TH_ZodeLightningFlash1 = TH_ZodeLightningFlash1_EW;
  $TH_ZodeLightningFlash2 = TH_ZodeLightningFlash2_EW;
  $TH_ZodeLightningFlash3 = TH_ZodeLightningFlash3_EW;
  $TH_ZodeLightningFlash4 = TH_ZodeLightningFlash4_EW;
  $TH_ZodeLightningFlash5 = TH_ZodeLightningFlash5_EW;
  $TH_ZodeLightningFlash6 = TH_ZodeLightningFlash6_EW;
  $TH_ZodeLightningFlash7 = TH_ZodeLightningFlash7_EW;
  $TH_ZodeLightningFlash8 = TH_ZodeLightningFlash8_EW;
  $TH_ZodeLightningFlash9 = TH_ZodeLightningFlash9_EW;
  $TH_ZodeLightningFlash10 = TH_ZodeLightningFlash10_EW;
  $TH_ZodeLightningFlash11 = TH_ZodeLightningFlash11_EW;
  $TH_ZodeLightningFlash12 = TH_ZodeLightningFlash12_EW;
  $TH_ZodeLightningFlash13 = TH_ZodeLightningFlash13_EW;
  $TH_ZodeLightningFlash14 = TH_ZodeLightningFlash14_EW;
  $TH_ZodeLightningFlash15 = TH_ZodeLightningFlash15_EW;
  $TH_ZodeLightningFlash16 = TH_ZodeLightningFlash16_EW;
  $TH_ZodeLightningFlash17 = TH_ZodeLightningFlash17_EW;
  $TH_ZodeLightningFlash18 = TH_ZodeLightningFlash18_EW;
  $TH_ZodeLightningFlash19 = TH_ZodeLightningFlash19_EW;
  $TH_ZodeLightningFlash20 = TH_ZodeLightningFlash20_EW;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CASTER LIGHTNING

//
// Lightning geometry appears around and through the caster as he
// attempts to swing the hammer.  The lightning itself is a dts 
// model, but carefully-timed lights placed where the bolts appear 
// add to the effectiveness of the effect (i.e. the effect's
// effectiveness).
//

// lightning model; individual bolts have visibility animation
datablock afxModelData(TH_CasterLightningShape_CE)
{
  shapeFile = %mySpellDataPath @ "/TH/models/TH_casterLightning.dts";
  sequence = "flash";
  forceOnMaterialFlags = $MaterialFlags::Additive | $MaterialFlags::SelfIlluminating;
};
//
datablock afxEffectWrapperData(TH_CasterLightning_EW)
{
  effect     = TH_CasterLightningShape_CE;
  constraint = caster;
  delay = 0.0;
  lifetime = 8.0;
  propagateTimeFactor = true;
};


// these local offsets describe the generalized position of each 
//  lightning-flash
datablock afxXM_LocalOffsetData(TH_CasterLightningFlash_offset1_XM)
{
  localOffset = "-0.018 -1.665 0.739";
};
datablock afxXM_LocalOffsetData(TH_CasterLightningFlash_offset2_XM)
{
  localOffset = "0.997 -0.527 0.665";
};
datablock afxXM_LocalOffsetData(TH_CasterLightningFlash_offset3_XM)
{
  localOffset = "-1.329 -1.74 0.987";
};
datablock afxXM_LocalOffsetData(TH_CasterLightningFlash_offset4_XM)
{
  localOffset = "0.494 1.143 0.768";
};
datablock afxXM_LocalOffsetData(TH_CasterLightningFlash_offset5_XM)
{
  localOffset = "0.667 0.039 0.768";
};
datablock afxXM_LocalOffsetData(TH_CasterLightningFlash_offset6_XM)
{
 localOffset = "-0.017 -0.594 1.473";
};
datablock afxXM_LocalOffsetData(TH_CasterLightningFlash_offset7_XM)
{
  localOffset = "-0.017 -0.495 3.268";
};
datablock afxXM_LocalOffsetData(TH_CasterLightningFlash_offset8_XM)
{
  localOffset = "-0.232 0.341 3.71";
};
datablock afxXM_LocalOffsetData(TH_CasterLightningFlash_offset9_XM)
{
  localOffset = "-0.917 0.599 1.051";
};
datablock afxXM_LocalOffsetData(TH_CasterLightningFlash_offset10_XM)
{
  localOffset = "0.04 1.213 0.866";
};

// All 10 lightning-flashes, re-using the same light, each with its 
//  own local offset; the timing was derived in Maya to match the 
//  animated visibility in the dts model

%TH_LightningFlash_intensity = ($isTGEA) ? 5.0 : 0.5;
%TH_LightningFlash_color_TGE  = 0.00*%TH_LightningFlash_intensity SPC
                                0.53*%TH_LightningFlash_intensity SPC
                                1.00*%TH_LightningFlash_intensity;
%TH_LightningFlash_color_TGEA = 0.00*%TH_LightningFlash_intensity SPC
                                0.08*%TH_LightningFlash_intensity SPC
                                1.00*%TH_LightningFlash_intensity;
datablock afxLightData(TH_CasterLightningFlashLight_CE)
{
  type = "Point";  
  radius = 8;
  sgCastsShadows = true;

  sgLightingModelName = "Inverse Square Fast Falloff";
  color = ($isTGEA) ? %TH_LightningFlash_color_TGEA : %TH_LightningFlash_color_TGE;
};

datablock afxEffectWrapperData(TH_CasterLightningFlashLight1_EW)
{
  effect = TH_CasterLightningFlashLight_CE;
  constraint = caster;
  fadeInTime  = 0.12;
  fadeOutTime = 0.12;

  delay = 3.27;
  lifetime = 0.16;
  xfmModifiers[0] = TH_CasterLightningFlash_offset1_XM;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashLight2_EW : TH_CasterLightningFlashLight1_EW)
{
  delay = 3.83;
  lifetime = 0.23;
  xfmModifiers[0] = TH_CasterLightningFlash_offset2_XM;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashLight3_EW : TH_CasterLightningFlashLight1_EW)
{
  delay = 3.97;  
  lifetime = 0.23;
  xfmModifiers[0] = TH_CasterLightningFlash_offset3_XM;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashLight4_EW : TH_CasterLightningFlashLight1_EW)
{
  delay = 4.07;  
  lifetime = 0.20;
  xfmModifiers[0] = TH_CasterLightningFlash_offset4_XM;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashLight5_EW : TH_CasterLightningFlashLight1_EW)
{
  delay = 5.37;  
  lifetime = 0.23;
  xfmModifiers[0] = TH_CasterLightningFlash_offset5_XM;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashLight6_EW : TH_CasterLightningFlashLight1_EW)
{
  delay = 6.20;  
  lifetime = 0.27;
  xfmModifiers[0] = TH_CasterLightningFlash_offset6_XM;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashLight7_EW : TH_CasterLightningFlashLight1_EW)
{
  delay = 6.23;  
  lifetime = 0.27;
  xfmModifiers[0] = TH_CasterLightningFlash_offset7_XM;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashLight8_EW : TH_CasterLightningFlashLight1_EW)
{
  delay = 6.27;  
  lifetime = 0.27;
  xfmModifiers[0] = TH_CasterLightningFlash_offset8_XM;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashLight9_EW : TH_CasterLightningFlashLight1_EW)
{
  delay = 6.6;  
  lifetime = 0.23;
  xfmModifiers[0] = TH_CasterLightningFlash_offset9_XM;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashLight10_EW : TH_CasterLightningFlashLight1_EW)
{
  delay = 6.77;  
  lifetime = 0.20;
  xfmModifiers[0] = TH_CasterLightningFlash_offset10_XM;
};

%TH_LightningFlashWhite_intensity = ($isTGEA) ? 2.0 : 1.0*0.75;
datablock afxLightData(TH_CasterLightningFlashWhiteLight_CE)
{
  type = "Point";  
  radius = 3;
  sgCastsShadows = true;

  sgLightingModelName =  ($isTGEA) ? "Inverse Square Fast Falloff" : "Near Linear";
  color = 1.0*%TH_LightningFlashWhite_intensity SPC
          1.0*%TH_LightningFlashWhite_intensity SPC
          1.0*%TH_LightningFlashWhite_intensity;
};

datablock afxEffectWrapperData(TH_CasterLightningFlashWhiteLight1_EW : TH_CasterLightningFlashLight1_EW)
{
  effect = TH_CasterLightningFlashWhiteLight_CE;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashWhiteLight2_EW : TH_CasterLightningFlashLight2_EW)
{
  effect = TH_CasterLightningFlashWhiteLight_CE;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashWhiteLight3_EW : TH_CasterLightningFlashLight3_EW)
{
  effect = TH_CasterLightningFlashWhiteLight_CE;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashWhiteLight4_EW : TH_CasterLightningFlashLight4_EW)
{
  effect = TH_CasterLightningFlashWhiteLight_CE;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashWhiteLight5_EW : TH_CasterLightningFlashLight5_EW)
{
  effect = TH_CasterLightningFlashWhiteLight_CE;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashWhiteLight6_EW : TH_CasterLightningFlashLight6_EW)
{
  effect = TH_CasterLightningFlashWhiteLight_CE;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashWhiteLight7_EW : TH_CasterLightningFlashLight7_EW)
{
  effect = TH_CasterLightningFlashWhiteLight_CE;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashWhiteLight8_EW : TH_CasterLightningFlashLight8_EW)
{
  effect = TH_CasterLightningFlashWhiteLight_CE;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashWhiteLight9_EW : TH_CasterLightningFlashLight9_EW)
{
  effect = TH_CasterLightningFlashWhiteLight_CE;
};
datablock afxEffectWrapperData(TH_CasterLightningFlashWhiteLight10_EW : TH_CasterLightningFlashLight10_EW)
{
  effect = TH_CasterLightningFlashWhiteLight_CE;
};  


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// The LIGHT FROM ABOVE

//
// Immediately upon casting, a great beam of light shines down from
// the heavens, emanating from a glowing ball of god-glow complete
// with twirling aura.  In case there is doubt over which god is 
// meddling in the mortal world, lightning flashes through and through
// his glow.  As to why Thor has taken an interest in orcish
// affairs -- not your concern.
//

// all these effects are aim-constrained to the camera, but some only
//  by means of z-rotation; hence the need for two aim contraints
datablock afxXM_AimData(TH_LightFromAbove_Aim1_XM)
{
  aimZOnly = true;
};
datablock afxXM_AimData(TH_LightFromAbove_Aim2_XM)
{
  aimZOnly = false;
};

// all these effects needed to be nudged a bit in Y to better align
//  with the orc; but some also needed to be sent high into the sky,
//  hence the need for two local offsets
datablock afxXM_LocalOffsetData(TH_LightFromAbove_offset1_XM)
{
  localOffset = "0 0.16 0.0";
};
datablock afxXM_LocalOffsetData(TH_LightFromAbove_offset2_XM)
{
  localOffset = "0 0.16 23.5";
};

// light beam model, animated
datablock afxModelData(TH_LightBeamShape_CE)
{
  shapeFile = %mySpellDataPath @ "/TH/models/TH_lightBeam.dts";   
  alphaMult = 0.15;
  forceOnMaterialFlags = $MaterialFlags::Additive | $MaterialFlags::SelfIlluminating;
  sequence = "alight";
};
//
datablock afxEffectWrapperData(TH_LightBeam_EW)
{
  effect        = TH_LightBeamShape_CE;
  constraint    = caster;
  posConstraint2 = "camera";
  delay = 0.0;
  fadeInTime  = 0.3;
  lifetime    = 5.0;
  xfmModifiers[0] = "TH_LightFromAbove_offset1_XM";
  xfmModifiers[1] = "TH_LightFromAbove_Aim1_XM";
  propagateTimeFactor = true;
};

// glow-ball model, animated
datablock afxModelData(TH_GlowBallShape_CE)
{
  shapeFile = %mySpellDataPath @ "/TH/models/TH_glowBall.dts";   
  alphaMult = 0.5;
  forceOnMaterialFlags = $MaterialFlags::Additive | $MaterialFlags::SelfIlluminating;
  sequence = "pulsate";
};
datablock afxEffectWrapperData(TH_GlowBall_EW)
{
  effect        = TH_GlowBallShape_CE;
  constraint    = caster;
  posConstraint2 = "camera";
  delay = 0.0;
  fadeInTime  = 0.3;
  fadeOutTime = 0.5;
  lifetime = 2.6;
  xfmModifiers[0] = "TH_LightFromAbove_offset2_XM";
  xfmModifiers[1] = "TH_LightFromAbove_Aim2_XM";
  propagateTimeFactor = true;
};

// aura model, texture-mapped planes with animated rotations
datablock afxModelData(TH_AurasShape_CE)
{
  shapeFile = %mySpellDataPath @ "/TH/models/TH_auras.dts";   
  alphaMult = 0.3;   
  forceOnMaterialFlags = $MaterialFlags::Additive | $MaterialFlags::SelfIlluminating;
  sequence = "auration";
};
datablock afxEffectWrapperData(TH_Auras_EW)
{
  effect        = TH_AurasShape_CE;
  constraint    = caster;
  posConstraint2 = "camera";
  delay = 0.0;
  fadeInTime  = 0.3;
  fadeOutTime = 0.5;
  lifetime = 2.6;
  xfmModifiers[0] = "TH_LightFromAbove_offset2_XM";
  xfmModifiers[1] = "TH_LightFromAbove_Aim2_XM";
  propagateTimeFactor = true;
};

// aura lightning dts model, consisting of planes with animated 
//  visibility; the lightning texture maps used on the planes are
//  actually from the casting lightning zodiacs 
datablock afxModelData(TH_AuraLightningShape_CE)
{
  shapeFile = %mySpellDataPath @ "/TH/models/TH_auraLightning.dts";
  forceOnMaterialFlags = $MaterialFlags::Additive | $MaterialFlags::SelfIlluminating;
  sequence = "flash";
};
datablock afxEffectWrapperData(TH_AuraLightning_EW)
{
  effect        = TH_AuraLightningShape_CE;
  constraint    = caster;
  posConstraint2 = "camera";
  delay = 0.0;
  lifetime    = 3.1;
  xfmModifiers[0] = "TH_LightFromAbove_offset2_XM";
  xfmModifiers[1] = "TH_LightFromAbove_Aim2_XM";
  propagateTimeFactor = true;
};

%TH_LightFromAbove_intensity = 5.0;
datablock afxLightData(TH_LightFromAbove_CE)
{
  type = "Point";  
  radius = 30;
  sgCastsShadows = true;

  sgLightingModelName = "Original Advanced";
  color = 1.0*%TH_LightFromAbove_intensity SPC
          1.0*%TH_LightFromAbove_intensity SPC
          0.6*%TH_LightFromAbove_intensity;
};
//
datablock afxEffectWrapperData(TH_LightFromAbove_EW)
{
  effect = TH_LightFromAbove_CE;
  constraint    = caster;

  delay       = 0.0;
  fadeInTime  = 1.0;
  fadeOutTime = 0.5;
  lifetime    = 2.5;

  xfmModifiers[0] = "TH_LightFromAbove_offset2_XM";
  propagateTimeFactor = true;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// HAMMER MODEL

//
// The mighty hammer model, animated to fall into the orc's hands
// then be swung by him.  It disappears simply by fading off...  The
// model itself, however, does not have animated visibility, because
// this is not compatible with its reflection-mapping.
//
// A freeze modifier is used to prevent TH_pound_damage_CE from
// influencing the hammer.  This damage adds an impulse to the
// caster, which the hammer will inherit per its constraint unless
// this freeze is applied.
//

// hammer freeze modifier
datablock afxXM_FreezeData(TH_Hammer_freeze_XM)
{
  mask = $afxXfmMod::POS;
};

datablock afxModelData(TH_HammerShape_CE)
{
  shapeFile = %mySpellDataPath @ "/TH/models/TH_hammer.dts";
  sequence = "mightySwing";
  remapTextureTags = "TH_hammer.png:TH_hammer_A TH_hammer.png:TH_hammer_B TH_hammer.png:TH_hammer_C TH_hammer.png:TH_hammer_D TH_hammer.png:TH_hammer_E";
};

datablock afxEffectWrapperData(TH_Hammer_EW)
{
  effect     = TH_HammerShape_CE;
  constraint = caster;
  delay = 1.0;
  fadeInTime  = 1.0;
  fadeOutTime = 0.7;
  lifetime = 7.6;
  propagateTimeFactor = true;

  xfmModifiers[0] = TH_Hammer_freeze_XM;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// HAMMER-STRIKE ZODIACS

//
// The hammer strikes the ground twice, and the first consequence of 
// each is a glowing strike-zodiac as the hammer's god-power is 
// released.  The initial strike is the most forceful and the 
// primary one.
//

// these local offsets describe the locations of the primary and
//  secondary hammer-strikes; they are used often in all that follows
datablock afxXM_LocalOffsetData(TH_Strike1_Offset_XM)
{
  localOffset = "0.0 4.186 0.0";
};
datablock afxXM_LocalOffsetData(TH_Strike2_Offset_XM)
{
  localOffset = "0.0 4.933 0.0";
};

// the primary strike zodiac
datablock afxZodiacData(TH_ZodeStrike1_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_strike.png";
  radius = 6.0;   
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  growInTime = 0.15;
  shrinkOutTime = 0.30;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(TH_ZodeStrike1_Effect_EW)
{
  effect = TH_ZodeStrike1_CE;
  constraint = caster;
  lifetime = 0.5;
  delay    = 7.33;
  xfmModifiers[0] = "TH_Strike1_Offset_XM";
};

// the secondary strike zodiac, smaller radius
datablock afxZodiacData(TH_ZodeStrike2_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_strike.png";
  radius = 2.5;   
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  growInTime = 0.10;
  shrinkOutTime = 0.25;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(TH_ZodeStrike2_Effect_EW)
{
  effect = TH_ZodeStrike2_CE;
  constraint = caster;
  lifetime = 0.4;
  delay    = 8.34;
  xfmModifiers[0] = "TH_Strike2_Offset_XM";
};

%TH_HammerStrikeLight_intensity = 0.5;
datablock afxLightData(TH_HammerStrikeLight_CE)
{
  type = "Point";  
  radius = 60; //20;
  sgCastsShadows = true;

  //sgLightingModelName = "SG - Original Advanced (Lighting Pack)";
  sgLightingModelName = "Inverse Square Fast Falloff";
  color = 1.0*%TH_HammerStrikeLight_intensity SPC
          1.0*%TH_HammerStrikeLight_intensity SPC
          1.0*%TH_HammerStrikeLight_intensity;
};
//
datablock afxEffectWrapperData(TH_HammerStrikeLight_EW : TH_ZodeStrike1_Effect_EW)
{
  effect = TH_HammerStrikeLight_CE;
  //lifetime = 0.4;
  //fadeInTime = 0.5;
  //fadeOutTime = 0.5;
  lifetime = 0.1;
  fadeOutTime = 0.2;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// IMPACT WAVE

//
// The second consequence of the first impact is a growing wave
// zodiac with trailing particle dust.
//
// The dust particle emitters work by using cone emitters: both angles
// of the cone are set to their maximum, resulting in emission nearly
// in the shape of a disc; then, by setting "fadeOffset" and setting 
// the "ejectionOffset" to an appropriate value, as the particles
// fade-in they are emitted at an ever-increasing distance from the
// center or apex of the cone.  This creates the effect of a growing
// ring of particles.
//

// the impact wave zodiac generated by the primary strike
datablock afxZodiacData(TH_ZodeWave_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_wave.png";
  radius = 1.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  growthRate = 30.0;
  showOnInteriors = false;
};
//
datablock afxEffectWrapperData(TH_ZodeWave_Effect_EW)
{
  effect = TH_ZodeWave_CE;
  constraint = caster;  
  lifetime = 1.0;
  delay    = 7.33;
  fadeInTime  = 0.25;
  fadeOutTime = 0.5;
  xfmModifiers[0] = "TH_Strike1_Offset_XM";
};

// dust particles, small size, dark and light variations
datablock ParticleData(TH_HammerDust_P1)
{
   textureName          = %mySpellDataPath @ "/TH/particles/smoke";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 500;
   useInvAlpha          = true;
   spinRandomMin        = -60.0;
   spinRandomMax        = 60.0;
   colors[0]            = "0.4 0.33 0.2 0.0";  
   colors[1]            = "0.4 0.33 0.2 0.25";
   colors[2]            = "0.4 0.33 0.2 0.15";
   colors[3]            = "0.4 0.33 0.2 0.0";
   sizes[0]             = 0.8;
   sizes[1]             = 2.0;
   sizes[2]             = 3.5;
   sizes[3]             = 5.0;
   times[0]             = 0.0;
   times[1]             = 0.2;
   times[2]             = 0.7;
   times[3]             = 1.0;   
};
datablock ParticleData(TH_HammerDust_P2)
{
   textureName          = %mySpellDataPath @ "/TH/particles/smoke";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 500;
   useInvAlpha          = true;
   spinRandomMin        = -60.0;
   spinRandomMax        = 60.0;
   colors[0]            = "0.66 0.55 0.33 0.0";   
   colors[1]            = "0.66 0.55 0.33 0.25";
   colors[2]            = "0.66 0.55 0.33 0.15";
   colors[3]            = "0.66 0.55 0.33 0.0";
   sizes[0]             = 0.8;
   sizes[1]             = 2.0;
   sizes[2]             = 3.5;
   sizes[3]             = 5.0;
   times[0]             = 0.0;
   times[1]             = 0.2;
   times[2]             = 0.7;
   times[3]             = 1.0;   
};

// dust particles, large size, dark and light variations
datablock ParticleData(TH_HammerDustBig_P1)
{
   textureName          = %mySpellDataPath @ "/TH/particles/smoke";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 500;
   useInvAlpha          = true;
   spinRandomMin        = -90.0;
   spinRandomMax        = 90.0;
   colors[0]            = "0.4 0.33 0.2 0.0";  
   colors[1]            = "0.4 0.33 0.2 0.25";
   colors[2]            = "0.4 0.33 0.2 0.15";
   colors[3]            = "0.4 0.33 0.2 0.0";
   sizes[0]             = 5.0;
   sizes[1]             = 7.0;
   sizes[2]             = 4.0;
   sizes[3]             = 3.0;
   times[0]             = 0.0;
   times[1]             = 0.2;
   times[2]             = 0.7;
   times[3]             = 1.0;   
};
datablock ParticleData(TH_HammerDustBig_P2)
{
   textureName          = %mySpellDataPath @ "/TH/particles/smoke";
   dragCoeffiecient     = 0.5;
   gravityCoefficient   = 0.2;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 500;
   useInvAlpha          = true;
   spinRandomMin        = -90.0;
   spinRandomMax        = 90.0;
   colors[0]            = "0.66 0.55 0.33 0.0";   
   colors[1]            = "0.66 0.55 0.33 0.25";
   colors[2]            = "0.66 0.55 0.33 0.15";
   colors[3]            = "0.66 0.55 0.33 0.0";
   sizes[0]             = 5.0;
   sizes[1]             = 7.0;
   sizes[2]             = 4.0;
   sizes[3]             = 3.0;
   times[0]             = 0.0;
   times[1]             = 0.2;
   times[2]             = 0.7;
   times[3]             = 1.0;   
};

// large dust particle emitter used during the primary hammer-strike;
//  see above for description of how it works
datablock afxParticleEmitterConeData(TH_HammerDust_Strike1A_E) // TGEA
{
  ejectionOffset        = 30.0;
  ejectionPeriodMS      = 2;
  periodVarianceMS      = 1;
  ejectionVelocity      = 20.0;
  velocityVariance      = 5.0;  
  particles             = "TH_HammerDustBig_P1 TH_HammerDustBig_P2";

  // TGE emitterType = "cone";
  vector = "0.0 0.0 1.0";
  spreadMin = 179.0;
  spreadMax = 179.0;

  fadeOffset = true;
};
//
datablock afxEffectWrapperData(TH_HammerDust_Strike1A_EW)
{
  effect = TH_HammerDust_Strike1A_E;
  constraint = caster;
  lifetime = 1.5;
  delay    = 7.34;
  fadeInTime  = 1.5;
  xfmModifiers[0] = "TH_Strike1_Offset_XM";
};

// small dust particle emitter used during the primary hammer-strike;
//  see above for description of how it works
datablock afxParticleEmitterConeData(TH_HammerDust_Strike1B_E) // TGEA
{
  ejectionOffset        = 30.0;
  ejectionPeriodMS      = 2;
  periodVarianceMS      = 1;
  ejectionVelocity      = 10.0;
  velocityVariance      = 2.0;  
  particles             = "TH_HammerDust_P1 TH_HammerDust_P2";

  // TGE emitterType = "cone";
  vector = "0.0 0.0 1.0";
  spreadMin = 179.0;
  spreadMax = 179.0;

  fadeOffset = true;
};
//
datablock afxEffectWrapperData(TH_HammerDust_Strike1B_EW)
{
  effect = TH_HammerDust_Strike1B_E;
  constraint = caster;
  lifetime = 1.5;
  delay    = 7.34;  
  fadeInTime  = 1.5;
  xfmModifiers[0] = "TH_Strike1_Offset_XM";
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// GROUND-CRACK ZODIACS

//
// The third and final consequence of the primary hammer-strike is
// zodiacs that create a ground-cracked texture.  The effect is
// composed of three stages (A, B and C) each of which is a larger
// cracked area; they are revealed in sequence and attempt to create
// an impression of growing cracks.  Each stage is itself composed 
// of three zodiacs: first, a crack glow zodiac that reveals the
// other two and suggests the hammer's power coursing through the 
// earth; second, a crack texture that is blended subtractively; and
// third, a crack texture that is blended additively.  The reason for
// subtractive and additive blends is to create an effect that will
// work on any terrain regardless of the underlying texture.  The
// cracks then linger on for a time as residue.
//


// crack glow zodiac A (small)
datablock afxZodiacData(TH_ZodeCracksGlowA_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_crackGlowA.png";
  radius = 13.0;   
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(TH_ZodeCracksGlowA_EW)
{
  effect = TH_ZodeCracksGlowA_CE;
  levelOfDetailRange = 0;
  constraint = caster;
  lifetime = 1.15;
  delay    = 7.35;  
  fadeInTime  = 0.05;
  fadeOutTime = 0.25;
  xfmModifiers[0] = "TH_Strike1_Offset_XM";
};

// crack glow zodiac B (medium)
datablock afxZodiacData(TH_ZodeCracksGlowB_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_crackGlowB.png";
  radius = 13.0;   
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  showOnInteriors = false;
};
//
datablock afxEffectWrapperData(TH_ZodeCracksGlowB_EW)
{
  effect = TH_ZodeCracksGlowB_CE;
  levelOfDetailRange = 1;
  constraint = caster;
  lifetime = 0.95;
  delay    = 7.85;
  fadeInTime  = 0.05;
  fadeOutTime = 0.25;
  xfmModifiers[0] = "TH_Strike1_Offset_XM";
};

// crack glow zodiac C (large)
datablock afxZodiacData(TH_ZodeCracksGlowC_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_crackGlowC.png";
  radius = 13.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  showOnInteriors = false;
};
//
datablock afxEffectWrapperData(TH_ZodeCracksGlowC_EW)
{
  effect = TH_ZodeCracksGlowC_CE;
  levelOfDetailRange = 2;
  constraint = caster;
  lifetime = 0.75;
  delay    = 8.35;
  fadeInTime  = 0.05;
  fadeOutTime = 0.25;
  xfmModifiers[0] = "TH_Strike1_Offset_XM";
};

// subtractive crack zodiac A (small)
datablock afxZodiacData(TH_ZodeCracksA_dark_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_cracksA-dark.png";
  radius = 13.0;  
  color = "1.0 1.0 1.0 1.0";
  blend = subtractive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(TH_ZodeCracksA_dark_EW)
{
  effect = TH_ZodeCracksA_dark_CE;
  levelOfDetailRange = 0;
  constraint = caster;
  lifetime = 0.25;
  delay    = 7.65;  // 7.35+0.3
  fadeInTime  = 0.25;
  residueLifetime = 20;
  fadeOutTime = 5.0;
  xfmModifiers[0] = "TH_Strike1_Offset_XM";
};

// additive crack zodiac A (small)
datablock afxZodiacData(TH_ZodeCracksA_light_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_cracksA-light.png";
  radius = 13.0;
  color = "0.5 0.5 0.5 0.5";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(TH_ZodeCracksA_light_EW)
{
  effect = TH_ZodeCracksA_light_CE;
  levelOfDetailRange = 0;
  constraint = caster;
  lifetime = 0.25;
  delay    = 7.65;  // 7.35+0.3
  fadeInTime  = 0.25;
  residueLifetime = 20;
  fadeOutTime = 5.0;
  xfmModifiers[0] = "TH_Strike1_Offset_XM";
};

// subtractive crack zodiac B (medium)
datablock afxZodiacData(TH_ZodeCracksB_dark_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_cracksB-dark.png";
  radius = 13.0;   
  color = "0.8 0.8 0.8 0.8";
  blend = subtractive;
  showOnInteriors = false;
};
//
datablock afxEffectWrapperData(TH_ZodeCracksB_dark_EW)
{
  effect = TH_ZodeCracksB_dark_CE;
  levelOfDetailRange = 1;
  constraint = caster;
  lifetime = 0.25;
  delay    = 8.15;  // 7.85+0.3
  fadeInTime  = 0.2;
  residueLifetime = 20-(8.15-7.65);
  fadeOutTime = 5.0;
  xfmModifiers[0] = "TH_Strike1_Offset_XM";
};

// additive crack zodiac B (medium)
datablock afxZodiacData(TH_ZodeCracksB_light_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_cracksB-light.png";
  radius = 13.0;   
  color = "0.65 0.65 0.65 0.65";
  blend = additive;
  showOnInteriors = false;
};
//
datablock afxEffectWrapperData(TH_ZodeCracksB_light_EW)
{
  effect = TH_ZodeCracksB_light_CE;
  levelOfDetailRange = 1;
  constraint = caster; 
  lifetime = 0.25;
  delay    = 8.15;  // 7.85+0.3
  fadeInTime  = 0.2;
  residueLifetime = 20-(8.15-7.65);
  fadeOutTime = 5.0;
  xfmModifiers[0] = "TH_Strike1_Offset_XM";
};

// subtractive crack zodiac C (large)
datablock afxZodiacData(TH_ZodeCracksC_dark_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_cracksC-dark.png";
  radius = 13.0;   
  color = "0.6 0.6 0.6 0.6";
  blend = subtractive;
  showOnInteriors = false;
};
//
datablock afxEffectWrapperData(TH_ZodeCracksC_dark_EW)
{
  effect = TH_ZodeCracksC_dark_CE;
  levelOfDetailRange = 2;
  constraint = caster; 
  lifetime = 0.25;
  delay    = 8.65;  // 8.35+0.3
  fadeInTime  = 0.2;
  residueLifetime = 20-(8.65-7.65);
  fadeOutTime = 5.0;
  xfmModifiers[0] = "TH_Strike1_Offset_XM";
};

// additive crack zodiac C (large)
datablock afxZodiacData(TH_ZodeCracksC_light_CE)
{  
  texture = %mySpellDataPath @ "/TH/zodiacs/TH_cracksC-light.png";
  radius = 13.0;
  color = "0.6 0.6 0.6 0.6";
  blend = additive;
  showOnInteriors = false;
};
//
datablock afxEffectWrapperData(TH_ZodeCracksC_light_EW)
{
  effect = TH_ZodeCracksC_light_CE;
  levelOfDetailRange = 2;
  constraint = caster;
  lifetime = 0.25;
  delay    = 8.65;  // 8.35+0.3
  fadeInTime  = 0.2;
  residueLifetime = 20-(8.65-7.65);
  fadeOutTime = 5.0;
  xfmModifiers[0] = "TH_Strike1_Offset_XM";
};



//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SECONDARY DUST

// 
// A second and final consequence of the secondary hammer impact
// is its own dust ring, but much smaller than that of the primary
// impact.  This is just a subtle touch.
//

// small dust particle emitter used during the secondary hammer-strike;
//  see description under "IMPACT WAVE" for how it works
datablock afxParticleEmitterConeData(TH_HammerDust_Strike2_E) // TGEA
{
  ejectionOffset        = 4.5;
  ejectionPeriodMS      = 8;
  periodVarianceMS      = 2;
  ejectionVelocity      = 6.0;
  velocityVariance      = 1.5;  
  particles             = "TH_HammerDust_P1 TH_HammerDust_P2";

  // TGE emitterType = "cone";
  vector = "0.0 0.0 1.0";
  spreadMin = 179.0;
  spreadMax = 179.0;

  fadeOffset = true;
};
//
datablock afxEffectWrapperData(TH_HammerDust_Strike2_EW)
{
  effect = TH_HammerDust_Strike2_E;
  constraint = caster;
  lifetime = 0.6;
  delay    = 8.34;  
  fadeInTime  = 0.6;
  xfmModifiers[0] = "TH_Strike2_Offset_XM";
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// Camera Shake

datablock afxCameraShakeData(TH_CamShake_CE)
{
   frequency = "10.0 10.0 5.0";
   amplitude = "2.0 2.0 4.0";
   radius = 25.0;
   falloff = 10.0;
};

datablock afxEffectWrapperData(TH_CamShake_EW)
{
  effect = TH_CamShake_CE;
  posConstraint = caster;
  posConstraint2 = camera;
  lifetime = 3.0;
  delay  = 7.33;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// Damage

datablock afxDamageData(TH_tumble_damage_CE)
{
  label = "tumble";
  flavor = "fall";
  directDamage = 5;
};

datablock afxEffectWrapperData(TH_tumble_damage_EW)
{
  effect = TH_tumble_damage_CE;
  posConstraint = "caster";
  posConstraint2 = "caster";
  delay = 8.8;
};

datablock afxDamageData(TH_pound_damage_CE)
{
  label = "pounding";
  flavor = "area";
  areaDamage = 50;
  areaDamageRadius = 25;
  areaDamageImpulse = 1200;
};

datablock afxEffectWrapperData(TH_pound_damage_EW)
{
  effect = TH_pound_damage_CE;
  posConstraint = "caster";
  posConstraint2 = "caster";
  delay = 7.5;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// Sounds

datablock AudioProfile(TH_ZodiacSnd_CE)
{
   fileName = %mySpellDataPath @ "/TH/sounds/TH_zodiac.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(TH_ZodiacSnd_EW)
{
  effect = TH_ZodiacSnd_CE;
  constraint = "caster";
  delay = 0.0;
  //scaleFactor = 0.8;
  lifetime = 3.543;
};

datablock AudioProfile(TH_BodyfallSnd_CE)
{
   fileName = %mySpellDataPath @ "/TH/sounds/TH_bodyfall.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(TH_BodyfallSnd_EW)
{
  effect = TH_BodyfallSnd_CE;
  constraint = "caster";
  delay = 8.8;
  //scaleFactor = 0.8;
  lifetime = 0.603;
};

datablock AudioProfile(TH_DropcatchSnd_CE)
{
   fileName = %mySpellDataPath @ "/TH/sounds/TH_dropcatch.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(TH_DropcatchSnd_EW)
{
  effect = TH_DropcatchSnd_CE;
  constraint = "caster";
  delay = 1.8;
  //scaleFactor = 0.8;
  lifetime = 1.526;
};

//datablock AudioDescription(TH_SpellAudioCasting_loud_AD : SpellAudioCasting_loud_AD)
//{
//  volume = 2.0;
//};
datablock AudioProfile(TH_ImpactSnd_CE)
{
   fileName = %mySpellDataPath @ "/TH/sounds/TH_impact_louder.ogg"; //TH_impact.ogg";
   description = SpellAudioCasting_loud_AD;
   preload = false;
};
datablock afxEffectWrapperData(TH_ImpactSnd_EW)
{
  effect = TH_ImpactSnd_CE;
  constraint = "caster";
  delay = 7.3;
  //scaleFactor = 0.8;
  //scaleFactor = 2.0;
  lifetime = 3.525;
};

datablock AudioProfile(TH_ThunderElecSnd_CE)
{
   fileName = %mySpellDataPath @ "/TH/sounds/TH_thunderElec_bed.ogg";
   description = SpellAudioCasting_loud_AD;
   preload = false;
};
datablock afxEffectWrapperData(TH_ThunderElecSnd_EW)
{
  effect = TH_ThunderElecSnd_CE;
  constraint = "caster";
  delay = 0.0;
  //scaleFactor = 0.8;
  lifetime = 7.867;
};

datablock AudioProfile(TH_TossSnd_CE)
{
   fileName = %mySpellDataPath @ "/TH/sounds/TH_toss.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(TH_TossSnd_EW)
{
  effect = TH_TossSnd_CE;
  constraint = "caster";
  delay = 5.9;
  //scaleFactor = 0.8;
  lifetime = 2.15;
};

datablock AudioProfile(TH_WindupSnd_CE)
{
   fileName = %mySpellDataPath @ "/TH/sounds/TH_windup.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(TH_WindupSnd_EW)
{
  effect = TH_WindupSnd_CE;
  constraint = "caster";
  delay = 2.2;
  //scaleFactor = 0.8;
  lifetime = 3.346;
};

datablock afxXM_LocalOffsetData(TH_ThunderSnd_offset1_XM)
{
  localOffset = "50 3 50";
};
datablock AudioProfile(TH_ThunderSnd_CE)
{
   fileName = %mySpellDataPath @ "/TH/sounds/thunder4.ogg";
   description = SpellAudioImpact_AD;
   preload = false;
};
datablock afxEffectWrapperData(TH_ThunderSnd_EW)
{
  effect = TH_ThunderSnd_CE;
  constraint = "caster";
  delay = 12.2;
  lifetime = 5.207;
  xfmModifiers[0] = TH_ThunderSnd_offset1_XM;
};

datablock afxXM_LocalOffsetData(TH_ThunderSnd_offset2_XM)
{
  localOffset = "-40 -3 40";
};
datablock AudioProfile(TH_ThunderSnd_CE)
{
   fileName = %mySpellDataPath @ "/TH/sounds/thunder1.ogg";
   description = SpellAudioImpact_AD;
   preload = false;
};
datablock afxEffectWrapperData(TH_Thunder2Snd_EW)
{
  effect = TH_ThunderSnd_CE;
  constraint = "caster";
  delay = 11.0;
  //scaleFactor = 0.5;
  lifetime = 3.233;
  xfmModifiers[0] = TH_ThunderSnd_offset2_XM;
};


datablock AudioDescription(TH_SpellAudioCasting_soft_AD : SpellAudioCasting_AD)
{
  volume = 0.06;  // threshold = .06-.07
};
datablock AudioDescription(TH_SpellAudioCasting_softer_AD : SpellAudioCasting_AD)
{
  volume = 0.01;
};

datablock AudioProfile(TH_CasterLightningSnd_1_CE)
{
  fileName = %mySpellDataPath @ "/TH/sounds/TH_lightningEdit_1-4.ogg";
  description = SpellAudioCasting_AD;
  preload = false;
};
datablock AudioProfile(TH_CasterLightningSnd_2_CE)
{
  fileName = %mySpellDataPath @ "/TH/sounds/TH_lightningEdit_2-4.ogg";
  description = SpellAudioCasting_AD;
  preload = false;
};
datablock AudioProfile(TH_CasterLightningSnd_3_CE)
{
  fileName = %mySpellDataPath @ "/TH/sounds/TH_lightningEdit_3-2.ogg";
  description = SpellAudioCasting_AD;
  preload = false;
};
datablock AudioProfile(TH_CasterLightningSnd_4_CE)
{
  fileName = %mySpellDataPath @ "/TH/sounds/TH_lightningEdit_4.ogg";
  description = SpellAudioCasting_AD;
  preload = false;
};
datablock AudioProfile(TH_CasterLightningSnd_5_CE)
{
  fileName = %mySpellDataPath @ "/TH/sounds/TH_lightningEdit_5-2.ogg";
  description = SpellAudioCasting_AD;
  preload = false;
};

//datablock AudioProfile(TH_CasterLightningSnd_1_softer_CE : TH_CasterLightningSnd_1_soft_CE)
//{
//  fileName = %mySpellDataPath @ "/TH/sounds/TH_lightningEdit_1-4--soft.ogg";
//  //description = TH_SpellAudioCasting_softer_AD;
//};
//datablock AudioProfile(TH_CasterLightningSnd_2_softer_CE : TH_CasterLightningSnd_2_soft_CE)
//{
//  fileName = %mySpellDataPath @ "/TH/sounds/TH_lightningEdit_2-4--soft.ogg";//--softer.ogg";
//  //description = TH_SpellAudioCasting_softer_AD;
//};
//datablock AudioProfile(TH_CasterLightningSnd_3_softer_CE : TH_CasterLightningSnd_3_soft_CE)
//{
//  fileName = %mySpellDataPath @ "/TH/sounds/TH_lightningEdit_3-2--soft.ogg";
//  //description = TH_SpellAudioCasting_softer_AD;
//};
//datablock AudioProfile(TH_CasterLightningSnd_4_softer_CE : TH_CasterLightningSnd_4_soft_CE)
//{
//  fileName = %mySpellDataPath @ "/TH/sounds/TH_lightningEdit_4--soft.ogg";
//  //description = TH_SpellAudioCasting_softer_AD;
//};
//datablock AudioProfile(TH_CasterLightningSnd_5_softer_CE : TH_CasterLightningSnd_5_soft_CE)
//{
//  fileName = %mySpellDataPath @ "/TH/sounds/TH_lightningEdit_5-2--soft.ogg";
//  //description = TH_SpellAudioCasting_softer_AD;
//};

%TH_CasterLightningSnd_1_lifetime = 0.5018;
%TH_CasterLightningSnd_2_lifetime = 0.4233;
%TH_CasterLightningSnd_3_lifetime = 0.5018;
%TH_CasterLightningSnd_4_lifetime = 1.1304;
%TH_CasterLightningSnd_5_lifetime = 0.9977;

%TH_ZodeLightning_volume   = 0.70; //0.85;
%TH_CasterLightning_volume = 0.86; //0.95;

datablock afxEffectWrapperData(TH_ZodeLightningSnd1_EW : TH_ZodeLightningEffect1_EW)
{
  effect = TH_CasterLightningSnd_1_CE;
  lifetime = %TH_CasterLightningSnd_1_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd2_EW : TH_ZodeLightningEffect2_EW)
{
  effect = TH_CasterLightningSnd_2_CE;
  lifetime = %TH_CasterLightningSnd_2_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd3_EW : TH_ZodeLightningEffect3_EW)
{
  effect = TH_CasterLightningSnd_4_CE;
  lifetime = %TH_CasterLightningSnd_4_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd4_EW : TH_ZodeLightningEffect4_EW)
{
  effect = TH_CasterLightningSnd_3_CE;
  lifetime = %TH_CasterLightningSnd_3_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd5_EW : TH_ZodeLightningEffect5_EW)
{
  effect = TH_CasterLightningSnd_5_CE;
  lifetime = %TH_CasterLightningSnd_5_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd6_EW : TH_ZodeLightningEffect6_EW)
{
  effect = TH_CasterLightningSnd_2_CE;
  lifetime = %TH_CasterLightningSnd_2_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd7_EW : TH_ZodeLightningEffect7_EW)
{
  effect = TH_CasterLightningSnd_2_CE;
  lifetime = %TH_CasterLightningSnd_2_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd8_EW : TH_ZodeLightningEffect8_EW)
{
  effect = TH_CasterLightningSnd_4_CE;
  lifetime = %TH_CasterLightningSnd_4_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd9_EW : TH_ZodeLightningEffect9_EW)
{
  effect = TH_CasterLightningSnd_5_CE;
  lifetime = %TH_CasterLightningSnd_5_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd10_EW : TH_ZodeLightningEffect10_EW)
{
  effect = TH_CasterLightningSnd_1_CE;
  lifetime = %TH_CasterLightningSnd_1_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd11_EW : TH_ZodeLightningEffect11_EW)
{
  effect = TH_CasterLightningSnd_4_CE;
  lifetime = %TH_CasterLightningSnd_4_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd12_EW : TH_ZodeLightningEffect12_EW)
{
  effect = TH_CasterLightningSnd_5_CE;
  lifetime = %TH_CasterLightningSnd_5_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd13_EW : TH_ZodeLightningEffect13_EW)
{
  effect = TH_CasterLightningSnd_5_CE;
  lifetime = %TH_CasterLightningSnd_5_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd14_EW : TH_ZodeLightningEffect14_EW)
{
  effect = TH_CasterLightningSnd_1_CE;
  lifetime = %TH_CasterLightningSnd_1_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd15_EW : TH_ZodeLightningEffect15_EW)
{
  effect = TH_CasterLightningSnd_3_CE;
  lifetime = %TH_CasterLightningSnd_3_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd16_EW : TH_ZodeLightningEffect16_EW)
{
  effect = TH_CasterLightningSnd_5_CE;
  lifetime = %TH_CasterLightningSnd_5_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd17_EW : TH_ZodeLightningEffect17_EW)
{
  effect = TH_CasterLightningSnd_2_CE;
  lifetime = %TH_CasterLightningSnd_2_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd18_EW : TH_ZodeLightningEffect18_EW)
{
  effect = TH_CasterLightningSnd_5_CE;
  lifetime = %TH_CasterLightningSnd_5_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd19_EW : TH_ZodeLightningEffect19_EW)
{
  effect = TH_CasterLightningSnd_4_CE;
  lifetime = %TH_CasterLightningSnd_4_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_ZodeLightningSnd20_EW : TH_ZodeLightningEffect20_EW)
{
  effect = TH_CasterLightningSnd_3_CE;
  lifetime = %TH_CasterLightningSnd_3_lifetime;
  scaleFactor = %TH_ZodeLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};

datablock afxEffectWrapperData(TH_CasterLightningSnd1_EW : TH_CasterLightningFlashLight1_EW)
{
  effect = TH_CasterLightningSnd_1_CE;
  lifetime = %TH_CasterLightningSnd_1_lifetime;
  scaleFactor = %TH_CasterLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_CasterLightningSnd2_EW : TH_CasterLightningFlashLight2_EW)
{
  effect = TH_CasterLightningSnd_3_CE;
  lifetime = %TH_CasterLightningSnd_3_lifetime;
  scaleFactor = %TH_CasterLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_CasterLightningSnd3_EW : TH_CasterLightningFlashLight3_EW)
{
  effect = TH_CasterLightningSnd_2_CE;
  lifetime = %TH_CasterLightningSnd_2_lifetime;
  scaleFactor = %TH_CasterLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_CasterLightningSnd4_EW : TH_CasterLightningFlashLight4_EW)
{
  effect = TH_CasterLightningSnd_5_CE;
  lifetime = %TH_CasterLightningSnd_5_lifetime;
  scaleFactor = %TH_CasterLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_CasterLightningSnd5_EW : TH_CasterLightningFlashLight5_EW)
{
  effect = TH_CasterLightningSnd_4_CE;
  lifetime = %TH_CasterLightningSnd_4_lifetime;
  scaleFactor = %TH_CasterLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_CasterLightningSnd6_EW : TH_CasterLightningFlashLight6_EW)
{
  effect = TH_CasterLightningSnd_3_CE;
  lifetime = %TH_CasterLightningSnd_3_lifetime;
  scaleFactor = %TH_CasterLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_CasterLightningSnd7_EW : TH_CasterLightningFlashLight7_EW)
{
  effect = TH_CasterLightningSnd_1_CE;
  lifetime = %TH_CasterLightningSnd_1_lifetime;
  scaleFactor = %TH_CasterLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_CasterLightningSnd8_EW : TH_CasterLightningFlashLight8_EW)
{
  effect = TH_CasterLightningSnd_5_CE;
  lifetime = %TH_CasterLightningSnd_5_lifetime;
  scaleFactor = %TH_CasterLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_CasterLightningSnd9_EW : TH_CasterLightningFlashLight9_EW)
{
  effect = TH_CasterLightningSnd_2_CE;
  lifetime = %TH_CasterLightningSnd_2_lifetime;
  scaleFactor = %TH_CasterLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};
datablock afxEffectWrapperData(TH_CasterLightningSnd10_EW : TH_CasterLightningFlashLight10_EW)
{
  effect = TH_CasterLightningSnd_1_CE;
  lifetime = %TH_CasterLightningSnd_1_lifetime;
  scaleFactor = %TH_CasterLightning_volume;
  fadeInTime = 0;
  fadeOutTime = 0;
};



datablock afxAnimLockData(TH_AnimLock_CE)
{
  priority = 0;
};
//
datablock afxEffectWrapperData(TH_AnimLock_EW)
{
  effect = TH_AnimLock_CE;
  delay = 2.5;
  constraint = caster;
  lifetime = 10.0-2.5;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// THOR'S HAMMER SPELL
//

datablock afxMagicSpellData(ThorsHammerSpell)
{
  castingDur = 3.6; //10
 
    // spellcaster animation //
  addCastingEffect = TH_Swing_Clip_EW;
  addCastingEffect = TH_AnimLock_EW;
    // casting zodiac //
  addCastingEffect = TH_ZodeEffect_Reveal_EW;
  addCastingEffect = TH_MainZodeRevealLight_1_EW;
  addCastingEffect = TH_MainZodeRevealLight_2_EW;
  addCastingEffect = TH_MainZodeRevealLight_3_EW;
  addCastingEffect = TH_Zode1_TLKunderglow_EW;
  addCastingEffect = TH_ZodeEffect1_EW;
  addCastingEffect = TH_ZodeEffect2_EW;
  addCastingEffect = $TH_CastingZodeLight;
    // casting zodiac lightning
  addCastingEffect = TH_ZodeLightningEffect1_EW;
  addCastingEffect = TH_ZodeLightningEffect2_EW;
  addCastingEffect = TH_ZodeLightningEffect3_EW;
  addCastingEffect = TH_ZodeLightningEffect4_EW;
  addCastingEffect = TH_ZodeLightningEffect5_EW;
  addCastingEffect = TH_ZodeLightningEffect6_EW;
  addCastingEffect = TH_ZodeLightningEffect7_EW;
  addCastingEffect = TH_ZodeLightningEffect8_EW;
  addCastingEffect = TH_ZodeLightningEffect9_EW;
  addCastingEffect = TH_ZodeLightningEffect10_EW;
  addCastingEffect = TH_ZodeLightningEffect11_EW;
  addCastingEffect = TH_ZodeLightningEffect12_EW;
  addCastingEffect = TH_ZodeLightningEffect13_EW;
  addCastingEffect = TH_ZodeLightningEffect14_EW;
  addCastingEffect = TH_ZodeLightningEffect15_EW;
  addCastingEffect = TH_ZodeLightningEffect16_EW;
  addCastingEffect = TH_ZodeLightningEffect17_EW;
  addCastingEffect = TH_ZodeLightningEffect18_EW;
  addCastingEffect = TH_ZodeLightningEffect19_EW;
  addCastingEffect = TH_ZodeLightningEffect20_EW;
  addCastingEffect = $TH_ZodeLightningFlash1;
  addCastingEffect = $TH_ZodeLightningFlash2;
  addCastingEffect = $TH_ZodeLightningFlash3;
  addCastingEffect = $TH_ZodeLightningFlash4;
  addCastingEffect = $TH_ZodeLightningFlash5;
  addCastingEffect = $TH_ZodeLightningFlash6;
  addCastingEffect = $TH_ZodeLightningFlash7;
  addCastingEffect = $TH_ZodeLightningFlash8;
  addCastingEffect = $TH_ZodeLightningFlash9;
  addCastingEffect = $TH_ZodeLightningFlash10;
  addCastingEffect = $TH_ZodeLightningFlash11;
  addCastingEffect = $TH_ZodeLightningFlash12;
  addCastingEffect = $TH_ZodeLightningFlash13;
  addCastingEffect = $TH_ZodeLightningFlash14;
  addCastingEffect = $TH_ZodeLightningFlash15;
  addCastingEffect = $TH_ZodeLightningFlash16;
  addCastingEffect = $TH_ZodeLightningFlash17;
  addCastingEffect = $TH_ZodeLightningFlash18;
  addCastingEffect = $TH_ZodeLightningFlash19;
  addCastingEffect = $TH_ZodeLightningFlash20;
	  // caster lightning //
  addCastingEffect = TH_CasterLightning_EW;
  addCastingEffect = TH_CasterLightningFlashLight1_EW;
  addCastingEffect = TH_CasterLightningFlashLight2_EW;
  addCastingEffect = TH_CasterLightningFlashLight3_EW;
  addCastingEffect = TH_CasterLightningFlashLight4_EW;
  addCastingEffect = TH_CasterLightningFlashLight5_EW;
  addCastingEffect = TH_CasterLightningFlashLight6_EW;
  addCastingEffect = TH_CasterLightningFlashLight7_EW;
  addCastingEffect = TH_CasterLightningFlashLight8_EW;
  addCastingEffect = TH_CasterLightningFlashLight9_EW;
  addCastingEffect = TH_CasterLightningFlashLight10_EW;
  addCastingEffect = TH_CasterLightningFlashWhiteLight1_EW;
  addCastingEffect = TH_CasterLightningFlashWhiteLight2_EW;
  addCastingEffect = TH_CasterLightningFlashWhiteLight3_EW;
  addCastingEffect = TH_CasterLightningFlashWhiteLight4_EW;
  addCastingEffect = TH_CasterLightningFlashWhiteLight5_EW;
  addCastingEffect = TH_CasterLightningFlashWhiteLight6_EW;
  addCastingEffect = TH_CasterLightningFlashWhiteLight7_EW;
  addCastingEffect = TH_CasterLightningFlashWhiteLight8_EW;
  addCastingEffect = TH_CasterLightningFlashWhiteLight9_EW;
  addCastingEffect = TH_CasterLightningFlashWhiteLight10_EW;
    // the light from above //
  addCastingEffect = TH_LightBeam_EW;
  addCastingEffect = TH_GlowBall_EW;
  addCastingEffect = TH_Auras_EW;
  addCastingEffect = TH_AuraLightning_EW;
  addCastingEffect = TH_LightFromAbove_EW;
    // hammer model //
  addCastingEffect = TH_Hammer_EW;  
    // impact wave //
  addCastingEffect = TH_ZodeWave_Effect_EW;
  addCastingEffect = TH_HammerDust_Strike1A_EW;
  addCastingEffect = TH_HammerDust_Strike1B_EW;
    // ground-crack zodiacs //
  addCastingEffect = TH_ZodeCracksGlowA_EW;
  addCastingEffect = TH_ZodeCracksA_dark_EW;
  addCastingEffect = TH_ZodeCracksA_light_EW;
  addCastingEffect = TH_ZodeCracksGlowB_EW;
  addCastingEffect = TH_ZodeCracksB_dark_EW;
  addCastingEffect = TH_ZodeCracksB_light_EW;
  addCastingEffect = TH_ZodeCracksGlowC_EW;
  addCastingEffect = TH_ZodeCracksC_dark_EW;
  addCastingEffect = TH_ZodeCracksC_light_EW;
    // hammer-strike zodiacs //
    // (note: TH_ZodeStrike2_Effect_EW should be composited over
    //        the cracks, so it is added after)
  addCastingEffect = TH_ZodeStrike1_Effect_EW;
  addCastingEffect = TH_ZodeStrike2_Effect_EW;
  addCastingEffect = TH_HammerStrikeLight_EW;
    // secondary dust //  
  addCastingEffect = TH_HammerDust_Strike2_EW;
    // camera shake //
  addCastingEffect = TH_CamShake_EW;
    // damage //
  addCastingEffect = TH_tumble_damage_EW;
  addCastingEffect = TH_pound_damage_EW;
    // sounds //
  addCastingEffect = TH_ZodiacSnd_EW;
  addCastingEffect = TH_BodyfallSnd_EW;
  addCastingEffect = TH_DropcatchSnd_EW;
  addCastingEffect = TH_ImpactSnd_EW;
  addCastingEffect = TH_ThunderElecSnd_EW;
  addCastingEffect = TH_TossSnd_EW;
  addCastingEffect = TH_WindupSnd_EW;
  addCastingEffect = TH_ThunderSnd_EW;
  addCastingEffect = TH_Thunder2Snd_EW;

  addCastingEffect = TH_ZodeLightningSnd1_EW;
  addCastingEffect = TH_ZodeLightningSnd2_EW;
  addCastingEffect = TH_ZodeLightningSnd3_EW;
  addCastingEffect = TH_ZodeLightningSnd4_EW;
  addCastingEffect = TH_ZodeLightningSnd5_EW;
  addCastingEffect = TH_ZodeLightningSnd6_EW;
  addCastingEffect = TH_ZodeLightningSnd7_EW;
  addCastingEffect = TH_ZodeLightningSnd8_EW;
  addCastingEffect = TH_ZodeLightningSnd9_EW;
  addCastingEffect = TH_ZodeLightningSnd10_EW;
  addCastingEffect = TH_ZodeLightningSnd11_EW;
  addCastingEffect = TH_ZodeLightningSnd12_EW;
  addCastingEffect = TH_ZodeLightningSnd13_EW;
  addCastingEffect = TH_ZodeLightningSnd14_EW;
  addCastingEffect = TH_ZodeLightningSnd15_EW;
  addCastingEffect = TH_ZodeLightningSnd16_EW;
  addCastingEffect = TH_ZodeLightningSnd17_EW;
  addCastingEffect = TH_ZodeLightningSnd18_EW;
  addCastingEffect = TH_ZodeLightningSnd19_EW;
  addCastingEffect = TH_ZodeLightningSnd20_EW;

  addCastingEffect = TH_CasterLightningSnd1_EW;
  addCastingEffect = TH_CasterLightningSnd2_EW;
  addCastingEffect = TH_CasterLightningSnd3_EW;
  addCastingEffect = TH_CasterLightningSnd4_EW;
  addCastingEffect = TH_CasterLightningSnd5_EW;
  addCastingEffect = TH_CasterLightningSnd6_EW;
  addCastingEffect = TH_CasterLightningSnd7_EW;
  addCastingEffect = TH_CasterLightningSnd8_EW;
  addCastingEffect = TH_CasterLightningSnd9_EW;
  addCastingEffect = TH_CasterLightningSnd10_EW;
};
//
datablock afxRPGMagicSpellData(ThorsHammerSpell_RPG)
{
  name = "Thor's Hammer";
  desc = "Beg the thunder-god for one swing of his mighty HAMMER! " @
         "A divine gound-pounder inflicting 50 damage in a 25 meter radius." @
         "\n" @
         "\nspell design: Matthew Durante" @
         "\nsound effects: Dave Schroeder" @ 
         "\nspell concept: Matthew Durante";
  sourcePack = "Spell Pack 1";

  iconBitmap = %mySpellDataPath @ "/TH/icons/th";
  target = "nothing";
  manaCost = 10;

  castingDur = ThorsHammerSpell.castingDur;
};

// set a level of detail
function ThorsHammerSpell::onActivate(%this, %spell, %caster, %target)
{
  Parent::onActivate(%this, %spell, %caster, %target);
  %spell.setLevelOfDetail($pref::AFX::fxLevelOfDetail);
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
  ThorsHammerSpell.scriptFile = $afxAutoloadScriptFile;
  ThorsHammerSpell.spellDataPath = %mySpellDataPath;
  if (isFunction(addDemoSpellbookSpell))
    addDemoSpellbookSpell(ThorsHammerSpell, ThorsHammerSpell_RPG);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//