uniform vec2 BaseDrift;
uniform float SurfaceParallax;
varying vec4 HPosition;
varying vec3 HalfVec, EyeVec;
uniform vec3 LightDirection, EyePos;

void main()
{
   mat4 scale =   mat4(4.0, 0.0, 0.0, 0.0,
                       0.0, 4.0, 0.0, 0.0,
                       0.0, 0.0, 4.0, 0.0,
                       0.0, 0.0, 0.0, 1.0);
   gl_TexCoord[0].st = gl_MultiTexCoord0.st + BaseDrift;
   gl_TexCoord[0].zw = gl_MultiTexCoord0.st + (BaseDrift * SurfaceParallax);
   gl_TexCoord[1].st = gl_MultiTexCoord2.st;
   vec4 texCoord = gl_TexCoord[0] * scale;
   gl_TexCoord[1].zw = texCoord.st;
   gl_TexCoord[2] = gl_MultiTexCoord0;
   gl_Position = ftransform();
   HPosition = gl_Position;
   gl_FrontColor = gl_Color;
  
   vec3 eyeVec = EyePos - gl_Vertex.xyz;
   eyeVec = normalize(eyeVec);
   EyeVec = vec3(eyeVec);
   HalfVec = -LightDirection + EyeVec;
   HalfVec = normalize(HalfVec);
}
