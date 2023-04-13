#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

const float blurSize = 1.0/2048.0;
const float intensity = 0.5;

uniform sampler2D u_tex0;

void main()
{
   vec4 sum = vec4(0);
   vec2 texcoord = v_texCoord.xy;

    sum += texture2D(u_tex0, vec2(texcoord.x - 4.0*blurSize, texcoord.y)) * 0.05;
    sum += texture2D(u_tex0, vec2(texcoord.x - 3.0*blurSize, texcoord.y)) * 0.09;
    sum += texture2D(u_tex0, vec2(texcoord.x - 2.0*blurSize, texcoord.y)) * 0.12;
    sum += texture2D(u_tex0, vec2(texcoord.x - blurSize, texcoord.y)) * 0.15;
    sum += texture2D(u_tex0, vec2(texcoord.x, texcoord.y)) * 0.16;
    sum += texture2D(u_tex0, vec2(texcoord.x + blurSize, texcoord.y)) * 0.15;
    sum += texture2D(u_tex0, vec2(texcoord.x + 2.0*blurSize, texcoord.y)) * 0.12;
    sum += texture2D(u_tex0, vec2(texcoord.x + 3.0*blurSize, texcoord.y)) * 0.09;
    sum += texture2D(u_tex0, vec2(texcoord.x + 4.0*blurSize, texcoord.y)) * 0.05;
	 
    sum += texture2D(u_tex0, vec2(texcoord.x, texcoord.y - 4.0*blurSize)) * 0.05;
    sum += texture2D(u_tex0, vec2(texcoord.x, texcoord.y - 3.0*blurSize)) * 0.09;
    sum += texture2D(u_tex0, vec2(texcoord.x, texcoord.y - 2.0*blurSize)) * 0.12;
    sum += texture2D(u_tex0, vec2(texcoord.x, texcoord.y - blurSize)) * 0.15;
    sum += texture2D(u_tex0, vec2(texcoord.x, texcoord.y)) * 0.16;
    sum += texture2D(u_tex0, vec2(texcoord.x, texcoord.y + blurSize)) * 0.15;
    sum += texture2D(u_tex0, vec2(texcoord.x, texcoord.y + 2.0*blurSize)) * 0.12;
    sum += texture2D(u_tex0, vec2(texcoord.x, texcoord.y + 3.0*blurSize)) * 0.09;
    sum += texture2D(u_tex0, vec2(texcoord.x, texcoord.y + 4.0*blurSize)) * 0.05;

    gl_FragColor = sum * 1.0 + texture2D(u_tex0, v_texCoord);
}
