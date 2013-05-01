
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// SPIRIT OF ROACH SPELL
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
$spell_reload = isObject(SpiritOfRoachSpell);
if ($spell_reload)
{
  // mark datablocks so we can detect which are reloaded this script
  markDataBlocks();
  // reset data path from previously saved value
  %mySpellDataPath = SpiritOfRoachSpell.spellDataPath;
  SpiritOfRoachSpell.reset();
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

datablock afxZodiacData(SoR_ZodeReveal_CE)
{  
  texture = %mySpellDataPath @ "/SoR/zodiacs/SoR_caster_reveal";
  radius = 3.0;
  startAngle = 7.5; //0.0+7.5
  rotationRate = -30.0;  
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorIgnoreVertical = true;
};
//
datablock afxEffectWrapperData(SoR_ZodeReveal_EW)
{
  effect = SoR_ZodeReveal_CE;
  posConstraint = caster;
  delay = 0.0; //0.01;
  lifetime = 0.75;
  fadeInTime = 0.5;
  fadeOutTime = 0.25;
  xfmModifiers[0] = SHARED_freeze_XM;
};

%SoR_MainZodeRevealLight_intensity = 2.5;

datablock afxXM_LocalOffsetData(SoR_MainZodeRevealLight_offset_XM)
{
  localOffset = "0 2 -4";
};
datablock afxXM_SpinData(SoR_MainZodeRevealLight_spin1_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 0;
  spinRate = -30;
};
datablock afxXM_SpinData(SoR_MainZodeRevealLight_spin2_XM : SoR_MainZodeRevealLight_spin1_XM)
{
  spinAngle = 120;
};
datablock afxXM_SpinData(SoR_MainZodeRevealLight_spin3_XM : SoR_MainZodeRevealLight_spin1_XM)
{
  spinAngle = 240;
};

// main zode reveal light
datablock afxLightData(SoR_MainZodeRevealLight_CE) // AFX-SPOTLIGHT
{
  type = "Spot";
  radius = ($isTGEA) ? 10 : 5;
  direction = "0 -0.313 0.95";
  sgCastsShadows = false;

  color = 1.0*%SoR_MainZodeRevealLight_intensity SPC
          1.0*%SoR_MainZodeRevealLight_intensity SPC
          1.0*%SoR_MainZodeRevealLight_intensity;
  lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF; // TGEA (ignored by TGE)
  sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Near Linear";
};

datablock afxEffectWrapperData(SoR_MainZodeRevealLight_1_EW : SoR_ZodeReveal_EW)
{
  effect = SoR_MainZodeRevealLight_CE;
  xfmModifiers[1] = SoR_MainZodeRevealLight_spin1_XM;
  xfmModifiers[2] = SoR_MainZodeRevealLight_offset_XM;
};
datablock afxEffectWrapperData(SoR_MainZodeRevealLight_2_EW : SoR_ZodeReveal_EW)
{
  effect = SoR_MainZodeRevealLight_CE;
  xfmModifiers[1] = SoR_MainZodeRevealLight_spin2_XM;
  xfmModifiers[2] = SoR_MainZodeRevealLight_offset_XM;
};
datablock afxEffectWrapperData(SoR_MainZodeRevealLight_3_EW : SoR_ZodeReveal_EW)
{
  effect = SoR_MainZodeRevealLight_CE;
  xfmModifiers[1] = SoR_MainZodeRevealLight_spin3_XM;
  xfmModifiers[2] = SoR_MainZodeRevealLight_offset_XM;
};

// purple and blue primary zodiac
datablock afxZodiacData(SoR_Zode1_CE)
{  
  texture = %mySpellDataPath @ "/SoR/zodiacs/SoR_caster";
  radius = 3.0;
  startAngle = 0.0;
  rotationRate = -30.0;
  color = "1.0 1.0 1.0 0.9";
  blend = additive;
  interiorIgnoreVertical = true;
};
//
datablock afxEffectWrapperData(SoR_Zode1_EW)
{
  effect = SoR_Zode1_CE;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 2.5;
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
datablock afxZodiacData(SoR_Zode1_TLKunderglow_CE : SoR_Zode1_CE)
{
  color = "0.75 0.75 0.75 0.75";
  blend = normal;
};
//
datablock afxEffectWrapperData(SoR_Zode1_TLKunderglow_EW : SoR_Zode1_EW)
{
  effect = SoR_Zode1_TLKunderglow_CE;
  execConditions = $BrightLighting_mask;
};

// purple runes (inner ring) and skulls (outer ring)
datablock afxZodiacData(SoR_Zode2_CE)
{  
  texture = %mySpellDataPath @ "/SoR/zodiacs/zode_text";
  radius = 3.0;
  startAngle = 0.0;
  rotationRate = 20.0; //60
  color = "0.861 0.0 0.9 1.0";
  blend = additive;
  interiorIgnoreVertical = true;
};
//
datablock afxEffectWrapperData(SoR_Zode2_EW)
{
  effect = SoR_Zode2_CE;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 2.5;
  xfmModifiers[0] = SHARED_freeze_XM;
};

// sketchy white glyph symbols
datablock afxZodiacData(SoR_Zode3_CE)
{  
  texture = %mySpellDataPath @ "/SoR/zodiacs/zode_symbols";
  radius = 3.0;
  startAngle = 0.0;
  rotationRate = -30.0;
  color = "1.0 1.0 1.0 0.5";
  blend = additive;
  interiorIgnoreVertical = true;
};
//
datablock afxEffectWrapperData(SoR_Zode3_EW)
{
  effect = SoR_Zode3_CE;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 2.5;
  xfmModifiers[0] = SHARED_freeze_XM;
};

if ($isTGEA)
{
  %SoR_CastingZodeLight_LMODELS_intensity = 25.0;
  %SoR_CastingZodeLight_LTERRAIN_intensity = 2.5;

  // this offset defines the lights distance beneath the terrain  
  datablock afxXM_LocalOffsetData(SoR_CastingZodeLight_offset_XM)
  {
    localOffset = "0 0 -2";
  };

  datablock afxLightData(SoR_CastingZodeLight_LMODELS_CE)
  {
    type = "Point";
    radius = 3;
    sgCastsShadows = false;
    sgDoubleSidedAmbient = true;
    
    sgLightingModelName = "Inverse Square Fast Falloff";
    color = 0.067*%SoR_CastingZodeLight_LMODELS_intensity SPC
            0.698*%SoR_CastingZodeLight_LMODELS_intensity SPC
            0.773*%SoR_CastingZodeLight_LMODELS_intensity;

    lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF;
  };
  datablock afxLightData(SoR_CastingZodeLight_LTERRAIN_CE : SoR_CastingZodeLight_LMODELS_CE)
  {
    radius = 2.25;
    sgLightingModelName = "Inverse Square Fast Falloff";
    color = 0.067*%SoR_CastingZodeLight_LTERRAIN_intensity SPC
            0.698*%SoR_CastingZodeLight_LTERRAIN_intensity SPC
            0.773*%SoR_CastingZodeLight_LTERRAIN_intensity;

    lightIlluminationMask = $AFX::ILLUM_TERRAIN | $AFX::ILLUM_ATLAS;
  };

  datablock afxMultiLightData(SoR_CastingZodeLight_Multi_CE)
  {
    lights[0] = SoR_CastingZodeLight_LMODELS_CE;
    lights[1] = SoR_CastingZodeLight_LTERRAIN_CE;
  };
  datablock afxEffectWrapperData(SoR_CastingZodeLight_Multi_EW : SoR_Zode1_EW)
  {
    effect = SoR_CastingZodeLight_Multi_CE;
    xfmModifiers[0] = SoR_CastingZodeLight_offset_XM;
  };

  $SoR_CastingZodeLight = SoR_CastingZodeLight_Multi_EW;
}
else
{
  %SoR_CastingZodeLight_intensity = 2.5;

  // this offset defines the lights distance beneath the terrain
  datablock afxXM_LocalOffsetData(SoR_CastingZodeLight_offset_XM)
  {
    localOffset = "0 0 -4";
  };

  datablock afxLightData(SoR_CastingZodeLight_CE)
  {
    type = "Point";
    radius = 5;
    sgCastsShadows = false;
    sgDoubleSidedAmbient = true;

    sgLightingModelName = "SG - Near Linear (Lighting Pack)";
    color = 0.067*%SoR_CastingZodeLight_intensity SPC
            0.698*%SoR_CastingZodeLight_intensity SPC
            0.773*%SoR_CastingZodeLight_intensity;
  };
  //
  datablock afxEffectWrapperData(SoR_CastingZodeLight_EW : SoR_Zode1_EW)
  {
    effect = SoR_CastingZodeLight_CE;
    xfmModifiers[0] = "SoR_CastingZodeLight_offset_XM";
  };

  $SoR_CastingZodeLight = SoR_CastingZodeLight_EW;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SPELLCASTER ANIMATION

datablock afxAnimClipData(SoR_Summon_Clip_CE)
{
  clipName = "summon";
  rate = 0.35;
};

datablock afxEffectWrapperData(SoR_Summon_Clip_EW)
{
  effect = SoR_Summon_Clip_CE;
  constraint = "caster";
  lifetime = 1.75;
  delay = 1.0;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// FALLOUT RING

datablock afxZodiacData(SoR_FalloutZode1_CE)
{  
  texture = %mySpellDataPath @ "/SoR/zodiacs/SoR_castingA";
  radius = 1.0;
  startAngle = 0.0;
  rotationRate = 80.0; //40.0;
  color = "1.0 1.0 1.0 0.3";
  blend = additive;
  growthRate = 20.0; //30.0;
  showOnInteriors = false;
};
//
datablock afxEffectWrapperData(SoR_FalloutZode1_EW)
{
  effect = SoR_FalloutZode1_CE;
  posConstraint = "caster";
  delay = 1.25;
  fadeInTime = 0.25;
  fadeOutTime = 2.75;
  lifetime = 1.25;
  xfmModifiers[0] = SHARED_freeze_XM;
};

datablock afxZodiacData(SoR_FalloutZode2_CE : SoR_FalloutZode1_CE)
{  
  texture = %mySpellDataPath @ "/SoR/zodiacs/SoR_castingB";
  rotationRate = -80.0; //-40.0;
  showOnInteriors = false;
};
//
datablock afxEffectWrapperData(SoR_FalloutZode2_EW : SoR_FalloutZode1_EW)
{
  effect = SoR_FalloutZode2_CE;
};

if ($isTGEA)
{
  %SoR_FalloutLight_LMODELS_intensity = 20.0;
  %SoR_FalloutLight_LTERRAIN_intensity = 1.0;

  // this offset defines the lights distance beneath the terrain
  datablock afxXM_LocalOffsetData(SoR_FalloutLight_offset_XM)
  {
    localOffset = "0 0 -2";
  };

  datablock afxLightData(SoR_FalloutLight_LMODELS_CE)
  {
    type = "Point";
    radius = 5;
    sgCastsShadows = false;
    sgDoubleSidedAmbient = true;  // necessary for interiors
    
    sgLightingModelName = "Inverse Square Fast Falloff";
    color = 1.0*%SoR_FalloutLight_LMODELS_intensity SPC
            1.0*%SoR_FalloutLight_LMODELS_intensity SPC
            1.0*%SoR_FalloutLight_LMODELS_intensity;

    lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF;
  };
  datablock afxLightData(SoR_FalloutLight_LTERRAIN_CE : SoR_FalloutLight_LMODELS_CE)
  {
    color = 1.0*%SoR_FalloutLight_LTERRAIN_intensity SPC
            1.0*%SoR_FalloutLight_LTERRAIN_intensity SPC
            1.0*%SoR_FalloutLight_LTERRAIN_intensity;

    lightIlluminationMask = $AFX::ILLUM_TERRAIN | $AFX::ILLUM_ATLAS;
  };

  datablock afxMultiLightData(SoR_FalloutLight_Multi_CE)
  {
    lights[0] = SoR_FalloutLight_LMODELS_CE;
    lights[1] = SoR_FalloutLight_LTERRAIN_CE;
  };
  datablock afxEffectWrapperData(SoR_FalloutLight_Multi_EW)
  {
    effect = SoR_FalloutLight_Multi_CE;
    posConstraint = caster;

    delay = 1.25;
    fadeInTime = 0.50;
    fadeOutTime = 0.5; //0.25;
    lifetime = 0.50;

    xfmModifiers[0] = SoR_FalloutLight_offset_XM;  
  };

  $SoR_FalloutLight = SoR_FalloutLight_Multi_EW;
}
else
{
  %SoR_FalloutLight_intensity = 1.0;

  // this offset defines the lights distance beneath the terrain
  datablock afxXM_LocalOffsetData(SoR_FalloutLight_offset_XM)
  {
    localOffset = "0 0 -2";
  };

  datablock afxLightData(SoR_FalloutLight_CE)
  {
    type = "Point";
    radius = 5;
    sgCastsShadows = false;
    sgDoubleSidedAmbient = true;  // necessary for interiors

    sgLightingModelName = "SG - Near Linear (Lighting Pack)";
    color = 1.0*%SoR_FalloutLight_intensity SPC
            1.0*%SoR_FalloutLight_intensity SPC
            1.0*%SoR_FalloutLight_intensity;
  };
  //
  datablock afxEffectWrapperData(SoR_FalloutLight_EW)
  {
    effect = SoR_FalloutLight_CE;
    posConstraint = caster;

    delay = 1.25;
    fadeInTime = 0.50;
    fadeOutTime = 0.5; //0.25;
    lifetime = 0.50;

    xfmModifiers[0] = "SoR_FalloutLight_offset_XM";   
  };

  $SoR_FalloutLight = SoR_FalloutLight_EW;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// ROACH HALO

//
// A neon-like roach icon hovers above the target's head for a
// a shorttime. Note the use of a camera aim constraint to force
// the 2D icon to always face the camera.
//

datablock afxModelData(SoR_Halo_CE)
{
   shapeFile = %mySpellDataPath @ "/SoR/models/SoR_plane.dts";
   forceOnMaterialFlags = $MaterialFlags::Additive | $MaterialFlags::SelfIlluminating;
};

datablock afxXM_WorldOffsetData(SoR_Halo_offset_XM)
{
  worldOffset = "0.0 0.0 3.4";
};
datablock afxXM_AimData(SoR_Halo_aim_XM)
{
  aimZOnly = false;
};
datablock afxEffectWrapperData(SoR_Halo_EW)
{
  effect        = SoR_Halo_CE;

  posConstraint = "impactedObject";
  posConstraint2 = camera;
  delay = 0.5;
  fadeInTime  = 0.5;
  fadeOutTime = 0.5;
  lifetime = 2.5;
  scaleFactor = 1.4;
  xfmModifiers[0] = SoR_Halo_offset_XM;
  xfmModifiers[1] = SoR_Halo_aim_XM;
};

// this represents light cast from the halo
datablock afxLightData(SoR_RoachLight_CE)
{
  type = "Point";  
  radius = ($isTGEA) ? 5 : 2;
  color  = "0.48 0.085 0.5";
  sgLightingModelName = ($isTGEA) ? "Inverse Square Fast Falloff" : "Near Linear";
};

datablock afxXM_WorldOffsetData(SoR_RoachLight_offset_XM)
{
  worldOffset = "0.0 0.0 3.0";
};
datablock afxEffectWrapperData(SoR_RoachLight_EW)
{
  effect = SoR_RoachLight_CE;
  posConstraint = "impactedObject";  
  delay = 0.5;
  fadeInTime  = 0.5;
  fadeOutTime = 0.5;
  lifetime = 2.5;
  xfmModifiers[0] = SoR_RoachLight_offset_XM;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SHIELD BUBBLE

//
// A bubble of protection with insect wing details surrounds
// the target for a short time.
//

datablock afxModelData(SoR_Shield_CE)
{
  shapeFile = %mySpellDataPath @ "/SoR/models/SoR_sphere.dts";
  sequence = "bubble";
  alphaMult = 0.7;
  forceOnMaterialFlags = $MaterialFlags::Additive | $MaterialFlags::SelfIlluminating;
};

datablock afxXM_WorldOffsetData(SoR_Shield_offset_XM)
{
  worldOffset = "0.0 0.0 -0.55";
};
datablock afxXM_SpinData(SoR_Shield_spin_XM)
{
  spinAxis = "0 0 1";
  spinRate = 120.0;
};
datablock afxEffectWrapperData(SoR_Shield_EW)
{
  effect = SoR_Shield_CE;
  posConstraint = "impactedObject";
  fadeInTime  = 0.5;
  fadeOutTime = 0.5;
  lifetime = 5.5;
  xfmModifiers[0] = SoR_Shield_offset_XM;
  xfmModifiers[1] = SoR_Shield_spin_XM;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// TARGET ZODIAC

//
// This is a simple barbed zodiac that appears beneath the target
// of Spirit of Roach. Since this spell can be cast upon oneself,
// the spellcaster can also be the target. This zodiac is designed
// to combine well with the casting zodiac.
//

// the reveal glow for the target's zodiac
datablock afxZodiacData(SoR_TargetRevealZode_CE)
{  
  texture = %mySpellDataPath @ "/SoR/zodiacs/SoR_target_reveal";
  radius = 1.59;
  startAngle = 18.75;
  rotationRate = -75.0; //-45.0; //-30.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorIgnoreVertical = true;
};
//
datablock afxEffectWrapperData(SoR_TargetRevealZode_EW)
{
  effect = SoR_TargetRevealZode_CE;
  posConstraint = "impactedObject";
  lifetime = 0.75;
  fadeInTime = 0.5;
  fadeOutTime = 0.25;
  delay = 0.0;
};

// the barbed zodiac ring that appears under the target
datablock afxZodiacData(SoR_TargetZode_CE)
{  
  texture = %mySpellDataPath @ "/SoR/zodiacs/SoR_target";
  radius = 1.59; //3.0;
  startAngle = 0.0;
  rotationRate = -75.0; //-45.0; //-30.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorIgnoreVertical = true;
};
//
datablock afxEffectWrapperData(SoR_TargetZode_EW)
{
  effect = SoR_TargetZode_CE;
  posConstraint = "impactedObject";
  delay = 0.25;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 4.75;
};

// Target Zode Underglow
datablock afxZodiacData(SoR_TargetZode_TLKunderglow_CE : SoR_TargetZode_CE)
{
  color = "0.5 0.5 0.5 0.5";
  blend = normal;
};
//
datablock afxEffectWrapperData(SoR_TargetZode_TLKunderglow_EW : SoR_TargetZode_EW)
{
  effect = SoR_TargetZode_TLKunderglow_CE;
  execConditions = $BrightLighting_mask;
};

// this represents lighting from the target zodiac reveal
datablock afxLightData(SoR_RevealLight_CE)
{
  type = "Point";  
  radius = 3;
  color  = ($isTGEA) ? "4 4 4" : "0.5 0.5 0.5";

  lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF; // TGEA (TGA ignores)
  sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Near Linear";
};

datablock afxXM_WorldOffsetData(SoR_RevealLight_offset_XM)
{
  worldOffset = "0.0 0.0 1.0";
};
datablock afxEffectWrapperData(SoR_RevealLight_EW)
{
  effect = SoR_RevealLight_CE;
  posConstraint = "impactedObject";  
  delay = 0.0;
  fadeInTime  = 0.4;
  fadeOutTime = 0.75;
  lifetime = 0.75;
  xfmModifiers[0] = SoR_RevealLight_offset_XM;
};

if ($isTGEA)
{
  %SoR_TargetZodeLight_LMODELS_intensity = 25.0;
  %SoR_TargetZodeLight_LTERRAIN_intensity = 2.5;

  // this offset defines the lights distance beneath the terrain
  datablock afxXM_LocalOffsetData(SoR_TargetZodeLight_offset_XM)
  {
    localOffset = "0 0 -1";
  };

  datablock afxLightData(SoR_TargetZodeLight_LMODELS_CE)
  {
    type = "Point";
    radius = 2;
    sgCastsShadows = false;
    sgDoubleSidedAmbient = true;

    sgLightingModelName = "Inverse Square Fast Falloff";
    color = 0.067*%SoR_TargetZodeLight_LMODELS_intensity SPC
            0.698*%SoR_TargetZodeLight_LMODELS_intensity SPC
            0.773*%SoR_TargetZodeLight_LMODELS_intensity;

    lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF;
  };
  datablock afxLightData(SoR_TargetZodeLight_LTERRAIN_CE : SoR_TargetZodeLight_LMODELS_CE)
  {
    radius = 1.5;
    sgLightingModelName = "Inverse Square Fast Falloff";
    color = 0.067*%SoR_TargetZodeLight_LTERRAIN_intensity SPC
            0.698*%SoR_TargetZodeLight_LTERRAIN_intensity SPC
            0.773*%SoR_TargetZodeLight_LTERRAIN_intensity;

    lightIlluminationMask = $AFX::ILLUM_TERRAIN | $AFX::ILLUM_ATLAS;
  };

  datablock afxMultiLightData(SoR_TargetZodeLight_Multi_CE)
  {
    lights[0] = SoR_TargetZodeLight_LMODELS_CE;
    lights[1] = SoR_TargetZodeLight_offset_XM;
  };
  datablock afxEffectWrapperData(SoR_TargetZodeLight_Multi_EW : SoR_TargetZode_EW)
  {
    effect = SoR_TargetZodeLight_Multi_CE;
    xfmModifiers[0] = SoR_TargetZodeLight_offset_XM;
  };

  $SoR_TargetZodeLight = SoR_TargetZodeLight_Multi_EW;
}
else
{
  %SoR_TargetZodeLight_intensity = 2.5;

  // this offset defines the lights distance beneath the terrain
  datablock afxXM_LocalOffsetData(SoR_TargetZodeLight_offset_XM)
  {
    localOffset = "0 0 -4";
  };

  datablock afxLightData(SoR_TargetZodeLight_CE)
  {
    type = "Point";
    radius = 4;
    sgCastsShadows = false;
    sgDoubleSidedAmbient = true;

    sgLightingModelName = "SG - Near Linear (Lighting Pack)";
    color = 0.067*%SoR_TargetZodeLight_intensity SPC
            0.698*%SoR_TargetZodeLight_intensity SPC
            0.773*%SoR_TargetZodeLight_intensity;
  };
  //
  datablock afxEffectWrapperData(SoR_TargetZodeLight_EW : SoR_TargetZode_EW)
  {
    effect = SoR_TargetZodeLight_CE;
    xfmModifiers[0] = "SoR_TargetZodeLight_offset_XM";
  };

  $SoR_TargetZodeLight = SoR_TargetZodeLight_EW;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SOUNDS

// caster sounds

datablock AudioProfile(SoR_ZodiacSnd_CE)
{
   fileName = %mySpellDataPath @ "/SoR/sounds/sor_zodiac.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(SoR_ZodiacSnd_EW)
{
  effect = SoR_ZodiacSnd_CE;
  constraint = "caster";
  delay = 0.0;
  lifetime = 1.999;
};

datablock AudioProfile(SoR_ConjureSnd_CE)
{
   fileName = %mySpellDataPath @ "/SoR/sounds/sor_conjure.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(SoR_ConjureSnd_EW)
{
  effect = SoR_ConjureSnd_CE;
  constraint = "caster";
  delay = 0.8;
  lifetime = 2.141;
};

// target sounds //

datablock AudioProfile(SoR_TargetBugoffSnd_CE)
{
   fileName = %mySpellDataPath @ "/SoR/sounds/sor_targ_bugoff.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(SoR_TargetBugoffSnd_EW)
{
  effect = SoR_TargetBugoffSnd_CE;
  constraint = "impactedObject";
  delay = 3.0;
  scaleFactor = 0.8;
  lifetime = 0.269;
};

datablock AudioProfile(SoR_TargetEndSnd_CE)
{
   fileName = %mySpellDataPath @ "/SoR/sounds/sor_targ_end.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(SoR_TargetEndSnd_EW)
{
  effect = SoR_TargetEndSnd_CE;
  constraint = "impactedObject";
  delay = 5.5;
  scaleFactor = 0.8;
  lifetime = 1.096;
};

datablock AudioProfile(SoR_TargetManifestSnd_CE)
{
   fileName = %mySpellDataPath @ "/SoR/sounds/sor_targ_manif.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(SoR_TargetManifestSnd_EW)
{
  effect = SoR_TargetManifestSnd_CE;
  constraint = "impactedObject";
  delay = 0.5;
  scaleFactor = 0.8;
  lifetime = 2.494;
};

datablock AudioProfile(SoR_TargetManifestLoop_CE)
{
   fileName = %mySpellDataPath @ "/SoR/sounds/sor_targ_manif_loop.ogg";
   description = SpellAudioLoop_AD;
   preload = false;
};
datablock afxEffectWrapperData(SoR_TargetManifestLoop_EW)
{
  effect = SoR_TargetManifestLoop_CE;
  constraint = "impactedObject";
  delay = 1.0;
  lifetime = 4.5;
  fadeOutTime = 1.2;
  scaleFactor = 0.8;
};

datablock AudioProfile(SoR_TargetZodeSnd_CE)
{
   fileName = %mySpellDataPath @ "/SoR/sounds/sor_targ_zode.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(SoR_TargetZodeSnd_EW)
{
  effect = SoR_TargetZodeSnd_CE;
  constraint = "impactedObject";
  delay = 0;
  scaleFactor = 0.8;
  lifetime = 1.483;
};


// shockwave sounds //

datablock afxXM_ShockwaveData(SoR_Shockwave_XM)
{
  rate = 20.0;
  aimZOnly = true;
};
datablock afxXM_GroundConformData(SoR_Shockwave_Ground_XM)
{
  height = 1.0;
  conformToInteriors = false;
};

datablock AudioProfile(SoR_ShockwaveSnd_CE)
{
   fileName = %mySpellDataPath @ "/SoR/sounds/sor_ring_loop.ogg";
   description = SpellAudioShockwaveLoop_soft_AD;
   preload = false;
};
datablock afxEffectWrapperData(SoR_ShockwaveSnd_EW)
{
  effect = SoR_ShockwaveSnd_CE;
  delay = 1.35;
  lifetime = 3.0;
  fadeInTime = 0.5;
  fadeOutTime = 1.0;

  posConstraint = "caster";
  posConstraint2 = "listener";
  xfmModifiers[0] = SoR_Shockwave_XM;
  xfmModifiers[1] = SoR_Shockwave_Ground_XM;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SPIRIT OF ROACH
//

datablock afxMagicSpellData(SpiritOfRoachSpell)
{
  reagentCost = 2;
  reagentName = "Cockroach Carapace";

    // warmup //
  castingDur = 1.0;

    // casting zodiac //
  addCastingEffect = SoR_ZodeReveal_EW;
  addCastingEffect = SoR_MainZodeRevealLight_1_EW;
  addCastingEffect = SoR_MainZodeRevealLight_2_EW;
  addCastingEffect = SoR_MainZodeRevealLight_3_EW;
  addCastingEffect = SoR_Zode1_TLKunderglow_EW;
  addCastingEffect = SoR_Zode1_EW;
  addCastingEffect = SoR_Zode2_EW;
  addCastingEffect = SoR_Zode3_EW;
  addCastingEffect = $SoR_CastingZodeLight;
    // casting sounds //
  addCastingEffect = SoR_ZodiacSnd_EW;
  addCastingEffect = SoR_ConjureSnd_EW;
    // spellcaster animation //
  addCastingEffect = SoR_Summon_Clip_EW;
    // fallout ring //
  addCastingEffect = SoR_FalloutZode1_EW;
  addCastingEffect = SoR_FalloutZode2_EW;
  addCastingEffect = SoR_ShockwaveSnd_EW;
  addCastingEffect = $SoR_FalloutLight;

    // roach icon halo //
  addImpactEffect = SoR_Halo_EW;
  addImpactEffect = SoR_RoachLight_EW;
    // shield bubble //
  addImpactEffect = SoR_Shield_EW;
    // target zodiac //
  addImpactEffect = SoR_TargetRevealZode_EW;
  addImpactEffect = SoR_TargetZode_TLKunderglow_EW;
  addImpactEffect = SoR_TargetZode_EW;
  addImpactEffect = SoR_RevealLight_EW;
  addImpactEffect = $SoR_TargetZodeLight;
    // target sounds //
  addImpactEffect = SoR_TargetZodeSnd_EW;
  addImpactEffect = SoR_TargetManifestSnd_EW;
  addImpactEffect = SoR_TargetManifestLoop_EW;
  addImpactEffect = SoR_TargetBugoffSnd_EW;
  addImpactEffect = SoR_TargetEndSnd_EW;
};
//
datablock afxRPGMagicSpellData(SpiritOfRoachSpell_RPG)
{
  name = "Spirit of Roach";
  desc = "The proclivity of the lowly cockroach to RESIST all imaginable forms of MAGIC " @
         "is now yours. Respect the Cockroach!" @
         "\n" @
         "\nspell design: Matthew Durante" @
         "\nsound effects: Dave Schroeder" @ 
         "\nspell concept: Jeff Faust";
  sourcePack = "Spell Pack 1";
         
  iconBitmap = %mySpellDataPath @ "/SoR/icons/sor";
  target = "friend";
  canTargetSelf = true;
  range = 40;
  manaCost = 10;
  reagentCost = 2;
  reagentName = "Cockroach Carapace";
  castingDur = SpiritOfRoachSpell.castingDur;
};

// set a level of detail
function SpiritOfRoachSpell::onActivate(%this, %spell, %caster, %target)
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
  SpiritOfRoachSpell.scriptFile = $afxAutoloadScriptFile;
  SpiritOfRoachSpell.spellDataPath = %mySpellDataPath;
  if (isFunction(addDemoSpellbookSpell))
    addDemoSpellbookSpell(SpiritOfRoachSpell, SpiritOfRoachSpell_RPG);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
