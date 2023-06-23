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
	gl_FragColor = v_fragmentColor * texture2D(u_tex0, v_texCoord);
}
