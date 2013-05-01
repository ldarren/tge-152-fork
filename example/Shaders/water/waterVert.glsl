uniform vec2 BaseDrift;
uniform float SurfaceParallax;
varying vec4 HPosition;
varying vec3 HalfVec, EyeVec;
uniform vec3 LightDirection, EyePos;

void main()
{
   // We do a tighter sample of the normal map to make the reflection/refraction/specular
   // less patterned
   mat4 scale =   mat4(4.0, 0.0, 0.0, 0.0,
                       0.0, 4.0, 0.0, 0.0,
                       0.0, 0.0, 4.0, 0.0,
                       0.0, 0.0, 0.0, 1.0);
                       
   // Account for base drift and surface parallax so it looks like the water moves
   gl_TexCoord[0].st = gl_MultiTexCoord0.st + BaseDrift;
   gl_TexCoord[0].zw = gl_MultiTexCoord0.st + (BaseDrift * SurfaceParallax);
   gl_TexCoord[1].st = gl_MultiTexCoord2.st;
   vec4 texCoord = gl_TexCoord[0] * scale;
   gl_TexCoord[1].zw = texCoord.zw;
   gl_TexCoord[2] = gl_MultiTexCoord0;
   
   // Fixed function transform
   gl_Position = ftransform();
   HPosition = gl_Position;
   
   // FOG
   gl_FrontColor = gl_Color;
  
   // Specular calcs
   vec3 eyeVec = EyePos - gl_Vertex.xyz;
   eyeVec = normalize(eyeVec);
   EyeVec = vec3(eyeVec);
   HalfVec = -LightDirection + EyeVec;
   HalfVec = normalize(HalfVec);
}
