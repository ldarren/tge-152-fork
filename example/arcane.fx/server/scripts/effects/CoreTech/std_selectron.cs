
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// STANDARD SELECTRON (Core Tech)
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

// style numbers
$AFX_Default_Style = 0;

%mySelectronDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder @ "/SELE";

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

$isTGEA = (afxGetEngine() $= "TGEA");

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
  radius = 2.0;//1.5;
  startAngle = -60.0*0.5;
  rotationRate = 60.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorIgnoreVertical = true;
  verticalRange = "2.0 0.3";
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
datablock afxLightData(AFX_Default_RevealLight_CE) // AFX-SPOTLIGHT
{
  type = "Spot";
  radius = ($isTGEA) ? 8 : 4;    
  direction = "0 -0.313 0.95";
  sgCastsShadows = false;
  color = "2.5 2.5 2.5";
  lightIlluminationMask = $AFX::ILLUM_DTS | $AFX::ILLUM_DIF; // TGEA (ignored by TGE)
  sgLightingModelName = ($isTGEA) ? "Original Advanced" : "Near Linear";
};

datablock afxEffectWrapperData(AFX_Default_RevealLight_1_EW : AFX_Default_glow_Zode_EW)
{
  effect = AFX_Default_RevealLight_CE;
  xfmModifiers[0] = AFX_Default_RevealLight_spin1_XM;
  xfmModifiers[1] = AFX_Default_RevealLight_offset_XM;
};
datablock afxEffectWrapperData(AFX_Default_RevealLight_2_EW : AFX_Default_glow_Zode_EW)
{
  effect = AFX_Default_RevealLight_CE;
  xfmModifiers[0] = AFX_Default_RevealLight_spin2_XM;
  xfmModifiers[1] = AFX_Default_RevealLight_offset_XM;
};
datablock afxEffectWrapperData(AFX_Default_RevealLight_3_EW : AFX_Default_glow_Zode_EW)
{
  effect = AFX_Default_RevealLight_CE;
  xfmModifiers[0] = AFX_Default_RevealLight_spin3_XM;
  xfmModifiers[1] = AFX_Default_RevealLight_offset_XM;
};

// Main Zodiac A1 (clockwise)
datablock afxZodiacData(AFX_Default_Zode1_CE)
{  
  texture = %mySelectronDataPath @ "/afx_default/DEF_SELE_zode.png";
  radius = 2.0;//1.5;
  startAngle = 0.0;
  rotationRate = 60.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorIgnoreVertical = true;
  verticalRange = "2.0 0.3";
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
  addSelectEffect = AFX_Default_RevealLight_1_EW;
  addSelectEffect = AFX_Default_RevealLight_2_EW;
  addSelectEffect = AFX_Default_RevealLight_3_EW;

  addMainEffect = AFX_Default_Zode1_EW;
  addMainEffect = AFX_Default_Zode2_EW;

  // sounds
  addSelectEffect = AFX_Default_Select_Snd_EW;
  addDeselectEffect = AFX_Default_Deselect_Snd_Ew;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// Add styles to the demo's selectron manager. (This is only
// needed to allow selectron cycling using the 't' key.)
// 
addDemoSelectronStyle("AFX DEFAULT",  $AFX_Default_Style);

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
