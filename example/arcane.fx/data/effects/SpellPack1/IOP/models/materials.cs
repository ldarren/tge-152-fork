
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// INSECTOPLASM (Materials)
//    This file contains shader and material definitions compatible
//    with the TGEA engine. It should not be loaded when using TGE.
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

new Material(IOP_beamA_MTL)
{
   mapTo = "IOP_beamA.png";
   baseTex[0] = "IOP_beamA";
   emissive[0] = true;
   translucent = true;
   translucentBlendOp = AddAlpha;
};

new Material(IOP_beamB_MTL)
{
   mapTo = "IOP_beamB.png";
   baseTex[0] = "IOP_beamB";
   emissive[0] = true;
   translucent = true;
   translucentBlendOp = AddAlpha;
};

new Material(IOP_beamC_MTL)
{
   mapTo = "IOP_beamC.png";
   baseTex[0] = "IOP_beamC";
   emissive[0] = true;
   translucent = true;
   translucentBlendOp = AddAlpha;
};

new Material(IOP_beamRing_MTL)
{
   mapTo = "IOP_beamRing.png";
   baseTex[0] = "IOP_beamRing";
   emissive[0] = true;
   translucent = true;
   translucentBlendOp = AddAlpha;
};

new Material(IOP_bug_MTL)
{
   mapTo = "IOP_bug.png";
   baseTex[0] = "IOP_bug";
};

new Material(IOP_bugPulse_MTL)
{
   mapTo = "IOP_bugPulse.png";
   baseTex[0] = "IOP_bugPulse";
   emissive[0] = true;
   translucent = true;
   translucentBlendOp = AddAlpha;
};

new Material(IOP_bugPulseSparkle_MTL)
{
   mapTo = "IOP_bugPulseSparkle.png";
   baseTex[0] = "IOP_bugPulseSparkle";
   emissive[0] = true;
   translucent = true;
   translucentBlendOp = AddAlpha;
};

new Material(IOP_bugSymbolA_MTL)
{
   mapTo = "IOP_bugSymbolA.png";
   baseTex[0] = "IOP_bugSymbolA";
   emissive[0] = true;
   translucent = true;
   translucentBlendOp = AddAlpha;
};

new Material(IOP_bugSymbolB_MTL)
{
   mapTo = "IOP_bugSymbolB.png";
   baseTex[0] = "IOP_bugSymbolB";
   emissive[0] = true;
   translucent = true;
   translucentBlendOp = AddAlpha;
};

new CustomMaterial(IOP_insectSparkle_CMTL)
{
   mapTo = "IOP_insectSparkle.png";
   texture[0] = "IOP_insectSparkle";
   texture[1] = "$fog";
   translucent = true;
   blendOp = AddAlpha;
   shader = afxDiffFogEmiss_Add_Shader;
};

new CustomMaterial(IOP_insectHead_CMTL)
{
   mapTo = "IOP_insectHead.png";
   texture[0] = "IOP_insectHead";
   texture[1] = "$fog";
   shader = afxDiffFogShader;
   blendOp = LerpAlpha;
   translucentZWrite = true;
};

// TODO: the eye should have a cubemap
new CustomMaterial(IOP_insectEye_CMTL)
{
   mapTo = "IOP_insectEye";
   texture[0] = "IOP_insectHead";
   texture[1] = "$fog";
   blendOp = LerpAlpha;
   shader = afxDiffFogEmissShader;
};

new CustomMaterial(IOP_insectJaws_CMTL)
{
   mapTo = "IOP_insectJaws";
   texture[0] = "IOP_insectHead";
   texture[1] = "$fog";
   blendOp = LerpAlpha;
   shader = afxDiffFogEmissShader;
};

new CustomMaterial(IOP_insectSegment_CMTL)
{
   mapTo = "IOP_insectSegment.png";
   texture[0] = "IOP_insectSegment";
   texture[1] = "$fog";
   shader = afxDiffFogShader;
   blendOp = LerpAlpha;
   translucentZWrite = true;
};

new CustomMaterial(IOP_insectWings_CMTL)
{
   mapTo = "IOP_insectWings.png";
   texture[0] = "IOP_insectWings";
   texture[1] = "$fog";
   translucent = true;
   blendOp = AddAlpha;
   shader = afxDiffFogAlpha_Add_Shader;
};

new CustomMaterial(IOP_insectWings_transA_CMTL)
{
   mapTo = "IOP_insectWings_transA.png";
   texture[0] = "IOP_insectWings_transA";
   texture[1] = "$fog";
   translucent = true;
   blendOp = AddAlpha;
   shader = afxDiffFogAlpha_Add_Shader;
};

new CustomMaterial(IOP_insectWings_transB_CMTL)
{
   mapTo = "IOP_insectWings_transB.png";
   texture[0] = "IOP_insectWings_transB";
   texture[1] = "$fog";
   translucent = true;
   blendOp = AddAlpha;
   shader = afxDiffFogAlpha_Add_Shader;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
