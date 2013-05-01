uniform sampler2D LightMap, BaseTexture, NormalMap, FogTexture, DetailMap;
varying vec3 eyeVec;
uniform vec3 LightDir;

// If DRL is enabled, this is 1, otherwise it is 2.  This way we get shaders and
// standard TLK overbright, yay!
uniform float tge_LightingOverbright;

void main()
{   
   // Normalize the eye
   vec3 eye = normalize(eyeVec);
   
   float height = texture2D(NormalMap, gl_TexCoord[0].zw).a;
   height += (texture2D(DetailMap, gl_TexCoord[2].st).a * 0.25);
   height += (texture2D(DetailMap, gl_TexCoord[2].zw).a * 0.125);
   height = height * 0.08 - 0.04;
   vec2 modTexST = gl_TexCoord[0].zw + height * eye.xy;
   vec4 modDetailST = gl_TexCoord[2] + height * eye.xyxy * vec4(0.25, 0.25, 0.125, 0.125);

   // Sample our normal map and diffuse map
   vec3 LightMapColor = texture2D(LightMap, gl_TexCoord[0].st).rgb;
   vec4 baseColor = texture2D(BaseTexture, modTexST);
   float detailColor0 = texture2D(DetailMap, modDetailST.st).a * 2.0;
   float detailColor1 = texture2D(DetailMap, modDetailST.zw).a * 2.0;
   vec3 normal = 2.0 * texture2D(NormalMap, modTexST).rgb - 1.0;
   vec3 normal1 = (( 2.0 * texture2D(DetailMap, modDetailST.st).rgb - 1.0));
   vec3 normal2 = ((2.0 * texture2D(DetailMap, modDetailST.zw).rgb - 1.0));
   
   // TOTAL HAXXOR!!
   vec3 light = vec3(1.0, 1.0, 1.0);
   
   float contrib = dot(normal, light);
   contrib = clamp(contrib, 0.3, 1.0);
   baseColor *= contrib;
   detailColor0 *= clamp(dot(normal1, light), 0.5, 1.0);
   detailColor1 *= clamp(dot(normal2, light), 0.5, 1.0);
   
   vec4 LitColor = baseColor * detailColor0 * detailColor1;
   // Modulate the lightmap
   LitColor.rgb *= LightMapColor;
   
   // Silly texture fog
   vec4 fog = texture2D(FogTexture, gl_TexCoord[1].st);
   vec3 color = mix(LitColor.rgb * tge_LightingOverbright, fog.rgb, fog.a);
   
   // Maybe I should deal with alpha properly...
   gl_FragColor = vec4(color, 1.0);
}