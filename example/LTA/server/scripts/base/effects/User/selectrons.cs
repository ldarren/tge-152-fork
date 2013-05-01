// ============================================================
// Project            :  VR20
// File               :  .\LTA\server\scripts\base\effects\User\selectrons.cs
// Copyright          :  
// Author             :  Darren
// Created on         :  Thursday, September 20, 2007 10:22 AM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

// style numbers
$Marker_Style = 20;

%mySelectronDataPath = $afxSpellDataPath @ "/" @ $afxAutoloadScriptFolder @ "/SELE";

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

$AdvancedLighting = ($pref::AFX::advancedFXLighting && !afxLegacyLighting());

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Marker Selectron Style

datablock afxModelData(Marker_Sele_CE)
{
  shapeFile = %mySelectronDataPath @ "/marker/RED_ARROW_arrow.dts";
  forceOnMaterialFlags = $MaterialFlags::SelfIlluminating;
  useVertexAlpha = true;
};

datablock afxXM_LocalOffsetData(Marker_Sele_offset_XM)
{
  localOffset = "0 0 4";
};
datablock afxXM_SpinData(Marker_Sele_spin_XM)
{
  spinAxis  = "0 0 1";
  spinAngle = 0.0;
  spinRate  = 120;//60;
};

datablock afxPathData(Marker_Sele_Path)
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
datablock afxXM_PathConformData(Marker_Sele_path_XM)
{
  paths = Marker_Sele_Path;
};

datablock afxEffectWrapperData(Marker_Sele_EW)
{
  effect = Marker_Sele_CE;
  posConstraint = selected;

  //lifetime = 2.0;
  fadeInTime = 0.20;
  fadeOutTime = 0.20;
  //delay = 0;

  xfmModifiers[0] = Marker_Sele_path_XM;
  xfmModifiers[1] = Marker_Sele_offset_XM;
  xfmModifiers[2] = Marker_Sele_spin_XM;
};

datablock afxZodiacData(Marker_Sele_Zode_CE)
{  
  texture = %mySelectronDataPath @ "/marker/RED_ARROW_Zode_C.png";
  radius = 1.0;
  //growInTime = 0.7;
  //startAngle = 180.0;
  rotationRate = (360.0/2.5)*2.0*2.0;
  color = "1.0 1.0 1.0 0.9";
  blend = additive;
  interiorHorizontalOnly = true;
};
//
datablock afxEffectWrapperData(Marker_Sele_Zode_EW)
{
  effect = Marker_Sele_Zode_CE;
  posConstraint = selected;

  //lifetime = 2.0;
  fadeInTime = 0.20;
  fadeOutTime = 0.20;
  //delay = 0;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// Marker Selectron
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

datablock afxSelectronData(Marker_SELE)
{
  selectionTypeMask = $TypeMasks::StaticObjectType; // destination type, default terrain and interior
  selectionTypeStyle = $Marker_Style;

  mainDur = 2;

  addMainEffect = Marker_Sele_EW;
  addMainEffect = Marker_Sele_Zode_EW;
};
