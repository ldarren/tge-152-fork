uniform sampler2D tex;
uniform vec2 TextureSize;

const vec3 kernel = vec3(0.3, 0.59, 0.11);

void calcOffsetTexCoords(vec2 st, vec2 texSize, out vec2 offsets[9])
{
   vec2 offset = 1.0/texSize;
   float s, t;
   
   s = st.x - offset.x;
   t = st.y + offset.y;
   offsets[0] = vec2(s, t);
   
   s = st.x;
   offsets[1] = vec2(s, t);
   
   s = st.x + offset.x;
   offsets[2] = vec2(s, t);
   
   s = st.x - offset.x;
   t = st.y;
   offsets[3] = vec2(s, t);
   
   s = st.x;
   offsets[4] = vec2(s, t);
   
   s = st.x + offset.x;
   offsets[5] = vec2(s, t);
   
   s = st.x - offset.x;
   t = st.y - offset.y;
   offsets[6] = vec2(s, t);
   
   s = st.x;
   offsets[7] = vec2(s, t);
   
   s = st.x + offset.x;
   offsets[8] = vec2(s, t);
}

vec4 mean3x3(sampler2D tex, vec2 st[9])
{
   float kernel[9];
   kernel[0] = 1.0/9.0; kernel[1] = 1.0/9.0; kernel[2] = 1.0/9.0;
   kernel[3] = 1.0/9.0; kernel[4] = 1.0/9.0; kernel[5] = 1.0/9.0;
   kernel[6] = 1.0/9.0; kernel[7] = 1.0/9.0; kernel[8] = 1.0/9.0;
   vec4 sample;
   sample = texture2D(tex, st[0]) * kernel[0];
   for(int i = 1; i < 9; i++)
   {
      sample += texture2D(tex, st[i]) * kernel[i];
   }
   return sample;
}

vec4 grayscale(vec4 color)
{
   float gray = dot(color.rgb, vec3(0.3, 0.59, 0.11));
   return vec4(gray, gray, gray, 1.0);
}

void main()
{
   vec2 offsets[9];
   calcOffsetTexCoords(gl_TexCoord[0].st, TextureSize, offsets);
   
   vec4 color = mean3x3(tex, offsets);
   
   vec4 gray = grayscale(color);
   
   gl_FragColor = vec4(gray * 1.26 - 0.052);
}