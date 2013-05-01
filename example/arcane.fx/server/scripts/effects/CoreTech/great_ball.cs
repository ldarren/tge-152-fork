
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// GREAT BALL OF FIRE SPELL
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
$spell_reload = isObject(GreatBallSpell);
if ($spell_reload)
{
  // mark datablocks so we can detect which are reloaded this script
  markDataBlocks();
  // reset data path from previously saved value
  %mySpellDataPath = GreatBallSpell.spellDataPath;
  GreatBallSpell.reset();
}
else
{
  // set data path from default plus containing folder name
  %mySpellDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

$BrightLighting_mask = 0x800000; // BIT(23);
$isTGEA = (afxGetEngine() $= "TGEA");

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CASTING ZODIAC

//
// The main casting zodiac is formed by three zodiacs plus a white
// reveal glow when the casting first starts. Holes are left for 
// the five small portal zodiacs described below.
//

// this is the white reveal glow
datablock afxZodiacData(GBoF_ZodeReveal_CE)
{  
  texture = %mySpellDataPath @ "/GBoF/zodiacs/GBofF_reveal";
  radius = 3.0;
  startAngle = 0.0;
  rotationRate = -30.0;
  //color = "1.0 0.5 0.0 0.4";
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(GBoF_Zode_Reveal_EW)
{
  effect = GBoF_ZodeReveal_CE;
  posConstraint = caster;
  lifetime = 0.75;
  fadeInTime = 0.5;
  fadeOutTime = 0.25;
  xfmModifiers[0] = SHARED_freeze_XM;
  propagateTimeFactor = true;
};

%GBoF_MainZodeRevealLight_intensity = 2.5;

datablock afxXM_LocalOffsetData(GBoF_MainZodeRevealLight_offset_XM)
{
  //localOffset = "0 2 0";  // point
  localOffset =  "0 2 -4";  // spot
};
datablock afxXM_SpinData(GBoF_MainZodeRevealLight_spin1_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 0;
  spinRate = -30;
};
datablock afxXM_SpinData(GBoF_MainZodeRevealLight_spin2_XM : GBoF_MainZodeRevealLight_spin1_XM)
{
  spinAngle = 120;
};
datablock afxXM_SpinData(GBoF_MainZodeRevealLight_spin3_XM : GBoF_MainZodeRevealLight_spin1_XM)
{
  spinAngle = 240;
};

// main zode reveal light
datablock afxLightData(GBoF_MainZodeRevealLight_CE) // AFX-SPOTLIGHT
{
  type = "Spot";
  radius = ($isTGEA) ? 10 : 5;
  direction = "0 -0.313 0.95";
  sgCastsShadows = false;
  sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Near Linear";
  color = 1.0*%GBoF_MainZodeRevealLight_intensity SPC
          1.0*%GBoF_MainZodeRevealLight_intensity SPC
          1.0*%GBoF_MainZodeRevealLight_intensity;
  lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF; // TGEA (ignored by TGE)
};

datablock afxEffectWrapperData(GBoF_MainZodeRevealLight_1_EW : GBoF_Zode_Reveal_EW)
{
  effect = GBoF_MainZodeRevealLight_CE;
  xfmModifiers[1] = GBoF_MainZodeRevealLight_spin1_XM;
  xfmModifiers[2] = GBoF_MainZodeRevealLight_offset_XM;
};
datablock afxEffectWrapperData(GBoF_MainZodeRevealLight_2_EW : GBoF_Zode_Reveal_EW)
{
  effect = GBoF_MainZodeRevealLight_CE;
  xfmModifiers[1] = GBoF_MainZodeRevealLight_spin2_XM;
  xfmModifiers[2] = GBoF_MainZodeRevealLight_offset_XM;
};
datablock afxEffectWrapperData(GBoF_MainZodeRevealLight_3_EW : GBoF_Zode_Reveal_EW)
{
  effect = GBoF_MainZodeRevealLight_CE;
  xfmModifiers[1] = GBoF_MainZodeRevealLight_spin3_XM;
  xfmModifiers[2] = GBoF_MainZodeRevealLight_offset_XM;
};

// this is the main pattern of the casting zode
datablock afxZodiacData(GBoF_Zode1)
{  
  texture = %mySpellDataPath @ "/GBoF/zodiacs/GBofF_rings";
  radius = 3.0;
  startAngle = 0.0;
  rotationRate = -30.0;
  //color = "1.0 0.5 0.0 0.4";
  color = "1.0 1.0 1.0 0.8";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(GBoF_Zode1_EW)
{
  effect = GBoF_Zode1;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 5.0;
  xfmModifiers[0] = SHARED_freeze_XM;
  propagateTimeFactor = true;
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
datablock afxZodiacData(GBoF_Zode1_TLKunderglow_CE : GBoF_Zode1)
{
  texture = %mySpellDataPath @ "/GBoF/zodiacs/GBofF_rings-underglow256";
  color = "0.45 0.45 0.45 0.45";
  blend = normal;
};
//
datablock afxEffectWrapperData(GBoF_Zode1_TLKunderglow_EW : GBoF_Zode1_EW)
{
  effect = GBoF_Zode1_TLKunderglow_CE;
  execConditions = $BrightLighting_mask;
};

// this is the rune ring portion of the casting zode
datablock afxZodiacData(GBoF_Zode2)
{  
  texture = %mySpellDataPath @ "/GBoF/zodiacs/zode_text";
  radius = 3.0;
  startAngle = 0.0;
  rotationRate = 20.0; //60
  color = "1.0 0.0 0.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(GBoF_Zode2_EW)
{
  effect = GBoF_Zode2;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 5.0;
  xfmModifiers[0] = SHARED_freeze_XM;
  propagateTimeFactor = true;
};

// Runes & Skulls Zode Underglow
//  Here the zode is made black but only slightly opaque to subtly
//  darken the ground, making the additive glow zode appear more
//  saturated.
datablock afxZodiacData(GBoF_Zode2_TLKunderglow_CE : GBoF_Zode2)
{
  color = "0 0 0 0.25";
  blend = normal;
};
//
datablock afxEffectWrapperData(GBoF_Zode2_TLKunderglow_EW : GBoF_Zode2_EW)
{
  effect = GBoF_Zode2_TLKunderglow_CE;
  execConditions = $BrightLighting_mask;
};

// this layer adds sketchy white symbols to the casting zode
datablock afxZodiacData(GBoF_Zode3)
{  
  texture = %mySpellDataPath @ "/GBoF/zodiacs/zode_symbols";
  radius = 3.0;
  startAngle = 0.0;
  rotationRate = -30.0;
  color = "1.0 1.0 1.0 0.5";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(GBoF_Zode3_EW)
{
  effect = GBoF_Zode3;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 5.0;
  xfmModifiers[0] = SHARED_freeze_XM;
  propagateTimeFactor = true;
};

if ($isTGEA)
{
  %GBoF_CastingZodeLight_LMODELS_intensity = 25.0;
  %GBoF_CastingZodeLight_LTERRAIN_intensity = 2.5;

  // this offset defines the lights distance beneath the terrain  
  datablock afxXM_LocalOffsetData(GBoF_CastingZodeLight_offset_XM)
  {
    localOffset = "0 0 -2";
  };

  datablock afxLightData(GBoF_CastingZodeLight_LMODELS_CE)
  {
    type = "Point";
    radius = 3;
    sgCastsShadows = false;
    sgDoubleSidedAmbient = true;

    sgLightingModelName = "Inverse Square Fast Falloff";

    color = 1.0*%GBoF_CastingZodeLight_LMODELS_intensity SPC
            0.5*%GBoF_CastingZodeLight_LMODELS_intensity SPC
            0.0*%GBoF_CastingZodeLight_LMODELS_intensity;

    lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF;
  };
  datablock afxLightData(GBoF_CastingZodeLight_LTERRAIN_CE : GBoF_CastingZodeLight_LMODELS_CE)
  {
    radius = 2.25;
    color = 1.0*%GBoF_CastingZodeLight_LTERRAIN_intensity SPC
            0.5*%GBoF_CastingZodeLight_LTERRAIN_intensity SPC
            0.0*%GBoF_CastingZodeLight_LTERRAIN_intensity;

    lightIlluminationMask = $AFX::ILLUM_TERRAIN | $AFX::ILLUM_ATLAS;
  };

  datablock afxMultiLightData(GBoF_CastingZodeLight_Multi_CE)
  {
    lights[0] = GBoF_CastingZodeLight_LMODELS_CE;
    lights[1] = GBoF_CastingZodeLight_LTERRAIN_CE;
  };
  datablock afxEffectWrapperData(GBoF_CastingZodeLight_Multi_EW : GBoF_Zode1_EW)
  {
    effect = GBoF_CastingZodeLight_Multi_CE;
    xfmModifiers[0] = GBoF_CastingZodeLight_offset_XM;
  };

  $GBoF_CastingZodeLight = GBoF_CastingZodeLight_Multi_EW;
}
else
{
  %GBoF_CastingZodeLight_intensity = 2.5;

  // this offset defines the lights distance beneath the terrain
  datablock afxXM_LocalOffsetData(GBoF_CastingZodeLight_offset_XM)
  {
    localOffset = "0 0 -4";
  };

  datablock afxLightData(GBoF_CastingZodeLight_CE)
  {
    type = "Point";
    radius = 5;
    sgCastsShadows = false;
    sgDoubleSidedAmbient = true;

    color = 1.0*%GBoF_CastingZodeLight_intensity SPC
            0.5*%GBoF_CastingZodeLight_intensity SPC
            0.0*%GBoF_CastingZodeLight_intensity;
  };
  //
  datablock afxEffectWrapperData(GBoF_CastingZodeLight_EW : GBoF_Zode1_EW)
  {
    effect = GBoF_CastingZodeLight_CE;
    xfmModifiers[0] = GBoF_CastingZodeLight_offset_XM;
  };

  $GBoF_CastingZodeLight = GBoF_CastingZodeLight_EW;
}


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// PORTAL ZODIACS

//
// These five smaller zodiacs match holes in the main zodiac and
// define the portal locations where the portal-beams materialize.
//

datablock afxZodiacData(GBoF_ZodePortal)
{  
  texture = %mySpellDataPath @ "/GBoF/zodiacs/ALL_portal";
  radius = 0.45;
  startAngle = 0.0;
  rotationRate = 240.0;
  color = "1.0 1.0 1.0 1.0";
 // blend = additive;

  growInTime = 1.0;
  interiorHorizontalOnly = true;
};

// this offset defines the radius of the portal-zodes
datablock afxXM_LocalOffsetData(GBoF_ZodePortal_Offset_XM)
{
  localOffset = "0 1.5 0";
};

// this and the other spin modifiers set the starting
// angle and rotation rate of the portal-beams.
datablock afxXM_SpinData(GBoF_ZodePortal_Spin1_XM)
{
  spinAxis = "0 0 1";
  spinAngle = -15; // 0-15
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_ZodePortal1_EW)
{
  effect = GBoF_ZodePortal;
  posConstraint = "caster";
  delay = 0.75;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 3.0;
  xfmModifiers[0] = "GBoF_ZodePortal_Spin1_XM";
  xfmModifiers[1] = "GBoF_ZodePortal_Offset_XM";
};

datablock afxXM_SpinData(GBoF_ZodePortal_Spin2_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 57; // 72-15
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_ZodePortal2_EW)
{
  effect = GBoF_ZodePortal;
  posConstraint = caster;
  delay = 0.75;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 3.0;
  xfmModifiers[0] = "GBoF_ZodePortal_Spin2_XM";
  xfmModifiers[1] = "GBoF_ZodePortal_Offset_XM";
};

datablock afxXM_SpinData(GBoF_ZodePortal_Spin3_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 129; // 144-15
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_ZodePortal3_EW)
{
  effect = GBoF_ZodePortal;
  posConstraint = caster;
  delay = 0.75;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 3.0;
  xfmModifiers[0] = "GBoF_ZodePortal_Spin3_XM";
  xfmModifiers[1] = "GBoF_ZodePortal_Offset_XM";
};

datablock afxXM_SpinData(GBoF_ZodePortal_Spin4_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 201; // 216-15
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_ZodePortal4_EW)
{
  effect = GBoF_ZodePortal;
  posConstraint = caster;
  delay = 0.75;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 3.0;
  xfmModifiers[0] = "GBoF_ZodePortal_Spin4_XM";
  xfmModifiers[1] = "GBoF_ZodePortal_Offset_XM";
};

datablock afxXM_SpinData(GBoF_ZodePortal_Spin5_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 273; // 288-15
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_ZodePortal5_EW)
{
  effect = GBoF_ZodePortal;
  posConstraint = caster;
  delay = 0.75;
  fadeInTime = 0.75;
  fadeOutTime = 1.0;
  lifetime = 3.0;
  xfmModifiers[0] = "GBoF_ZodePortal_Spin5_XM";
  xfmModifiers[1] = "GBoF_ZodePortal_Offset_XM";
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// PORTAL BEAMS
//

//
// These five glowing columns of light materialize from the
// portal zodiacs.
//

// this glowing cylinder is shared by the five portal-beam effects
datablock afxModelData(GBoF_portalBeam_model_CE)
{
   shapeFile = %mySpellDataPath @ "/GBoF/models/GBofF_portalBeam.dts"; 
   sequence = "arise";
   alphaMult = ($isTGEA) ? 0.5 : 0.65;
   forceOnMaterialFlags = $MaterialFlags::Translucent; // TGE (ignored by TGEA)
   remapTextureTags = "portalbeamcolor2.png:GBoF_beam"; // TGEA (ignored by TGE)
};

// this offset defines the radius of the portal-beams
datablock afxXM_LocalOffsetData(GBoF_portalBeam_Offset)
{
  localOffset = "0 1.1 0";
};

// this modifier keeps the beams at ground level
datablock afxXM_GroundConformData(GBoF_portalBeam_Ground)
{
  height = 0.0;
};

// this and the other spin modifiers set the starting
// angle and rotation rate of the portal-beams.
datablock afxXM_SpinData(GBoF_portalBeam_Spin1_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 0;
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_portalBeam1_EW)
{
  effect     = GBoF_portalBeam_model_CE;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.1;
  fadeOutTime = 0.75;
  lifetime = 2.25;
  xfmModifiers[0] = "GBoF_portalBeam_Spin1_XM";
  xfmModifiers[1] = "GBoF_portalBeam_Offset";
  xfmModifiers[2] = "GBoF_portalBeam_Ground";
  propagateTimeFactor = true;
};

datablock afxXM_SpinData(GBoF_portalBeam_Spin2_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 72;
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_portalBeam2_EW)
{
  effect     = GBoF_portalBeam_model_CE;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.1;
  fadeOutTime = 0.75;
  lifetime = 2.25;
  xfmModifiers[0] = "GBoF_portalBeam_Spin2_XM";
  xfmModifiers[1] = "GBoF_portalBeam_Offset";
  xfmModifiers[2] = "GBoF_portalBeam_Ground";
  propagateTimeFactor = true;
};

datablock afxXM_SpinData(GBoF_portalBeam_Spin3_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 144;
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_portalBeam3_EW)
{
  effect     = GBoF_portalBeam_model_CE;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.1;
  fadeOutTime = 0.75;
  lifetime = 2.25;
  xfmModifiers[0] = "GBoF_portalBeam_Spin3_XM";
  xfmModifiers[1] = "GBoF_portalBeam_Offset";
  xfmModifiers[2] = "GBoF_portalBeam_Ground";
  propagateTimeFactor = true;
};

datablock afxXM_SpinData(GBoF_portalBeam_Spin4_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 216;
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_portalBeam4_EW)
{
  effect     = GBoF_portalBeam_model_CE;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.1;
  fadeOutTime = 0.75;
  lifetime = 2.25;
  xfmModifiers[0] = "GBoF_portalBeam_Spin4_XM";
  xfmModifiers[1] = "GBoF_portalBeam_Offset";
  xfmModifiers[2] = "GBoF_portalBeam_Ground";
  propagateTimeFactor = true;
};

datablock afxXM_SpinData(GBoF_portalBeam_Spin5_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 288;
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_portalBeam5_EW)
{
  effect     = GBoF_portalBeam_model_CE;
  posConstraint = caster;
  delay = 0.25;
  fadeInTime = 0.1;
  fadeOutTime = 0.75;
  lifetime = 2.25;
  xfmModifiers[0] = "GBoF_portalBeam_Spin5_XM";
  xfmModifiers[1] = "GBoF_portalBeam_Offset";
  xfmModifiers[2] = "GBoF_portalBeam_Ground";
  propagateTimeFactor = true;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// PORTAL FLAMES

//
// portal flame paths
//
// These paths are used for the particle flames and 
// point lights of the fire-portals. 
//
// Objects following these paths start with a small hovering
// movement then zip up to a point above the caster's head
// where the big fireball appears.
//

datablock afxPathData(GBoF_FirePortalPath1_U)
{
  points = "0 0 0	0 0 .7";  
  lifetime = 1.0;
};

datablock afxPathData(GBoF_FirePortalPath2_U)
{
  points = "0 0 0	0 0.4 1		0 -1.4 2.65";  
  delay  = 2.0;
  lifetime = 0.75;
};

//
// portal flames particles
//
// The following particles and emitter are used to create the 
// five small portal flames that appear to coalesce, forming
// the huge fireball.
//

datablock ParticleData(GBoF_FirePortal_P)
{
   // TGE textureName          = %mySpellDataPath @ "/GBoF/particles/starfire";
   dragCoeffiecient     = 0.5;
   //gravityCoefficient   = 0.2;
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
   sizes[0]             = 0.1;
   sizes[1]             = 0.3;
   sizes[2]             = 0.1;
   sizes[3]             = 0.07;
   times[0]             = 0.0;
   times[1]             = 0.2;
   times[2]             = 0.55;
   times[3]             = 1.0;

   textureName          = %mySpellDataPath @ "/GBoF/particles/gbof_tiled_parts"; // starfire
   textureCoords[0]     = "0.25 0.75";
   textureCoords[1]     = "0.25 1.0";
   textureCoords[2]     = "0.5  1.0";
   textureCoords[3]     = "0.5  0.75";
};
//
datablock ParticleEmitterData(GBoF_FirePortal_E)
{
  ejectionOffset        = 0.02;
  ejectionPeriodMS      = 10;
  periodVarianceMS      = 0;
  ejectionVelocity      = 0.4; //0.8;
  velocityVariance      = 0.1; //0.00;
  thetaMin              = 0.0;
  thetaMax              = 0.0;
  //phiReferenceVel       = 90;
  //phiVariance           = 180;
  particles             = GBoF_FirePortal_P;
};

// the fire-portal modifiers are shared with the 
// fire-portal lights.

// this offset defines the radius of the portal-flames
datablock afxXM_LocalOffsetData(GBoF_FirePortal_Offset_XM)
{
  localOffset = "0 1.5 0";
};

// this modifier conforms the portal flames to a path
datablock afxXM_PathConformData(GBoF_FirePortal_Path_XM)
{
  paths = "GBoF_FirePortalPath1_U GBoF_FirePortalPath2_U";
};

//
// portal flames 
//
// The following effects create the five small portal
// flames.
//

// this and the other spin modifiers set the starting
// angle and rotation rate of the portal-flames.
datablock afxXM_SpinData(GBoF_FirePortal_Spin1_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 15; //0+15
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_FirePortal1_EW)
{
  effect = GBoF_FirePortal_E;
  posConstraint = caster;
  delay = 2.0;
  lifetime = 3.75;
  xfmModifiers[0] = "GBoF_FirePortal_Spin1_XM";
  xfmModifiers[1] = "GBoF_FirePortal_Offset_XM";
  xfmModifiers[2] = "GBoF_FirePortal_Path_XM";
};

datablock afxXM_SpinData(GBoF_FirePortal_Spin2_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 87; // 72+15
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_FirePortal2_EW)
{
  effect = GBoF_FirePortal_E;
  posConstraint = caster;
  delay = 2.0;
  lifetime = 3.75;
  xfmModifiers[0] = "GBoF_FirePortal_Spin2_XM";
  xfmModifiers[1] = "GBoF_FirePortal_Offset_XM";
  xfmModifiers[2] = "GBoF_FirePortal_Path_XM";
};

datablock afxXM_SpinData(GBoF_FirePortal_Spin3_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 159; //144+15
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_FirePortal3_EW)
{
  effect = GBoF_FirePortal_E;
  posConstraint = caster;
  delay = 2.0;
  lifetime = 3.75;
  xfmModifiers[0] = "GBoF_FirePortal_Spin3_XM";
  xfmModifiers[1] = "GBoF_FirePortal_Offset_XM";
  xfmModifiers[2] = "GBoF_FirePortal_Path_XM";
};

datablock afxXM_SpinData(GBoF_FirePortal_Spin4_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 231; //216+15
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_FirePortal4_EW)
{
  effect = GBoF_FirePortal_E;
  posConstraint = caster;
  delay = 2.0;
  lifetime = 3.75;
  xfmModifiers[0] = "GBoF_FirePortal_Spin4_XM";
  xfmModifiers[1] = "GBoF_FirePortal_Offset_XM";
  xfmModifiers[2] = "GBoF_FirePortal_Path_XM";
};

datablock afxXM_SpinData(GBoF_FirePortal_Spin5_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 303; // 288+15
  spinRate = -30;
};
//
datablock afxEffectWrapperData(GBoF_FirePortal5_EW)
{
  effect = GBoF_FirePortal_E;
  posConstraint = caster;
  delay = 2.0;
  lifetime = 3.75;
  xfmModifiers[0] = "GBoF_FirePortal_Spin5_XM";
  xfmModifiers[1] = "GBoF_FirePortal_Offset_XM";
  xfmModifiers[2] = "GBoF_FirePortal_Path_XM";
};

//
// portal flame lights 
//
// The following lights represent lighting cast by the
// five small portal flames. Therefore, the share the
// same set of modifiers. 
// 

datablock sgLightObjectData(GBoF_FirePortalLight1_CE) 
{
  CastsShadows = false;
  Radius = 0.8; 
  Brightness = 0.75;
  Colour = "1.0 0.6 0.0";

  FlareOn = true;
  LinkFlare = true;
  //LinkFlareSize = true;
  //FlareBitmap = "common/lighting/corona"; //lightFalloffMono";
  FlareBitmap = %mySpellDataPath @ "/GBoF/lights/GBoF_firePortalFlare";
  NearSize = 1;
  FarSize  = 0.5;
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
datablock sgLightObjectData(GBoF_FirePortalLight2_CE : GBoF_FirePortalLight1_CE) 
{
  BrightnessTime = 0.2;
  RotationTime = 1.7*0.2;
};
datablock sgLightObjectData(GBoF_FirePortalLight3_CE : GBoF_FirePortalLight1_CE) 
{
  BrightnessTime = 0.3;
  RotationTime = 2.0*0.2;
};
datablock sgLightObjectData(GBoF_FirePortalLight4_CE : GBoF_FirePortalLight1_CE) 
{
  BrightnessTime = 0.15;
  RotationTime = 1.0*0.2;
};
datablock sgLightObjectData(GBoF_FirePortalLight5_CE : GBoF_FirePortalLight1_CE) 
{
  BrightnessTime = 0.25;
  RotationTime = 2.3*0.2;
};

datablock afxEffectWrapperData(GBoF_FirePortalLight1_EW)
{
  effect = GBoF_FirePortalLight1_CE;
  posConstraint = caster;
  delay = 2.0;
  lifetime = 3.5;
  fadeInTime  = 0.5;
  fadeOutTime = 0.25;
  xfmModifiers[0] = "GBoF_FirePortal_Spin1_XM";
  xfmModifiers[1] = "GBoF_FirePortal_Offset_XM";
  xfmModifiers[2] = "GBoF_FirePortal_Path_XM";
};
datablock afxEffectWrapperData(GBoF_FirePortalLight2_EW)
{
  effect = GBoF_FirePortalLight2_CE;
  posConstraint = caster;
  delay = 2.0;
  lifetime = 3.5;
  fadeInTime  = 0.5;
  fadeOutTime = 0.25;
  xfmModifiers[0] = "GBoF_FirePortal_Spin2_XM";
  xfmModifiers[1] = "GBoF_FirePortal_Offset_XM";
  xfmModifiers[2] = "GBoF_FirePortal_Path_XM";
};
datablock afxEffectWrapperData(GBoF_FirePortalLight3_EW)
{
  effect = GBoF_FirePortalLight3_CE;
  posConstraint = caster;
  delay = 2.0;
  lifetime = 3.5;
  fadeInTime  = 0.5;
  fadeOutTime = 0.25;
  xfmModifiers[0] = "GBoF_FirePortal_Spin3_XM";
  xfmModifiers[1] = "GBoF_FirePortal_Offset_XM";
  xfmModifiers[2] = "GBoF_FirePortal_Path_XM";
};
datablock afxEffectWrapperData(GBoF_FirePortalLight4_EW)
{
  effect = GBoF_FirePortalLight4_CE;
  posConstraint = caster;
  delay = 2.0;
  lifetime = 3.5;
  fadeInTime  = 0.5;
  fadeOutTime = 0.25;
  xfmModifiers[0] = "GBoF_FirePortal_Spin4_XM";
  xfmModifiers[1] = "GBoF_FirePortal_Offset_XM";
  xfmModifiers[2] = "GBoF_FirePortal_Path_XM";
};
datablock afxEffectWrapperData(GBoF_FirePortalLight5_EW)
{
  effect = GBoF_FirePortalLight5_CE;
  posConstraint = caster;
  delay = 2.0;
  lifetime = 3.5;
  fadeInTime  = 0.5;
  fadeOutTime = 0.25;
  xfmModifiers[0] = "GBoF_FirePortal_Spin5_XM";
  xfmModifiers[1] = "GBoF_FirePortal_Offset_XM";
  xfmModifiers[2] = "GBoF_FirePortal_Path_XM";
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// HUGE FIREBALL

//
// fireball particles
//
// The following particles and emitter are used to create the 
// huge fireball. They are used both by the missile and effects
// that statically form the fireball over the spellcaster's head.
//

datablock ParticleData(GBoF_HugeFireBall_P)
{
   // TGE textureName          = %mySpellDataPath @ "/GBoF/particles/smokeParticle";
   dragCoeffiecient     = 0.0;
   gravityCoefficient   = 0.0;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 500;
   lifetimeVarianceMS   = 100;
   useInvAlpha          = false;
   spinRandomMin        = -900.0;
   spinRandomMax        = 900.0;
   colors[0]            = "1.0 1.0 1.0 1.0";
   colors[1]            = "1.0 0.8 0.0 1.0";
   colors[2]            = "1.0 0.0 0.0 1.0";
   sizes[0]             = 1.75;
   sizes[1]             = 0.85;
   sizes[2]             = 0.1;
   times[0]             = 0.0;
   times[1]             = 0.3;
   times[2]             = 1.0;

   textureName          = %mySpellDataPath @ "/GBoF/particles/gbof_tiled_parts"; // smokeParticle
   textureCoords[0]     = "0.75 0.75";
   textureCoords[1]     = "0.75 1.0";
   textureCoords[2]     = "1.0  1.0";
   textureCoords[3]     = "1.0  0.75";
};
//
datablock ParticleData(GBoF_HugeFireBall_P2)
{
   // TGE textureName          = %mySpellDataPath @ "/GBoF/particles/GBofF_fireBall"; //firetest3";
   dragCoeffiecient     = 0.0;
   gravityCoefficient   = 0.0;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 500;
   lifetimeVarianceMS   = 100;
   useInvAlpha          = false;
   spinRandomMin        = -900.0;
   spinRandomMax        = 900.0;
   colors[0]            = "1.0 1.0 0.0 1.0";
   colors[1]            = "1.0 0.0 0.0 1.0";
   colors[2]            = "1.0 0.0 0.0 1.0";
   sizes[0]             = 3.0;
   sizes[1]             = 1.2;
   sizes[2]             = 0.2;
   times[0]             = 0.0;
   times[1]             = 0.3;
   times[2]             = 1.0;

   textureName          = %mySpellDataPath @ "/GBoF/particles/gbof_tiled_parts"; // fireBall
   textureCoords[0]     = "0.0 0.0";
   textureCoords[1]     = "0.0 0.5";
   textureCoords[2]     = "0.5 0.5";
   textureCoords[3]     = "0.5 0.0";
};
//
datablock ParticleEmitterData(GBoF_HugeFireBall_E)
{
  ejectionPeriodMS      = 2;
  periodVarianceMS      = 1;
  ejectionVelocity      = 0.25;
  velocityVariance      = 0.10;
  thetaMin              = 0;
  thetaMax              = 180;
  phiReferenceVel       = 90;
  phiVariance           = 180;
  particles             = "GBoF_HugeFireBall_P GBoF_HugeFireBall_P2"; 
};

//
// fireball location
//
// This offset defines the starting point of the huge fireball
//
datablock afxXM_WorldOffsetData(GBoF_FireBall_Offset_XM)
{
  worldOffset = "0 0 3.4";
};

//
// static fireball
//
// This effect places introduces the huge fireball above
// the spellcaster's head.
// 
datablock afxEffectWrapperData(GBoF_FireBall_Static_EW)
{
  effect = GBoF_HugeFireBall_E;
  constraint = caster;
  delay = 4.6; //3.6; //3.75;
  //lifetime = 1.4; //2.4;
  lifetime = 1.2; //2.4;
  xfmModifiers[0] = GBoF_FireBall_Offset_XM;
};

// 
// static fireball light
//
// This light represents light from the huge fireball. Note that
// the parameters match those of the light belonging to the missile.
//
// The fireball light is simulated using two lights, each casting 
// dynamic shadows and animated with a path.  The path gives the
// lights a fast wiggle which is very visible in the shadow.  Two
// lights are used to create an effective color on the groundplane:
// the larger-radius yellow light followed by a smaller white light
// that whitens the center.
//

datablock afxPathData(GBoF_FireBallLight_TLK_1_Path)
{
  points = " 0    0    0"   SPC
           " 0.1 -0.3  0.8" SPC
           "-0.3  0.2 -0.6" SPC
           " 0.0 -0.6  0.4" SPC
           " -0.7 0.4 -0.8" SPC
           " 0    0    0";
  lifetime = 0.25;
  loop = cycle;
  mult = 0.65;
};
//
datablock afxXM_PathConformData(GBoF_FireBallLight_TLK_1_path_XM)
{
  paths = "GBoF_FireBallLight_TLK_1_Path";
};

%GBoF_FireBallLight_1_intensity = ($isTGEA) ? 0.5 : 1.5;
datablock afxLightData(GBoF_FireBallLight_1_CE)
{
  type = "Point";  
  radius = 7;
  sgCastsShadows = true;

  sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Inverse Square Fast Falloff";
  color = 1.0*%GBoF_FireBallLight_1_intensity SPC
          0.9*%GBoF_FireBallLight_1_intensity SPC
          0.0*%GBoF_FireBallLight_1_intensity;
};  
     
datablock afxEffectWrapperData(GBoF_FireBallLight_1_EW)
{
  effect = GBoF_FireBallLight_1_CE;
  posConstraint = "caster";
  delay    = 4.6;
  lifetime = 1.4;
  fadeInTime  = 0.5;
  xfmModifiers[0] = GBoF_FireBall_Offset_XM;
  xfmModifiers[1] = GBoF_FireBallLight_TLK_1_path_XM;
};

datablock afxPathData(GBoF_FireBallLight_TLK_2_Path)
{
  points = " 0    0    0"   SPC
           " 0.4  0.7 -0.3" SPC
           "-0.3  0.0  0.4" SPC
           " 0.2  0.4 -0.8" SPC
           "-0.4 -0.8  0.5" SPC
           " 0    0    0";
  lifetime = 0.20;
  loop = cycle;
  mult = 0.65;
};
//
datablock afxXM_PathConformData(GBoF_FireBallLight_TLK_2_path_XM)
{
  paths = "GBoF_FireBallLight_TLK_2_Path";
};

%GBoF_FireBallLight_2_intensity = ($isTGEA) ? 3.0 : 5.0;
datablock afxLightData(GBoF_FireBallLight_2_CE)
{
  type = "Point";  
  radius = 5;
  sgCastsShadows = true;

  sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Inverse Square Fast Falloff";
  color = 1.0*%GBoF_FireBallLight_2_intensity SPC
          1.0*%GBoF_FireBallLight_2_intensity SPC
          1.0*%GBoF_FireBallLight_2_intensity;
};
     
datablock afxEffectWrapperData(GBoF_FireBallLight_2_EW)
{
  effect = GBoF_FireBallLight_2_CE;
  posConstraint = "caster";  
  delay    = 4.6;
  lifetime = 1.4;
  fadeInTime  = 0.5;
  xfmModifiers[0] = GBoF_FireBall_Offset_XM;
  xfmModifiers[1] = GBoF_FireBallLight_TLK_2_path_XM;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SPELLCASTER ANIMATION

//
// This clip selects the main animation sequence for casting this
// spell. The spellcaster moves something like a weight lifter to
// concentrate the magic required to summon a very heavy fireball.
//

datablock afxAnimClipData(GBoF_FlameCast_Clip_CE)
{
  clipName = "gbof";
  rate = 1.0;
};
//
datablock afxEffectWrapperData(GBoF_FlameCast_Clip_EW)
{
  effect = GBoF_FlameCast_Clip_CE;
  constraint = "caster";
  lifetime = 7;
  propagateTimeFactor = true;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// explosions

datablock ParticleData(GBoF_ExplosionFire_P)
{
   // TGE textureName          = %mySpellDataPath @ "/GBoF/particles/fireExplosion";
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

   textureName          = %mySpellDataPath @ "/GBoF/particles/gbof_tiled_parts"; // fireExplosion
   textureCoords[0]     = "0.5 0.0";
   textureCoords[1]     = "0.5 0.5";
   textureCoords[2]     = "1.0 0.5";
   textureCoords[3]     = "1.0 0.0";
};
//
datablock ParticleEmitterData(GBoF_ExplosionFire2_E)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;
   ejectionVelocity = 0.8;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles = "GBoF_ExplosionFire_P";
};

datablock ParticleData(GBoF_ExplosionSmoke_P)
{
   // TGE textureName          = %mySpellDataPath @ "/GBoF/particles/smoke";
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

   textureName          = %mySpellDataPath @ "/GBoF/particles/gbof_tiled_parts"; // smoke
   textureCoords[0]     = "0.0  0.75";
   textureCoords[1]     = "0.0  1.0";
   textureCoords[2]     = "0.25 1.0";
   textureCoords[3]     = "0.25 0.75";
};
//
datablock ParticleEmitterData(GBoF_ExplosionSmoke_E)
{
   ejectionPeriodMS = 5;
   periodVarianceMS = 0;
   ejectionVelocity = 10;
   velocityVariance = 1.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles        = "GBoF_ExplosionSmoke_P";
};

datablock ParticleData(GBoF_ExplosionBigSmoke_P)
{
   // TGE textureName          = %mySpellDataPath @ "/GBoF/particles/smoke";
   dragCoeffiecient     = 100.0;
   gravityCoefficient   = 0;
   inheritedVelFactor   = 0.25;
   constantAcceleration = -0.30;
   lifetimeMS           = 3000;
   lifetimeVarianceMS   = 500;
   useInvAlpha =  true;
   spinRandomMin = -80.0;
   spinRandomMax =  80.0;

   colors[0]     = "1.0 1.0 1.0 1.0";
   colors[1]     = "1.0 0.5 0.0 0.9";
   colors[2]     = "0.4 0.4 0.4 0.6";
   colors[3]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 4.5;
   sizes[1]      = 7.0;
   sizes[2]      = 9.0;
   sizes[3]      = 12.0;

   times[0]      = 0.0;
   times[1]      = 0.33;
   times[2]      = 0.66;
   times[3]      = 1.0;

   textureName          = %mySpellDataPath @ "/GBoF/particles/gbof_tiled_parts"; // smoke
   textureCoords[0]     = "0.0  0.75";
   textureCoords[1]     = "0.0  1.0";
   textureCoords[2]     = "0.25 1.0";
   textureCoords[3]     = "0.25 0.75";
};
//
datablock ParticleEmitterData(GBoF_ExplosionBigSmoke_E)
{
   ejectionPeriodMS = 4;
   periodVarianceMS = 0;
   ejectionVelocity = 13;
   velocityVariance = 2.0;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles = "GBoF_ExplosionBigSmoke_P";
};

//----

datablock ParticleEmitterData(GBoF_ExplosionFire_E)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;
   ejectionVelocity = 0.8;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles        = "GBoF_ExplosionFire_P";
};
//
datablock ExplosionData(GBoF_Explosion_CE)
{
   lifeTimeMS = 1200;

   // Volume particles
   particleEmitter = GBoF_ExplosionFire_E;
   particleDensity = 20; //50;
   particleRadius = 3;

   // Point emission
   emitter[0] = GBoF_ExplosionSmoke_E;
   emitter[1] = GBoF_ExplosionSmoke_E;

   // Impulse
   impulseRadius = 10;
   impulseForce = 15;
};

datablock ExplosionData(GBoF_Explosion2_CE)
{
   lifeTimeMS = 1200;

   // Volume particles
   particleEmitter = GBoF_ExplosionFire2_E;
   particleDensity = 20; //50;
   particleRadius = 3;

   // Point emission
   emitter[0] = GBoF_ExplosionSmoke_E;
   emitter[1] = GBoF_ExplosionSmoke_E;

   // Impulse
   impulseRadius = 10;
   impulseForce = 15;
};

datablock ExplosionData(GBoF_ExplosionBig_CE)
{
   lifeTimeMS = 1200;

   // Volume particles
   particleEmitter = GBoF_ExplosionFire2_E;
   particleDensity = 80;
   particleRadius = 7;

   // Point emission
   emitter[0] = GBoF_ExplosionBigSmoke_E;
   emitter[1] = GBoF_ExplosionBigSmoke_E;
   emitter[2] = GBoF_ExplosionBigSmoke_E;

  
   // Impulse
   impulseRadius = 10;
   impulseForce = 15;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// ON TARGET IMPACT

//
// When the intended target is hit, a three stage explosion occurs,
// First a couple quick explosions, followed by a larger explosion.
//

// explosions

//
// A three-stage explosion... boom, boom, ba-boom!
//

datablock afxEffectWrapperData(GBoF_Explosion1_EW)
{
  effect = GBoF_Explosion_CE;
  constraint = "impactPoint";
  execConditions[0] = $AFX::IMPACTED_SOMETHING;
};

datablock afxEffectWrapperData(GBoF_Explosion2_EW)
{
  effect = GBoF_Explosion2_CE;
  delay = 0.4;
  constraint = "impactedObject.Eye";
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};

datablock afxEffectWrapperData(GBoF_Explosion3_EW)
{
  effect = GBoF_ExplosionBig_CE;
  delay = 1.2;
  constraint = "impactedObject.Bip01 Spine1";
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};

// shockwaves

//
// Three fast moving shockwaves that follow the three
// explosions. All very similar, but the third has a 
// little more density, since the third explosion is
// biggest.
//

datablock afxZodiacData(GBoF_ImpactZodeFast_CE)
{  
  texture = %mySpellDataPath @ "/GBoF/zodiacs/zode_impactA";
  radius = 1.0;
  startAngle = 0.0;
  rotationRate = 0.0;  
  color = "1.0 1.0 1.0 0.35";
  blend = additive;
  growthRate = 300.0;
  showOnInteriors = false;
};
//
datablock afxEffectWrapperData(GBoF_ImpactZodeFast1_EW)
{
  effect = GBoF_ImpactZodeFast_CE;
  posConstraint = "impactedObject";
  delay = 0.05;
  fadeInTime = 0.0;
  fadeOutTime = 1.0;
  lifetime = 0.5;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};
//
datablock afxEffectWrapperData(GBoF_ImpactZodeFast2_EW)
{
  effect = GBoF_ImpactZodeFast_CE;
  posConstraint = "impactedObject";
  delay = 0.45;
  fadeInTime = 0.0;
  fadeOutTime = 1.0;
  lifetime = 0.5;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};

datablock afxZodiacData(GBoF_ImpactZodeFast3_CE)
{  
  texture = %mySpellDataPath @ "/GBoF/zodiacs/zode_impactA";
  radius = 1.0;
  startAngle = 0.0;
  rotationRate = 0.0;
  color = "1.0 1.0 1.0 0.6";
  blend = additive;
  growthRate = 300.0;
  showOnInteriors = false;
};
//
datablock afxEffectWrapperData(GBoF_ImpactZodeFast3_EW)
{
  effect = GBoF_ImpactZodeFast3_CE;
  posConstraint = "impactedObject";
  delay = 1.25;
  fadeInTime = 0.0;
  fadeOutTime = 1.0;
  lifetime = 0.5;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};

//
// A slow moving white cloud shockwave formed from two
// similar zodiacs that rotate in opposite directions.
//

datablock afxZodiacData(GBoF_EtherealImpactZode1_CE)
{  
  texture = %mySpellDataPath @ "/GBoF/zodiacs/GBofF_impactB-1";
  radius = 1.0;
  startAngle = 0.0;
  rotationRate = 40.0;
  color = "1.0 1.0 1.0 0.5";
  blend = additive;
  growthRate = 30.0;
  showOnInteriors = false;
};
//
datablock afxEffectWrapperData(GBoF_EtherealImpactZode1_EW)
{
  effect = GBoF_EtherealImpactZode1_CE;
  posConstraint = "impactPoint";
  delay = 1.0;
  fadeInTime = 0.25;
  fadeOutTime = 3.25;
  lifetime = 1.75;
  execConditions[0] = $AFX::IMPACTED_TARGET;
};

datablock afxZodiacData(GBoF_EtherealImpactZode2_CE)
{  
  texture = %mySpellDataPath @ "/GBoF/zodiacs/GBofF_impactB-2";
  radius = 1.0;
  startAngle = 0.0;
  rotationRate = -40.0;
  //color = "1.0 0.5 0.0 0.4";
  color = "1.0 1.0 1.0 0.5";
  blend = additive;
  growthRate = 30.0;
  showOnInteriors = false;
};
//
datablock afxEffectWrapperData(GBoF_EtherealImpactZode2_EW)
{
  effect = GBoF_EtherealImpactZode2_CE;
  posConstraint = "impactPoint";
  delay = 1.0;
  fadeInTime = 0.25;
  fadeOutTime = 3.25;
  lifetime = 1.75;
  execConditions[0] = $AFX::IMPACTED_TARGET;
};

// lights
/*
datablock afxLightData(GBoF_ImpactLight_CE)
{
  type = "Point";  
  radius = 100;
  color  = "0.5 0.2 0.0";

  // TGEA
  lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF;
};

datablock afxLightData(GBoF_BigImpactLight_CE)
{
  type = "Point";  
  radius = 200;
  color  = "0.5 0.2 0.0";

  // TGEA
  lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF;
};

datablock afxEffectWrapperData(GBoF_ImpactLight2_EW)
{
  effect = GBoF_ImpactLight_CE;
  posConstraint = "impactedObject";  
  delay    = 0.4;
  lifetime = 0.25;
  fadeInTime  = 0.25;
  fadeOutTime = 0.75;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};

datablock afxEffectWrapperData(GBoF_ImpactLight3_EW)
{
  effect = GBoF_BigImpactLight_CE;
  posConstraint = "impactedObject";  
  delay    = 1.2;
  lifetime = 0.5;
  fadeInTime  = 0.25;
  fadeOutTime = 1.5;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};

datablock afxEffectWrapperData(GBoF_ImpactLight1_EW)
{
  effect = GBoF_ImpactLight_CE;
  posConstraint = "impactPoint";  
  delay    = 0.0;
  lifetime = 0.25;
  fadeInTime  = 0.25;
  fadeOutTime = 0.75;
  execConditions[0] = $AFX::IMPACTED_SOMETHING;
};
*/

%GBoF_ImpactLight1_intensity = 10.0;
datablock sgLightObjectData(FireFX_ImpactLight1_flare_TLK_CE) 
{
  CastsShadows = false;
  Radius = 10;
  Brightness = %GBoF_ImpactLight1_intensity;
  Colour = "1.0 1.0 1.0";

  FlareOn = true;
  LinkFlare = true;
  //LinkFlareSize = true;
  FlareBitmap = "common/lighting/lightFalloffMono";
  NearSize = 20;
  FarSize  = 8;
  NearDistance = 2;
  FarDistance  = 50;
};
// flare line-of-sight...
datablock afxXM_LocalOffsetData(GBoF_ImpactFlare_offset_XM)
{
  localOffset = "0 0 1.5";
};
//
datablock afxEffectWrapperData(GBoF_ImpactLight1A_EW) // : GBoF_ImpactLight1_EW)
{
  effect = FireFX_ImpactLight1_flare_TLK_CE;
  posConstraint = "impactPoint";  
  delay    = 0.0;
  lifetime = 0.25;
  fadeInTime  = 0.25;
  fadeOutTime = 0.75;
  execConditions[0] = $AFX::IMPACTED_SOMETHING;
  xfmModifiers[0] = GBoF_ImpactFlare_offset_XM;
};

%GBoF_ImpactLight2_intensity = ($isTGEA) ? 1.0 : 5.0;
datablock sgLightObjectData(FireFX_ImpactLight2_flare_CE) 
{
  CastsShadows = false;
  Radius = 18;
  Brightness = %GBoF_ImpactLight2_intensity;
  Colour = ($isTGEA) ? "1.0 0.6 0.0" : "1.0 0.2 0.0";

  FlareOn = true;
  LinkFlare = true;
  //LinkFlareSize = true;
  FlareBitmap = "common/lighting/corona";
  NearSize = 20*1.5;
  FarSize  = 8*1.5;
  NearDistance = 2;
  FarDistance  = 50;
};
//
datablock afxEffectWrapperData(GBoF_ImpactLight1B_EW : GBoF_ImpactLight1A_EW) //GBoF_ImpactLight1_EW)
{
  effect = FireFX_ImpactLight2_flare_CE;
  delay  = 0.05;
  xfmModifiers[0] = GBoF_ImpactFlare_offset_XM;
};

datablock afxEffectWrapperData(GBoF_ImpactLight2A_EW : GBoF_ImpactLight1A_EW)
{
  delay = 0.4;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};
datablock afxEffectWrapperData(GBoF_ImpactLight2B_EW : GBoF_ImpactLight1B_EW)
{
  delay = 0.45;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};

datablock sgLightObjectData(FireFX_ImpactLightBig1_flare_TLK_CE : FireFX_ImpactLight1_flare_TLK_CE) 
{
  Radius = 10*2;
  NearSize = 20*2;
  FarSize  = 8*2;
};
datablock sgLightObjectData(FireFX_ImpactLightBig2_flare_TLK_CE : FireFX_ImpactLight2_flare_CE)
{
  Radius = 18*2;
  NearSize = 20*1.5*2;
  FarSize  = 8*1.5*2;
};

datablock afxEffectWrapperData(GBoF_ImpactLight3A_EW : GBoF_ImpactLight1A_EW)
{
  effect = FireFX_ImpactLightBig1_flare_TLK_CE;
  delay    = 1.2;
  lifetime = 0.5;
  fadeInTime  = 0.25;
  fadeOutTime = 1.5;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};
datablock afxEffectWrapperData(GBoF_ImpactLight3B_EW : GBoF_ImpactLight1B_EW)
{
  effect = FireFX_ImpactLightBig2_flare_TLK_CE;
  delay    = 1.3;
  lifetime = 0.5;
  fadeInTime  = 0.25;
  fadeOutTime = 1.5;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SCORCHED EARTH RESIDUE

//
// The scorched earth texture will stick around for 30 seconds
// and slowly fade off.
//

datablock afxZodiacData(GBoF_ScorchedEarth_CE)
{  
  texture = %mySpellDataPath @ "/GBoF/zodiacs/GBoF_blastimpact";
  radius = 40.0;
  startAngle = 0.0;
  rotationRate = 0.0;
  color = "1.0 1.0 1.0 1.0";
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(GBoF_ScorchedEarth_EW)
{
  effect = GBoF_ScorchedEarth_CE;
  constraint = "impactPoint";
  delay = 1.25;
  fadeInTime = 0.5;
  lifetime = 0.5;
  residueLifetime = 20;
  fadeOutTime = 5;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// CASTING SOUNDS

datablock AudioProfile(GBoF_ZodeSnd_CE)
{
   fileName = %mySpellDataPath @ "/GBoF/sounds/gbof_zodiac.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(GBoF_ZodeSnd_EW)
{
  effect = GBoF_ZodeSnd_CE;
  constraint = "caster";
  delay = 0;
  lifetime = 1.907;
};

datablock AudioProfile(GBoF_ConjureSnd_CE)
{
   fileName = %mySpellDataPath @ "/GBoF/sounds/gbof_conjure.ogg";
   description = SpellAudioCasting_AD;
   preload = false;
};
datablock afxEffectWrapperData(GBoF_ConjureSnd_EW)
{
  effect = GBoF_ConjureSnd_CE;
  constraint = "caster";
  delay = 0.5;
  lifetime = 4.159;
};

datablock AudioProfile(GBoF_Conjure2Snd_CE)
{
  fileName = %mySpellDataPath @ "/GBoF/sounds/gbof_conjure2.ogg";
  description = SpellAudioCasting_AD;
  preload = false;
};
datablock afxEffectWrapperData(GBoF_Conjure2Snd_EW)
{
  effect = GBoF_Conjure2Snd_CE;
  constraint = "caster";
  delay = 4.2;
  lifetime = 3.076;
};

datablock AudioProfile(GBoF_FireBallSnd_CE)
{
   fileName = %mySpellDataPath @ "/GBoF/sounds/projectile_loopFire1a_SR.ogg";
   description = SpellAudioMissileLoop_AD;
   preload = false;
};
datablock afxEffectWrapperData(GBoF_FireBallSnd_EW)
{
  effect = GBoF_FireBallSnd_CE;
  constraint = "caster";
  delay = 4.6;
  lifetime = 2.0;
  fadeoutTime = 0.5;
};

datablock AudioProfile(GBoF_ImpactSnd_CE)
{
   fileName = %mySpellDataPath @ "/GBoF/sounds/gbof_impact1.ogg";
   description = SpellAudioImpact_AD;
   preload = false;
};
datablock afxEffectWrapperData(GBoF_ImpactSnd_EW)
{
  effect = GBoF_ImpactSnd_CE;
  constraint = "impactPoint";
  delay = 0;
  lifetime = 7.271; 
  execConditions[0] = $AFX::IMPACTED_SOMETHING;
};

datablock AudioProfile(GBoF_Impact2Snd_CE)
{
   fileName = %mySpellDataPath @ "/GBoF/sounds/gbof_Impactextra.ogg";
   description = SpellAudioImpact_AD;
   preload = false;
};
datablock afxEffectWrapperData(GBoF_Impact2Snd_EW)
{
  effect = GBoF_Impact2Snd_CE;
  constraint = "impactPoint";
  delay = 0.4;
  lifetime = 2.288;
  execConditions[0] = $AFX::IMPACTED_PRIMARY;
};
datablock afxEffectWrapperData(GBoF_Impact3Snd_EW : GBoF_Impact2Snd_EW)
{
  delay = 1.2;
};


datablock afxXM_ShockwaveData(GBoF_Shockwave_XM)
{
  rate = 30.0;
  aimZOnly = true;
};
datablock afxXM_GroundConformData(GBoF_Shockwave_Ground_XM)
{
  height = 1.0;
  conformToInteriors = false;
};

datablock AudioProfile(GBoF_ShockwaveSnd_CE)
{
   fileName = %mySpellDataPath @ "/GBoF/sounds/shwave_loop_SR.ogg";
   description = SpellAudioShockwaveLoop_AD;
   preload = false;
};
datablock afxEffectWrapperData(GBoF_ShockwaveSnd_EW)
{
  effect = GBoF_ShockwaveSnd_CE;
  delay = 1.0;
  lifetime = 4.0;
  fadeInTime = 0.5;
  fadeOutTime = 1.0;

  // the following causes the shockwave sound to move
  // directly toward the listener while tracking the
  // slow white shockwave ring
  posConstraint = "impactedObject";
  posConstraint2 = "listener";
  xfmModifiers[0] = GBoF_Shockwave_XM;
  xfmModifiers[1] = GBoF_Shockwave_Ground_XM;
  execConditions[0] = $AFX::IMPACTED_TARGET;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// HUGE FIREBALL MISSILE

datablock afxMagicMissileData(GBoF_Fireball)
{
  particleEmitter       = GBoF_HugeFireBall_E;
  muzzleVelocity        = 30; //12;
  velInheritFactor      = 0;
  lifetime              = 20000;
  isBallistic           = true;
  ballisticCoefficient  = 0.95;
  gravityMod            = 0.05;
  isGuided              = true;
  precision             = 30;
  trackDelay            = 7;

  hasLight    = false;

  sound = GBoF_FireBallSnd_CE;

  launchOffset = "0.0 0.0 3.4";
  echoLaunchOffset = false;
};

datablock afxEffectWrapperData(GBoF_FireBallMissileLight_1_EW)
{
  effect = GBoF_FireBallLight_1_CE;
  posConstraint = "missile";  
  xfmModifiers[0] = GBoF_FireBallLight_TLK_1_path_XM;
};

datablock afxEffectWrapperData(GBoF_FireBallMissileLight_2_EW)
{
  effect = GBoF_FireBallLight_2_CE;
  posConstraint = "missile";  
  xfmModifiers[0] = GBoF_FireBallLight_TLK_2_path_XM;
};


//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// GREAT BALL OF FIRE SPELL
//

datablock afxMagicSpellData(GreatBallSpell)
{
    // warmup //
  //castingDur = 5.85; //6.0; //1.2;
  castingDur = 5.7; //6.0; //1.2;

    // lingering //
  lingerDur = 6.0;

    // magic missile //
  missile = GBoF_Fireball;

    // casting zodiac //
  addCastingEffect = GBoF_Zode_Reveal_EW;
  addCastingEffect = GBoF_MainZodeRevealLight_1_EW;
  addCastingEffect = GBoF_MainZodeRevealLight_2_EW;
  addCastingEffect = GBoF_MainZodeRevealLight_3_EW;

  addCastingEffect = GBoF_Zode1_TLKunderglow_EW;
  addCastingEffect = GBoF_Zode1_EW;
  addCastingEffect = GBoF_Zode2_TLKunderglow_EW;
  addCastingEffect = GBoF_Zode2_EW;
  addCastingEffect = GBoF_Zode3_EW;
  addCastingEffect = $GBoF_CastingZodeLight;
  
    // portal beams //
  addCastingEffect = GBoF_portalBeam1_EW;
  addCastingEffect = GBoF_portalBeam2_EW;
  addCastingEffect = GBoF_portalBeam3_EW;
  addCastingEffect = GBoF_portalBeam4_EW;
  addCastingEffect = GBoF_portalBeam5_EW;

    // portal flames //
  addCastingEffect = GBoF_FirePortal1_EW;
  addCastingEffect = GBoF_FirePortal2_EW;
  addCastingEffect = GBoF_FirePortal3_EW;
  addCastingEffect = GBoF_FirePortal4_EW;
  addCastingEffect = GBoF_FirePortal5_EW;
  addCastingEffect = GBoF_FirePortalLight1_EW;
  addCastingEffect = GBoF_FirePortalLight2_EW;
  addCastingEffect = GBoF_FirePortalLight3_EW;
  addCastingEffect = GBoF_FirePortalLight4_EW;
  addCastingEffect = GBoF_FirePortalLight5_EW;
  
    // portal zodes //
  addCastingEffect = GBoF_ZodePortal1_EW;
  addCastingEffect = GBoF_ZodePortal2_EW;
  addCastingEffect = GBoF_ZodePortal3_EW;
  addCastingEffect = GBoF_ZodePortal4_EW;
  addCastingEffect = GBoF_ZodePortal5_EW;
    // huge fireball //
  addCastingEffect = GBoF_FireBall_Static_EW;  
  addCastingEffect = GBoF_FireBallLight_1_EW;
  addCastingEffect = GBoF_FireBallLight_2_EW;
    // spellcaster animation //
  addCastingEffect = GBoF_FlameCast_Clip_EW;
    // casting sounds //
  addCastingEffect = GBoF_ZodeSnd_EW;
  addCastingEffect = GBoF_ConjureSnd_EW;
  addCastingEffect = GBoF_Conjure2Snd_EW;
  //addCastingEffect = GBoF_FireBallSnd_EW;

  addDeliveryEffect = GBoF_FireBallMissileLight_1_EW;
  addDeliveryEffect = GBoF_FireBallMissileLight_2_EW;

    // on-target explosions //
  addImpactEffect = GBoF_Explosion1_EW;
  addImpactEffect = GBoF_Explosion2_EW;
  addImpactEffect = GBoF_Explosion3_EW;  
    // shockwaves //
  addImpactEffect = GBoF_EtherealImpactZode1_EW;
  addImpactEffect = GBoF_EtherealImpactZode2_EW;
  addImpactEffect = GBoF_ImpactZodeFast1_EW;
  addImpactEffect = GBoF_ImpactZodeFast2_EW;
  addImpactEffect = GBoF_ImpactZodeFast3_EW;
    // lights //
  addImpactEffect = GBoF_ImpactLight1A_EW;
  addImpactEffect = GBoF_ImpactLight1B_EW;
  addImpactEffect = GBoF_ImpactLight2A_EW;
  addImpactEffect = GBoF_ImpactLight2B_EW;
  addImpactEffect = GBoF_ImpactLight3A_EW;
  addImpactEffect = GBoF_ImpactLight3B_EW;
    // residue //
  addImpactEffect = GBoF_ScorchedEarth_EW;
    // sound //
  addImpactEffect = GBoF_ImpactSnd_EW;
  addImpactEffect = GBoF_Impact2Snd_EW;
  addImpactEffect = GBoF_Impact3Snd_EW;
  addImpactEffect = GBoF_ShockwaveSnd_EW;
};
//
datablock afxRPGMagicSpellData(GreatBallSpell_RPG)
{
  name = "Great Ball of Fire";
  desc = "Hurls a ball of FLAMING DEATH that only the most intrepid caster " @
         "dare summon! Goodness Gracious! " @
         "Does 50 damage plus 20 radius damage." @ 
         "\n" @
         "\nspell design: Matthew Durante" @
         "\nsound effects: Dave Schroeder" @ 
         "\nspell concept: Matthew Durante";
  sourcePack = "Core Tech";

  iconBitmap = %mySpellDataPath @ "/GBoF/icons/gbof";
  target = "enemy";
  range = 80;
  manaCost = 10;
  directDamage = 50.0;
  areaDamage = 20;
  areaDamageRadius = 25;
  areaDamageImpulse = 1000;
  castingDur = GreatBallSpell.castingDur;
};

// set a level of detail
function GreatBallSpell::onActivate(%this, %spell, %caster, %target)
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
  GreatBallSpell.scriptFile = $afxAutoloadScriptFile;
  GreatBallSpell.spellDataPath = %mySpellDataPath;
  if (isFunction(addDemoSpellbookSpell))
    addDemoSpellbookSpell(GreatBallSpell, GreatBallSpell_RPG);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
