attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec2 a_texCoord1;

varying vec2 TextureCoordOut;
varying vec2 VertAttrib1;

uniform float u_time;
uniform float u_wind;
uniform float u_sway;
uniform float u_infl;
uniform mat4 u_MVPMatrix;

void main()
{
    TextureCoordOut = a_texCoord;
    VertAttrib1 = a_texCoord1;
    TextureCoordOut.y = 1.0 - TextureCoordOut.y;
    float absWind = abs(u_wind) / 10.0;
    vec4 grassAnim = vec4(sin(u_time * 10.0 * (VertAttrib1.y * absWind) * VertAttrib1.y * u_sway) * u_infl + u_wind, 0.0, 0.0, 0.0);
    gl_Position = u_MVPMatrix * a_position + (grassAnim * pow(VertAttrib1.x * 0.7, 2.1));
}
