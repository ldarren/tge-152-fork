//new SubShader(InteriorParallax14)
//{
//   VertexFile[0] = "Shaders/interiorParallax/interiorParallaxVert";
//   PixelFile[0] = "Shaders/interiorParallax/interiorParallaxPixl";
//   ShaderQuality[0] = 200;
//   ShaderType = $ShaderTypes::ATITextShaderType;
//};

new SubShader(Test20)
{
   VertexFile[0] = "Shaders/UnitTests/basicTestV.glsl";
   PixelFile[0] = "Shaders/UnitTests/basicTestP.glsl";
   ShaderQuality[0] = 0;
};

new Material()
{
   mapTo = "unittest";
   Shader20 = Test20;
   textures[0] = "Lightmap";
   textures[1] = "Fog";
};

new SubShader(InteriorParallaxDetail20)
{
   VertexFile[0] = "Shaders/interiorParallax/interiorPDV.glsl";
   PixelFile[0] = "Shaders/interiorParallax/interiorPDP.glsl";
   ShaderQuality[0] = 300;
   
   VertexFile[1] = "Shaders/interiorParallax/interiorFFV.glsl";
   PixelFile[1] = "Shaders/interiorParallax/interiorFFP.glsl";
   ShaderQuality[1] = 0;
   
   VertexFile[2] = "Shaders/interiorParallax/interiorParallaxVert.glsl";
   PixelFile[2] = "Shaders/interiorParallax/interiorParallaxPixl.glsl";
   ShaderQuality[2] = 200;
   
   VertexFile[3] = "Shaders/interiorBump/interiorBumpVert.glsl";
   PixelFile[3] = "Shaders/interiorBump/interiorBumpPixl.glsl";
   ShaderQuality[3] = 100;
   
   ShaderType = $ShaderTypes::GLSLShaderType;
};

new SubShader(InteriorFF20)
{
   VertexFile[0] = "Shaders/interiorParallax/interiorFFV.glsl";
   PixelFile[0] = "Shaders/interiorParallax/interiorFFP.glsl";
   ShaderQuality[0] = 0;
};

new SubShader(InteriorParallax20)
{
   VertexFile[0] = "Shaders/interiorParallax/interiorParallaxVert.glsl";
   PixelFile[0] = "Shaders/interiorParallax/interiorParallaxPixl.glsl";
   ShaderQuality[0] = 200;
   
   VertexFile[1] = "Shaders/interiorBump/interiorBumpVert.glsl";
   PixelFile[1] = "Shaders/interiorBump/interiorBumpPixl.glsl";
   ShaderQuality[1] = 100;
   
   VertexFile[2] = "Shaders/interiorParallax/interiorFFV.glsl";
   PixelFile[2] = "Shaders/interiorParallax/interiorFFP.glsl";
   ShaderQuality[2] = 0;
   
   ShaderType = $ShaderTypes::GLSLShaderType;
};

//new SubShader(InteriorParallax20a)
//{
//   VertexFile[0] = "Shaders/interiorParallax/interiorParallaxVert";
//   PixelFile[0] = "Shaders/interiorParallax/interiorParallaxPixl";
//   ShaderQuality[0] = 200;
   
//   ShaderType = $ShaderTypes::ASMShaderType;
//};

new Material()
{
   name = "InteriorParallax";
   
//   Shader14 = InteriorParallax14;
   Shader20 = InteriorParallax20;
//   Shader20a = InteriorParallax20a;
   
   samplers[0] = "LightMap";
   samplers[1] = "BaseTexture";
   samplers[2] = "NormalMap";
   samplers[3] = "FogTexture";
};

new SubShader(HeightmapBlur20)
{
   VertexFile[0] = "Shaders/nmapConversion/hmapBlurVert.glsl";
   PixelFile[0] = "Shaders/nmapConversion/hmapBlurPixl.glsl";
   ShaderQuality[0] = 0;
   
   ShaderType = $ShaderTypes::GLSLShaderType;
};

new Material()
{
   name = "heightmapCreator";
   
   Shader20 = HeightmapBlur20;
};

new SubShader(NormalMapGen20)
{
   VertexFile[0] = "Shaders/nmapConversion/nmapConversionVert.glsl";
   PixelFile[0] = "Shaders/nmapConversion/nmapConversionPixl.glsl";
   ShaderQuality[0] = 0;
   
   ShaderType = $ShaderTypes::GLSLShaderType;
};

new Material()
{
   name = "nmapCreator";
   
   Shader20 = NormalMapGen20;
};

new SubShader(waterRM20)
{
   VertexFile[0] = "Shaders/general/refractVert.glsl";
   PixelFile[0] = "Shaders/general/refractPixl.glsl";
   ShaderQuality[0] = 0;
   
   ShaderType = $ShaderTypes::GLSLShaderType;
};

new Material()
{
   name = "waterRefractionMask";
   
   Shader20 = waterRM20;
};

new SubShader(waterSub20)
{
   VertexFile[0] = "Shaders/waterSubmerged/waterSubmergedVert.glsl";
   PixelFile[0] = "Shaders/waterSubmerged/waterSubmergedPixl.glsl";
   ShaderQuality[0] = 0;
   
   ShaderType = $ShaderTypes::GLSLShaderType;
};

new Material()
{
   name = "waterSubmerged";
   
   Shader20 = waterSub20;
   samplers[1] = "NormalMap";
   samplers[2] = "DepthTexture";
   samplers[3] = "BaseTexture";
   samplers[4] = "RefractTexture";
};

new SubShader(waterRR20)
{
   VertexFile[0] = "Shaders/water/waterVert.glsl";
   PixelFile[0] = "Shaders/water/waterPixlLow.glsl";
   ShaderQuality[0] = 0;
   
   VertexFile[1] = "Shaders/water/waterVert.glsl";
   PixelFile[1] = "Shaders/water/waterPixlMed.glsl";
   ShaderQuality[1] = 150;
   
   VertexFile[2] = "Shaders/water/waterVert.glsl";
   PixelFile[2] = "Shaders/water/waterPixlHigh.glsl";
   ShaderQuality[2] = 300;
   
   // Not quite ready for primetime.
   //VertexFile[3] = "Shaders/water/waterVert.glsl";
   //PixelFile[3] = "Shaders/water/waterPixlVHigh.glsl";
   //ShaderQuality[3] = 500;
   
   ShaderType = $ShaderTypes::GLSLShaderType;
};

new Material()
{
   name = "waterReflectRefract";
   
   Shader20 = waterRR20;
   
   samplers[1] = "NormalMap";
   samplers[2] = "DepthTexture";
   samplers[3] = "BaseTexture";
   samplers[4] = "RefractTexture";
   samplers[5] = "ShoreTexture";
};

new SubShader(waterRO20)
{
   VertexFile[0] = "Shaders/waterReflectOnly/waterReflectOnlyVert.glsl";
   PixelFile[0] = "Shaders/waterReflectOnly/waterReflectOnlyPixlVLow.glsl";
   ShaderQuality[0] = 0;
   
   VertexFile[1] = "Shaders/waterReflectOnly/waterReflectOnlyVert.glsl";
   PixelFile[1] = "Shaders/waterReflectOnly/waterReflectOnlyPixlLow.glsl";
   ShaderQuality[1] = 50;
   
   VertexFile[2] = "Shaders/waterReflectOnly/waterReflectOnlyVert.glsl";
   PixelFile[2] = "Shaders/waterReflectOnly/waterReflectOnlyPixlMed.glsl";
   ShaderQuality[2] = 100;
   
   VertexFile[3] = "Shaders/waterReflectOnly/waterReflectOnlyVert.glsl";
   PixelFile[3] = "Shaders/waterReflectOnly/waterReflectOnlyPixlHigh.glsl";
   ShaderQuality[3] = 200;
   
   ShaderType = $ShaderTypes::GLSLShaderType;
};

new Material()
{
   name = "waterReflectOnly";
   
   Shader20 = waterRO20;
   samplers[1] = "NormalMap";
   samplers[2] = "DepthTexture";
   samplers[3] = "BaseTexture";
};

new SubShader(DRLBloomGen20)
{
   VertexFile[0] = "Shaders/DRL/bloomGenV.glsl";
   PixelFile[0] = "Shaders/DRL/bloomGenP.glsl";
   ShaderQuality[0] = 0;
   
   ShaderType = $ShaderTypes::GLSLShaderType;
};

new Material()
{
   name = "DRLBloomGen";
   
   Shader20 = DRLBloomGen20;
   samplers[0] = "BaseTexture";
   samplers[1] = "InfoTexture";
};

new SubShader(DRLBlur20)
{
   VertexFile[0] = "Shaders/DRL/blurV.glsl";
   PixelFile[0] = "Shaders/DRL/blurP.glsl";
   ShaderQuality[0] = 0;
   
   ShaderType = $ShaderTypes::GLSLShaderType;
};

new Material()
{
   name = "DRLBlur";
   
   Shader20 = DRLBlur20;
   samplers[0] = "BaseTexture";
};

new SubShader(DRLComposition20)
{
   VertexFile[0] = "Shaders/DRL/bloomGenV.glsl";
   PixelFile[0] = "Shaders/DRL/compositionP.glsl";
   ShaderQuality[0] = 0;
   
   ShaderType = $ShaderTypes::GLSLShaderType;
};

new Material()
{
   name = "DRLComposition";

   Shader20 = DRLComposition20;
   samplers[0] = "BaseTexture";
   samplers[1] = "BloomTexture";
   samplers[2] = "InfoTexture";
};

new SubShader(DRLInfoCalc20)
{
   VertexFile[0] = "Shaders/DRL/bloomGenV.glsl";
   PixelFile[0] = "Shaders/DRL/infoCalcP.glsl";
   ShaderQuality[0] = 0;
   
   ShaderType = $ShaderTypes::GLSLShaderType;
};

new Material()
{
   name = "DRLInfoCalc";
   
   Shader20 = DRLInfoCalc20;
   samplers[0] = "PrevInfoTexture";
   samplers[1] = "AverageIntTexture";
};

new SubShader(DRLFakeBloom20)
{
   VertexFile[0] = "Shaders/DRL/bloomGenV.glsl";
   PixelFile[0] = "Shaders/DRL/fakeBloomP.glsl";
   ShaderQuality[0] = 0;
   
   ShaderType = $ShaderTypes::GLSLShaderType;
};

new Material()
{
   name = "DRLFakeBloom";
   
   Shader20 = DRLFakeBloom20;
   samplers[0] = "BaseTexture";
   samplers[2] = "InfoTexture";
};