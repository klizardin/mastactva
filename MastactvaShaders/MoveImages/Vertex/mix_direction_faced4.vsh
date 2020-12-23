/*@shader @description default vertex shader to swipe images
path: MastactvaShaders/MoveImages/Vertex/mix_line_horizontal_faced4.vsh
@geomentrySize (10,15)
*/
attribute highp vec4 vertexArg;
attribute mediump vec4 texCoordArg;

uniform mediump mat4 matrixArg;
uniform mediump mat4 texMatrix1Arg;
uniform mediump mat4 texMatrix2Arg;

/*@argument
  @defaultValue rand(-1.0, 1.0)
  @description translate value from
*/
uniform mediump vec2 direction;

/*@argument
    @defaultValue renderRectSize()
    @description render rectangle size
*/
uniform mediump vec2 rectSize;

/*@argument
    @defaultValue 0.1
    @description k of v_t line
*/
uniform mediump float slope;

/* @argument
  @defaultValue rand(-1.0, 1.0)
  @description
    > 0.0 - rotate on axe X to the right
    < 0.0 - rotate on axe X to the left
*/
uniform mediump vec4 faceDirections;


uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump float v_t;


const mediump float M_PI = 3.14159265359;
const mediump float MIN_SLOPE = 0.01;


mediump float atan2(in mediump float y, in mediump float x)
{
    bool s = (abs(x) > abs(y));
    return mix(M_PI*0.5 - atan(x,y), atan(y,x), s ? 1.0 : 0.0);
}

void main(void)
{
    gl_Position = matrixArg * vertexArg;

    highp vec4 cubeVertex = vec4(
                vertexArg.x/vertexArg.w - rectSize.x * 0.5,
                vertexArg.y/vertexArg.w - rectSize.y * 0.5,
                0.0,
                1.0);
    mediump float a = atan2(
                -direction.y,
                direction.x == 0.0 && direction.y == 0.0 ? 1.0 : direction.x);
    mediump mat4 rotateMat = mat4(
                vec4(cos(a), -sin(a), 0.0, 0.0),
                vec4(sin(a), cos(a), 0.0, 0.0),
                vec4(0.0, 0.0, 1.0, 0.0),
                vec4(0.0, 0.0, 0.0, 1.0));
    cubeVertex = rotateMat * cubeVertex;


    mediump float slope1 = slope < MIN_SLOPE ? MIN_SLOPE : slope;
    mediump float szl = length(rectSize);
    mediump float x1 =  szl*(1.0 + slope1);
    mediump float x0 =  -x1;

    mediump float rotateDir =
         cubeVertex.y < -rectSize.y * 0.2
            ? faceDirections.x
            : cubeVertex.y < 0.0
                ? faceDirections.y
                : cubeVertex.y < rectSize.y * 0.3
                    ? faceDirections.z
                    : faceDirections.w;

    mediump float x = 0.0;
    if(rotateDir >= 0.0)
    {
        x = mix(x0, x1, t);
        v_t = -(cubeVertex.x - x)/(szl*slope1);
    }
    else
    {
        x = mix(x1, x0, t);
        v_t = (cubeVertex.x - x)/(szl*slope1);
    }
    texCoord1Var = texMatrix1Arg * texCoordArg;
    texCoord2Var = texMatrix2Arg * texCoordArg;
}
