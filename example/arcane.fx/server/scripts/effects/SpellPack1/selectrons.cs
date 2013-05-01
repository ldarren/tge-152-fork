
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// SELECTRONS (Spell Pack 1)
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
$Red_Arrow_Style = 10;
$SciFiDeco_Style = 11;

%mySelectronDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder @ "/SELE";

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// RED ARROW Selectron Style

datablock afxModelData(RedArrow_Sele_CE)
{
  shapeFile = %mySelectronDataPath @ "/red_arrow/RED_ARROW_arrow.dts";
  forceOnMaterialFlags = $MaterialFlags::SelfIlluminating;
  useVertexAlpha = true; // TGE (ignored by TGEA)
};

datablock afxXM_LocalOffsetData(RedArrow_Sele_offset_XM)
{
  localOffset = "0 0 4";
};
datablock afxXM_SpinData(RedArrow_Sele_spin_XM)
{
  spinAxis  = "0 0 1";
  spinAngle = 0.0;
  spinRate  = 120;//60;
};

datablock afxPathData(RedArrow_Sele_Path)
{
  points = "0 0  1.0" SPC
           "0 0 -1.0" SPC
           "0 0  1.0" SPC
           "0 0 -1.0" SPC
           "0 0  1.0";

  lifetime = 2.5;
  loop = cycle;

  mult = 0.7;
};
//
datablock afxXM_PathConformData(RedArrow_Sele_path_XM)
{
  paths = "RedArrow_Sele_Path";
};

datablock afxEffectWrapperData(RedArrow_Sele_EW)
{
  effect = RedArrow_Sele_CE;
  posConstraint = selected;

  //lifetime = 2.0;
  fadeInTime = 0.20;
  fadeOutTime = 0.20;
  //delay = 0;

  xfmModifiers[0] = RedArrow_Sele_path_XM;
  xfmModifiers[1] = RedArrow_Sele_offset_XM;
  xfmModifiers[2] = RedArrow_Sele_spin_XM;
};

datablock afxZodiacData(RedArrow_Sele_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/red_arrow/RED_ARROW_Zode_C.png";
  radius = 3.0;
  //growInTime = 0.7;
  //startAngle = 180.0;
  rotationRate = (360.0/2.5)*2.0*2.0;
  color = "1.0 1.0 1.0 0.9";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(RedArrow_Sele_Zode_EW)
{
  effect = RedArrow_Sele_Zode_CE;
  posConstraint = selected;

  //lifetime = 2.0;
  fadeInTime = 0.20;
  fadeOutTime = 0.20;
  //delay = 0;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// RED ARROW Selectron
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

datablock afxSelectronData(RedArrow_SELE)
{
  selectionTypeMask = $TypeMasks::PlayerObjectType | $TypeMasks::CorpseObjectType;
  selectionTypeStyle = $Red_Arrow_Style;

  mainDur = $AFX::INFINITE_TIME;

  addMainEffect = RedArrow_Sele_EW;
  addMainEffect = RedArrow_Sele_Zode_EW;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// SCI-FI Selectron Style A

datablock afxZodiacData(SciFiDeco_Sele_1_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/scifi/SCIFI_SELE_Zode_A1.png";
  radius = 2.0;
  startAngle = 0.0;
  rotationRate = 270.0;
  color = "1.0 1.0 1.0 1.0";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(SciFiDeco_Sele_1_Zode_EW)
{
  effect = SciFiDeco_Sele_1_Zode_CE;
  posConstraint = selected;

  //lifetime = 2.0;
  fadeInTime = 0.20;
  fadeOutTime = 0.20;
  delay = 0;
};

datablock afxZodiacData(SciFiDeco_Sele_2_Zode_CE : SciFiDeco_Sele_1_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/scifi/SCIFI_SELE_Zode_A2.png";
  //startAngle = 0.0;
  rotationRate = -120.0;
  //color = "1.0 1.0 1.0 1.0";
};
//
datablock afxEffectWrapperData(SciFiDeco_Sele_2_Zode_EW : SciFiDeco_Sele_1_Zode_EW)
{
  effect = SciFiDeco_Sele_2_Zode_CE;

  //lifetime = 2.0;
  fadeInTime = 0.20;
  fadeOutTime = 0.20;
  delay = 0.15;
};

datablock afxZodiacData(SciFiDeco_Sele_3_Zode_CE : SciFiDeco_Sele_1_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/scifi/SCIFI_SELE_Zode_A3.png";
  //startAngle = 0.0;
  rotationRate = 120.0;
  //color = "1.0 1.0 1.0 1.0";
};
//
datablock afxEffectWrapperData(SciFiDeco_Sele_3_Zode_EW : SciFiDeco_Sele_1_Zode_EW)
{
  effect = SciFiDeco_Sele_3_Zode_CE;

  //lifetime = 2.0;
  fadeInTime = 0.20;
  fadeOutTime = 0.20;
  delay = 0.3;
};

datablock afxZodiacData(SciFiDeco_Sele_4_Zode_CE : SciFiDeco_Sele_1_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/scifi/SCIFI_SELE_Zode_A4.png";
  rotationRate = 0;
  color = "0.5 0.5 0.5 0.5";
};
//
datablock afxEffectWrapperData(SciFiDeco_Sele_4_Zode_EW : SciFiDeco_Sele_1_Zode_EW)
{
  effect = SciFiDeco_Sele_4_Zode_CE;

  lifetime = 0.7;
  fadeInTime = 0.5;
  fadeOutTime = 0.20;
  delay = 0.0;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// SCI-FI DECO Selectron
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

datablock afxSelectronData(SciFiDeco_SELE)
{
  selectionTypeMask = $TypeMasks::PlayerObjectType | $TypeMasks::CorpseObjectType;
  selectionTypeStyle = $SciFiDeco_Style;

  mainDur = $AFX::INFINITE_TIME;

  addMainEffect = SciFiDeco_Sele_1_Zode_EW;
  addMainEffect = SciFiDeco_Sele_3_Zode_EW;
  addMainEffect = SciFiDeco_Sele_2_Zode_EW;
  addMainEffect = SciFiDeco_Sele_4_Zode_EW;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// Add styles to the demo's selectron manager. (This is only
// needed to allow selectron cycling using the 't' key.)
// 
addDemoSelectronStyle("RED ARROW",    $Red_Arrow_Style);
addDemoSelectronStyle("SCI-FI DECO",  $SciFiDeco_Style);

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
