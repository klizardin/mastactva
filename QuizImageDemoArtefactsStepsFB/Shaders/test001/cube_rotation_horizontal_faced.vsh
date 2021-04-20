/*@shader @description default vertex shader to rotate in cube images
path: MastactvaShaders/MoveImages/Vertex/cube_rotation_horizontal_faced.vsh
@geomentryFaced
@geomentrySize (2, 4)
@facedGeometryCoef (0.0, 1e-3)
*/
attribute highp vec4 renderVertexAttribute;
attribute mediump vec4 renderTextureAttribute;

uniform mediump mat4 renderMatrix;
uniform mediump mat4 renderFromImageMatrix;
uniform mediump mat4 renderToImageMatrix;

/* @argument
  @defaultValue renderRectSize()
  @description pass render rect size from render to the shader
*/
uniform highp vec2 renderScreenRect;

/* @argument
  @defaultValue rand(-1.0, 1.0)
  @description
    > 0.0 - rotate on axe X to the right
    < 0.0 - rotate on axe X to the left
*/
uniform highp vec4 faceRotateDir;

uniform mediump float renderT;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;

const mediump float M_PI = 3.14159265359;

void main(void)
{
    highp vec4 pos0 = renderMatrix * renderVertexAttribute;

    highp vec4 cubeVertex = vec4(renderVertexAttribute.x/renderVertexAttribute.w, renderVertexAttribute.y/renderVertexAttribute.w, 0.0, 1.0);
    highp vec4 pos1 = cubeVertex;
    highp float a1 = -renderT * M_PI * 0.5;
    highp vec4 scale = vec4(1.0, 1.0, 1.0, 1.0);
    highp vec4 shift1 = vec4(0.0, 0.0, 0.0, 0.0);
    highp vec4 shift2 = vec4(0.0, 0.0, 0.0, 0.0);

    highp float rotateDir =
         renderVertexAttribute.y < renderScreenRect.y * 0.25
            ? faceRotateDir.x
            : renderVertexAttribute.y < renderScreenRect.y * 0.5
                ? faceRotateDir.y
                : renderVertexAttribute.y < renderScreenRect.y * 0.75
                    ? faceRotateDir.z
                    : faceRotateDir.w;
    highp float s = rotateDir >= 0.0 ? 1.0 : -1.0;
    scale.x = 2.0;
    cubeVertex.x *= 2.0;
    highp float k = 1.0;
    if(s <= 0.0)
    {
        shift2.x = -1.0;
        if(cubeVertex.x >= renderScreenRect.x)
        {
            cubeVertex.z = -(cubeVertex.x - renderScreenRect.x);
            cubeVertex.x = renderScreenRect.x;
        }
        k = 0.5;
    }
    else
    {
        shift1.x = -1.0;
        if(cubeVertex.x <= renderScreenRect.x)
        {
            cubeVertex.z = -(renderScreenRect.x - cubeVertex.x);
            cubeVertex.x = renderScreenRect.x;
        }
        k = 1.5;
    }
    cubeVertex.x -= renderScreenRect.x * k;
    cubeVertex.z += renderScreenRect.x * 0.5;

    highp mat4 rotateMat = mat4(
                vec4(cos(s*a1), 0.0, sin(s*a1), 0.0),
                vec4(0.0, 1.0, 0.0, 0.0),
                vec4(-sin(s*a1), 0.0, cos(s*a1), 0.0),
                vec4(0.0, 0.0, 0.0, 1.0));

    pos1 = rotateMat * cubeVertex;
    pos1.x += renderScreenRect.x * 0.5;

    pos1 = renderMatrix * pos1;
    gl_Position = vec4(pos1.x/pos1.w, pos1.y/pos1.w, pos0.z/pos0.z, 1.0);
    texCoord1Var = renderFromImageMatrix * ((renderTextureAttribute * scale) + shift1);
    texCoord2Var = renderToImageMatrix * ((renderTextureAttribute * scale) + shift2);
}
