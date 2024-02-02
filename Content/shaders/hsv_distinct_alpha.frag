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
	vec4 f = v_fragmentColor;
	f.a = 1.0;
	if (v_texCoord.x > 0.5)
		gl_FragColor = c * f * v_fragmentColor.a;
	else
		gl_FragColor = c * f;
}
