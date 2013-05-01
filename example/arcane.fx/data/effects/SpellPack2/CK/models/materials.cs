
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// CHILL KILL (Materials)
//    This file contains shader and material definitions compatible
//    with the TGEA engine. It should not be loaded when using TGE.
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// Shaders

%shader_path = expandFilename("./shaders/");

new ShaderData(CK_iceShards_Shader)
{
  DXVertexShaderFile = %shader_path @ "CK_ice_V.hlsl";
  DXPixelShaderFile = %shader_path @ "CK_ice_P.hlsl";
  pixVersion = 2.0;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//
// Materials and CustomMaterials

new CustomMaterial(CK_SnowBall_CMTL)
{
  mapTo = "CK_SnowBall.png";
  texture[0] = "CK_SnowBall";
  texture[1] = "$fog";
  shader = afxDiffFogShader;
  blendOp = LerpAlpha;
  translucentZWrite = true;
};

new CustomMaterial(CK_IceCrystalsGlint_CMTL)
{
  mapTo = "CK_IceCrystalsGlint";
  texture[0] = "CK_IceCrystalsA";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

new CustomMaterial(CK_IceCrystalsA_CMTL)
{
  mapTo = "CK_IceCrystalsA.png";
  texture[0] = "$backbuff";
  texture[1] = "CK_IceCrystalsB_alpha";
  texture[2] = "CK_IceCrystals_Bump";
  shader = CK_iceShards_Shader;
  refract = true;
  blendOp = LerpAlpha;
  translucentZWrite = true;
  specular[0] = "1 1 1 1";
  specularPower[0] = 16.0;
  version = 2.0;
};

new CustomMaterial(CK_IceCrystalsB_CMTL)
{
  mapTo = "CK_IceCrystalsB.png";
  texture[0] = "$backbuff";
  texture[1] = "CK_IceCrystalsB_alpha";
  texture[2] = "CK_IceCrystals_Bump";
  shader = CK_iceShards_Shader;
  refract = true;
  blendOp = LerpAlpha;
  translucentZWrite = true;
  specular[0] = "1 1 1 1";
  specularPower[0] = 16.0;
  version = 2.0;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//


