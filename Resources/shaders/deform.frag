#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

uniform float u_time;

uniform sampler2D u_tex0;

void main()
{
	float waveu = sin((v_texCoord.x + u_time) * 20.0) * 0.5 * 0.05 * 0.1;
	float wavev = cos((v_texCoord.y + u_time) * 20.0) * 0.5 * 0.05 * 0.1;
	gl_FragColor = v_fragmentColor * texture2D(u_tex0, v_texCoord + vec2(wavev, waveu));
}
