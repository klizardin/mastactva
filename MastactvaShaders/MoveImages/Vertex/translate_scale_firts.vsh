/*@shader @description default vertex shader to swipe images
path: MastactvaShaders/MoveImages/Vertex/swipe.vert */
attribute highp vec4 vertexArg;
attribute mediump vec4 texCoordArg;

uniform mediump mat4 matrixArg;
uniform mediump mat4 texMatrix1Arg;
uniform mediump mat4 texMatrix2Arg;

/*@argument
  @defaultValue (0.0, 0.0, 0.0, 0.0)
  @description translate value
*/
uniform mediump vec4 translate0;

/*@argument
  @defaultValue (0.0, 0.0, 0.0, 0.0)
  @description translate value
*/
uniform mediump vec4 translate1;

/*@argument
  @defaultValue (1.0, 1.0, 1.0, 1.0)
  @description sacle value
*/
uniform mediump vec4 scale0;

/*@argument
  @defaultValue (1.0, 1.0, 1.0, 1.0)
  @description sacle value
*/
uniform mediump vec4 scale1;

uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;

void main(void)
{
    gl_Position = matrixArg * vertexArg;
    mediump vec4 translate = (translate1 - translate0) * t + translate0;
    mediump vec4 scale = vec4((vec3(scale1) - vec3(scale0)) * t + vec3(scale0), scale0.w);

    texCoord1Var = texMatrix1Arg * ((texCoordArg + translate) * scale);
    texCoord2Var = texMatrix2Arg * texCoordArg;
}
