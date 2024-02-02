#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

uniform float threshold;

uniform sampler2D u_tex0;

void main()
{
	vec4 v_color = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 input_color = texture2D(u_tex0, v_texCoord);
	float value = 1.0;
	if (input_color.a < 0.5) value = 0.0;
	gl_FragColor = v_color * texture2D(u_tex0, v_texCoord).a * value;
}
