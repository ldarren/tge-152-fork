uniform sampler2D LightMap, BaseTexture, NormalMap, FogTexture;
varying vec3 eyeVec;
uniform vec3 LightDir;

// If DRL is enabled, this is 1, otherwise it is 2.  This way we get shaders and
// standard TLK overbright, yay!
uniform float tge_LightingOverbright;

// Samples nmap at st and returns 2 * rgb - 1 (standard normal expansion)
vec3 expandNormal(sampler2D nmap, vec2 st)
{
   return 2.0 * texture2D(nmap, st).rgb - 1.0;
}

// Does parallax mapping, and clamps the diffuse contribution from the light to min, max
vec4 parallaxMap(sampler2D diffuse, sampler2D pmap, vec2 st, vec3 eyeDir, vec3 lightDir, 
                 float scale, float bias, float min, float max)
{
   float height = texture2D(pmap, st).a;
   
   height = height * scale - bias;
   
   vec2 modTexST = height * eyeDir.xy;
   modTexST += st;
   
   vec4 BaseColor = texture2D(diffuse, modTexST);
   vec3 Normal = expandNormal(pmap, modTexST);
   
   float contrib = dot(Normal, lightDir);
   contrib = clamp(contrib, min, max);
   
   return BaseColor * contrib;
}

void main()
{   
   // Normalize the eye
   vec3 eye = normalize(eyeVec);

   // Sample our normal map and diffuse map
   vec3 LightMapColor = texture2D(LightMap, gl_TexCoord[0].st).rgb;
   vec3 baseColor = texture2D(BaseTexture, gl_TexCoord[0].zw).rgb;
   
   // TOTAL HAXXOR!!
   vec3 light = vec3(1.0, 1.0, 1.0);
   vec4 LitColor = parallaxMap(BaseTexture, NormalMap, gl_TexCoord[0].zw, 
                                                       eye, light, 0.080000, 0.040000, 0.3, 1.0);
   // Modulate the lightmap
   LitColor.rgb *= LightMapColor;
   
   // Silly texture fog
   vec4 fog = texture2D(FogTexture, gl_TexCoord[1].st);
   vec3 color = mix(LitColor.rgb * tge_LightingOverbright, fog.rgb, fog.a);
   
   // Maybe I should deal with alpha properly...
   gl_FragColor = vec4(color, 1.0);
}