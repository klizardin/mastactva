/*@shader @description default vertex shader to mix 2 by 2 eclipses in 2 steps
path: MastactvaShaders/MoveImages/Vertex/mix_elipse2x2_2steps.vsh
@geomentryFaced
@geomentrySize (10,10)
*/
attribute highp vec4 vertexArg;
attribute mediump vec4 texCoordArg;

uniform mediump mat4 matrixArg;
uniform mediump mat4 texMatrix1Arg;
uniform mediump mat4 texMatrix2Arg;

/*@argument
  @defaultValue rand(0.0, 1.0)
  @description translate value from
*/
uniform mediump vec2 elipsePos11;

/*@argument
  @defaultValue rand(0.0, 1.0)
  @description translate value from
*/
uniform mediump vec2 elipsePos12;

/*@argument
  @defaultValue rand(0.0, 1.0)
  @description translate value from
*/
uniform mediump vec2 elipsePos21;

/*@argument
  @defaultValue rand(0.0, 1.0)
  @description translate value from
*/
uniform mediump vec2 elipsePos22;

/*@argument
    @defaultValue renderRectSize()
    @description render rectangle size
*/
uniform mediump vec2 rectSize;

/*@argument
    @defaultValue rand(0.02, 0.2)
    @description k of v_t line
*/
uniform mediump float slope;

uniform mediump float t;
varying mediump vec4 texCoord1Var;
varying mediump vec4 texCoord2Var;
varying mediump vec2 v_t;


const mediump float M_PI = 3.14159265359;
const mediump float MIN_SLOPE = 0.01;
const mediump float MANH_DIST_MAX = 0.2;


mediump float atan2(in mediump float y, in mediump float x)
{
    bool s = (abs(x) > abs(y));
    return mix(M_PI*0.5 - atan(x,y), atan(y,x),  s ? 1.0 : 0.0);
}

void main(void)
{
    gl_Position = matrixArg * vertexArg;

    mediump float slope1 = slope < MIN_SLOPE ? MIN_SLOPE : slope;
    mediump float szl = length(rectSize);
    mediump float x1 =  szl*(1.0 + slope1);
    mediump float x0 =  -x1;
    if(t < 0.5)
    {
        highp vec4 vertexCoord1 = vec4(
                    vertexArg.x/vertexArg.w - elipsePos11.x * rectSize.x,
                    vertexArg.y/vertexArg.w - elipsePos11.y * rectSize.y,
                    0.0,
                    1.0);
        highp vec4 vertexCoord2 = vec4(
                    vertexArg.x/vertexArg.w - (elipsePos11.x + (elipsePos12.x - 0.5) * MANH_DIST_MAX) * rectSize.x,
                    vertexArg.y/vertexArg.w - (elipsePos11.y + (elipsePos12.y - 0.5) * MANH_DIST_MAX) * rectSize.y,
                    0.0,
                    1.0);
        mediump float x = mix(x0, x1, t * 2.0);
        mediump float posLength1 = length(vertexCoord1);
        v_t.x = -(posLength1 - x)/(szl*slope1);
        mediump float posLength2 = length(vertexCoord1);
        v_t.y = -(posLength1 - x)/(szl*slope1);
    }
    else
    {
        highp vec4 vertexCoord1 = vec4(
                    vertexArg.x/vertexArg.w - elipsePos21.x * rectSize.x,
                    vertexArg.y/vertexArg.w - elipsePos21.y * rectSize.y,
                    0.0,
                    1.0);
        highp vec4 vertexCoord2 = vec4(
                    vertexArg.x/vertexArg.w - (elipsePos21.x + (elipsePos22.x - 0.5) * MANH_DIST_MAX) * rectSize.x,
                    vertexArg.y/vertexArg.w - (elipsePos21.y + (elipsePos22.y - 0.5) * MANH_DIST_MAX) * rectSize.y,
                    0.0,
                    1.0);
        mediump float x = mix(x0, x1, (t - 0.5) * 2.0);
        mediump float posLength1 = length(vertexCoord1);
        v_t.x = -(posLength1 - x)/(szl*slope1);
        mediump float posLength2 = length(vertexCoord2);
        v_t.y = -(posLength2 - x)/(szl*slope1);
    }

    texCoord1Var = texMatrix1Arg * texCoordArg;
    texCoord2Var = texMatrix2Arg * texCoordArg;
}
