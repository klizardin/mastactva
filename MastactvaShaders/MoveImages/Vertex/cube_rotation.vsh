/*@shader @description default vertex shader to rotate in cube images
path: MastactvaShaders/MoveImages/Vertex/cube_rotation.vsh */
attribute highp vec4 vertexArg;
attribute mediump vec4 texCoordArg;

uniform mediump mat4 matrixArg;
uniform mediump mat4 texMatrix1Arg;
uniform mediump mat4 texMatrix2Arg;

/* @argument
  @defaultvalue (1.0, 0.0)
  @description
    (1.0, 0.0) - rotate on axe X forward
    (-1.0, 0.0) - rotate on axe X backward
    (0.0, 1.0) - rotate on axe Y forward
    (0.0, -1.0) - rotate on axe Y backward
*/
uniform mediump vec2 rotateAxe;

uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;

const mediump float M_PI = 3.14159265359;

void main(void)
{
    mediump float a1 = t * M_PI * 0.5;
    mediump float a2 = (t - 1.0) * M_PI * 0.5;
    mediump mat4 rotateMat;
    float sX = rotateAxe.x > 0.0 ? 1.0: rotateAxe.x < 0.0? -1.0 : 0.0;
    float sY = rotateAxe.y > 0.0 ? 1.0: rotateAxe.y < 0.0? -1.0 : 0.0;
    mediump vec4 scale = vec4(1.0, 1.0, 1.0, 0.0);
    mediump vec4 shift = vec4(0.0, 0.0, 0.0, 0.0);
    if(rotateAxe.x != 0.0)
    {
        rotateMat = mat4(
                    vec4(sX*cos(a1), 0.0, sX*sin(a1), 0.0),
                    vec4(0.0, 1.0, 0.0, 0.0),
                    vec4(-sX*sin(a1), 0.0, sX*cos(a1), 0.0),
                    vec4(0.0, 0.0, 0.0, 1.0));
        scale.x = 0.5;
        shift.x = 0.5;
    }
    else if(rotateAxe.y != 0.0)
    {
        rotateMat = mat4(
                    vec4(1.0, 0.0, 0.0, 0.0),
                    vec4(0.0, sY*cos(a1), -sY*sin(a1), 0.0),
                    vec4(0.0, sY*sin(a1), sY*cos(a1), 0.0),
                    vec4(0.0, 0.0, 0.0, 1.0));
        scale.y = 0.5;
        shift.y = 0.5;
    }
    gl_Position = matrixArg * vertexArg;
    texCoord1Var = texMatrix1Arg * (texCoordArg * scale);
    texCoord2Var = texMatrix2Arg * (texCoordArg * scale + shift);
}
