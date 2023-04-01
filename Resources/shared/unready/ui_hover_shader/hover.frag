#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform float u_val;

uniform sampler2D u_tex0;
uniform sampler2D u_tex1;

float quadEaseOut(float time)
{
    return -1.0 * time * (time - 2.0);
}

void main()
{
	float val = clamp(u_val * 4.0, 0.0, 1.0);

	val = quadEaseOut(val);

	if (texture2D(u_tex1, v_texCoord).a < val)
		gl_FragColor = v_fragmentColor * texture2D(u_tex0, v_texCoord);
	else
		gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
}