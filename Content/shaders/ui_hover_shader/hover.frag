#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform float u_time;
uniform float u_val;

uniform sampler2D u_tex0;
uniform sampler2D u_tex1;
uniform sampler2D u_tex2;

float getAlphaEdge(float coord) {
	if (coord < 0.1) {
		return coord / 0.1;
	} else if (coord > 0.9) {
		return 1.0 - ((coord - 0.9) / 0.1);
	} else {
		return 1.0;
	}
}

void main()
{
	float val = clamp((u_val + 0.08) * 3.0, 0.0, 1.0);
	
	float time = u_time * 0.25;
    
	vec2 coord = vec2(mod(v_texCoord.x - time, 1.0), mod(v_texCoord.y + time, 1.0));

	float opacity = getAlphaEdge(v_texCoord.x) * getAlphaEdge(v_texCoord.y);

	if (texture2D(u_tex1, v_texCoord).a < val)
		gl_FragColor = v_fragmentColor * texture2D(u_tex0, coord) * texture2D(u_tex2, v_texCoord).a * opacity;
	else
		gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
}