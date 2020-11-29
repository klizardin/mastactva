/*  @shader
    @description
    vertext test shader
*/

attribute highp vec4 vertexArg;
attribute mediump vec4 texCoordArg;
uniform mediump mat4 matrixArg;
varying mediump vec4 texCoord;

void main(void)
{
    gl_Position = matrixArg * vertexArg;
    texCoord = texCoordArg;
}
