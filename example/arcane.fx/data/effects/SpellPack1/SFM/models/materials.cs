
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// SUMMON FECKLESS MOTH (Materials)
//    This file contains shader and material definitions compatible
//    with the TGEA engine. It should not be loaded when using TGE.
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// CATERPILLAR

new Material(SFM_caterpillar_head_MTL)
{
  mapTo = "SFM_caterpillar_head.png";
  baseTex[0] = "SFM_caterpillar_head";
};

new Material(SFM_caterpillar_headseg_MTL)
{
  mapTo = "SFM_caterpillar_headsegment.png";
  baseTex[0] = "SFM_caterpillar_headsegment";
};

new Material(SFM_caterpillar_hair_MTL)
{
  mapTo = "SFM_caterpillar_hair.png";
  baseTex[0] = "SFM_caterpillar_hair";
};

new Material(SFM_caterpillar_seg_MTL)
{
  mapTo = "SFM_caterpillar_segment.png";
  baseTex[0] = "SFM_caterpillar_segment";
};

// TINY MOTHS

new Material(SFM_tmoth_body_MTL)
{
  mapTo = "SFM_tmoth_body.png";
  baseTex[0] = "SFM_tmoth_body";
};

new Material(SFM_tmoth_body_blue_MTL)
{
  mapTo = "SFM_tmoth_body_blue.png";
  baseTex[0] = "SFM_tmoth_body_blue";
};

new Material(SFM_tmoth_body_green_MTL)
{
  mapTo = "SFM_tmoth_body_green.png";
  baseTex[0] = "SFM_tmoth_body_green";
};

new Material(SFM_tmoth_body_magenta_MTL)
{
  mapTo = "SFM_tmoth_body_magenta.png";
  baseTex[0] = "SFM_tmoth_body_magenta";
};

new Material(SFM_tmoth_body_orange_MTL)
{
  mapTo = "SFM_tmoth_body_orange.png";
  baseTex[0] = "SFM_tmoth_body_orange";
};

new Material(SFM_tmoth_wings_MTL)
{
  mapTo = "SFM_tmoth_wings.png";
  baseTex[0] = "SFM_tmoth_wings";
};

new Material(SFM_tmoth_wings_blue_MTL)
{
  mapTo = "SFM_tmoth_wings_blue.png";
  baseTex[0] = "SFM_tmoth_wings_blue";
};

new Material(SFM_tmoth_wings_green_MTL)
{
  mapTo = "SFM_tmoth_wings_green.png";
  baseTex[0] = "SFM_tmoth_wings_green";
};

new Material(SFM_tmoth_wings_magenta_MTL)
{
  mapTo = "SFM_tmoth_wings_magenta.png";
  baseTex[0] = "SFM_tmoth_wings_magenta";
};

new Material(SFM_tmoth_wings_orange_MTL)
{
  mapTo = "SFM_tmoth_wings_orange.png";
  baseTex[0] = "SFM_tmoth_wings_orange";
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// COCOON

new CustomMaterial(SFM_cocoon_top_CMTL)
{
  mapTo = "SFM_cocoon_top.png";
  texture[0] = "SFM_cocoon_top";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

new CustomMaterial(SFM_cocoon_bot_CMTL)
{
  mapTo = "SFM_cocoon_bot.png";
  texture[0] = "SFM_cocoon_bot";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

// LARVA

new CustomMaterial(SFM_larvaA2_CMTL)
{
  mapTo = "SFM_larvaA2.png";
  texture[0] = "SFM_larvaA2";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

new CustomMaterial(SFM_larvaA_CMTL)
{
  mapTo = "SFM_larvaA.png";
  texture[0] = "SFM_larvaA";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

new CustomMaterial(SFM_larvaB2_CMTL)
{
  mapTo = "SFM_larvaB2.png";
  texture[0] = "SFM_larvaB2";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

new CustomMaterial(SFM_larvaB_CMTL)
{
  mapTo = "SFM_larvaB.png";
  texture[0] = "SFM_larvaB";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

new CustomMaterial(SFM_larvaC_CMTL)
{
  mapTo = "SFM_larvaC.png";
  texture[0] = "SFM_larvaC";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

// BIG MOTH

new CustomMaterial(SFM_mothbody_CMTL)
{
  mapTo = "SFM_mothbody.png";
  texture[0] = "SFM_mothbody";
  texture[1] = "$fog";
  shader = afxDiffFogShader;
  blendOp = LerpAlpha;
  translucentZWrite = true;
};

new CustomMaterial(SFM_mothhead_CMTL)
{
  mapTo = "SFM_mothhead.png";
  texture[0] = "SFM_mothhead";
  texture[1] = "$fog";
  shader = afxDiffFogShader;
  blendOp = LerpAlpha;
  translucentZWrite = true;
};

new CustomMaterial(SFM_mothlegs_CMTL)
{
  mapTo = "SFM_mothlegs.png";
  texture[0] = "SFM_mothlegs";
  texture[1] = "$fog";
  shader = afxDiffFogShader;
  blendOp = LerpAlpha;
  translucentZWrite = true;
};

new CustomMaterial(SFM_mothwing_lower_CMTL)
{
  mapTo = "SFM_mothwing_lower.png";
  texture[0] = "SFM_mothwing_lower";
  texture[1] = "$fog";
  shader = afxDiffFogShader;
  blendOp = LerpAlpha;
  translucentZWrite = true;
};

new CustomMaterial(SFM_mothwing_upper_CMTL)
{
  mapTo = "SFM_mothwing_upper.png";
  texture[0] = "SFM_mothwing_upper";
  texture[1] = "$fog";
  shader = afxDiffFogShader;
  blendOp = LerpAlpha;
  translucentZWrite = true;
};

new CustomMaterial(SFM_mothwing_blur_CMTL)
{
  mapTo = "SFM_mothwing_blur.png";
  texture[0] = "SFM_mothwing_blur";
  texture[1] = "$fog";
  translucent = true;
  shader = afxDiffFogAlphaShader;
  blendOp = LerpAlpha;
};

new CustomMaterial(SFM_mothwing_lower_trans1_CMTL)
{
  mapTo = "SFM_mothwing_lower_trans1.png";
  texture[0] = "SFM_mothwing_lower_trans1";
  texture[1] = "$fog";
  translucent = true;
  shader = afxDiffFogAlphaShader;
  blendOp = LerpAlpha;
};

new CustomMaterial(SFM_mothwing_lower_trans2_CMTL)
{
  mapTo = "SFM_mothwing_lower_trans2.png";
  texture[0] = "SFM_mothwing_lower_trans2";
  texture[1] = "$fog";
  translucent = true;
  shader = afxDiffFogAlphaShader;
  blendOp = LerpAlpha;
};

new CustomMaterial(SFM_mothwing_upper_trans1_CMTL)
{
  mapTo = "SFM_mothwing_upper_trans1.png";
  texture[0] = "SFM_mothwing_upper_trans1";
  texture[1] = "$fog";
  translucent = true;
  shader = afxDiffFogAlphaShader;
  blendOp = LerpAlpha;
};

new CustomMaterial(SFM_mothwing_upper_trans2_CMTL)
{
  mapTo = "SFM_mothwing_upper_trans2.png";
  texture[0] = "SFM_mothwing_upper_trans2";
  texture[1] = "$fog";
  translucent = true;
  shader = afxDiffFogAlphaShader;
  blendOp = LerpAlpha;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

