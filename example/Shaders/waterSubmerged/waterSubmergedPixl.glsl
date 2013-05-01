#ifndef __GLSL_CG_DATA_TYPES
   #define half  float
   #define half2 vec2
   #define half3 vec3
   #define half4 vec4
#endif

#ifndef __GLSL_CG_STDLIB
   #define h4tex2D texture2D
   #define h3tex2D texture2D
   #define h2tex2D texture2D
   #define h1tex2D texture2D
   #define lerp    mix
#endif

uniform sampler2D DepthTexture, BaseTexture, RefractTexture, NormalMap;
varying vec4 HPosition;
varying vec3 EyeVec;
uniform vec2 RefractTexSize, TexRatio;
uniform float IOR;

void main()
{
   vec2 refCoord = HPosition.xy / HPosition.w;
   refCoord = (refCoord + 1.0) * 0.5;
   refCoord = clamp(refCoord, vec2(0.0, 0.0), vec2(1.0, 1.0));
   refCoord *= TexRatio;
   half depthAlpha = h4tex2D(DepthTexture, gl_TexCoord[1].st).a;
   
   half4 refA = h4tex2D(RefractTexture, refCoord);
   
   half3 normal0 = h3tex2D(NormalMap, gl_TexCoord[0].st).rgb;
   half3 normal1 = h3tex2D(NormalMap, gl_TexCoord[0].zw).rgb;
   half3 normal2 = h3tex2D(NormalMap, gl_TexCoord[1].zw).rgb;   
    half3 normal = 2.0 * (normal0 + normal1 + normal2) - 3.0;
   normal *= 0.015;
   normal.z = -1.0;
   //normal = vec3(0.0, 0.0, 1.0);
   normal = normalize(normal);
   half3 eyeVec = normalize(half3(EyeVec));
   
   float pixX = 1.0/RefractTexSize.x;
   float pixY = 1.0/RefractTexSize.y;
   
   refCoord.xy = refCoord.xy + refract(eyeVec, normal, IOR).xy;
   
   refCoord.x = min(max(pixX, refCoord.x), TexRatio.x - pixX);
   refCoord.y = min(max(pixY, refCoord.y), TexRatio.y - pixY);
   
   half4 refB = h4tex2D(RefractTexture, refCoord);
   half4 refracted = refA * refB.a + refB * (1.0 - refB.a);
   //refB *= (1.0 - refA.a);
   //vec4 refracted = refB;

   half3 waterColor = h3tex2D(BaseTexture, gl_TexCoord[2].st).rgb * (depthAlpha);
   //refracted *= 1.0 - depthAlpha;
   waterColor += refracted.rgb;
            
   waterColor *= 1.0 - gl_Color.a;
   vec3 fog = gl_Color.rgb * gl_Color.a;
   waterColor += fog;
   
   gl_FragColor = vec4(waterColor.rgb, 1.0);
}
