
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// ARCANE IN THE MEMBRANE (Materials)
//    This file contains shader and material definitions compatible
//    with the TGEA engine. It should not be loaded when using TGE.
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//


new CustomMaterial(AitM_casterEye_CMTL)
{
  mapTo = "AitM_casterEye.png";
  texture[0] = "AitM_casterEye";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

new CustomMaterial(AitM_casterHead_CMTL)
{
  mapTo = "AitM_casterHead.png";
  texture[0] = "AitM_casterHead";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

new CustomMaterial(AitM_casterTeeth_CMTL)
{
  mapTo = "AitM_casterTeeth.png";
  texture[0] = "AitM_casterTeeth";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

new CustomMaterial(AitM_crazyMaskA_CMTL)
{
  mapTo = "AitM_crazyMaskA.png";
  texture[0] = "AitM_crazyMaskA";
  texture[1] = "$fog";
  blendOp = lerpAlpha;
  shader = afxDiffFogEmissShader;
};

new CustomMaterial(AitM_crazyMaskB_CMTL : AitM_crazyMaskA_CMTL)
{
  mapTo = "AitM_crazyMaskB.png";
  texture[0] = "AitM_crazyMaskB";
};

new CustomMaterial(AitM_crazyMaskEyes_CMTL)
{
  mapTo = "AitM_MaskEyes";
  texture[0] = "AitM_casterEye";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
