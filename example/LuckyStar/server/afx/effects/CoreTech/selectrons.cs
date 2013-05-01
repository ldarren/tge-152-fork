
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// SELECTRONS (Core Tech)
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// style numbers
$AFX_Default_Style = 0;
$Blue_Flower_Style = 1;
$Booming_Style = 2;

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
  radius = 0.5;//2.0;
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
    radius = 1.5; //5;
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
  radius = 1;//2.0;
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
    radius = 1.5; //5;
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
  radius = 1.5-0.5;
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
  radius = 1.0;
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
  radius = 1.5;//4.0;
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

%BlueFlower_main_radius3 = 1.0;
%BlueFlower_main_radius2 = 2.0;
%BlueFlower_main_radius1 = 2.5;

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

// Add styles to the demo's selectron manager. (This is only
// needed to allow selectron cycling using the 't' key.)
// 
addDemoSelectronStyle("AFX DEFAULT",  $AFX_Default_Style);
addDemoSelectronStyle("BLUE FLOWER",  $Blue_Flower_Style);
addDemoSelectronStyle("EXPLODING",    $Booming_Style);

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
