void main() 
{	
   // Pass through texcoords
	gl_TexCoord[0] = gl_MultiTexCoord0;
   gl_TexCoord[1] = gl_MultiTexCoord3;

   // Fixed function transform and clipping
	gl_Position = ftransform();
   #ifdef __GLSL_CG_DATA_TYPES
   // Clipping is wonderful
   gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
   #endif
}