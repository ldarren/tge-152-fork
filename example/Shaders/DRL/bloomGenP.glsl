uniform sampler2D BaseTexture;
uniform sampler2D InfoTexture;
uniform float SubtractiveTerm;

void main()
{
   vec4 info = texture2D(InfoTexture, vec2(0.5, 0.5));
   vec4 color = texture2D(BaseTexture, gl_TexCoord[0].st);
   
   color = color * (info.r * 2.0 + info.a * 2.0) - info.g - SubtractiveTerm;
   
   gl_FragColor = color;
}