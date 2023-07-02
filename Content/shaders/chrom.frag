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
	float aberrationAmount = .2;

    vec2 uv = v_texCoord;
	vec2 distFromCenter = uv - 0.5;

    // stronger aberration near the edges by raising to power 3
    vec2 aberrated = aberrationAmount * pow(distFromCenter, vec2(1.0, 1.0));
	
	gl_FragColor = vec4
    (
    	texture2D(u_tex0, uv - 0.001).r,
    	texture2D(u_tex0, uv).g,
    	texture2D(u_tex0, uv + 0.001).b,
    	texture2D(u_tex0, uv + 0.001).a
    );
}
