new SubShader(InteriorTest)
{
   VertexFile[0] = "Shaders/UnitTests/basicTestV.glsl";
   PixelFile[0] = "Shaders/UnitTests/basicTestP.glsl";
   ShaderQuality[0] = 0;
};

new Material()
{
   mapTo = "modernization.fps/data/interiors/lightingPack/MTLPILRFRONT_B";
   Shader20 = InteriorParallax20;
   textures[0] = "Lightmap";
   textures[1] = "modernization.fps/data/interiors/lightingPack/MTLPILRFRONT_B";
   textures[2] = "modernization.fps/data/interiors/lightingPack/MTLPILRFRONT_B_height_auto";
   textures[3] = "Fog";

   samplers[0] = "LightMap";
   samplers[1] = "BaseTexture";
   samplers[2] = "NormalMap";
   samplers[3] = "FogTexture";
};

new Material()
{
   mapTo = "modernization.fps/data/interiors/lightingPack/RED";
   Shader20 = InteriorParallax20;
   textures[0] = "Lightmap";
   textures[1] = "modernization.fps/data/interiors/lightingPack/RED";
   textures[2] = "modernization.fps/data/interiors/lightingPack/RED_height_auto";
   textures[3] = "Fog";

   samplers[0] = "LightMap";
   samplers[1] = "BaseTexture";
   samplers[2] = "NormalMap";
   samplers[3] = "FogTexture";
};

new Material()
{
   mapTo = "modernization.fps/data/interiors/lightingPack/WALNOGROOVE_BM";
   Shader20 = InteriorParallax20;
   textures[0] = "Lightmap";
   textures[1] = "modernization.fps/data/interiors/lightingPack/WALNOGROOVE_BM";
   textures[2] = "modernization.fps/data/interiors/lightingPack/WALNOGROOVE_BM_height_auto";
   textures[3] = "Fog";

   samplers[0] = "LightMap";
   samplers[1] = "BaseTexture";
   samplers[2] = "NormalMap";
   samplers[3] = "FogTexture";
};

new Material()
{
   mapTo = "modernization.fps/data/interiors/lightingPack/WALMTLBASE1_01_";
   Shader20 = InteriorParallax20;
   textures[0] = "Lightmap";
   textures[1] = "modernization.fps/data/interiors/lightingPack/WALMTLBASE1_01_";
   textures[2] = "modernization.fps/data/interiors/lightingPack/WALMTLBASE1_01__height_auto";
   textures[3] = "Fog";

   samplers[0] = "LightMap";
   samplers[1] = "BaseTexture";
   samplers[2] = "NormalMap";
   samplers[3] = "FogTexture";
};

new Material()
{
   mapTo = "modernization.fps/data/interiors/lightingPack/FILLER_TILE101M";
   Shader20 = InteriorParallax20;
   textures[0] = "Lightmap";
   textures[1] = "modernization.fps/data/interiors/lightingPack/FILLER_TILE101M";
   textures[2] = "modernization.fps/data/interiors/lightingPack/FILLER_TILE101M_height_auto";
   textures[3] = "Fog";

   samplers[0] = "LightMap";
   samplers[1] = "BaseTexture";
   samplers[2] = "NormalMap";
   samplers[3] = "FogTexture";
};

new Material()
{
   mapTo = "modernization.fps/data/interiors/lightingPack/WALL_FILLER101M";
   Shader20 = InteriorParallaxDetail20;
   textures[0] = "Lightmap";
   textures[1] = "modernization.fps/data/interiors/lightingPack/WALL_FILLER101M";
   textures[2] = "modernization.fps/data/interiors/lightingPack/WALL_FILLER101M_height_auto";
   textures[3] = "Fog";
   textures[4] = "modernization.fps/data/interiors/lightingPack/Wall_detail.png";

   samplers[0] = "LightMap";
   samplers[1] = "BaseTexture";
   samplers[2] = "NormalMap";
   samplers[3] = "FogTexture";
   samplers[4] = "DetailMap";
};

new Material()
{
   mapTo = "modernization.fps/data/interiors/lightingPack/WALL_PANEL01_B";
   Shader20 = InteriorParallax20;
   textures[0] = "Lightmap";
   textures[1] = "modernization.fps/data/interiors/lightingPack/WALL_PANEL01_B";
   textures[2] = "modernization.fps/data/interiors/lightingPack/WALL_PANEL01_B_height_auto";
   textures[3] = "Fog";

   samplers[0] = "LightMap";
   samplers[1] = "BaseTexture";
   samplers[2] = "NormalMap";
   samplers[3] = "FogTexture";
};

new Material()
{
   mapTo = "modernization.fps/data/interiors/lightingPack/MTLPILRTOP_B";
   Shader20 = InteriorParallax20;
   textures[0] = "Lightmap";
   textures[1] = "modernization.fps/data/interiors/lightingPack/MTLPILRTOP_B";
   textures[2] = "modernization.fps/data/interiors/lightingPack/MTLPILRTOP_B_height_auto";
   textures[3] = "Fog";

   samplers[0] = "LightMap";
   samplers[1] = "BaseTexture";
   samplers[2] = "NormalMap";
   samplers[3] = "FogTexture";
};

new Material()
{
   mapTo = "modernization.fps/data/interiors/lightingPack/WHITE";
   Shader20 = InteriorParallax20;
   textures[0] = "Lightmap";
   textures[1] = "modernization.fps/data/interiors/lightingPack/WHITE";
   textures[2] = "modernization.fps/data/interiors/lightingPack/WHITE_height_auto";
   textures[3] = "Fog";

   samplers[0] = "LightMap";
   samplers[1] = "BaseTexture";
   samplers[2] = "NormalMap";
   samplers[3] = "FogTexture";
};

new Material()
{
   mapTo = "modernization.fps/data/interiors/lightingPack/OAK2_B";
   Shader20 = InteriorParallax20;
   textures[0] = "Lightmap";
   textures[1] = "modernization.fps/data/interiors/lightingPack/OAK2_B";
   textures[2] = "modernization.fps/data/interiors/lightingPack/OAK2_B_height_auto";
   textures[3] = "Fog";

   samplers[0] = "LightMap";
   samplers[1] = "BaseTexture";
   samplers[2] = "NormalMap";
   samplers[3] = "FogTexture";
};

new Material()
{
   mapTo = "modernization.fps/data/interiors/lightingPack/KNOT_B";
   Shader20 = InteriorParallax20;
   textures[0] = "Lightmap";
   textures[1] = "modernization.fps/data/interiors/lightingPack/KNOT_B";
   textures[2] = "modernization.fps/data/interiors/lightingPack/KNOT_B_height_auto";
   textures[3] = "Fog";

   samplers[0] = "LightMap";
   samplers[1] = "BaseTexture";
   samplers[2] = "NormalMap";
   samplers[3] = "FogTexture";
};
