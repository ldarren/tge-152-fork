#ifndef __GLSL_CG_DATA_TYPES
   #define half float
   #define half4 vec4
#endif

#ifndef __GLSL_CG_STDLIB
   #define h4tex2D texture2D
#endif

uniform half GoalIntensity;
uniform sampler2D texture;

void main()
{
   half4 kernel = vec4(0.375, 0.325, 0.225, 0.075) / 2.0;
   
   half4 color0 = h4tex2D(texture, gl_TexCoord[0].st);
   half4 color1 = h4tex2D(texture, gl_TexCoord[0].zw);
   half4 color2 = h4tex2D(texture, gl_TexCoord[1].st);
   half4 color3 = h4tex2D(texture, gl_TexCoord[1].zw);
   
   half4 color4 = h4tex2D(texture, gl_TexCoord[2].st);
   half4 color5 = h4tex2D(texture, gl_TexCoord[2].zw);
   half4 color6 = h4tex2D(texture, gl_TexCoord[3].st);
   half4 color7 = h4tex2D(texture, gl_TexCoord[3].zw);
   
   half4 finalColor = color0 * kernel.x + color1 * kernel.y + color2 * kernel.z + color3 * kernel.w;
   finalColor = finalColor + color4 * kernel.x + color5 * kernel.y + color6 * kernel.z + color7 * kernel.w;
   
   gl_FragColor = half4(finalColor.rgb, 1.0);
}