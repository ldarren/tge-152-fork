uniform sampler2D tex;

const vec3 kernel = vec3(0.3, 0.59, 0.11);

void main()
{
   vec3 color = texture2D(tex, gl_TexCoord[0].st).rgb;
   float avg = dot(color, kernel);
   gl_FragColor = vec4(avg, avg, avg, 1.0);
}