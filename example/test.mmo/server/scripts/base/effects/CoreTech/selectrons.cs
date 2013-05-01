
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// SELECTRONS (Core Tech)
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// style numbers
$AFX_Default_Style = 0;
$AFX_Old_Style = 1;
$Like_WoW_Style = 2;
$Blue_Flower_Style = 3;
$Booming_Style = 4;
$SciFi_Style = 5;

%mySelectronDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder @ "/SELE";

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

$AdvancedLighting = ($pref::AFX::advancedFXLighting && !afxLegacyLighting());

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// AFX DEFAULT Selectron Style
//
//    This is the default selectron used in the AFX demo.
//    It's a rippling yellow ring with a preliminary glow
//    plus sfx on selection and deselection.
//

// Initial Glow A1 (clockwise)
datablock afxZodiacData(AFX_Default_glow_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/afx_default/DEF_SELE_glow.png";
  radius = 1; // DARREN MOD: selectron ring radius
  startAngle = -60.0*0.5;
  rotationRate = 60.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
datablock afxEffectWrapperData(AFX_Default_glow_Zode_EW)
{
  effect = AFX_Default_glow_Zode_CE;
  posConstraint = selected;
  lifetime = 0.7;
  fadeInTime = 0.50;
  fadeOutTime = 0.20;
  delay = 0;
};

// Initial Glow A2 (counter-clockwise)
datablock afxZodiacData(AFX_Default_glow2_Zode_CE : AFX_Default_glow_Zode_CE)
{  
  startAngle = 60.0*0.5;
  rotationRate = -60.0;
};
datablock afxEffectWrapperData(AFX_Default_glow2_Zode_EW : AFX_Default_glow_Zode_EW)
{
  effect = AFX_Default_glow2_Zode_CE;
};

if ($AdvancedLighting)
{
  %AFX_Select_A_RevealLight_TLK_intensity = 5.0;

  datablock afxXM_LocalOffsetData(AFX_Default_RevealLight_offset_XM)
  {
    localOffset = "0 2 -4";
  };
  datablock afxXM_SpinData(AFX_Default_RevealLight_spin1_XM)
  {
    spinAxis = "0 0 1";
    spinAngle = 0;
    spinRate = -30;
  };
  datablock afxXM_SpinData(AFX_Default_RevealLight_spin2_XM : AFX_Default_RevealLight_spin1_XM)
  {
    spinAngle = 120;
  };
  datablock afxXM_SpinData(AFX_Default_RevealLight_spin3_XM : AFX_Default_RevealLight_spin1_XM)
  {
    spinAngle = 240;
  };

  // main zode reveal light
  datablock afxLightData(AFX_Default_RevealLight_TLK_CE)
  {
    type = "Spot";
    radius = 4; //5;
    direction = "0 -0.313 -0.95";
    sgCastsShadows = false;
    color = "2.5 2.5 2.5";
  };

  datablock afxEffectWrapperData(AFX_Default_RevealLight_1_TLK_EW : AFX_Default_glow_Zode_EW)
  {
    effect = AFX_Default_RevealLight_TLK_CE;
    xfmModifiers[1] = AFX_Default_RevealLight_spin1_XM;
    xfmModifiers[2] = AFX_Default_RevealLight_offset_XM;
  };
  datablock afxEffectWrapperData(AFX_Default_RevealLight_2_TLK_EW : AFX_Default_glow_Zode_EW)
  {
    effect = AFX_Default_RevealLight_TLK_CE;
    xfmModifiers[1] = AFX_Default_RevealLight_spin2_XM;
    xfmModifiers[2] = AFX_Default_RevealLight_offset_XM;
  };
  datablock afxEffectWrapperData(AFX_Default_RevealLight_3_TLK_EW : AFX_Default_glow_Zode_EW)
  {
    effect = AFX_Default_RevealLight_TLK_CE;
    xfmModifiers[1] = AFX_Default_RevealLight_spin3_XM;
    xfmModifiers[2] = AFX_Default_RevealLight_offset_XM;
  };

  $AFX_Default_RevealLight_1_TLK = AFX_Default_RevealLight_1_TLK_EW;
  $AFX_Default_RevealLight_2_TLK = AFX_Default_RevealLight_2_TLK_EW;
  $AFX_Default_RevealLight_3_TLK = AFX_Default_RevealLight_3_TLK_EW;
}
else
{
  $AFX_Default_RevealLight_1_TLK = "";
  $AFX_Default_RevealLight_2_TLK = "";
  $AFX_Default_RevealLight_3_TLK = "";
}

// Main Zodiac A1 (clockwise)
datablock afxZodiacData(AFX_Default_Zode1_CE)
{  
  texture = %mySelectronDataPath @ "/afx_default/DEF_SELE_zode.png";
  radius = 1; // DARREN Seletron ring radius
  startAngle = 0.0;
  rotationRate = 60.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
datablock afxEffectWrapperData(AFX_Default_Zode1_EW)
{
  effect = AFX_Default_Zode1_CE;
  posConstraint = selected;
  fadeInTime = 0.20;
  fadeOutTime = 0.20;
  delay = 0.5;
};

// Main Zodiac A2 (counter-clockwise)
datablock afxZodiacData(AFX_Default_Zode2_CE : AFX_Default_Zode1_CE)
{  
  rotationRate = -60.0;
};
datablock afxEffectWrapperData(AFX_Default_Zode2_EW : AFX_Default_Zode1_EW)
{
  effect = AFX_Default_Zode2_CE;
};

if ($AdvancedLighting)
{
  // this offset defines the lights distance beneath the terrain
  datablock afxXM_LocalOffsetData(AFX_Default_Light_offset_XM)
  {
    localOffset = "0 0 -2"; //-4";
  };

  datablock afxLightData(AFX_Default_Light_TLK_CE)
  {
    type = "Point";
    radius = 3; //1; //5;
    sgCastsShadows = false;
    sgLightingModelName = "SG - Inverse Square (Lighting Pack)";
    color = "2.5 2.5 2.5";
  };
  //
  datablock afxEffectWrapperData(AFX_Default_Light_TLK_EW : AFX_Default_Zode1_EW)
  {
    effect = AFX_Default_Light_TLK_CE;
    xfmModifiers[0] = "AFX_Default_Light_offset_XM";
  };

  $AFX_Select_A_Light_TLK = AFX_Default_Light_TLK_EW;
}
else
{
  $AFX_Select_A_Light_TLK = "";
}

// Selectron Sounds

// Shared Audio Description
datablock AudioDescription(AFX_Default_Snd_AD)
{
  volume             = 1.0;
  isLooping          = false;
  is3D               = true;
  ReferenceDistance  = 50.0;
  MaxDistance        = 180.0;
  type               = $SimAudioType;
};

// Selection Sound
datablock AudioProfile(AFX_Default_Select_Snd_CE)
{
   fileName = %mySelectronDataPath @ "/afx_default/DEF_SELE_select_snd.ogg";
   description = AFX_Default_Snd_AD;
   preload = false;
};
datablock afxEffectWrapperData(AFX_Default_Select_Snd_EW)
{
  effect = AFX_Default_Select_Snd_CE;
  constraint = "selected";
  lifetime = 1.483;
  scaleFactor = 0.5;
};

// Deselection Sound
datablock AudioProfile(AFX_Default_Deselect_Snd_CE)
{
   fileName = %mySelectronDataPath @ "/afx_default/DEF_SELE_deselect_snd.ogg";
   description = AFX_Default_Snd_AD;
   preload = false;
};
datablock afxEffectWrapperData(AFX_Default_Deselect_Snd_Ew)
{
  effect = AFX_Default_Deselect_Snd_CE;
  constraint = "selected";
  lifetime = 0.269;
  scaleFactor = 0.5;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// AFX DEFAULT Selectron 
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

datablock afxSelectronData(AFX_Default_SELE)
{
  selectionTypeStyle = $AFX_Default_Style;
  selectionTypeMask = $TypeMasks::PlayerObjectType | $TypeMasks::CorpseObjectType;

  mainDur = $AFX::INFINITE_TIME;

  addSelectEffect = AFX_Default_glow_Zode_EW;
  addSelectEffect = AFX_Default_glow2_Zode_EW;
  addSelectEffect = $AFX_Default_RevealLight_1_TLK;
  addSelectEffect = $AFX_Default_RevealLight_2_TLK;
  addSelectEffect = $AFX_Default_RevealLight_3_TLK;

  addMainEffect = AFX_Default_Zode1_EW;
  addMainEffect = AFX_Default_Zode2_EW;

  // sounds
  addSelectEffect = AFX_Default_Select_Snd_EW;
  addDeselectEffect = AFX_Default_Deselect_Snd_Ew;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// AFX OLD Selectron Style
//
//    This selectron style reproduces the plain red zodiac used for
//    AFX selections before selectrons.
//

datablock afxZodiacData(AFX_Old_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/afx_old/OLD_SELE_ring.png";
  radius = 1.5;
  startAngle = 0.0;
  rotationRate = 0.0;
  color = "1.0 0.0 0.0 0.5";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(AFX_Old_Zode_EW)
{
  effect = AFX_Old_Zode_CE;
  posConstraint = selected;
  fadeInTime = 0.20;
  fadeOutTime = 0.20;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// AFX OLD Selectron
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

datablock afxSelectronData(AFX_OldDefault_SELE)
{
  selectionTypeStyle = $AFX_Old_Style;
  selectionTypeMask = $TypeMasks::PlayerObjectType | $TypeMasks::CorpseObjectType;
  mainDur = $AFX::INFINITE_TIME;
  addMainEffect = AFX_Old_Zode_EW;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// LIKE WOW Selectron Style
//
//    This selectron style reproduces some of the behavior of the 
//    selections found in World of Warcraft. While visually, they
//    only approximate the ones in WoW, they do implement type
//    specific coloring and continuous orientation of the zodiac
//    relative to the camera position.
//

//$AIObjectType_mask = 0x2000000; // BIT(25);

// used to point wide part of crescent toward camera
datablock afxXM_AimData(Like_WoW_aim_XM)
{
  aimZOnly = true;
};

// LIKE WOW Red (for NPCs)

datablock afxZodiacData(Like_WoW_RED_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/like_wow/WOW_SELE_crescent.png";
  radius = 1.5;
  startAngle = 180.0;
  rotationRate = 0.0;
  color = "1.0 0.0 0.0 0.5";
  blend = additive;
  interiorHorizontalOnly = true;
  trackOrientConstraint = true;
};
datablock afxEffectWrapperData(Like_WoW_RED_Zode_EW)
{
  effect = Like_WoW_RED_Zode_CE;
  constraint = selected;
  fadeInTime = 0.15;
  fadeOutTime = 0.15;
  posConstraint2 = camera; // aim
  xfmModifiers[0] = Like_WoW_aim_XM;
};


// LIKE WOW White (for corpses)

datablock afxZodiacData(Like_WoW_WHITE_Zode_CE : Like_WoW_RED_Zode_CE)
{  
  color = "1.0 1.0 1.0 0.4";
};
datablock afxEffectWrapperData(Like_WoW_WHITE_Zode_EW : Like_WoW_RED_Zode_EW)
{
  effect = Like_WoW_WHITE_Zode_CE;
};
//


// LIKE WOW Purple (for players)
datablock afxZodiacData(Like_WoW_PURPLE_Zode_CE : Like_WoW_RED_Zode_CE)
{  
  color = "0.57 0.15 0.56 0.5";
};
datablock afxEffectWrapperData(Like_WoW_PURPLE_Zode_EW : Like_WoW_RED_Zode_EW)
{
  effect = Like_WoW_PURPLE_Zode_CE;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// The LIKE WOW Selectrons
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

// Ordering is important here... try to match corpses first since corpses also
// match AIs. Then try AIs since they will also match Players. Try Players last
// after corpse and AI tests have failed. 
//
datablock afxSelectronData(Like_WoW_WHITE_SELE)
{
  selectionTypeStyle = $Like_WoW_Style;

  // match corpses only
  selectionTypeMask = $TypeMasks::CorpseObjectType;

  mainDur = $AFX::INFINITE_TIME;
  addMainEffect = Like_WoW_WHITE_Zode_EW;
};

datablock afxSelectronData(Like_WoW_RED_SELE)
{
  selectionTypeStyle = $Like_WoW_Style;

  // match living (or undead) NPCs only
  selectionTypeMask = $TypeMasks::AIObjectType; //$AIObjectType_mask;

  mainDur = $AFX::INFINITE_TIME;
  addMainEffect = Like_WoW_RED_Zode_EW;
};

datablock afxSelectronData(Like_WoW_PURPLE_SELE)
{
  selectionTypeStyle = $Like_WoW_Style;

  // match Players only (not NPCs)
  selectionTypeMask = $TypeMasks::PlayerObjectType;

  mainDur = $AFX::INFINITE_TIME;
  addMainEffect = Like_WoW_PURPLE_Zode_EW;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// EXPLODING Selectron Style

datablock afxZodiacData(Booming_Stain_CE)
{  
  texture = %mySelectronDataPath @ "/exploding/EXP_SELE_stain.png";
  radius = 3.5;
  startAngle = 0.0;
  rotationRate = 0.0;
  color = "1.0 1.0 1.0 0.9";
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(Booming_Stain_EW)
{
  effect = Booming_Stain_CE;
  constraint = "selected";
  fadeInTime = 1.0;
  fadeOutTime = 1.0;
};

datablock ParticleData(Booming_ExplosionSmoke_P)
{
   textureName          = %mySelectronDataPath @ "/exploding/smoke";
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
datablock ParticleData(Booming_ExplosionFire_P)
{
   textureName          = %mySelectronDataPath @ "/exploding/fireExplosion";
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
datablock ParticleEmitterData(Booming_ExplosionFire_E)
{
   ejectionPeriodMS = 10;
   periodVarianceMS = 0;
   ejectionVelocity = 0.8;
   velocityVariance = 0.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles        = "Booming_ExplosionFire_P";
};
//
datablock ParticleEmitterData(Booming_ExplosionSmoke_E)
{
   ejectionPeriodMS = 5;
   periodVarianceMS = 0;
   ejectionVelocity = 10;
   velocityVariance = 1.5;
   thetaMin         = 0.0;
   thetaMax         = 180.0;
   lifetimeMS       = 250;
   particles        = "Booming_ExplosionSmoke_P";
};
//
datablock ExplosionData(Booming_Explosion_CE)
{
   lifeTimeMS = 1200;

   // Volume particles
   particleEmitter = Booming_ExplosionFire_E;
   particleDensity = 20; //50;
   particleRadius = 3;

   // Point emission
   emitter[0] = Booming_ExplosionSmoke_E;
   emitter[1] = Booming_ExplosionSmoke_E;

   // Impulse
   impulseRadius = 10;
   impulseForce = 15;
};
//
datablock afxEffectWrapperData(Booming_Explosion_EW)
{
  effect = Booming_Explosion_CE;
  posConstraint = "selected";
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// BOOMING Selectron
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

datablock afxSelectronData(Booming_SELE)
{
  selectionTypeStyle = $Booming_Style;
  selectionTypeMask = $TypeMasks::PlayerObjectType | $TypeMasks::CorpseObjectType;

  mainDur = $AFX::INFINITE_TIME;
  addMainEffect = Booming_Stain_EW;
  addSelectEffect = Booming_Explosion_EW;
  addDeselectEffect = Booming_Explosion_EW;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// BLUE FLOWER Selectron Style

datablock afxZodiacData(BlueFlower_glow_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/blue_flower/BLUE_FLOWER_glow.png";
  radius = 4.0-0.5;
  growthRate = 0.75;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(BlueFlower_glow_Zode_EW)
{
  effect = BlueFlower_glow_Zode_CE;
  posConstraint = selected;

  lifetime = 2.0;
  fadeInTime = 1.5; //0.50;
  fadeOutTime = 0.20;
  delay = 0.25;
};

datablock afxZodiacData(BlueFlower_glowring1_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/blue_flower/BLUE_FLOWER_glowring.png";
  radius = 2.0;
  startAngle = 0;
  rotationRate = 81.0;
  color = "1.0 1.0 1.0 1.0";
  //color = "0.7 0.7 0.7 0.7";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(BlueFlower_glowring1_Zode_EW)
{
  effect = BlueFlower_glowring1_Zode_CE;
  posConstraint = selected;

  lifetime = 2.0+0.35;
  fadeInTime = 1.0; //0.50;
  fadeOutTime = 0.20;
  delay = 0;
};

datablock afxZodiacData(BlueFlower_glowring2_Zode_CE : BlueFlower_glowring1_Zode_CE)
{  
  startAngle = 180;
  rotationRate = -123; //-90.0;
};
//
datablock afxEffectWrapperData(BlueFlower_glowring2_Zode_EW : BlueFlower_glowring1_Zode_EW)
{
  effect = BlueFlower_glowring2_Zode_CE;
};

datablock afxZodiacData(BlueFlower_rays1_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/blue_flower/BLUE_FLOWER_rays.png";
  radius = 3.0;//4.0;
  startAngle = 0;
  rotationRate = 60.0; //90.0;
  color = "0.5 0.5 0.5 0.5";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(BlueFlower_rays1_Zode_EW)
{
  effect = BlueFlower_rays1_Zode_CE;
  posConstraint = selected;

  lifetime = 2.0;
  fadeInTime = 1.0; //0.50;
  fadeOutTime = 0.4;
  delay = 0;
};

datablock afxZodiacData(BlueFlower_rays2_Zode_CE : BlueFlower_rays1_Zode_CE)
{  
  startAngle = 180;
  rotationRate = -60.0; //-90.0;
};
//
datablock afxEffectWrapperData(BlueFlower_rays2_Zode_EW : BlueFlower_rays1_Zode_EW)
{
  effect = BlueFlower_rays2_Zode_CE;
};

%BlueFlower_main_radius3 = 2.4;
%BlueFlower_main_radius2 = 3.2;
%BlueFlower_main_radius1 = 4.0;

datablock afxZodiacData(BlueFlower_main_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/blue_flower/BLUE_FLOWER_zode.png";
  radius = %BlueFlower_main_radius3;
  startAngle = 0;
  rotationRate = 180.0;
  //color = "1.0 1.0 1.0 1.0";
  color = "0.9 0.9 0.9 0.9";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(BlueFlower_main_Zode_EW)
{
  effect = BlueFlower_main_Zode_CE;
  posConstraint = selected;

  //lifetime = 2.0;
  fadeInTime = 0.2;
  fadeOutTime = 0.3;
  delay = 2.6; //2.2;//2.35;
};

datablock afxZodiacData(BlueFlower_mainGlow_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/blue_flower/BLUE_FLOWER_zodeGlow.png";
  radius = 1.8; //2.4;
  startAngle = 0;
  rotationRate = 180.0;
  //growInTime = 0.5;
  growthRate = 8.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(BlueFlower_mainGlow_Zode_EW)
{
  effect = BlueFlower_mainGlow_Zode_CE;
  posConstraint = selected;

  lifetime = 0.15; //0.05;
  fadeInTime = 0.05;
  fadeOutTime = 0.2; //0.2;
  delay = 2.05;
};

datablock afxZodiacData(BlueFlower_mainGlow1_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/blue_flower/BLUE_FLOWER_zodeGlow.png";
  radius = %BlueFlower_main_radius1; //5.5;
  startAngle = 0;
  rotationRate = 0.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(BlueFlower_mainGlow1_Zode_EW)
{
  effect = BlueFlower_mainGlow1_Zode_CE;
  posConstraint = selected;

  lifetime = 0.2;
  fadeInTime = 0.3; //0.5;
  fadeOutTime = 0.25;
  delay = 2.1;
};

datablock afxZodiacData(BlueFlower_mainGlow2_Zode_CE : BlueFlower_mainGlow1_Zode_CE)
{
  radius = %BlueFlower_main_radius2; //3.5;
  rotationRate = 60.0;
};
//
datablock afxEffectWrapperData(BlueFlower_mainGlow2_Zode_EW : BlueFlower_mainGlow1_Zode_EW)
{
  effect = BlueFlower_mainGlow2_Zode_CE;
  delay = 2.3;
};

datablock afxZodiacData(BlueFlower_mainGlow3_Zode_CE : BlueFlower_mainGlow1_Zode_CE)
{
  radius = %BlueFlower_main_radius3; //2.4;
  rotationRate = 120.0;
};
//
datablock afxEffectWrapperData(BlueFlower_mainGlow3_Zode_EW : BlueFlower_mainGlow1_Zode_EW)
{
  effect = BlueFlower_mainGlow3_Zode_CE;
  delay = 2.5;
};


datablock afxZodiacData(BlueFlower_main1_Zode_CE : BlueFlower_mainGlow1_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/blue_flower/BLUE_FLOWER_zode.png";
  color = "0.9 0.9 0.9 0.9";
};
//
datablock afxEffectWrapperData(BlueFlower_main1_Zode_EW)
{
  effect = BlueFlower_main1_Zode_CE;
  posConstraint = selected;

  lifetime = 0.3;
  fadeInTime = 0.2;
  fadeOutTime = 0.3;
  delay = 2.1+0.2;
};

datablock afxZodiacData(BlueFlower_main2_Zode_CE : BlueFlower_mainGlow2_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/blue_flower/BLUE_FLOWER_zode.png";
  color = "0.9 0.9 0.9 0.9";
};
//
datablock afxEffectWrapperData(BlueFlower_main2_Zode_EW)
{
  effect = BlueFlower_main2_Zode_CE;
  posConstraint = selected;

  lifetime = 0.3;
  fadeInTime = 0.2;
  fadeOutTime = 0.3;
  delay = 2.3+0.2;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// BLUE FLOWER Selectron
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

datablock afxSelectronData(BlueFlower_SELE)
{
  selectionTypeMask = $TypeMasks::PlayerObjectType | $TypeMasks::CorpseObjectType;
  selectionTypeStyle = $Blue_Flower_Style;

  mainDur = $AFX::INFINITE_TIME;

  addSelectEffect = BlueFlower_glow_Zode_EW;
  addSelectEffect = BlueFlower_rays1_Zode_EW;
  addSelectEffect = BlueFlower_rays2_Zode_EW;
  addSelectEffect = BlueFlower_glowring1_Zode_EW;
  addSelectEffect = BlueFlower_glowring2_Zode_EW;
  addSelectEffect = BlueFlower_mainGlow1_Zode_EW;
  addSelectEffect = BlueFlower_mainGlow2_Zode_EW;
  addSelectEffect = BlueFlower_mainGlow3_Zode_EW;
  addSelectEffect = BlueFlower_main1_Zode_EW;
  addSelectEffect = BlueFlower_main2_Zode_EW;
  
  addMainEffect = BlueFlower_main_Zode_EW;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// SCI-FI Selectron

datablock afxXM_ScaleData(SCIFI_SelectTarget_scale_XM)
{
  scale = "1.5";

  delay = 0.0;
  lifetime = 0.0;
  fadeOutTime = 0.5;
};

datablock afxZodiacData(SCIFI_SelectTarget_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/scifi/SCIFI_SELE_Zode_B1.png";
  radius = 1.8;
  startAngle = 0.0;
  rotationRate = 0.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(SCIFI_SelectTarget_Zode_EW)
{
  effect = SCIFI_SelectTarget_Zode_CE;
  posConstraint = selected;

  lifetime = 0.5;
  fadeInTime = 0.40;
  fadeOutTime = 0;
  delay = 0;

  xfmModifiers[0] = SCIFI_SelectTarget_scale_XM;
};

datablock afxZodiacData(SCIFI_SelectTargetSpin_Zode_CE : SCIFI_SelectTarget_Zode_CE)
{  
  startAngle = 0.0;
  rotationRate = 180.0;
};
//
datablock afxEffectWrapperData(SCIFI_SelectTargetSpin_Zode_EW)
{
  effect = SCIFI_SelectTargetSpin_Zode_CE;
  posConstraint = selected;

  //lifetime = 0.5;
  fadeInTime = 0;
  fadeOutTime = 0.20;
  delay = 0.5;
};

datablock afxXM_AimData(SCIFI_SelectPointer_aim_XM)
{
  aimZOnly = true;
};

datablock afxZodiacData(SCIFI_SelectPointer1_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/scifi/SCIFI_SELE_Zode_B2.png";
  radius = 6.0*0.5;
  startAngle = 180.0;
  rotationRate = 0.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
  trackOrientConstraint = true;
};
//
datablock afxEffectWrapperData(SCIFI_SelectPointer1_Zode_EW)
{
  effect = SCIFI_SelectPointer1_Zode_CE;
  constraint = selected;

  fadeInTime = 0.1;
  fadeOutTime = 0.2;
  delay = 1.0;

  posConstraint2 = camera; // aim
  xfmModifiers[0] = SCIFI_SelectPointer_aim_XM;
};

datablock afxZodiacData(SCIFI_SelectPointer2_Zode_CE : SCIFI_SelectPointer1_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/scifi/SCIFI_SELE_Zode_B3.png";
  radius = 6.0;
};
//
datablock afxEffectWrapperData(SCIFI_SelectPointer2_Zode_EW : SCIFI_SelectPointer1_Zode_EW)
{
  effect = SCIFI_SelectPointer2_Zode_CE;
  delay = 1.2;
};

datablock afxZodiacData(SCIFI_SelectPointer3_Zode_CE : SCIFI_SelectPointer1_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/scifi/SCIFI_SELE_Zode_B4.png";
  radius = 6.0;
};
//
datablock afxEffectWrapperData(SCIFI_SelectPointer3_Zode_EW : SCIFI_SelectPointer1_Zode_EW)
{
  effect = SCIFI_SelectPointer3_Zode_CE;
  delay = 1.4;
};

datablock afxZodiacData(SCIFI_SelectPointer4A_Zode_CE : SCIFI_SelectPointer1_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/scifi/SCIFI_SELE_Zode_B5.png";
  radius = 6.0;
};
//
datablock afxEffectWrapperData(SCIFI_SelectPointer4A_Zode_EW : SCIFI_SelectPointer1_Zode_EW)
{
  effect = SCIFI_SelectPointer4A_Zode_CE;
  delay = 1.6;
  fadeOutTime = 0.1;
  lifetime = 0.4;
};


datablock afxEffectWrapperData(SCIFI_SelectPointer4B_Zode_EW : SCIFI_SelectPointer4A_Zode_EW)
{
  delay = 2.2;
};

datablock afxEffectWrapperData(SCIFI_SelectPointer4C_Zode_EW : SCIFI_SelectPointer4A_Zode_EW)
{
  delay = 2.8;
  lifetime = $AFX::INFINITE_TIME;
};

datablock afxZodiacData(SCIFI_SelectPointer5_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/scifi/SCIFI_SELE_Zode_B6.png";
  radius = 6.0*0.5;
  startAngle = 180.0;
  rotationRate = -120.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
  trackOrientConstraint = true;
};
//
datablock afxEffectWrapperData(SCIFI_SelectPointer5_Zode_EW)
{
  effect = SCIFI_SelectPointer5_Zode_CE;
  constraint = selected;

  fadeInTime = 0.1;
  fadeOutTime = 0.2;
  delay = 1.4;

  posConstraint2 = camera; // aim
  xfmModifiers[0] = SCIFI_SelectPointer_aim_XM;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SCI-FI Selectron
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

datablock afxSelectronData(SCIFI_SELE)
{
  selectionTypeMask = $TypeMasks::PlayerObjectType | $TypeMasks::CorpseObjectType;
  selectionTypeStyle = $SciFi_Style;

  mainDur = $AFX::INFINITE_TIME;

  addMainEffect = SCIFI_SelectTarget_Zode_EW;
  addMainEffect = SCIFI_SelectTargetSpin_Zode_EW;
  addMainEffect = SCIFI_SelectPointer1_Zode_EW;
  addMainEffect = SCIFI_SelectPointer2_Zode_EW;
  addMainEffect = SCIFI_SelectPointer3_Zode_EW;
  addMainEffect = SCIFI_SelectPointer4A_Zode_EW;
  addMainEffect = SCIFI_SelectPointer4B_Zode_EW;
  addMainEffect = SCIFI_SelectPointer4C_Zode_EW;
  addMainEffect = SCIFI_SelectPointer5_Zode_EW;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// Add styles to the demo's selectron manager. (This is only
// needed to allow selectron cycling using the 't' key.)
// 
addDemoSelectronStyle("AFX DEFAULT",  $AFX_Default_Style);
addDemoSelectronStyle("AFX OLD",      $AFX_Old_Style);
addDemoSelectronStyle("LIKE WOW",     $Like_WoW_Style);
addDemoSelectronStyle("BLUE FLOWER",  $Blue_Flower_Style);
addDemoSelectronStyle("EXPLODING",    $Booming_Style);
addDemoSelectronStyle("SCI-FI",       $SciFi_Style);

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
