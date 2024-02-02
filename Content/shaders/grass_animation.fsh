#ifdef GL_ES
varying mediump vec2 TextureCoordOut;
varying mediump vec4 ColorOut;
#else
varying vec2 TextureCoordOut;
varying vec2 VertAttrib1;
#endif
uniform vec4 u_color;
uniform vec4 u_topColor;
uniform vec4 u_bottomColor;

uniform sampler2D u_tex0;

void main(void)
{
    vec4 final = texture2D(u_tex0, TextureCoordOut) * u_color * mix(u_bottomColor, u_topColor, 1.0 - TextureCoordOut.y);
    if (final.a == 0.0)
        discard;
    gl_FragColor = final;
}
