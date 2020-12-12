/*@shader @description default vertex shader to swipe images
path: MastactvaShaders/MoveImages/Vertex/swipe.vert */
attribute highp vec4 vertexArg;
attribute mediump vec4 texCoordArg;

uniform mediump mat4 matrixArg;
uniform mediump mat4 texMatrix1Arg;
uniform mediump mat4 texMatrix2Arg;

/*@argument
  @defaultvalue (1.0, 0.0)
  @description swipe direction
*/
uniform mediump vec2 swipeDirection;

uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;

void main(void)
{
    gl_Position = matrixArg * vertexArg;
    texCoord1Var = texMatrix1Arg * texCoordArg - t*vec4(swipeDirection);
    texCoord2Var = texMatrix2Arg * texCoordArg - t*vec4(swipeDirection) + vec4(swipeDirection);
}
