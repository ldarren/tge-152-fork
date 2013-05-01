
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// THOR'S HAMMER (Materials)
//    This file contains shader and material definitions compatible
//    with the TGEA engine. It should not be loaded when using TGE.
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

new CustomMaterial(TH_auraA_CMTL)
{
  mapTo = "TH_auraA.png";
  texture[0] = "TH_auraA";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

new CustomMaterial(TH_auraB_CMTL : TH_auraA_CMTL)
{
  mapTo = "TH_auraB.png";
  texture[0] = "TH_auraB";
};

new CustomMaterial(TH_casterGlowA_CMTL)
{
  mapTo = "TH_casterGlowA.png";
  texture[0] = "TH_casterGlowA";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

new CustomMaterial(TH_casterGlowB_CMTL : TH_casterGlowA_CMTL)
{
  mapTo = "TH_casterGlowB.png";
  texture[0] = "TH_casterGlowB";
};

new CustomMaterial(TH_casterGlowC_CMTL : TH_casterGlowA_CMTL)
{
  mapTo = "TH_casterGlowC.png";
  texture[0] = "TH_casterGlowC";
};

new Material(TH_casterLightningA_MTL)
{
  mapTo = "TH_casterLightningA.png";
  baseTex[0] = "TH_casterLightningA";
  emissive[0] = true;
  translucent = true;
  translucentBlendOp = AddAlpha;
};

new Material(TH_casterLightningB_MTL)
{
  mapTo = "TH_casterLightningB.png";
  baseTex[0] = "TH_casterLightningB";
  emissive[0] = true;
  translucent = true;
  translucentBlendOp = AddAlpha;
};

new Material(TH_casterLightningC_MTL)
{
  mapTo = "TH_casterLightningC.png";
  baseTex[0] = "TH_casterLightningC";
  emissive[0] = true;
  translucent = true;
  translucentBlendOp = AddAlpha;
};

new CustomMaterial(TH_glowball_CMTL)
{
  mapTo = "TH_glowball.png";
  texture[0] = "TH_glowball";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

// For unknown reason the ends of the hammer head go away with a CustomShader.
new Material(TH_hammer_MTL)
{
  mapTo = "TH_hammer_A";
  baseTex[0] = "TH_hammer";
};

new CustomMaterial(TH_hammer_lighting_CMTL)
{
  texture[0] = "TH_hammer";
  texture[1] = "$dynamiclight";
  texture[2] = "$fog";
  blendOp = AddAlpha;
  shader = afxLightingFogShader;
};

new CustomMaterial(TH_hammer_B_CMTL)
{
  mapTo = "TH_hammer_B";
  texture[0] = "TH_hammer";
  texture[1] = "$fog";
  shader = afxDiffFogShader;
  dynamicLightingMaterial = TH_hammer_lighting_CMTL;
  blendOp = LerpAlpha;
  translucentZWrite = true;
};

new CustomMaterial(TH_hammer_C_CMTL : TH_hammer_B_CMTL)
{
  mapTo = "TH_hammer_C";
};

new CustomMaterial(TH_hammer_D_CMTL : TH_hammer_B_CMTL)
{
  mapTo = "TH_hammer_D";
};

new CustomMaterial(TH_hammer_E_CMTL : TH_hammer_B_CMTL)
{
  mapTo = "TH_hammer_E";
};

new CustomMaterial(TH_lightbeam_CMTL)
{
  mapTo = "TH_lightbeam.png";
  texture[0] = "TH_lightbeam";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//


