uniform vec4 xOffsets;
uniform vec4 yOffsets;
uniform vec2 MaxExtents;

void main()
{
   gl_Position = gl_Vertex;
   gl_TexCoord[0].st = clamp(gl_MultiTexCoord0.st + (vec2(xOffsets.x, yOffsets.x)), vec2(0.0, 0.0), MaxExtents);
   gl_TexCoord[0].zw = clamp(gl_MultiTexCoord0.st + (vec2(xOffsets.y, yOffsets.y)), vec2(0.0, 0.0), MaxExtents);
   gl_TexCoord[1].st = clamp(gl_MultiTexCoord0.st + (vec2(xOffsets.z, yOffsets.z)), vec2(0.0, 0.0), MaxExtents);
   gl_TexCoord[1].zw = clamp(gl_MultiTexCoord0.st + (vec2(xOffsets.w, yOffsets.w)), vec2(0.0, 0.0), MaxExtents);
   gl_TexCoord[2].st = clamp(gl_MultiTexCoord0.st - (vec2(xOffsets.x, yOffsets.x)), vec2(0.0, 0.0), MaxExtents);
   gl_TexCoord[2].zw = clamp(gl_MultiTexCoord0.st - (vec2(xOffsets.y, yOffsets.y)), vec2(0.0, 0.0), MaxExtents);
   gl_TexCoord[3].st = clamp(gl_MultiTexCoord0.st - (vec2(xOffsets.z, yOffsets.z)), vec2(0.0, 0.0), MaxExtents);
   gl_TexCoord[3].zw = clamp(gl_MultiTexCoord0.st - (vec2(xOffsets.w, yOffsets.w)), vec2(0.0, 0.0), MaxExtents);
}