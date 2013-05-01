
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// FLAME BROIL SPELL
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
$spell_reload = isObject(FlameBroilSpell);
if ($spell_reload)
{
  // mark datablocks so we can detect which are reloaded this script
  markDataBlocks();
  // reset data path from previously saved value
  %mySpellDataPath = FlameBroilSpell.spellDataPath;
  FlameBroilSpell.reset();
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
// The casting zodiac is created with three primary
// layers and two glows. Using additive blends helps
// to suggest that the patterns are projected light.
// The SHARED_freeze_XM transform modifier (defined in
// another file) is used to lock some of the zodiacs
// to their initial constraint positions.
// 

// A white glowing zodiac that fades in and
// out at start of casting-time
datablock afxZodiacData(FB_ZodeReveal_CE)
{  
  texture = %mySpellDataPath @ "/FB/zodiacs/FB_caster_reveal";
  radius = 3.0;
  startAngle = 187.5; 
  rotationRate = -30.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(FB_ZodeReveal_EW)
{
  effect = FB_ZodeReveal_CE;
  posConstraint = caster;
  lifetime = 0.75;
  fadeInTime = 0.5;
  fadeOutTime = 0.25;
  xfmModifiers[0] = SHARED_freeze_XM;
};

%FB_MainZodeRevealLight_intensity = 2.5;

datablock afxXM_LocalOffsetData(FB_MainZodeRevealLight_offset_XM)
{
  localOffset = "0 2 -4";
};
datablock afxXM_SpinData(FB_MainZodeRevealLight_spin2_XM : SHARED_MainZodeRevealLight_spin1_XM)
{
  spinAngle = 120;
};
datablock afxXM_SpinData(FB_MainZodeRevealLight_spin3_XM : SHARED_MainZodeRevealLight_spin1_XM)
{
  spinAngle = 240;
};

datablock afxLightData(FB_MainZodeRevealLight_CE) // AFX-SPOTLIGHT
{
  type = "Spot";
  radius = ($isTGEA) ? 10 : 5;
  direction = "0 -0.313 0.95";
  sgCastsShadows = false;
  color = 1.0*%FB_MainZodeRevealLight_intensity SPC
          1.0*%FB_MainZodeRevealLight_intensity SPC
          1.0*%FB_MainZodeRevealLight_intensity;
  lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF; // TGEA (ignored by TGE)
  sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Near Linear";
};

datablock afxEffectWrapperData(FB_MainZodeRevealLight_1_EW : FB_ZodeReveal_EW)
{
  effect = FB_MainZodeRevealLight_CE;
  xfmModifiers[1] = SHARED_MainZodeRevealLight_spin1_XM;
  xfmModifiers[2] = FB_MainZodeRevealLight_offset_XM;
};
datablock afxEffectWrapperData(FB_MainZodeRevealLight_2_EW : FB_ZodeReveal_EW)
{
  effect = FB_MainZodeRevealLight_CE;
  xfmModifiers[1] = FB_MainZodeRevealLight_spin2_XM;
  xfmModifiers[2] = FB_MainZodeRevealLight_offset_XM;
};
datablock afxEffectWrapperData(FB_MainZodeRevealLight_3_EW : FB_ZodeReveal_EW)
{
  effect = FB_MainZodeRevealLight_CE;
  xfmModifiers[1] = FB_MainZodeRevealLight_spin3_XM;
  xfmModifiers[2] = FB_MainZodeRevealLight_offset_XM;
};

// An orange/red zodiac.
datablock afxZodiacData(FB_Zode1_CE)
{  
  texture = %mySpellDataPath @ "/FB/zodiacs/FB_caster";
  radius = 3.0;
  startAngle = 180.0;
  rotationRate = -30.0;
  color = "1.0 1.0 1.0 0.8";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(FB_Zode1_EW)
{
  effect = FB_Zode1_CE;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 3.0;
  xfmModifiers[0] = SHARED_freeze_XM;
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
datablock afxZodiacData(FB_Zode1_TLKunderglow_CE : FB_Zode1_CE)
{
  texture = %mySpellDataPath @ "/FB/zodiacs/FB_caster-underglow256";
  color = "0.5 0.5 0.5 0.5";
  blend = normal;
};
//
datablock afxEffectWrapperData(FB_Zode1_TLKunderglow_EW : FB_Zode1_EW)
{
  effect = FB_Zode1_TLKunderglow_CE;
  execConditions = $BrightLighting_mask;
};

// A sparse orange zodiac with inner rune ring
// and outer skulls.
datablock afxZodiacData(FB_Zode2_CE)
{  
  texture = %mySpellDataPath @ "/FB/zodiacs/zode_text";
  radius = 3.0;
  startAngle = 180.0;
  rotationRate = 20.0;
  color = "1.0 0.0 0.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(FB_Zode2_EW)
{
  effect = FB_Zode2_CE;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 3.0;
  xfmModifiers[0] = SHARED_freeze_XM;
};

// Runes & Skulls Zode Underglow
//  Here the zode is made black but only slightly opaque to subtly
//  darken the ground, making the additive glow zode appear more
//  saturated.
datablock afxZodiacData(FB_Zode2_TLKunderglow_CE : FB_Zode2_CE)
{
  color = "0 0 0 0.25";
  blend = normal;
};
//
datablock afxEffectWrapperData(FB_Zode2_TLKunderglow_EW : FB_Zode2_EW)
{
  effect = FB_Zode2_TLKunderglow_CE;
  execConditions = $BrightLighting_mask;
};

// A white zodiac with sketchy runes. 
datablock afxZodiacData(FB_Zode3_CE)
{  
  texture = %mySpellDataPath @ "/FB/zodiacs/FB_caster-symbols";
  radius = 3.0;
  startAngle = 180.0;
  rotationRate = -30.0;
  color = "1.0 1.0 1.0 0.5";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(FB_Zode3_EW)
{
  effect = FB_Zode3_CE;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 3.0;
  xfmModifiers[0] = SHARED_freeze_XM;
};

// A white glowing zodiac that fades in and out at middle
// of casting-time. It establishes the circle where the 
// ring-of-fire emerges.
datablock afxZodiacData(FB_ZodeGlowRing_CE)
{  
  texture = %mySpellDataPath @ "/FB/zodiacs/FB_glowring";
  radius = 3.0;
  startAngle = 165.0;
  rotationRate = -30.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(FB_ZodeGlowRing_EW)
{
  effect = FB_ZodeGlowRing_CE;
  posConstraint = caster;
  delay = 0.6;
  lifetime = 1.25;
  fadeInTime = 0.6;
  fadeOutTime = 0.25;
  xfmModifiers[0] = SHARED_freeze_XM;
};

%FB_ZodeGlowRingLight_intensity = 2.5;

datablock afxXM_LocalOffsetData(FB_ZodeGlowRingLight_offset_XM)
{
  localOffset = "0 2 -4";
};
datablock afxXM_SpinData(FB_ZodeGlowRingLight_spin1_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 0-60;
  spinRate = -30;
};
datablock afxXM_SpinData(FB_ZodeGlowRingLight_spin2_XM : FB_ZodeGlowRingLight_spin1_XM)
{
  spinAngle = 120-60;
};
datablock afxXM_SpinData(FB_ZodeGlowRingLight_spin3_XM : FB_ZodeGlowRingLight_spin1_XM)
{
  spinAngle = 240-60;
};

// 
datablock afxLightData(FB_ZodeGlowRingLight_CE) // AFX-SPOTLIGHT
{
  type = "Spot";
  radius = ($isTGEA) ? 10 : 5;
  direction = "0 -0.313 0.95";
  sgCastsShadows = false;
  color = 1.0*%FB_ZodeGlowRingLight_intensity SPC
          1.0*%FB_ZodeGlowRingLight_intensity SPC
          1.0*%FB_ZodeGlowRingLight_intensity;
  lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF; // TGEA (ignored by TGE)
  sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Near Linear";
};

datablock afxEffectWrapperData(FB_ZodeGlowRingLight_1_EW : FB_ZodeGlowRing_EW)
{
  effect = FB_ZodeGlowRingLight_CE;
  lifetime = 0.7; //1.25;

  xfmModifiers[1] = FB_ZodeGlowRingLight_spin1_XM;
  xfmModifiers[2] = FB_ZodeGlowRingLight_offset_XM;
};
datablock afxEffectWrapperData(FB_ZodeGlowRingLight_2_EW : FB_ZodeGlowRingLight_1_EW)
{
  xfmModifiers[1] = FB_ZodeGlowRingLight_spin2_XM;
  xfmModifiers[2] = FB_ZodeGlowRingLight_offset_XM;
};
datablock afxEffectWrapperData(FB_ZodeGlowRingLight_3_EW : FB_ZodeGlowRingLight_1_EW)
{
  xfmModifiers[1] = FB_ZodeGlowRingLight_spin3_XM;
  xfmModifiers[2] = FB_ZodeGlowRingLight_offset_XM;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// RING OF FIRE

//
// The ring-of-fire is created by layering ten emitters that 
// orbit the caster at ground level. To fill out the ring, 
// the emitters start out at different angles distributed 
// around the circle, and also move at varying rates in both
// clockwise and counter-clockwise directions.  
//

//
// This particle system is the main component used in the
// ring-of-fire and also by the fireball missile.
//
datablock ParticleData(FB_Flames_P)
{
   textureName          = %mySpellDataPath @ "/FB/particles/FB_fireZodiac";
   dragCoeffiecient     = 0.5;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 900;
   lifetimeVarianceMS   = 200;
   useInvAlpha          = false;
   spinRandomMin        = -720.0;
   spinRandomMax        = 720.0;
   colors[0]            = "1.0 1.0 1.0 1.0";
   colors[1]            = "1.0 1.0 0.0 1.0";
   colors[2]            = "1.0 0.0 0.0 1.0";
   colors[3]            = "1.0 0.0 0.0 0.0";
   sizes[0]             = 0.6;
   sizes[1]             = 1.05;
   sizes[2]             = 0.5;
   sizes[3]             = 0.15;
   times[0]             = 0.0;
   times[1]             = 0.2;
   times[2]             = 0.55;
   times[3]             = 1.0;
};
//
datablock ParticleEmitterData(FB_Flames_E)
{
  ejectionOffset        = 0.02;
  ejectionPeriodMS      = 10;
  periodVarianceMS      = 0;
  ejectionVelocity      = 1.2;
  velocityVariance      = 0.9;
  thetaMin              = 0.0;
  thetaMax              = 0.0;
  particles             = FB_Flames_P;
};

// this offset defines the radius of ring-of-fire
datablock afxXM_LocalOffsetData(FB_FireRing_offset_XM)
{
  localOffset = "0 1.75 0";
};

datablock afxXM_GroundConformData(FB_FireRing_ground_XM)
{
  height = 0.01;
};

// this and the other spin modifiers set the starting
// angle and rotation rate of the ring-of-fire emitters.
datablock afxXM_SpinData(FB_FireRing_spin1_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 0;
  spinRate = 900;
};
//
datablock afxEffectWrapperData(FB_FireRing1_EW)
{
  effect = FB_Flames_E;
  posConstraint = caster;
  delay = 1.35; //1.5-.25
  lifetime = 1.6; //1.25
  xfmModifiers[0] = "FB_FireRing_spin1_XM";
  xfmModifiers[1] = "FB_FireRing_offset_XM";
  xfmModifiers[2] = "FB_FireRing_ground_XM";
};

datablock afxXM_SpinData(FB_FireRing_spin2_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 60;
  spinRate = 500;
};
//
datablock afxEffectWrapperData(FB_FireRing2_EW : FB_FireRing1_EW)
{
  xfmModifiers[0] = "FB_FireRing_spin2_XM";
  xfmModifiers[1] = "FB_FireRing_offset_XM";
  xfmModifiers[2] = "FB_FireRing_ground_XM";
};

datablock afxXM_SpinData(FB_FireRing_spin3_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 120;
  spinRate = -850;
};
//
datablock afxEffectWrapperData(FB_FireRing3_EW : FB_FireRing1_EW)
{
  xfmModifiers[0] = "FB_FireRing_spin3_XM";
  xfmModifiers[1] = "FB_FireRing_offset_XM";
  xfmModifiers[2] = "FB_FireRing_ground_XM";
};

datablock afxXM_SpinData(FB_FireRing_spin4_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 180;
  spinRate = 600;
};
//
datablock afxEffectWrapperData(FB_FireRing4_EW : FB_FireRing1_EW)
{
  xfmModifiers[0] = "FB_FireRing_spin4_XM";
  xfmModifiers[1] = "FB_FireRing_offset_XM";
  xfmModifiers[2] = "FB_FireRing_ground_XM";
};

datablock afxXM_SpinData(FB_FireRing_spin5_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 240;
  spinRate = -560;
};
//
datablock afxEffectWrapperData(FB_FireRing5_EW : FB_FireRing1_EW)
{
  xfmModifiers[0] = "FB_FireRing_spin5_XM";
  xfmModifiers[1] = "FB_FireRing_offset_XM";
  xfmModifiers[2] = "FB_FireRing_ground_XM";
};

datablock afxXM_SpinData(FB_FireRing_spin6_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 300;
  spinRate = -360;
};
//
datablock afxEffectWrapperData(FB_FireRing6_EW : FB_FireRing1_EW)
{
  xfmModifiers[0] = "FB_FireRing_spin6_XM";
  xfmModifiers[1] = "FB_FireRing_offset_XM";
  xfmModifiers[2] = "FB_FireRing_ground_XM";
};

datablock afxXM_SpinData(FB_FireRing_spin7_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 30;
  spinRate = -780;
};
//
datablock afxEffectWrapperData(FB_FireRing7_EW : FB_FireRing1_EW)
{
  xfmModifiers[0] = "FB_FireRing_spin7_XM";
  xfmModifiers[1] = "FB_FireRing_offset_XM";
  xfmModifiers[2] = "FB_FireRing_ground_XM";
};

datablock afxXM_SpinData(FB_FireRing_spin8_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 90;
  spinRate = 450;
};
//
datablock afxEffectWrapperData(FB_FireRing8_EW : FB_FireRing1_EW)
{
  xfmModifiers[0] = "FB_FireRing_spin8_XM";
  xfmModifiers[1] = "FB_FireRing_offset_XM";
  xfmModifiers[2] = "FB_FireRing_ground_XM";
};

datablock afxXM_SpinData(FB_FireRing_spin9_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 150;
  spinRate = -700;
};
//
datablock afxEffectWrapperData(FB_FireRing9_EW : FB_FireRing1_EW)
{
  xfmModifiers[0] = "FB_FireRing_spin9_XM";
  xfmModifiers[1] = "FB_FireRing_offset_XM";
  xfmModifiers[2] = "FB_FireRing_ground_XM";
};

datablock afxXM_SpinData(FB_FireRing_spin10_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 210;
  spinRate = -340;
};
//
datablock afxEffectWrapperData(FB_FireRing10_EW : FB_FireRing1_EW)
{
  xfmModifiers[0] = "FB_FireRing_spin10_XM";
  xfmModifiers[1] = "FB_FireRing_offset_XM";
  xfmModifiers[2] = "FB_FireRing_ground_XM";
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// RING-OF-FIRE LIGHTS

//
// Light from the ring of fire is simultated using two point lights
// positioned below the caster.  Paths are used to simulate a subtle
// flicker.
//

// this offset defines the lights distance beneath the terrain
datablock afxXM_LocalOffsetData(FB_FireRingLight_offset_XM)
{
  localOffset = "0 0 -4";
};

// flicker path 1
datablock afxPathData(FB_FireRingLight_1_Path)
{
  points = " 0    0    0"   SPC
           " 0.1 -0.3  0.8" SPC
           "-0.3  0.2 -0.6" SPC
           " 0.0 -0.6  0.4" SPC
           " -0.7 0.4 -0.8" SPC
           " 0    0    0";
  lifetime = 0.25;
  loop = cycle;
  mult = 0.06;
};
//
datablock afxXM_PathConformData(FB_FireRingLight_1_path_XM)
{
  paths = "FB_FireRingLight_1_Path";
};

// flicker path 2
datablock afxPathData(FB_FireRingLight_2_Path)
{
  points = " 0    0    0"   SPC
           " 0.4  0.7 -0.3" SPC
           "-0.3  0.0  0.4" SPC
           " 0.2  0.4 -0.8" SPC
           "-0.4 -0.8  0.5" SPC
           " 0    0    0";
  lifetime = 0.20;
  loop = cycle;
  mult = 0.05;
};
//
datablock afxXM_PathConformData(FB_FireRingLight_2_path_XM)
{
  paths = "FB_FireRingLight_2_Path";
};

if ($isTGEA)
{
  %FB_FireRingLight_LMODELS_intensity = 25.0;
  %FB_FireRingLight_LTERRAIN_intensity = 2.5;

  datablock afxXM_LocalOffsetData(FB_FireRingLight_offset2_XM)
  {
    localOffset = "0 0 -2";
  };

  // ring-of-fire lights  (illuminate dts beyond radius???)
  datablock afxLightData(FB_FireRingLight_LMODELS_CE)
  {
    type = "Point";
    radius = 5;
    sgCastsShadows = false;
    sgDoubleSidedAmbient = true;
    
    sgLightingModelName = "Inverse Square Fast Falloff";
    color = 1.0*%FB_FireRingLight_LMODELS_intensity SPC
            0.5*%FB_FireRingLight_LMODELS_intensity SPC
            0.0*%FB_FireRingLight_LMODELS_intensity;

    lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF;
  };
  //
  datablock afxLightData(FB_FireRingLight_LTERRAIN_CE : FB_FireRingLight_LMODELS_CE)
  {
    radius = 2.0;
    color = 1.0*%FB_FireRingLight_LTERRAIN_intensity SPC
            0.5*%FB_FireRingLight_LTERRAIN_intensity SPC
            0.0*%FB_FireRingLight_LTERRAIN_intensity;

    lightIlluminationMask = $AFX::ILLUM_TERRAIN | $AFX::ILLUM_ATLAS;
  };

  datablock afxMultiLightData(FB_FireRingLight_Multi_CE)
  {
    lights[0] = FB_FireRingLight_LMODELS_CE;
    lights[1] = FB_FireRingLight_LTERRAIN_CE;
  };

  datablock afxEffectWrapperData(FB_FireRingLight_1_Multi_EW)
  {
    effect = FB_FireRingLight_Multi_CE;
    posConstraint = caster;

    delay = 1.35; //1.5-.25
    fadeInTime  = 0.5;
    fadeOutTime = 0.5;
    lifetime = 1.6; //1.25
    
    xfmModifiers[0] = FB_FireRingLight_offset_XM;
    xfmModifiers[1] = FB_FireRingLight_1_path_XM;
  };

  datablock afxEffectWrapperData(FB_FireRingLight_2_Multi_EW : FB_FireRingLight_1_Multi_EW)
  {
    xfmModifiers[1] = FB_FireRingLight_2_path_XM;
  };

  $FB_FireRingLight_1 = FB_FireRingLight_1_Multi_EW;
  $FB_FireRingLight_2 = FB_FireRingLight_2_Multi_EW;
}
else
{
  %FB_FireRingLight_intensity = 2.5;

  // ring-of-fire lights  (illuminate dts beyond radius???)
  datablock afxLightData(FB_FireRingLight_CE)
  {
    type = "Point";
    radius = 5;
    sgCastsShadows = false;
    sgDoubleSidedAmbient = true;

    sgLightingModelName = "SG - Near Linear (Lighting Pack)";
    color = 1.0*%FB_FireRingLight_intensity SPC
            0.5*%FB_FireRingLight_intensity SPC
            0.0*%FB_FireRingLight_intensity;
  };
  //
  datablock afxEffectWrapperData(FB_FireRingLight_1_EW)
  {
    effect = FB_FireRingLight_CE;
    posConstraint = caster;

    delay = 1.35; //1.5-.25
    fadeInTime  = 0.5;
    fadeOutTime = 0.5;
    lifetime = 1.6; //1.25
    xfmModifiers[0] = FB_FireRingLight_offset_XM;
    xfmModifiers[1] = FB_FireRingLight_1_path_XM;
  };
  datablock afxEffectWrapperData(FB_FireRingLight_2_EW : FB_FireRingLight_1_EW)
  {
    xfmModifiers[1] = FB_FireRingLight_2_path_XM;
  };

  $FB_FireRingLight_1 = FB_FireRingLight_1_EW;
  $FB_FireRingLight_2 = FB_FireRingLight_2_EW;
}

datablock afxXM_LocalOffsetData(FB_FireRingSpotLight_offset_XM)
{
  localOffset = "0 2 -4";
};
datablock afxXM_SpinData(FB_FireRingSpotLight_spin1_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 0;
  spinRate = -30;
};
datablock afxXM_SpinData(FB_FireRingSpotLight_spin2_XM : FB_FireRingSpotLight_spin1_XM)
{
  spinAngle = 120;
};
datablock afxXM_SpinData(FB_FireRingSpotLight_spin3_XM : FB_FireRingSpotLight_spin1_XM)
{
  spinAngle = 240;
};

%FB_FireRingSpotLight_intensity = 2.5;
datablock afxLightData(FB_FireRingSpotLight_CE) // AFX-SPOTLIGHT
{
  type = "Spot";
  radius = ($isTGEA) ? 10 : 5;
  direction = "0 -0.313 0.95";
  sgSpotAngle = 100; // ignored for TGEA dynamic spots, need to check for TGE
  sgCastsShadows = false;

  sgLocalAmbientAmount = 0.2;
  sgDoubleSidedAmbient = true;

  color = 1.0*%FB_FireRingSpotLight_intensity*2 SPC
          0.5*%FB_FireRingSpotLight_intensity*2 SPC
          0.0*%FB_FireRingSpotLight_intensity*2;
  ambientColor = 1.0*2*%FB_FireRingSpotLight_intensity SPC
                 0.5*2*%FB_FireRingSpotLight_intensity SPC
                 0.0*2*%FB_FireRingSpotLight_intensity;
 
  lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF;  // TGEA (ignored by TGE)
  sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Near Linear";
};
datablock afxEffectWrapperData(FB_FireRingSpotLight_1_EW)
{
  effect = FB_FireRingSpotLight_CE;
  posConstraint = caster;
  delay = 1.35;
  fadeInTime  = 0.5;
  fadeOutTime = 0.5;
  lifetime = 1.6;
  xfmModifiers[0] = FB_FireRingSpotLight_spin1_XM;
  xfmModifiers[1] = FB_FireRingSpotLight_offset_XM;
};
datablock afxEffectWrapperData(FB_FireRingSpotLight_2_EW : FB_FireRingSpotLight_1_EW)
{
  xfmModifiers[0] = FB_FireRingSpotLight_spin2_XM;
  xfmModifiers[1] = FB_FireRingSpotLight_offset_XM;
};
datablock afxEffectWrapperData(FB_FireRingSpotLight_3_EW : FB_FireRingSpotLight_1_EW)
{
  xfmModifiers[0] = FB_FireRingSpotLight_spin3_XM;
  xfmModifiers[1] = FB_FireRingSpotLight_offset_XM;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SPELLCASTER ANIMATION
//

// caster performs a summoning animation 
datablock afxAnimClipData(FB_SummonFire_Clip_CE)
{
  clipName = "summon";
  rate = 0.45;
};
datablock afxEffectWrapperData(FB_SummonFire_Clip_EW)
{
  effect = FB_SummonFire_Clip_CE;
  constraint = "caster";
  lifetime = 1.0;
  delay = 0.5;
};

// caster performs a casting animation 
datablock afxAnimClipData(FB_Casting1_Clip_CE)
{
  clipName = "fb";
  rate = 1.8;
};
datablock afxEffectWrapperData(FB_Casting1_Clip_EW)
{
  effect = FB_Casting1_Clip_CE;
  constraint = "caster";
  delay = 1.8;
};

// caster performs a throwing animation 
datablock afxAnimClipData(FB_Casting2_Clip_CE)
{
  clipName = "throw";
  rate = 0.7;
};
datablock afxEffectWrapperData(FB_Casting2_Clip_EW)
{
  effect = FB_Casting2_Clip_CE;
  constraint = "caster";
  delay = 2.8;
  lifetime = 2.0; 
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// FIREBALL

//
// here the fire particles are used to make a fireball
// appear in the caster's right hand.
//
datablock afxEffectWrapperData(FB_FireballEmerge_EW)
{
  effect = FB_Flames_E;
  posConstraint = "caster.Bip01 R Hand";  
  delay = 2.0;
  fadeInTime  = 0.1;
  fadeOutTime = 0.3;
  lifetime = 0.7;
};

// this light coordinates with FB_FireballEmerge_EW to create
// a fireball in the caster's right hand.
//
datablock sgLightObjectData(FB_FireballLight1_CE) 
{
  CastsShadows = false;
  Radius = ($isTGEA) ? 3.5 : 3.0;
  Brightness = 0.75*0.5;
  Colour = "1.0 0.6 0.0";

  FlareOn = true;
  LinkFlare = true;
  FlareBitmap = %mySpellDataPath @ "/FB/lights/FB_firePortalFlare";
  NearSize = 1*2.0;
  FarSize  = 0.5*2.0;
  NearDistance = 2;
  FarDistance  = 50;

  AnimBrightness = true;
  LerpBrightness = true;
  MinBrightness = 0.3;
  MaxBrightness = 0.9;
  BrightnessKeys = "AZA";
  BrightnessTime = 0.1;

  AnimRotation = true;
  LerpRotation = true;
  MinRotation = 0;
  MaxRotation = 359;
  RotationKeys = "AZA";
  RotationTime = 1.5*0.2;
};
datablock sgLightObjectData(FB_FireballLight2_CE : FB_FireballLight1_CE) 
{
  BrightnessTime = 0.15;
  RotationTime   = 2.0*0.2;
  
  LightingModelName = ($isTGEA) ? "SG - Inverse Square (Lighting Pack)" : "SG - Original Advanced (Lighting Pack)";
  Radius = ($isTGEA) ? 1.5 : 3.0;  
};

datablock afxEffectWrapperData(FB_FireballRevealLight1_EW)
{
  effect = FB_FireballLight1_CE;
  posConstraint = "caster.Bip01 R Hand";  
  delay = 2.7-0.5;
  fadeInTime  = 0.2;
  fadeOutTime = 0.1;
  lifetime = 0.9+0.5;
};
datablock afxEffectWrapperData(FB_FireballRevealLight2_EW : FB_FireballRevealLight1_EW)
{
  effect = FB_FireballLight2_CE;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// IMPACT EXPLOSION
//

datablock ParticleData(FB_ExplosionSmoke_P)
{
   textureName          = %mySpellDataPath @ "/FB/particles/smoke";
   dragCoeffiecient     = 100.0;
   gravityCoefficient   = 0;
   inheritedVelFactor   = 0.25;
   constantAcceleration = -0.30;
   lifetimeMS           = 1200;
   lifetimeVarianceMS   = 300;
   useInvAlpha =  true;
   spinRandomMin = -80.0;
   spinRandomMax =  80.0;

   colors[0]     = "1.0 1.0 1.0 1.0";
   colors[1]     = "1.0 0.5 0.0 1.0";
   colors[2]     = "0.4 0.4 0.4 1.0";
   colors[3]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 4.0;
   sizes[1]      = 6.0;
   sizes[2]      = 8.0;
   sizes[3]      = 10.0;

   times[0]      = 0.0;
   times[1]      = 0.33;
   times[2]      = 0.66;
   times[3]      = 1.0;
};
//
datablock ParticleData(FB_ExplosionFire_P)
{
   textureName          = %mySpellDataPath @ "/FB/particles/fireExplosion";
   dragCoeffiecient     = 100.0;
   gravityCoefficient   = 0;
   inheritedVelFactor   = 0.25;
   constantAcceleration = 0.1;
   lifetimeMS           = 600; //1200;
   lifetimeVarianceMS   = 150; //300;
   useInvAlpha =  false;
   spinRandomMin        = -900.0;
   spinRandomMax        = 900.0;
   colors[0]            = "1.0 1.0 1.0 1.0";
   colors[1]            = "1.0 1.0 0.0 1.0";
   colors[2]            = "1.0 0.0 0.0 1.0";
   colors[3]            = "1.0 0.0 0.0 0.0";
   sizes[0]             = 3.0;
   sizes[1]             = 5.0;
   sizes[2]             = 7.0;//2.2;
   sizes[3]             = 3.0; //1.2;
   times[0]             = 0.0;
   times[1]             = 0.2;
   times[2]             = 0.7;
   times[3]             = 1.0;
};
//
datablock ParticleEmitterData(FB_ExplosionFire_E)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;
   ejectionVelocity = 0.8;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles        = "FB_ExplosionFire_P";
};
//
datablock ParticleEmitterData(FB_ExplosionSmoke_E)
{
   ejectionPeriodMS = 5;
   periodVarianceMS = 0;
   ejectionVelocity = 10;
   velocityVariance = 1.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles        = "FB_ExplosionSmoke_P";
};
//
datablock ExplosionData(FB_Explosion_CE)
{
   lifeTimeMS = 1200;

   // Volume particles
   particleEmitter = FB_ExplosionFire_E;
   particleDensity = 20; //50;
   particleRadius = 3;

   // Point emission
   emitter[0] = FB_ExplosionSmoke_E;
   emitter[1] = FB_ExplosionSmoke_E;

   // Impulse
   impulseRadius = 10;
   impulseForce = 15;
};
//
datablock afxEffectWrapperData(FB_Explosion1_EW)
{
  effect = FB_Explosion_CE;
  constraint = "impactPoint";
  execConditions[0] = $AFX::IMPACTED_SOMETHING;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// IMPACT SHOCKWAVE
//

datablock afxZodiacData(FB_ImpactZodeFast_CE)
{  
  texture = %mySpellDataPath @ "/FB/zodiacs/zode_impactA";
  radius = 1.0;
  startAngle = 0.0;
  rotationRate = 0.0;  
  color = "1.0 1.0 1.0 0.10";
  blend = additive;
  growthRate = 110.0;
  showOnInteriors = false;
};

// A fast growing zodiac used as an impact shockwave.
datablock afxEffectWrapperData(FB_ImpactZodeFast_EW)
{
  effect = FB_ImpactZodeFast_CE;
  posConstraint = "impactPoint";
  delay = 0.05;
  fadeInTime = 0.0;
  fadeOutTime = 0.5;
  lifetime = 0.5;
  xfmModifiers[0] = SHARED_freeze_XM;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SOUNDS

// casting sounds //

datablock AudioProfile(FB_ZodeSnd_CE)
{
   fileName = %mySpellDataPath @ "/FB/sounds/fb_zodiac.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(FB_ZodeSnd_EW)
{
  effect = FB_ZodeSnd_CE;
  constraint = "caster";
  delay = 0;
  lifetime = 1.907;
};

datablock AudioProfile(FB_ConjureSnd_CE)
{
   fileName = %mySpellDataPath @ "/FB/sounds/fb_conjure.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(FB_ConjureSnd_EW)
{
  effect = FB_ConjureSnd_CE;
  constraint = "caster";
  delay = 0.6;
  lifetime = 1.907;
};

datablock AudioProfile(FB_FireRingSnd_CE)
{
   fileName = %mySpellDataPath @ "/FB/sounds/fb_firering.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(FB_FireRingSnd_EW)
{
  effect = FB_FireRingSnd_CE;
  constraint = "caster";
  delay = 1.33;
  lifetime = 2.296;
};

// impact sounds //

datablock AudioProfile(FB_Impact1ASnd_CE)
{
   fileName = %mySpellDataPath @ "/FB/sounds/fb_impact1a.ogg";
   description = SpellAudioImpact_AD;
   preload = false;
};
datablock afxEffectWrapperData(FB_Impact1ASnd_EW)
{
  effect = FB_Impact1ASnd_CE;
  constraint = "impactedObject";
  delay = 0;
  lifetime = 2.417;
};

datablock AudioProfile(FB_Impact1BSnd_CE)
{
   fileName = %mySpellDataPath @ "/FB/sounds/fb_impact1b.ogg";
   description = SpellAudioImpact_AD;
   preload = false;
};
datablock afxEffectWrapperData(FB_Impact1BSnd_EW)
{
  effect = FB_Impact1BSnd_CE;
  posConstraint = "impactPoint";
  delay = 0;
  lifetime = 2.191;
  execConditions[0] = $AFX::IMPACTED_SOMETHING;
};

// fireball sounds //

datablock AudioProfile(FB_FireBallSnd_CE)
{
   fileName = %mySpellDataPath @ "/FB/sounds/projectile_loopFire1a_SR.ogg";
   description = SpellAudioMissileLoop_AD;
   preload = false;
};
datablock afxEffectWrapperData(FB_FireBallSnd_EW)
{
  effect = FB_FireBallSnd_CE;
  posConstraint = "caster.Bip01 R Hand";  
  delay = 1.8;
  lifetime = 1.5;
  fadeInTime = 0.5;
  fadeOutTime = 0.5;
};

datablock AudioProfile(FB_FireCrackle_CE)
{
   fileName = %mySpellDataPath @ "/FB/sounds/fireloop.ogg";
   description = SpellAudioLoop_AD;
   preload = false;
};

datablock afxEffectWrapperData(FB_FireCrackle1_EW)
{
  effect = FB_FireCrackle_CE;
  constraint = "caster";
  delay = 1.33;
  lifetime = 1.35;
  fadeInTime = 0.6;
  fadeOutTime = 1.25;
  scaleFactor = 0.4;
  xfmModifiers[0] = "FB_FireRing_spin6_XM";
  xfmModifiers[1] = "FB_FireRing_offset_XM";
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// DAMAGE

datablock afxDamageData(FB_dot_damage_CE)
{
  label = "fb_dot";
  flavor = "fire";
  directDamage = 3;
  directDamageRepeats = 6;
};

datablock afxEffectWrapperData(FB_dot_damage_EW)
{
  effect = FB_dot_damage_CE;
  posConstraint = "impactPoint";
  posConstraint2 = "impactedObject"; 
  lifetime = 6;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// FIREBALL MISSILE

datablock afxMagicMissileData(FB_Fireball)
{
  particleEmitter = FB_Flames_E;
  muzzleVelocity = 30;
  velInheritFactor = 0;
  lifetime = 20000;
  isBallistic = true;
  ballisticCoefficient = 0.95;
  gravityMod = 0.05;
  isGuided  = true;
  precision = 50;
  trackDelay  = 7;
  sound = FB_FireBallSnd_CE;

  // NOTE - Replace launchNode with best node given the character's skeleton
  // and casting animation.
  launchNode = "Bip01 R Hand";

  // NOTE - The character is not animating on the server, so a fixed launch point
  // approximating the correct location is set. (launchOffsetServer overrides
  // launchNode on the server.) By setting echoLaunchOffset to true, you can see
  // in the console what offset is used on the client and use that for the
  // launchOffsetServer value.
  launchOffsetServer = "0.49 1.20 1.88";
  echoLaunchOffset = false;
};

datablock afxEffectWrapperData(FB_FireballFlare1_EW)
{
  effect = FB_FireballLight1_CE;
  constraint = missile;
};
datablock afxEffectWrapperData(FB_FireballFlare2_EW : FB_FireballFlare1_EW)
{
  effect = FB_FireballLight2_CE;
};

%FB_ImpactLight1_intensity = 5.0;
datablock afxLightData(FireFX_ImpactLight1_CE)
{
  type = "Point";
  radius = 9;
  sgCastsShadows = false;
  sgDoubleSidedAmbient = true;
  color = 1.0*%FB_ImpactLight1_intensity SPC
          1.0*%FB_ImpactLight1_intensity SPC
          1.0*%FB_ImpactLight1_intensity;
};
datablock sgLightObjectData(FireFX_ImpactLight1_flare_CE) 
{
  CastsShadows = false;
  Radius = 10;
  Brightness = 2*%FB_ImpactLight1_intensity;
  Colour = "1.0 1.0 1.0";

  FlareOn = true;
  LinkFlare = true;
  FlareBitmap = "common/lighting/lightFalloffMono"; //corona"; //lightFalloffMono";
  NearSize = 20;
  FarSize  = 8;
  NearDistance = 2;
  FarDistance  = 50;
};
// flare line-of-sight...
datablock afxXM_LocalOffsetData(FB_ImpactFlare_offset_XM)
{
  localOffset = "0 0 1.5";
};
//
datablock afxEffectWrapperData(FB_ImpactLight1_EW)
{
  effect = FireFX_ImpactLight1_flare_CE; //FireFX_ImpactLight1_CE;
  posConstraint = "impactPoint";  
  delay    = 0.0;
  lifetime = 0.25;
  fadeInTime  = 0.25;
  fadeOutTime = 0.75;
  execConditions[0] = $AFX::IMPACTED_SOMETHING;
  xfmModifiers[0] = FB_ImpactFlare_offset_XM;
};

%FB_ImpactLight2_intensity = 2.5;
datablock afxLightData(FireFX_ImpactLight2_CE)
{
  type = "Point";
  radius = 18;
  sgCastsShadows = true;
  sgDoubleSidedAmbient = true;
  color = 1.0*%FB_ImpactLight2_intensity SPC
          0.2*%FB_ImpactLight2_intensity SPC
          0.0*%FB_ImpactLight2_intensity; 
};
datablock sgLightObjectData(FireFX_ImpactLight2_flare_CE) 
{
  CastsShadows = false;
  Radius = 18;
  Brightness = 2*%FB_ImpactLight2_intensity;
  Colour = "1.0 0.2 0.0";

  FlareOn = true;
  LinkFlare = true;
  FlareBitmap = "common/lighting/corona"; //corona"; //lightFalloffMono";
  NearSize = 20*1.5;
  FarSize  = 8*1.5;
  NearDistance = 2;
  FarDistance  = 50;
};
//
datablock afxEffectWrapperData(FB_ImpactLight2_EW : FB_ImpactLight1_EW)
{
  effect = FireFX_ImpactLight2_flare_CE; //FireFX_ImpactLight2_CE;
  delay = 0.05;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// FLAME BROIL SPELL
//

datablock afxMagicSpellData(FlameBroilSpell)
{
  castingDur = 3.0;
  lingerDur = 6.0;

    // magic missile //
  missile = FB_Fireball;

    // casting zodiac //
  addCastingEffect = FB_ZodeReveal_EW;
  addCastingEffect = FB_MainZodeRevealLight_1_EW;
  addCastingEffect = FB_MainZodeRevealLight_2_EW;
  addCastingEffect = FB_MainZodeRevealLight_3_EW;
  addCastingEffect = FB_Zode1_TLKunderglow_EW;
  addCastingEffect = FB_Zode1_EW;
  addCastingEffect = FB_Zode2_TLKunderglow_EW;
  addCastingEffect = FB_Zode2_EW;
  addCastingEffect = FB_Zode3_EW;
  addCastingEffect = FB_ZodeGlowRing_EW;
  addCastingEffect = FB_ZodeGlowRingLight_1_EW;
  addCastingEffect = FB_ZodeGlowRingLight_2_EW;
  addCastingEffect = FB_ZodeGlowRingLight_3_EW;
    // ring-of-fire //
  addCastingEffect = FB_FireRing1_EW;
  addCastingEffect = FB_FireRing2_EW;
  addCastingEffect = FB_FireRing3_EW;
  addCastingEffect = FB_FireRing4_EW;
  addCastingEffect = FB_FireRing5_EW;
  addCastingEffect = FB_FireRing6_EW;
  addCastingEffect = FB_FireRing7_EW;
  addCastingEffect = FB_FireRing8_EW;
  addCastingEffect = FB_FireRing9_EW;
  addCastingEffect = FB_FireRing10_EW;
   // ring-of-fire lights //
  addCastingEffect = $FB_FireRingLight_1;
  addCastingEffect = $FB_FireRingLight_2;
  addCastingEffect = FB_FireRingSpotLight_1_EW;
  addCastingEffect = FB_FireRingSpotLight_2_EW;
  addCastingEffect = FB_FireRingSpotLight_3_EW;
   // spellcaster animation //
  addCastingEffect = FB_SummonFire_Clip_EW;
  addCastingEffect = FB_Casting1_Clip_EW;
  addCastingEffect = FB_Casting2_Clip_EW;
    // fireball //
  addCastingEffect = FB_FireballRevealLight1_EW;
  addCastingEffect = FB_FireballRevealLight2_EW;
  addCastingEffect = FB_FireballEmerge_EW;
    // sounds //
  addCastingEffect = FB_ZodeSnd_EW;
  addCastingEffect = FB_ConjureSnd_EW;
  addCastingEffect = FB_FireRingSnd_EW;
  addCastingEffect = FB_FireBallSnd_EW;
  addCastingEffect = FB_FireCrackle1_EW;

  addDeliveryEffect = FB_FireballFlare1_EW;
  addDeliveryEffect = FB_FireballFlare2_EW;

    // impact effects //
  addImpactEffect = FB_Explosion1_EW;
  addImpactEffect = FB_ImpactLight1_EW;
  addImpactEffect = FB_ImpactLight2_EW;
  addImpactEffect = FB_Impact1BSnd_EW;
  addImpactEffect = FB_ImpactZodeFast_EW;
  addImpactEffect = FB_dot_damage_EW;
};
//
datablock afxRPGMagicSpellData(FlameBroilSpell_RPG)
{
  name = "Flame Broil";
  desc = "Conjure up a broiling ring of fire and fling a fiery charcoal briquette " @
         "to inflict " @ 35 @ " damage plus an additional " @ 18 @ 
         " over " @ 6 @ " seconds." @
         "\n" @
         "\nspell design: Matthew Durante" @
         "\nsound effects: Dave Schroeder" @ 
         "\nspell concept: Jeff Faust";
  sourcePack = "Spell Pack 1";
  iconBitmap = %mySpellDataPath @ "/FB/icons/fb";

  target = "enemy";
  range = 75;
  manaCost = 10;

  directDamage = 35.0;

  castingDur = FlameBroilSpell.castingDur;
};

// set a level of detail
function FlameBroilSpell::onActivate(%this, %spell, %caster, %target)
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
  FlameBroilSpell.scriptFile = $afxAutoloadScriptFile;
  FlameBroilSpell.spellDataPath = %mySpellDataPath;
  if (isFunction(addDemoSpellbookSpell))
    addDemoSpellbookSpell(FlameBroilSpell, FlameBroilSpell_RPG);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
