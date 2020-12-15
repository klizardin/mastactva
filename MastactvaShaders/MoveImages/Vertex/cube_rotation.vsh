/*@shader @description default vertex shader to rotate in cube images
path: MastactvaShaders/MoveImages/Vertex/cube_rotation.vsh
@geomentryfaced
@geomentrysize (2,2) */
attribute highp vec4 vertexArg;
attribute mediump vec4 texCoordArg;

uniform mediump mat4 matrixArg;
uniform mediump mat4 texMatrix1Arg;
uniform mediump mat4 texMatrix2Arg;

/* @argument
  @defaultvalue renderRectSize()
  @description pass render rect size from render to the shader
*/
uniform mediump vec2 rectSize;

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
    highp vec4 pos0 = matrixArg * vertexArg;

    highp vec4 cubeVertex = vec4(vertexArg.x/vertexArg.w, vertexArg.y/vertexArg.w, 0.0, 1.0);
    cubeVertex.x *= 2.0;
    if(cubeVertex.x >= rectSize.x)
    {
        cubeVertex.z = -(cubeVertex.x - rectSize.x);
        cubeVertex.x = rectSize.x;
    }
    cubeVertex.x -= rectSize.x * 0.5;
    cubeVertex.z += rectSize.x * 0.5;

    mediump float a1 = t * M_PI * 0.5;

    mediump mat4 rotateMat = mat4(
                vec4(cos(a1), 0.0, sin(a1), 0.0),
                vec4(0.0, 1.0, 0.0, 0.0),
                vec4(-sin(a1), 0.0, cos(a1), 0.0),
                vec4(0.0, 0.0, 0.0, 1.0));

    highp vec4 pos1 = rotateMat * cubeVertex;
    pos1.x += rectSize.x * 0.5;
    pos1 = matrixArg * pos1;
    gl_Position = vec4(pos1.x/pos1.w, pos1.y/pos1.w, pos0.z/pos0.z, 1.0);
    texCoord1Var = texMatrix1Arg * texCoordArg * vec4(2.0, 1.0, 1.0, 1.0);
    texCoord2Var = texMatrix2Arg * ((texCoordArg * vec4(2.0, 1.0, 1.0, 1.0)) - vec4(1.0, 0.0, 0.0, 0.0));
}
