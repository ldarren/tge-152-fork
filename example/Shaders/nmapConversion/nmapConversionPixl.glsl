uniform sampler2D tex;
uniform vec2 TextureSize;

// Abandon hope all ye who enter here.
// This does a 3x3 sobel filter in the most horrifically
// inefficient manner possible.  It's still loads faster
// than any CPU based solution.

void main()
{
   float pixX = 1.0/TextureSize.x;
   float pixY = 1.0/TextureSize.y;
   vec2 texCoord = gl_TexCoord[0].st;
   vec4 color = texture2D(tex, texCoord);
   //color.r *= 0.111;
   vec4 textureColor;
   vec4 filterColor[2];
   
   texCoord.s = gl_TexCoord[0].s - pixX;
   texCoord.t = gl_TexCoord[0].t + pixY;
   texCoord = clamp(texCoord, 0.0, 1.0);
   textureColor = texture2D(tex, texCoord);
   //color.r += textureColor.r * 0.111;
   filterColor[0] = textureColor * -1.0;
   
   texCoord.t = gl_TexCoord[0].t;
   texCoord = clamp(texCoord, 0.0, 1.0);
   textureColor = texture2D(tex, texCoord);
   //color.r += textureColor.r * 0.111;
   filterColor[0] += textureColor * -1.0;
   
   texCoord.t = gl_TexCoord[0].t - pixY;
   texCoord = clamp(texCoord, 0.0, 1.0);
   textureColor = texture2D(tex, texCoord);
   //color.r += textureColor.r * 0.111;
   filterColor[0] += textureColor * -1.0;
   
   texCoord.s = gl_TexCoord[0].s + pixX;
   texCoord.t = gl_TexCoord[0].t + pixY;
   texCoord = clamp(texCoord, 0.0, 1.0);
   textureColor = texture2D(tex, texCoord);
   //color.r += textureColor.r * 0.111;
   filterColor[0] += textureColor;
   
   texCoord.t = gl_TexCoord[0].t;
   texCoord = clamp(texCoord, 0.0, 1.0);
   textureColor = texture2D(tex, texCoord);
   //color.r += textureColor.r * 0.111;
   filterColor[0] += textureColor;
   
   texCoord.t = gl_TexCoord[0].t - pixY;
   texCoord = clamp(texCoord, 0.0, 1.0);
   textureColor = texture2D(tex, texCoord);
   //color.r += textureColor.r * 0.111;
   filterColor[0] += textureColor;
   
   texCoord.s = gl_TexCoord[0].s - pixX;
   texCoord.t = gl_TexCoord[0].t + pixY;
   texCoord = clamp(texCoord, 0.0, 1.0);
   filterColor[1] = texture2D(tex, texCoord);
   
   texCoord.s = gl_TexCoord[0].s;
   texCoord = clamp(texCoord, 0.0, 1.0);
   textureColor = texture2D(tex, texCoord);
   //color.r += textureColor.r * 0.111;
   filterColor[1] += textureColor;
   
   texCoord.s = gl_TexCoord[0].s + pixX;
   texCoord = clamp(texCoord, 0.0, 1.0);
   filterColor[1] += texture2D(tex, texCoord);
   
   texCoord.s = gl_TexCoord[0].s - pixX;
   texCoord.t = gl_TexCoord[0].t - pixY;
   texCoord = clamp(texCoord, 0.0, 1.0);
   filterColor[1] += texture2D(tex, texCoord) * -1.0;
   
   texCoord.s = gl_TexCoord[0].s;
   textureColor = texture2D(tex, texCoord);
   //color.r += textureColor.r * 0.111;
   texCoord = clamp(texCoord, 0.0, 1.0);
   filterColor[1] += textureColor * -1.0;
   
   texCoord.s = gl_TexCoord[0].s + pixX;
   texCoord = clamp(texCoord, 0.0, 1.0);
   filterColor[1] += texture2D(tex, texCoord) * -1.0;
   
   vec3 normal = vec3(-filterColor[0].r * 2.0, -filterColor[1].r * 2.0, 1.0);
   normal = normalize(normal);
   normal += 1.0;
   normal *= 0.5;
   
   gl_FragColor = vec4(normal, color.r);
}