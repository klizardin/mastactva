/*@shader @description default vertex shader to swipe images
path: MastactvaShaders/MoveImages/Vertex/swipe.vert */
attribute highp vec4 vertexArg;
attribute mediump vec4 texCoordArg;

uniform mediump mat4 matrixArg;
uniform mediump mat4 texMatrix1Arg;
uniform mediump mat4 texMatrix2Arg;

/*@argument
  @defaultValue (0.0, 0.0, 0.0)
  @description translate value from
*/
uniform mediump vec3 translate0;

/*@argument
  @defaultValue (-0.239, 0.285, 0.0)
  @description translate value to
*/
uniform mediump vec3 translate1;

/*@argument
  @defaultValue (1.0, 1.0, 1.0)
  @description scale value from
*/
uniform mediump vec3 scale0;

/*@argument
  @defaultValue (0.635, 0.635, 1.0)
  @description scale value to
*/
uniform mediump vec3 scale1;

uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;

void main(void)
{
    gl_Position = matrixArg * vertexArg;
    mediump vec4 translate = vec4(
        (translate1.x - translate0.x)*t + translate0.x,
        (translate1.y - translate0.y)*t + translate0.y,
        (translate1.z - translate0.z)*t + translate0.z,
        0.0
        );
    mediump vec3 scale = vec3(
        (scale1.x - scale0.x) * t + scale0.x,
        (scale1.y - scale0.y) * t + scale0.y,
        (scale1.z - scale0.z) * t + scale0.z
        );
    mediump vec4 ttc = texCoordArg + translate;
    mediump vec4 sttc = vec4(ttc.x * scale.x, ttc.y * scale.y, ttc.z * scale.z, 1.0);

    texCoord1Var = texMatrix1Arg * sttc;
    texCoord2Var = texMatrix2Arg * texCoordArg;
}
