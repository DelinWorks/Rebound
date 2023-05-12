varying vec2 v_texCoord;
varying vec4 v_fragmentColor;

float offset = 1.0 / 512.0;

float ratio = 1.777777777777778;

uniform sampler2D u_tex0;

void main()
{
	vec4 col = texture2D(u_tex0, v_texCoord);
	if (col.a > 0.5)
		gl_FragColor = col;
	else {
		float a = texture2D(u_tex0, vec2(v_texCoord.x + offset / ratio, v_texCoord.y)).a +
			texture2D(u_tex0, vec2(v_texCoord.x, v_texCoord.y - offset)).a +
			texture2D(u_tex0, vec2(v_texCoord.x - offset / ratio, v_texCoord.y)).a +
			texture2D(u_tex0, vec2(v_texCoord.x, v_texCoord.y + offset)).a;
		if (col.a < 1.0 && a > 0.0)
			gl_FragColor = vec4(0.0, 0.0, 0.0, 0.8);
		else
			gl_FragColor = col;
	}
}