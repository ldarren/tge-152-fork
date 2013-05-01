// This shader should compile, but the program will fail to link.
varying vec4 pos;
void main()
{
   gl_FragColor = pos;
}
