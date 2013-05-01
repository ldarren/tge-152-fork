uniform sampler2D LightMap, BaseTexture, NormalMap, FogTexture;
uniform vec3 LightDir;

// If DRL is enabled, this is 1, otherwise it is 2.  This way we get shaders and
// standard TLK overbright, yay!
uniform float tge_LightingOverbright;

// Samples nmap at st and returns 2 * rgb - 1 (standard normal expansion)
vec3 expandNormal(sampler2D nmap, vec2 st)
{
   return 2.0 * texture2D(nmap, st).rgb - 1.0;
}

// Dots lightDir and the normal from nmap, clamps that to (min, max) then modulates the diffuse texture by that
vec4 normalmapDiffuse(sampler2D diffuse, sampler2D nmap, vec2 st, vec3 lightDir, float min, float max)
{
   vec3 normal = expandNormal(nmap, st);
   float contrib = dot(lightDir, normal);
   return clamp(contrib, min, max) * texture2D(diffuse, st);
}

void main()
{
   // We don't even use this
   vec3 light = LightDir * -1.0;

   // Lightmap
   vec3 LightMapColor = texture2D(LightMap, gl_TexCoord[0].st).rgb;
   
   // TOTAL HAXXOR
   vec4 BaseColor = normalmapDiffuse(BaseTexture, NormalMap, gl_TexCoord[0].zw, vec3(1, 1, 1), 0.3, 1.0);
   
   // Lighting
   BaseColor.rgb *= LightMapColor;

   // Silly texture fog
   vec4 fog = texture2D(FogTexture, gl_TexCoord[1].st);
   vec3 color = mix(BaseColor.rgb * tge_LightingOverbright, fog.rgb, fog.a);

   // Should probably deal with alpha properly some day
   gl_FragColor = vec4(color, 1.0);
}