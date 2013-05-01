uniform sampler2DRect samp;

void main()
{
	gl_FragColor = texture2DRect(samp, vec2(0.0, 0.0));
}
