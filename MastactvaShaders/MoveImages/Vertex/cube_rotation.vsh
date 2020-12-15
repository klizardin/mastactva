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
    (1.0, 0.0) - rotate on axe X to the right
    (-1.0, 0.0) - rotate on axe X to the left
    (0.0, 1.0) - rotate on axe Y to the up
    (0.0, -1.0) - rotate on axe Y to the down
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
    highp vec4 pos1 = cubeVertex;
    mediump float a1 = -t * M_PI * 0.5;
    mediump vec4 scale = vec4(1.0, 1.0, 1.0, 1.0);
    mediump vec4 shift1 = vec4(0.0, 0.0, 0.0, 0.0);
    mediump vec4 shift2 = vec4(0.0, 0.0, 0.0, 0.0);

    if(abs(rotateAxe.y) <= abs(rotateAxe.x))
    {
        scale.x = 2.0;
        mediump float s = rotateAxe.x >= 0.0 ? 1.0 : -1.0;
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
    }
    else
    {
        scale.y = 2.0;
        mediump float s = rotateAxe.y >= 0.0 ? 1.0 : -1.0;
        cubeVertex.y *= 2.0;
        mediump float k = 1.0;
        if(s <= 0.0)
        {
            shift2.y = -1.0;
            if(cubeVertex.y >= rectSize.y)
            {
                cubeVertex.z = -(cubeVertex.y - rectSize.y);
                cubeVertex.y = rectSize.y;
            }
            k = 0.5;
        }
        else
        {
            shift1.y = -1.0;
            if(cubeVertex.y <= rectSize.y)
            {
                cubeVertex.z = -(rectSize.y - cubeVertex.y);
                cubeVertex.y = rectSize.y;
            }
            k = 1.5;
        }
        cubeVertex.y -= rectSize.y * k;
        cubeVertex.z += rectSize.x * 0.5;

        mediump mat4 rotateMat = mat4(
                    vec4(1.0, 0.0, 0.0, 0.0),
                    vec4(0.0, cos(s*a1), -sin(s*a1), 0.0),
                    vec4(0.0, sin(s*a1), cos(s*a1), 0.0),
                    vec4(0.0, 0.0, 0.0, 1.0));

        pos1 = rotateMat * cubeVertex;
        pos1.y += rectSize.y * 0.5;
    }
    pos1 = matrixArg * pos1;
    gl_Position = vec4(pos1.x/pos1.w, pos1.y/pos1.w, pos0.z/pos0.z, 1.0);
    texCoord1Var = texMatrix1Arg * ((texCoordArg * scale) + shift1);
    texCoord2Var = texMatrix2Arg * ((texCoordArg * scale) + shift2);
}
