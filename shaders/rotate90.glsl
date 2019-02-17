uniform sampler2D tex;

void main()
{
	vec4 t0 = texture2D(tex, vec2(gl_TexCoord[0].t, gl_TexCoord[0].s));
	gl_FragColor = t0;
}