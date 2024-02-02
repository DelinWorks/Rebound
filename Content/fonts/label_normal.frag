#ifdef GL_ES
precision lowp float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;

uniform vec4 u_textColor;
uniform sampler2D u_texture;

void main()
{
    gl_FragColor =  v_fragmentColor * vec4(u_textColor.rgb,// RGB from uniform
        u_textColor.a * texture2D(u_texture, v_texCoord).a// A from texture & uniform
    );
}