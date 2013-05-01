
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// SPELLPACK1 SELECTRONS (Materials)
//    This file contains shader and material definitions compatible
//    with the TGEA engine. It should not be loaded when using TGE.
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

new CustomMaterial(SELE_red_arrow_CMTL)
{
  mapTo = "afx_selectionArrow.png";
  texture[0] = "afx_selectionArrow";
  texture[1] = "$fog";
  blendOp = LerpAlpha;
  shader = afxDiffFogEmissShader;
  translucentZWrite = true;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//





