/*@shader @description default vertex shader to swipe images
path: MastactvaShaders/MoveImages/Vertex/swipe.vert */
attribute highp vec4 vertexArg;
attribute mediump vec4 texCoordArg;

uniform mediump mat4 matrixArg;
uniform mediump mat4 texMatrix1Arg;
uniform mediump mat4 texMatrix2Arg;

/*@argument
  @defaultValue (-1.0, 0.0)
  @description swipe direction to swipe to the direction
  (1.0, 0.0) -- to the right
  (-1.0, 0.0) -- to the left
  (0.0, 1.0) -- to the up
  (0.0, -1.0) -- to the down
*/
uniform mediump vec2 swipeDirection;

uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;

void main(void)
{
    mediump vec4 sd = vec4(
                swipeDirection.x > 0.0 ? -1.0 : swipeDirection.x < 0.0 ? 1.0 : 0.0 ,
                swipeDirection.y > 0.0 ? -1.0 : swipeDirection.y < 0.0 ? 1.0 : 0.0 ,
                0.0,
                0.0);
    gl_Position = matrixArg * vertexArg;
    texCoord1Var = texMatrix1Arg * (texCoordArg + t * sd);
    texCoord2Var = texMatrix2Arg * (texCoordArg + (t - 1.0) * sd);
}
