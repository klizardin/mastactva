/*@shader @description default vertex shader to rotate in cube images
path: MastactvaShaders/MoveImages/Vertex/cube_rotation_horizontal_faced.vsh
@attributecolors
@geomentryfaced
@geomentrysize (4, 4) */
attribute highp vec4 vertexArg;
attribute mediump vec4 texCoordArg;
attribute mediump vec3 colorArg;

uniform mediump mat4 matrixArg;
uniform mediump mat4 texMatrix1Arg;
uniform mediump mat4 texMatrix2Arg;

/* @argument
  @defaultvalue renderRectSize()
  @description pass render rect size from render to the shader
*/
uniform mediump vec2 rectSize;

/* @argument
  @defaultvalue rand(-1.0, 1.0)
  @description
    > 0.0 - rotate on axe X to the right
    < 0.0 - rotate on axe X to the left
*/
uniform mediump vec4 faceRotateDir;

uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump vec3 posColor;

const mediump float M_PI = 3.14159265359;

void main(void)
{
    highp vec4 pos0 = matrixArg * vertexArg;

    posColor = colorArg;

    highp vec4 cubeVertex = vec4(vertexArg.x/vertexArg.w, vertexArg.y/vertexArg.w, 0.0, 1.0);
    highp vec4 pos1 = cubeVertex;
    mediump float a1 = -t * M_PI * 0.5;
    mediump vec4 scale = vec4(1.0, 1.0, 1.0, 1.0);
    mediump vec4 shift1 = vec4(0.0, 0.0, 0.0, 0.0);
    mediump vec4 shift2 = vec4(0.0, 0.0, 0.0, 0.0);

    //highp int face = int(texCoordArg.y * 2.0);
    //mediump float rotateDir =
    //     face == 0
    //        ? faceRotateDir.x
    //        : face == 1
    //            ? faceRotateDir.y
    //            : face == 2
    //                ? faceRotateDir.z
    //                : faceRotateDir.w;
    mediump float rotateDir = (vertexArg.y < rectSize.y * 0.5) ? faceRotateDir.x : faceRotateDir.y;
    mediump float s = rotateDir >= 0.0 ? 1.0 : -1.0;

    scale.x = 2.0;
    cubeVertex.x *= 2.0;
    mediump float k = 1.0;
    if(s <= 0.0)
    {
        shift2.x = -1.0;
        if(cubeVertex.x >= rectSize.x)
        {
            cubeVertex.z = -(cubeVertex.x - rectSize.x);
            cubeVertex.x = rectSize.x;
        }
        k = 0.5;
    }
    else
    {
        shift1.x = -1.0;
        if(cubeVertex.x <= rectSize.x)
        {
            cubeVertex.z = -(rectSize.x - cubeVertex.x);
            cubeVertex.x = rectSize.x;
        }
        k = 1.5;
    }
    cubeVertex.x -= rectSize.x * k;
    cubeVertex.z += rectSize.x * 0.5;

    mediump mat4 rotateMat = mat4(
                vec4(cos(s*a1), 0.0, sin(s*a1), 0.0),
                vec4(0.0, 1.0, 0.0, 0.0),
                vec4(-sin(s*a1), 0.0, cos(s*a1), 0.0),
                vec4(0.0, 0.0, 0.0, 1.0));

    pos1 = rotateMat * cubeVertex;
    pos1.x += rectSize.x * 0.5;

    pos1 = matrixArg * pos1;
    gl_Position = vec4(pos1.x/pos1.w, pos1.y/pos1.w, pos0.z/pos0.z, 1.0);
    texCoord1Var = texMatrix1Arg * ((texCoordArg * scale) + shift1);
    texCoord2Var = texMatrix2Arg * ((texCoordArg * scale) + shift2);
}
