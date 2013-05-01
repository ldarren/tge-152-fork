#ifndef __GLSL_CG_DATA_TYPES
   #define half  float
   #define half2 vec2
   #define half3 vec3
   #define half4 vec4
#endif

#ifndef __GLSL_CG_STDLIB
vec4 h4tex2D(sampler2D tex, vec2 st)
{
   return texture2D(tex, st);
}

vec3 h3tex2D( sampler2D tex, vec2 st)
{
   return texture2D(tex, st).rgb;
}

vec3 lerp(vec3 x, vec3 y, float a)
{
   return mix(x, y, a);
}
#endif

uniform float ReflectTexSize, ReflectionChoppiness, FresBias, FresPower;
uniform half SpecularPower;
uniform vec3 EyePos;
uniform sampler2D reflectionTex, NormalMap, DepthTexture, BaseTexture;
uniform half4 SpecularColor;

varying vec4 HPosition;
varying vec3 EyeVec;

void main()
{
   vec2 refCoord = HPosition.xy / HPosition.w;
   refCoord = (refCoord + 1.0) * 0.5;
   half3 normal0 =  h3tex2D(NormalMap, gl_TexCoord[0].st).rgb;
   half3 normal1 =  h3tex2D(NormalMap, gl_TexCoord[0].zw).rgb;
   half3 normal = 2.0 * (normal0 + normal1) - 2.0;
   
   vec2 pix = vec2(1.0/ReflectTexSize, 1.0/ReflectTexSize);
   refCoord += vec2(normal.xy * vec2(ReflectionChoppiness, ReflectionChoppiness));
   refCoord = clamp(refCoord, pix, 1.0 - pix);
   
   half depthAlpha = h4tex2D(DepthTexture, gl_TexCoord[1].st).a;
   half3 reflected = h3tex2D(reflectionTex, refCoord).rgb;
   half3 waterColor = h3tex2D(BaseTexture, gl_TexCoord[0].st).rgb;
      
   reflected = lerp(reflected, waterColor, depthAlpha);
   reflected = lerp(reflected.rgb, gl_Color.rgb, gl_Color.a);
   gl_FragColor = vec4(reflected.rgb, depthAlpha);
}
