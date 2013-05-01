
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// SPACE HELMET SCI-FI (Materials)
//    This file contains shader and material definitions compatible
//    with the TGEA engine. It should not be loaded when using TGE.
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

new CustomMaterial(SFH_helmet_lighting_CMTL)
{
  texture[0] = "helmet";
  texture[1] = "$dynamiclight";
  texture[2] = "$fog";
  shader = afxLightingFogShader;
  blendOp = AddAlpha;
};

new CustomMaterial(SFH_helmet_CMTL)
{
  mapTo = "helmet.png";
  texture[0] = "helmet";
  texture[1] = "$fog";
  blendOp = LerpAlpha;
  shader = afxDiffFogShader;
  dynamicLightingMaterial = SFH_helmet_lighting_CMTL;
  translucentZWrite = true;
};

new CustomMaterial(SFH_flasher_CMTL)
{
  mapTo = "helmet_light";
  texture[0] = "helmet";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
