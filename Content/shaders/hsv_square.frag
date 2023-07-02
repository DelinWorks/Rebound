#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

uniform sampler2D u_tex0;

void main()
{
	vec4 c = texture2D(u_tex0, v_texCoord);
	if (c.b == 1.0) discard;
	gl_FragColor = c + (v_fragmentColor * (1.0 - c.a) * v_fragmentColor.a);
}
