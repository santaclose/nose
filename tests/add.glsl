uniform sampler2D tex0;
uniform sampler2D tex1;
uniform int a;

void main()
{
    vec4 t0 = texture2D(tex0, gl_TexCoord[0].st);
    vec4 t1 = texture2D(tex1, gl_TexCoord[0].st);
    gl_FragColor = t0 * t1 + a;
}