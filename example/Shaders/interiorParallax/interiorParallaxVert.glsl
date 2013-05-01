uniform vec3 EyePos;

varying vec3 eyeVec;

void main() 
{	
   // Pass through texcoord
   gl_TexCoord[0] = gl_MultiTexCoord0;

   // Translate our input texcoords into TBN info
   vec3 tangent = gl_MultiTexCoord1.xyz;
   vec3 bitangent = gl_MultiTexCoord2.xyz;
   vec3 normal = gl_MultiTexCoord4.xyz;
    
   // Pass through texcoord
   gl_TexCoord[1] = gl_MultiTexCoord3;
   // Fixed function transform
   gl_Position = ftransform();
   #ifdef __GLSL_CG_DATA_TYPES
   // Clipping is wonderful
   gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
   #endif
   
   // Calc eyevec
   vec3 EyeVec = EyePos - gl_Vertex.xyz;
   // Create TBN matrix
   mat3 tangentSpace = mat3(tangent, bitangent, normal);
   
   // Rotate EyeVec into tangent space
   eyeVec = EyeVec * tangentSpace;
}