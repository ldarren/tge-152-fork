
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// GREAT BALL OF FIRE (Materials)
//    This file contains shader and material definitions compatible
//    with the TGEA engine. It should not be loaded when using TGE.
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

new CustomMaterial(GBoF_PortalBeam_Lighting_fallback_CMTL) // v1.1
{
  texture[0] = "portalbeamcolor2";
  texture[1] = "$dynamiclight";
  texture[2] = "$fog";
  blendOp = AddAlpha;
  version = 1.1;
  shader = afxLightingFog_fallback_Shader;
};

new CustomMaterial(GBoF_PortalBeam_Lighting_CMTL)
{
  texture[0] = "portalbeamcolor2";
  texture[1] = "$dynamiclight";
  texture[2] = "$fog";
  blendOp = AddAlpha;
  shader = afxLightingFogShader;
  version = 2.0;
  fallback = GBoF_PortalBeam_Lighting_fallback_CMTL;
};

new CustomMaterial(GBoF_PortalBeam_CMTL)
{
  mapTo = "GBoF_beam";
  texture[0] = "portalbeamcolor2";
  texture[1] = "$fog";
  shader = afxDiffFogShader;
  dynamicLightingMaterial = GBoF_PortalBeam_Lighting_CMTL;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//



