
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// OCCAM'S Laser (Materials)
//    This file contains shader and material definitions compatible
//    with the TGEA engine. It should not be loaded when using TGE.
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// Drone

new CustomMaterial(OL_Drone_Lighting_fallback_CMTL) // v1.1
{
  texture[0] = "satellite";
  texture[1] = "$dynamiclight";
  texture[2] = "$fog";
  blendOp = AddAlpha;
  version = 1.1;
  shader = afxLightingFog_fallback_Shader;
};
new CustomMaterial(OL_Drone_Lighting_CMTL)
{
  texture[0] = "satellite";
  texture[1] = "$dynamiclight";
  texture[2] = "$fog";
  blendOp = AddAlpha;
  shader = afxLightingFogShader;
  version = 2.0;
  fallback = OL_Drone_Lighting_fallback_CMTL;
};

new CustomMaterial(OL_Drone_CMTL)
{
  mapTo = "satellite.png";
  texture[0] = "satellite";
  texture[1] = "$fog";
  shader = afxDiffFogShader;
  dynamicLightingMaterial = OL_Drone_Lighting_CMTL;
  blendOp = LerpAlpha;
  translucentZWrite = true;
};

new CustomMaterial(OL_DroneFlare1_CMTL)
{
  mapTo = "satellite_flare1";
  texture[0] = "satellite";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

new CustomMaterial(OL_DroneFlare2_CMTL : OL_DroneFlare1_CMTL)
{
  mapTo = "satellite_flare2";
};

// Laser Beams

new Material(OL_beamA_MTL)
{
  mapTo = "beamA.png";
  baseTex[0] = "beamA";
  emissive[0] = true;
  translucent = true;
  translucentBlendOp = AddAlpha;
};

new Material(OL_beamB_MTL)
{
  mapTo = "beamB.png";
  baseTex[0] = "beamB";
  emissive[0] = true;
  translucent = true;
  translucentBlendOp = AddAlpha;
};

new Material(OL_beamC_MTL)
{
  mapTo = "beamC.png";
  baseTex[0] = "beamC";
  emissive[0] = true;
  translucent = true;
  translucentBlendOp = AddAlpha;
};

// Flares

new Material(OL_beam_flare_head_MTL)
{
  mapTo = "beam_flare_head.png";
  baseTex[0] = "beam_flare_head";
  emissive[0] = true;
  translucent = true;
  translucentBlendOp = AddAlpha;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//





