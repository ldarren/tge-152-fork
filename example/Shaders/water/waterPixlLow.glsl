// This is here so the shader will suck less on the FX series.
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

vec4 lerp(vec4 x, vec4 y, float a)
{
   return mix(x, y, a);
}
#endif

// Enough uniforms?
uniform vec2 BaseDrift, TexRatio, RefractTexSize;
uniform float SurfaceParallax, ReflectTexSize;
uniform half SpecularPower;
uniform vec3 EyePos;
uniform sampler2D reflectionTex, NormalMap, DepthTexture, BaseTexture, RefractTexture;
uniform half4 SpecularColor;
uniform float IOR;
uniform float FresPower;
uniform float FresBias;
uniform float WaterColorBlend, ReflectionChoppiness, RefractionChoppiness;

varying vec4 HPosition;
varying vec3 HalfVec, EyeVec;

void main()
{
   vec4 offsets = vec4(ReflectionChoppiness, ReflectionChoppiness, RefractionChoppiness, RefractionChoppiness);
   vec4 refCoord;
   // Turn our projected position into a texture coordinate
   refCoord.xy = HPosition.xy / HPosition.w;
   refCoord.zw = refCoord.xy;
   refCoord = (refCoord + 1.0) * 0.5;
   refCoord = clamp(refCoord, 0.0, 1.0);
   refCoord.zw *= TexRatio;
   
   // Sample the unrefracted backbuffer and two normal map samples
   half4 refA = h4tex2D(RefractTexture, refCoord.zw);
   half3 normal0 =  h3tex2D(NormalMap, gl_TexCoord[0].st).rgb;
   half3 normal = 2.0 * normal0 - 1.0;
   vec4 texRatio = vec4(1.0, 1.0, TexRatio.x, TexRatio.y);
   
   vec4 pix = vec4(1.0/ReflectTexSize, 1.0/ReflectTexSize, 1.0/RefractTexSize.x, 1.0/RefractTexSize.y);
   
   half depthAlpha = h4tex2D(DepthTexture, gl_TexCoord[1].st).a;
      
   // Get our shifted texture coordinates
   refCoord += vec4(normal.xyxy * offsets);
   refCoord = clamp(refCoord, pix, texRatio - pix);
   
   // Dependent texture reads!
   half3 reflected = h3tex2D(reflectionTex, refCoord.xy).rgb;
   half4 refB = h4tex2D(RefractTexture, refCoord.zw);
   
   // This is a neat little thing.  See, anything that is
   // supposed to refract will have black in the alpha buffer, everything else
   // will be white.  This keeps us from having an ugly overlap where something
   // IN FRONT of the refracted surface is incorporated into the refraction.
   half4 refracted = lerp(refB, refA, refB.a);
   
   half3 waterColor = h3tex2D(BaseTexture, gl_TexCoord[0].st).rgb;
   
   // Get our base water color, include reflection
   reflected = lerp(reflected, waterColor, WaterColorBlend);
   
   
   half alpha = depthAlpha;
   
   // Toss in refraction
   reflected = lerp(refracted.rgb, reflected, alpha);
   
   // FOG
   reflected = lerp(reflected, gl_Color.rgb, gl_Color.a);
   
   // No need for alpha, we handled that with refraction
   gl_FragColor = vec4(reflected.rgb, 1.0);
}
