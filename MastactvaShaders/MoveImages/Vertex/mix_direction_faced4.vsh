/*@shader @description default vertex shader to swipe images
path: MastactvaShaders/MoveImages/Vertex/mix_line_horizontal_faced4.vsh
@geomentryFaced
@geomentrySize (20,20)
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
const mediump float B = 0.3;


mediump float atan2(in mediump float y, in mediump float x)
{
    bool s = (abs(x) > abs(y));
    return mix(M_PI*0.5 - atan(x,y), atan(y,x), s ? 1.0 : 0.0);
}

mediump float line(in mediump float rotateDir,
            in mediump float x0, 
            in mediump float x1, 
            in mediump float vx, 
            in mediump float t, 
            in mediump float szl,
            in mediump float slope1)
{
    mediump float x = 0.0;
    mediump float res = 0.0;
    if(rotateDir >= 0.0)
    {
        x = mix(x0, x1, t);
        res = -(vx - x)/(szl*slope1);
    }
    else
    {
        x = mix(x1, x0, t);
        res = (vx - x)/(szl*slope1);
    }
    return res;
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


    mediump float slope1 = slope < MIN_SLOPE ? MIN_SLOPE : slope > B * 0.5? B * 0.5 : slope;
    mediump float szl = length(rectSize);
    mediump float x1 =  szl*(1.0 + slope1);
    mediump float x0 =  -x1;

    mediump float faceDir =
         cubeVertex.y < - rectSize.y * B
            ? faceDirections.x
            : cubeVertex.y < 0.0
                ? faceDirections.y
                : cubeVertex.y < rectSize.y * B
                    ? faceDirections.z
                    : faceDirections.w;

    v_t = line(faceDir, x0, x1,  cubeVertex.x, t, szl, slope1);

    mediump float slope2 = szl*slope1 * 0.5;
    mediump float b1 = rectSize.y * B;
    mediump float lowerFaceDir =
         cubeVertex.y < - b1 - slope2
            ? faceDirections.x
            : cubeVertex.y < - b1 + slope2
                ? faceDirections.x
                : cubeVertex.y < 0.0 - slope2
                    ? faceDirections.y
                    : cubeVertex.y < 0.0 + slope2
                         ? faceDirections.y
                         : cubeVertex.y < b1 - slope2
                             ? faceDirections.z
                                 : cubeVertex.y < b1 + slope2
                                     ? faceDirections.z
                                     : faceDirections.w;

    mediump float upperFaceDir =
         cubeVertex.y < - b1 - slope2
            ? faceDirections.x
            : cubeVertex.y < - b1 + slope2
                ? faceDirections.y
                : cubeVertex.y < 0.0 - slope2
                    ? faceDirections.y
                    : cubeVertex.y < 0.0 + slope2
                         ? faceDirections.z
                         : cubeVertex.y < b1 - slope2
                             ? faceDirections.z
                                 : cubeVertex.y < b1 + slope2
                                     ? faceDirections.w
                                     : faceDirections.w;

    mediump float vtl = line(lowerFaceDir, x0, x1,  cubeVertex.x, t, szl, slope1);
    mediump float vtu = line(upperFaceDir, x0, x1,  cubeVertex.x, t, szl, slope1);

    mediump float b0 =
         cubeVertex.y < - b1 - slope2
            ? cubeVertex.y
            : cubeVertex.y < - b1 + slope2
                ? - b1 - slope2
                : cubeVertex.y < 0.0 - slope2
                    ? cubeVertex.y
                    : cubeVertex.y < 0.0 + slope2
                         ? 0.0 - slope2
                         : cubeVertex.y < b1 - slope2
                             ? cubeVertex.y
                                 : cubeVertex.y < b1 + slope2
                                     ? b1 - slope2
                                     : cubeVertex.y;

    mediump float yt = mix(vtl, vtu, (cubeVertex.y - b0)/slope2*0.5);
    if(abs(vtl - vtu) > 0.5)
    {
        v_t = yt;
    }

    texCoord1Var = texMatrix1Arg * texCoordArg;
    texCoord2Var = texMatrix2Arg * texCoordArg;
}
