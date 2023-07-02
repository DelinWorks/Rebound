#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

uniform float u_time;

uniform sampler2D u_tex0;

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main()
{
	float alpha = abs(map(v_texCoord.x - 0.5, -0.5, 0.5, -1.0, 1.0));
	float alpha2 = abs(map(v_texCoord.y - 0.5, -0.5, 0.5, -1.0, 1.0));
	gl_FragColor = v_fragmentColor * texture2D(u_tex0, v_texCoord + vec2(0.0, alpha * 0.1 * alpha2));
}
