
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// ASTRAL PASSPORT (Materials)
//    This file contains shader and material definitions compatible
//    with the TGEA engine. It should not be loaded when using TGE.
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

new CustomMaterial(AP_ring_CMTL)
{
  mapTo = "AP_ring.png";
  texture[0] = "AP_ring";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

new CustomMaterial(AP_beam_CMTL)
{
  mapTo = "AP_beam.png";
  texture[0] = "AP_beam";
  texture[1] = "$fog";
  translucent = true;
  blendOp = AddAlpha;
  shader = afxDiffFogEmiss_Add_Shader;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//


