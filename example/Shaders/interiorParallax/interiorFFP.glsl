uniform sampler2D LightMap, BaseTexture, FogTexture;

// If DRL is enabled, this is 1, otherwise it is 2.  This way we get shaders and
// standard TLK overbright, yay!
uniform float tge_LightingOverbright;

void main()
{
   // Lightmap
   vec3 LightMapColor = texture2D(LightMap, gl_TexCoord[0].st).rgb;
   
   // TOTAL HAXXOR
   vec4 BaseColor = texture2D(BaseTexture, gl_TexCoord[0].zw);
   
   // Lighting
   BaseColor.rgb *= LightMapColor;

   // Silly texture fog
   vec4 fog = texture2D(FogTexture, gl_TexCoord[1].st);
   vec3 color = mix(BaseColor.rgb * tge_LightingOverbright, fog.rgb, fog.a);

   // Should probably deal with alpha properly some day
   gl_FragColor = vec4(color, 1.0);
}